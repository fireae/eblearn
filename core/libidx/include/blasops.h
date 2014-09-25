/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
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

#ifndef BLASOPS_H
#define BLASOPS_H

#include "config.h"
#include "numerics.h"
#include "idx.h"

#ifdef __CBLAS__

namespace ebl {

  // idx_copy //////////////////////////////////////////////////////////////////

  template <> EXPORT void idx_copy(const idx<double> &src, idx<double> &dst);
  template <> EXPORT void idx_copy(const idx<float> &src, idx<float> &dst);

  // idx_dot ///////////////////////////////////////////////////////////////////

  //! dot product of two idx. Returns sum of product of all elements.
  EXPORT float64 idx_dot(idx<double> &i1, idx<double> &i2);
#if defined(__CBLAS__) || (defined(__IPP__) && defined(__IPP_DOT__))
  //! dot product of two idx. Returns sum of product of all elements.
  EXPORT float64 idx_dot(idx<float32> &i1, idx<float32> &i2);
#endif

  // m2dotm1 ///////////////////////////////////////////////////////////////////

  //! matrix-vector multiplication y <- a.x
  EXPORT void idx_m2dotm1(idx<double> &a, idx<double> &x, idx<double> &y);
  //! matrix-vector multiplication y <- a.x
  EXPORT void idx_m2dotm1(idx<float> &a, idx<float> &x, idx<float> &y);
  //! matrix-vector multiplication y <- y + a.x
  EXPORT void idx_m2dotm1acc(idx<double> &a, idx<double> &x, idx<double> &y);
  //! matrix-vector multiplication y <- y + a.x
  EXPORT void idx_m2dotm1acc(idx<float> &a, idx<float> &x, idx<float> &y);

  // m4dotm2 ///////////////////////////////////////////////////////////////////
  
  //! vector-vector outer product a <- x.y'
  EXPORT void idx_m1extm1(idx<double> &a, idx<double> &x, idx<double> &y);
  //! vector-vector outer product a <- x.y'
  EXPORT void idx_m1extm1(idx<float> &a, idx<float> &x, idx<float> &y);
  //! vector-vector outer product a <- a + x.y'
  EXPORT void idx_m1extm1acc(idx<double> &a, idx<double> &x, idx<double> &y);
  //! vector-vector outer product a <- a + x.y'
  EXPORT void idx_m1extm1acc(idx<float> &a, idx<float> &x, idx<float> &y);

  // norm_columns //////////////////////////////////////////////////////////////

  //! normalize the colums of a matrix
  EXPORT void norm_columns(idx<double> &m);
  //! normalize the colums of a matrix, float version
  EXPORT void norm_columns(idx<float> &m);
  
} // end namespace ebl

#endif /* __CBLAS__ */

#endif /* BLASOPS_H */
