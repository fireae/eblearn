/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#ifndef DETECTION_THREAD_H_
#define DETECTION_THREAD_H_

#include <stdio.h>
#include <stdlib.h>

#include "defines_tools.h"
#include "thread.h"
#include "netconf.h"
#include "configuration.h"
#include "bbox.h"
#include "bootstrapping.h"

namespace ebl {

// A detection thread class ////////////////////////////////////////////////////

template <typename T> class detection_thread : public thread {
 public:
  //! \param om A mutex used to synchronize threads outputs/
  //!   To synchronize all threads, give the same mutex to each of them.
  //! \param sync If true, synchronize outputs between threads, using
  //!    om, otherwise use regular unsynced outputs.
  //! \param tc The channels type of the input image, e.g. brightness Y,
  //!    or color RGB.
  detection_thread(configuration &conf, mutex *om = NULL,
                   const char *name = NULL,
                   const char *arg2 = NULL, bool sync = true,
                   t_chans tc = CHANS_RGB);
  virtual ~detection_thread();

  //! Execute the detection thread.
  virtual void execute();

  // thread communication ////////////////////////////////////////////////////

  //! Sends new data to the detection thread.
  //! Return true if new data was copied to the thread, false otherwise,
  //! if we could not obtain the mutex lock.
  virtual bool set_data(idx<ubyte> &frame, std::string &frame_fullname,
                        std::string &frame_name, uint frame_id);
  //! Sends new data to the detection thread.
  //! Return true if new data was copied to the thread, false otherwise,
  //! if we could not obtain the mutex lock.
  virtual bool set_data(std::string &frame_fullname, std::string &frame_name,
                        uint frame_id);
  //! Receives results from the detection thread into parameters.
  //! Return true if new data was copied from the thread, false otherwise.
  //! We get the frame back even though it was set via set_data,
  //! because we do not know which frame was actually used.
  //! (could use some kind of id, and remember frames to avoid copy).
  //! \param frame_id An optional variable that will be filled with frame's id
  //! \param samples Extracted samples corresponding to bboxes (optional)
  //! \param skipped If frame was ignored, skipped == true.
  virtual bool get_data(bboxes &bb, idx<ubyte> &frame,
                        uint &total_saved, std::string &frame_name,
                        uint *frame_id = NULL,
                        svector<midx<T> > *samples = NULL,
                        bboxes *bbsamples = NULL,
                        bool *skipped = NULL);
  //! Return true if the thread is available to process a new frame, false
  //! otherwise.
  virtual bool available();
  //! Returns true if at least 1 input has been fed for detection.
  virtual bool fed();
  //! Returns the string of the target directory given configuration 'conf'.
  static std::string get_output_directory(configuration &conf);

  //! Initialize detector settings given configuration.
  static void init_detector(detector<T> &detector, configuration &conf,
                            std::string &odir, bool silent = true);

  // internal methods ////////////////////////////////////////////////////////
 private:
  //! Clear current bboxes.
  void clear_bboxes();
  //! Thread-safely copy passed bounding boxes into bboxes class member
  //! (allocating new 'bbox' objects).
  void copy_bboxes(bboxes &bb);
  //! Thread-safely copy bootstrapping outputs.
  void copy_bootstrapping(svector<midx<T> > &samples, bboxes &bb);
  //! Turn 'out_updated' flag on, so that other threads know we just outputed
  //! new data.
  void set_out_updated();
  //! Set skipped and updated flags to true.
  void skip_frame();

  // private members /////////////////////////////////////////////////////////
 private:
  configuration	 conf;
  const char		*arg2;
  idx<ubyte>		 uframe;
  idx<T>		 frame;
  mutex		 mutex_in;	// mutex for thread input
  mutex 		 mutex_out;	// mutex for thread output
  bboxes   		 bbs;
  bboxes::iterator	 ibox;
  bool		 in_updated;	// thread input updated
  bool		 out_updated;	// thread output updated
  bool                 bavailable;	// thread is available
  bool                 bfed;  //!< Thread has been fed data.
  std::string          frame_name;	// name of current frame
  std::string          frame_fullname;//!< Full path of current frame.
  uint                 frame_id;	//!< Unique ID for frame.
  std::string          outdir;	// output directory
  uint                 total_saved;
  using		 thread::mout;	//!< Synchronized cout.
  using		 thread::merr;	//!< Synchronized cerr.
  t_chans              color_space;
  bool                 silent;
  svector<midx<T> > returned_samples; //!< Samples to be returned.
  bboxes               returned_samples_bboxes;
  bootstrapping<T>  boot; //!< Bootstrapping manager.
  bool                 frame_skipped; //!< Processing skipped for this frame.
  bool                 frame_loaded; //!< Frame was loaded or not.

 public:
  detector<T>       *pdetect;
};

} // end namespace ebl

#include "detection_thread.hpp"

#endif /* DETECTION_THREAD_H_ */
