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

// tell header that we are in the libidx scope
#define LIBIDX

#include "utils.h"

#ifndef __NOSTL__
#include <sstream>
#include <iostream>
#include <iomanip>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#ifdef __WINDOWS__
#include <Windows.h>
#include <direct.h>
#include <sys/utime.h>
#else
#include <utime.h>
#endif

#define STRING_BUFFER (int)4096	/* string operations buffer size */

namespace ebl {

INIT_TIMING()

ubyte color_list[COLOR_LIST_SIZE][3] = {
  {0,0,255}, // blue
  {69,91,255}, // sun blue
  {0,255,255}, // light blue
  {0,255,0}, // green
  {255,0,0}, // red
  {255,255,255}, // white
  {255,0,255}, // pink
  {255,255,0}, // yellow
  {255,128,0}, // orange
  {0,128,255}, // medium blue
  {128,0,255}, // blue/pink
  {0,128,128}, // blue/green
  {128,128,128} // gray
};

char string_buffer[STRING_BUFFER];

////////////////////////////////////////////////////////////////
// file IO utilities

//! Scan an int.
int fscan_int(FILE *fp) {
  int res = 0;
  if (fscanf(fp, "%d", &res) < 1)
    eblthrow("cannot read integer from stream");
  return res;
}

//! Scan a float.
float fscan_float(FILE *fp) {
  float res = 0;
  if (fscanf(fp, "%f", &res) < 1)
    eblthrow("cannot read float from stream");
  return res;
}

//! Scan a single char.
char fscan_char(FILE *fp) {
  char res = 0;
  if (fscanf(fp, "%c", &res) < 1)
    eblthrow("cannot read float from stream");
  return res;
}

//! Scan a word and return it. The caller is responsible for deleting
//! returned string.
char *fscan_str(FILE *fp) {
  char buffer[1024], *res = NULL;
  int s;
  s = fscanf(fp,"%s",buffer);
  if ( s < 1)
    buffer[0]=0;
  try {
    res = new char[strlen(buffer) + 1];
    strcpy(res, buffer);
  } eblcatcherror();
  return res;
}

////////////////////////////////////////////////////////////////
// directory utilities

#define FILELEN 512

bool mkdir_full(std::string &dir) {
  return mkdir_full(dir.c_str());
}

bool mkdir_full(const char *dir) {
  if (dir_exists(dir))
    return true;
#ifdef __WINDOWS__
  // recursive directory creation, check if parent exists first
  std::string parent = dirname(dir);
  if (!dir_exists(parent.c_str()))
    mkdir_full(parent.c_str());
  // parents are created or already exist, make current directory.
  if (!_mkdir(dir))
    return true;
  return false;
#else
  // recursive directory creation, check if parent exists first
  std::string parent = dirname(dir);
  if (!dir_exists(parent.c_str()))
    mkdir_full(parent.c_str());
  // parents are created or already exist, make current directory.
  if (mkdir(dir, 0700) < 0)
    return false;
  return true;

  //     std::string cmd = "mkdir -p ";
  //     cmd += dir;
  //     int ret = system(cmd.c_str());
  //     if (ret != 0) {
  //       cerr << "warning: (error " << ret
  // 	   << ") failed to create directory " << dir;
  //       return false;
  //     }
  //     eblprint("mkdir ret: " << ret << std::endl);
  //     return true;
#endif
}

bool dir_exists(const char *s) {
#ifndef __WINDOWS__
  struct stat buf;
  if (stat(s,&buf)==0)
    if (buf.st_mode & S_IFDIR)
      return true;
#else /* WINDOWS */
  char *last;
  char buffer[FILELEN];
  struct _stat buf;
  if ((s[0]=='/' || s[0]=='\\') && (s[1]=='/' || s[1]=='\\') && !s[2])
    return true;
  if (strlen(s) > sizeof(buffer) - 4) eblerror("Filename too long");
  strcpy(buffer,s);
  last = buffer + strlen(buffer) - 1;
  if (*last=='/' || *last=='\\' || *last==':')
    strcat(last,".");
  if (_stat(buffer,&buf)==0)
    if (buf.st_mode & S_IFDIR)
      return true;
#endif
  return false;
}

bool dir_exists(const std::string &s) {
  return dir_exists(s.c_str());
}

bool file_exists(const char *s) {
#ifndef __WINDOWS__
  struct stat buf;
  if (stat(s, &buf) == -1)
    return false;
  if (buf.st_mode & S_IFREG)
    return true;
#else /* WINDOWS */
  struct _stat buf;
  if (_stat(s, &buf) == -1)
    return false;
  if (buf.st_mode & S_IFREG)
    return true;
#endif
  return false;
}

bool file_exists(const std::string &s) {
  return file_exists(s.c_str());
}

uint file_size(const char *s) {
#ifndef __WINDOWS__
  struct stat buf;
  if (stat(s, &buf) == -1)
    return 0; // file not found
#else /* WINDOWS */
  struct _stat buf;
  if (_stat(s, &buf) == -1)
    return 0; // file not found
#endif
  return buf.st_size;
}

uint file_size(const std::string &s) {
  return file_size(s.c_str());
}

time_t file_modified(const char *s) {
#ifndef __WINDOWS__
  struct stat buf;
  if (stat(s, &buf) == -1)
    return 0; // file not found
#else /* WINDOWS */
  struct _stat buf;
  if (_stat(s, &buf) == -1)
    return 0; // file not found
#endif
  return buf.st_mtime;
}

time_t file_modified(const std::string &s) {
  return file_size(s.c_str());
}

int file_modified_elapsed(const char *s) {
  time_t tm = file_modified(s);
  time_t t = time(NULL);
  return (int) (t - tm);
}

bool touch_file(const char *s) {
#ifndef __WINDOWS__
  if (utime(s, NULL) == 0)
    return true;
#else /* WINDOWS */
  eblerror("not implemented");
#endif
  return false;
}

bool touch_file(const std::string &s) {
  return touch_file(s.c_str());
}

bool rm_file(const std::string &s) {
  return rm_file(s.c_str());
}

bool rm_file(const char *s) {
#ifdef __WINDOWS__
  if (dir_exists(s))
    return _rmdir(s) == 0;
  else
    return _unlink(s) == 0;
#else
  if (dir_exists(s))
    return rmdir(s) == 0;
  else
    return unlink(s) == 0;
#endif
}

bool mv_file(const char *src, const char *tgt) {
  if (rename(src, tgt))
    return false;
  return true;
}

//     std::string dirname(const char *s_) {
// #ifdef __LINUX__
//     char c = '/';
// #else /* __WINDOWS__ */
//     char c = '\\';
// #endif
//     std::string s = s_;
//     size_t pos = s.find_last_of(c);
//     // if there is no dirname, return local dirname .
//     if (pos == string::npos) {
//       s = ".";
//       s += c;
//       return s;
//     }
//     return s.substr(0, pos);
//   }

const char *dirname(const char *fname) {
#ifndef __WINDOWS__ // UNIX and MAC
  const char *s = fname;
  const char *p = 0;
  char *q = string_buffer;
  char *q0 = q;
  while (*s) {
    if (s[0]=='/' && s[1]) p = s;
    s++;
  }
  if (!p) {
    if (fname[0]=='/') return fname;
    else return ".";
  }
  s = fname;
  if (p-s > STRING_BUFFER-1) eblerror("filename is too long: " << fname);
  do *q++ = *s++;
  while (s<p);
  *q = 0;
  // remove trailing /
  while (*(--q) == '/' && q >= q0) *q = 0;
  return string_buffer;
#else // WINDOWS
  const char *s, *p;
  char *q = string_buffer;
  /* Handle leading drive specifier */
  if (fname[0] && fname[1]==':') {
    *q++ = *fname++;
    *q++ = *fname++;
  }
  /* Search last non terminal / or \ */
  p = 0;
  s = (char*)fname;
  while (*s) {
    if (s[0]=='\\' || s[0]=='/')
      if (s[1] && s[1]!='/' && s[1]!='\\')
        p = s;
    s++;
  }
  /* Cannot find non terminal / or \ */
  if (p == 0) {
    if (q>string_buffer) {
      if (fname[0]==0 || fname[0]=='/' || fname[0]=='\\')
        return "\\\\";
      *q = 0;
      return string_buffer;
    } else {
      if (fname[0]=='/' || fname[0]=='\\')
        return "\\\\";
      else
        return ".";
    }
  }
  /* Single leading slash */
  if (p == fname) {
    strcpy(q,"\\");
    return string_buffer;
  }
  /* Backtrack all slashes */
  while (p>fname && (p[-1]=='/' || p[-1]=='\\'))
    p--;
  /* Multiple leading slashes */
  if (p == fname)
    return "\\\\";
  /* Regular case */
  s = fname;
  if (p-s > STRING_BUFFER-4)
    eblerror("filename is too long: " << fname);
  do {
    *q++ = *s++;
  } while (s<p);
  *q = 0;
  return string_buffer;
#endif
}

static char *strcpyif(char *d, const char *s) {
  if (d != s)
    return strcpy(d,s);
  return d;
}

const char *basename(const char *fname, const char *suffix) {
#ifndef __WINDOWS__ // LINUX and MAC
  int sl;
  char *s;
  if (strlen(fname) > STRING_BUFFER-4)
    eblerror("filename is too long: " << fname);
  s = (char *) strrchr(fname,'/');
  if (s) fname = s+1;
  /* Process suffix */
  if (suffix==0 || suffix[0]==0)
    return fname;
  if (suffix[0]=='.')
    suffix += 1;
  if (suffix[0]==0)
    return fname;
  strcpyif(string_buffer,fname);
  sl = strlen(suffix);
  s = string_buffer + strlen(string_buffer);
  if (s > string_buffer + sl) {
    s =  s - (sl + 1);
    if (s[0]=='.' && strcmp(s+1,suffix)==0)
      *s = 0;
  }
  return string_buffer;
#else // WINDOWS
  int sl;
  char *p = (char*)fname;
  char *s = (char*)fname;
  /* Special cases */
  if (fname[0] && fname[1]==':') {
    strcpyif(string_buffer,fname);
    if (fname[2]==0)
      return string_buffer;
    string_buffer[2] = '\\';
    if (fname[3]==0 && (fname[2]=='/' || fname[2]=='\\'))
      return string_buffer;
  }
  /* Position p after last slash */
  while (*s) {
    if (s[0]=='\\' || s[0]=='/')
      p = s + 1;
    s++;
  }
  /* Copy into buffer */
  if (strlen(p) > STRING_BUFFER-10)
    eblerror("filename is too long: " << fname);
  s = string_buffer;
  while (*p && *p!='/' && *p!='\\')
    *s++ = *p++;
  *s = 0;
  /* Process suffix */
  if (suffix==0 || suffix[0]==0)
    return string_buffer;
  if (suffix[0]=='.')
    suffix += 1;
  if (suffix[0]==0)
    return string_buffer;
  sl = strlen(suffix);
  if (s > string_buffer + sl) {
    s = s - (sl + 1);
    if (s[0]=='.' && stricmp(s+1,suffix)==0)
      *s = 0;
  }
  return string_buffer;
#endif
}

std::string noext_name(const char *fname) {
  std::string res = fname;
  size_t pos = res.find_last_of('.');
  res = res.substr(0, pos);
  return res;
}
std::string ext_name(const char *fname) {
  std::string res = fname;
  size_t pos = res.find_last_of('.');
  res = res.substr(pos, res.size() - pos);
  return res;
}

////////////////////////////////////////////////////////////////
// timing utilities

std::string tstamp() {
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [80];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  strftime (buffer,80,"%Y%m%d.%H%M%S",timeinfo);
  std::string s = buffer;
  return s;
}

//   std::string tstamp() {
//     static time_t t = time(NULL);
//     static struct tm *lt = localtime(&t);
// #ifdef __NOSTL__
//     std::string ts;
//     ts << lt->tm_year + 1900 << lt->tm_mon << lt->tm_mday
//        << "." << lt->tm_hour << lt->tm_min << lt->tm_sec;
//     return ts;
// #else
//     ostringstream ts;
//     ts << setw(2) << setfill('0') << lt->tm_year + 1900
//        << setw(2) << setfill('0') << lt->tm_mon
//        << setw(2) << setfill('0') << lt->tm_mday
//        << "."
//        << setw(2) << setfill('0') << lt->tm_hour
//        << setw(2) << setfill('0') << lt->tm_min
//        << setw(2) << setfill('0') << lt->tm_sec;
//     return ts.str();
// #endif
//   }

timer::timer() : total_micros(0), last_micros(0), running(false) {
  start();
}

timer::~timer() {
}

void timer::start() {
  running = true;
#ifdef __WINDOWS__
  // TODO
#else // linux & mac
  gettimeofday(&t0, NULL);
#endif
}

void timer::stop() {
  last_micros = elapsed_microseconds();
  total_micros += last_micros;
  running = false;
}

void timer::restart() {
  running = true;
#ifdef __WINDOWS__
  // TODO
#else // linux & mac
  gettimeofday(&t0, NULL);
#endif
  reset();
}

void timer::reset() {
  total_micros = 0;
  last_micros = 0;
}

double timer::elapsed_minutes() {
  if (!running) return last_micros / 60000000;
#ifdef __WINDOWS__
  return 0; // TODO
#else // linux & mac
  gettimeofday(&t1, NULL);
  return (t1.tv_sec - t0.tv_sec) / (double) 60;
#endif
}

long timer::elapsed_seconds() {
  if (!running) return last_micros / 1000000;
#ifdef __WINDOWS__
  return 0; // TODO
#else // linux & mac
  gettimeofday(&t1, NULL);
  return t1.tv_sec - t0.tv_sec;
#endif
}

long timer::elapsed_milliseconds() {
  if (!running) return last_micros / 1000;
#ifdef __WINDOWS__
  return 0; // TODO
#else // linux & mac
  gettimeofday(&t1, NULL);
  // if running return accumulated + time since last start
  return (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_usec - t0.tv_usec) / 1000;
#endif
}

long timer::elapsed_microseconds() {
  if (!running) return last_micros;
#ifdef __WINDOWS__
  return 0; // TODO
#else // linux & mac
  gettimeofday(&t1, NULL);
  // if running return accumulated + time since last start
  return (t1.tv_sec - t0.tv_sec) * 1000000 + (t1.tv_usec - t0.tv_usec);
#endif
}

long timer::accumulated_milliseconds() {
  return (total_micros + elapsed_microseconds()) / 1000;
}

void timer::pretty_elapsed() {
  pretty_secs(elapsed_seconds());
}

std::string timer::elapsed() {
  return elapsed(elapsed_seconds());
}

std::string timer::elapsed_ms() {
  return elapsed_ms(elapsed_milliseconds());
}

std::string timer::accumulated_ms() {
  return elapsed_ms(accumulated_milliseconds());
}

void timer::pretty_secs(long seconds) {
  eblprint( elapsed(seconds));
}

// second equivalences
#define SECMIN 60
#define SECHOUR 3600
#define SECDAY 86400
#define SECWEEK 604800
#define SECMONTH 18144000

std::string timer::elapsed(long seconds) {
  std::string sout;
  long div, mod;
  bool pretty = false;
  div = seconds / SECMONTH; mod = seconds % SECMONTH;
  if (div > 0 || pretty) {
    sout << (int) div << "m ";
    pretty = true;
  }
  div = mod / SECWEEK; mod = mod % SECWEEK;
  if (div > 0 || pretty) {
    sout << (int) div << "w ";
    pretty = true;
  }
  div = mod / SECDAY; mod = mod % SECDAY;
  if (div > 0 || pretty) {
    sout << (int) div << "d ";
    pretty = true;
  }
  div = mod / SECHOUR; mod = mod % SECHOUR;
  if (div > 0 || pretty) {
    sout << (int) div << "h ";
    pretty = true;
  }
  div = mod / SECMIN; mod = mod % SECMIN;
  if (div > 0 || pretty) {
    sout << (int) div << "m ";
  }
  sout << (int) mod << "s";
  return sout;
}

// second equivalences
#define MSMIN 60000
#define MSHOUR 3600000
#define MSDAY 86400000
#define MSWEEK 604800000
#define MSMONTH 18144000000ULL

std::string timer::elapsed_ms(long milliseconds) {
  std::string sout;
  long div, mod;
  bool pretty = false;
  div = milliseconds / MSMONTH; mod = milliseconds % MSMONTH;
  if (div > 0 || pretty) {
    sout << (int) div << "m ";
    pretty = true;
  }
  div = mod / MSWEEK; mod = mod % MSWEEK;
  if (div > 0 || pretty) {
    sout << (int) div << "w ";
    pretty = true;
  }
  div = mod / MSDAY; mod = mod % MSDAY;
  if (div > 0 || pretty) {
    sout << (int) div << "d ";
    pretty = true;
  }
  div = mod / MSHOUR; mod = mod % MSHOUR;
  if (div > 0 || pretty) {
    sout << (int) div << "h ";
    pretty = true;
  }
  div = mod / MSMIN; mod = mod % MSMIN;
  if (div > 0 || pretty) {
    sout << (int) div << "m ";
    pretty = true;
  }
  div = mod / 1000; mod = mod % 1000;
  if (div > 0 || pretty) {
    sout << (int) div << "s ";
  }
  sout << (int) mod << "ms";
  return sout;
}

void millisleep(long millis) {
#ifdef __WINDOWS__
  Sleep(millis);
#else
  usleep(millis * 1000);
#endif
}

void secsleep(long seconds) {
#ifdef __WINDOWS__
  Sleep(seconds * 1000);
#else
  usleep(seconds * 1000000);
#endif
}

std::string timer::eta(uint n, uint total) {
#ifdef __WINDOWS__
	return elapsed((long)((total - n)
                        * (elapsed_seconds() / (float)max((uint)1,n))));
#else
  return elapsed((long)((total - n)
                        * (elapsed_seconds() / (float)std::max((uint)1,n))));
#endif
}

// process utilities ///////////////////////////////////////////////////////////

#ifndef __WINDOWS__
#include <sys/types.h>
#include <unistd.h>
#endif

int pid() {
#ifdef __WINDOWS__
  eblwarn( "pid() not implemented for Windows" << std::endl);
  return 0;
#else
  return (int) getpid();
#endif
}

} // namespace ebl
