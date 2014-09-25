/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef CAMERA_SHMEM_HPP_
#define CAMERA_SHMEM_HPP_

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera_shmem<Tdata>::camera_shmem(const char *shmem_path,
				    int height_, int width_)
    : camera<Tdata>(height_, width_), buffer(NULL) {
#if defined(__WINDOWS__) || defined(__ANDROID__)
    eblerror("missing shmem implementation for windows and android");
#else
    std::cout << "Initializing shared buffer camera..." << std::endl;
    // connect to shared memory segment
   if ((shmem_key = ftok(shmem_path, 'A')) == -1) {
     std::cerr << "ftok couldnt get the shared mem descriptor from ";
     std::cerr << shmem_path << std::endl;
     eblerror("could not connect to shared memory");
   }
   // get segment
   if ((shmem_id = shmget(shmem_key, 16, 0644 | IPC_CREAT)) == -1) {
     eblerror("shmget couldnt sync the shared mem segment");
   }
   // link data to the segment
   buffer = (struct video_buffer *)shmat(shmem_id, (void *)0, 0);
   std::cout << "shared frame size: " << buffer->height << "x" << buffer->width;
   std::cout << "x" << buffer->bytes_per_pixel << std::endl;
   // get segment according to frame size
   if ((shmem_id = shmget(shmem_key, 16 + buffer->height * buffer->width *
			  buffer->bytes_per_pixel, 0644 | IPC_CREAT)) == -1) {
     eblerror("shmget couldnt sync the shared mem segment");
   }
   // link data to the segment
   buffer = (struct video_buffer *)shmat(shmem_id, (void *)0, 0);
#endif /* __WINDOW__ */
  }

  template <typename Tdata>
  camera_shmem<Tdata>::~camera_shmem() {
#if  !defined(__WINDOWS__) && !defined(__ANDROID__)
    // detach from shared memory
    if (buffer)
      shmdt((const void*)buffer);
#endif /* __WINDOW__ */
  }

  ////////////////////////////////////////////////////////////////
  // frame grabbing

  template <typename Tdata>
  idx<Tdata> camera_shmem<Tdata>::grab() {
#if  !defined(__WINDOWS__) && !defined(__ANDROID__)
    // make a frame request
    buffer->dump_to_file = 0;
    buffer->request_frame = 1;
    while (buffer->request_frame) millisleep(1); // request received
    while (!buffer->frame_ready) millisleep(1); // wait for frame to be ready
    // check that shared segment is compatible
    if (buffer->bytes_per_pixel != 3)
      eblerror("shared segment doesnt contain an RGBA image\n");
    // allocate if necessary
    if (!grabbed) // first time, allocate frame
      frame = idx<Tdata>(buffer->height, buffer->width,
			 buffer->bytes_per_pixel);
    // cast and copy data
    ubyte *in = (ubyte*) &(buffer->data);
    Tdata *out = frame.idx_ptr();
    uint sz = frame.nelements();
    // cast and copy data
    for (uint i = 0; i < sz; ++i, ++out, ++in)
      *out = (Tdata) *in;
#endif
    frame_id_++;
    return this->postprocess();
  }

} // end namespace ebl

#endif /* CAMERA_SHMEM_HPP_ */
