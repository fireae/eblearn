/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet   *
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

#ifndef MPIJOB_H_
#define MPIJOB_H_

#ifdef __MPI__
#include <mpi.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/mpi.hpp>
#endif

#include "job.h"

namespace ebl {

  //! Jobs to be executed in a MPI system.
  class EXPORT mpijob : public job {
  public:
    //! Create a job given a configuration conf.
    //! \param conf The job's configuration.
    //! \param oconffname The original configuration's filename.
    //! \param resume If true, find latest saved parameters and start
    //!        from them.
    mpijob(configuration &conf, const char *oconffname = NULL,
	   bool resume = false);
    //! Destructor.
    virtual ~mpijob();

    //! Execute job (fork and call run_child()).
    virtual void run();

  protected:

    //! Execute child's code.
    virtual void run_child();

    ////////////////////////////////////////////////////////////////
    // members
  protected:
  };

  ////////////////////////////////////////////////////////////////
  // mpijob manager

  //! A class to manage jobs, that can be ran over a cluster using MPI.
  class EXPORT mpijob_manager : public job_manager {
  public:
    //! Constructor. argc and argv are used by mpi to initialize and
    //! manager communications.
    mpijob_manager(int argc, char **argv);
    //! Destructor.
    virtual ~mpijob_manager();
    //! Read meta configuration.
    //! @param tstamp An optional timestamp to be used for the job's name
    //!               instead of the current timestamp.
    //! \param resume_name If not null, resume the existing job with this name.
    //! \param resumedir If true, resume by reading existing job dirs, use global
    //!                  conf file otherwise.
    virtual bool read_metaconf(const char *fname,
                               const std::string *tstamp = NULL,
															 const char *resume_name = NULL,
															 bool resumedir = false,
															 bool nomax = false, int maxjobs = -1,
															 const std::string *extra = NULL);
    //! Prepare all jobs (create folders and copy/create files).
    virtual void prepare(bool reset_progress = false);
    //! Run all jobs (assumes a call to prepare() beforehand).
    virtual void run(bool force_start = false);
    //! Return the size of the world (including master).
    virtual uint world_size();
    //! Returns true if this instance is the master.
    virtual bool is_master();

  protected:
    //! Run the master manager, which creates all initial files and
    //! configuration and controls jobs assignments to slaves managers.
    virtual void run_master();
    //! Stop all slaves.
    virtual void stop_all();
    //! Assign job with id 'jobid' to slave with rank 'slave_id' and
    //! return true on success.
    virtual bool assign(uint jobid, uint slave_id);
    //! Gather and print information about jobs, such as number of jobs running,
    //! number to run left, min/max running time.
    void jinfos(int running[]);
    //! Run a slave manager, which takes orders for the master manager.
    virtual void run_slave();

    // members /////////////////////////////////////////////////////////////////
  protected:
#ifdef __MPI__
    boost::mpi::communicator world;
    boost::mpi::environment env;
#endif
    int rank;
    int jslots;
    int id_running; //!< id of running job (-1 if none).
    bool use_master; //!< Use master to run a job or not.
  };

} // end namespace ebl

#endif /* MPIJOB_H_ */
