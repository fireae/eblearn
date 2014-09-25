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

#ifndef CAMERA_OPENCV_H_
#define CAMERA_OPENCV_H_

#include "camera.h"
#include "opencv.h"

namespace ebl {

  //! The camera_opencv class interfaces with the opencv camera and images.
  //! It allows to grab images from opencv in the idx format, and also
  //! to save gui outputs into video files.
  template <typename Tdata> class camera_opencv : public camera<Tdata> {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors/allocation

    //! Initialize opencv camera using id to choose which camera_opencv to use.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param id The ID of the camera. -1 chooses the first available camera.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    camera_opencv(int id, int height = -1, int width = -1);

    //! Destructor.
    virtual ~camera_opencv();

    ////////////////////////////////////////////////////////////////
    // frame grabbing

    //! Return a new frame.
    virtual idx<Tdata> grab();

    // members ////////////////////////////////////////////////////////
  protected:
    using camera<Tdata>::frame;		//!< frame buffer
    using camera<Tdata>::grabbed;	//!< frame buffer grabbed yet or not
    using camera<Tdata>::frame_id_;	//!< frame counter
#ifdef __OPENCV__
    CvCapture	*capture;	//!< opencv capture object
    IplImage	*ipl_frame;	//!< opencv image
#endif /* __OPENCV__ */
  };

} // end namespace ebl

#include "camera_opencv.hpp"


#endif /* CAMERA_OPENCV_H_ */
