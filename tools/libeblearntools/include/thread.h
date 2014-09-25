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

#ifndef THREAD_H_
#define THREAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "utils.h"

#ifdef __PTHREAD__
#include <pthread.h>
#endif

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // mutex

  //! A mutex abstraction class.
  class EXPORT mutex {
  public:
    mutex();
    ~mutex();

    //! Try to lock the mutex but do not wait until mutex is available.
    //! This returns true if mutex was available and successfully locked,
    //! false otherwise.
    bool trylock();

    //! Lock the mutex (wait if necessary until mutex is available).
    void lock();

    //! Unlock the mutex.
    void unlock();

  protected:
#ifdef __PTHREAD__
    //! Use non-recursive mutex so that lock can be called multiple times
    //! by the same thread but require only 1 unlock.
    //! Also this has to be the 'fast' kind of mutex that blocks the
    //! thread until locking/unlocking is successful.
    pthread_mutex_t m;
#endif
  };

  ////////////////////////////////////////////////////////////////
  // sbuf

  class mutex_ostream;

  // Specialize a stringbuf to handle flushing to unblock mutex.
  class EXPORT sbuf : public std::stringbuf {
  public:
    //! Output to ostream o but prevent other
    //! \param m The mutex shared by all threads used to synchronize.
    //! \param prefix If not null, prefix is printed before each new line.
    sbuf(std::ostream &o, mutex *m = NULL, const char *prefix = NULL);
    ~sbuf();

    // When we sync the stream with the output.
    // 1) Output prefix then the buffer
    // 2) Reset the buffer
    // 3) flush the actual output stream we are using.
    virtual int sync();
    //! Put sequence of characters in buffer.
    virtual std::streamsize xsputn(const char *s, std::streamsize n);

    friend class mutex_ostream;

  protected:
    std::ostream& out;
    bool new_line; //! Indicate if we are starting new line or not.
    bool own_lock; //! True if mutex was locked by this instance.
    mutex *busy; //! Mutex prevents simultaneous output.
    const char *prefix;
  };

  ////////////////////////////////////////////////////////////////
  // mutex_ostream

  //! An ostream class that contains a mutex, preventing different threads
  //! from writing before current thread finishes outputting a line.
  class EXPORT mutex_ostream : public std::ostream {
  public:
    //! Output to ostream o but prevent other
    //! \param m The mutex shared by all threads used to synchronize.
    //! \param prefix If not null, prefix is printed before each new line.
    mutex_ostream(std::ostream &o, mutex *m = NULL, const char *prefix = NULL);
    ~mutex_ostream();
    // update buffer's prefix string
    void update_prefix(const char *p);

  protected:
    sbuf buffer;
  };

  ////////////////////////////////////////////////////////////////
  // thread

  //! A thread abstraction class
  class EXPORT thread {
  public:
    //! \param outmutex A mutex used to synchronize threads outputs/
    //!   To synchronize all threads, give the same mutex to each of them.
    //! \param sync If true, synchronize outputs between threads, using
    //!    outmutex, otherwise use regular unsynced outputs.
    thread(mutex *outmutex = NULL, const char *name = "Thread",
	   bool sync = true);
    virtual ~thread();
    //! Start the thread.
    int start();
    //! Stop the thread. If wait_seconds is zero, stop the thread right now,
    //! otherwise wait until the thread has finished its current task or force
    //! it to exit if it's not finished after wait_seconds seconds.
    void stop(long wait_seconds = 0);
    //! Tell the thread it should stop. The thread might be busy finishing
    //! some task, so this only tells it to quit when it is finished.
    //! One can check if the thread is finished with the finished() method.
    void ask_stop();
    //! Return true if thread has finished executing.
    bool finished();
    //! Return name of this thread.
    std::string& name();
    //! Return a reference this thread's output stream.
    std::ostream& get_mout();
    //! Return a reference this thread's output error stream.
    std::ostream& get_merr();

  protected:
    void run();
    static void* entrypoint(void *pthis);
    virtual void execute() = 0;

  protected:
    bool 		_stop;
    std::string      	_name;  //! Name of this thread.
    mutex_ostream       mutout; // synchronized cout
    mutex_ostream       muterr; // synchronized cerr
    std::ostream       &mout; // may contained synced or standard output
    std::ostream       &merr; // may contained synced or standard err output
  private:
#ifdef __PTHREAD__
    pthread_t 		threadptr;
#endif
    mutex 	        mutex1;
    bool 		_finished;
  protected:
    uint                _id;
  private:
    static uint         _id_counter;
  };

} // end namespace ebl

#endif /* THREAD_H_ */
