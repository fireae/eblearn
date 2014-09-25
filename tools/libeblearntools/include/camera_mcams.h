/***************************************************************************
 *   Copyright (C) 2011 by Soumith Chintala   *
 *   soumith@gmail.com   *
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

#ifndef MCAMS_H_
#define MCAMS_H_

#include "camera.h"
#include "configuration.h"

namespace ebl {

//! The camera_mcams class stitches several v4l2 cameras.
template <typename Tdata> class camera_mcams : public camera<Tdata> {
 public:
  //! Initialize multiple cameras
  //! height and width are optional parameters that resize the input image
  //! to those dimensions if given (different than -1). One may want to
  //! decrease the input resolution first to speed up operations, for example
  //! when computing multiple resolutions.
  //! \param mode_rgb sets the mode to RGB, if false, it sets YUV
  camera_mcams(configuration &conf, std::vector<std::string> &devices,
               int height = -1, int width = -1, bool grayscale = false,
               bool mode_rgb = true);
  //! Destructor.
  virtual ~camera_mcams();

  //! Return a new frame.
  virtual idx<Tdata> grab();

 private:
#ifdef __LINUX__
  void start();
#endif

  // members ////////////////////////////////////////////////////////
 protected:
  using camera<Tdata>::frame;		//!< frame buffer
  using camera<Tdata>::frame_id_;	//!< frame counter
  using camera<Tdata>::grabbed;         //!< frame buffer grabbed yet or not
  using camera<Tdata>::height;          //!< height
  using camera<Tdata>::width;	        //!< width
  using camera<Tdata>::bresize;         //!< resize or not during postproc.
  using camera<Tdata>::grayscale;       //!< Output grayscale or not.
  std::vector< camera<Tdata>* > cams;
  std::vector<idx<Tdata> >	 cam_frames; //!< frame buffer for all cams
  bool started;
  int  nbuffers;
};

} // end namespace ebl

#include "camera_mcams.hpp"


#endif /* MCAMS_H_ */
