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

#ifndef CAMERA_H_
#define CAMERA_H_

#include "libidx.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif

namespace ebl {

//! The camera class is an abstract class that serves as an interface
//! to different camera implementation, such as camera_opencv, etc.
//! It allows to grab images from camera in the idx format, and also
//! to save gui outputs into video files.
template <typename Tdata> class camera {
 public:
  // constructors/allocation /////////////////////////////////////////////////

  //! Initialize a camera.
  //! height and width are optional parameters that resize the input image
  //! to those dimensions if given (different than -1). One may want to
  //! decrease the input resolution first to speed up operations, for example
  //! when computing multiple resolutions.
  //! \param height Resize input frame to this height if different than -1.
  //! \param width Resize input frame to this width if different than -1.
  camera(int height = -1, int width = -1, std::ostream &out = std::cout,
         std::ostream &err = std::cerr);
  //! Destructor.
  virtual ~camera();

  // frame grabbing //////////////////////////////////////////////////////////

  //! Return a new frame.
  virtual idx<Tdata> grab() = 0;
  //! Do not read the file, instead return the filename to be grabbed.
  //! This method should only be used by file-based cameras.
  virtual std::string grab_filename();
  //! Move to the next frame, without returning the frame.
  //! This is called by grab before grabbing.
  //! This can be used to get frames infos without grabbing.
  virtual void next();
  //! Move to the previous frame, without returning the frame.
  virtual void previous();
  //! Return true if no frames available, false otherwise.
  virtual bool empty();
  //! Skip n frames.
  virtual void skip(uint n);
  //! This will narrow all images coming out of the camera in dimension
  //! 'dim' with size 'size' starting at offset 'off'.
  virtual void set_input_narrow(int dim, int size, int off);
  //! This will force the camera to output grayscale images.
  virtual void set_grayscale();

  // video recording /////////////////////////////////////////////////////////

  //! Start recording of frames from window window_id into path.
  //! This creates a directory name in path.
  //! No frames are actually recorded,
  virtual bool start_recording(uint window_id = 0, const char *name = NULL);
  //! Dump all frames declared to be recorded by start_recording().
  virtual bool record_frame();
  //! Create all videos started with start_recording() using frames dumped
  //! with record_frame(), using fps frames per second.
  //! \param root Indicates the root where the images are and where
  //!   to save the video.
  virtual bool stop_recording(float fps, const char *root = NULL);

  // info ////////////////////////////////////////////////////////////////////

  //! Return the number of frames per second obtained via grab().
  virtual float fps();
  //! Return the id for current frame.
  virtual uint frame_id();
  //! Return a name for current frame.
  virtual std::string frame_name();
  //! Return the complete filename for current frame (including path).
  virtual std::string frame_fullname();
  //! Return the number of frames left to process, -1 if unknown.
  virtual int remaining();
  //! Return the total number of frames to process from the initialization,
  //! of the camera, -1 if unknown.
  virtual int size();
  //! Prevents info printing.
  virtual void set_silent();

  // internal methods ////////////////////////////////////////////////////////
 protected:

  //! Return post processed frame after grabbing it, e.g. resize frame to
  //! target height and width (if specified).
  //! This also increments frame counter.
  virtual idx<Tdata> postprocess();

  // members /////////////////////////////////////////////////////////////////
 protected:
  idx<Tdata>   frame;		//!< frame buffer
  int          height;        //!< resize input
  int          width;         //!< resize input
  bool         bresize;       //!< resize or not
  bool         mresize;       //!< mean resize or not
  uint         resize_mode;   //!< resize mode: 0, 1 or 2
  uint         frame_id_;     //!< frame counter
  std::string  frame_name_;	//!< frame name
  bool         grabbed;       //!< false if no frame grabbed yet
  uint         wid;           //!< window to record from
  std::string  recording_name;//!< name of video
  uint         record_cnt;    //!< frame counter for recording
  float        fps_grab;      //!< frames per second of grabbing
  std::string  audio_filename;//!< filename of audio file
  std::ostream &out;          //!< stream where to output info
  std::ostream &err;          //!< stream where to output info
  timer        tfps;          //!< timer for computing fps.
  long         fps_ms_elapsed;//!< ms elapsed since last reset
  uint         cntfps;        //!< counter for computing fps.
  int          narrow_dim, narrow_size, narrow_off; //!< Narrow parameters.
  bool         grayscale;     //!< If true, output grayscale images.
  bool         silent;
};


} // end namespace ebl

#include "camera.hpp"

#endif /* CAMERA_H_ */
