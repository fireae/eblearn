/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef NUMERICS_H
#define NUMERICS_H

#include "defines.h"
#include "stl.h"

#ifndef __NOSTL__
#include <limits>
#endif

//#ifdef __ANDROID__
//#include <math.h>
//#include <float.h>
//#else
#include <cfloat>
#include <cmath>
//#endif

#ifdef __WINDOWS__
#define isinf(a) (!_finite(a))
#endif

#define TWOPI						6.283185308
#define PI							3.141592654
#define PI_OVER2				1.570796327

namespace ebl {

  // tanh ///////////////////////////////////////////////////////////////////////

  //! derivative of tanh
  EXPORT double dtanh(double x);

  //! The cotangent inverse function.
  EXPORT double arccot(double x);

  // stdsigmoid /////////////////////////////////////////////////////////////////

  //! "standard" sigmoid, used in Lush.
  //! Rational polynomial for computing y = 1.71593428*tanh(0.66666666*x)
  EXPORT float stdsigmoid(float x);
  //! derivative of standard digmoid.
  EXPORT float dstdsigmoid(float x);

  //! "standard" sigmoid, used in Lush.
  //! Rational polynomial for computing y = 1.71593428*tanh(0.66666666*x)
  EXPORT double stdsigmoid(double x);
  //! derivative of standard digmoid.
  EXPORT double dstdsigmoid(double x);

  // randomization //////////////////////////////////////////////////////////////

  //! flag used to know if dseed has been already called
  //! This flag is raised only when init_rand is called, not
  //! when dseed is called.
  //! At the moment only the forget functions use this flag (see ebm)
#ifdef LIBIDX // declared from inside this library
  extern EXPORT bool drand_ini;
#else // declared from outside
  extern IMPORT bool drand_ini;
#endif

  //! initializes drand by calling dseed, and raises drand_ini
  EXPORT void init_drand(int x);
  //! Initializes drand by calling dseed with a random seed taken
  //! from current time and the sum of all arguments characters,
  //! if present.
  //! This can be useful if several programs are called at the same
  //! time with different input arguments.
  //! Returns the random seed used.
  EXPORT int dynamic_init_drand(int argc = 0, char **argv = NULL);
  //! Initializes drand by calling dseed with a fixed seed (0),
  //! and raises drand_ini. This returns the seed used (0).
  EXPORT int fixed_init_drand();
  //! sets the seed of the random number generator.
  //! This MUST be called at least once before
  //! the random number generator is used. Otherwise
  //! calls to drand() and dgauss() always return the
  //! same number.
  EXPORT void dseed(int x);
  //! random number generator. Return a random number
  //! drawn from a uniform distribution over [0,1].
  EXPORT double drand(void);
  //! random number generator. Return a random number
  //! drawn from a uniform distribution over [-v,+v].
  EXPORT double drand(double v);
  //! random number generator. Return a random number
  //! drawn from a uniform distribution over [v0,v1].
  EXPORT double drand(double v0, double v1);
  //! draw a random number from a quasi-Gaussian
  //! distribution with mean 0 and variance 1.
  EXPORT double dgauss(void);
  //! draw a random number from a quasi-Gaussian
  //! distribution with mean 0 and variance sigma.
  EXPORT double dgauss(double sigma);
  //! draw a random number from a quasi-Gaussian
  //! distribution with mean m and variance sigma.
  EXPORT double dgauss(double m, double sigma);
  //! Returns the gaussian of x, with a mean 'm' and a variance 'sigma'.
  EXPORT double gaussian(double x, double m, double sigma);
  //! n choose k (k must be <= n)
  EXPORT int choose(int n, int k);
  //! Returns the minimum distance between angles a1 and a2 (in radians).
  //! e.g. angle_distance(0, 1.5 * PI) -> .5 * PI.
  template <typename T> EXPORT T angle_distance(T a1, T a2);

  // limits ////////////////////////////////////////////////////////////////////

  template <typename T> class limits {
  public:
    static inline T max ();
    static inline T min ();
  };

  template <> class limits<uint32> {
  public:
    static inline uint32 max () { return UINT_MAX; }
    static inline uint32 min () { return 0; }
  };

  template <> class limits<float32> {
  public:
    static inline float32 max () { return FLT_MAX; }
    static inline float32 min () { return FLT_MIN; }
  };

  template <> class limits<float64> {
  public:
    static inline float64 max () { return DBL_MAX; }
    static inline float64 min () { return - DBL_MAX; }
  };

  template <> class limits<long double> {
  public:
    static inline long double max () { return LDBL_MAX; }
    static inline long double min () { return - LDBL_MAX; }
  };

  template <typename T> class saturator {
  public:
    template <typename T2> inline static T saturate (T2 in);
  };

#ifdef __WINDOWS__
  // Windows replacements for missing functions

  //! rint replacement for Windows.
  EXPORT double rint(double x);

#endif

} // end namespace ebl

#include "numerics.hpp"

#endif /* NUMERICS_H */
