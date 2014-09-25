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
#include "eblapp.h"

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#ifdef __GUI__
#include "libeblearngui.h"
#endif

////////////////////////////////////////////////////////////////
// drawing and position interpolation

#ifdef __GUI__

void draw(bbox *b, rect &pos, idx<ubyte> &frame,
	  idx<ubyte> &tpl, configuration &conf) {
  disable_window_updates();
  clear_window();
  if (b) {
    char red = 0, green = 0, blue = 0;
    if (b->class_id == -42)
      green = 255;
    else
      blue = 255;
    draw_box(pos.h0, pos.w0, pos.width, pos.height,
	     red, green, blue);
  }
  draw_matrix(frame);
  // draw_matrix(tpl, "template");
  enable_window_updates();
}

void estimate_position(rect &srcpos, rect &pos, rect &tgtpos, idx<ubyte> &frame,
		       configuration &conf,
		       float tgt_time_distance) {
  tgt_time_distance = tgt_time_distance * conf.get_float("smooth_factor");
  // update current position
  pos.h0 =
    std::max((uint) 0, (uint) (srcpos.h0 + (tgtpos.h0 - (float) srcpos.h0)
			       * MIN(1.0, tgt_time_distance)));
  pos.w0 =
    std::max((uint) 0, (uint) (srcpos.w0 + (tgtpos.w0 - (float) srcpos.w0)
			       * MIN(1.0, tgt_time_distance)));
  pos.height =
    std::max((uint) 0, (uint) (srcpos.height +
			       (tgtpos.height - (float) srcpos.height)
			       * MIN(1.0, tgt_time_distance)));
  pos.width = std::max((uint) 0, (uint) (srcpos.width +
					 (tgtpos.width - (float) srcpos.width)
					 * MIN(1.0, tgt_time_distance)));
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
    eblerror("QT library missing, install and recompile.");
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
      bool display = conf.exists_bool("display");
      // vision threads
      tracking_thread<float> tt(conf, argc == 3 ? argv[2] : NULL);
      tt.start();
      // input and output variables
      idx<ubyte> frame(1,1,1), tpl(1,1,1);
      vector<bbox*> bboxes;
      rect pos(0, 0, 10, 10), srcpos(0, 0, 10, 10), tgtpos(0, 0, 10, 10);
      bbox *b = NULL;
      bool updated = false;
      bool first_time = true;
      // timing variables
      QTime main_timer, bg_timer, dt_timer, gui_timer, detection_timer;
      int main_time, dt_time, gui_time, detection_time; // time in milliseconds
      float tgt_time_distance = 0;
      main_timer.start();
      bg_timer.start();
      dt_timer.start();
      gui_timer.start();
      detection_timer.start();
      // loop
      while(1) {
	try {
	  // get cam image and detected region
	  updated = tt.get_data(bboxes, frame, tpl);
	  // update target position if tracking thread ready
	  if (updated) {
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
		//		detection_timer.restart();
	      }
	      if (first_time) {
		pos = tgtpos;
		srcpos = pos;
		first_time = false;
	      }
	    } else
	      b = NULL;
	    // update dt time
	    dt_time = dt_timer.elapsed();
	    dt_timer.restart();
	    // print timing info
	    cout << "main: " << main_time << " ms "
	    	 << "gui: " << gui_time << " ms "
	    	 << "grabbing+tracking: " << dt_time << " ms " << endl;
	  }
	  // update position and draw if gui thread ready
	  if (!gui.busy_drawing()) {
	    // recompute position
	    if (dt_time > 0)
	      tgt_time_distance = detection_timer.elapsed() / (float) dt_time;
	    estimate_position(srcpos, pos, tgtpos, frame, conf,
	  		      tgt_time_distance);
	    // draw
	    if (display)
	      draw(b, pos, frame, tpl, conf);
	    // update gui time
	    gui_time = gui_timer.elapsed();
	    gui_timer.restart();
	  }
	  // sleep for a little bit
	  millisleep(conf.get_uint("mainsleep"));
	  // main timing
	  main_time = main_timer.elapsed();
	  main_timer.restart(); 
	} catch (string &err) {
	  cerr << err << endl;
	}
      }
    } catch(string &err) {
      cerr << err << endl;
    }
#endif  
    return 0;
  }

#endif /* __WINDOWS__ */
