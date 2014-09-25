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

#ifndef CAMERA_MAC_HPP_
#define CAMERA_MAC_HPP_

#ifdef __LINUX__

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>

#endif

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera_mac<Tdata>::camera_mac(const char *device, int height_, int width_,
				  bool grayscale_, bool mode_rgb_)
    : camera<Tdata>(height_, width_), started(false),
      nbuffers(grayscale_ ? 1 : 3), buffers(new void*[nbuffers]),
      sizes(new int[nbuffers]), mode_rgb(mode_rgb_) {
    std::cout << "Initializing Mac camera from device " << device
              << " to " << height_ << "x" << width_ << std::endl;
  }

  template <typename Tdata>
  camera_mac<Tdata>::~camera_mac() {
    if (buffers)
      delete buffers;
    if (sizes)
      delete sizes;
  }

  ////////////////////////////////////////////////////////////////
  // frame grabbing
#ifdef __LINUX__
  template <typename Tdata>
  void camera_mac<Tdata>::start() {
    started = true;
  }
#endif
  template <typename Tdata>
  idx<Tdata> camera_mac<Tdata>::grab() {
    if(mode_rgb) {
      // convert yuv to rgb
      idx<float>  fyuv(height,width,nbuffers);
      idx_copy(frame,fyuv);
      idx<float> frame_frgb(height,width,nbuffers);
      yuv_to_rgb(fyuv ,frame_frgb);
      idx<Tdata> frame_rgb(height,width,nbuffers);
      idx_copy(frame_frgb,frame_rgb);
      frame = frame_rgb;
    }
    frame_id_++;
    return this->postprocess();
  }

} // end namespace ebl

#endif /* CAMERA_MAC_HPP_ */
