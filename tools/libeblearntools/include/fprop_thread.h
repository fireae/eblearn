/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
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

#ifndef FPROP_THREAD_H_
#define FPROP_THREAD_H_

#include <stdio.h>
#include <stdlib.h>

#include "defines_tools.h"
#include "thread.h"
#include "netconf.h"
#include "configuration.h"

namespace ebl {

// A detection thread class ////////////////////////////////////////////////////

template <typename T> class fprop_thread : public thread {
 public:
  //! \param om A mutex used to synchronize threads outputs/
  //!   To synchronize all threads, give the same mutex to each of them.
  //! \param sync If true, synchronize outputs between threads, using
  //!    om, otherwise use regular unsynced outputs.
  //! \param tc The channels type of the input image, e.g. brightness Y,
  //!    or color RGB.
  fprop_thread(configuration &conf, mutex *om = NULL, const char *name = "",
               const char *arg2 = NULL, bool sync = true,
               t_chans tc = CHANS_RGB);
  ~fprop_thread();

  //! Execute the detection thread.
  virtual void execute();
  //! Return true if new data was copied to the thread, false otherwise,
  //! if we could not obtain the mutex lock.
  virtual bool set_data(idx<ubyte> &frame, std::string &frame_name,
                        uint frame_id);
  //! Return true if new data was copied from the thread, false otherwise.
  //! We get the frame back even though it was set via set_data,
  //! because we do not know which frame was actually used.
  //! (could use some kind of id, and remember frames to avoid copy).
  //! Note: this method will return true only once, after a frame has been
  //! processed. Subsequent calls will return false until a new frame is
  //! processed again.
  virtual bool get_data(idx<ubyte> &frame, std::string &frame_name,
                        uint &frame_id);
  //! Ask the thread to fprop the file 'frame_fname' and dump the results
  //! in 'dump_fname' (which should not contain an extension, it will be
  //! added along the matrix sizes).
  virtual bool set_dump(std::string &frame_fname, std::string &dump_fname);
  //! Return true if the thread has processed and dump the data.
  //! Note: this method will return true only once, after a frame has been
  //! processed. Subsequent calls will return false until a new frame is
  //! processed again.
  virtual bool dumped();
  //! Return true if the thread is available to process a new frame, false
  //! otherwise.
  virtual bool available();
  //! Set the directory where to write outputs.
  virtual void set_output_directory(std::string &out);

 private:
  //! Turn 'out_updated' flag on, so that other threads know we just outputed
  //! new data.
  void set_out_updated();

  // private members
 private:
  configuration	 conf;
  const char	*arg2;
  idx<ubyte>	 uframe;
  idx<T>	 frame;
  mutex		 mutex_in;              // mutex for thread input
  mutex 	 mutex_out;             // mutex for thread output
  bool		 in_updated;            // thread input updated
  bool		 out_updated;           // thread output updated
  bool           bavailable;            // thread is available
  std::string    frame_name;            // name of current frame
  uint           frame_id;              //! Unique ID for frame.
  std::string    outdir;                // output directory
  using          thread::mout;          //! synchronized cout
  using          thread::merr;          //! synchronized cerr
  t_chans        color_space;
  bool           bload_image;           //! Load image or not.

 public:
  detector<T> *pdetect;
};

} // end namespace ebl

#include "fprop_thread.hpp"

#endif /* FPROP_THREAD_H_ */
