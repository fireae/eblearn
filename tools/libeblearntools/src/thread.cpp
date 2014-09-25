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

#include "thread.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // mutex

  //! Use non-recursive mutex so that lock can be called multiple times
  //! by the same thread but require only 1 unlock.
  //! Also this has to be the 'fast' kind of mutex that blocks the
  //! thread until locking/unlocking is successful.
  mutex::mutex() {
#ifdef __PTHREAD__
    pthread_mutex_init(&m, NULL);
#else
    eblerror("pthread missing, install it and recompile.");
#endif
    }

    mutex::~mutex() {
#ifdef __PTHREAD__
      pthread_mutex_destroy(&m);
#endif
    }

    bool mutex::trylock() {
#ifdef __PTHREAD__
      return (pthread_mutex_trylock(&m) == 0);
#endif
      return false;
    }

    void mutex::lock() {
#ifdef __PTHREAD__
      pthread_mutex_lock(&m);
#endif
    }

    void mutex::unlock() {
#ifdef __PTHREAD__
      pthread_mutex_unlock(&m);
#endif
    }

  ////////////////////////////////////////////////////////////////
  // sbuf

  sbuf::sbuf(std::ostream &o, mutex *m, const char *p)
    : out(o), new_line(true), own_lock(false), busy(m), prefix(p) {
  }

  sbuf::~sbuf() {
  }

  int sbuf::sync() {
    if (own_lock) {
      own_lock = false;
      out << str();
      str("");
      out.flush();
      new_line = true;
      if (busy)
	busy->unlock();
    } else { // we don't own the lock, wait for it to unlock
      if (busy)
	busy->lock();
      out << str();
      str("");
      out.flush();
      new_line = true;
      if (busy)
	busy->unlock();
    }
    return 0;
  }

  std::streamsize sbuf::xsputn (const char * s, std::streamsize n) {
    if (!own_lock && busy)
      busy->lock();
    own_lock = true; // own lock until we output endl
    if (prefix && new_line)
      out << prefix << ": ";
    new_line = false;
    return std::stringbuf::xsputn(s, n);
   }

  ////////////////////////////////////////////////////////////////
  // mutex_ostream

  mutex_ostream::mutex_ostream(std::ostream &o, mutex *m, const char *p)
    : std::ostream(&buffer), buffer(o, m, p) {
  }

  mutex_ostream::~mutex_ostream() {
  }

  void mutex_ostream::update_prefix(const char *p) {
    buffer.prefix = p;
  }

  ////////////////////////////////////////////////////////////////
  // thread
  // Initialize it to satisfy the linker (correct behavior)
  uint thread::_id_counter=0;

  thread::thread(mutex *outmutex, const char *name_, bool sync)
    : _stop(false), mutout(std::cout, outmutex, _name.c_str()),
      muterr(std::cerr, outmutex, _name.c_str()),
      mout(sync ? mutout : std::cout), merr(sync ? muterr : std::cerr),
      _finished(false), _id(_id_counter++) {
    if (name_ == NULL) _name << "Thread " << _id;
    else _name = name_;
    // update name in mutex outs
    mutout.update_prefix(_name.c_str());
    muterr.update_prefix(_name.c_str());
    mout << "initializing thread (" << (sync ? "synched" : "unsynched" )
	 << " outputs)" << std::endl;
  }

  thread::~thread() {
    //      pthread_exit((void*)&threadptr);
    //mout << "destroyed." << std::endl;
  }

  int thread::start() {
#ifndef __PTHREAD__
    eblerror("pthread missing, install it and recompile");
    return -1;
#else
    return pthread_create(&threadptr, NULL, thread::entrypoint, this);
#endif
  }

  void thread::run() {
    execute();
  }

  std::string& thread::name() {
    return _name;
  }

  std::ostream& thread::get_mout() {
    return mout;
  }

  std::ostream& thread::get_merr() {
    return merr;
  }

  void thread::stop(long wait_seconds) {
    ask_stop(); // make sure the thread knows it should stop
    if (wait_seconds > 0) { // wait for thread to stop by itself
      timer wait;
      wait.start();
      while (!_finished && wait.elapsed_seconds() < wait_seconds)
	millisleep(5);
    }
    // force stopping thread
#ifndef __PTHREAD__
    eblerror("pthread missing, install it and recompile.");
#else
#if !defined(__ANDROID__)
    // Android's bionic rewrite doesn't have pthread_cancel
    int ret = pthread_cancel(threadptr);
    if (ret) {
      merr << "Warning: failed to cancel thread, with error code " << ret
	   << std::endl;
      return ;
    }
#endif // __ANDROID__
#endif // __PTHREAD__
    mutex1.lock();
    _finished = true;
    mutex1.unlock();
  }

  void thread::ask_stop() {
    if (!_stop) {
      mutex1.lock();
      _stop = true;
      mout << "required to stop." << std::endl;
      mutex1.unlock();
    }
  }

  bool thread::finished() {
    return _finished;
  }

  /*static */
  void* thread::entrypoint(void * pthis) {
    thread *pt = (thread*) pthis;
    pt->run();
    pt->_finished = true;
    //pt->mout << "finished" << std::endl;
    return pt;
  }

} // end namespace ebl
