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

// tell header that we are in the libidx scope
#define LIBIDX

#ifdef __TH__
#include "config.h"
#include "idx.h"
#include "idxops.h"
#include "thops.h"
#include "th.h"
#include "idxops_th.hpp"

namespace ebl {

  // idx_add (not-in-place) ////////////////////////////////////////////////////

  idx_add_macro(float32)
  idx_add_macro(float64)

  // idx_add (in-place) ////////////////////////////////////////////////////////

  idx_add_macro_in_place(float32)
  idx_add_macro_in_place(float64)
  
  // idx_copy ///////////////////////////////////////////
  idx_copy_macro(float32)
  idx_copy_macro(float64)

  // idx_tanh
  idx_tanh_macro(float32)
  idx_tanh_macro(float64)
  //idx_dtanh_macro(float32)
  //idx_dtanh_macro(float64)

  idx_power_macro(float32)
  idx_power_macro(float64)
  idx_power_macro2(float32, int)
  idx_power_macro2(float32, intg)
  idx_power_macro2(float32, float64)

  idx_power_macro2(float64, int)
  idx_power_macro2(float64, intg)
  idx_power_macro2(float64, float32)

  
} // end namespace ebl

#endif /* __TH __ */
