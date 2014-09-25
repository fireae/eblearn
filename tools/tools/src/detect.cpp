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

//typedef double t_net; // network precision
typedef float t_net; // network precision

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
  int main(int argc, char **argv) { // regular main without gui
#endif
    try {
      // check input parameters
      if ((argc != 2) && (argc != 3) ) {
	cerr << "warning: wrong number of parameters." << endl;
	cerr << "usage: detect <config file> [directory or file]" << endl;
	//	return -1;
      }
#ifdef __LINUX__
      feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
      // load configuration
      configuration	conf(argv[1], true, true, false);
      if (conf.exists_true("fixed_randomization"))
	cout << "Using fixed seed: " << fixed_init_drand() << endl;
      else
	cout << "Using random seed: " << dynamic_init_drand(argc, argv) << endl;
      if (!conf.exists("root2") || !conf.exists("current_dir")) {
	string dir;
	dir << dirname(argv[1]) << "/";
	cout << "Looking for trained files in: " << dir << endl;
	conf.set("root2", dir.c_str());
	conf.set("current_dir", dir.c_str());
      }
      conf.set("run_type", "detect"); // tell conf that we are in detect mode
      conf.resolve(); // manual call to resolving variable
      bool              silent        = conf.exists_true("silent");
      if (conf.exists_true("show_conf") && !silent) conf.pretty();
      // output synchronization
      bool sync = conf.exists_true("sync_outputs");
      mutex out_mutex;
      mutex_ostream mutout(std::cout, &out_mutex, "Thread M");
      mutex_ostream muterr(std::cerr, &out_mutex, "Thread M");
      ostream &mout = sync ? mutout : cout;
      ostream &merr = sync ? muterr : cerr;
      bootstrapping<t_net> boot(conf);
      // output dir
      string outdir = detection_thread<t_net>::get_output_directory(conf);
      mout << "Saving outputs to " << outdir << endl;
      // save conf to output dir
      string cname = outdir;
      cname << filename(argv[1]);
      if (conf.write(cname.c_str()))
	mout << "Wrote configuration to " << cname << endl;
      // load classes of network
      idx<ubyte> classes(1,1);
      vector<string> sclasses;
      try { // try loading classes names but do not stop upon failure
	load_matrix<ubyte>(classes, conf.get_cstring("classes"));
      } catch(string &err) { merr << "warning: " << err << endl; }
      sclasses = ubyteidx_to_stringvector(classes);
      t_bbox_saving bbsaving = bbox_none;
      if (conf.exists("bbox_saving"))
	bbsaving = (t_bbox_saving) conf.get_int("bbox_saving");
      bboxes boxes(bbsaving, &outdir, mout, merr);

      uint              ipp_cores     = 1;
      if (conf.exists("ipp_cores")) ipp_cores = conf.get_uint("ipp_cores");
      ipp_init(ipp_cores); // limit IPP (if available) to 1 core
      bool		save_video    = conf.exists_true("save_video");
      bool              save_detections = conf.exists_true("save_detections");
      int		height        = -1;
      int		width         = -1;
      if (conf.exists("input_height")) height = conf.get_int("input_height");
      if (conf.exists("input_width")) width = conf.get_int("input_width");
      bool              input_random  = conf.exists_true("input_random");
      uint              npasses       = 1;
      char              next_on_key   = 0;
      if (conf.exists("next_on_key")) {
	next_on_key = conf.get_char("next_on_key");
	mout << "Press " << next_on_key << " to process next frame." << endl;
      }
      uint skip_frames = conf.try_get_uint("skip_frames", 0);
      if (conf.exists("input_npasses"))
	npasses = conf.get_uint("input_npasses");
      string viddir;
      if (save_video) {
	viddir << outdir << "video/";
	mkdir_full(viddir);
      }
      bool precomputed_boxes = conf.exists("bbox_file");
      uint save_bbox_period = conf.try_get_uint("save_bbox_period", 500);
      idxdim crop(1, 1, 1);
      if (conf.exists("input_crop"))
	crop = string_to_idxdim(conf.get_string("input_crop"));

      string		cam_type;
#ifdef __LINUX__ // default camera for linux if not defined
      cam_type = "v4l2";
#endif
      if (conf.exists("camera"))
	cam_type = conf.get_string("camera");

      // allocate threads
      uint nthreads = 1;
      bool updated = false;
      idx<ubyte> detframe; // frame returned by detection thread
      uint frame_id = 0;
      svector<midx<t_net> > all_samples, samples; // extracted samples
      bboxes all_bbsamples, bbsamples; // boxes corresponding to samples
      if (conf.exists("nthreads"))
	nthreads = (std::max)((uint) 1, conf.get_uint("nthreads"));
      list<detection_thread<t_net>*>  threads;
      list<detection_thread<t_net>*>::iterator ithreads;
      idx<uint> total_saved(nthreads);
      idx_clear(total_saved);
      mout << "Initializing " << nthreads << " detection threads." << endl;
      for (uint i = 0; i < nthreads; ++i) {
	detection_thread<t_net> *dt =
	  new detection_thread<t_net>(conf, &out_mutex, NULL, NULL, sync);
	threads.push_back(dt);
	dt->start();
      }
      // image search can be configured with a search pattern
      const char *fpattern = IMAGE_PATTERN_MAT;
      if (conf.exists("file_pattern"))
	fpattern = conf.get_cstring("file_pattern");

      // initialize camera (opencv, directory, shmem or video)
      idx<ubyte> frame(std::max(height, 1), std::max(width, 1), 3);
      camera<ubyte> *cam = NULL, *cam2 = NULL;
      if (!strcmp(cam_type.c_str(), "directory")) {
	string dir;
	if (argc >= 3) // read input dir from command line
	  dir = argv[2];
	else if (conf.exists("input_dir"))
	  dir = conf.get_string("input_dir");
	// given list
	list<string> files;
	if (conf.exists("input_list")) {
	  files = string_to_stringlist(conf.get_string("input_list"));
	  cam = new camera_directory<ubyte>(dir.c_str(), height, width,
					    input_random, npasses, mout, merr,
					    fpattern, &files);
	} else // given directory only
	  cam = new camera_directory<ubyte>(dir.c_str(), height, width,
					    input_random, npasses, mout, merr,
					    fpattern, &files);
      } else if (!strcmp(cam_type.c_str(), "opencv"))
	cam = new camera_opencv<ubyte>(-1, height, width);
#ifdef __LINUX__
      else if (!strcmp(cam_type.c_str(), "v4l2"))
	cam = new camera_v4l2<ubyte>(conf.get_cstring("device"),
				     height, width,
				     conf.exists_true("camera_grayscale"),
                                     conf.exists_true("camera_rgb"));
      else if (!strcmp(cam_type.c_str(), "mac"))
	cam = new camera_mac<ubyte>(conf.get_cstring("device"),
				     height, width,
				     conf.exists_true("camera_grayscale"),
                                     conf.exists_true("camera_rgb"));
      else if (!strcmp(cam_type.c_str(), "mcams")) {
        vector<string> devices = conf.get_all_strings("device");
	cam = new camera_mcams<ubyte>(conf, devices, height, width,
                                      conf.exists_true("camera_grayscale"),
                                      conf.exists_true("camera_rgb"));
      }
#endif
#ifdef __KINECT__
      else if (!strcmp(cam_type.c_str(), "kinect"))
	cam = new camera_kinect<ubyte>(height, width);
#endif
      else if (!strcmp(cam_type.c_str(), "shmem"))
	cam = new camera_shmem<ubyte>("shared-mem", height, width);
      else if (!strcmp(cam_type.c_str(), "video")) {
	if (argc >= 3)
	  cam = new camera_video<ubyte>
	    (argv[2], height, width, conf.get_uint("input_video_sstep"),
	     conf.get_uint("input_video_max_duration"));
	else eblerror("expected 2nd argument");
      } else if (!strcmp(cam_type.c_str(), "datasource")) {
        cam = new camera_datasource<ubyte,int>(conf);
      } else eblerror("unknown camera type, set \"camera\" in your .conf");
      // a camera directory may be used first, then switching to regular cam
      if (conf.exists_true("precamera"))
	cam2 = new camera_directory<ubyte>(conf.get_cstring("precamdir"),
					   height, width, input_random,
					   npasses, mout, merr, fpattern);
      if (conf.exists_true("camera_grayscale")) cam->set_grayscale();
      if (conf.exists_true("silent")) cam->set_silent();

      // answer variables & initializations
      bboxes bb;

      // gui
#ifdef __GUI__
      bool              bkey_msg      = false; // display key message
      bool display	     = conf.exists_bool("display");
      bool show_parts        = conf.exists_true("show_parts");
      bool bbox_show_conf = !conf.exists_false("bbox_show_conf");
      bool bbox_show_class = !conf.exists_false("bbox_show_class");
      // mindisplay     = conf.exists_bool("minimal_display");
      uint display_sleep  = 0;
      if (conf.exists("display_sleep"))
	display_sleep = conf.get_uint("display_sleep");
      // display_states = conf.exists_bool("display_states");
      // uint qstep1 = 0, qheight1 = 0, qwidth1 = 0,
      // 	qheight2 = 0, qwidth2 = 0, qstep2 = 0;
      // if (conf.exists_bool("queue1")) {
      // 	qstep1 = conf.get_uint("qstep1");
      // 	qheight1 = conf.get_uint("qheight1");
      // 	qwidth1 = conf.get_uint("qwidth1"); }
      // if (conf.exists_bool("queue2")) {
      // 	qstep2 = conf.get_uint("qstep2");
      // 	qheight2 = conf.get_uint("qheight2");
      // 	qwidth2 = conf.get_uint("qwidth2"); }
      // wid_states  = display_states ? new_window("network states"):0;
      // night_mode();
      uint wid  = display ? new_window("eblearn object recognition") : 0;
      night_mode();
      float display_transp = 0.0;
      if (conf.exists("display_bb_transparency"))
	display_transp = conf.get_float("display_bb_transparency");
      detector_gui<t_net> dgui(conf.exists_true("show_extracted"));
#endif
      // timing variables
      timer tpass, toverall, tstop;
      uint cnt = 0;
      bool stop = false, finished = false;

      // loop
      toverall.start();
      while (!finished) {
	// check for results and send new image for each thread
	uint i = 0;
	finished = true;
	for (ithreads = threads.begin();
	     ithreads != threads.end(); ++ithreads, ++i) {
	  // do nothing if thread is finished already
	  if ((*ithreads)->finished()) continue ;
	  finished = false; // a thread is not finished
	  string processed_fname;
	  uint processed_id = 0;
	  // retrieve new data if present
	  bool skipped = false;
	  updated = (*ithreads)->get_data
	    (bb, detframe, *(total_saved.idx_ptr() + i), processed_fname,
	     &processed_id, &samples, &bbsamples, &skipped);
	  if (skipped) cnt++; // a new skipped frame was received
	  // save bounding boxes
	  if (updated) {
	    idxdim d(detframe);
	    if (boot.activated()) bb.clear();
	    if (bbsaving != bbox_none) {
              if (!silent)
                mout << "Adding " << bb.size() << " boxes into new group: "
                     << processed_fname << " with id " << processed_id << endl;
	      boxes.new_group(d, &processed_fname, processed_id);
	      boxes.add(bb, d, &processed_fname, processed_id);
	      if (cnt % save_bbox_period == 0) boxes.save();
	      // avoid sample accumulation if not using bootstrapping
	      if (boot.activated())
		mout << "Received " << samples.size()
		     << " bootstrapping samples." << endl;
	    }
	    if (conf.exists_true("bootstrapping_save")) {
	      all_samples.push_back_new(samples);
	      all_bbsamples.push_back_new(bbsamples);
	    }
            // datasource mode, check and log answers
            if (dynamic_cast<camera_datasource<ubyte,int>*>(cam)) {
              camera_datasource<ubyte,int>* dscam =
                  (camera_datasource<ubyte,int>*) cam;
              dscam->log_answers(bb);
            }
	    cnt++;
	    // display processed frame
#ifdef __GUI__
	    if (display) {
	      select_window(wid);
	      disable_window_updates();
	      clear_resize_window();
	      set_window_title(processed_fname.c_str());
	      uint h = 0, w = 0;
	      // display frame with resulting boxes
	      dgui.display_minimal
		(detframe, bb, ((*ithreads)->pdetect ?
				(*ithreads)->pdetect->get_labels() : sclasses),
		 h, w, 1, 0, 255, wid, show_parts, display_transp,
		 bbox_show_class, bbox_show_conf, &bbsamples);
	      // display extracted samples
	      if (boot.activated()) {
		dgui.display_preprocessed
		  (samples, bbsamples, ((*ithreads)->pdetect ?
					(*ithreads)->pdetect->get_labels() : sclasses),
		   h, w, 1, -1, 1);
	      }
	      enable_window_updates();
	      if (save_video && display) {
		string fname;
		fname << viddir << processed_fname;
		save_window(fname.c_str());
		if (!silent) mout << "saved " << fname << endl;
	      }
	    }
	    // sleep display
	    if (display_sleep > 0) {
	      mout << "sleeping for " << display_sleep << "ms." << endl;
	      millisleep(display_sleep);
	    }
#endif
            if (!silent) {
              // output info
              uint k = cnt, tot = cam->size() - cnt; // progress variables
              if (conf.exists("save_max")) tot = conf.get_uint("save_max");
              if (!silent) {
                if (save_detections) {
                  mout << "total_saved=" << idx_sum(total_saved);
                  if (conf.exists("save_max")) mout << " / " << tot;
                  mout << endl;
                }
              }
              if (boot.activated())
                mout << "total_bootstrapping=" << all_samples.size() << endl;
              mout << "remaining=" << (cam->size() - cnt)
                   << " elapsed=" << toverall.elapsed();
              if (cam->size() > 0)
                mout << " ETA=" << toverall.eta(cnt, cam->size());
              if (conf.exists("save_max") && save_detections) {
                k = idx_sum(total_saved);
                mout << " save_max_ETA=" << toverall.eta(k, tot);
              }
              mout << endl;
              mout << "i=" << cnt << " processing: " << tpass.elapsed_ms()
                   << " fps: " << cam->fps() << endl;
              // save progress
              if (!conf.exists_false("save_progress"))
                job::write_progress(k, tot);
            }
	  }
	  // check if ready
	  if ((*ithreads)->available()) {
	    if (stop)
	      (*ithreads)->ask_stop(); // stop but let thread finish
	    else {
	      // grab a new frame if available
	      if (cam->empty()) {
		stop = true;
		tstop.start(); // start countdown timer
		(*ithreads)->ask_stop(); // ask this thread to stop
		millisleep(50);
	      } else {
#ifdef __GUI__
		int key = gui.pop_key_pressed();
		// if thread has already received data, wait for next key
		if ((*ithreads)->fed() && next_on_key) {
		  if ((int)next_on_key != key && (int)next_on_key != key + 32) {
		    if (!bkey_msg)
		      mout << "Press " << next_on_key
			   << " to process next frame." << endl;
		    bkey_msg = true;
		    continue ; // pause until key is pressed
		  } else {
		    mout << "Key pressed (" << key
			 << ") allowing next frame to process." << endl;
		    bkey_msg = false;
		    tpass.restart();
		  }
		}
#endif
		bool frame_grabbed = false;
		frame_id = cam->frame_id();
		// if the pre-camera is defined use it until empty
		if (cam2 && !cam2->empty())
		  frame = cam2->grab();
		else { // empty pre-camera, use regular camera
		  if (skip_frames > 0)
		    cam->skip(skip_frames); // skip frames if skip_frames > 0
		  if (cam->empty()) continue ;
		  if (precomputed_boxes && !save_video)
		    cam->next(); // move to next frame but without grabbing
		  else if (dynamic_cast<camera_directory<ubyte>*>(cam)) {
		    cam->grab_filename(); // just get the filename, no data
		  } else { // actually grab the frame
		    frame = cam->grab();
		    frame_grabbed = true;
		    // cropping
		    if (crop.nelements() > crop.order()) {
		      cout << "cropping frame from " << frame;
		      for (uint i = 0; i < crop.order(); ++i)
			if (crop.dim(i) > 1)
			  frame = frame.narrow(i, crop.dim(i), 0);
		      cout << " to " << frame << endl;
		    }
		  }
		}
		// send new frame to this thread
		string ffname = cam->frame_fullname();
		string fname = cam->frame_name();
		if (frame_grabbed) {
		  while (!(*ithreads)->set_data(frame, ffname, fname, frame_id))
		  millisleep(5);
		} else {
		  while (!(*ithreads)->set_data(ffname, fname, frame_id))
		    millisleep(5);
		}
		// we just sent a new frame
		tpass.restart();
	      }
	    }
	  }
	}
	if ((conf.exists("save_max") && !stop &&
	     idx_sum(total_saved) > conf.get_uint("save_max"))
	    || (boot.activated()
		&& (intg) all_samples.size() > boot.max_size())) {
	  mout << "Reached max number of detections, exiting." << endl;
	  stop = true; // limit number of detection saves
	  tstop.start(); // start countdown timer
	}
	// sleep a bit between each iteration
	millisleep(5);
	// check if stop countdown reached 0
	if (stop && tstop.elapsed_minutes() >= 20) {
	  cerr << "threads did not all return 20 min after request, stopping"
	       << endl;
	  break ; // program too long to stop, force exit
	}
      }
      // saving boxes
      if (bbsaving != bbox_none) boxes.save();
      mout << "Execution time: " << toverall.elapsed() << endl;
      if (save_video)
	cam->stop_recording(conf.exists_bool("use_original_fps") ?
			    cam->fps() : conf.get_uint("save_video_fps"),
			    outdir.c_str());
      // saving bootstrapping
      if (conf.exists_true("bootstrapping_save") && boot.activated())
	boot.save_dataset(all_samples, all_bbsamples, outdir, classes);
      // free variables
      if (cam) delete cam;
      for (ithreads = threads.begin(); ithreads != threads.end(); ++ithreads) {
	if (!(*ithreads)->finished())
	  (*ithreads)->stop(); // stop thread without waiting
	delete *ithreads;
      }
#ifdef __GUI__
      if (!conf.exists_true("no_gui_quit") && !conf.exists("next_on_key")) {
	mout << "Closing windows..." << endl;
	quit_gui(); // close all windows
	mout << "Windows closed." << endl;
      }
#endif
      job::write_finished(); // declare job finished
      mout << "Detection finished." << endl;
      // evaluation of bbox
      if (conf.exists_true("evaluate") && conf.exists("evaluate_cmd")) {
	string cmd;
	cmd << "cd " << outdir << " && " << conf.get_string("evaluate_cmd");
	int res = std::system(cmd.c_str());
	if (res != 0)
	  cerr << "bbox evaluation failed with command " << cmd << endl;
      }
    } eblcatcherror();
  return 0;
}
