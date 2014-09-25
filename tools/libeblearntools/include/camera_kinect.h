/***************************************************************************
 *   Copyright (C) 2010 by Soumith Chintala and Pierre Sermanet  *
 *   chin@nyu.edu, sermanet@cs.nyu.edu   *
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


#ifndef CAMERA_KINECT_H_
#define CAMERA_KINECT_H_

#include "camera.h"

namespace ebl {

  //! The camera_kinect class interfaces with the microsoft xbox kinect
  //! module and grabs depth maps.
  //! It allows to grab images from kinect in the idx format, and also
  //! to save gui outputs into video files(not yet).
  //TODO: also add interface to grab RGB image(simple but haven't done yet)
  template <typename Tdata> class camera_kinect : public camera<Tdata> {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors/allocation

    //! Initialize kinect.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    camera_kinect(int height = -1, int width = -1,
		  std::ostream &out = std::cout,
		  std::ostream &err = std::cerr);

    //! Destructor.
    virtual ~camera_kinect();

    ////////////////////////////////////////////////////////////////
    // frame grabbing

    //! Return a new frame.
    virtual idx<Tdata> grab();

    // members ////////////////////////////////////////////////////////
  protected:
    using camera<Tdata>::frame;		//!< frame buffer
    using camera<Tdata>::frame_id_;	//!< frame counter
    using camera<Tdata>::grabbed;	//!< frame buffer grabbed yet or not
    using camera<Tdata>::height;	//!< height
    using camera<Tdata>::width;	        //!< width
    using camera<Tdata>::bresize;       //!< resize or not during postproc.
    using camera<Tdata>::mresize;       //!< mean resize or not during postproc.
    using camera<Tdata>::resize_mode;   //!< resize_mode during postproc.
    using camera<Tdata>::out;	//!< output stream
    using camera<Tdata>::err;	//!< error output stream
  };

} // end namespace ebl

#include "camera_kinect.hpp"


#endif /* CAMERA_KINECT_H_ */
