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

// tell header that we are in the libidx scope
#define LIBIDX

#include "config.h"
#include "idx.h"
#include "idxops.h"
#include "ippops.h"
#include "ipp.h"
#include "idxops_ipp.hpp"

#ifndef __TH__
#ifdef __IPP__

namespace ebl {

  // idx_copy //////////////////////////////////////////////////////////////////

  idx_copy_macro(ubyte)
  idx_copy_macro(uint16)
  idx_copy_macro(int16)
  idx_copy_macro(int32)

  template <>
  void idx_copy(const idx<float> &src, idx<float> &dst) {
    if (src.contiguousp() && dst.contiguousp()) {
      ipp_copy(src, dst);
    } else {
      {idx_aloop2(isrc, src, float, idst, dst, float) {*idst = *isrc;}}
    }
  }

  // idx_clear /////////////////////////////////////////////////////////////////

  idx_clear_macro(ubyte)
  idx_clear_macro(uint16)
  idx_clear_macro(int16)
  idx_clear_macro(int32)
  idx_clear_macro(float32)

  // idx_fill //////////////////////////////////////////////////////////////////

  idx_fill_macro(ubyte)
  idx_fill_macro(uint16)
  idx_fill_macro(int16)
  idx_fill_macro(int32)
  idx_fill_macro(float32)

  // idx_minus /////////////////////////////////////////////////////////////////
  
  idx_minus_macro(int16)
  idx_minus_macro(float32)

  // idx_minus_acc /////////////////////////////////////////////////////////////
  
  idx_minus_acc_macro(int16)
  idx_minus_acc_macro(float32)

  // idx_inv ///////////////////////////////////////////////////////////////////

  template<>
  void idx_inv (idx<float32> & inp, idx<float32> & out) {
    if (inp.contiguousp() && out.contiguousp()) {
      ipp_inv(inp, out);
    } else {
      idxiter<float32> pinp; idxiter<float32> pout;
      idx_aloop2_on(pinp,inp,pout,out) { *pout = 1 / *pinp; }
    }
  }

  // idx_add (not-in-place) ////////////////////////////////////////////////////

  idx_add_macro(ubyte)
  idx_add_macro(uint16)
  idx_add_macro(int16)
  idx_add_macro(float32)

  // idx_add (in-place) ////////////////////////////////////////////////////////

  idx_add_macro_in_place(ubyte)
  idx_add_macro_in_place(uint16)
  idx_add_macro_in_place(int16)

#if 0
  template<> void idx_add(idx<float> &src, idx<float> &dst) {
    // loop and copy
    idxop_ii(src, dst,
	     // idx0 version
	     { *(dst.idx_ptr()) = *(src.idx_ptr()) +  *(dst.idx_ptr()); },
	     // idx1 version
	     { idxiter<float> psrc; idxiter<float> pdst;
	     idx_aloop2_on(psrc, src, pdst, dst) { *pdst = *pdst + *psrc; }},
	     // contiguous version
	     { 
#ifdef __IPP__
  	       ipp_add(src, dst);
#else
	       idxiter<float> psrc; idxiter<float> pdst;
	       idx_aloop2_on(psrc, src, pdst, dst) { *pdst = *pdst + *psrc; }
#endif /* __IPP__ */
	     },
	     // recursive version
	     { idx_bloop2(psrc, src, float, pdst, dst, float) {
		 idx_add(psrc, pdst); }},
	     // any version
	     { idxiter<float> psrc; idxiter<float> pdst;
	       idx_aloop2_on(psrc, src, pdst, dst) { *pdst = *pdst + *psrc; }}
	     );
  }
#endif

  // idx_sub ///////////////////////////////////////////////////////////////////

  idx_sub_macro(ubyte)
  idx_sub_macro(uint16)
  idx_sub_macro(int16)
  idx_sub_macro(float32)

  // idx_mul ///////////////////////////////////////////////////////////////////

  idx_mul_macro(ubyte)
  idx_mul_macro(uint16)
  idx_mul_macro(int16)
  idx_mul_macro(float32)
 
  // idx_m2dotm1 ///////////////////////////////////////////////////////////////

  idx_m2dotm1_macro(float32)
  idx_m2dotm1_macro(float64)

  // idx_m2dotm2 ///////////////////////////////////////////////////////////////

  idx_m2dotm2_macro(float32)
  idx_m2dotm2_macro(float64)

  // idx_div ///////////////////////////////////////////////////////////////////

  idx_div_macro(ubyte)
  idx_div_macro(uint16)
  idx_div_macro(int16)
  idx_div_macro(float32)

  // idx_addc //////////////////////////////////////////////////////////////////

  idx_addc_macro(ubyte)
  idx_addc_macro(uint16)
  idx_addc_macro(int16)
  idx_addc_macro(float32)

  // idx_addc_bounded //////////////////////////////////////////////////////////

  idx_addc_bounded_macro(ubyte)
  idx_addc_bounded_macro(uint16)
  idx_addc_bounded_macro(int16)
  idx_addc_bounded_macro(float32)

  // idx_subc_bounded //////////////////////////////////////////////////////////

  idx_subc_bounded_macro(ubyte)
  idx_subc_bounded_macro(uint16)
  idx_subc_bounded_macro(int16)
  idx_subc_bounded_macro(float32)

  // idx_dotc //////////////////////////////////////////////////////////////////

  idx_dotc_macro(ubyte)
  idx_dotc_macro(uint16)
  idx_dotc_macro(int16)
  idx_dotc_macro(float32)

  // idx_dotc_bounded //////////////////////////////////////////////////////////

  idx_dotc_bounded_macro(ubyte)
  idx_dotc_bounded_macro(uint16)
  idx_dotc_bounded_macro(int16)
  idx_dotc_bounded_macro(float32)

  // idx_abs ///////////////////////////////////////////////////////////////////

  idx_abs_macro(int16)
  idx_abs_macro(float32)

  // idx_sqrt //////////////////////////////////////////////////////////////////

  idx_sqrt_macro(ubyte)
  idx_sqrt_macro(uint16)
  idx_sqrt_macro(int16)
  idx_sqrt_macro(float32)

  // idx_exp ///////////////////////////////////////////////////////////////////

  idx_exp_macro(ubyte)
  idx_exp_macro(uint16)
  idx_exp_macro(int16)
  //idx_exp_macro(float32)

  // idx_sum ///////////////////////////////////////////////////////////////////

  idx_sum_macro(ubyte)
  idx_sum_macro(uint16)
  idx_sum_macro(int16)
  idx_sum_macro(float32)

#if 0 // TODO
#if defined(__USE_SSE__) and defined(__OPENMP__)
  template<> float idx_sum(idx<float> & inp, float* out) {
    int n_threads;
    int ith;
    const intg n = inp.nelements();
    float sums[MAX_THREADS];
    if (inp.contiguousp()) {
      sse_4float sum;
      sse_4float *local, *end;
      const int n_frags = floor((float)n / (float)OPENMP_CHUNK);
      const size_t size = OPENMP_CHUNK*sizeof(sse_4float);
      int it;
      sse_4float* i;
      #pragma omp parallel private(sum, end, local, it, i, ith)
      {
	posix_memalign((void**)&local, 16, size);
	n_threads = omp_get_num_threads();
	sum = _mm_setzero_ps();
	ith = omp_get_thread_num();
	#pragma omp for
	for (it = 0; it < n_frags; ++it) {
	  memcpy(local, inp.idx_ptr() + it * OPENMP_CHUNK, size);
	  end = local+OPENMP_CHUNK;
	  for (i = local; i < end; ++i) {
	    sum = _mm_add_ps(sum, *i);
	  }
	}
	free(local);
	sums[ith] = 0;
	for (it = 0; it < 4; ++it)
	  sums[ith] += ((float*)(&sum))[it]; //TODO : could be faster
      }
      float* end2 = inp.idx_ptr() + n; //TODO: sse
      float* i2;
      for (i2 = inp.idx_ptr() + n_frags * OPENMP_CHUNK; i2 < end2; ++i2)
	sums[0] += *i2;
    } else {
      float sum;
      noncontiguous_idxiter<float> it (inp);
      int i;
      #pragma omp parallel private(sum, it, ith, i)
      {
	sum = 0;
	ith = omp_get_thread_num();
	it = noncontiguous_idxiter<float>(inp);
	it += ith / n_threads;
	it.n = std::min(it.i + (ith + 1) / n_threads, it.n);
	#pragma omp for
	for (i = 0; i < n_threads; ++i) {
	  for (it = it; it.notdone(); it.next())
	    sum += *it;
	  sums[ith] = sum;
	}
      }
    }
    float sum = 0;
    for (int i = 0; i < n_threads; ++i)
      sum += sums[i];
    return sum;
  }
#endif
#endif


  /*
  template<> float idx_sum(idx<float> &inp, float *out) {
#ifdef __IPP__
    if (inp.contiguousp()) {
      if (out != NULL) {
	*out = ipp_sum(inp);
	return *out;
      } else {
	return ipp_sum(inp);
      }
    }
#endif
#if USING_STL_ITERS == 0
    float z = 0;
    if (inp.order() == 0) {
      z = inp.get();
      // TODO-0: bug: test cblas sum
      //} else if (inp.order() == 1) {
      // z = cblas_sasum(inp.dim(0), inp.idx_ptr(), inp.mod(0));
      // } else if (inp.contiguousp()) {
      // z = cblas_sasum(inp.nelements(), inp.idx_ptr(), 1);
      //  } else {
      //      idxiter<float> pinp;
      //      idx_aloop1_on(pinp,inp) { z += *pinp; }
      idx_aloop1(pinp,inp,float) { z += (float)(*pinp); }
    }
    if (out != NULL)
      *out = z;
    return z;
#else
    float z = 0;
    ScalarIter<float32> pinp(inp);
    idx_aloop1_on(pinp,inp) { z += (float)(*pinp); }
    if (out != NULL)
      *out = z;
    return z;
#endif
  }


  template<> double idx_sum(idx<double> &inp, double *out) {
#if USING_STL_ITERS == 0
    double z = 0;
    if (inp.order() == 0) {
      z = inp.get();
      // TODO-0: bug: test cblas sum, asum takes the ABSOLUTE values
      //  } else if (inp.order() == 1) {
      //    z = cblas_dasum(inp.dim(0), inp.idx_ptr(), inp.mod(0));
      //  } else if (inp.contiguousp()) {
      //    z = cblas_dasum(inp.nelements(), inp.idx_ptr(), 1);
      //
    } else {
      idx_aloop1(pinp,inp,double) {
    	z += *pinp; }
    }
    if (out != NULL)
      *out = z;
    return z;
#else
    float64 z = 0;
    ScalarIter<float64> pinp(inp);
    idx_aloop1_on(pinp,inp) { z += *pinp; }
    if (out != NULL)
      *out = z;
    return z;
#endif
  }
  */

  // idx_sumabs ////////////////////////////////////////////////////////////////

  idx_sumabs_macro(ubyte)
  idx_sumabs_macro(uint16)
  idx_sumabs_macro(int16)
  idx_sumabs_macro(float32)

  // idx_l2norm ////////////////////////////////////////////////////////////////

  idx_l2norm_macro(ubyte)
  idx_l2norm_macro(uint16)
  idx_l2norm_macro(int16)
  idx_l2norm_macro(float32)

  // idx_mean //////////////////////////////////////////////////////////////////

  idx_mean_macro(ubyte)
  idx_mean_macro(uint16)
  idx_mean_macro(int16)
  idx_mean_macro(float32)

  // idx_dot ///////////////////////////////////////////////////////////////////

#ifdef __IPP_DOT__
  idx_dot_macro(ubyte)
  idx_dot_macro(byte)
  idx_dot_macro(uint16)
  idx_dot_macro(int16)
  idx_dot_macro(uint32)
  idx_dot_macro(int32)
#endif

#ifdef __CBLAS__
  double idx_dot(idx<float32> &i1, idx<float32> &i2) {
    idxop_ii(i1, i2,
	     // idx0 version
	     { return *(i1.idx_ptr()) * *(i2.idx_ptr()); },
	     // idx1 version
	     { return cblas_sdot(N1, i1.idx_ptr(), i1.mod(0), i2.idx_ptr(), 
				 i2.mod(0)); },
	     // contiguous version
	     {
#if defined(__IPP__) and defined(__IPP_DOT__)
	       return ipp_dot(i1, i2);
#else
	       return cblas_sdot(N1, i1.idx_ptr(), 1, i2.idx_ptr(), 1);
#endif
	     },
	     // recursive version
	     { float64 z = 0; idx_bloop2(li1, i1, float, li2, i2, float) { 
		 z += (float64)idx_dot(li1, li2); } return z; },
	     // any version
	     { float64 z = 0;
	       //     idxiter<float> ii1; idxiter<float> ii2;
	       //     idx_aloop2_on(ii1, i1, ii2, i2) { z += (*ii1)*(*ii2); }
	       idx_aloop2(ii1, i1, float, ii2, i2, float) { 
		 z += ((float64)(*ii1))*((float64)(*ii2)); }
	       return z; }
	     );
  }

  double idx_dot(idx<double> &i1, idx<double> &i2) {
    idxop_ii(i1, i2,
	     // idx0 version
	     { return *(i1.idx_ptr()) * *(i2.idx_ptr()); },
	     // idx1 version
	     { return cblas_ddot(N1, i1.idx_ptr(), i1.mod(0), i2.idx_ptr(), 
				 i2.mod(0)); },
	     // contiguous version
	     { return cblas_ddot(N1, i1.idx_ptr(), 1, i2.idx_ptr(), 1); },
	     // recursive version
	     { double z = 0; idx_bloop2(li1, i1, double, li2, i2, double) { 
		 z += idx_dot(li1, li2); } return z; },
	     // any version
	     { double z = 0;
	       //     idxiter<double> ii1; idxiter<double> ii2;
	       //     idx_aloop2_on(ii1, i1, ii2, i2) { z += (*ii1)*(*ii2); }
	       idx_aloop2(ii1, i1, double, ii2, i2, double) { 
		 z += (*ii1)*(*ii2); }
	       return z; }
	     );
  }
#else
#if defined(__IPP__) && defined(__IPP_DOT__)
  idx_dot_macro(float32)
#endif
#endif

  // idx_max ///////////////////////////////////////////////////////////////////

  idx_max_macro(ubyte)
  idx_max_macro(uint16)
  idx_max_macro(int16)
  idx_max_macro(float32)

  // idx_min ///////////////////////////////////////////////////////////////////

  idx_min_macro(ubyte)
  idx_min_macro(uint16)
  idx_min_macro(int16)
  idx_min_macro(float32)

  // idx_indexmax //////////////////////////////////////////////////////////////

  idx_indexmax_macro(ubyte)
  idx_indexmax_macro(uint16)
  idx_indexmax_macro(int16)
  idx_indexmax_macro(float32)

  // idx_indexmin //////////////////////////////////////////////////////////////

  idx_indexmin_macro(ubyte)
  idx_indexmin_macro(uint16)
  idx_indexmin_macro(int16)
  idx_indexmin_macro(float32)

  // idx_max between 2 idx's (in-place) ////////////////////////////////////////

  idx_maxevery_macro(ubyte)
  idx_maxevery_macro(uint16)
  idx_maxevery_macro(int16)
  idx_maxevery_macro(float32)

  // idx_max between 2 idx's (not-in-place) ////////////////////////////////////

  idx_maxevery2_macro(ubyte)
  idx_maxevery2_macro(uint16)
  idx_maxevery2_macro(int16)
  idx_maxevery2_macro(float32)

  // idx_sqrdist (in-place) ////////////////////////////////////////////////////

  idx_sqrdist_macro(ubyte)
  idx_sqrdist_macro(uint16)
  idx_sqrdist_macro(int16)
  idx_sqrdist_macro(float32)

  // idx_sqrdist (with out idx0) ///////////////////////////////////////////////

  idx_sqrdist2_macro(ubyte)
  idx_sqrdist2_macro(uint16)
  idx_sqrdist2_macro(int16)
  idx_sqrdist2_macro(float32)

  // idx_threshold (in-place) //////////////////////////////////////////////////

  idx_threshold_in_place_macro(ubyte)
  idx_threshold_in_place_macro(uint16)
  idx_threshold_in_place_macro(int16)
  idx_threshold_in_place_macro(float32)

  // idx_threshold (not-in-place) //////////////////////////////////////////////

  idx_threshold_macro(ubyte)
  idx_threshold_macro(uint16)
  idx_threshold_macro(int16)
  idx_threshold_macro(float32)

  // idx_threshold (with value, in-place) //////////////////////////////////////

  idx_threshold_in_place_val_macro(ubyte)
  idx_threshold_in_place_val_macro(uint16)
  idx_threshold_in_place_val_macro(int16)
  idx_threshold_in_place_val_macro(float32)

  // idx_threshold (with value, not-in-place) //////////////////////////////////

  idx_threshold_val_macro(ubyte)
  idx_threshold_val_macro(uint16)
  idx_threshold_val_macro(int16)
  idx_threshold_val_macro(float32)
  
} // end namespace ebl

#endif /* __IPP __ */

#endif /* __TH__ */
