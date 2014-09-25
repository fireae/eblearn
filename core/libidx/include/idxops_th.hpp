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

#ifndef IDXOPS_TH_HPP_
#define IDXOPS_TH_HPP_
#ifdef __TH__

////////////////////////////////////////////////////////////////////////
// idx_add
#define idx_add_macro_in_place(T)				\
  template<> void idx_add(idx<T> &in, idx<T> &out) {		\
    th_add(in, out);						\
  }

#define idx_add_macro(T)						\
  template<> void idx_add(idx<T> &in1, idx<T> & in2, idx<T> &out) {	\
    th_add(in1, in2, out);						\
  }

#define idx_copy_macro(T)						\
  template<> void idx_copy(const idx<T> &in, idx<T> &out) {             \
    idx_checknelems2_all(in, out);                                      \
    const idx<T>* const_in_ptr = &in;                                   \
    idx<T>* unconst_in_ptr = const_cast<idx<T>* > (const_in_ptr);       \
    th_copy(*unconst_in_ptr, out);                                      \
  }

#define idx_tanh_macro(T)						\
  template<> void idx_tanh(idx<T> &inp, idx<T> &out) {                  \
    th_tanh(inp, out);                                                  \
  }
#define idx_dtanh_macro(T)						\
  template<> void idx_dtanh(idx<T> &inp, idx<T> &out) {                 \
    th_dtanh(inp, out);                                                 \
  }

#define idx_power_macro(T)						\
  template<> void idx_power(idx<T> &in, T p, idx<T> &out) {             \
    idx_checknelems2_all(in, out);                                      \
    th_pow(in, out, p);                                                 \
  }

#define idx_power_macro2(T, T2)						\
  template<> void idx_power(idx<T> &in, T2 p, idx<T> &out) {            \
    idx_checknelems2_all(in, out);                                      \
    th_pow(in, out, (T) p);                                             \
  }


#endif /* __TH__ */
#endif /* IDXOPS_TH_HPP_ */
