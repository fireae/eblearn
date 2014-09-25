/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
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

#ifndef UTILS_H_
#define UTILS_H_

#ifdef __WINDOWS__
#include <time.h>
#else // linux & mac
#include <sys/time.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include "defines.h"
#include "stl.h"

namespace ebl {

#define COLOR_LIST_SIZE 13
//! A pre-defined color list
extern IDXEXPORT ubyte color_list[COLOR_LIST_SIZE][3];

// file IO utilities /////////////////////////////////////////////////////////

//! Scan an int.
EXPORT int fscan_int(FILE *fp);
//! Scan a float.
EXPORT float fscan_float(FILE *fp);
//! Scan a single char.
EXPORT char fscan_char(FILE *fp);
//! Scan a word and return it. The caller is responsible for deleting
//! returned string.
EXPORT char *fscan_str(FILE *fp);

// directory utilities ///////////////////////////////////////////////////////

//! Create specified directory and all its parents if they do not exists.
//! Return false upon failure.
EXPORT bool mkdir_full(const char *dir);
//! Create specified directory and all its parents if they do not exists.
//! Return false upon failure.
EXPORT bool mkdir_full(std::string &dir);
//! Returns true if directory 's' exists.
EXPORT bool dir_exists(const char *s);
//! Returns true if directory 's' exists.
EXPORT bool dir_exists(const std::string &s);
//! Returns true if file 's' exists.
EXPORT bool file_exists(const char *s);
//! Returns true if file 's' exists.
EXPORT bool file_exists(const std::string &s);
//! Returns size of file 's'. If the file does not exist, return 0.
EXPORT uint file_size(const char *s);
//! Returns size of file 's'. If the file does not exist, return 0.
EXPORT uint file_size(const std::string &s);
//! Returns the time of last modification of file 's'.
EXPORT time_t file_modified(const char *s);
//! Returns the time of last modification of file 's'.
EXPORT time_t file_modified(const std::string &s);
//! Returns the number of seconds elapsed since file 's'
//! was last modified.
EXPORT int file_modified_elapsed(const char *s);
//! Update the modified timestamp of file 's'. Returns true on success.
EXPORT bool touch_file(const char *s);
//! Update the modified timestamp of file 's'. Returns true on success.
EXPORT bool touch_file(const std::string &s);
//! Remove the file or empty directory 's' and return true on success.
EXPORT bool rm_file(const char *s);
//! Remove the file or empty directory 's' and return true on success.
EXPORT bool rm_file(const std::string &s);
//! Move file 'src' to 'tgt' and return true on success.
EXPORT bool mv_file(const char *src, const char *tgt);
//! Returns the directory part of 'fname'. E.g. when fname equals
//! '/usr/lib', it will return '/usr'.
EXPORT const char *dirname(const char *fname);
//! Returns the file part of 'fname'. E.g. when fname equals
//! '/usr/lib', it will return 'lib'.
//! If suffix is provided, the suffix 'suffix' is removed from the
//! filename.
EXPORT const char *basename(const char *fname, const char *suffix = NULL);
//! Returns a name without its extension, basically returns the
//! first part before the last '.' character.
EXPORT std::string noext_name(const char *fname);
//! Returns the extension of a name, basically returns the
//! second part before the last '.' character.
EXPORT std::string ext_name(const char *fname);

// timing utilities //////////////////////////////////////////////////////////

//! Return a string containing a timestamp of localtime in the following
//! format: "<year><month><day>.<hour><minutes><seconds>".
EXPORT std::string tstamp();

//! A timer class.
class EXPORT timer {
public:
  timer();
  virtual ~timer();
  //! Start timer.
  void start();
  //! Stop timer.
  void stop();
  //! Restart timer (and call reset()).
  void restart();
  //! Reset the accumulation of milliseconds.
  void reset();
  //! Return elapsed time in minutes since start() or restart().
  double elapsed_minutes();
  //! Return elapsed time in seconds since start() or restart().
  long elapsed_seconds();
  //! Return elapsed time in milliseconds since start() or restart().
  long elapsed_milliseconds();
  //! Return elapsed time in microseconds since start() or restart().
  long elapsed_microseconds();

  //! Return all accumulated milliseconds since last reset()/restart().
  long accumulated_milliseconds();

  //! Print elapsed time in a human-friendly way since start() or restart().
  void pretty_elapsed();
  //! Print the provided seconds in a human-friendly way.
  void pretty_secs(long seconds);
  //! Return seconds in a human-friendly string.
  std::string elapsed(long seconds);
  //! Return elapsed time in a human-friendly string since start()/restart().
  std::string elapsed();
  //! Return seconds in a human-friendly string,
  //! with millisecond precision
  std::string elapsed_ms(long milliseconds);
  //! Return elapsed time in a human-friendly string since start()/restart(),
  //! with millisecond precision.
  std::string elapsed_ms();
  //! Return elapsed time in a human-friendly string since start()/restart(),
  //! with millisecond precision.
  std::string accumulated_ms();
  //! Return a string with the human-friendly remaining time based on
  //! elapsed time and n/total progress.
  std::string eta(uint n, uint total);

private:
#ifdef __WINDOWS__
#else // linux & mac
  struct timeval t0, t1;
#endif
  long total_micros; //! Total accumulated micros since last reset.
  long last_micros; //! Last elapsed microseconds after stop().
  bool running; //! Timer is running or not.
};

//! Sleep for 'millis' milliseconds.
EXPORT void millisleep(long millis);
//! Sleep for 'seconds' seconds.
EXPORT void secsleep(long seconds);

// process utilities ///////////////////////////////////////////////////////////

//! Return PID of current process.
EXPORT int pid();

// TIMING MACROS ///////////////////////////////////////////////////////////////

#ifdef __TIMING__

extern EXPORT ebl::timer debug_timer1;
extern EXPORT ebl::timer debug_timer2;
extern EXPORT ebl::timer debug_timer_resizing; // timing of resizing
extern EXPORT uint debug_resizing_cnt; // # of resizing

#define INIT_TIMING()                           \
  ebl::timer debug_timer1;                      \
  ebl::timer debug_timer2;                      \
  ebl::timer debug_timer_resizing;              \
  uint debug_resizing_cnt;

#define TIMING(s, t) {                                          \
    eblprint( s << ": " << t.elapsed_ms() << std::endl);	\
    t.restart();                                                \
  }
#define TIMINGACC(s, t) {                                       \
    eblprint( s << ": " << t.accumulated_ms() << std::endl);	\
    t.restart();                                                \
  }
#define TIMING_ACCSTART(t) t.start();
#define TIMING_ACCSTOP(t) t.stop();
#define TIMING_RESET(t) t.reset();
#define LOCAL_TIMING_START() ebl::timer local_debug_timer;
#define LOCAL_TIMING2_START() ebl::timer local_debug_timer2;
#define LOCAL_TIMING_REPORT(s) TIMING(s, local_debug_timer)
#define LOCAL_TIMING2_REPORT(s) TIMING(s, local_debug_timer2)

#define TIMING1(s) TIMING(s, debug_timer1)
#define TIMING2(s) TIMING(s, debug_timer2)
#define TIMING_RESIZING_ACCSTART()		\
  TIMING_ACCSTART(debug_timer_resizing);	\
  debug_resizing_cnt++;
#define TIMING_RESIZING_ACCSTOP() TIMING_ACCSTOP(debug_timer_resizing)
#define TIMING_RESIZING_RESET()                 \
  TIMING_RESET(debug_timer_resizing);           \
  debug_resizing_cnt = 0;
#define TIMING_RESIZING(s)						\
  TIMINGACC(s, debug_timer_resizing);					\
  eblprint( s << ": number of resizings: " << debug_resizing_cnt <<std::endl);

#else
#define INIT_TIMING()
#define TIMING(s, t)
#define TIMING_START_ACC(t)
#define TIMING_STOP_ACC(t)
#define TIMING_RESET(t)
#define TIMING_START()
#define TIMING_REPORT()
#define TIMING1(s)
#define TIMING2(s)
#define TIMING_RESIZING_ACCSTART()
#define TIMING_RESIZING_ACCSTOP()
#define TIMING_RESIZING_RESET()
#define TIMING_RESIZING(s)
#define LOCAL_TIMING_START()
#define LOCAL_TIMING2_START()
#define LOCAL_TIMING_REPORT(s)
#define LOCAL_TIMING2_REPORT(s)

#endif

} // end namespace ebl

#include "utils.hpp"

#endif /* UTILS_ */
