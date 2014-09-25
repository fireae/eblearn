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
#include <stdio.h>
#include <map>
#include <string.h>

#include "job.h"
#include "mpijob.h"
#include "tools_utils.h"
#include "eblapp.h"

// global variables ////////////////////////////////////////////////////////////

string 	copy_path				= ""; 	// path to copy to target folder
string 	manual_tstamp		= ""; 	// tstamp string prefix to the experiment name
bool		resume					= false;				// resume metajob or not
bool		resumedir				= false;				// resume metajob or not
bool		resume_other		= false;				// resume from metajob or not
string  other_directory = "";
bool		create					= false;				// create metajob directories only
bool    monitor					= false;				// monitor jobs from existing directories
int     maxjobs					= -1;   // max concurrent number of jobs
bool    reset_progress	= false;				// ignore existing progress in target dir
bool    force_start			= false;				// ignore timeout wait if true.
string  extra           = ""; // extra configuration

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  // if requesting help, print usage
  if ((strcmp(argv[1], "-help") == 0) ||
      (strcmp(argv[1], "-h") == 0))
    return false;
  // loop over arguments
  for (int i = 2; i < argc; ++i) {
    if (strcmp(argv[i], "-copy") == 0) {
      ++i;
      if (i >= argc) {
				cerr << "input error: expecting string after -copy." << endl;
				return false;
      }
      copy_path = argv[i];
    } else if (strcmp(argv[i], "-tstamp") == 0) {
      ++i;
      if (i >= argc) {
				cerr << "input error: expecting string after -tstamp." << endl;
				return false;
      }
      manual_tstamp = argv[i];
    } else if (strcmp(argv[i], "-maxjobs") == 0) {
      ++i;
      if (i >= argc) {
				cerr << "input error: expecting number after -maxjobs." << endl;
				return false;
      }
      maxjobs = string_to_int(argv[i]);
    } else if (strcmp(argv[i], "-resumedir") == 0) {
      resumedir = true;
      resume = true;
    } else if (strcmp(argv[i], "-resume") == 0) {
      resume = true;
    } else if (strcmp(argv[i], "-resume_other") == 0) {
      resume_other = true;
      other_directory = argv[++i];
    } else if (strcmp(argv[i], "-monitor") == 0) {
      monitor = true;
    } else if (strcmp(argv[i], "-create") == 0) {
      create = true;
    } else if (strcmp(argv[i], "-reset_progress") == 0) {
      reset_progress = true;
    } else if (strcmp(argv[i], "-force_start") == 0) {
      force_start = true;
    } else if (strcmp(argv[i], "-extra") == 0) {
      ++i;
      if (i >= argc) {
				cerr << "input error: expecting string after -extra." << endl;
				return false;
      }
      extra << argv[i] << "\n";
    } else {
      cerr << "input error: unknown parameter: " << argv[i] << endl;
      return false;
    }
  }
  return true;
}

// print command line usage
void print_usage() {
  cout << "Usage: ./metarun <config_file> [OPTIONS]" << endl;
  cout << "Options are:" << endl;
  cout << "  -copy <path>" << endl;
  cout << "      Copy the specified path recursively to each job's directory."
       << endl;
  cout << "  -tstamp <timestamp>" << endl
       << "      Manually set the timestamp prefix for the experiment's name."
       << endl;
  cout << "  -maxjobs <n>" << endl
       << "      Limits the concurrent number of jobs." << endl;
  cout << "  -monitor" 
       << "      Monitor jobs progress from the given configuration file" << endl
       << "      and its working directory." << endl;
  cout << "  -resume" 
       << "      Resume an already created job from the given " << endl
       << "      configuration file." << endl;
  cout << "  -resumedir" 
       << "      Resume an already created job from the directory " << endl
       << "      of the given configuration file." << endl;
  cout << "  -resume_other <other job directory>" 
       << "      Start a new job directory but initialize each job from last "
       << "      weights of another job" << endl;
  cout << "  -create" << endl
       << "      Only create the directory structure and other files in " 
       << endl
       << "      preparation for running all possible configurations." << endl
       << "      Jobs can subsequently be started with -resume" << endl;
  cout << "  -reset_progress" << endl
       << "      Remove previous progress and finished files." << endl;
  cout << "  -force_start" << endl
       << "      Ignore timeout wait and starts all jobs." << endl;
  cout << "  -extra <string>" << endl
       << "      Adds extra variables at end of configuration." << endl
			 << "      example: -extra \"a=1\" -extra \"b=2\"" << endl;
}

int main(int argc, char **argv) {
  cout << "________________________________Meta Run";
  cout << "________________________________" << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  for (int i = 0; i < argc; ++i)
    cout << "arg " << i << ": " << argv[i] << endl;
  string metaconf = argv[1];
  job_manager *jm = NULL;
#ifdef __MPI__
  mpijob_manager *mjm = new mpijob_manager(argc, argv);
  if (mjm->world_size() == 1) { // we are not in a MPI environment
    cout << "Switching to non-mpi execution." << endl;
    jm = new job_manager();
    delete mjm;
  } else { // use MPI env
    if (mjm->is_master()) {
      cout << "Found MPI world of size " << mjm->world_size()
					 << ", executing in MPI mode." << endl;
    }
    jm = mjm;
  }
#else
  jm = new job_manager();
#endif
  string resume_name;
  if (resume) { // extract metajob name to be resumed
    string dir = dirname(metaconf.c_str());
    resume_name = ebl::basename(dir.c_str());
    resumedir = true;
    cout << "Configuration file: " << metaconf << endl;
    cout << "Directory: " << dir << endl;
    cout << "Resuming metajob with name: " << resume_name << endl;
  }
  if (monitor) // just monitor progress in job directories
    jm->monitor(metaconf.c_str());
  else { 
    jm->read_metaconf(metaconf.c_str(), &manual_tstamp, 
											(resume_name.empty() ? NULL : resume_name.c_str()), 
											resumedir, false, maxjobs, &extra);
    jm->set_copy(copy_path);
    if (resume_other) jm->initialize_other(other_directory);
    // jm->prepare(reset_progress);    
    if (!resume) jm->prepare(reset_progress);
    if (create) // do not run if creating structure only
      cout << "Not running jobs, only creating directories and files." << endl;
    else {
      cout << "Running..." << endl;
      jm->run(force_start);
    }
  }
  delete jm;
  return 0;
}
