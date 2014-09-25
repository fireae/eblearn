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

#include "mpijob.h"
#include "utils.h"
#include "string_utils.h"
#include "tools_utils.h"

#define CMD_RUN 0
#define CMD_RUNNING 1
#define CMD_RUNTIME 2
#define CMD_CONF 3
#define CMD_RESUME 4
#define CMD_STOP 5

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // mpijob

  mpijob::mpijob(configuration &conf_, const char *oconffname,
		 bool resume)
    : job(conf_, oconffname, resume) {
  }

  mpijob::~mpijob() {
  }

  void mpijob::run() {
#ifndef __WINDOWS__
    // no forking, call child serially directly
    _started = true;
    _alive = true;
    declare_started();
    run_child();
    //t.stop();
#else
    eblerror("not implemented");
#endif
  }

  void mpijob::run_child() {
#ifndef __WINDOWS__
    // check that this child hasn't been finished by others
    if (check_finished()) {
      cout << "child " << conf.get_name()
	   << " is already finished, do nothing." << endl;
      return ; // already finished, do nothing
    }
    // start timer
    t.start();
    std::string cmd, log, errlog;
    log << "out_" << conf.get_name() << ".log";
    errlog << "out_" << conf.get_name() << ".errlog";
    // prepare command
    cmd << "cd " << outdir << " && echo \"job=" << conf.get_name();
    if (conf.exists("meta_conf_shortname"))
      cmd << " meta_conf_shortname=" << conf.get_string("meta_conf_shortname");
    // not finished, check if it has been started in the past
    if (check_started()) {
      cout << "child " << conf.get_name()
	   << " is already started but not finished, resuming." << endl;
      figure_resume_out();
      if (conf.exists("retrain_iteration") && conf.exists("retrain_weights")
	  && conf.exists("retrain")) {
	// add retrain params at the end of job's conf
	ofstream of(confname.c_str(), ios_base::app);
	if (!of)
	  eblerror("failed to open conf for appending retrain params: "
		   << confname);
	of << endl
	   << " retrain_iteration=" << conf.get_string("retrain_iteration")
	   << endl << " retrain_weights=" << conf.get_string("retrain_weights")
	   << endl << " retrain=" << conf.get_string("retrain") << endl;
	of.close();
      }
    }
    // set classe filename if defined
    if (rconf.exists("train") || rconf.exists("train_classes")) {
      std::string classesname = conf.get_output_dir();
      if (rconf.exists("train"))
	classesname << "/" << rconf.get_string("train");
      else if (rconf.exists("train_classes"))
	classesname << "/" << rconf.get_string("train_classes");
      classesname << CLASSES_NAME << MATRIX_EXTENSION;
      cmd << " classes=" << classesname;
    }
    // set rest of command
    cmd << " config=" << confname << "\" >> "
	<< log << " && ((" << exe << " " << confname
	<< " 3>&1 1>&2 2>&3 | tee /dev/tty | tee -a " << errlog
	<< ") 3>&1 1>&2 2>&3) >> " << log << " 2>&1 && exit 0";

    // execute child
    std::string jname = basename(confname.c_str());
    cout << endl << "(mpi) Executing job " << jname
	 << " with cmd:" << endl << cmd << endl;
    int ret = std::system(cmd.c_str());
    if (ret != 0) {
      cerr << "execution of job " << jname << " with command "
	   << cmd << " failed with return error: " << ret << endl;
      cerr << "WIFSIGNALED(" << ret << "): " << WIFSIGNALED(ret) << endl;
      cerr << "WTERMSIG(" << ret << "): " << WTERMSIG(ret) << endl;
    }
#else
    eblerror("not implemented");
#endif
  }

  ////////////////////////////////////////////////////////////////
  // mpijob manager

  mpijob_manager::mpijob_manager(int argc, char **argv)
#ifdef __MPI__
    : env(argc, argv), use_master(false)
#endif
  {
#ifndef __MPI__
  eblerror("MPI was not found during compilation, install and recompile");
#else
  rank = world.rank();
  jslots = world.size(); // number of available cpus
  ready_slots = jslots;
  id_running = -1; // no job running intially
#endif
  }

  mpijob_manager::~mpijob_manager() {
  }

  bool mpijob_manager::read_metaconf(const char *fname, const std::string *tstamp,
																		 const char *resume_name, bool resumedir,
																		 bool nomax, int maxjobs, const string *extra) {
    if (is_master()) { // only read metaconf if we are the master
      job_manager::read_metaconf(fname, tstamp, resume_name, resumedir, true,
																 maxjobs, extra);
    }
//       mconf_fullfname = fname;
//       size_t pos = mconf_fullfname.find_last_of('/');
//       mconf_fname = mconf_fullfname.substr(pos == std::string::npos ? 0 : pos);
//       // read meta configuration
//       if (!mconf.read(mconf_fullfname.c_str(), false, tstamp, false,
// 		      resume_name))
// 	return false;
//       // read meta configuration as a simple configuration, to be resolved
//       rmconf.read(mconf_fullfname.c_str(), true, tstamp, false, resume_name,
// 		  true);
//       // set resolved elements into unresolved mconf
//       mconf.set_output_dir(rmconf.get_output_dir());
//       mconf.set_name(rmconf.get_name());
//       // determine all possible configurations, based on conf or existing dirs
//       vector<configuration> confs;
//       if (!resume_name) // we are not resuming, only 1 choice: based on conf
// 	confs = mconf.configurations();
//       else { // we are resuming, based on conf or dirs
// 	if (rmconf.exists("meta_resume")) {
// 	  const char *r = rmconf.get_cstring("meta_resume");
// 	  if (!strcmp(r, "conf")) { // determine jobs based on conf
// 	    cout << "Resuming jobs based on configuration: " << fname << endl;
// 	    confs = mconf.configurations();
// 	  } else if (!strcmp(r, "directories")) { // determine jobs based on dir
// 	    // list all directories that start with same dir name + _conf
// 	    std::string dir = dirname(fname);
// 	    std::string patt = resume_name;
// 	    patt << "_conf*";
// 	    cout << "Resuming jobs based on directory " << dir
// 		 << " and pattern " << patt << endl;
// 	    // find dirs non recursively
// 	    std::list<std::string> *l = find_fullfiles(dir, patt.c_str(),
// 					     NULL, true, false, false);
// 	    if (!l)
// 	      eblerror("failed to find existing job directories to resume in "
// 		       << dir << " with pattern: " << patt);
// 	    // for each dir, check that its conf exists and add it
// 	    std::string odir = rmconf.get_output_dir();
// 	    for (std::list<std::string>::iterator li = l->begin(); li != l->end(); ++li) {
// 	      std::string c = *li;
// 	      c << "/" << basename(c.c_str()) << ".conf";
// 	      if (file_exists(c.c_str())) {
// 		configuration con;
// 		con.read(c.c_str(), false, false, true, odir.c_str());
// 		confs.push_back(con);
// 	      } else {
// 		cerr << "warning: no job conf found in " << *li << endl;
// 	      }
// 	      delete l;
// 	    }
// 	  } else
// 	    eblerror("unknown meta_resume value: " << r);
// 	}
//       }
//       // create job list from all possible configurations
//       vector<configuration>::iterator iconf = confs.begin();
//       for ( ; iconf != confs.end(); ++iconf) {
// 	//iconf->resolve();
// 	jobs.push_back(new mpijob(*iconf, mconf_fname.c_str(),
// 				  (resume_name == NULL) ? false : true));
//       }
//       if (mconf.exists("meta_watch_interval"))
// 	swait = rmconf.get_uint("meta_watch_interval");
//       max_jobs = jslots;
//     }
    return true;
  }

  void mpijob_manager::prepare(bool reset_progress) {
    // only the master prepares the directory structure
    if (is_master()) prepare();
  }

  void mpijob_manager::run(bool force_start) {
    if (is_master()) run_master();
    else run_slave();
  }

  uint mpijob_manager::world_size() {
#ifdef __MPI__
    return world.size();
#else
    eblerror("not implemented");
    return 0;
#endif
  }

  bool mpijob_manager::is_master() {
    return (bool) (rank == 0);
  }

  /////////////////////////////////////////////////////////////////////////////
  // protected members

  void mpijob_manager::run_master() {
    cout << "Running master job." << endl;
#ifdef __MPI__
    time.start();
    // print info
    std::string prefix = "master: ";
    cout << prefix << "world has " << jslots << " slots." << endl;
    cout << prefix << jobs.size() << " jobs to process." << endl;
    // ask each slave if available
    boost::mpi::request *reqs = new boost::mpi::request[world.size()];
    int *running = new int[world.size()];
    for (int i = 0; i < world.size(); ++i) {
      running[i] = -1; // not running initially
      if (i > 0) {
	world.isend(i, CMD_RUNNING, 0);
	reqs[i] = world.irecv(i, CMD_RUNNING, running[i]);
      }
    }
    jinfos(running);
    unstarted = jobs.size();
    // loop until all jobs are finished
    while (nalive || unfinished > 0) {
      // if there are jobs waiting to be started, start them if possible
      for (uint i = 0; i < jobs.size(); ++i) {
	// check status of job i from files
	jobs[i]->check_finished();
	jobs[i]->check_started();
	jobs[i]->check_running();
	// start job if not finished and not alive
	if (!jobs[i]->finished() && !jobs[i]->running()) {
	  // check if master can run this job
	  if (use_master && running[0] == -1) {
	    cout << prefix << "slot 0 is free" << endl;
	    if (assign(i, 0)) {
	      running[0] = i;
	      jinfos(running);
	      break ;
	    }
	  }
	  // or check slaves
	  for (int j = 1; j < world.size(); ++j) {
	    // check if this slot has answered and what its answer is
	    if (reqs[j].test() && running[j] == -1) { // answer and free
	      cout << prefix << "slot " << j << " is free" << endl;
	      if (assign(i, j)) {
		running[j] = i;
		// re-ask if slot j is available for when its done
		world.isend(j, CMD_RUNNING, 0);
		reqs[j] = world.irecv(j, CMD_RUNNING, running[j]);
		jinfos(running);
		break ; // move to next job
	      }
	    }
	  }
	}
      }
      jobs_info();
      jinfos(running);
      secsleep(swait);
      report();
    }
    stop_all();
    last_report();
    delete running;
    // deleting reqs makes the program crash
    //delete reqs;
    cout << "MPI master is finished after " << time.elapsed() << endl;
    exit(0);
#endif
  }

  void mpijob_manager::stop_all() {
#ifdef __MPI__
    cout << "master: ordering all slaves to stop." << endl;
    boost::mpi::request *reqs = new boost::mpi::request[world.size()];
    for (int i = 1; i < world.size(); ++i)
      reqs[i] = world.isend(i, CMD_STOP, 0);
    wait_all(reqs + 1, reqs + world.size());
    cout << "master: all slaves stopped." << endl;
    // deleting reqs makes the program crash
    //    delete reqs;
#endif
  }

  bool mpijob_manager::assign(uint jobid, uint slave_id) {
#ifdef __MPI__
    std::string prefix = "master: ";
    cout << prefix << "assigning job " << jobid << " to slot " << slave_id
	 << endl;
    if (slave_id == 0) { // master
      if (id_running != -1) eblerror("already running job " << id_running);
      id_running = jobid;
      cout << prefix << "assigned job " << jobid << " to slot " << slave_id
	   << endl;
      // output dir is the one up where conf is
      timer t;
      std::string confname = jobs[jobid]->name();
      bool resume = jobs[jobid]->resumed();
      std::string outdir = dirname(confname.c_str());
      outdir = dirname(outdir.c_str());
      configuration c;
      c.read(confname.c_str(), true, false, true, outdir.c_str());
      mpijob *j = new mpijob(c, NULL, (bool) resume);
      t.start();
      j->run();
      delete j;
      cout << prefix << "job " << jobid << " has finished after "
	   << t.elapsed() << endl;
      return true;
    } else { // slaves
      // ask slave
      std::string confname = jobs[jobid]->name();
      bool resumed = jobs[jobid]->resumed();
      world.send(slave_id, CMD_RUN, jobid);
      world.send(slave_id, CMD_CONF, confname);
      world.send(slave_id, CMD_RESUME, resumed);
      // remember in master that this job has been started
      jobs[jobid]->force_started();
      cout << prefix << "assigned job " << jobid << " to slot " << slave_id
	   << endl;
      return true;
    }
    cerr << prefix << "failed to assign job " << jobid << " to slot "
	 << slave_id << endl;
#endif
    return false;
  }

  void mpijob_manager::jinfos(int running[]) {
#ifdef __MPI__
    nalive = 0;
    nrunning = 0;
    unstarted = 0;
    mintime = 0.0;
    maxtime = 0.0;
    infos.str("");
    // count alive slots
    for (int i = 0; i < world.size(); ++i)
      if (running[i] >= 0) nalive++;
    ready_slots = jslots - nalive;
    if (!use_master) ready_slots--;
    // count unstarted/unfinished/running
    for (uint i = 0; i < jobs.size(); ++i) {
      jobs[i]->check_running();
      jobs[i]->check_finished();
      jobs[i]->check_started();
      if (!jobs[i]->started() && !jobs[i]->finished()) unstarted++;
      if (jobs[i]->running()) nrunning++;
      infos << i << ". pid: " << jobs[i]->getpid() << ", name: "
	    << jobs[i]->name() << ", status: "
	    << (jobs[i]->alive() ? "alive" : "dead")
	    << ", minutes: " << jobs[i]->minutes() << endl;
    }
    cout << "Jobs alive: " << nalive << ", waiting to start: " << unstarted
	 << " / " << jobs.size()
	 << ", empty (MPI) job slots: " << ready_slots << ", Iteration: "
	 << maxiter << ", elapsed: " << time.elapsed() << ", ETA: "
	 << time.eta(jobs.size() - unstarted, jobs.size()) << endl;
    cout << "Running: ";
    int k = use_master ? 0 : 1;
    for ( ; k < world.size(); ++k) cout << running[k] << " ";
    cout << endl;
#endif
  }

  void mpijob_manager::run_slave() {
#ifdef __MPI__
    int running;
    while (1) {
      std::string prefix;
      prefix << "slave " << rank << ": ";
      // wait for master command
      int value = -1, resume = 0;
      std::string confname, outdir;
      mpijob *j;
      configuration c;
      timer t;
      boost::mpi::status s = world.recv(0, boost::mpi::any_tag, value);
      //cout << prefix << "received command " << s.tag() << " with value " << value << endl;
      // execute command
      switch (s.tag()) {
      case CMD_STOP: // stop
	cout << prefix << "stopping." << endl;
	break ;
      case CMD_RUN: // run a new job
	cout << prefix << "running job " << value << endl;
	s = world.recv(0, CMD_CONF, confname);
	cout << prefix << "job's configuration: " << confname << endl;
	s = world.recv(0, CMD_RESUME, resume);
	cout << prefix << "resume job or not: " << resume << endl;
	// output dir is the one up where conf is
	outdir = dirname(confname.c_str());
	outdir = dirname(outdir.c_str());
	c.read(confname.c_str(), true, false, true, outdir.c_str());
	j = new mpijob(c, NULL, (bool) resume);
	t.start();
	j->run();
	delete j;
	id_running = value; // remember which job we are running
	cout << prefix << "job " << value << " has finished after "
	     << t.elapsed() << endl;
	break ;
      case CMD_RUNNING: // tell master if we are running or not
	running = -1;
	world.send(0, CMD_RUNNING, running);
	break ;
      case CMD_RUNTIME: // send master job's running time
	break ;
      default:
	cerr << "unknown master command: " << s.tag() << endl;
      }
    }
#endif
  }

} // namespace ebl
