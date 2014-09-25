/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet   *
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

#ifndef CAMERA_DATASOURCE_H_
#define CAMERA_DATASOURCE_H_

#include "datasource.h"
#include "camera.h"
#include "tools_utils.h"
#include "ebl_logger.h"

#ifdef __BOOST__
#ifndef BOOST_FILESYSTEM_VERSION
#define BOOST_FILESYSTEM_VERSION 3
#endif
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#endif

namespace ebl {

//! The camera_datasource class interfaces with datasource objects.
template <typename Tdata, typename Tlabel>
class camera_datasource : public camera<Tdata> {
 public:
  // constructors/allocation /////////////////////////////////////////////////

  //! Initialize a datasource camera from a dataset defined in conf.
  //! \param randomize Randomize image list if true.
  //! \param npasses Repeat list this number of times.
  camera_datasource(configuration &conf, bool randomize = false,
                    uint npasses = 1, std::ostream &out = std::cout,
                    std::ostream &err = std::cerr);
  //! Destructor.
  virtual ~camera_datasource();

  // frame grabbing //////////////////////////////////////////////////////////

  //! Return a new frame.
  virtual idx<Tdata> grab();
  //! Move to the next frame, without returning the frame.
  //! This is called by grab before grabbing.
  //! This can be used to get frames infos without grabbing.
  virtual void next();
  //! Move to the previous frame, without returning the frame.
  virtual void previous();
  //! Return true until all images have been processed.
  virtual bool empty();
  //! Skip n frames.
  virtual void skip(uint n);
  //! Return the number of frames left to process, -1 if unknown.
  virtual int remaining();
  //! Return the total number of frames to process from the initialization,
  //! of the camera, -1 if unknown.
  virtual int size();
  //! Compare answers to groundtruth and log results.
  virtual void log_answers(bboxes &bb);

  // members /////////////////////////////////////////////////////////////////
 protected:
  using camera<Tdata>::frame;           //!< frame buffer
  using camera<Tdata>::frame_id_;	//!< frame counter
  using camera<Tdata>::frame_name_;	//!< frame name
  using camera<Tdata>::out;             //!< output stream
  using camera<Tdata>::err;             //!< error output stream
  using camera<Tdata>::silent;
  std::ostringstream                   oss; //!< temporary string
  bool                                 randomize; //!< Randomize order or not.
  uint                                 npasses; //!< Number of passes on ds.
  class_datasource<Tdata,Tdata,Tlabel> ds;
  idx<Tdata>                           frame_raw; //!< frame buffer
  state<Tlabel>                        label;
  classifier_meter log;
  uint save_correct; //!< If >0, save incorrect with this period.
  uint save_incorrect; //!< If >0, save incorrect with this period.
  std::string save_root; //!< Root directory where to save stuff.
};

} // end namespace ebl

#include "camera_datasource.hpp"


#endif /* CAMERA_DATASOURCE_H_ */
