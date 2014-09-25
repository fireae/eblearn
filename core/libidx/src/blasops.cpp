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
#include "blasops.h"

#ifdef __CBLAS__

namespace ebl {

  // idx_copy //////////////////////////////////////////////////////////////////
  
  // specialization for doubles: can use blas versions.
  template <>
  void idx_copy(idx<double> &src, idx<double> &dst) {
    // loop and copy
    idxop_ii(src, dst,
	     // idx0 version
	     { *(dst.idx_ptr()) = *(src.idx_ptr()); },
	     // idx1 version
	     { cblas_dcopy(N1, src.idx_ptr(), src.mod(0), dst.idx_ptr(), 
			   dst.mod(0)); },
	     // contiguous version
	     {

// #ifdef __IPP__
// 	       ipp_set(in, (float) 0);
// #else
	       memset(src.idx_ptr(), 0, src.nelements() * sizeof (float));
		 // TODO: cblas version?
		 //cblas_scopy(N1, src.idx_ptr(), 1, dst.idx_ptr(), 1);
// #endif
	     },
	     // recursive version
	     { idx_bloop2(lsrc, src, double, ldst, dst, double) { 
		 idx_copy(lsrc, ldst); } },
	     // any version
	     { idx_aloop2(isrc, src, double, idst, dst, double) { 
		 *idst = *isrc; }
	     }
	     );
  }

  // specialization for floats: can use blas versions.
  template <>
  void idx_copy(idx<float> &src, idx<float> &dst) {
    // loop and copy
    idxop_ii(src, dst,
	     // idx0 version
	     { *(dst.idx_ptr()) = *(src.idx_ptr()); },
	     // idx1 version
	     { cblas_scopy(N1, src.idx_ptr(), src.mod(0), dst.idx_ptr(), 
			   dst.mod(0)); },
	     // contiguous version
	     {
#ifdef __IPP__
	       ipp_copy(src, dst);
#else
	       cblas_scopy(N1, src.idx_ptr(), 1, dst.idx_ptr(), 1);
#endif
	     },
	     // recursive version
	     { idx_bloop2(lsrc, src, float, ldst, dst, float) { 
		 idx_copy(lsrc, ldst); } },
	     // any version
	     { idx_aloop2(isrc, src, float, idst, dst, float) { 
		 *idst = *isrc; }
	     }
	     );
  }

  // idx_add (in-place) ////////////////////////////////////////////////////////

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

  // idx_dot ///////////////////////////////////////////////////////////////////

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

  // idx_m2dotm1 ///////////////////////////////////////////////////////////////
  
  // matrix-vector multiplication: y <- a.x
  void idx_m2dotm1(idx<double> &a, idx<double> &x, idx<double> &y) {
    check_m2dotm1(a,x,y);
    if (a.mod(0) > a.mod(1)) {
      cblas_dgemv(CblasRowMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(0), x.idx_ptr(), x.mod(0),
		  0.0, y.idx_ptr(), y.mod(0));
    } else {
      cblas_dgemv(CblasColMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(1), x.idx_ptr(), x.mod(0),
		  0.0, y.idx_ptr(), y.mod(0));
    }
  }

  // matrix-vector multiplication: y <- a.x
  void idx_m2dotm1(idx<float> &a, idx<float> &x, idx<float> &y) {
    check_m2dotm1(a,x,y);
    if (a.mod(0) > a.mod(1)) {
      cblas_sgemv(CblasRowMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(0), x.idx_ptr(), x.mod(0),
		  0.0, y.idx_ptr(), y.mod(0));
    } else {
      cblas_sgemv(CblasColMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(1), x.idx_ptr(), x.mod(0),
		  0.0, y.idx_ptr(), y.mod(0));
    }
  }

  // idx_m2dotm1acc ////////////////////////////////////////////////////////////

  // matrix-vector multiplication: y <- y + a.x
  void idx_m2dotm1acc(idx<double> &a, idx<double> &x, idx<double> &y) {
    check_m2dotm1(a,x,y);
    if (a.mod(0) > a.mod(1)) {
      cblas_dgemv(CblasRowMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(0), x.idx_ptr(), x.mod(0),
		  1.0, y.idx_ptr(), y.mod(0));
    } else {
      cblas_dgemv(CblasColMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(1), x.idx_ptr(), x.mod(0),
		  1.0, y.idx_ptr(), y.mod(0));
    }
  }

  // matrix-vector multiplication: y <- y + a.x
  void idx_m2dotm1acc(idx<float> &a, idx<float> &x, idx<float> &y) {
    check_m2dotm1(a,x,y);
    if (a.mod(0) > a.mod(1)) {
      cblas_sgemv(CblasRowMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(0), x.idx_ptr(), x.mod(0),
		  1.0, y.idx_ptr(), y.mod(0));
    } else {
      cblas_sgemv(CblasColMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(1), x.idx_ptr(), x.mod(0),
		  1.0, y.idx_ptr(), y.mod(0));
    }
  }

  // idx_m1extm1 ///////////////////////////////////////////////////////////////

  // vector-vector outer product: a <- x.y'
  void idx_m1extm1(idx<double> &x, idx<double> &y, idx<double> &a) {
    check_m1extm1(x,y,a);
    idx_clear(a);
    cblas_dger(CblasRowMajor, a.dim(0), a.dim(1),
	       1.0, x.idx_ptr(), x.mod(0), y.idx_ptr(), y.mod(0),
	       a.idx_ptr(), a.mod(0));
  }

  // vector-vector outer product: a <- x.y'
  void idx_m1extm1(idx<float> &x, idx<float> &y, idx<float> &a) {
    check_m1extm1(x,y,a);
    idx_clear(a);
    cblas_sger(CblasRowMajor, a.dim(0), a.dim(1),
	       1.0, x.idx_ptr(), x.mod(0), y.idx_ptr(), y.mod(0),
	       a.idx_ptr(), a.mod(0));
  }

  // idx_m1extm1acc ////////////////////////////////////////////////////////////

  // vector-vector outer product: a <- a + x.y'
  void idx_m1extm1acc(idx<double> &x, idx<double> &y, idx<double> &a) {
    check_m1extm1(x,y,a);
    cblas_dger(CblasRowMajor, a.dim(0), a.dim(1),
	       1.0, x.idx_ptr(), x.mod(0), y.idx_ptr(), y.mod(0),
	       a.idx_ptr(), a.mod(0));
  }

  // vector-vector outer product: a <- a + x.y'
  void idx_m1extm1acc(idx<float> &x, idx<float> &y, idx<float> &a) {
    check_m1extm1(x,y,a);
    cblas_sger(CblasRowMajor, a.dim(0), a.dim(1),
	       1.0, x.idx_ptr(), x.mod(0), y.idx_ptr(), y.mod(0),
	       a.idx_ptr(), a.mod(0));
  }

  // norm_columns //////////////////////////////////////////////////////////////

  void norm_columns(idx<double> &m) {
    if ( m.order() != 2) { eblerror("norm_columns: must be an idx2"); }
    idx_eloop1(lm,m,double) {
      double *p = lm.idx_ptr();
      double z = cblas_dnrm2(m.dim(0),p,m.mod(0));
      cblas_dscal(m.dim(0),1/z,p,m.mod(0));
    }
  }

  void norm_columns(idx<float> &m) {
    if ( m.order() != 2) { eblerror("norm_columns: must be an idx2"); }
    idx_eloop1(lm,m,float) {
      float *p = lm.idx_ptr();
      float z = cblas_snrm2(m.dim(0),p,m.mod(0));
      cblas_sscal(m.dim(0),1/z,p,m.mod(0));
    }
  }
  
} // end namespace ebl

#endif /* __CBLAS__ */
