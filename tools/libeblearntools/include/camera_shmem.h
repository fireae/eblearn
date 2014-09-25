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

#ifndef CAMERA_SHMEM_H_
#define CAMERA_SHMEM_H_

#include "camera.h"

#if  !defined(__WINDOWS__) && !defined(__ANDROID__)

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#endif /* __WINDOW__ */

namespace ebl {

  //! Data structure representing a frame in shared memory
  typedef struct video_buffer {
    char request_frame;
    char frame_ready;
    char bytes_per_pixel;
    char dump_to_file;
    int width;
    int height;
    unsigned char data;
  } t_video_buffer;

  //! The camera_shmem class interfaces with camera images that come
  //! from a shared buffer in memory.
  //! It allows to grab images from a shared buffer in the idx format, and also
  //! to save gui outputs into video files.
  template <typename Tdata> class camera_shmem : public camera<Tdata> {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors/allocation

    //! Initialize shmem camera using a shared memory descriptor.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param shmem_path The path id of the shared memory.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    camera_shmem(const char *shmem_path, int height = -1, int width = -1);

    //! Destructor.
    virtual ~camera_shmem();

    ////////////////////////////////////////////////////////////////
    // frame grabbing

    //! Return a new frame.
    virtual idx<Tdata> grab();

    // members ////////////////////////////////////////////////////////
  protected:
    using camera<Tdata>::frame;	//!< frame buffer
    using camera<Tdata>::grabbed;	//!< frame buffer grabbed yet or not
    using camera<Tdata>::frame_id_;	//!< frame counter
    t_video_buffer	*buffer;
#if  !defined(__WINDOWS__) && !defined(__ANDROID__)
    key_t		 shmem_key;
#endif
    int			 shmem_id;
  };

} // end namespace ebl

#include "camera_shmem.hpp"


#endif /* CAMERA_SHMEM_H_ */
