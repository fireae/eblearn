/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef JOB_H_
#define JOB_H_

#include "configuration.h"
#include "metaparser.h"
#include "utils.h"

#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#ifndef __WINDOWS__
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // job

  //! Jobs to be executed.
  class EXPORT job {
  public:
    //! Create a job given a configuration conf.
    //! \param conf The job's configuration.
    //! \param oconffname The original configuration's filename.
    //! \param resume If true, find latest saved parameters and start
    //!        from them.
    job(configuration &conf, const char *oconffname = NULL,
				bool resume = false);
    //! Destructor.
    virtual ~job();

    //! Execute job (fork and call run_child()).
    virtual void run();

    //! Returns true if the job has been started, false otherwise.
    virtual bool started();

    //! Write job's files in configuration's output directory.
    //! \param reset_progress If true, ignore existing progress in target dir.
    virtual bool write(bool reset_progress = false);

    //! Return true if the job is alive and running.
    //! This differs from running() in that it indicates if the job
    //! is being run by us.
    virtual bool alive();

    //! Return true if the job is running.
    //! This differs from alive() in that it indicates if the job
    //! is being run by anybody else or us.
    virtual bool running();
    //! Return pid of this job.
    virtual int getpid();
    //! Return the name of this job (its configuration filename).
    virtual std::string &name();
    //! Return the short name of this job.
    virtual std::string shortname();

    //! Return root directory of this job.
    virtual std::string get_root();
    //! Return job's running time in minutes.
    virtual double minutes();
    //! Force the start flag to be on. This is be useful to mpi job manager.
    virtual void force_started();
    //! Force the start flag to be off. This is be useful to mpi job manager.
    virtual void reset_started();
    //! Return elapsed running time string.
    virtual std::string elapsed();
    //! Returns true if this job is being resumed from before.
    virtual bool resumed();
    //! Returns true if this job is finished, false otherwise.
    virtual bool finished();
    //! Returns the progress percentage or -1 if not started.
    //! This value is updated after a check_progress() call only.
    virtual int progress() const;

    ////////////////////////////////////////////////////////////////
    // file-based resuming capabilities

    //! Create an empty file named 'progress' in the job's directory,
    //! to show that this job is being processed.
    //! Returns false if writing the file failed.
    virtual bool declare_started();
    //! Create an empty file named 'finished' in the job's directory,
    //! to show that this job was finished.
    //! Returns false if writing the file failed.
    virtual bool declare_finished();
    //! Returns true if a file named 'progress' exists in the job's directory.
    virtual bool check_started();
    //! Returns true if job's progress file was modified less than an hour ago.
    //! This also updates the value returned by running() method.
    //! This information is different than alive(), in the sense that
    //! this job could be run by somebody else or ourselves.
    virtual bool check_running();
    //! Returns the percentage progress of the job by looking at the
    //! 'progress' file in the job's directory.
    //! If progress does not exist, returns -1.
    //! If progress exists and empty, returns 0.
    //! If 'retrain_iteration' and 'iterations' are defined, return
    //! retrain_iteration / iterations * 100 as percentage.
    virtual int check_progress();
    //! Returns true if a file named 'finished' exists in the job's directory.
    virtual bool check_finished();
    //! Write the file 'progress' in current directory or in root directory
    //! if specified, and write 'i' and 'total'
    //! variables to describre progress.
    //! \param additional Additional string the user may want to put in progress
    //!   such as resuming information.
    static void write_progress(uint i, uint total,
															 const char *additional = NULL,
															 const char *root = NULL);
    //! Returns the filename where progress info is written by write_progress().
    static std::string get_progress_filename(const char *root = NULL);
    //! Write the file 'finished' in current directory or in root directory
    //! if specified. If 'progress' file already exists, move it to 'finished'
    //! (to avoid having too many files, otherwise, just touch file 'finished'.
    static void write_finished(const char *root = NULL);
    //! Figure out resume parameters from existing weights files.
    //! \param dir If null, use job's output directory.
    virtual void figure_resume_from_weights(const char *dir = NULL);

  protected:
    //! Execute child's code.
    virtual void run_child();
    //! Figure out resume parameters from 'progress' file.
    //! If no info found in progress file, this tries
    //! figure_resume_from_weights().
    virtual void figure_resume_out();

    ////////////////////////////////////////////////////////////////
    // members
  protected:
    configuration	conf; //!< Job's configuration.
    configuration	rconf; //!< Resolved job's configuration.
    std::string		exe;	//!< executable full path
    std::string		outdir;	//!< job's output directory
    std::string		confname;	//!< job's configuration filename
    std::string		oconffname_;	//!< job's original conf filename
    timer               t;
    bool                _locally_started; //!< This job is/was started.
    bool                _started; //!< This job is/was started.
    bool                _running; //!< This job is running right now
    bool                _alive; //!< This job is run by us right now
    int 		pid;	//!< pid of this job
    bool                resumed_; //!< This job is being resumed or not.
    bool                _finished; //!< This is is finished or not.
    int                 _progress; //!< Progress percentage.
    std::string         progress_fname; //!< Filename of progress file.
    std::string         finished_fname; //!< Filename of finished file.
  };

  //! Job comparison definintion based on their progress.
  EXPORT bool job_progress_cmp(const job *a, const job *b);

  ////////////////////////////////////////////////////////////////
  // job manager

  //! A class to manage jobs.
  class EXPORT job_manager {
  public:
    //! Constructor.
    job_manager();

    //! Destructor.
    virtual ~job_manager();

    //! Read meta configuration.
    //! @param tstamp An optional timestamp to be used for the job's name
    //!               instead of the current timestamp.
    //! \param resume_name If not null, resume the existing job with this name.
    //! \param nomax If true, lift the maximum concurrent jobs limit.
    //! \param resumedif If true, resume by reading existing job dirs, use global
    //!                  conf file otherwise.
    //! \param maxjobs Limit the number of concurrent jobs to maxjobs. If -1,
    //!                defaut, use conf's 'meta_max_jobs' instead if defined.
    virtual bool read_metaconf(const char *fname,
															 const std::string *tstamp = NULL,
															 const char *resume_name = NULL,
															 bool resumedir = false,
															 bool nomax = false, int maxjobs = -1,
															 const std::string *extra = NULL);
    //! Enable recursive copy of this path into jobs folders.
    virtual void set_copy(const std::string &path);
    //! Prepare all jobs (create folders and copy/create files).
    //! \param reset_progress If true, ignore existing progress in target dir.
    virtual void prepare(bool reset_progress = false);
    //! Set the weights initialization for each job given corresponding jobs
    //! found in 'other' directory.
    virtual void initialize_other(const std::string &other);
    //! Run all jobs (assumes a call to prepare() beforehand).
    virtual void run(bool force_start = false);
    //! Monitor progress of jobs given existing directories and
    //! progress files ('progress' and 'finished').
    virtual void monitor(const char *conffname);

  protected:

    //! Release child processes that have terminated from their zombie state.
    virtual void release_dead_children();
    //! Gather and print information about jobs, such as number of jobs running,
    //! number to run left, min/max running time.
    virtual void jobs_info();
    //! Analyze and send a report.
    virtual void report();
    //! Print stopping message and send last report.
    virtual void last_report();
    //! List all job directories found in conf's root directory and
    //! return that list. Caller is responsible for delete the list.
    virtual std::list<std::string> *list_job_dirs(const char *conffname);

    ////////////////////////////////////////////////////////////////
    // members
  protected:
    meta_configuration	mconf;	   //!< (unresolved) Meta configuration
    meta_configuration	rmconf;	   //!< (resolved) Meta configuration
    std::string		mconf_fullfname;//!< Full filename of metaconf
    std::string		mconf_fname;	//!< Filename of metaconf
    std::vector<job*>	jobs;	   //!< A vector of jobs to run
    std::string         copy_path; //!< Copy path to jobs folders.
    int                 max_jobs;  //!< Max number of jobs at the same time.
    metaparser          parser;
    int			maxiter;
    int			maxiter_tmp;
    double		mintime;
    double		maxtime;
    uint		nalive;
    uint		nrunning;
    uint		unstarted;
    uint                finished;
    uint                unfinished;
    int		        ready_slots;
    std::ostringstream  infos;
    std::ostringstream  summary; //!< Jobs status summary.
    varmaplist          best; //!< best results
    varmaplist          besteach; //!< best result of each job
    uint                swait; //!< Waiting time when looping, in seconds.
    timer               time; //!< Total running time.
  };

} // end namespace ebl

#endif /* JOB_H_ */
