/***************************************************************************
 *   Copyright (C) 2012 by Soumith Chintala *
 *   soumith@gmail.com *
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

#ifndef TH_H
#define TH_H

#include "config.h"
#include "numerics.h"
#include "idx.h"
#include "thops.h"

#ifdef __TH__

namespace ebl {

  // idx_add (not-in-place) ////////////////////////////////////////////////////

  //! add two idx's, specialized float32 version
  template<> void idx_add(idx<float32> &i1, idx<float32> &i2,
        		  idx<float32> &out);
  //! add two idx's, specialized float64 version
  template<> void idx_add(idx<float64> &i1, idx<float64> &i2,
        		  idx<float64> &out);

  // idx_add (in-place) ////////////////////////////////////////////////////////

  //! Add two idx's as follow: out = out + in, specialized float32 version
  template<> void idx_add(idx<float32> &in, idx<float32> &out);
  //! Add two idx's as follow: out = out + in, specialized float64 version
  template<> void idx_add(idx<float64> &in, idx<float64> &out);

  // idx_copy //////////////////////////////////////////////////////////////////

  //! copy, specialized float32 version
  template <> void idx_copy(const idx<float32> &src, idx<float32> &dst);
  //! copy, specialized float64 version
  template <> void idx_copy(const idx<float64> &src, idx<float64> &dst);

  // idx_tanh ////////////////////////////////////////////////////////////////
  //! hyperbolic tangent
    template <> void idx_tanh(idx<float32> &inp, idx<float32> &out);
    template <> void idx_tanh(idx<float64> &inp, idx<float64> &out);
  /* //! derivative of hyperbolic tangent */
  /* template <> void idx_dtanh(idx<float32> &inp, idx<float32> &out); */
  /* template <> void idx_dtanh(idx<float64> &inp, idx<float64> &out); */

  
} // end namespace ebl

#include "idxops_th.hpp"

#endif /* ifdef __TH__ */

#endif /* define TH_H */
