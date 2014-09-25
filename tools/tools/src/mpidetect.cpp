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
#include <stdio.h>
#include "eblapp.h"

#ifdef __MPI__
#include <mpi.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/mpi.hpp>

//namespace mpi = boost::mpi;
#endif

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#ifdef __GUI__
#include "libeblearngui.h"
#endif

typedef float t_net; // network precision

//////////////////////////////////////////////////////////////////////////
// serialization functions

namespace boost {
namespace serialization {

//! Serializes an idx.
template<class Archive>
void serialize(Archive & ar, idx<ubyte>& mat, const unsigned int version) {
	intg d1, d2, d3;
	if (Archive::is_saving::value) { // saving to stream
		if (!mat.contiguousp())
			eblerror("expected contiguous idx for serialization");
		if (mat.order() != 3)
			eblerror("no support for idx order != 3 for now, got: " << mat);

		d1 = mat.dim(0);
		d2 = mat.dim(1);
		d3 = mat.dim(2);

		ar & d1;
		ar & d2;
		ar & d3;
		idx_aloop1(m, mat, ubyte) {
			ar & *m;
		}
	} else { // loading from stream
		ar & d1;
		ar & d2;
		ar & d3;
		idx<ubyte> m(d1, d2, d3);
		ubyte b;
		idx_aloop1(mm, m, ubyte) {
			ar & b; // get ubyte
			*mm = b;
		}
		mat = m; // assign new idx
	}
}

//! Serializes a svector<midx>.
template<class Archive>
void serialize(Archive &ar, midx<t_net> &mats, const unsigned int version) {
	intg d0, d1, d2, d3;
	if (Archive::is_saving::value) { // saving to stream
		d0 = mats.dim(0);
		ar & d0;
		if (mats.order() != 1) eblerror("expected order 1 in " << mats);
		for (intg j = 0; j < mats.dim(0); ++j) {
			idx<t_net> mat = mats.mget(j);
			if (!mat.contiguousp())
				eblerror("expected contiguous idx for serialization");
			if (mat.order() != 3)
				eblerror("no support for idx order != 3 for now, got: " << mat);
			d1 = mat.dim(0);
			d2 = mat.dim(1);
			d3 = mat.dim(2);
			ar & d1;
			ar & d2;
			ar & d3;
			idx_aloop1(mm, mat, t_net) {
				ar & *mm;
			}
		}
	} else { // loading from stream
		ar & d0;
		midx<t_net> m(d0);
		for (intg j = 0; j < m.dim(0); ++j) {
			ar & d1;
			ar & d2;
			ar & d3;
			idx<t_net> mat(d1, d2, d3);
			t_net b;
			idx_aloop1(mm, mat, t_net) {
				ar & b; // get ubyte
				*mm = b;
			}
			m.mset(mat, j);
		}
		mats = m;
	}
}

//! Serializes a bbox.
template<class Archive>
void serialize(Archive & ar, bbox& b, const unsigned int version) {
	ar & b.class_id;
	ar & b.confidence;
	ar & b.h0;
	ar & b.w0;
	ar & b.height;
	ar & b.width;
	ar & b.oscale_index;
	// we dont' really care about other members here
}

} // namespace serialization
} // namespace boost

enum tag { cmd_finished = 0, cmd_get_data = 1, cmd_set_data = 2,
					 cmd_available = 3, cmd_stop = 4 };

// child //////////////////////////////////////////////////////////////

class mpichild {
public:
	//! Constructor.
	mpichild(configuration &conf, boost::mpi::communicator &world_,
					 mutex &out_mutex, const char *thread_name, bool sync)
		: dt(conf, &out_mutex, thread_name, NULL, sync), world(world_),
			mout(dt.get_mout()), merr(dt.get_merr()) {
	}
	virtual ~mpichild() {
	}

	// comm methods //////////////////////////////////////////////////////////////

	// non-blocking check if question was asked, then blocking send data.
	void answer_get_data() {
		// data variables
		idx<ubyte> frame;
		string frame_name;
		uint frame_id;
		bboxes bb;
		vector<bbox> bb2, bbs2;
		uint total_saved;
		svector<midx<t_net> > samples; // extracted samples
		vector<midx<t_net> > vsamples;
		bboxes bbsamples; // boxes corresponding to samples
		bool skipped = false; // the frame was not processed and skipped.
		// we got the question, answer it
		bool new_data = dt.get_data(bb, frame, total_saved, frame_name, &frame_id,
																&samples, &bbsamples, &skipped);
		world.send(0, cmd_get_data, new_data);
		world.send(0, cmd_get_data, skipped);
		if (!new_data)
			return ; // no new data, stop here
		// create a non-pointer vector
		for (uint i = 0; i < bb.size(); ++i) {
			bbox b = bb[i];
			bb2.push_back(b);
		}
		// create a non-pointer vector
		for (uint i = 0; i < bbsamples.size(); ++i) {
			bbox b = bbsamples[i];
			bbs2.push_back(b);
		}
		// create a non-pointer vector
		for (uint i = 0; i < samples.size(); ++i)
			vsamples.push_back(samples[i]);
		int frame_height = frame.dim(0);
		int frame_width = frame.dim(1);
		// send the new data
		world.send(0, cmd_get_data, bb2);
		//    world.send(0, cmd_get_data, frame);
		world.send(0, cmd_get_data, frame_height);
		world.send(0, cmd_get_data, frame_width);
		world.send(0, cmd_get_data, total_saved);
		world.send(0, cmd_get_data, frame_name);
		world.send(0, cmd_get_data, frame_id);
		world.send(0, cmd_get_data, vsamples);
		world.send(0, cmd_get_data, bbs2);
	}

	//////////////////////////////////////////////////////////////////////////

	//! Constructor
	void run() {
		// data variables
		string fullname, frame_name;
		uint frame_id;
		idx<ubyte> uframe;
		bool stopped = false, available = false, finished = false, loaded = false;

		// start thread
		dt.start();

		boost::mpi::request req = world.irecv(0, boost::mpi::any_tag);
		// loop to receive/send messages
		while (1) {
			// wait for master command
			boost::optional<boost::mpi::status> ret = req.test();
			if (ret) {
				boost::mpi::status s = ret.get();
				// execute command
				switch (s.tag()) {
				case cmd_finished:
					finished = dt.finished();
					if (finished)
						mout << "is finished." << endl;
					world.send(0, cmd_finished, finished);
					break ;
				case cmd_get_data:
					if (stopped)
						eblerror("Thread " << world.rank() << " cannot ask stopped thread");
					answer_get_data();
					break ;
				case cmd_set_data:
					if (stopped)
						eblerror("Thread " << world.rank() << " cannot ask stopped thread");
					// get the new data
					world.recv(0, cmd_set_data, fullname);
					world.recv(0, cmd_set_data, frame_name);
					world.recv(0, cmd_set_data, frame_id);
					// try to set data until successful (meaning we got the mutex lock)
					loaded = true; // we don't actually load the image here
					// confirm that image was loaded (fake)
					world.send(0, cmd_set_data, loaded);
					while (!dt.set_data(fullname, frame_name, frame_id))
						millisleep(5);
					break ;
				case cmd_available:
					available = dt.available();
					world.send(0, cmd_available, available);
					break ;
				case cmd_stop:
					if (!stopped) {
						dt.ask_stop();
						// wait that thread finishes
						while (!dt.finished())
							millisleep(5);
						stopped = true;
					}
					break ;
				default:
					eblerror("child " << world.rank() << ": unknown command "
									 << s.tag());
				}
				// wait for a new command
				req = world.irecv(0, boost::mpi::any_tag);
			}
			millisleep(20); // sleep to avoid eating cpu
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// members
private:
	detection_thread<t_net> dt;
	boost::mpi::communicator &world;
	boost::mpi::request req;
	std::ostream &mout, &merr;
};

//////////////////////////////////////////////////////////////////////////
// master class

class mpimaster {
public:

	mpimaster(int argc_, char **argv_, configuration &conf_,
						boost::mpi::communicator &world_,
						ostream &mout_, ostream &merr_)
		: argc(argc_), argv(argv_), conf(conf_), world(world_),
			mout(mout_), merr(merr_) {
	}

	virtual ~mpimaster() {
	}

	//////////////////////////////////////////////////////////////////////////
	// comm methods

	// blocking-ask if thread 'rank' has finished.
	bool ask_thread_finished(int rank) {
		bool finished = 0;
		world.send(rank, cmd_finished); // send command (no data)
		world.recv(rank, cmd_finished, finished); // get bool
		//     if (finished)
		//       mout << "thread " << rank << " is saying its finished." << endl;
		return finished;
	}

	// blocking-ask for new data, return false if no new data is available.
	bool ask_get_data(int rank, vector<bbox> &bb,
										int &frame_height, int &frame_width,
										uint &total_saved, string &frame_name,
										uint &frame_id, svector<midx<t_net> > &samples,
										bboxes &bbsamples, bool &skipped) {
		bool new_data = 0;
		vector<bbox> bbs2;
		vector<midx<t_net> > vsamples;
		world.send(rank, cmd_get_data);
		world.recv(rank, cmd_get_data, new_data);
		world.recv(rank, cmd_get_data, skipped);
		if (!new_data)
			return false; // no new data, stop here
		// get the new data
		bb.clear();
		world.recv(rank, cmd_get_data, bb);
		//    world.recv(rank, cmd_get_data, frame);
		world.recv(rank, cmd_get_data, frame_height);
		world.recv(rank, cmd_get_data, frame_width);
		world.recv(rank, cmd_get_data, total_saved);
		world.recv(rank, cmd_get_data, frame_name);
		world.recv(rank, cmd_get_data, frame_id);
		world.recv(rank, cmd_get_data, vsamples);
		world.recv(rank, cmd_get_data, bbs2);
		// convert outputs
		bbsamples.clear();
		for (uint i = 0; i < bbs2.size(); ++i) bbsamples.push_back_new(bbs2[i]);
		samples.clear();
		for (uint i = 0; i < vsamples.size();++i) samples.push_back_new(vsamples[i]);
		return true;
	}

	// blocking-ask to send data.
	bool ask_set_data(int rank, string &fullname,
										string &frame_name, uint frame_id) {
		bool success = false;
		world.send(rank, cmd_set_data);
		// send the new data
		world.send(rank, cmd_set_data, fullname);
		world.send(rank, cmd_set_data, frame_name);
		world.send(rank, cmd_set_data, frame_id);
		world.recv(rank, cmd_set_data, success);
		if (!success)
			mout << "failed to assign image to slot " << rank << ": "
					 << fullname << endl;
		return success;
	}

	// blocking-ask if thread is available for new data
	bool ask_available(int rank) {
		bool available = false;
		// ask the question, blocking
		world.send(rank, cmd_available);
		world.recv(rank, cmd_available, available);
		return available;
	}

	// blocking-ask the thread to stop
	void ask_ask_stop(int rank) {
		mout << "asking Thread " << rank << " to finish and stop." << endl;
		world.send(rank, cmd_stop);
	}

	//////////////////////////////////////////////////////////////////////////////

	void run() {
		if (conf.exists_true("show_conf")) conf.pretty();
		// config
		uint        ipp_cores				= 1;
		if (conf.exists("ipp_cores")) ipp_cores = conf.get_uint("ipp_cores");
		ipp_init(ipp_cores);	// limit IPP (if available) to 1 core
		bool	save_video				= conf.exists_true("save_video");
		string	cam_type				= conf.get_string("camera");
		int		height					= conf.get_int("input_height");
		int		width					= conf.get_int("input_width");
		bool        input_random				= conf.exists_true("input_random");
		uint        npasses					= 1;
		bool        silent								= conf.exists_true("silent");
		bool        save_detections 			= conf.exists_true("save_detections");
		uint 	save_bbox_period 			= 1;
		uint        timeout                     = conf.try_get_uint("timeout", 0);
		if (conf.exists("save_bbox_period"))
			save_bbox_period = std::max((uint)1, conf.get_uint("save_bbox_period"));
		if (conf.exists("input_npasses"))
			npasses = conf.get_uint("input_npasses");
		// outputs
		string outdir = detection_thread<t_net>::get_output_directory(conf);
		mout << "Saving outputs to " << outdir << endl;
		string viddir;
		if (save_video) {
			viddir << outdir << "video/";
			mkdir_full(viddir);
		}
		// save conf to output dir
		string cname = outdir;
		cname << filename(argv[1]);
		if (conf.write(cname.c_str()))
			mout << "Wrote configuration to " << cname << endl;
		idx<ubyte> classes(1,1);
		try { // try loading classes names but do not stop upon failure
			load_matrix<ubyte>(classes, conf.get_cstring("classes"));
		} catch(string &err) { merr << "warning: " << err << endl; }
		// bbox saving
		t_bbox_saving bbsaving = bbox_none;
		if (conf.exists("bbox_saving"))
			bbsaving = (t_bbox_saving) conf.get_int("bbox_saving");
		bboxes boxes(bbsaving, &outdir, mout, merr);
		bootstrapping<t_net> boot(conf);

		int nprocs = world.size();
		int nthreads = nprocs - 1;
		mout << "Thread M: Initialized " << nthreads << " detection threads." << endl;

		// initialize camera (opencv, directory, shmem or video)
		idx<ubyte> frame;
		camera<ubyte> *cam = NULL;
		if (!strcmp(cam_type.c_str(), "directory")) {
			string dir;
			if (argc >= 3) // read input dir from command line
				dir = argv[2];
			else if (conf.exists("input_dir"))
				dir = conf.get_string("input_dir");
			if (argc >= 3) // read input dir from command line
				cam = new camera_directory<ubyte>(dir.c_str(), height, width,
																					input_random, npasses, mout, merr);
			else if (conf.exists("input_dir")) // read input dir from conf
				cam = new camera_directory<ubyte>(dir.c_str(), height, width,
																					input_random, npasses, mout, merr);
			else eblerror("expected 2nd argument");
		} else if (!strcmp(cam_type.c_str(), "opencv"))
			cam = new camera_opencv<ubyte>(-1, height, width);
#ifdef __LINUX__
		else if (!strcmp(cam_type.c_str(), "v4l2"))
			cam = new camera_v4l2<ubyte>(conf.get_cstring("device"),
																	 height, width);
#endif
		else if (!strcmp(cam_type.c_str(), "shmem"))
			cam = new camera_shmem<ubyte>("shared-mem", height, width);
		else if (!strcmp(cam_type.c_str(), "video")) {
			if (argc >= 3)
				cam = new camera_video<ubyte>
					(argv[2], height, width, conf.get_uint("input_video_sstep"),
					 conf.get_uint("input_video_max_duration"));
			else eblerror("expected 2nd argument");
		} else eblerror("unknown camera type, set \"camera\" in your .conf");

		// answer variables & initializations
		vector<bbox> bb;

		// gui
#ifdef __GUI__
		bool display			 = conf.exists_bool("display");
		bool show_parts        = conf.exists_true("show_parts");
		// mindisplay     = conf.exists_bool("minimal_display");
		// display_sleep  = conf.get_uint("display_sleep");
		// display_states = conf.exists_bool("display_states");
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
		// wid_states  = display_states ? new_window("network states"):0;
		// night_mode();
		uint wid  = display ? new_window("eblearn object recognition") : 0;
		night_mode();
#endif
		// timing variables
		timer tpass, toverall, tstop;
		uint cnt = 0;
		mout << "i=" << cnt << endl;
		bool stop = false, finished = false, updated = false;
		//    idx<ubyte> detframe; // frame returned by detection thread
		int frame_height = -1, frame_width = -1;
		idx<uint> total_saved(nthreads);
		idx_clear(total_saved);
		svector<midx<t_net> > all_samples, samples; // extracted samples
		bboxes all_bbsamples, bbsamples; // boxes corresponding to samples
		// bookkeep jobs assigments
		idx<int> jobs(world.size());
		idx_fill(jobs, -1);
		jobs.set(-2, 0); // master slot is considered finished already

		// loop
		toverall.start();
		while (!finished) {
			// check for results and send new image for each thread
			for (int rank = 1; rank < nprocs; ++rank) {
				if (jobs.get(rank) == -2)
					continue ; // slave is finished, do nothing
				string processed_fname;
				uint processed_id = 0;
				if (jobs.get(rank) >= 0) { // slot is assigned a job
					// retrieve new data if present
					bool skipped = false;
					updated = ask_get_data(rank, bb, frame_height, frame_width,
																 *(total_saved.idx_ptr() + rank - 1),
																 processed_fname, processed_id, samples,
																 bbsamples, skipped);
					if (skipped) cnt++; // a new skipped frame was received
					// save bounding boxes
					if (updated) {
						mout << "received new data of frame " << processed_id
								 << " from Thread " << rank << endl;
						updated = false;
						if (frame_height == -1 || frame_width == -1)
							eblerror("expected positive frame sizes but got: " << frame_height
											 << "x" << frame_width);
						idxdim d(frame_height, frame_width);
						if (bbsaving != bbox_none) {
							mout << "Adding " << bb.size() << " boxes into new group: "
									 << processed_fname << " with id " << processed_id << endl;
							boxes.new_group(d, &processed_fname, processed_id);
							boxes.add(bb, d, &processed_fname, processed_id);
							if (cnt % save_bbox_period == 0)
								boxes.save();
							// avoid sample accumulation if not using bootstrapping
							mout << "Received " << all_samples.size() << " bootstrapping samples."
									 << endl;
						}
						if (conf.exists_true("bootstrapping_save")) {
							all_samples.push_back_new(samples);
							all_bbsamples.push_back_new(bbsamples);
						}
						cnt++;
						// output info
						if (!silent) {
							if (save_detections) {
								mout << "total_saved=" << idx_sum(total_saved);
								if (conf.exists("save_max"))
									mout << " / " << conf.get_uint("save_max");
								mout << endl;
							}
							if (boot.activated())
								mout << "total_bootstrapping=" << all_samples.size() << endl;
							mout << "remaining=" << (cam->size() - cnt)
									 << " elapsed=" << toverall.elapsed();
							if (cam->size() > 0)
								mout << " ETA=" << toverall.eta(cnt, cam->size());
							if (conf.exists("save_max") && save_detections) {
								uint total = idx_sum(total_saved);
								mout << " save_max_ETA="
										 << toverall.eta(total, (conf.get_uint("save_max")));
							}
							mout << endl;
						}
						mout << "i=" << cnt << " processing: " << tpass.elapsed_ms()
								 << " fps: " << cam->fps() << endl;
					}
				}
				// check if ready
				if (jobs.get(rank) == -1 || ask_available(rank)) {
					jobs.set(-1, rank); // remember that this slave is available
					if (stop) {
						ask_ask_stop(rank); // stop but let thread finish
						// check if it is finished
						if (ask_thread_finished(rank))
							jobs.set(-2, rank);
					} else {
						// grab a new frame if available
						if (cam->empty()) {
							stop = true;
							tstop.start(); // start countdown timer
							ask_ask_stop(rank); // ask this thread to stop
							millisleep(50);
							// check if it is finished
							if (ask_thread_finished(rank))
								jobs.set(-2, rank);
						} else {
							string fullname = cam->grab_filename();
							string frame_name = cam->frame_name();
							// send new frame to this thread
							if (ask_set_data(rank, fullname, frame_name, cam->frame_id())) {
								jobs.set(cam->frame_id(), rank);
								// we just sent a new frame
								tpass.restart();
							} else { // failed to set new frame, go back
								cam->previous(); // move back 1 step
							}
						}
					}
				}
			}
			// check if all jobs are done
			if (stop && idx_sum(jobs) == -2 * world.size()) {
				mout << "All jobs finished (sum(jobs): "
						 << idx_sum(jobs) << ")" << endl;
				finished = true;
			}
			// print all jobs status
			jobs.printElems(mout);
			mout << " elapsed=" << toverall.elapsed() << endl;
			// sleep display
			// 	if (display_sleep > 0) {
			// 		mout << "sleeping for " << display_sleep << "ms." << endl;
			// 		millisleep(display_sleep);
			// 	}
			if ((conf.exists("save_max") && !stop &&
					 idx_sum(total_saved) > conf.get_uint("save_max"))
					|| (boot.activated() && all_samples.size() > boot.max_size())) {
				mout << "Reached max number of detections, exiting." << endl;
				stop = true; // limit number of detection saves
				tstop.start(); // start countdown timer
			}
			// sleep a bit between each iteration
			millisleep(5);
			// check if stop countdown reached 0
			if (timeout > 0 && stop && tstop.elapsed_minutes() >= timeout) {
				merr << "threads did not all return 5 min after request, stopping"
						 << endl;
				break ; // program too long to stop, force exit
			}
		}
		// saving boxes
		boxes.save();
		// saving bootstrapping
		if (stop && conf.exists_true("bootstrapping_save"))
			boot.save_dataset(all_samples, all_bbsamples, outdir, classes);
		mout << "Execution time: " << toverall.elapsed() << endl;
		// check all processes are correctly finished
		mout << "Checking all processes are finished..." << endl;
		while (idx_sum(jobs) != -2 * world.size()) {
			for (int rank = 1; rank < nprocs; ++rank) {
				// check if it is finished
				if (ask_thread_finished(rank))
					jobs.set(-2, rank);
			}
			jobs.printElems(mout); mout << endl;
		}

		if (save_video)
			cam->stop_recording(conf.exists_bool("use_original_fps") ?
													cam->fps() : conf.get_uint("save_video_fps"),
													outdir.c_str());
		// free variables
		if (cam) delete cam;
		mout << "Detection finished." << endl;
	}

	//////////////////////////////////////////////////////////////////////////
	// members
private:
	int argc;
	char **argv;
	configuration &conf;
	boost::mpi::communicator &world;
	std::ostream &mout, &merr;
};

//////////////////////////////////////////////////////////////////////////
// main

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
	int main(int argc, char **argv) { // regular main without gui
#endif
#ifndef __MPI__
		eblerror("MPI was not found during compilation, install and recompile");
#else
		// check input parameters
		if ((argc != 2) && (argc != 3) ) {
			cerr << "warning: wrong number of parameters." << endl;
			cerr << "usage: mpidetect <config file> [directory or file]" << endl;
			//	return -1;
		}
#ifdef __LINUX__
		feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
		// MPI vars
		boost::mpi::environment env(argc, argv);
		boost::mpi::communicator world;
		int myid = world.rank();
		try {

			// load configuration
			configuration	conf(argv[1], false, true, false);
			if (!conf.exists("root2") || !conf.exists("current_dir")) {
				string dir;
				dir << dirname(argv[1]) << "/";
				cout << "Looking for trained files in: " << dir << endl;
				conf.set("root2", dir.c_str());
				conf.set("current_dir", dir.c_str());
			}
			conf.resolve(); // manual call to resolving variable
			uint              ipp_cores     = 1;
			if (conf.exists("ipp_cores")) ipp_cores = conf.get_uint("ipp_cores");
			ipp_init(ipp_cores); // limit IPP (if available) to 1 core
			// output synchronization
			bool sync = conf.exists_true("sync_outputs");
			mutex out_mutex;
			mutex_ostream mutout(std::cout, &out_mutex, "Thread M");
			mutex_ostream muterr(std::cerr, &out_mutex, "Thread M");
			ostream &mout = sync ? mutout : cout;
			ostream &merr = sync ? muterr : cerr;

			// At this point, all programs are running equivalently, the rank
			// distinguishes the roles of the programs in the SPMD model,
			// with rank 0 often used specially...
			if (myid == 0) {
				mpimaster master(argc, argv, conf, world, mout, merr);
				master.run();
				mout << "stopping all MPI processes." << endl;
				env.abort(0);
			} else {
				string tname;
				tname << "Thread " << myid;
				if (conf.exists("HOSTNAME"))
					tname << " (" << conf.get_string("HOSTNAME") << ")";
				mpichild child(conf, world, out_mutex, tname.c_str(), sync);
				child.run();
			}
		} catch(eblexception &e) {
			cout << "Thread " << myid << ": " << e << endl;
		}
#endif
		return 0;
	}
