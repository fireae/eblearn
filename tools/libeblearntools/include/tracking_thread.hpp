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

#ifndef TRACKING_THREAD_HPP_
#define TRACKING_THREAD_HPP_

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include "libeblearn.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // tracking thread

  template <typename Tnet>
  tracking_thread<Tnet>::tracking_thread(configuration &conf_,
					 const char *arg2_)
    : conf(conf_), arg2(arg2_), mutex_out(),
      out_updated(false), dt(conf, arg2_), tpl(10, 10, 1) {
    dt.start();
  }

  template <typename Tnet>
  tracking_thread<Tnet>::~tracking_thread() {
  }

  template <typename Tnet>
  void tracking_thread<Tnet>::copy_bboxes(std::vector<bbox*> &bb) {
    // lock data
    pthread_mutex_lock(&mutex_out);
    // clear bboxes
    bboxes.clear();
    // copy bboxes
    for (ibox = bb.begin(); ibox != bb.end(); ++ibox) {
      bboxes.push_back(new bbox(**ibox));
    }
    // unlock data
    pthread_mutex_unlock(&mutex_out);
  }

  template <typename Tnet>
  void tracking_thread<Tnet>::set_out_updated() {
    // lock data
    pthread_mutex_lock(&mutex_out);
    // set flag
    out_updated = true;
    // unlock data
    pthread_mutex_unlock(&mutex_out);
  }

  template <typename Tnet>
  bool tracking_thread<Tnet>::get_data(std::vector<bbox*> &bboxes2,
				       idx<ubyte> &frame2, idx<ubyte> &tpl2) {
    // lock data
    pthread_mutex_lock(&mutex_out);
    // only read data if it has been updated
    if (!out_updated) {
      // unlock data
      pthread_mutex_unlock(&mutex_out);
      return false;
    }
    // clear bboxes
    for (ibox = bboxes2.begin(); ibox != bboxes2.end(); ++ibox) {
      if (*ibox)
	delete *ibox;
    }
    bboxes2.clear();
    // copy bboxes pointers (now responsible for deleting them).
    for (ibox = bboxes.begin(); ibox != bboxes.end(); ++ibox) {
      bboxes2.push_back(*ibox);
    }
    // check frame is correctly allocated, if not, allocate.
    if (frame2.order() != frame.order())
      frame2 = idx<ubyte>(frame.get_idxdim());
    else if (frame2.get_idxdim() != frame.get_idxdim())
      frame2.resize(frame.get_idxdim());
    // copy frame
    idx_copy(frame, frame2);
    // check tpl is correctly allocated, if not, allocate.
    if (tpl2.order() != tpl.order())
      tpl2 = idx<ubyte>(tpl.get_idxdim());
    else if (tpl2.get_idxdim() != tpl.get_idxdim())
      tpl2.resize(tpl.get_idxdim());
    // copy tpl
    idx_copy(tpl, tpl2);
    // reset updated flag
    out_updated = false;
    // unlock data
    pthread_mutex_unlock(&mutex_out);
    // confirm that we copied data.
    return true;
  }

  template <typename Tnet>
  void tracking_thread<Tnet>::draw(bbox *b) {
#ifdef __GUI__
    disable_window_updates();
    clear_window();
    draw_matrix(frame, "in");
    draw_matrix(tpl, "tpl", 0, frame.dim(1));
    if (b) {
      ubyte red = 0, green = 0, blue = 0;
      if (b->class_id == -42)
	green = 255;
      else
	blue = 255;
      draw_box(b->h0, b->w0, b->width, b->height, red, green, blue);
    }
    enable_window_updates();
#endif
  }

  template <typename Tnet>
  void tracking_thread<Tnet>::execute() {
    try {
      // configuration
      std::string    cam_type   = conf.get_string("camera");
      int       height     = conf.get_int("input_height");
      int       width      = conf.get_int("input_width");
      bool      save_video = conf.exists_bool("save_video");
      bool      display    = conf.exists_bool("tracking_display");
      // initialize camera (opencv, directory, shmem or video)
      camera<ubyte> *cam = NULL;
      if (!strcmp(cam_type.c_str(), "directory")) {
	if (arg2) cam = new camera_directory<ubyte>(arg2, height,width);
	else eblerror("expected 2nd argument");
      } else if (!strcmp(cam_type.c_str(), "opencv"))
	cam = new camera_opencv<ubyte>(-1, height, width);
#ifdef __LINUX__
      else if (!strcmp(cam_type.c_str(), "v4l2"))
	cam = new camera_v4l2<ubyte>(conf.get_cstring("device"), height, width);
#endif
      else if (!strcmp(cam_type.c_str(), "shmem"))
	cam = new camera_shmem<ubyte>("shared-mem", height, width);
      else if (!strcmp(cam_type.c_str(), "video")) {
	if (arg2)
	  cam = new camera_video<ubyte>
	    (arg2, height, width, conf.get_uint("input_video_sstep"),
	     conf.get_uint("input_video_max_duration"));
	else eblerror("expected 2nd argument");
      } else eblerror("unknown camera type");
      if (save_video)
	cam->start_recording();
      // other variables
      bool updated = false;
      bbox *b = new bbox;
      std::vector<bbox*> bb;
#ifdef __OPENCV__
      IplImage *iplframe = NULL;
      IplImage *ipltpl = NULL;
#endif
      // main loop
      while(!cam->empty()) {
	try {
	  frame = cam->grab();
	  // send new frame to vision_thread and check if new data was output
	  dt.set_data(frame);
	  updated = dt.get_data(bb, detframe);
	  // update target position if vision thread ready
	  if (updated) {
	    // find bbox with max confidence
	    if (bb.size() > 0) {
	      double maxconf = -5.0;
	      uint maxi = 0;
	      for (uint i = 0; i < bb.size(); ++i)
		if (bb[i]->confidence > maxconf) {
		  maxconf = bb[i]->confidence;
		  maxi = i;
		}
	      //	      cout << "found bbox" << endl;
	      b = bb[maxi];
	      if (b) {
		// update template
		idx<ubyte> tmptpl = detframe.narrow(0, b->height, b->h0);
		tmptpl = tmptpl.narrow(1, b->width, b->w0);
		tpl = idx<ubyte>(tmptpl.get_idxdim());
		idx_copy(tmptpl, tpl);
	      }
	    }
	  } else { // tracking (only when we didnt just receive an update)
#ifdef __OPENCV__
	    iplframe = idx_to_iplptr(frame);
	    ipltpl = idx_to_iplptr(tpl);
	    CvPoint minloc, maxloc;
	    double minval, maxval;
	    // std::vector<CvPoint> found;
	    // std::vector<double> confidences;
	    // FastMatchTemplate(*iplframe, *ipltpl, &found, &confidences, 1, false,
	    // 			5, 1, 15);
	    //      CvRect searchRoi;
	    //      cvSetImageROI( searchImage, searchRoi );
	    // std::vector<CvPoint>::iterator p = found.begin();
	    // std::vector<double>::iterator c = confidences.begin();
	    // for ( ; p != found.end() && c != confidences.end(); ++p, ++c) {
	    // 	gui << at(p->x, p->y) << *c;
	    // }
	    IplImage *tm = cvCreateImage(cvSize(frame.dim(1)  - tpl.dim(1)  + 1,
						frame.dim(0) - tpl.dim(0) + 1 ),
					 IPL_DEPTH_32F, 1 );
	    cvMatchTemplate(iplframe, ipltpl, tm, CV_TM_SQDIFF_NORMED);
	    cvMinMaxLoc(tm, &minval, &maxval, &minloc, &maxloc, 0);
	    //	    gui << at(minloc.y, minloc.x) << "M";
	    b->class_id = -42; // tell that this bbox is result of tracking
	    b->h0 = minloc.y;
	    b->w0 = minloc.x;
	    //	    cout << "maxloc h: " << maxloc.y << " w: " << maxloc.x
            // << " minloc h:" << minloc.y << " w: " << minloc.x << endl;
#endif
	  }
	  // make a copy of found bounding boxes
	  copy_bboxes(bb);
	  // switch 'updated' flag on to warn we just added new data
	  set_out_updated();
	  // display
	  if (display)
	    draw(b);
	} eblcatchwarn();
      }
      if (save_video)
	cam->stop_recording(conf.exists_bool("use_original_fps") ?
			    cam->fps() : conf.get_uint("save_video_fps"));
      if (cam) delete cam;
    } eblcatcherror();
  }

} // end namespace ebl

#endif /* TRACKING_THREAD_HPP_ */
