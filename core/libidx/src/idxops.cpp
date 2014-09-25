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

// tell header that we are in the libidx scope
#define LIBIDX

#include "config.h"
#include "idxops.h"

namespace ebl {

// simple operations /////////////////////////////////////////////////////////

#define idxop_ii(i1,i2,op_idx0, op_idx1, op_contig, op_recursive, op_any) { \
    intg N1=(i1).nelements();						\
    intg N2 =(i2).nelements();						\
    if (N1 != N2) {							\
      eblwarn( "incompatible idxs: " << i1 << " and " << i2 << endl);	\
      eblerror("idx_op: idxs have different number of elements\n"); }	\
    if ( ((i1).order() == 0) && ((i2).order() == 0) ) {			\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx0;								\
    } else if ( (i1).contiguousp() && (i2).contiguousp() ) {		\
      /* they are both contiguous: call the stride 1 routine */		\
      op_contig;							\
    } else if ( ((i1).order() == 1) && ((i2).order() == 1) ) {		\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx1;								\
    } else if ( same_dim((i1).spec,(i2).spec) ) {			\
      op_recursive;							\
    } else {								\
      /* else, they don't have the same structure: do it "by hand".	\
	 This is slower */						\
      op_any;								\
    }									\
  }

#define idxop_i(i,op_idx0, op_idx1, op_contig, op_recursive) {		\
    if ((i).order() == 0) {						\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx0;								\
    } else if ((i).contiguousp()) {					\
      /* they are both contiguous: call the stride 1 routine */		\
      op_contig;							\
    } else if ((i).order() == 1) {					\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx1;								\
    } else {                                                            \
      op_recursive;							\
    }									\
  }

#define idxop_simple_ii(i1,i2,op_idx0, op_idx1, op_contig,		\
			op_recursive, op_any) {				\
    intg N1=(i1).nelements();						\
    intg N2 =(i2).nelements();						\
    if (N1 != N2) {							\
      eblwarn( "incompatible idxs: " << i1 << " and " << i2 << endl);	\
      eblerror("idx_op: idxs have different number of elements\n"); }	\
    if ( ((i1).order() == 0) && ((i2).order() == 0) ) {			\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx0;								\
    } else if ( (i1).contiguousp() && (i2).contiguousp() ) {		\
      /* they are both contiguous: call the stride 1 routine */		\
      op_contig;							\
    } else if ( ((i1).order() == 1) && ((i2).order() == 1) ) {		\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx1;								\
    } else if ( same_dim((i1).spec,(i2).spec) ) {			\
      op_recursive;							\
    } else {								\
      /* else, they don't have the same structure: do it "by hand".	\
	 This is slower */						\
      op_any;								\
    }									\
  }

// size compatibility checking macros ////////////////////////////////////////

#define check_m2dotm1(m, x, y) {			\
    idx_checkorder3(m, 2, x, 1, y, 1);			\
    idx_checkdim2(y, 0, m.dim(0), x, 0, m.dim(1));	\
  }

#define check_m1extm1(x, y, m) {			\
    idx_checkorder3(m, 2, x, 1, y, 1);			\
    idx_checkdim2(y, 0, m.dim(1), x, 0, m.dim(0));	\
  }

// idx_copy //////////////////////////////////////////////////////////////////

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

// idx_exp ///////////////////////////////////////////////////////////////////

template <> void idx_exp(idx<float> &m) {
  idx_aloopf1(i, m, float32, { *i = expf(*i); });
}

template <> void idx_exp(idx<float64> &m) {
  idx_aloopf1(i, m, float64, { *i = exp(*i); });
}

// idx_power /////////////////////////////////////////////////////////////////

#ifndef __TH__
//disabling for TH
template<> void idx_power(idx<float>& in, float p, idx<float>& out) {
  idx_aloopf2(pin, in, float, pout, out, float, {
      *pout = powf(*pin, p);
    });
}

template<> void idx_power(idx<float64>& in, float64 p, idx<float64>& out) {
  idx_aloopf2(pin, in, float64, pout, out, float64, {
      *pout = pow(*pin, p); });
}
#endif

// idx_sum ///////////////////////////////////////////////////////////////////

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

// idx_std_normalize /////////////////////////////////////////////////////////

template<> void idx_std_normalize(idx<ubyte> & in, idx<ubyte> & out,
                                  ubyte* mean) {
  eblerror("idx_std_normalize<ubyte>: makes no sense with an unsigned type");
}

template<> void idx_std_normalize(idx<uint16> & in, idx<uint16> & out,
                                  uint16* mean) {
  eblerror("idx_std_normalize<uint16>: makes no sense with an unsigned type");
}

template<> void idx_std_normalize(idx<uint32> & in, idx<uint32> & out,
                                  uint32* mean) {
  eblerror("idx_std_normalize<uint32>: makes no sense with an unsigned type");
}

template<> void idx_std_normalize(idx<float32> &in, idx<float32> &out,
                                  float32 *mean_) {
#ifdef __IPP__
  if (in.contiguousp() && out.contiguousp()) {
    ipp_std_normalize(in, out, mean_);
    return;
  }
#endif
  float32 mean = mean_ ? *mean_ : (float32) idx_mean(in);
  idx_addc(in, -mean, out); // remove mean
  float32 sumsq = idx_sumsqr(out);
  if (sumsq != 0) {
    float32 coeff = sqrt(sumsq / out.nelements()); // std deviation
    idx_dotc(out, 1 / coeff, out);
  }
}

// idx_m2squdotm1 ////////////////////////////////////////////////////////////

// square matrix-vector multiplication: Yi = sum((Aij)^2 * Xj)
void idx_m2squdotm1(idx<double> &a, idx<double> &x, idx<double> &y) {
  check_m2dotm1(a,x,y);
  idx_bloop2(la,a,double, ly,y,double) {
    double *pa = la.idx_ptr(); intg amod = la.mod(0);
    double *px =  x.idx_ptr(); intg xmod = x.mod(0);
    double *py = ly.idx_ptr();
    // we don't use bloop for efficiency
    *py = 0;
    for(intg i=0; i<la.dim(0); i++) {
      *py += (*pa)*(*pa)*(*px);
      pa += amod; px += xmod;
    }
  }
}

// square matrix-vector multiplication: Yi = sum((Aij)^2 * Xj)
void idx_m2squdotm1(idx<float> &a, idx<float> &x, idx<float> &y) {
  check_m2dotm1(a,x,y);
  idx_bloop2(la,a,float, ly,y,float) {
    float *pa = la.idx_ptr(); intg amod = la.mod(0);
    float *px =  x.idx_ptr(); intg xmod = x.mod(0);
    float *py = ly.idx_ptr();
    // we don't use bloop for efficiency
    *py = 0;
    for(intg i=0; i<la.dim(0); i++) {
      *py += (*pa)*(*pa)*(*px);
      pa += amod; px += xmod;
    }
  }
}

// idx_m2squdotm1acc /////////////////////////////////////////////////////////

// square matrix-vector multiplication: Yi += sum((Aij)^2 * Xj)
void idx_m2squdotm1acc(idx<double> &a, idx<double> &x, idx<double> &y) {
  check_m2dotm1(a,x,y);
  idx_bloop2(la,a,double, ly,y,double) {
    double *pa = la.idx_ptr(); intg amod = la.mod(0);
    double *px =  x.idx_ptr(); intg xmod = x.mod(0);
    double *py = ly.idx_ptr();
    // we don't use bloop for efficiency
    for(intg i=0; i<la.dim(0); i++) {
      *py += (*pa)*(*pa)*(*px);
      pa += amod; px += xmod;
    }
  }
}

// square matrix-vector multiplication: Yi += sum((Aij)^2 * Xj)
void idx_m2squdotm1acc(idx<float> &a, idx<float> &x, idx<float> &y) {
  check_m2dotm1(a,x,y);
  idx_bloop2(la,a,float, ly,y,float) {
    float *pa = la.idx_ptr(); intg amod = la.mod(0);
    float *px =  x.idx_ptr(); intg xmod = x.mod(0);
    float *py = ly.idx_ptr();
    // we don't use bloop for efficiency
    for(intg i=0; i<la.dim(0); i++) {
      *py += (*pa)*(*pa)*(*px);
      pa += amod; px += xmod;
    }
  }
}

// idx_m1sqextm1 /////////////////////////////////////////////////////////////

// Aij = Xi * Yj^2
void idx_m1squextm1(idx<double> &x, idx<double> &y, idx<double> &a) {
  check_m1extm1(x,y,a);
  idx_bloop2(lx,x,double, la,a,double) {
    // TODO: change to aloop
    idx_bloop2(ly,y,double, lla,la,double) {
      //        *lla = (*lx)*(*ly)*(*ly);
      lla.set(lx.get() * ly.get() * ly.get());
    }
  }
}

//Aij = Xi * Yj^2
void idx_m1squextm1(idx<float> &x, idx<float> &y, idx<float> &a) {
  check_m1extm1(x,y,a);
  idx_bloop2(lx,x,float, la,a,float) {
    idx_bloop2(ly,y,float, lla,la,float) {
      //       *lla = (*lx)*(*ly)*(*ly);
      lla.set(lx.get() * ly.get() * ly.get());
    }
  }
}

// idx_m1sqextm1acc //////////////////////////////////////////////////////////

// Aij += Xi * Yj^2
void idx_m1squextm1acc(idx<double> &x, idx<double> &y, idx<double> &a) {
  check_m1extm1(x,y,a);
  idx_bloop2(lx,x,double, la,a,double) {
    idx_bloop2(ly,y,double, lla,la,double) {
      //      *lla += (*lx)*(*ly)*(*ly);
      lla.set(lla.get() + lx.get() * ly.get() * ly.get());
    }
  }
}

// Aij += Xi * Yj^2
void idx_m1squextm1acc(idx<float> &x, idx<float> &y, idx<float> &a) {
  check_m1extm1(x,y,a);
  idx_bloop2(lx,x,float, la,a,float) {
    idx_bloop2(ly,y,float, lla,la,float) {
      //        *lla += (*lx)*(*ly)*(*ly);
      lla.set(lla.get() + lx.get() * ly.get() * ly.get());
      // TODO-0: BUG: this doesn't seem to work:
      // *(lla.ptr()) += lx.get() * ly.get() * ly.get();
    }
  }
}

// strings_to_idx ////////////////////////////////////////////////////////////

idx<ubyte> strings_to_idx(idx<const char *> &strings) {
  // determine max length of strings
  size_t max = 0;
  { idx_bloop1(s, strings, const char *)
	max = (std::max)(max, strlen(s.get())); }
  // allocate output idx
  idx<ubyte> out(strings.dim(0), max + 1);
  // copy classes strings
  idx_clear(out);
  idx_bloop2(s, strings, const char *, o, out, ubyte) {
    memcpy(o.idx_ptr(), s.get(), strlen(s.get()) * sizeof (ubyte));
  }
  return out;
}

// idx_modulo ////////////////////////////////////////////////////////////////

template <> void idx_modulo(idx<double> &m, double mod) {
  idx_aloopf1(e, m, double, { *e = fmod(*e, mod); });
}

template <> void idx_modulo(idx<long double> &m, long double mod) {
  idx_aloopf1(e, m, long double, { *e = fmod(*e, mod); });
}

template <> void idx_modulo(idx<float> &m, float mod) {
  idx_aloopf1(e, m, float, { *e = fmod(*e, mod); });
}

template <> ubyte idx_max(idx<ubyte> &m) {
  ubyte v = *(m.idx_ptr());
  idx_aloopf1(i, m, ubyte, { if (*i > v) v = *i; });
  return v;
}

} // end namespace ebl
