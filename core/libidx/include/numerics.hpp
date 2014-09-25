/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef NUMERICS_HPP_
#define NUMERICS_HPP_

namespace ebl {

////////////////////////////////////////////////////////////////

template <typename T>
T limits<T>::max() {
#ifdef __NOSTL__
  eblerror("limits::max not implemented for this type");
#else
  return (std::numeric_limits<T>::max)();
#endif
}

template <typename T>
T limits<T>::min() {
#ifdef __NOSTL__
  eblerror("limits::min not implemented for this type");
#else
  return (std::numeric_limits<T>::min)();
#endif
}

template <typename T> template <typename T2>
T saturator<T>::saturate (T2 in) {
  if (in > limits<T>::max()) return limits<T>::max();
  else if (in < limits<T>::min()) return limits<T>::min();
  else return (T)in;
}

template <typename T>
T angle_distance(T a1, T a2) {
  double d = (double) a1 - (double) a2;
  double fd = std::fabs(d);
  if (fd < TWOPI - fd) return (T) d;
  else {
    if (d < 0) return (T) (TWOPI - fd);
    else return (T) (fd - TWOPI);
  }
}

#define saturate(in, T) (saturator<T>::saturate(in))

} // end namespace ebl

#endif /* NUMERICS_HPP_ */
