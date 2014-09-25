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

#include <iomanip>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include "libidx.h"
#include "libeblearn.h"
#include "libeblearntools.h"
#include "eblapp.h"

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#ifdef __GUI__
#include "libeblearngui.h"
#endif

typedef float t_net; // precision at which network is fprop (float ok)
typedef int t_label; // label's type

// fprop images ////////////////////////////////////////////////////////////////

// fprop dataset ///////////////////////////////////////////////////////////////

// main ////////////////////////////////////////////////////////////////////////

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
  cout << "* Generic fprop" << endl;
  if (argc < 2) {
    cout << "Usage: ./imfprop <config file>" << endl;
    eblerror("config file not specified");
  }
#ifdef __LINUX__
    feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
  try {
    cout << "Using random seed " << dynamic_init_drand(argc, argv) << endl;
    timer textraction, gtimer;
    gtimer.start(); // total running time
    configuration conf(argv[1]); // configuration file
    idxdim dims;
    intg nsamples = 0;
    string outdir = conf.try_get_string("out", "./");
    // output synchronization
    bool sync = !conf.exists_false("sync_outputs");
    mutex out_mutex;
    mutex_ostream mutout(std::cout, &out_mutex, "Thread M");
    mutex_ostream muterr(std::cerr, &out_mutex, "Thread M");
    ostream &mout = sync ? mutout : cout;
    ostream &merr = sync ? muterr : cerr;
    uint ipp_cores = conf.try_get_uint("ipp_cores", 1);
    ipp_init(ipp_cores); // limit IPP (if available) to 1 core
    uint skip_frames = conf.try_get_uint("skip_frames", 0);

    // camera //////////////////////////////////////////////////////////////////
    string cam_type = conf.try_get_string("camera", "directory");
    int	height = conf.try_get_int("input_height", -1);
    int	width = conf.try_get_int("input_width", -1);
    bool input_random  = conf.exists_true("input_random");
    uint npasses = conf.try_get_uint("input_npasses", 1);
    string patt = conf.try_get_string("file_pattern", IMAGE_PATTERN_MAT);
    idx<ubyte> frame(1,1,1);
    camera<ubyte> *cam = NULL;
    if (!strcmp(cam_type.c_str(), "directory")) {
      if (argc >= 3) // read input dir from command line
	cam = new camera_directory<ubyte>(argv[2], height, width,
					  input_random, npasses, mout, merr,
					  patt.c_str());
      else if (conf.exists("input_dir")) // read input dir from conf
	cam = new camera_directory<ubyte>(conf.get_cstring("input_dir"), 
					  height, width, input_random,
					  npasses, mout, merr, patt.c_str());
	else eblerror("expected 2nd argument");
    } else if (!strcmp(cam_type.c_str(), "video")) {
      const char *file = conf.get_cstring("input_file");
      outdir = file;
      outdir << "_features/";
      cam = new camera_video<ubyte>
	(file, height, width, conf.get_uint("input_video_sstep"),
	 conf.get_uint("input_video_max_duration"));
    } else eblerror("unknown camera type, set \"camera\" in your .conf");
    dims = idxdim(3, height, width);

    // extraction //////////////////////////////////////////////////////////////
    // now do training iterations
    ostringstream name, fname;
    // fstate_idx<t_net> in(dims), out(1,1,1);
    // fstate_idx<t_label> label(1);

    // allocate threads
    uint nthreads = conf.try_get_uint("nthreads", 1);
    bool updated = false;
    idx<ubyte> detframe; // frame returned by detection thread
    list<fprop_thread<t_net>*>  threads;
    list<fprop_thread<t_net>*>::iterator ithreads;
    mout << "Initializing " << nthreads << " fprop threads." << endl;
    for (uint i = 0; i < nthreads; ++i) {
      ostringstream tname;
      tname << "Thread " << i;
      fprop_thread<t_net> *dt =
	new fprop_thread<t_net>(conf, &out_mutex, tname.str().c_str(),
				NULL, sync);
      threads.push_back(dt);
      dt->start();
      dt->set_output_directory(outdir);
    }
    
    // input/output names
    mkdir_full(outdir);
    mout << "Saving outputs to " << outdir << endl;
    string arch_name = conf.try_get_string("arch_name", "arch");
    string out_name;
    out_name << outdir << arch_name << "_";
    
    string in_fname, out_fname;
    bool threads_loading = conf.exists_true("threads_load");

    // timing variables
    timer tpass, toverall, tstop;
    uint cnt = 0;
    mout << "i=" << cnt << endl;
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
	if ((*ithreads)->finished())
	  continue ;
	finished = false; // a thread is not finished
	string processed_fname;

	// retrieve new data if present
	updated = (*ithreads)->dumped();
	// deal with processed frame 
	if (updated) {
	  cnt++;
	  // output info
	  mout << "i=" << cnt << " processing=" << tpass.elapsed_ms()
	       << " fps=" << cam->fps() << " remaining=" << (cam->size() - cnt)
	       << " elapsed=" << toverall.elapsed();
	  if (cam->size() > 0)
	    mout << " ETA=" << toverall.eta(cnt, cam->size());
	  mout << endl;
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
	      if (skip_frames > 0)
		cam->skip(skip_frames); // skip frames if skip_frames > 0
	      if (cam->empty()) continue ;
	      // grab from cam
	      if (threads_loading) { // we load data here
		frame = cam->grab();
		// send new frame to this thread
		string frame_name = cam->frame_name();
		while (!(*ithreads)->set_data(frame, frame_name, 
					      cam->frame_id()))
		  millisleep(5);
	      } else { // thread loads data itself, just get filename
		in_fname = cam->grab_filename();
		out_fname = "";
		out_fname << out_name << ebl::basename(in_fname.c_str());
		while (!(*ithreads)->set_dump(in_fname, out_fname))
		  millisleep(5);		
	      }
	      // we just sent a new frame
	      tpass.restart();
	    }
	  }
	}
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
    mout << "Execution time: " << toverall.elapsed() << endl;
    // free variables
    if (cam) delete cam;
    for (ithreads = threads.begin(); ithreads != threads.end(); ++ithreads) {
      if (!(*ithreads)->finished())
	  (*ithreads)->stop(); // stop thread without waiting
      delete *ithreads;
    }
#ifdef __GUI__
    if (!conf.exists_true("no_gui_quit")) {
      mout << "Closing windows..." << endl;
      quit_gui(); // close all windows
      mout << "Windows closed." << endl;
    }
#endif
    mout << "fprop done. Running time: " << gtimer.elapsed() << endl;
  } eblcatcherror();
  return 0;
}
