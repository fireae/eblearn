/***************************************************************************
 *   Copyright (C) 2008 by Pierre Sermanet and Yann LeCun   *
 *   pierre.sermanet@gmail.com, yann@cs.nyu.edu   *
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


#ifndef IPPOPS_H_
#define IPPOPS_H_

#ifdef __IPP__
  #define DO_IF_IPP(with_ipp, without_ipp) with_ipp
#else
  #define DO_IF_IPP(with_ipp, without_ipp) without_ipp
#endif


#ifdef __IPP__
  #include <ipp.h>
/*   #if IPP_VERSION_MAJOR == 5 */
/*       #pragma message "IPP version is 5.x, ipp_dot is disabled" */
/*   #else */
/* //TODO: does not compile when IPP_DOT is defined */
/* //    #define __IPP_DOT__ */
/*   #endif */
  //TODO : check version (< 5)
#endif

#include "idx.h"

namespace ebl {

  // idx to IPP wrapper functions //////////////////////////////////////////////

  //! Initialize ipp to ncores, or without core limit by default,
  //! then print how many cores are enabled. If not compiled with IPP,
  //! this will print a message.
  inline void ipp_init(int ncores = -1);
  
#ifdef __IPP__

  // ipp_convolution ///////////////////////////////////////////////////////////

  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  //! <ker> is actually in reverse order, so you might
  //! want to reverse it first.
  template <typename T>
  inline void ipp_convolution(const idx<T> &in, const idx<T> &ker, idx<T> &out);
  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  //! <ker> is actually in reverse order, so you might
  //! want to reverse ubyteit first.
  template <>
  inline void ipp_convolution(const idx<ubyte> &in, const idx<ubyte> &ker,
			      idx<ubyte> &out);
  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  //! <ker> is actually in reverse order, so you might
  //! want to reverse it first.
  template <>
  inline void ipp_convolution(const idx<int16> &in, const idx<int16> &ker,
			      idx<int16> &out);
  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  //! <ker> is actually in reverse order, so you might
  //! want to reverse it first.
  template <>
  inline void ipp_convolution(const idx<float32> &in, const idx<float32> &ker,
			      idx<float32> &out);

  // ipp_copy //////////////////////////////////////////////////////////////////

  //! Copy in to out.
  template <typename T> inline void ipp_copy(const idx<T> &in, idx<T> &out);
  //! This calls ippiCopy_8u_C1R
  //! Copy in to out.
  template <> inline void ipp_copy(const idx<ubyte> &in, idx<ubyte> &out);
  //! This calls ippiCopy_16u_C1R
  //! Copy in to out.
  template <> inline void ipp_copy(const idx<uint16> &in, idx<uint16> &out);
  //! This calls ippiCopy_16s_C1R
  //! Copy in to out.
  template <> inline void ipp_copy(const idx<int16> &in, idx<int16> &out);
  //! This calls ippiCopy_32s_C1R
  //! Copy in to out.
  template <> inline void ipp_copy(const idx<int32> &in, idx<int32> &out);
  //! This calls ippiCopy_32f_C1R
  //! Copy in to out.
  template <> inline void ipp_copy(const idx<float32> &in, idx<float32> &out);

  // ipp_clear /////////////////////////////////////////////////////////////////

  //! Fill this idx with zeros.
  template <typename T> inline void ipp_clear(idx<T> &inp);
  //! this calls ippiSet_8u_C1R
  //! Fill this idx with zeros.
  template <> inline void ipp_clear(idx<ubyte> &inp);
  //! this calls ippiSet_16u_C1R
  //! Fill this idx with zeros.
  template <> inline void ipp_clear(idx<uint16> &inp);
  //! this calls ippiSet_16s_C1R
  //! Fill this idx with zeros.
  template <> inline void ipp_clear(idx<int16> &inp);
  //! this calls ippiSet_32s_C1R
  //! Fill this idx with zeros.
  template <> inline void ipp_clear(idx<int32> &inp);
  //! this calls ippiSet_32f_C1R
  //! Fill this idx with zeros.
  template <> inline void ipp_clear(idx<float32> &inp);

  // ipp_fill //////////////////////////////////////////////////////////////////

  //! Fill this idx with v.
  template <typename T> inline void ipp_fill(idx<T> &inp, T v);
  //! this calls ippiSet_8u_C1R
  //! Fill this idx with v.
  template <> inline void ipp_fill(idx<ubyte> &inp, ubyte v);
  //! this calls ippiSet_16u_C1R
  //! Fill this idx with v.
  template <> inline void ipp_fill(idx<uint16> &inp, uint16 v);
  //! this calls ippiSet_16s_C1R
  //! Fill this idx with v.
  template <> inline void ipp_fill(idx<int16> &inp, int16 v);
  //! this calls ippiSet_32s_C1R
  //! Fill this idx with v.
  template <> inline void ipp_fill(idx<int32> &inp, int32 v);
  //! this calls ippiSet_32f_C1R
  //! Fill this idx with v.
  template <> inline void ipp_fill(idx<float32> &inp, float32 v);

  // ipp_add ///////////////////////////////////////////////////////////////////
  
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in the second idx
  //! Overflows are handled by saturating
  //! Returns the ipp return code for the wrapped function
  template <typename T> inline void ipp_add(const idx<T> &in1, idx<T> &in2);
  //! this calls ippiAdd_32f_C1IR
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in the second idx.
  //! Returns the ipp return code for the wrapped function
  template <> inline void ipp_add(const idx<float32> &in1, idx<float32> &in2);
  //! this calls ippiAdd_8u_C1IRSfs
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in the second idx.
  //! Overflows are handled by saturating
  //! Returns the ipp return code for the wrapped function
  template <> inline void ipp_add(const idx<ubyte> &in1, idx<ubyte> &in2);
  //! this calls ippiAdd_16u_C1IRSfs
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in the second idx.
  //! Overflows are handled by saturating
  //! Returns the ipp return code for the wrapped function
  template <> inline void ipp_add(const idx<uint16> &in1, idx<uint16> &in2);
  //! this calls ippiAdd_16s_C1IRSfs
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in the second idx.
  //! Overflows are handled by saturating
  //! Returns the ipp return code for the wrapped function
  template <> inline void ipp_add(const idx<int16> &in1, idx<int16> &in2);
  
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in out.
  //! Overflows are handled by saturating
  //! Returns the ipp return code for the wrapped function
  template <typename T> inline void ipp_add(const idx<T> &in1,
					    const idx<T> &in2, idx<T> &out);
  //! this calls ippiAdd_32f_C1R
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in out.
  //! Returns the ipp return code for the wrapped function
  template <> inline void ipp_add(const idx<float32> &in1,
				  const idx<float32> &in2, idx<float32> &out);
  //! this calls ippiAdd_8u_C1RSfs
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in out.
  //! Overflows are handled by saturating
  //! Returns the ipp return code for the wrapped function
  template <> inline void ipp_add(const idx<ubyte> &in1, const idx<ubyte> &in2,
				  idx<ubyte> &out);
  //! this calls ippiAdd_16u_C1RSfs
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in out.
  //! Overflows are handled by saturating
  //! Returns the ipp return code for the wrapped function
  template <> inline void ipp_add(const idx<uint16> &in1,
				  const idx<uint16> &in2, idx<uint16> &out);
  //! this calls ippiAdd_16s_C1RSfs
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in out.
  //! Overflows are handled by saturating
  //! Returns the ipp return code for the wrapped function
  template <> inline void ipp_add(const idx<int16> &in1, const idx<int16> &in2,
				  idx<int16> &out);

  // ipp_addc //////////////////////////////////////////////////////////////////

  //! This adds a constant to each component of an idx. In place
  //! Overflows are handled by saturating
  template <typename T> inline void ipp_addc(idx<T> &in, T c);
  //! this calls ippiAddC_32f_C1IR
  //! This adds a constant to each component of an idx. In place
  template <> inline void ipp_addc(idx<float32> &in, float32 c);
  //! this calls ippiAddC_8u_C1IRSfs
  //! This adds a constant to each component of an idx. In place
  //! Overflows are handled by saturating
  template <> inline void ipp_addc(idx<ubyte> &in, ubyte c);
  //! this calls ippiAddC_16u_C1IRSfs
  //! This adds a constant to each component of an idx. In place
  //! Overflows are handled by saturating
  template <> inline void ipp_addc(idx<uint16> &in, uint16 c);
  //! this calls ippiAddC_16s_C1IRSfs
  //! This adds a constant to each component of an idx. In place
  //! Overflows are handled by saturating
  template <> inline void ipp_addc(idx<int16> &in, int16 c);

  //! This adds a constant to each component of an idx. NOT In place
  //! Overflows are handled by saturating
  template <typename T> inline void ipp_addc(const idx<T> &in, T c,
					     idx<T> &out);
  //! this calls ippiAddC_32f_C1R
  //! This adds a constant to each component of an idx. NOT In place
  template <> inline void ipp_addc(const idx<float32> &in, float32 c,
				   idx<float32> &out);
  //! this calls ippiAddC_8u_C1RSfs
  //! This adds a constant to each component of an idx. NOT In place
  //! Overflows are handled by saturating
  template <> inline void ipp_addc(const idx<ubyte> &in, ubyte c,
				   idx<ubyte> &out);
  //! this calls ippiAddC_16u_C1RSfs
  //! This adds a constant to each component of an idx. NOT In place
  //! Overflows are handled by saturating
  template <> inline void ipp_addc(const idx<uint16> &in, uint16 c,
				   idx<uint16> &out);
  //! this calls ippiAddC_16s_C1RSfs
  //! This adds a constant to each component of an idx. NOT In place
  //! Overflows are handled by sataurating
  template <> inline void ipp_addc(const idx<int16> &in, int16 c,
				   idx<int16> &out);

  // ipp_sub ///////////////////////////////////////////////////////////////////

  //! Substract 2 idx's. The result is stored in in1 (be careful, it is
  //! usually the opposite)
  template <typename T>
  inline void ipp_sub(idx<T> &in1, const idx<T> &in2);

  //! this calls ippiSub_32f_C1IR
  //! Substract 2 idx's. The result is stored in in1.
  template <>
  inline void ipp_sub(idx<float32> &in1, const idx<float32> &in2);

  //! this calls ippiSub_8u_C1IRSf
  //! Substract 2 idx's. The result is stored in in1.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_sub(idx<ubyte> &in1, const idx<ubyte> &in2);

  //! this calls ippiSub_16u_C1IRSf
  //! Substract 2 idx's. The result is stored in in1.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_sub(idx<uint16> &in1, const idx<uint16> &in2);

  //! this calls ippiSub_16s_C1IRSf
  //! Substract 2 idx's. The result is stored in in1.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_sub(idx<int16> &in1, const idx<int16> &in2);


  //! Substract 2 idx's. The result is stored in out.
  template <typename T>
  inline void ipp_sub(const idx<T> &in1, const idx<T> &in2, idx<T> &out);

  //! this calls ippiSub_32f_C1R
  //! Substract 2 idx's. The result is stored in out.
  template <>
  inline void ipp_sub(const idx<float32> &in1, const idx<float32> &in2,
		      idx<float32> &out);

  //! this calls ippiSub_8u_C1RSf
  //! Substract 2 idx's. The result is stored in out.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_sub(const idx<ubyte> &in1, const idx<ubyte> &in2,
		      idx<ubyte> &out);

  //! this calls ippiSub_16u_C1RSf
  //! Substract 2 idx's. The result is stored in out.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_sub(const idx<uint16> &in1, const idx<uint16> &in2,
		      idx<uint16> &out);

  //! this calls ippiSub_16s_C1RSf
  //! Substract 2 idx's. The result is stored in out.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_sub(const idx<int16> &in1, const idx<int16> &in2,
		      idx<int16> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_subc

  //! This subs a constant to each component of an idx.
  template <typename T>
  inline void ipp_subc(idx<T> &inp, T c);

  //! This calls ippiSubC_8u_C1IRSfs
  //! This subs a constant to each component of an idx.
  //! Overflows are handled by sataurating.
  template <>
  inline void ipp_subc(idx<ubyte> &inp, ubyte c);

  //! This calls ippiSubC_16u_C1IRSfs
  //! This subs a constant to each component of an idx.
  //! Overflows are handled by sataurating.
  template <>
  inline void ipp_subc(idx<uint16> &inp, uint16 c);

  //! This calls ippiSubC_16s_C1IRSfs
  //! This subs a constant to each component of an idx.
  //! Overflows are handled by sataurating.
  template <>
  inline void ipp_subc(idx<int16> &inp, int16 c);

  //! This calls ippiSubC_32f_C1IR
  //! This subs a constant to each component of an idx.
  template <>
  inline void ipp_subc(idx<float32> &inp, float32 c);


  //! This subs a constant to each component of an idx.
  //! The result is stored in <out>.
  template <typename T>
  inline void ipp_subc(const idx<T> &inp, T c, idx<T> &out);

  //! This calls ippiSubC_8u_C1IRSfs
  //! This subs a constant to each component of an idx.
  //! The result is stored in <out>.
  //! Overflows are handled by sataurating.
  template <>
  inline void ipp_subc(const idx<ubyte> &inp, ubyte c, idx<ubyte> &out);

  //! This calls ippiSubC_8u_C1IRSfs
  //! This subs a constant to each component of an idx.
  //! The result is stored in <out>.
  //! Overflows are handled by sataurating.
  template <>
  inline void ipp_subc(const idx<uint16> &inp, uint16 c, idx<uint16> &out);

  //! This calls ippiSubC_8u_C1IRSfs
  //! This subs a constant to each component of an idx.
  //! The result is stored in <out>.
  //! Overflows are handled by sataurating.
  template <>
  inline void ipp_subc(const idx<int16> &inp, int16 c, idx<int16> &out);

  //! This calls ippiSubC_8u_C1IRSfs
  //! This subs a constant to each component of an idx.
  //! The result is stored in <out>.
  //! Overflows are handled by sataurating.
  template <>
  inline void ipp_subc(const idx<float32> &inp, float32 c, idx<float32> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_minus

  //! Negates all elements. Works only with types int16 and float32
  template <typename T>
  inline void ipp_minus(const idx<T> &in, idx<T> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_mul

  //! Multiply 2 idx's element-wise. The result is stored in in2.
  template <typename T>
  inline void ipp_mul(const idx<T> &in1, idx<T> &in2);
  //! this calls ippiMul_32f_C1IR
  //! Multiply 2 idx's element-wise. The result is stored in in2.
  template <>
  inline void ipp_mul(const idx<float32> &in1, idx<float32> &in2);
  //! this calls ippiMul_8u_C1IRSfs
  //! Multiply 2 idx's element-wise. The result is stored in in2.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_mul(const idx<ubyte> &in1, idx<ubyte> &in2);  
  //! this calls ippiMul_16u_C1IRSfs
  //! Multiply 2 idx's element-wise. The result is stored in in2.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_mul(const idx<uint16> &in1, idx<uint16> &in2);  
  //! this calls ippiMul_16s_C1IRSfs
  //! Multiply 2 idx's element-wise. The result is stored in in2.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_mul(const idx<int16> &in1, idx<int16> &in2);  

  //! Multiply 2 idx's element-wise. The result is stored in out.
  template <typename T>
  inline void ipp_mul(const idx<T> &in1, const idx<T> &in2, idx<T> &out);

  //! this calls ippiMul_32f_C1R
  //! Multiply 2 idx's element-wise. The result is stored in out.
  template <>
  inline void ipp_mul(const idx<float32> &in1, const idx<float32> &in2,
		      idx<float32> &out);

  //! this calls ippiMul_8u_C1RSfs
  //! Multiply 2 idx's element-wise. The result is stored in out.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_mul(const idx<ubyte> &in1, const idx<ubyte> &in2,
		      idx<ubyte> &out);

  //! this calls ippiMul_16u_C1RSfs
  //! Multiply 2 idx's element-wise. The result is stored in out.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_mul(const idx<uint16> &in1, const idx<uint16> &in2,
		      idx<uint16> &out);

  //! this calls ippiMul_16s_C1RSfs
  //! Multiply 2 idx's element-wise. The result is stored in out.
  //! Overflows are handled by saturating
  template <>
  inline void ipp_mul(const idx<int16> &in1, const idx<int16> &in2,
		      idx<int16> &out);

  // ipp_m2dotm1 ///////////////////////////////////////////////////////////////

  //! Matrix-vector multiplication: y <- in1.in2
  template <typename T>
    inline void ipp_m2dotm1(const idx<T> &in1, const idx<T> &in2, idx<T> &y);
  //! Matrix-vector multiplication: y <- in1.in2
  template <>
    inline void ipp_m2dotm1(const idx<float32> &in1, const idx<float32> &in2,
			    idx<float32> &y);
  //! Matrix-vector multiplication: y <- in1.in2
  template <>
    inline void ipp_m2dotm1(const idx<float64> &in1, const idx<float64> &in2,
			    idx<float64> &y);
  
  // ipp_m2dotm2 ///////////////////////////////////////////////////////////////

  //! Matrix-matrix multiplication: y <- in1.in2
  template <typename T>
    inline void ipp_m2dotm2(const idx<T> &in1, const idx<T> &in2, idx<T> &y);
  //! Matrix-matrix multiplication: y <- in1.in2
  template <>
    inline void ipp_m2dotm2(const idx<float32> &in1, const idx<float32> &in2,
			    idx<float32> &y);
  //! Matrix-matrix multiplication: y <- in1.in2
  template <>
    inline void ipp_m2dotm2(const idx<float64> &in1, const idx<float64> &in2,
			    idx<float64> &y);
  
  ////////////////////////////////////////////////////////////////
  // ipp_dotc

  //! Multiply all elements by a constant. The result is stored in inp.
  //! For integers types, overflow are handled by saturating
  template <typename T>
  inline void ipp_dotc(idx<T> &inp, T c);

  //! this calls ippiMulC_32f_C1IR
  //! Multiply all elements by a constant. The result is stored in inp.
  template <>
  inline void ipp_dotc(idx<float32> &inp, float32 c);

  //! this calls ippiMulC_8u_C1IRSfs
  //! Multiply all elements by a constant. The result is stored in inp.
  template <>
  inline void ipp_dotc(idx<ubyte> &inp, ubyte c);

  //! this calls ippiMulC_16u_C1IRSfs
  //! Multiply all elements by a constant. The result is stored in inp.
  template <>
  inline void ipp_dotc(idx<uint16> &inp, uint16 c);

  //! this calls ippiMulC_16s_C1IRSfs
  //! Multiply all elements by a constant. The result is stored in inp.
  template <>
  inline void ipp_dotc(idx<int16> &inp, int16 c);


  //! Multiply all elements by a constant. The result is stored in out.
  //! For integers types, overflow are handled by saturating
  template <typename T>
  inline void ipp_dotc(const idx<T> &inp, T c, idx<T> &out);

  //! this calls ippiMulC_8u_C1RSfs
  //! Multiply all elements by a constant. The result is stored in inp.
  template <>
  inline void ipp_dotc(const idx<ubyte> &inp, ubyte c, idx<ubyte> &out);

  //! this calls ippiMulC_16u_C1RSfs
  //! Multiply all elements by a constant. The result is stored in inp.
  template <>
  inline void ipp_dotc(const idx<uint16> &inp, uint16 c, idx<uint16> &out);

  //! this calls ippiMulC_16s_C1RSfs
  //! Multiply all elements by a constant. The result is stored in inp.
  template <>
  inline void ipp_dotc(const idx<int16> &inp, int16 c, idx<int16> &out);

  //! this calls ippiMulC_32f_C1R
  //! Multiply all elements by a constant. The result is stored in inp.
  template <>
  inline void ipp_dotc(const idx<float32> &inp, float32 c, idx<float32> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_div

  //! Divides 2 idx's element-wise. The result is stored in in1 (be careful
  //! it is usually the opposite).
  //! For integer types, the result is rounded (not truncated),
  //! with a non-deterministic result for half integers
  //! (eg. 0.5 can be rounded to 0 or to 1)
  template <typename T>
  inline void ipp_div(idx<T> &in1, const idx<T> &in2);

  //! this calls ippiDiv_32f_C1IR
  //! Divides 2 idx's element-wise. The result is stored in in1.
  template <>
  inline void ipp_div(idx<float32> &in1, const idx<float32> &in2);

  //! this calls ippiDiv_8u_C1IRSfs
  //! Divides 2 idx's element-wise. The result is stored in in1.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_div(idx<ubyte> &in1, const idx<ubyte> &in2);

  //! this calls ippiDiv_16u_C1IRSfs
  //! Divides 2 idx's element-wise. The result is stored in in1.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_div(idx<uint16> &in1, const idx<uint16> &in2);

  //! this calls ippiDiv_16s_C1IRSfs
  //! Divides 2 idx's element-wise. The result is stored in in1.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_div(idx<int16> &in1, const idx<int16> &in2);


  //! Divides 2 idx's element-wise. The result is stored in out.
  //! For integer types, the result is rounded (not truncated),
  //! with a non-deterministic result for half integers
  //! (eg. 0.5 can be rounded to 0 or to 1)
  template <typename T>
  inline void ipp_div(const idx<T> &in1, const idx<T> &in2, idx<T> &out);

  //! this calls ippiDiv_32f_C1R
  //! Divides 2 idx's element-wise. The result is stored in out.
  template <>
  inline void ipp_div(const idx<float32> &in1, const idx<float32> &in2,
		      idx<float32> &out);

  //! this calls ippiDiv_8u_C1RSfs
  //! Divides 2 idx's element-wise. The result is stored in out.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_div(const idx<ubyte> &in1, const idx<ubyte> &in2,
		      idx<ubyte> &out);

  //! this calls ippiDiv_16u_C1RSfs
  //! Divides 2 idx's element-wise. The result is stored in out.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_div(const idx<uint16> &in1, const idx<uint16> &in2,
		      idx<uint16> &out);

  //! this calls ippiDiv_16s_C1RSfs
  //! Divides 2 idx's element-wise. The result is stored in out.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_div(const idx<int16> &in1, const idx<int16> &in2,
		      idx<int16> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_inv

  //! Inverts all elements. Works only with type float32
  inline void ipp_inv(const idx<float32> &in, idx<float32> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_abs

  //! Computes the abolute values of each element. The result is stored in inp
  template <typename T>
  inline void ipp_abs(idx<T> &inp);

  //! this calls ippiAbs_16s_C1IR
  //! Computes the abolute values of each element. The result is stored in inp
  template <>
  inline void ipp_abs(idx<int16> &inp);

  //! this calls ippiAbs_32f_C1IR
  //! Computes the abolute values of each element. The result is stored in inp
  template <>
  inline void ipp_abs(idx<float32> &inp);


  //! Computes the abolute values of each element. The result is stored in out
  template <typename T>
  inline void ipp_abs(const idx<T> &inp, idx<T> &out);

  //! this calls ippiAbs_16s_C1IR
  //! Computes the abolute values of each element. The result is stored in out
  template <>
  inline void ipp_abs(const idx<int16> &inp, idx<int16> &out);

  //! this calls ippiAbs_32f_C1IR
  //! Computes the abolute values of each element. The result is stored in out
  template <>
  inline void ipp_abs(const idx<float32> &inp, idx<float32> &out);

  
  ////////////////////////////////////////////////////////////////
  // ipp_sqrt

  //! Computes the square root of each element. The result is stored in inp.
  //! For integer types, the result is rounded (not truncated).
  template <typename T>
  inline void ipp_sqrt(idx<T> &inp);

  //! this calls ippiSqrt_8u_C1IRSfs
  //! Computes the square root of each element. The result is stored in inp.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_sqrt(idx<ubyte> &inp);

  //! this calls ippiSqrt_16u_C1IRSfs
  //! Computes the square root of each element. The result is stored in inp.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_sqrt(idx<uint16> &inp);

  //! this calls ippiSqrt_16s_C1IRSfs
  //! Computes the square root of each element. The result is stored in inp.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_sqrt(idx<int16> &inp);

  //! this calls ippiSqrt_32f_C1IR
  //! Computes the square root of each element. The result is stored in inp.
  template <>
  inline void ipp_sqrt(idx<float32> &inp);


  //! Computes the square root of each element. The result is stored in out.
  //! For integer types, the result is rounded (not truncated).
  template <typename T>
  inline void ipp_sqrt(const idx<T> &inp, idx<T> &out);

  //! this calls ippiSqrt_8u_C1IRSfs
  //! Computes the square root of each element. The result is stored in out.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_sqrt(const idx<ubyte> &inp, idx<ubyte> &out);

  //! this calls ippiSqrt_16u_C1IRSfs
  //! Computes the square root of each element. The result is stored in out.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_sqrt(const idx<uint16> &inp, idx<uint16> &out);

  //! this calls ippiSqrt_16s_C1IRSfs
  //! Computes the square root of each element. The result is stored in out.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_sqrt(const idx<int16> &inp, idx<int16> &out);

  //! this calls ippiSqrt_32f_C1IR
  //! Computes the square root of each element. The result is stored in out.
  template <>
  inline void ipp_sqrt(const idx<float32> &inp, idx<float32> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_exp

  //! Computes the exponential of each element. The result is stored in inp.
  //! For integer types, the result is rounded (not truncated).
  template <typename T>
  inline void ipp_exp(idx<T> &inp);

  //! this calls ippiExp_8u_C1IRSfs
  //! Computes the exponential of each element. The result is stored in inp.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_exp(idx<ubyte> &inp);

  //! this calls ippiExp_16u_C1IRSfs
  //! Computes the exponential of each element. The result is stored in inp.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_exp(idx<uint16> &inp);

  //! this calls ippiExp_16s_C1IRSfs
  //! Computes the exponential of each element. The result is stored in inp.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_exp(idx<int16> &inp);

  //! this calls ippiExp_32f_C1IR
  //! Computes the exponential of each element. The result is stored in inp.
  template <>
  inline void ipp_exp(idx<float32> &inp);


  //! Computes the exponential of each element. The result is stored in out.
  //! For integer types, the result is rounded (not truncated).
  template <typename T>
  inline void ipp_exp(const idx<T> &inp, idx<T> &out);

  //! this calls ippiExp_8u_C1IRSfs
  //! Computes the exponential of each element. The result is stored in out.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_exp(const idx<ubyte> &inp, idx<ubyte> &out);

  //! this calls ippiExp_16u_C1IRSfs
  //! Computes the exponential of each element. The result is stored in out.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_exp(const idx<uint16> &inp, idx<uint16> &out);

  //! this calls ippiExp_16s_C1IRSfs
  //! Computes the exponential of each element. The result is stored in out.
  //! The result is rounded (not truncated).
  template <>
  inline void ipp_exp(const idx<int16> &inp, idx<int16> &out);

  //! this calls ippiExp_32f_C1IR
  //! Computes the exponential of each element. The result is stored in out.
  template <>
  inline void ipp_exp(const idx<float32> &inp, idx<float32> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_sum

  //! Returns the sum of all the terms
  template <typename T>
  inline float64 ipp_sum (const idx<T> &inp);

  //! This calls ippiSum_8u_C1R
  //! Returns the sum of all the terms
  template <>
  inline float64 ipp_sum (const idx<ubyte> &inp);

  //! This calls ippiSum_16u_C1R
  //! Returns the sum of all the terms
  template <>
  inline float64 ipp_sum (const idx<uint16> &inp);

  //! This calls ippiSum_16s_C1R
  //! Returns the sum of all the terms
  template <>
  inline float64 ipp_sum (const idx<int16> &inp);

  //! This calls ippiSum_32f_C1R
  //! Returns the sum of all the terms
  template <>
  inline float64 ipp_sum (const idx<float32> &inp);

  ////////////////////////////////////////////////////////////////
  // ipp_sumacc

  //! Returns the sum of all the terms, accumulated in idx0 acc.
  //! Be careful that acc can saturate if it is an integer type.
  //! Returns a float64 version of the accumulated result (no saturation).
  template <typename T>
  inline float64 ipp_sumacc (const idx<T> &inp, idx<T> &acc);

  ////////////////////////////////////////////////////////////////
  // ipp_sumabs

  //! Returns the sum of the absolute values of all the terms
  template <typename T>
  inline float64 ipp_sumabs (const idx<T> &inp);

  //! This calls ippiNorm_L1_8u_C1R
  //! Returns the sum of the absolute values of all the terms
  template <>
  inline float64 ipp_sumabs (const idx<ubyte> &inp);

  //! This calls ippiNorm_L1_16u_C1R
  //! Returns the sum of the absolute values of all the terms
  template <>
  inline float64 ipp_sumabs (const idx<uint16> &inp);

  //! This calls ippiNorm_L1_16s_C1R
  //! Returns the sum of the absolute values of all the terms
  template <>
  inline float64 ipp_sumabs (const idx<int16> &inp);

  //! This calls ippiNorm_L1_32f_C1R
  //! Returns the sum of the absolute values of all the terms
  template <>
  inline float64 ipp_sumabs (const idx<float32> &inp);

  ////////////////////////////////////////////////////////////////
  // ipp_l2norm

  //! Returns the l2 norm of the idx.
  template <typename T>
  inline float64 ipp_l2norm (const idx<T> &inp);

  //! This calls ippiNorm_L2_8u_C1R
  //! Returns the l2 norm of the idx.
  template <>
  inline float64 ipp_l2norm (const idx<ubyte> &inp);

  //! This calls ippiNorm_L2_16u_C1R
  //! Returns the l2 norm of the idx.
  template <>
  inline float64 ipp_l2norm (const idx<uint16> &inp);

  //! This calls ippiNorm_L2_16s_C1R
  //! Returns the l2 norm of the idx.
  template <>
  inline float64 ipp_l2norm (const idx<int16> &inp);

  //! This calls ippiNorm_L2_32f_C1R
  //! Returns the l2 norm of the idx.
  template <>
  inline float64 ipp_l2norm (const idx<float32> &inp);

  ////////////////////////////////////////////////////////////////
  // ipp_mean

  //! Returns the mean of all the terms
  template <typename T>
  inline float64 ipp_mean (const idx<T> &inp);

  //! This calls ippiMean_8u_C1R
  //! Returns the mean of all the terms
  template <>
  inline float64 ipp_mean (const idx<ubyte> &inp);

  //! This calls ippiMean_16u_C1R
  //! Returns the mean of all the terms
  template <>
  inline float64 ipp_mean (const idx<uint16> &inp);

  //! This calls ippiMean_16s_C1R
  //! Returns the mean of all the terms
  template <>
  inline float64 ipp_mean (const idx<int16> &inp);

  //! This calls ippiMean_32f_C1R
  //! Returns the mean of all the terms
  template <>
  inline float64 ipp_mean (const idx<float32> &inp);

  ////////////////////////////////////////////////////////////////
  // ipp_std_normalize

  //! Removes the mean of <inp> and divides by the standard deviation.
  //! reuse <mean> if not null, otherwise recompute it.
  template <typename T>
  inline void ipp_std_normalize (const idx<T> &inp, idx<T> &out,
				 T* mean = NULL);

  //! Removes the mean of <inp> and divides by the standard deviation.
  //! reuse <mean> if not null, otherwise recompute it.
  //! This is much slower than the float32 version.
  template <>
  inline void ipp_std_normalize (const idx<ubyte> &inp, idx<ubyte> &out,
				 ubyte* mean);

  //! Removes the mean of <inp> and divides by the standard deviation.
  //! reuse <mean> if not null, otherwise recompute it.
  template <>
  inline void ipp_std_normalize (const idx<float32> &inp, idx<float32> &out,
				 float32* mean);


#ifdef __IPP_DOT__
  ////////////////////////////////////////////////////////////////
  // ipp_dot

  //! dot product of two idx. Returns sum of product of all elements.
  template <typename T>
  inline float64 ipp_dot(const idx<T> &in1, const idx<T> &in2);

  //! this calls ippiDotProd_8u64f_C1R
  //! dot product of two idx. Returns sum of product of all elements.
  template <>
  inline float64 ipp_dot(const idx<ubyte> &in1, const idx<ubyte> &in2);

  //! this calls ippiDotProd_8s64f_C1R
  //! dot product of two idx. Returns sum of product of all elements.
  template <>
  inline float64 ipp_dot(const idx<byte> &in1, const idx<byte> &in2);

  //! this calls ippiDotProd_16u64f_C1R
  //! dot product of two idx. Returns sum of product of all elements.
  template <>
  inline float64 ipp_dot(const idx<uint16> &in1, const idx<uint16> &in2);

  //! this calls ippiDotProd_16s64f_C1R
  //! dot product of two idx. Returns sum of product of all elements.
  template <>
  inline float64 ipp_dot(const idx<int16> &in1, const idx<int16> &in2);

  //! this calls ippiDotProd_32u64f_C1R
  //! dot product of two idx. Returns sum of product of all elements.
  template <>
  inline float64 ipp_dot(const idx<uint32> &in1, const idx<uint32> &in2);

  //! this calls ippiDotProd_32s64f_C1R
  //! dot product of two idx. Returns sum of product of all elements.
  template <>
  inline float64 ipp_dot(const idx<int32> &in1, const idx<int32> &in2);

  //! this calls ippiDotProd_32f64f_C1R
  //! dot product of two idx. Returns sum of product of all elements.
  template <>
  inline float64 ipp_dot(const idx<float32> &in1, const idx<float32> &in2);

  ////////////////////////////////////////////////////////////////
  // ipp_dotacc

  //! dot product of two idx. Accumulate result in an idx0.
  //! Be careful that it can saturate if it is an integer type.
  //! Return a float64 version of the accumulated result (no saturation)
  template <typename T>
  inline float64 ipp_dotacc(const idx<T> &in1, const idx<T> &in2,
			    idx<T> &acc);
#endif /* __IPP_DOT__ */

  ////////////////////////////////////////////////////////////////
  // ipp_max

  //! Returns largest element in inp
  template <typename T>
  inline T ipp_max(const idx<T> &inp);

  //! This calls ippiMax_8u_C1R
  //! Returns largest element in inp
  template <>
  inline ubyte ipp_max(const idx<ubyte> &inp);

  //! This calls ippiMax_16u_C1R
  //! Returns largest element in inp
  template <>
  inline uint16 ipp_max(const idx<uint16> &inp);

  //! This calls ippiMax_16s_C1R
  //! Returns largest element in inp
  template <>
  inline int16 ipp_max(const idx<int16> &inp);

  //! This calls ippiMax_32f_C1R
  //! Returns largest element in inp
  template <>
  inline float32 ipp_max(const idx<float32> &inp);

  ////////////////////////////////////////////////////////////////
  // ipp_indexmax

  //! Returns index of largest element of inp,
  //! considering inp as a 1 dimensional array
  template <typename T>
  inline intg ipp_indexmax(const idx<T> &inp);

  //! This calls ippiMaxIndx_8u_C1R
  //! Returns index of largest element of inp,
  //! considering inp as a 1 dimensional array
  template <>
  inline intg ipp_indexmax(const idx<ubyte> &inp);

  //! This calls ippiMaxIndx_16u_C1R
  //! Returns index of largest element of inp,
  //! considering inp as a 1 dimensional array
  template <>
  inline intg ipp_indexmax(const idx<uint16> &inp);

  //! This calls ippiMaxIndx_16s_C1R
  //! Returns index of largest element of inp,
  //! considering inp as a 1 dimensional array
  template <>
  inline intg ipp_indexmax(const idx<int16> &inp);

  //! This calls ippiMaxIndx_32f_C1R
  //! Returns index of largest element of inp,
  //! considering inp as a 1 dimensional array
  template <>
  inline intg ipp_indexmax(const idx<float32> &inp);

  ////////////////////////////////////////////////////////////////
  // ipp_min

  //! Returns smallest element in inp
  template <typename T>
  inline T ipp_min(const idx<T> &inp);

  //! This calls ippiMin_8u_C1R
  //! Returns smallest element in inp
  template <>
  inline ubyte ipp_min(const idx<ubyte> &inp);

  //! This calls ippiMin_16u_C1R
  //! Returns smallest element in inp
  template <>
  inline uint16 ipp_min(const idx<uint16> &inp);

  //! This calls ippiMin_16s_C1R
  //! Returns smallest element in inp
  template <>
  inline int16 ipp_min(const idx<int16> &inp);

  //! This calls ippiMin_32f_C1R
  //! Returns smallest element in inp
  template <>
  inline float32 ipp_min(const idx<float32> &inp);

  ////////////////////////////////////////////////////////////////
  // ipp_indexmin

  //! Returns index of smallest element of inp,
  //! considering inp as a 1 dimensional array
  template <typename T>
  inline intg ipp_indexmin(const idx<T> &inp);

  //! This calls ippiMinIndx_8u_C1R
  //! Returns index of smallest element of inp,
  //! considering inp as a 1 dimensional array
  template <>
  inline intg ipp_indexmin(const idx<ubyte> &inp);

  //! This calls ippiMinIndx_16u_C1R
  //! Returns index of smallest element of inp,
  //! considering inp as a 1 dimensional array
  template <>
  inline intg ipp_indexmin(const idx<uint16> &inp);

  //! This calls ippiMinIndx_16s_C1R
  //! Returns index of smallest element of inp,
  //! considering inp as a 1 dimensional array
  template <>
  inline intg ipp_indexmin(const idx<int16> &inp);

  //! This calls ippiMinIndx_32f_C1R
  //! Returns index of smallest element of inp,
  //! considering inp as a 1 dimensional array
  template <>
  inline intg ipp_indexmin(const idx<float32> &inp);

  ////////////////////////////////////////////////////////////////
  // ipp_maxevery (in-place)

  //! Puts maximum between each element of in1 and in2 into in2.
  template<typename T>
  inline void ipp_maxevery(const idx<T> &in1, idx<T> &in2);

  // This calls ippiMaxEvery_8u_C1IR
  //! Puts maximum between each element of in1 and in2 into in2.
  template<>
  inline void ipp_maxevery(const idx<ubyte> &in1, idx<ubyte> &in2);

  // This calls ippiMaxEvery_16u_C1IR
  //! Puts maximum between each element of in1 and in2 into in2.
  template<>
  inline void ipp_maxevery(const idx<uint16> &in1, idx<uint16> &in2);

  // This calls ippiMaxEvery_16s_C1IR
  //! Puts maximum between each element of in1 and in2 into in2.
  template<>
  inline void ipp_maxevery(const idx<int16> &in1, idx<int16> &in2);

  // This calls ippiMaxEvery_32f_C1IR
  //! Puts maximum between each element of in1 and in2 into in2
  template<>
  inline void ipp_maxevery(const idx<float32> &in1, idx<float32> &in2);

  ////////////////////////////////////////////////////////////////
  // ipp_maxevery (not-in-place)

  //! Puts maximum between each element of in1 and in2 into out.
  //! This calls the in-place version, and so it is slower.
  template<typename T>
  inline void ipp_maxevery(const idx<T> &in1, const idx<T> &in2, idx<T> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_sqrdist

  //! Returns the generalized Euclidean distance between <i1> and <i2>,
  //! i.e. the sum of squares of all the differences
  //! between corresponding terms of <i1> and <i2>.
  template<typename T>
  inline float64 ipp_sqrdist(const idx<T> &i1, const idx<T> &i2);

  //! This calls ippiNormDiff_L2_8u_C1R
  //! Returns the generalized Euclidean distance between <i1> and <i2>,
  template<>
  inline float64 ipp_sqrdist(const idx<ubyte> &i1, const idx<ubyte> &i2);

  //! This calls ippiNormDiff_L2_16u_C1R
  //! Returns the generalized Euclidean distance between <i1> and <i2>,
  template<>
  inline float64 ipp_sqrdist(const idx<uint16> &i1, const idx<uint16> &i2);

  //! This calls ippiNormDiff_L2_16s_C1R
  //! Returns the generalized Euclidean distance between <i1> and <i2>,
  template<>
  inline float64 ipp_sqrdist(const idx<int16> &i1, const idx<int16> &i2);

  //! This calls ippiNormDiff_L2_32f_C1R
  //! Returns the generalized Euclidean distance between <i1> and <i2>,
  template<>
  inline float64 ipp_sqrdist(const idx<float32> &i1, const idx<float32> &i2);


  //! Generalized Euclidean distance between <i1> and <i2>,
  //! i.e. the sum of squares of all the differences
  //! between corresponding terms of <i1> and <i2>.
  //! Returns a float64 version of the result
  //! The result is also stored in <out>, which must be an idx0.
  //! Be careful that it can saturate if it is an integer type.
  template<typename T>
  inline float64 ipp_sqrdist(const idx<T> &i1, const idx<T> &i2, idx<T> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_threshold_lt (in-place)

  //! if input is less than th, assign <th>
  template <typename T>
  inline void ipp_threshold_lt(idx<T> &in, T th);

  //! This calls ippiThreshold_LT_8u_C1IR
  //! if input is less than th, assign <th>
  template <>
  inline void ipp_threshold_lt(idx<ubyte> &in, ubyte th);

  //! This calls ippiThreshold_LT_16u_C1IR
  //! if input is less than th, assign <th>
  template <>
  inline void ipp_threshold_lt(idx<uint16> &in, uint16 th);

  //! This calls ippiThreshold_LT_16s_C1IR
  //! if input is less than th, assign <th>
  template <>
  inline void ipp_threshold_lt(idx<int16> &in, int16 th);

  //! This calls ippiThreshold_LT_32f_C1IR
  //! if input is less than th, assign <th>
  template <>
  inline void ipp_threshold_lt(idx<float32> &in, float32 th);

  ////////////////////////////////////////////////////////////////
  // ipp_threshold_lt (not-in-place)

  //! if input is less than th, assign <th>, otherwise copy <in>
  template <typename T>
  inline void ipp_threshold_lt(const idx<T> &in, T th, idx<T> &out);

  //! This calls ippiThreshold_LT_8u_C1R
  //! if input is less than th, assign <th>, otherwise copy <in>
  template <>
  inline void ipp_threshold_lt(const idx<ubyte> &in, ubyte th,
			       idx<ubyte> &out);

  //! This calls ippiThreshold_LT_16u_C1R
  //! if input is less than th, assign <th>, otherwise copy <in>
  template <>
  inline void ipp_threshold_lt(const idx<uint16> &in, uint16 th,
			       idx<uint16> &out);

  //! This calls ippiThreshold_LT_16s_C1R
  //! if input is less than th, assign <th>, otherwise copy <in>
  template <>
  inline void ipp_threshold_lt(const idx<int16> &in, int16 th,
			       idx<int16> &out);

  //! This calls ippiThreshold_LT_32f_C1R
  //! if input is less than th, assign <th>, otherwise copy <in>
  template <>
  inline void ipp_threshold_lt(const idx<float32> &in, float32 th,
			       idx<float32> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_threshold_gt (in-place)

  //! if input is greater than th, assign <th>
  template <typename T>
  inline void ipp_threshold_gt(idx<T> &in, T th);

  //! This calls ippiThreshold_GT_8u_C1IR
  //! if input is greater than th, assign <th>
  template <>
  inline void ipp_threshold_gt(idx<ubyte> &in, ubyte th);

  //! This calls ippiThreshold_GT_16u_C1IR
  //! if input is greater than th, assign <th>
  template <>
  inline void ipp_threshold_gt(idx<uint16> &in, uint16 th);

  //! This calls ippiThreshold_GT_16s_C1IR
  //! if input is greater than th, assign <th>
  template <>
  inline void ipp_threshold_gt(idx<int16> &in, int16 th);

  //! This calls ippiThreshold_GT_32f_C1IR
  //! if input is greater than th, assign <th>
  template <>
  inline void ipp_threshold_gt(idx<float32> &in, float32 th);

  ////////////////////////////////////////////////////////////////
  // ipp_threshold_gt (not-in-place)

  //! if input is greater than th, assign <th>, otherwise copy <in>
  template <typename T>
  inline void ipp_threshold_gt(const idx<T> &in, T th, idx<T> &out);

  //! This calls ippiThreshold_GT_8u_C1R
  //! if input is greater than th, assign <th>, otherwise copy <in>
  template <>
  inline void ipp_threshold_gt(const idx<ubyte> &in, ubyte th,
			       idx<ubyte> &out);

  //! This calls ippiThreshold_GT_16u_C1R
  //! if input is greater than th, assign <th>, otherwise copy <in>
  template <>
  inline void ipp_threshold_gt(const idx<uint16> &in, uint16 th,
			       idx<uint16> &out);

  //! This calls ippiThreshold_GT_16s_C1R
  //! if input is greater than th, assign <th>, otherwise copy <in>
  template <>
  inline void ipp_threshold_gt(const idx<int16> &in, int16 th,
			       idx<int16> &out);

  //! This calls ippiThreshold_GT_32f_C1R
  //! if input is greater than th, assign <th>, otherwise copy <in>
  template <>
  inline void ipp_threshold_gt(const idx<float32> &in, float32 th,
			       idx<float32> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_threshold_lt (with value, in-place)

  //! if input is less than th, assign value
  template <typename T>
  inline void ipp_threshold_lt(idx<T> &in, T th, T value);

  //! This calls ippiThreshold_LTVal_8u_C1IR
  //! if input is less than th, assign value
  template <>
  inline void ipp_threshold_lt(idx<ubyte> &in, ubyte th, ubyte value);

  //! This calls ippiThreshold_LTVal_16u_C1IR
  //! if input is less than th, assign value
  template <>
  inline void ipp_threshold_lt(idx<uint16> &in, uint16 th, uint16 value);

  //! This calls ippiThreshold_LTVal_16s_C1IR
  //! if input is less than th, assign value
  template <>
  inline void ipp_threshold_lt(idx<int16> &in, int16 th, int16 value);

  //! This calls ippiThreshold_LTVal_32f_C1IR
  //! if input is less than th, assign value
  template <>
  inline void ipp_threshold_lt(idx<float32> &in, float32 th, float32 value);

  ////////////////////////////////////////////////////////////////
  // ipp_threshold_lt (with value, not-in-place)

  //! if input is less than th, assign value, otherwise copy <in>
  template <typename T>
  inline void ipp_threshold_lt(const idx<T> &in, T th, T value,
			       idx<T> &out);

  //! This calls ippiThreshold_LTVal_8u_C1R
  //! if input is less than th, assign value, otherwise copy <in>
  template <>
  inline void ipp_threshold_lt(const idx<ubyte> &in, ubyte th, ubyte value,
			       idx<ubyte> &out);

  //! This calls ippiThreshold_LTVal_16u_C1R
  //! if input is less than th, assign value, otherwise copy <in>
  template <>
  inline void ipp_threshold_lt(const idx<uint16> &in, uint16 th, uint16 value,
			       idx<uint16> &out);

  //! This calls ippiThreshold_LTVal_16s_C1R
  //! if input is less than th, assign value, otherwise copy <in>
  template <>
  inline void ipp_threshold_lt(const idx<int16> &in, int16 th, int16 value,
			       idx<int16> &out);

  //! This calls ippiThreshold_LTVal_32f_C1R
  //! if input is less than th, assign value, otherwise copy <in>
  template <>
  inline void ipp_threshold_lt(const idx<float32> &in, float32 th,
			       float32 value, idx<float32> &out);

  ////////////////////////////////////////////////////////////////
  // ipp_threshold_gt (with value, in-place)

  //! if input is greater than th, assign value
  template <typename T>
  inline void ipp_threshold_gt(idx<T> &in, T th, T value);

  //! This calls ippiThreshold_GTVal_8u_C1IR
  //! if input is greater than th, assign value
  template <>
  inline void ipp_threshold_gt(idx<ubyte> &in, ubyte th, ubyte value);

  //! This calls ippiThreshold_GTVal_16u_C1IR
  //! if input is greater than th, assign value
  template <>
  inline void ipp_threshold_gt(idx<uint16> &in, uint16 th, uint16 value);

  //! This calls ippiThreshold_GTVal_16s_C1IR
  //! if input is greater than th, assign value
  template <>
  inline void ipp_threshold_gt(idx<int16> &in, int16 th, int16 value);

  //! This calls ippiThreshold_GTVal_32f_C1IR
  //! if input is greater than th, assign value
  template <>
  inline void ipp_threshold_gt(idx<float32> &in, float32 th, float32 value);

  ////////////////////////////////////////////////////////////////
  // ipp_threshold_gt (with value, not-in-place)

  //! if input is greater than th, assign value, otherwise copy <in>
  template <typename T>
  inline void ipp_threshold_gt(const idx<T> &in, T th, T value,
			       idx<T> &out);

  //! This calls ippiThreshold_GTVal_8u_C1R
  //! if input is greater than th, assign value, otherwise copy <in>
  template <>
  inline void ipp_threshold_gt(const idx<ubyte> &in, ubyte th, ubyte value,
			       idx<ubyte> &out);

  //! This calls ippiThreshold_GTVal_16u_C1R
  //! if input is greater than th, assign value, otherwise copy <in>
  template <>
  inline void ipp_threshold_gt(const idx<uint16> &in, uint16 th, uint16 value,
			       idx<uint16> &out);

  //! This calls ippiThreshold_GTVal_16s_C1R
  //! if input is greater than th, assign value, otherwise copy <in>
  template <>
  inline void ipp_threshold_gt(const idx<int16> &in, int16 th, int16 value,
			       idx<int16> &out);

  //! This calls ippiThreshold_GTVal_32f_C1R
  //! if input is greater than th, assign value, otherwise copy <in>
  template <>
  inline void ipp_threshold_gt(const idx<float32> &in, float32 th,
			       float32 value, idx<float32> &out);


#endif /* __IPP__ */

} // end namespace ebl

#include "ippops.hpp"

#endif /* IPPOPS_H_ */
