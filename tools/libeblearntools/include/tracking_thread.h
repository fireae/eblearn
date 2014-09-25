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

// Windows does not know linux/mac mutexes, TODO: implement windows mutexes
#ifndef __WINDOWS__

#ifndef TRACKING_THREAD_H_
#define TRACKING_THREAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "thread.h"
#include "detection_thread.h"
#include "configuration.h"

#ifdef __OPENCV__
#include <opencv/cv.h>
//#include "FastMatchTemplate.h"
#endif

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // A tracking thread class

  template <typename Tnet>
  class tracking_thread : public thread {
  public:
    //! Initialize the tracking thread.
    //! @param dt The detection thread that will provide templates to track.
    tracking_thread(configuration &conf, const char *arg2);
    ~tracking_thread();

    //! Execute the tracking thread.
    virtual void execute();
    //! Return true if new data was copied from the thread, false otherwise.
    virtual bool get_data(std::vector<bbox*> &bboxes, idx<ubyte> &frame,
			  idx<ubyte> &tpl);

  private:
    //! Copy passed bounding boxes into bboxes class member
    //! (allocating new 'bbox' objects).
    void copy_bboxes(std::vector<bbox*> &bb);
    //! Turn 'out_updated' flag on, so that other threads know we just outputed
    //! new data.
    void set_out_updated();
    //! Draw inputs/outputs.
    void draw(bbox *b);

    ////////////////////////////////////////////////////////////////
    // private members
  private:
    configuration		&conf;
    const char			*arg2;
    pthread_mutex_t		 mutex_out;	//! mutex for thread output
    idx<ubyte>			 frame;         //! tracking frame
    idx<ubyte>			 detframe;      //! detection frame
    std::vector<bbox*>		 bboxes;
    std::vector<bbox*>::iterator ibox;
    bool			 out_updated;	//! thread output updated
    detection_thread<Tnet>       dt;
    idx<ubyte>                   tpl;
  };

} // end namespace ebl

#include "tracking_thread.hpp"

#endif /* TRACKING_THREAD_H_ */

#endif /* __WINDOWS__ */
