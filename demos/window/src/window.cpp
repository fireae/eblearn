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
#include "libeblearntools.h"

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

typedef float t_net;

////////////////////////////////////////////////////////////////
// drawing and position interpolation

#ifdef __GUI__

void draw(bbox *b, rect &pos, idx<ubyte> &bgwin, idx<ubyte> &frame,
	  idx<ubyte> &tpl, configuration &conf, bool display_info) {
  uint control_offset = conf.get_uint("control_offset");
  uint text_hoffset = bgwin.dim(0) - conf.get_uint("text_hoffset");
  uint text_woffset = bgwin.dim(1) / 2 - conf.get_uint("text_woffset");
  uint text_height = conf.get_uint("text_height");
  disable_window_updates();
  clear_window();
  set_font_size(conf.get_int("font_size"));
  gui.draw_matrix_unsafe(bgwin);
  if (display_info) {
    uint hface = bgwin.dim(0) - frame.dim(0) - control_offset;
    uint wface = bgwin.dim(1) / 2 - frame.dim(1) / 2;
    gui << at(text_hoffset - text_height * 4, text_woffset)
	<< "Imagine this is a window to outside world.";
    gui << at(text_hoffset - text_height * 3, text_woffset) <<
      "Move your head down to see the sky,";
    gui << at(text_hoffset - text_height * 2, text_woffset) <<
      "up to see the ground,";
    gui << at(text_hoffset - text_height, text_woffset) <<
      "left to see right and right to see left.";
    if (b) {
      ubyte red = 0, green = 0, blue = 0;
      if (b->class_id == -42)
	green = 255;
      else
	blue = 255;
      draw_box(hface + pos.h0, wface + pos.w0, pos.width, pos.height,
	       red, green, blue);
    }
    draw_matrix(frame, hface, wface);
    //  draw_matrix(tpl, "template");
  }
  enable_window_updates();
}

void estimate_position(rect &srcpos, rect &pos, rect &tgtpos, idx<ubyte> &frame,
		       float &h, float &w, configuration &conf,
		       float tgt_time_distance) {
  //  pos = srcpos;
  // interpolate position with time
  tgt_time_distance = tgt_time_distance * conf.get_float("smooth_factor");
  // update current position
  pos.h0 = (uint) std::max((double)0,srcpos.h0 + (tgtpos.h0 - (float) srcpos.h0)
			   * MIN(1.0, tgt_time_distance));
  pos.w0 = (uint) std::max((double)0,srcpos.w0 + (tgtpos.w0 - (float) srcpos.w0)
			   * MIN(1.0, tgt_time_distance));
  pos.height = (uint) std::max((double)0, srcpos.height +
			       (tgtpos.height - (float) srcpos.height)
			       * MIN(1.0, tgt_time_distance));
  pos.width = (uint) std::max((double)0, srcpos.width +
			      (tgtpos.width - (float) srcpos.width)
			      * MIN(1.0, tgt_time_distance));
  // transform position into screen position
  // cout << "cur pos: " << pos << " src: " << srcpos
  //      << " target: " << tgtpos << endl;
  h = (((pos.h0 + pos.height / 2.0) / frame.dim(0))
       - conf.get_float("hoffset")) * conf.get_float("hfactor");
  w = (((pos.w0 + pos.width / 2.0) / frame.dim(1))
       - conf.get_float("woffset")) * conf.get_float("wfactor");
  //    cout << " h: " << h << " w: " << w << endl;
}

void change_background(vector<string>::iterator &bgi, vector<string> &bgs,
		       idx<ubyte> &bg, configuration &conf) {
  bgi++;
  if (bgi == bgs.end()) {
    random_shuffle(bgs.begin(), bgs.end());
    bgi = bgs.begin();
  }
  bg = load_image<ubyte>(*bgi);
  bg = image_resize(bg, conf.get_uint("winszhmax"),
		    conf.get_uint("winszhmax") * 3, 0);
  cout << "Changed background to " << *bgi << " (" << bg << ")." << endl;
}

#endif

////////////////////////////////////////////////////////////////
// main loop

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
  int main(int argc, char **argv) { // regular main without gui
#endif
#ifndef __GUI__
    eblerror("QT libraries missing, install and recompile.");
#else
    try {
      // check input parameters
      if ((argc != 2) && (argc != 3) ) {
	cerr << "wrong number of parameters." << endl;
	cerr << "usage: obj_detect <config file> [directory or file]" << endl;
	return -1;
      }
#ifdef __LINUX__
      feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
      ipp_init(1); // limit IPP (if available) to 1 core
      // load configuration
      configuration conf(argv[1]);
      if (!conf.exists("root2")) {
	string dir = dirname(argv[1]);
	cout << "Looking for trained files in: " << dir << endl;
	conf.set("root2", dir.c_str());
	conf.resolve();
      }      
      intg winszh = conf.get_int("winszh");
      intg winszw = conf.get_int("winszw");
      bool display = conf.exists_bool("window_display");
      bool silent = conf.exists_bool("silent");
      bool display_info = true;
      
      // TODO: read PAM format for alpha channel
      // idx<ubyte> window = load_image<ubyte>("/home/sermanet/eblearn/pvc_window.png");
      // cout << "window: " << window << endl;

      // load background images
      list<string> *lbgs = find_fullfiles(conf.get_string("bgdir"));
      if (!lbgs) eblerror("background files not found");
      vector<string> bgs;
      list_to_vector(*lbgs, bgs);
      srand(time(NULL));
      random_shuffle(bgs.begin(), bgs.end());
      vector<string>::iterator bgi = bgs.begin();
      for ( ; bgi != bgs.end(); ++bgi)
	cout << "found " << *bgi << endl;
      bgi = bgs.begin();
      idx<ubyte> bg = load_image<ubyte>(*bgi);
      bg = image_resize(bg, conf.get_uint("winszhmax"),
			conf.get_uint("winszhmax") * 5, 0);
      cout << "loaded " << *bgi << ": " << bg << endl;
      idx<ubyte> bgwin;
      bgwin = bg.narrow(0, MIN(bg.dim(0), winszh),
			std::max((intg) 0, bg.dim(0) / 2 - winszh/2));
      bgwin = bgwin.narrow(1, MIN(bg.dim(1), winszw),
			   std::max((intg) 0, bg.dim(1) / 2 - winszw/2));
      
      // vision threads
      // tracking_thread<float> tt(conf, argc == 3 ? argv[2] : NULL);
      // tt.start();
      
      // allocate threads
      uint nthreads = 1;
      bool sync = conf.exists_true("sync_outputs");
      mutex out_mutex; // used to synchronize outputs
      idx<ubyte> detframe; // frame returned by detection thread
      if (conf.exists("nthreads"))
	nthreads = (std::max)((uint) 1, conf.get_uint("nthreads"));
      idx<uint> total_saved(nthreads);
      idx_clear(total_saved);
      list<detection_thread<t_net>*>  threads;
      list<detection_thread<t_net>*>::iterator ithreads;
      cout << "Initializing " << nthreads << " detection threads." << endl;
      for (uint i = 0; i < nthreads; ++i) {
	ostringstream tname;
	tname << "Thread " << i;
	detection_thread<t_net> *dt =
	  new detection_thread<t_net>(conf, out_mutex, tname.str().c_str(),
				      NULL, sync);
	threads.push_back(dt);
	dt->start();
      }
      vector<bbox*> bboxes;
      vector<bbox*>::iterator ibboxes;
      uint cnt = 0;

      // initialize camera (opencv, directory, shmem or video)
      string		cam_type      = conf.get_string("camera");
      int		height        = conf.get_int("input_height");
      int		width         = conf.get_int("input_width");
      //      idx<ubyte> frame;
      camera<ubyte> *cam = NULL, *cam2 = NULL;
      if (!strcmp(cam_type.c_str(), "opencv"))
	cam = new camera_opencv<ubyte>(-1, height, width);
#ifdef __LINUX__
      else if (!strcmp(cam_type.c_str(), "v4l2"))
	cam = new camera_v4l2<ubyte>(conf.get_cstring("device"),
				     height, width);
#endif
      else if (!strcmp(cam_type.c_str(), "shmem"))
	cam = new camera_shmem<ubyte>("shared-mem", height, width);
      else eblerror("unknown camera type");
      
      // input and output variables
      idx<ubyte> frame(1,1,1), tpl(1,1,1);
      rect pos(0, 0, 10, 10), srcpos(0, 0, 10, 10), tgtpos(0, 0, 10, 10);
      bbox *b = NULL;
      float h = 0, w = 0;
      bool updated = false;
      bool first_time = true;
      // timing variables
      timer main_timer, bg_timer, dt_timer, gui_timer, detection_timer;
      int main_time, dt_time, gui_time; // time in milliseconds
      float tgt_time_distance = 0;
      main_timer.start();
      bg_timer.start();
      dt_timer.start();
      gui_timer.start();
      detection_timer.start();
      int bgtime = conf.get_uint("bgtime") * 1000;
      // loop
      while(1) {
	try {

	  // // get cam image and detected region
	  // updated = tt.get_data(bboxes, frame, tpl);
	  
	  // check for results and send new image for each thread
	  uint i = 0;
	  for (ithreads = threads.begin(); 
	       ithreads != threads.end(); ++ithreads, ++i) {
	    // do nothing if thread is finished already
	    string processed_fname;
	    // retrieve new data if present
	    updated = (*ithreads)->get_data(bboxes, detframe, 
					    *(total_saved.idx_ptr() + i),
					    processed_fname);
	    // we got a new answer
	    if (updated) {
	      updated = false;
	      cnt++;

	      // update target position
	      // find bbox with max confidence
	      if (bboxes.size() > 0) {
		double maxconf = -5.0;
		uint maxi = 0;
		// select bbox with maximum confidence
		for (uint i = 0; i < bboxes.size(); ++i)
		  if (bboxes[i]->confidence > maxconf) {
		    maxconf = bboxes[i]->confidence;
		    maxi = i;
		  }
		b = bboxes[maxi];
		if (b) {
		  tgtpos.h0 = b->h0;
		  tgtpos.w0 = b->w0;
		  tgtpos.height = b->height;
		  tgtpos.width = b->width;
		  srcpos = pos;
		  detection_timer.restart();
		}
		if (first_time) {
		  pos = tgtpos;
		  srcpos = pos;
		  first_time = false;
		}
	      } else
		b = NULL;
	      // update dt time
	      dt_time = dt_timer.elapsed_milliseconds();
	      dt_timer.restart();
	      // print timing info
	      //	      if (!silent)
		cout << "main: " << main_time << " ms "
		     << "gui: " << gui_time << " ms "
		     << "vision: " << dt_time << " ms "
		     << "fps: " << cam->fps() << endl;
	    }
	    // check if ready and send new frame to process
	    if ((*ithreads)->available()) {
	      if (!cam->empty()) {
		frame = cam->grab();
		// send new frame to this thread
		string frame_name = cam->frame_name();
		while (!(*ithreads)->set_data(frame, frame_name))
		  millisleep(5);
	      }
	    }
	  }
	    
	  // update position and draw if gui thread ready
	  if (!gui.busy_drawing() && gui_timer.elapsed_milliseconds() > 30) {
	    // recompute position
	    if (dt_time > 0)
	      tgt_time_distance = detection_timer.elapsed_milliseconds()
		/ (float) dt_time;
	    estimate_position(srcpos, pos, tgtpos, frame, h, w, conf,
			      tgt_time_distance);
	    // narrow original image into window
	    bgwin = bg.narrow(0, MIN(bg.dim(0), winszh),
			      MIN(std::max((intg) 0, bg.dim(0) - 1 - winszh),
				  std::max((intg) 0,
					   (intg) ((1 - h)
						   * (bg.dim(0) - winszh)))));
	    bgwin = bgwin.narrow(1, MIN(bg.dim(1), winszw),
				 MIN(std::max((intg) 0, bg.dim(1) - 1 - winszw),
				     std::max((intg) 0,
					      (intg) (w *
						      (bg.dim(1) - winszw)))));
	    // draw
	    if (display)
	      draw(b, pos, bgwin, frame, tpl, conf, display_info);
	    // update gui time
	    gui_time = gui_timer.elapsed_milliseconds();
	    gui_timer.restart();
	  }
	  
	  // sleep a bit between each iteration
	  millisleep(5);
	  // main timing
	  main_time = main_timer.elapsed_milliseconds();
	  main_timer.restart(); 
	  // change background every bgtime
	  int key = gui.pop_key_pressed();
	  if (key == Qt::Key_I)
	    display_info = !display_info;
	  if (bg_timer.elapsed_milliseconds() > bgtime
	      || key == Qt::Key_Space) {
	    bg_timer.restart();
	    change_background(bgi, bgs, bg, conf);
	  }
	} catch(string &err) {
	  cerr << err << endl;
	}
      }
    } catch(string &err) {
      cerr << err << endl;
    }
#endif  
    return 0;
  }
