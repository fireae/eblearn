/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet *
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

#ifndef RANDOM_H
#define RANDOM_H

#include "defines.h"
#include <string>

namespace ebl {

// random //////////////////////////////////////////////////////////////////////

class EXPORT random {
public:
  //! Empty constructor, doesn't set any seed.
  random();
  //! Constructs a random number generator with seed 'x'.
  //! Use x == 0 to construct a fixed-seed generator.
  random(int x);
  //! Destructor.
  virtual ~random();

  //! Initialize random seed from current time.
  void time_seed();
  //! Initialize seed from current time and the sum of all characters of s.
  //! This can be useful if several programs are called at the
  //! same time with different input arguments.
  void time_string_seed(std::string &s);
  //! Initialize seed from current time and the sum of all arguments characters,
  //! if present. This can be useful if several programs are called at the
  //! same time with different input arguments.
  void time_args_seed(int argc, char **argv);

  //! random number generator. Return a random number
  //! drawn from a uniform distribution over [0,1].
  double drand(void);
  //! random number generator. Return a random number
  //! drawn from a uniform distribution over [-v,+v].
  double drand(double v);
  //! random number generator. Return a random number
  //! drawn from a uniform distribution over [v0,v1].
  double drand(double v0, double v1);

protected:
  //! Initialize seed.
  void init(int x);
  //! sets the seed of the random number generator.
  //! This MUST be called at least once before
  //! the random number generator is used. Otherwise
  //! calls to drand() and dgauss() always return the
  //! same number.
  void dseed(int x);

private:
  int inext, inextp;
  int ma[56];		/* Should not be modified */
};

} // end namespace ebl

#endif /* RANDOM_H */
