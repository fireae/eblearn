/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <map>
#include <stdlib.h>
#include <signal.h>
#include <iomanip>

#ifndef __WINDOWS__
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#ifdef __BOOST__
#ifndef BOOST_FILESYSTEM_VERSION
#define BOOST_FILESYSTEM_VERSION 3
#endif
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#endif

#include "numerics.h"
#include "job.h"
#include "string_utils.h"
#include "tools_utils.h"
#include "metaparser.h"

using namespace std;

namespace ebl {

////////////////////////////////////////////////////////////////
// job

job::job(configuration &conf_, const char *oconffname, bool resume)
	: conf(conf_), rconf(conf_), _locally_started(false), _started(false),
		_running(false), _alive(false),
		pid(-1), resumed_(resume), _finished(false), _progress(-1) {
	// the resolved conf
	rconf.resolve();
	// resolve conf at user's request (default is unresolved)
	if (rconf.exists("meta_resolve")) {
		const char *r = rconf.get_cstring("meta_resolve");
		if (!strcmp(r, "all")) // resolve all
			conf.resolve();
		else if (!strcmp(r, "backquotes")) // resolve backquotes only
			conf.resolve_bq();
		else if (!strcmp(r, "none")) // do nothing
			;
		else
			eblerror("unknown meta_resolve value: " << r);
	}
	// set resolved elements into unresolved mconf
	conf.set_output_dir(rconf.get_output_dir());
	conf.set_name(rconf.get_name());
	// optional original configuration filename
	if (oconffname)
		oconffname_ = oconffname;
	// extract executable
	exe = rconf.get_string("meta_command");
	// remove quotes around executable command if present
	if ((exe[0] == '"') && (exe[exe.size() - 1] == '"'))
		exe = exe.substr(1, exe.size() - 2);
	// set output directory
	outdir = "";
	outdir << rconf.get_output_dir() << "/" << rconf.get_name();
	// set configuration filename
	confname = "";
	confname << outdir << "/" << rconf.get_name() << ".conf";
	// set progress and finished filename
	progress_fname = get_root();
	progress_fname << "/progress";
	finished_fname = get_root();
	finished_fname << "/finished";

	//     // check if jobs has been finished in the past based on existing files
	//     check_finished();
	//     // check if jobs has been started in the past based on existing files
	//     check_started();

}

job::~job() {
}

void job::run() {
#ifndef __WINDOWS__
	// fork job
	_started = true;
	_locally_started = true;
	_alive = true;
	declare_started();
	pid = (int) fork();
	if (pid == -1)
		eblerror("fork failed");
	if (pid == 0) { // child code
		run_child();
	}
#else
	eblerror("not implemented");
#endif
}

bool job::started() {
	return _started;
}

bool job::write(bool reset_progress) {
	std::string cmd, tmp;
	// create directories
	mkdir_full(outdir.c_str());
	// create configuration file
	conf.set("job_name", rconf.get_name().c_str());// add config name into conf
	// reset progress
	if (reset_progress) {
		std::string fn = progress_fname;
		if (!rm_file(fn.c_str()))
			cerr << "failed to remove " << fn << endl;
		else cout << "removed " << fn << endl;
		fn = finished_fname;
		if (!rm_file(fn.c_str()))
			cerr << "failed to remove " << fn << endl;
		else cout << "removed " << fn << endl;
	}
	//    conf.resolve();
	if (!conf.write(confname.c_str()))
		return false;
	// copy classes file into directory
	if (rconf.exists("train") && rconf.exists("root")
			&& rconf.exists_true("meta_copy_classes")) {
		std::string classesname, tmp, cmd;
		classesname << rconf.get_string("root") << "/" << rconf.get_string("train");
		classesname << "_" << CLASSES_NAME << MATRIX_EXTENSION;
		if (file_exists(classesname.c_str())) {
			cmd << "cp " << classesname << " " << outdir << "/";
			tmp << rconf.get_string("train") << "_" << CLASSES_NAME << MATRIX_EXTENSION;
			cmd << tmp;
			int res = std::system(cmd.c_str());
			if (res < 0) cerr << "warning: command failed: " << cmd << endl;
			else cout << "copied class names file: " << cmd << endl;
		}
	}
	//     // write conf in original metaconf filename
	//     if (!oconffname_.empty()) {
	//       tmp.str(""); tmp << outdir << "/" << oconffname_;
	//       if (!conf.write(tmp.str().c_str()))
	// 	return false;
	//     }
	return true;
}

bool job::running() {
	return _running;
}

bool job::alive() {
	// first check if this job was ever started locally
	if (!_locally_started)
		return false;
#ifndef __WINDOWS__
	// if job is alive, it will receive this harmless signal
	return (kill((pid_t) pid, SIGCHLD) == 0);
#else
	eblerror("not implemented");
	return false;
#endif
}

int job::getpid() {
	return pid;
}

std::string& job::name() {
	return confname;
}

std::string job::shortname() {
	return conf.get_string("meta_conf_shortname");
}

std::string job::get_root() {
	std::string root = rconf.get_output_dir();
	root << "/" << rconf.get_name();
	return root;
}

double job::minutes() {
	return t.elapsed_minutes();
}

void job::force_started() {
	_started = true;
}

void job::reset_started() {
	cout << "Setting " << name() << " as not started." << endl;
	_started = false;
}

std::string job::elapsed() {
	return t.elapsed();
}

bool job::resumed() {
	return resumed_;
}

bool job::finished() {
	return _finished;
}

int job::progress() const {
	return _progress;
}

////////////////////////////////////////////////////////////////
// file-based resuming capabilities

bool job::declare_started() {
	// create file if not created, but do not discard content (append)
	ofstream of(progress_fname.c_str(), ios_base::app);
	if (!of) {
		cerr << "error: failed to open " << progress_fname << endl;
		return false;
	}
	of.close();
	// update timestamp
	if (!touch_file(progress_fname))
		cerr << "warning: failed to update modified timestamp of "
				 << progress_fname << endl;
	return true;
}

bool job::declare_finished() {
	// create file if not created, but do not discard content (append)
	ofstream of(finished_fname.c_str(), ios_base::app);
	if (!of) {
		cerr << "error: failed to open " << finished_fname << endl;
		return false;
	}
	of.close();
	return true;
}

bool job::check_started() {
	bool res = file_exists(progress_fname);
	if (res)
		_started = true;
	return res;
}

bool job::check_running() {
	if (!_started) return false;
	// if progress last modified less than an hour, it is running
	int secs = 3600;
	if (conf.exists("meta_timeout"))
		secs = conf.get_int("meta_timeout");
	if (file_modified_elapsed(progress_fname.c_str()) < secs) {
		_running = true;
		return true;
	}
	_running = false;
	return false;
}

void job::write_progress(uint i, uint total, const char *additional,
												 const char *root) {
	std::string fname = get_progress_filename();
	ofstream progress(fname.c_str());
	if (!progress)
		cerr << "warning: failed to create file " << fname.c_str() << endl;
	else {
		// set retrain to next iteration with current saved weights
		progress << "i = " << i << endl
						 << "total = " << total << endl;
		if (additional)
			progress << additional;
		progress.close();
	}
}

std::string job::get_progress_filename(const char *root) {
	std::string fname;
	if (root)
		fname << root << "/";
	fname << "progress";
	return fname;
}

void job::write_finished(const char *root) {
	std::string progname, finname;
	if (root) {
		progname << root << "/";
		finname << root << "/";
	}
	progname << "progress";
	finname << "finished";
	// if progress file exists, move it to finished
	if (file_exists(progname.c_str())) {
		std::string cmd;
		cmd << "mv " << progname << " " << finname;
		int ret = std::system(cmd.c_str());
		if (ret < 0)
			cerr << "warning: failed to move " << progname << " to "
					 << finname << endl;
	} else { // otherwise just touch finished
		touch_file(finname);
	}
}

int job::check_progress() {
	_progress = 0;
	if (!file_exists(progress_fname))
		_progress = -1;
	else if (file_size(progress_fname) > 0) {
		// read progress file as a configuration
		conf.read(progress_fname.c_str(), false, false, true);
		// check if progress info is present
		if (conf.exists("i") &&
				conf.exists("total")) {
			float per = (conf.get_float("i") - 1) /
				conf.get_float("total");
			_progress = (int) (per * 100);
		}
	}
	return _progress;
}

bool job::check_finished() {
	bool res = file_exists(finished_fname);
	if (res) {
		_finished = true;
		_progress = 100;
	}
	return res;
}

////////////////////////////////////////////////////////////////
// protected

void job::figure_resume_out() {
	std::string fname = get_root();
	fname << "/progress";
	if (!file_exists(fname))
		cerr << "no progress file found to resume job " << rconf.get_name();
	else {
		// read progress file as a configuration
		conf.read(fname.c_str(), false, false, true);
		// check if progress info is present
		if (!conf.exists("retrain_iteration") ||
				!conf.exists("retrain_weights") ||
				(conf.exists("retrain_weights") &&
				 !file_exists(conf.get_cstring("retrain_weights")))) {
			// not present, try to figure out ourselves
			figure_resume_from_weights();
		}
		// check again if info was found
		if (!conf.exists("retrain_iteration") ||
				!conf.exists("retrain_weights")) {
			cerr << "no resume info found in progress file for job "
					 << rconf.get_name() << endl;
		} else { // info was found
			conf.set("retrain", "1"); // enable retraining
			cout << "Resuming job " << rconf.get_name() << " from iteration "
					 << conf.get_int("retrain_iteration") << " and file "
					 << conf.get_string("retrain_weights") << endl;
		}
	}
}

void job::figure_resume_from_weights(const char *dir_) {
	std::string dir = outdir;
	if (dir_) dir = dir_;
	// find latest saved parameters
	std::string pattern = ".*_net[0-9]*.mat";
	cout << "Looking for latest files for job " << rconf.get_name() << " here: "
			 << dir << " with pattern: " << pattern << endl;
	std::list<std::string> *files = find_fullfiles(dir, pattern.c_str(), NULL,
																								 true, false);
	if (!files || files->size() == 0)
		cout << "no parameters to resume for job " << rconf.get_name() << endl;
	else {
		std::string resume_file = files->back();
		cout << "Resuming job " << rconf.get_name() << " with file: "
				 << resume_file << endl;
		conf.set("retrain", "1");
		conf.set("retrain_weights", resume_file.c_str());
		delete files;
		// extract last iteration number
		size_t i1 = resume_file.find_last_of("0123456789");
		std::string iter = resume_file.substr(0, i1 + 1);
		i1 = iter.find_last_not_of("0123456789");
		iter = iter.substr(i1 + 1);
		uint i = string_to_uint(iter);
		cout << "Infered iteration # " << i << " from file name "
				 << resume_file << endl;
		conf.set("retrain_iteration", iter.c_str());
	}
}

void job::run_child() {
#ifndef __WINDOWS__
	// start timer
	t.start();
	std::string cmd, log, errlog;
	log << "out_" << rconf.get_name() << ".log";
	errlog << "out_" << rconf.get_name() << ".errlog";
	// prepare command
	cmd << "cd " << outdir << " && echo \"job=" << rconf.get_name();
	if (conf.exists("meta_conf_shortname"))
		cmd << " meta_conf_shortname=" << conf.get_string("meta_conf_shortname");
	if (conf.exists("meta_conf_fullname"))
		cmd << " meta_conf_fullname=" << conf.get_string("meta_conf_fullname");
	if (conf.exists("meta_conf_variables"))
		cmd << " meta_conf_variables=" << conf.get_string("meta_conf_variables");
	// resume params
	if (resumed_) {
		figure_resume_out();
		if (conf.exists("retrain_iteration") && conf.exists("retrain_weights")
				&& conf.exists("retrain")) {
			// add retrain params at the end of job's conf
			ofstream of(confname.c_str(), ios_base::app);
			if (!of)
				eblerror("failed to open conf for appending retrain params: "
								 << confname);
			of << endl
				 << " retrain_iteration=" << conf.get_uint("retrain_iteration")
				 << endl << " retrain_weights=" << conf.get_string("retrain_weights")
				 << endl << " retrain=" << conf.get_string("retrain") << endl;
			of.close();
		}
	}
	// set classe filename if defined
	if (rconf.exists("train") || rconf.exists("train_classes")) {
		std::string classesname = rconf.get_output_dir();
		if (rconf.exists("train"))
			classesname << "/" << rconf.get_string("train");
		else if (rconf.exists("train_classes"))
			classesname << "/" << rconf.get_string("train_classes");
		classesname << "_" << CLASSES_NAME << MATRIX_EXTENSION;
		cmd << " classes=" << classesname;
	}
	// rest of command
	cmd << " config="
			<< confname << "\" >> "
			<< log << " && ((" << exe << " " << confname
			<< " 3>&1 1>&2 2>&3 | tee /dev/tty | tee -a " << errlog
			<< ") 3>&1 1>&2 2>&3) >> " << log << " 2>&1 && touch " << outdir
			<< "/finished && exit 0";

	cout << endl << "Executing job " << filename(confname.c_str())
			 << " with cmd:" << endl << cmd << endl;
	// execl takes over this process (and its pid)
	execl("/bin/sh", "sh", "-c", cmd.c_str(), (char*)NULL);
#else
	eblerror("not implemented");
#endif
}

bool job_progress_cmp(const job *a, const job *b) {
	return a->progress() < b->progress();
}

////////////////////////////////////////////////////////////////
// job manager

job_manager::job_manager() : copy_path(""), max_jobs(limits<int32>::max()),
														 maxiter(-1), mintime(0.0), maxtime(0.0),
														 nalive(1), nrunning(0), unstarted(0), finished(0),
														 unfinished(1),
														 ready_slots(max_jobs), swait(30) {
}

job_manager::~job_manager() {
	for (uint i = 0; i < jobs.size(); ++i)
		if (jobs[i] != NULL)
			delete jobs[i];
}

bool job_manager::read_metaconf(const char *fname, const std::string *tstamp,
																const char *resume_name,
																bool resumedir, bool nomax, int maxjobs,
																const string *extra) {
	mconf_fullfname = fname;
	mconf_fname = basename(mconf_fullfname.c_str());
	// read meta configuration
	if (!mconf.read(mconf_fullfname.c_str(), false, tstamp, false,
									resume_name, true, extra))
		return false;
	// read meta configuration as a simple configuration, to be resolved
	rmconf.read(mconf_fullfname.c_str(), true, tstamp, true, resume_name, true,
							extra);
	// set resolved elements into unresolved mconf
	mconf.set_output_dir(rmconf.get_output_dir());
	mconf.set_name(rmconf.get_name());

	// determine all possible configurations, based on conf or existing dirs
	vector<configuration> confs;
	if (!resume_name) // we are not resuming, only 1 choice: based on conf
		confs = mconf.configurations();
	else { // we are resuming, based on conf or dirs
		if (!resumedir) { // determine jobs based on conf
			cout << "Resuming jobs based on configuration: " << fname << endl;
			confs = mconf.configurations();
		} else { // determine jobs based on dir
			// list all directories that start with same dir name + _conf
			std::list<std::string> *l = list_job_dirs(fname);
			if (!l || l->size() == 0) eblerror("no job directory found");
			uint lsize = l->size();
			// for each dir, check that its conf exists and add it
			std::string odir = rmconf.get_output_dir();
			uint k = 0;
			configuration tmpconf(rmconf);
			for (std::list<std::string>::iterator li = l->begin();
					 li != l->end(); ++li, ++k) {
				std::string c = *li;
				std::string j = basename(c.c_str());
				c << "/" << j << ".conf";
				if (file_exists(c.c_str())) {
					// 			configuration con;
					// 			con.read(c.c_str(), false, false, true, odir.c_str());

					// for efficiency, do not actually read conf, just set the right
					// name. the configuration is already written anyway.
					tmpconf.set_name(j);
					confs.push_back(tmpconf);
					cout << k << " / " << lsize << ": loaded job " << c << endl;
				} else {
					cerr << "warning: no job conf found in " << *li << endl;
				}
			}
			cout << "Found " << confs.size() << " jobs from directories." << endl;
			delete l;
		}
	}
	// create job list from all possible configurations
	vector<configuration>::iterator iconf = confs.begin();
	for ( ; iconf != confs.end(); ++iconf) {
		//iconf->resolve();
		jobs.push_back(new job(*iconf, mconf_fname.c_str(),
													 (resume_name == NULL) ? false : true));
	}
	// some minor parameters
	if (!nomax && (rmconf.exists("meta_max_jobs") || maxjobs > 0)) {
		if (maxjobs > 0) max_jobs = maxjobs;
		else if (rmconf.exists("meta_max_jobs"))
			max_jobs = rmconf.get_int("meta_max_jobs");
		cout << "Limiting to " << max_jobs << " jobs at the time." << endl;
	}
	if (rmconf.exists("meta_watch_interval"))
		swait = rmconf.get_uint("meta_watch_interval");
	if (rmconf.exists_bool("meta_send_email")) {
		if (rmconf.exists("meta_email"))
			cout << "Using email: " << rmconf.get_string("meta_email") << endl;
		else
			cout << "Warning: required sending email but no email address defined."
					 << endl;
	}
	return true;
}

void job_manager::set_copy(const std::string &path) {
	if (path.size()) {
		copy_path = path;
		cout << "Enabling copy into jobs folders of: " << path << endl;
	}
}

void job_manager::prepare(bool reset_progress) {
	std::string cmd;
	// create output dir
	mkdir_full(rmconf.get_output_dir().c_str());
	// copy metaconf into jobs' root
	cmd = "";
	cmd << "cp " << mconf_fullfname << " " << rmconf.get_output_dir();
	if (std::system(cmd.c_str()))
		cerr << "warning: failed to execute: " << cmd << endl;
	// copy resolved metaconf into jobs' root
	std::string fn = rmconf.get_output_dir();
	fn << "/" << mconf_fname << "_resolved.conf";
	rmconf.write(fn.c_str());
	// copy classes file if defined into job's root
	if ((rmconf.exists("train") || rmconf.exists("train_classes"))
			&& rmconf.exists("root")) {
		std::string classesname = rmconf.get_string("root");
		if (rmconf.exists("train"))
			classesname << "/" << rmconf.get_string("train");
		else if (rmconf.exists("train_classes"))
			classesname << "/" << rmconf.get_string("train_classes");
		classesname << "_" << CLASSES_NAME << MATRIX_EXTENSION;
		if (file_exists(classesname.c_str())) {
			cmd = "";
			cmd << "cp " << classesname << " " << rmconf.get_output_dir();
			int res = std::system(cmd.c_str());
			if (res < 0) cerr << "warning: command failed: " << cmd << endl;
			else cout << "copied class names file: " << cmd << endl;
		}
	}
	// create gnuplot param file in jobs' root
	try {
		if (rmconf.exists("meta_gnuplot_params")) {
			std::string params = rmconf.get_string("meta_gnuplot_params");
			std::string gpp;
			gpp << rmconf.get_output_dir() << "/" << "gnuplot_params.txt";
			ofstream of(gpp.c_str());
			if (!of) {
				cerr << "warning: failed to write gnuplot parameters to ";
				cerr << gpp << endl;
			} else {
				of << params;
				of.close();
			}
		}
	} eblcatchwarn();
	mconf.pretty_combinations();
	cout << "Creating " << jobs.size() << " different combinations in "
			 << rmconf.get_output_dir() << endl;
	// write job directories and files
	for (uint i = 0; i < jobs.size(); ++i) {
		cout << i << " / " << jobs.size() << ": creating " << jobs[i]->name()
				 << endl;
		// write conf
		jobs[i]->write(reset_progress);
		// copy bins into jobs' root
		if (copy_path.size()) {
			cout << "Copying " << copy_path << " to " << jobs[i]->get_root() << endl;
			cmd = "";
			cmd << "cp -R " << copy_path << " " << jobs[i]->get_root();
			if (std::system(cmd.c_str()))
				cerr << "warning: failed to execute: " << cmd << endl;
		}
	}
}

void job_manager::initialize_other(const std::string &other_directory) {
	// find weights in other directory corresponding to each current job
	cout << "Initializing weights from other job directory "
			 << other_directory << endl;
	for (uint i = 0; i < jobs.size(); ++i) {
		cout << i << " / " << jobs.size() << ": looking for equivalent of "
				 << jobs[i]->shortname() << ": ";
		std::string pattern;
		pattern << ".*" << jobs[i]->shortname() << ".*/.*"
						<< jobs[i]->shortname() << ".*[.]mat";
		std::list<std::string> *l = find_fullfiles(other_directory, pattern.c_str(), NULL,
																							 true, true, false, false, true);
		if (!l || l->size() == 0) {
			cout << "no equivalent found." << endl;
			if (l) delete l;
			continue ;
		}
		std::string dir = ebl::dirname(l->back().c_str());
		jobs[i]->figure_resume_from_weights(dir.c_str());
		if (l) delete l;
	}
}

void job_manager::run(bool force_start) {
	time.start();
	// loop until all jobs are finished
	uint n = 0;
	while (nalive || unfinished > 0) {
		jobs_info();
		release_dead_children();
		// sort jobs based on their progress
		vector<job*> sjobs = jobs;
		std::sort(sjobs.begin(), sjobs.end(), job_progress_cmp);
		// if there are jobs waiting to be started, start them if possible
		if (unfinished > 0 && ready_slots > 0) {
			for (vector<job*>::iterator i = sjobs.begin();
					 i != sjobs.end() && ready_slots > 0; ++i) {
				// check status of job i from files
				(*i)->check_finished();
				if (n == 0 && force_start) (*i)->reset_started();
				else (*i)->check_started();
				(*i)->check_running();
				// run if not finished, not alive/running and slots are available
				if (!(*i)->finished() && !(*i)->running()
						&& !(*i)->alive() && ready_slots > 0) {
					(*i)->run();
					ready_slots--;
				}
			}
		}
		report();
		secsleep(swait);
		n++;
	}
	last_report();
}

void job_manager::monitor(const char *fname) {
	cout << "Initializing monitoring from " << fname << endl;
	time.start();
	mconf_fullfname = fname;
	mconf_fname = basename(mconf_fullfname.c_str());
	// read meta configuration
	if (!mconf.read(mconf_fullfname.c_str()))
		eblerror("failed to load configuration from " << mconf_fullfname);
	mconf.resolve();
	std::string odir = dirname(fname);
	mconf.set_output_dir(odir);
	// establish list of jobs given conf's root
	std::list<std::string> *l = list_job_dirs(mconf_fullfname.c_str());
	if (!l || l->size() == 0) eblerror("no job directory found");
	// create corresponding jobs
	uint k = 0;
	for (std::list<std::string>::iterator li = l->begin();
			 li != l->end(); ++li, ++k) {
		std::string c = *li;
		std::string j = basename(c.c_str());
		mconf.set_name(j);
		jobs.push_back(new job(mconf, mconf_fname.c_str(), false));
	}
	cout << "Created " << jobs.size() << " jobs, start monitoring." << endl;
	delete l;
	// monitor jobs status
	while (1) {
		jobs_info();
		secsleep(swait);
	}
}

////////////////////////////////////////////////////////////////
// job manager: protected methods

void job_manager::release_dead_children() {
#ifndef __WINDOWS__
	int status = 0;
	waitpid(-1, &status, WNOHANG); // check children status
#else
	eblerror("not implemented");
#endif
}

void job_manager::jobs_info() {
	nalive = 0;
	nrunning = 0;
	unstarted = 0;
	ready_slots = max_jobs;
	finished = 0;
	unfinished = jobs.size();
	mintime = 0.0;
	maxtime = 0.0;
	infos.str("");
	summary.str("");
	summary << setfill(' ');
	int total_progress = 0;
	uint jwidth = 1 + (int) floor(log10((float)jobs.size()));
	uint k = 0;
	for (uint i = 0; i < jobs.size(); ++i) {
		job &j = *(jobs[i]);
		// check status of job i from files
		if (j.check_finished())
			finished++;
		if (j.check_running() && !j.finished())
			nrunning++;
		j.check_started();
		if (j.alive()) {
			nalive++;
			if (ready_slots > 0)
				ready_slots--;
		}
		if (!j.started() && !j.finished())
			unstarted++;
		maxtime = std::max(j.minutes(), maxtime);
		if (mintime == 0)
			mintime = j.minutes();
		else
			mintime = MIN(j.minutes(), mintime);
		// gather jobs infos for reporting
		infos << i << ". pid: " << j.getpid() << ", name: " << j.name()
					<< ", status: " << (j.alive() ? "alive" : "dead")
					<< ", minutes: " << j.minutes() << endl;
		// summary info
		if (!j.finished()) {
			summary << setw(jwidth) << i << ":";
			if (j.running())
				summary << "R";
			else
				summary << " ";
			if (j.started()) {
				summary << setw(2) << j.check_progress() << "%";
			} else // not started ever
				summary << setw(2) << " .";
			summary << " | ";
			k++;
			total_progress += j.progress();
			if (k % 10 == 0)
				summary << endl;
		}
	}
	total_progress += finished * 100;
	unfinished = jobs.size() - finished;
	cout << "Unfinished: " << endl << summary.str() << endl;
	cout << "Jobs progress: " << total_progress / (float) jobs.size()
			 << "%, finished: " << finished << " / " << jobs.size()
			 << ", unfinished: " << unfinished
			 << ", alive: " << nalive
			 << ", running: " << nrunning
			 << ", unstarted: " << unstarted
			 << ", empty job slots: " << ready_slots << ", Iteration: "
			 << maxiter << ", elapsed: " << time.elapsed() << ", ETA: "
			 << time.eta(total_progress, jobs.size() * 100) << endl;
}

void job_manager::report() {
	// analyze outputs if requested
	if (rmconf.exists_bool("meta_analyze")) {
		maxiter_tmp = parser.get_max_common_iter(rmconf, rmconf.get_output_dir());
		if (maxiter_tmp != maxiter) { // iteration number has changed
			maxiter = maxiter_tmp;
			if (rmconf.exists_bool("meta_send_email")) {
				// send reports at certain iterations
				if (rmconf.exists("meta_email_iters")) {
					// loop over set of iterations
					std::list<uint> l =
						string_to_uintlist(rmconf.get_string("meta_email_iters"));
					for (std::list<uint>::iterator i = l.begin(); i != l.end(); ++i) {
						if (*i == (uint) maxiter) {
							cout << "Reached iteration " << *i << endl;
							// analyze
							best = parser.analyze(rmconf, rmconf.get_output_dir(),
																		maxiter_tmp, besteach);
							// send report
							parser.send_report(rmconf, rmconf.get_output_dir(), best, maxiter,
																 mconf_fullfname, infos.str(), nalive,
																 maxtime, mintime, &besteach);
						}
					}
				} else if (rmconf.exists("meta_email_period") &&
									 (maxiter % rmconf.get_uint("meta_email_period") == 0)) {
					// analyze
					best = parser.analyze(rmconf, rmconf.get_output_dir(),
																maxiter_tmp, besteach);
					// send report
					parser.send_report(rmconf, rmconf.get_output_dir(), best, maxiter,
														 mconf_fullfname, infos.str(), nalive,
														 maxtime, mintime, &besteach);
				}
			}
		}
	}
}

void job_manager::last_report() {
	cout << "All processes are finished. Exiting." << endl;
	// email last results before exiting
	// parse output and get best results
	best = parser.analyze(rmconf, rmconf.get_output_dir(),
												maxiter_tmp, besteach, true);
	// send report
	parser.send_report(rmconf, rmconf.get_output_dir(), best, maxiter,
										 mconf_fullfname, infos.str(), nalive,
										 maxtime, mintime, &besteach);
}

std::list<std::string> *job_manager::list_job_dirs(const char *conffname) {
	// list all directories that start with same dir name + _conf
	std::string dir = dirname(conffname);
	std::string patt = ".*";// = ebl::basename(dir.c_str());
	dir << "/";
	patt << "_conf[0-9]*_.*";
	cout << "List jobs based on directory " << dir
			 << " and pattern " << patt << endl;
	// find dirs non recursively
	std::list<std::string> *l = find_fullfiles(dir, patt.c_str(),
																						 NULL, true, false, false, true);
	if (!l)
		eblerror("failed to find existing job directories to resume in "
						 << dir << " with pattern: " << patt);
	cout << "Found " << l->size() << " matching directories." << endl;
	return l;
}

} // namespace ebl
