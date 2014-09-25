/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun   *
 *   yann@cs.nyu.edu   *
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

#ifndef LIBIDX_DEFINES_H_
#define LIBIDX_DEFINES_H_

//#ifdef __ANDROID__
//#define __NOSTL__
//#else
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
//#endif

#include "config.h"

#if !defined(__WINDOWS__) && !defined(__ANDROID__)
#include <execinfo.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <new>

#ifndef NULL
#define NULL (void*)0
#endif

#define MATRIX_EXTENSION ".mat"
#define MAT_PATTERN ".*[.](mat|MAT)"
#define IMAGE_PATTERN                                                   \
  ".*[.](png|jpg|jpeg|PNG|JPG|JPEG|bmp|BMP|ppm|PPM|pnm|PNM|pgm|PGM|gif|GIF)"
#define IMAGE_PATTERN_MAT                                               \
  ".*[.](png|jpg|jpeg|PNG|JPG|JPEG|bmp|BMP|ppm|PPM|pnm|PNM|pgm|PGM|gif|GIF|mat|MAT)"

// official names for dataset files
#define DATA_NAME "data"
#define LABELS_NAME "labels"
#define JITTERS_NAME "jitters"
#define CLASSES_NAME "classes"
#define SCALES_NAME "scales"
#define CLASSPAIRS_NAME "classpairs"
#define DEFORMPAIRS_NAME "deformpairs"

// low-level debug
#define DEBUG_LOW(s) //std::cout << s << std::endl;

// debug message
#if defined(DEBUG) || defined(__DEBUG__)
#define __DEBUG_PRINT__
#define EDEBUG(s) std::cout << s << std::endl;
#define EDEBUG_MAT(s, m)                                                \
  std::cout << s << " " << m << std::setprecision(20) << " min " << idx_min(m) \
  << " max " << idx_max(m) << " mean " << idx_mean(m) << " sum " << idx_sum(m) \
  << std::endl;
#define MEDEBUG(s) mout << s << std::endl;
#else
#define EDEBUG(s)
#define EDEBUG_MAT(s, m)
#define MEDEBUG(s)
#endif

////////////////////////////////////////////////////////////////
// library export macros

#ifdef __WINDOWS__
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
// disable dllexport warnings
#pragma warning(disable:4251)
#else
#define EXPORT
#define IMPORT
#endif

////////////////////////////////////////////////////////////////
// error reporting macros

#if defined(__ANDROID__) || defined(__NOEXCEPTIONS__)  // no exceptions
#define eblthrow(s) {                           \
    eblerror(s);                                \
  }
#else
#define eblthrow(s) {                           \
    std::string eblthrow_error;                 \
    eblthrow_error << s;                        \
    throw ebl::eblexception(eblthrow_error);    \
  }
#endif

#define eblcatch()                              \
  catch(ebl::eblexception &e) {                 \
    cerr << "exception: " << e << endl;         \
  }                                             \
  catch(std::string &e) {                       \
    cerr << "exception: " << e << endl;         \
  }

#ifdef __NOEXCEPTIONS__
#define eblcatcherror()                         \
  catch(ebl::eblexception &e) {                 \
    eblerror("exception");                      \
  }                                             \
  catch(std::string &e) {                       \
    eblerror("exception");                      \
  }                                             \
  catch(std::bad_alloc& ba) {                   \
    eblerror("bad_alloc");                      \
  }
#define eblcatchwarn()                          \
  catch(ebl::eblexception &e) {                 \
    eblwarn("exception");                       \
  }                                             \
  catch(std::string &e) {                       \
    eblwarn("exception");                       \
  }                                             \
  catch(std::bad_alloc& ba) {                   \
    eblwarn("bad_alloc");                       \
  }
#define eblcatchwarn_msg(s)                     \
  catch(ebl::eblexception &e) {                 \
    eblwarn(s);                                 \
  }                                             \
  catch(std::string &e) {                       \
    eblwarn(s);                                 \
  }                                             \
  catch(std::bad_alloc& ba) {                   \
    eblwarn(s << ": bad_alloc");                \
  }
#define eblcatchwarn_extra(cmd)                 \
  catch(ebl::eblexception &e) {                 \
    eblwarn("exception");                       \
    cmd                                         \
	}                                       \
  catch(std::string &e) {                       \
    eblwarn("exception");                       \
    cmd                                         \
	}                                       \
  catch(std::bad_alloc& ba) {                   \
    eblwarn("bad_alloc");                       \
    cmd                                         \
	}
#define eblcatcherror_msg(s)                    \
  catch(ebl::eblexception &e) {                 \
    eblerror(s);                                \
  }                                             \
  catch(std::string &e) {                       \
    eblerror(s);                                \
  }                                             \
  catch(std::bad_alloc& ba) {                   \
    eblerror("bad_alloc: " << s);               \
  }
#else
#define eblcatcherror()                         \
  catch(ebl::eblexception &e) {                 \
    eblerror(e);                                \
  }                                             \
  catch(std::string &e) {                       \
    eblerror(e);                                \
  }                                             \
  catch(std::bad_alloc& ba) {                   \
    eblerror("bad_alloc: " << ba.what());       \
  }
#define eblcatchwarn_extra(cmd)                 \
  catch(ebl::eblexception &e) {                 \
    eblwarn(e);                                 \
    cmd                                         \
	}                                       \
  catch(std::string &e) {                       \
    eblwarn(e);                                 \
    cmd                                         \
	}                                       \
  catch(std::bad_alloc& ba) {                   \
    eblwarn("bad_alloc: " << ba.what());        \
    cmd                                         \
	}
#define eblcatchwarn()                          \
  catch(ebl::eblexception &e) {                 \
    eblwarn(e);                                 \
  }                                             \
  catch(std::string &e) {                       \
    eblwarn(e);                                 \
  }                                             \
  catch(std::bad_alloc& ba) {                   \
    eblwarn("bad_alloc: " << ba.what());        \
  }
#define eblcatchwarn_msg(s)                             \
  catch(ebl::eblexception &e) {				\
    eblwarn(s << ": " << e);                            \
  }							\
  catch(std::string &e) {				\
    eblwarn(s << ": " << e);                            \
  }                                                     \
  catch(std::bad_alloc& ba) {                           \
    eblwarn(s << ": " << "bad_alloc: " << ba.what());   \
  }
#define eblcatcherror_msg(s)                    \
  catch(ebl::eblexception &e) {                 \
    eblerror(e << ": " << s);                   \
  }                                             \
  catch(std::string &e) {                       \
    eblerror(e << ": " << s);                   \
  }                                             \
  catch(std::bad_alloc& ba) {                   \
    eblerror("bad_alloc: " << ba.what()         \
             << ", " << s);                     \
  }
#endif

#define not_implemented() {			\
    eblerror("not implemented"); }

#if defined(__ANDROID__) ///////////////////////////////////////////////////

#include <android/log.h>

#define LOG_TAG    "eblearn"
#define LOGI(info) {                                                    \
    std::stringstream ANDs; ANDs << info;                               \
    std::string ANDs2 = ANDs.str();                                     \
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,ANDs2.c_str()); }
#define LOGE(info) {                                                    \
    std::stringstream ANDs; ANDs << info;                               \
    std::string ANDs2 = ANDs.str();                                     \
    __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,ANDs2.c_str()); }
#define eblerror(s) do {					\
    std::stringstream errvar;					\
    errvar << "Error: " << s;					\
    errvar << ", in " << __FUNCTION__ << " at " << __FILE__;	\
    errvar << ":" << __LINE__ << std::endl;			\
    std::string errstr = errvar.str();				\
    LOGE(errstr);						\
    abort();                                                    \
  } while(0)
// #define eblerror(s) do{LOGE(s); ebltrace(); abort();} while(0)
#define eblwarn(s) do{LOGE(s)} while(0)
#define eblprint(s) do {LOGI(s)} while(0)
#define eblprinto(o,s) do {LOGI(s)} while(0)

#elif defined(__WINDOWS__) ///////////////////////////////////////////////////

#define eblerror(s) do {                                           \
    std::cerr << "Error: " << s;                                \
    std::cerr << ", in " << __FUNCTION__ << " at " << __FILE__; \
    std::cerr << ":" << __LINE__ << std::endl;                  \
    abort();                                                    \
  } while(0)

#define ebltrace()

#else ///////////////////////////////////////////////////

#define ebltrace() {				\
    void *array[10];                            \
    size_t size;                                \
    size = backtrace(array, 10);                \
    backtrace_symbols_fd(array, size, 2);       \
  }

#define eblerror(s) do {                                           \
    std::cerr << "\033[1;31mError:\033[0m " << s;               \
    std::cerr << ", in " << __FUNCTION__ << " at " << __FILE__; \
    std::cerr << ":" << __LINE__ << std::endl;                  \
    std::cerr << "\033[1;31mStack:\033[0m" << std::endl;        \
    ebltrace();                                                 \
    abort();                                                    \
  } while (0)

#endif /* __WINDOWS__ */

#ifndef __ANDROID__
#define eblprint(s) do { std::cout << s; } while(0)
#define eblprinto(o, s) do { o << s; } while(0)
#define eblwarn(s) { std::cerr << "Warning: " << s << "." << std::endl; }
#endif
/* #ifndef MAX */
/* # define MAX(a, b) (((a) > (b)) ? (a) : (b)) */
/* #endif */
#ifndef MIN
# define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef unsigned int uint; // not defined on MAC

namespace ebl {

// intg is used for array indexing, hence should be
// defined as long if you want very large arrays
// on 64 bit machines.
typedef long intg;
#if __LP64__ == 1 //intg size is 64bits
#define SIZEOFINTG 64
#else //else, assume that intg size is 32bits
#define SIZEOFINTG 32
#endif
#ifdef __IPP__
#include <ipp.h>
typedef Ipp8u ubyte;
typedef Ipp8s byte;
typedef Ipp16u uint16;
typedef Ipp16s int16;
typedef Ipp32u uint32;
typedef Ipp32s int32;
typedef long int64;
typedef unsigned long uint64;
typedef Ipp32f float32;
typedef Ipp64f float64;
#else
typedef unsigned char ubyte;
typedef signed char byte;
typedef unsigned short int uint16;
typedef signed short int int16;
typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned long uint64;
typedef signed long int64;
typedef float float32;
typedef double float64;
#endif

// iterators
#define USING_FAST_ITERS 1

#ifdef LIBIDX // we are inside the library
#define IDXEXPORT EXPORT
#else // we are outside
#define IDXEXPORT IMPORT
#endif

} // end namespace ebl

#endif /* LIBIDX_DEFINES_H_ */
