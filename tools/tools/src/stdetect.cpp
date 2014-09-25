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

#define SFUNC2(T) T,T,T,SBUF<T>

////////////////////////////////////////////////////////////////
// network

typedef float Tnet; // network precision

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
  int main(int argc, char **argv) { // regular main without gui
#endif
    try {
      // check input parameters
      if ((argc != 2) && (argc != 3) ) {
	cerr << "wrong number of parameters." << endl;
	cerr << "usage: objdetect <config file> [directory or file]" << endl;
	return -1;
      }
#ifdef __LINUX__
      feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
      ipp_init(1); // limit IPP (if available) to 1 core
      // load configuration
      configuration	conf(argv[1], true, true, false);
      if (!conf.exists("root2")) {
	string dir;
	dir << dirname(argv[1]) << "/";
	cout << "Looking for trained files in: " << dir << endl;
	conf.set("root2", dir.c_str());
	conf.set("current_dir", dir.c_str());
      }
      conf.resolve();
      if (conf.exists_true("show_conf")) conf.pretty();
      bool		color	      = conf.exists_bool("color");
      uint		norm_size     = conf.get_uint("normalization_size");
      Tnet		threshold     = (Tnet) conf.get_double("threshold");
      bool		display       = false;
      bool		display_states= false;
      bool		mindisplay    = false;
      uint		display_sleep = 0;
      bool		save_video    = false;
      string		cam_type      = conf.get_string("camera");
      int		height        = -1;
      int		width         = -1;
      if (conf.exists("input_height")) height = conf.get_int("input_height");
      if (conf.exists("input_width")) width = conf.get_int("input_width");
      bool              input_random  = conf.exists_true("input_random");
      uint              npasses       = 1;
      char              next_on_key   = 0;
      uint              wid           = 0; // window id
      uint              wid_states    = 0; // window id
      string		outdir        = "out_";
      if (conf.exists("next_on_key")) {
	next_on_key = conf.get_char("next_on_key");
	cout << "Press " << next_on_key << " to process next frame." << endl;
      }
      outdir += tstamp();
      outdir += "/";
      cout << "Saving outputs to " << outdir << endl;

      // load network and weights
      parameter<fs(Tnet)> theparam;
      idx<ubyte> classes(1,1);
      try {
	load_matrix<ubyte>(classes, conf.get_cstring("classes"));
      } catch(string &err) { cerr << "warning: " << err << endl; }
      vector<string> sclasses = ubyteidx_to_stringvector(classes);
      answer_module<SFUNC2(Tnet)> *ans =
       create_answer<SFUNC2(Tnet)>(conf, classes.dim(0));
      uint noutputs = ans->get_nfeatures();
      module_1_1<SFUNC(Tnet)> *net =
	create_network<SFUNC(Tnet)>(theparam, conf, noutputs);
      // loading weights
      if (!conf.exists("weights")) { // manual weights
	cerr << "warning: \"weights\" variable not defined, loading manually "
	     << "if manual_load defined" << endl;
       if (conf.exists_true("manual_load"))
	 manually_load_network(*((layers<SFUNC(Tnet)>*)net), conf);
      } else { // multiple-file weights
	// concatenate weights if multiple ones
	vector<string> w =
	  string_to_stringvector(conf.get_string("weights"));
	cout << "Loading weights from: " << w << endl;
	theparam.load_x(w);
      }
      
      // detector
      detector<fs(Tnet)> detect(*net, sclasses, *ans, NULL, NULL);
      // multi-scaling parameters      
      double maxs = conf.exists("max_scale")?conf.get_double("max_scale") : 1.0;
      double mins = conf.exists("min_scale")?conf.get_double("min_scale") : 1.0;
      t_scaling scaling_type = SCALES_STEP;
      vector<idxdim> scales;
      if (conf.exists("scaling_type"))
       scaling_type = (t_scaling) conf.get_uint("scaling_type");
      switch (scaling_type) {
      case MANUAL:
	if (!conf.exists("scales"))
	  eblerror("expected \"scales\" variable to be defined in manual mode");
	scales = string_to_idxdimvector(conf.get_cstring("scales"));
	detect.set_resolutions(scales);
	break ;
      case ORIGINAL: detect.set_scaling_original(); break ;
      case SCALES_STEP:
	detect.set_resolutions(conf.get_double("scaling"), maxs, mins);
	break ;
      case SCALES_STEP_UP:
	detect.set_resolutions(conf.get_double("scaling"), maxs, mins);
	detect.set_scaling_type(scaling_type);
	break ;
      default:
	detect.set_scaling_type(scaling_type);
      }
     // optimize memory usage by using only 2 buffers for entire flow
     SBUF<Tnet> input(1, 1, 1), output(1, 1, 1);
     if (!conf.exists_false("mem_optimization"))
       detect.set_mem_optimization(input, output, true);
     // zero padding
     float hzpad = 0, wzpad = 0;
     if (conf.exists("hzpad")) hzpad = conf.get_float("hzpad");
     if (conf.exists("wzpad")) wzpad = conf.get_float("wzpad");
     detect.set_zpads(hzpad, wzpad);
      
      bool bmask_class = false;
      if (conf.exists("mask_class"))
	bmask_class = detect.set_mask_class(conf.get_cstring("mask_class"));
      if (conf.exists("input_min")) // limit inputs size
	detect.set_min_resolution(conf.get_uint("input_min")); 
      if (conf.exists("input_max")) // limit inputs size
	detect.set_max_resolution(conf.get_uint("input_max"));
      detect.set_silent();
      if (conf.exists_bool("save_detections")) {
	string detdir = outdir;
	detdir += "detections";
	detdir = detect.set_save(detdir);
	if (conf.exists("save_max_per_frame"))
	  detect.set_save_max_per_frame(conf.get_uint("save_max_per_frame"));
      }
      // nms
      detect.set_cluster_nms(conf.exists_true("cluster_nms"));
      detect.set_scaler_mode(conf.exists_true("scaler_mode"));
      if (conf.exists("nms"))
	detect.set_pruning((t_pruning)conf.get_uint("nms"), 
			   conf.exists("min_hcenter_dist") ? 
			   conf.get_float("min_hcenter_dist") : 0.0,
			   conf.exists("min_wcenter_dist") ? 
			   conf.get_float("min_wcenter_dist") : 0.0,
			   conf.exists("bbox_max_overlap") ? 
			   conf.get_float("bbox_max_overlap") : 1.0,
			   conf.exists_true("share_parts"),
			   conf.exists("threshold2") ? 
			   (Tnet) conf.get_float("threshold2") : 0.0,
			   conf.exists("bbox_max_center_dist") ? 
			   conf.get_float("bbox_max_center_dist") : 0.0,
			   conf.exists("bbox_max_center_dist2") ? 
			   conf.get_float("bbox_max_center_dist2") : 0.0,
			   conf.exists("bbox_max_wcenter_dist") ? 
			   conf.get_float("bbox_max_wcenter_dist") : 0.0,
			   conf.exists("bbox_max_wcenter_dist2") ? 
			   conf.get_float("bbox_max_wcenter_dist2") : 0.0,
			   conf.exists("min_wcenter_dist2") ? 
			   conf.get_float("min_wcenter_dist2") : 0.0,
			   conf.exists("bbox_max_overlap2") ? 
			   conf.get_float("bbox_max_overlap2") : 0.0,
			   conf.exists_true("mean_bb"),
			   conf.exists("same_scale_mhd") ? 
			   conf.get_float("same_scale_mhd") : 0.0,
			   conf.exists("same_scale_mwd") ? 
			   conf.get_float("same_scale_mwd") : 0.0,
			   conf.exists("min_scale_pred") ? 
			   conf.get_float("min_scale_pred") : 0.0,
			   conf.exists("max_scale_pred") ? 
			   conf.get_float("max_scale_pred") : 0.0
			   );
      if (conf.exists("bbox_hfactor") && conf.exists("bbox_wfactor"))
	detect.set_bbox_factors(conf.get_float("bbox_hfactor"),
				conf.get_float("bbox_wfactor"),
				conf.exists("bbox_woverh") ?
				conf.get_float("bbox_woverh") : 1.0,
				conf.exists("bbox_hfactor2") ?
				conf.get_float("bbox_hfactor2") : 1.0,
				conf.exists("bbox_wfactor2") ?
				conf.get_float("bbox_wfactor2") : 1.0);
      if (conf.exists("max_object_hratio"))
	detect.set_max_object_hratio(conf.get_double("max_object_hratio"));
      if (conf.exists("net_min_height") && conf.exists("net_min_width"))
	detect.set_min_input(conf.get_int("net_min_height"),
			     conf.get_int("net_min_width"));
      if (conf.exists("smoothing"))
	detect.set_smoothing(conf.get_uint("smoothing"));
      if (conf.exists("background_name"))
	detect.set_bgclass(conf.get_cstring("background_name"));
      // image search can be configured with a search pattern
      const char *fpattern = IMAGE_PATTERN_MAT;
      if (conf.exists("file_pattern"))
	fpattern = conf.get_cstring("file_pattern");

      // initialize camera (opencv, directory, shmem or video)
      idx<ubyte> frame;
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
					    input_random, npasses,
					    std::cout, std::cerr,
					    fpattern, &files);
	} else // given directory only
	  cam = new camera_directory<ubyte>(dir.c_str(), height, width,
					    input_random, npasses,
					    std::cout, std::cerr,
					    fpattern, &files);
      } else if (!strcmp(cam_type.c_str(), "opencv"))
	cam = new camera_opencv<ubyte>(-1, height, width);
#ifdef __LINUX__
      else if (!strcmp(cam_type.c_str(), "v4l2"))
	cam = new camera_v4l2<ubyte>(conf.get_cstring("device"),
				     height, width,
				     conf.exists_true("camera_grayscale"));
#endif
      else if (!strcmp(cam_type.c_str(), "shmem"))
	cam = new camera_shmem<ubyte>("shared-mem", height, width);
      else if (!strcmp(cam_type.c_str(), "video")) {
	if (argc >= 3)
	  cam = new camera_video<ubyte>
	    (argv[2], height, width, conf.get_uint("input_video_sstep"),
	     conf.get_uint("input_video_max_duration"));
	else eblerror("expected 2nd argument");
      } else eblerror("unknown camera type");
      // a camera directory may be used first, then switching to regular cam
      if (conf.exists_bool("precamera"))
	cam2 = new camera_directory<ubyte>(conf.get_cstring("precamdir"),
					   height, width);

      // answer variables & initializations
      vector<bbox*> bboxes;
      vector<bbox*>::iterator ibboxes;
      ostringstream answer_fname;
      mkdir_full(outdir);
      answer_fname << outdir << "bbox.txt";
      // open file      
      ofstream fp(answer_fname.str().c_str());
      if (!fp) {
	cerr << "failed to open " << answer_fname.str() << endl;
	eblerror("open failed");
      }
    
      // gui
#ifdef __GUI__
      display	     = conf.exists_bool("display");
      mindisplay     = conf.exists_bool("minimal_display");
      display_sleep  = conf.get_uint("display_sleep");
      display_states = conf.exists_bool("display_states");
      save_video     = conf.exists_bool("save_video");
      uint qstep1 = 0, qheight1 = 0, qwidth1 = 0,
	qheight2 = 0, qwidth2 = 0, qstep2 = 0;
      if (conf.exists_bool("queue1")) {
	qstep1 = conf.get_uint("qstep1");
	qheight1 = conf.get_uint("qheight1");
	qwidth1 = conf.get_uint("qwidth1"); }
      if (conf.exists_bool("queue2")) {
	qstep2 = conf.get_uint("qstep2");
	qheight2 = conf.get_uint("qheight2");
	qwidth2 = conf.get_uint("qwidth2"); }
      module_1_1_gui netgui;
      wid_states  = display_states ? new_window("network states"):0;
      night_mode();
      wid  = display ? new_window("eblearn object recognition") : 0;
      night_mode();
      float	zoom = 1;
      detector_gui<fs(Tnet)> dgui(conf.exists_bool("queue1"), qstep1, qheight1,
				   qwidth1, conf.exists_bool("queue2"), qstep2,
				   qheight2, qwidth2);
      if (bmask_class)
	dgui.set_mask_class(conf.get_cstring("mask_class"),
			    (Tnet) conf.get_double("mask_threshold"));
      if (save_video) {
	string viddir = outdir;
	viddir += "video";
	cam->start_recording(wid, viddir.c_str());
      }
#endif  
      // timing variables
      timer tpass, toverall;
      long ms;
  
      // loop
      toverall.start();
      while(!cam->empty()) {
	// get a new frame
	tpass.restart();
	// if the pre-camera is defined use it until empty
	if (cam2 && !cam2->empty())
	  frame = cam2->grab();
	else // empty pre-camera, use regular camera
	  frame = cam->grab();
	string frame_name = cam->frame_name();
	// run detector
	if (!display) { // fprop without display
	  bboxes = detect.fprop(frame, threshold, frame_name.c_str());
	}
#ifdef __GUI__
	else { // fprop and display
	  disable_window_updates();
	  clear_window();
	  if (mindisplay)
	    bboxes = dgui.display(detect, frame, threshold, frame_name.c_str(),
				  0, 0, zoom, (Tnet)0, (Tnet)255, wid);
	  else
	    bboxes =
	      dgui.display_inputs_outputs(detect, frame, threshold,
					  frame_name.c_str(), 0, 0, zoom,
					  (Tnet)-1.1, (Tnet)1.1, wid);
	  enable_window_updates();
	  if (display_states) {
	    dgui.display_current(detect, frame, wid_states);
	    select_window(wid);
	  }
	  if (save_video)
	    cam->record_frame();
	}	    
#endif
	ms = tpass.elapsed_milliseconds();
	cout << "processing: " << ms << " ms." << endl;
	cout << "fps: " << cam->fps() << endl;
	// save bounding boxes
	for (ibboxes = bboxes.begin(); ibboxes != bboxes.end(); ++ibboxes) {
	  fp << cam->frame_name() << " " << (*ibboxes)->class_id << " "
	     << (*ibboxes)->confidence << " ";
	  fp << (*ibboxes)->w0 << " " << (*ibboxes)->h0 << " ";
	  fp << (*ibboxes)->w0 + (*ibboxes)->width << " ";
	  fp << (*ibboxes)->h0 + (*ibboxes)->height << endl;
	}
	// sleep display
	if (display_sleep > 0) {
	  cout << "sleeping for " << display_sleep << "ms." << endl;
	  millisleep(display_sleep);
	}
	if (conf.exists("save_max") && 
	    detect.get_total_saved() > conf.get_uint("save_max")) {
	  cout << "Reached max number of detections, exiting." << endl;
	  break ; // limit number of detection saves
	}
      }
      cout << "Execution time: " << toverall.elapsed_minutes() <<" mins" <<endl;
      if (save_video)
	cam->stop_recording(conf.exists_bool("use_original_fps") ?
			    cam->fps() : conf.get_uint("save_video_fps"));
      // free variables
      if (net) delete net;
      if (cam) delete cam;
      // close files
      fp.close();
#ifdef __GUI__
      if (!conf.exists_true("no_gui_quit")) {
	cout << "Closing windows..." << endl;
	quit_gui(); // close all windows
	cout << "Windows closed." << endl;
      }
#endif
    } eblcatcherror();
  return 0;
}
