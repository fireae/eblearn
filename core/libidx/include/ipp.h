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

#ifndef IPP_H
#define IPP_H

#include "config.h"
#include "numerics.h"
#include "idx.h"
#include "ippops.h"

#ifdef __IPP__

namespace ebl {

  // idx_copy //////////////////////////////////////////////////////////////////

  //! copy, specialized ubyte version
  template <> void idx_copy(const idx<ubyte> &src, idx<ubyte> &dst);
  //! copy, specialized uint16 version
  template <> void idx_copy(const idx<uint16> &src, idx<uint16> &dst);
  //! copy, specialized int16 version
  template <> void idx_copy(const idx<int16> &src, idx<int16> &dst);
  //! copy, specialized int32 version
  template <> void idx_copy(const idx<int32> &src, idx<int32> &dst);

  // idx_clear /////////////////////////////////////////////////////////////////

  //! Fill this idx with zeros, specialized ubyte version.
  template<> void idx_clear(idx<ubyte> &inp);
  //! Fill this idx with zeros, specialized uint16 version.
  template<> void idx_clear(idx<uint16> &inp);
  //! Fill this idx with zeros, specialized int16 version.
  template<> void idx_clear(idx<int16> &inp);
  //! Fill this idx with zeros, specialized float32 version.
  template<> void idx_clear(idx<float32> &inp);

  // idx_fill //////////////////////////////////////////////////////////////////

  //! Fill this idx with v, specialized ubyte version.  
  template<> void idx_fill(idx<ubyte> &inp, ubyte v);
  //! Fill this idx with v, specialized uint16 version.  
  template<> void idx_fill(idx<uint16> &inp, uint16 v);
  //! Fill this idx with v, specialized int16 version.  
  template<> void idx_fill(idx<int16> &inp, int16 v);
  //! Fill this idx with v, specialized float32 version.  
  template<> void idx_fill(idx<float32> &inp, float32 v);

  // idx_add (not-in-place) ////////////////////////////////////////////////////

  //! add two idx's, specialized ubyte version
  template<> void idx_add(idx<ubyte> &i1, idx<ubyte> &i2, idx<ubyte> &out);
  //! add two idx's, specialized uint16 version
  template<> void idx_add(idx<uint16> &i1, idx<uint16> &i2, idx<uint16> &out);
  //! add two idx's, specialized int16 version
  template<> void idx_add(idx<int16> &i1, idx<int16> &i2, idx<int16> &out);
  //! add two idx's, specialized float32 version
  template<> void idx_add(idx<float32> &i1, idx<float32> &i2,
			  idx<float32> &out);

  // idx_add (in-place) ////////////////////////////////////////////////////////

  //! Add two idx's as follow: out = out + in, specialized ubyte version
  template<> void idx_add(idx<ubyte> &in, idx<ubyte> &out);
  //! Add two idx's as follow: out = out + in, specialized uint16 version
  template<> void idx_add(idx<uint16> &in, idx<uint16> &out);
  //! Add two idx's as follow: out = out + in, specialized int16 version
  template<> void idx_add(idx<int16> &in, idx<int16> &out);

  // idx_addc //////////////////////////////////////////////////////////////////
  // TODO: add inplace addc, which can be used for speed up in
  // bias modules for example where output is the input

  //! add a constant to each element:  o1 <- i1+c, specialized ubyte version
  template<> void idx_addc(idx<ubyte> &inp, ubyte c, idx<ubyte> &out);
  //! add a constant to each element:  o1 <- i1+c, specialized uint16 version
  template<> void idx_addc(idx<uint16> &inp, uint16 c, idx<uint16> &out);
  //! add a constant to each element:  o1 <- i1+c, specialized int16 version
  template<> void idx_addc(idx<int16> &inp, int16 c, idx<int16> &out);
  //! add a constant to each element:  o1 <- i1+c, specialized float32 version
  template<> void idx_addc(idx<float32> &inp, float32 c, idx<float32> &out);

  // idx_addc_bounded //////////////////////////////////////////////////////////

  //! add a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized ubyte version
  template<> void idx_addc_bounded(idx<ubyte> &inp, ubyte c, idx<ubyte> &out);
  //! add a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized uint16 version
  template<> void idx_addc_bounded(idx<uint16> &inp, uint16 c,
				   idx<uint16> &out);
  //! add a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized int16 version
  template<> void idx_addc_bounded(idx<int16> &inp, int16 c, idx<int16> &out);
  //! add a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized float32 version
  template<> void idx_addc_bounded(idx<float32> &inp, float32 c,
				   idx<float32> &out);

  // idx_sub (not-in-place) ////////////////////////////////////////////////////

  //! sub two idx's, specialized ubyte version
  template<> void idx_sub(idx<ubyte> &i1, idx<ubyte> &i2, idx<ubyte> &out);
  //! sub two idx's, specialized uint16 version
  template<> void idx_sub(idx<uint16> &i1, idx<uint16> &i2, idx<uint16> &out);
  //! sub two idx's, specialized int16 version
  template<> void idx_sub(idx<int16> &i1, idx<int16> &i2, idx<int16> &out);
  //! sub two idx's, specialized float32 version
  template<> void idx_sub(idx<float32> &i1, idx<float32> &i2,
			  idx<float32> &out);

  // idx_subc_bounded //////////////////////////////////////////////////////////

  //! sub a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized ubyte version
  template<> void idx_subc_bounded(idx<ubyte> &inp, ubyte c, idx<ubyte> &out);
  //! sub a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized uint16 version
  template<>
  void idx_subc_bounded(idx<uint16> &inp, uint16 c, idx<uint16> &out);
  //! sub a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized int16 version
  template<> void idx_subc_bounded(idx<int16> &inp, int16 c, idx<int16> &out);
  //! sub a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized float32 version
  template<>
  void idx_subc_bounded(idx<float32> &inp, float32 c, idx<float32> &out);

  // idx_minus /////////////////////////////////////////////////////////////////

  //! negate all elements, specialized int16 version
  template<> void idx_minus(idx<int16> &in, idx<int16> &out);
  //! negate all elements, specialized float32 version
  template<> void idx_minus(idx<float32> &in, idx<float32> &out);

  // idx_minus_acc /////////////////////////////////////////////////////////////

  //! negate all elements, specialized int16 version
  template<> void idx_minus_acc(idx<int16> &in, idx<int16> &out);
  //! negate all elements, specialized float32 version
  template<> void idx_minus_acc(idx<float32> &in, idx<float32> &out);

  // idx_mul (not-in-place) ////////////////////////////////////////////////////

  //! mul two idx's, specialized ubyte version
  template<> void idx_mul(idx<ubyte> &i1, idx<ubyte> &i2, idx<ubyte> &out);
  //! mul two idx's, specialized uint16 version
  template<> void idx_mul(idx<uint16> &i1, idx<uint16> &i2, idx<uint16> &out);
  //! mul two idx's, specialized int16 version
  template<> void idx_mul(idx<int16> &i1, idx<int16> &i2, idx<int16> &out);
  //! mul two idx's, specialized float32 version
  template<> void idx_mul(idx<float32> &i1, idx<float32> &i2,
			  idx<float32> &out);

  // idx_dotc //////////////////////////////////////////////////////////////////

  //! multiply all elements by a constant, specialized ubyte version
  template<> void idx_dotc(idx<ubyte> &inp, ubyte c, idx<ubyte> &out);
  //! multiply all elements by a constant, specialized uint16 version
  template<> void idx_dotc(idx<uint16> &inp, uint16 c, idx<uint16> &out);
  //! multiply all elements by a constant, specialized int16 version
  template<> void idx_dotc(idx<int16> &inp, int16 c, idx<int16> &out);
  //! multiply all elements by a constant, specialized float32 version
  template<> void idx_dotc(idx<float32> &inp, float32 c, idx<float32> &out);

  // idx_dotc_bounded //////////////////////////////////////////////////////////

  //! sub a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized ubyte version
  template<> void idx_dotc_bounded(idx<ubyte> &inp, ubyte c, idx<ubyte> &out);
  //! sub a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized uint16 version
  template<>
  void idx_dotc_bounded(idx<uint16> &inp, uint16 c, idx<uint16> &out);
  //! sub a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized int16 version
  template<> void idx_dotc_bounded(idx<int16> &inp, int16 c, idx<int16> &out);
  //! sub a constant to each element:  o1 <- i1+c, bounding the min and max.
  // specialized float32 version
  template<>
  void idx_dotc_bounded(idx<float32> &inp, float32 c, idx<float32> &out);

  // idx_div ///////////////////////////////////////////////////////////////////

  //! div two idx's, specialized ubyte version
  template<> void idx_div(idx<ubyte> &i1, idx<ubyte> &i2, idx<ubyte> &out);
  //! div two idx's, specialized uint16 version
  template<> void idx_div(idx<uint16> &i1, idx<uint16> &i2, idx<uint16> &out);
  //! div two idx's, specialized int16 version
  template<> void idx_div(idx<int16> &i1, idx<int16> &i2, idx<int16> &out);
  //! div two idx's, specialized float32 version
  template<> void idx_div(idx<float32> &i1, idx<float32> &i2,
			  idx<float32> &out);
  
  // idx_inv ///////////////////////////////////////////////////////////////////

  //! Inverts all elements, i.e. for each element e, e = 1/e.
  //! This version is a specialized to float32 with IPP.
  template<> void idx_inv(idx<float32> &inp, idx<float32> &out);

  // idx_abs ///////////////////////////////////////////////////////////////////

  //! absolute value, specialized int16 version
  template<> void idx_abs(idx<int16>& inp, idx<int16>& out);
  //! absolute value, specialized float32 version
  template<> void idx_abs(idx<float32>& inp, idx<float32>& out);

  // idx_threshold (in-place) //////////////////////////////////////////////////

  //! if input is less than th, assign th, specialized ubyte version
  template<> void idx_threshold(idx<ubyte>& in, ubyte th);
  //! if input is less than th, assign th, specialized uint16 version
  template<> void idx_threshold(idx<uint16>& in, uint16 th);
  //! if input is less than th, assign th, specialized int16 version
  template<> void idx_threshold(idx<int16>& in, int16 th);
  //! if input is less than th, assign th, specialized float32 version
  template<> void idx_threshold(idx<float32>& in, float32 th);

  // idx_threshold (not-in-place) //////////////////////////////////////////////
  
  //! if input is less than th, assign th, otherwise copy <in>
  //! specialized ubyte version
  template<> void idx_threshold(idx<ubyte>& in, ubyte th, idx<ubyte>& out);
  //! if input is less than th, assign th, otherwise copy <in>
  //! specialized uint16 version
  template<> void idx_threshold(idx<uint16>& in, uint16 th, idx<uint16>& out);
  //! if input is less than th, assign th, otherwise copy <in>
  //! specialized int16 version
  template<> void idx_threshold(idx<int16>& in, int16 th, idx<int16>& out);
  //! if input is less than th, assign th, otherwise copy <in>
  //! specialized float32 version
  template<> void idx_threshold(idx<float32>& in, float32 th,idx<float32>& out);

  // idx_threshold (with value, in-place) //////////////////////////////////////
  
  //! if input is less than th, assign value, specialized ubyte version
  template<> void idx_threshold(idx<ubyte>& in, ubyte th, ubyte value);
  //! if input is less than th, assign value, specialized uint16 version
  template<> void idx_threshold(idx<uint16>& in, uint16 th, uint16 value);
  //! if input is less than th, assign value, specialized int16 version
  template<> void idx_threshold(idx<int16>& in, int16 th, int16 value);
  //! if input is less than th, assign value, specialized float32 version
  template<> void idx_threshold(idx<float32>& in, float32 th, float32 value);

  // idx_threshold (with value, not-in-place) //////////////////////////////////

  //! if input is less than th, assign value, otherwise copy <in>
  //! specialized ubyte version
  template<>
  void idx_threshold(idx<ubyte>& in, ubyte th, ubyte value, idx<ubyte>& out);
  //! if input is less than th, assign value, otherwise copy <in>
  //! specialized uint16 version
  template<>
  void idx_threshold(idx<uint16>& in, uint16 th, uint16 value,idx<uint16>& out);
  //! if input is less than th, assign value, otherwise copy <in>
  //! specialized int16 version
  template<>
  void idx_threshold(idx<int16>& in, int16 th, int16 value, idx<int16>& out);  
  //! if input is less than th, assign value, otherwise copy <in>
  //! specialized float32 version
  template<>
  void idx_threshold(idx<float32>& in, float32 th,
		     float32 value, idx<float32>& out);
 
  // idx_sqrt //////////////////////////////////////////////////////////////////
  
  //! square root, specialized ubyte version
  template<> void idx_sqrt(idx<ubyte>& inp, idx<ubyte>& out);
  //! square root, specialized uint16 version
  template<> void idx_sqrt(idx<uint16>& inp, idx<uint16>& out);
  //! square root, specialized int16 version
  template<> void idx_sqrt(idx<int16>& inp, idx<int16>& out);
  //! square root, specialized float32 version
  template<> void idx_sqrt(idx<float32>& inp, idx<float32>& out);

  // idx_exp ///////////////////////////////////////////////////////////////////

  //! exponential, specialized ubyte version
  template<> EXPORT void idx_exp(idx<ubyte>& inp);
  //! exponential, specialized uint16 version
  template<> EXPORT void idx_exp(idx<uint16>& inp);
  //! exponential, specialized int16 version
  template<> void idx_exp(idx<int16>& inp);
  //! exponential, specialized float32 version
  //template<> void idx_exp(idx<float32>& inp);

  // idx_sum ///////////////////////////////////////////////////////////////////

#if 0 //TODO

  //! returns the sum of all the terms, specialized ubyte version
  template<> ubyte idx_sum(idx<ubyte> &inp, ubyte *out);
  //! returns the sum of all the terms, specialized uint16 version
  template<> uint16 idx_sum(idx<uint16> &inp, uint16 *out);
  //! returns the sum of all the terms, specialized int16 version
  template<> int16 idx_sum(idx<int16> &inp, int16 *out);  

#if defined(__OPENMP__) and defined(__USE_SSE__)
  //! returns the sum of all the terms, specialized float32 version
  template<> float32 idx_sum(idx<float32> &inp, float32 *out);
#endif

#endif

  // idx_sumabs ////////////////////////////////////////////////////////////////

  //! returns the sum of the abs of all the terms, specialized ubyte version
  template<> float64 idx_sumabs(idx<ubyte> &inp, ubyte *out);
  //! returns the sum of the abs of all the terms, specialized uint16 version
  template<> float64 idx_sumabs(idx<uint16> &inp, uint16 *out);
  //! returns the sum of the abs of all the terms, specialized int16 version
  template<> float64 idx_sumabs(idx<int16> &inp, int16 *out);
  //! returns the sum of the abs of all the terms, specialized float32 version
  template<> float64 idx_sumabs(idx<float32> &inp, float32 *out);

  // idx_l2norm ////////////////////////////////////////////////////////////////

  //! l2 norm of an idx, seen as a vector, specialized ubyte version
  template<> float64 idx_l2norm(idx<ubyte> &in);
  //! l2 norm of an idx, seen as a vector, specialized uint16 version
  template<> float64 idx_l2norm(idx<uint16> &in);
  //! l2 norm of an idx, seen as a vector, specialized int16 version
  template<> float64 idx_l2norm(idx<int16> &in);
  //! l2 norm of an idx, seen as a vector, specialized float32 version
  template<> float64 idx_l2norm(idx<float32> &in);

  // idx_mean //////////////////////////////////////////////////////////////////

  //! l2 norm of an idx, seen as a vector, specialized ubyte version
  template<> ubyte idx_mean(idx<ubyte> &in, ubyte* out);
  //! l2 norm of an idx, seen as a vector, specialized uint16 version
  template<> uint16 idx_mean(idx<uint16> &in, uint16* out);
  //! l2 norm of an idx, seen as a vector, specialized int16 version
  template<> int16 idx_mean(idx<int16> &in, int16* out);
  //! l2 norm of an idx, seen as a vector, specialized float32 version
  template<> float32 idx_mean(idx<float32> &in, float32* out);

  // idx_std_normalize /////////////////////////////////////////////////////////

  //! removes the mean of <in> and divide by the standard deviation.
  //! reuse <mean> if not null, otherwise recompute it.
  //! specialized float32 version
  template<>
  EXPORT void idx_std_normalize(idx<float32> &in, idx<float32> &out,
				float32 *mean);
  
  // idx_dot ///////////////////////////////////////////////////////////////////

#if defined(__CBLAS__) || (defined(__IPP__) && defined(__IPP_DOT__))
  //! dot product of two idx. Returns sum of product of all elements.
  EXPORT float64 idx_dot(idx<float32> &i1, idx<float32> &i2);
#endif

#if defined(__IPP__) && defined(__IPP_DOT__)
  //! dot product of two idx, specialized ubyte version
  template<> float64 idx_dot(idx<ubyte> &i1, idx<ubyte> &i2);
  //! dot product of two idx, specialized byte version
  template<> float64 idx_dot(idx<byte> &i1, idx<byte> &i2);
  //! dot product of two idx, specialized uint16 version
  template<> float64 idx_dot(idx<uint16> &i1, idx<uint16> &i2);
  //! dot product of two idx, specialized int16 version
  template<> float64 idx_dot(idx<int16> &i1, idx<int16> &i2);
  //! dot product of two idx, specialized uint32 version
  template<> float64 idx_dot(idx<uint32> &i1, idx<uint32> &i2);
  //! dot product of two idx, specialized int32 version
  template<> float64 idx_dot(idx<int32> &i1, idx<int32> &i2);
#endif

  // m2dotm1 ///////////////////////////////////////////////////////////////////

  //! matrix-vector multiplication y <- a.x
  template <>
    void idx_m2dotm1(idx<float32> &a, idx<float32> &x, idx<float32> &y);

  // idx_max ///////////////////////////////////////////////////////////////////

  //! returns largest element in m, specialized ubyte version
  template<> ubyte idx_max(idx<ubyte> &m);
  //! returns largest element in m, specialized uint16 version
  template<> uint16 idx_max(idx<uint16> &m);
  //! returns largest element in m, specialized int16 version
  template<> int16 idx_max(idx<int16> &m);
  //! returns largest element in m, specialized float32 version
  template<> float32 idx_max(idx<float32> &m);

  // idx_max between two idx's (in-place) //////////////////////////////////////

  //! Copy maximum between each element of in1 and in2 into in2.
  //! specialized ubyte version
  template<> void idx_max(idx<ubyte> &in1, idx<ubyte> &in2);
  //! Copy maximum between each element of in1 and in2 into in2.
  //! specialized uint16 version
  template<> void idx_max(idx<uint16> &in1, idx<uint16> &in2);
  //! Copy maximum between each element of in1 and in2 into in2.
  //! specialized int16 version
  template<> void idx_max(idx<int16> &in1, idx<int16> &in2);
  //! Copy maximum between each element of in1 and in2 into in2.
  //! specialized float32 version
  template<> void idx_max(idx<float32> &in1, idx<float32> &in2);

  // idx_max between two idx's (not-in-place) //////////////////////////////////

  //! Copy maximum between each element of in1 and in2 into out.
  //! specialized ubyte version
  template<> void idx_max(idx<ubyte> &in1, idx<ubyte> &in2, idx<ubyte> &out);
  //! Copy maximum between each element of in1 and in2 into out.
  //! specialized uint16 version
  template<> void idx_max(idx<uint16> &in1, idx<uint16> &in2, idx<uint16> &out);
  //! Copy maximum between each element of in1 and in2 into out.
  //! specialized int16 version
  template<> void idx_max(idx<int16> &in1, idx<int16> &in2, idx<int16> &out);
  //! Copy maximum between each element of in1 and in2 into out.
  //! specialized float32 version
  template<>
  void idx_max(idx<float32> &in1, idx<float32> &in2, idx<float32> &out);

  // idx_min ///////////////////////////////////////////////////////////////////

  //! returns smallest element in m, specialized ubyte version
  template<> ubyte idx_min(idx<ubyte> &m);
  //! returnssmallest element in m, specialized uint16 version
  template<> uint16 idx_min(idx<uint16> &m);
  //! returns smallest element in m, specialized int16 version
  template<> int16 idx_min(idx<int16> &m);
  //! returns smallest element in m, specialized float32 version
  template<> float32 idx_min(idx<float32> &m);

  // idx_indexmax //////////////////////////////////////////////////////////////

  //! returns index of largest element of m, specialized ubyte version
  template<> intg idx_indexmax(idx<ubyte> &m);
  //! returns index of largest element of m, specialized uint16 version
  template<> intg idx_indexmax(idx<uint16> &m);
  //! returns index of largest element of m, specialized int16 version
  template<> intg idx_indexmax(idx<int16> &m);
  //! returns index of largest element of m, specialized float32 version
  template<> intg idx_indexmax(idx<float32> &m);

  // idx_indexmin //////////////////////////////////////////////////////////////

  //! returns index of smallest element of m, specialized ubyte version
  template<> intg idx_indexmin(idx<ubyte> &m);
  //! returns index of smallest element of m, specialized uint16 version
  template<> intg idx_indexmin(idx<uint16> &m);
  //! returns index of smallest element of m, specialized int16 version
  template<> intg idx_indexmin(idx<int16> &m);
  //! returns index of smallest element of m, specialized float32 version
  template<> intg idx_indexmin(idx<float32> &m);

  // idx_sqrdist ///////////////////////////////////////////////////////////////

  //! generalized Euclidean distance, specialized ubyte version
  template<> float64 idx_sqrdist(idx<ubyte> &i1, idx<ubyte> &i2);
  //! generalized Euclidean distance, specialized uint16 version
  template<> float64 idx_sqrdist(idx<uint16> &i1, idx<uint16> &i2);
  //! generalized Euclidean distance, specialized int16 version
  template<> float64 idx_sqrdist(idx<int16> &i1, idx<int16> &i2);
  //! generalized Euclidean distance, specialized float32 version
  template<> float64 idx_sqrdist(idx<float32> &i1, idx<float32> &i2);

  // idx_sqrdist (with idx out) ////////////////////////////////////////////////

  //! generalized Euclidean distance, specialized ubyte version
  template<>
  void idx_sqrdist(idx<ubyte> &i1, idx<ubyte> &i2, idx<ubyte> &out);
  //! generalized Euclidean distance, specialized uint16 version
  template<>
  void idx_sqrdist(idx<uint16> &i1, idx<uint16> &i2, idx<uint16> &out);
  //! generalized Euclidean distance, specialized int16 version
  template<>
  void idx_sqrdist(idx<int16> &i1, idx<int16> &i2, idx<int16> &out);
  //! generalized Euclidean distance, specialized float32 version
  template<>
  void idx_sqrdist(idx<float32> &i1, idx<float32> &i2, idx<float32> &out);
  
} // end namespace ebl

#include "idxops_ipp.hpp"

#endif /* ifdef __IPP__ */

#endif /* define IPP_H */
