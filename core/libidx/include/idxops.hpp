/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
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

#ifndef IDXOPS_HPP
#define IDXOPS_HPP

namespace ebl {

////////////////////////////////////////////////////////////////
// idx_copy

// generic copy for two different types.
//template<class T1, class T2> void idx_copy(idx<T1> &src, idx<T2> &dst) {
////  idxiter<T1> isrc;
////  idxiter<T2> idst;
////  idx_aloop2_on(isrc, src, idst, dst) { *idst = (T2)(*isrc); }
//  {idx_aloop2(isrc, src, T1, idst, dst, T2) { *idst = (T2)(*isrc); }}
//}

//// generic copy for two different types.
//template<class T1, class T2> void idx_copy(idx<T1> &src, idx<T2> &dst) {
////  idxiter<T1> isrc;
////  idxiter<T2> idst;
////  idx_aloop2_on(isrc, src, idst, dst) { *idst = (T2)(*isrc); }
//  {idx_aloop2(isrc, src, T1, idst, dst, T2) { *idst = (T2)(*isrc); }}
//}


// generic copy for the same type.
template <typename T> void idx_copy(const idx<T> &src, idx<T> &dst) {
  idx_checknelems2_all(src, dst);
  // loop and copy
  if (src.order() == 0 && dst.order() == 0)
    *(dst.idx_ptr()) = *(src.idx_ptr());
  else if (src.contiguousp() && dst.contiguousp()) {
    /* they are both contiguous: call the stride 1 routine */
    memcpy(dst.idx_ptr(), src.idx_ptr(), src.nelements() * sizeof(T));
  } else {
    // else, they don't have the same structure: do it "by hand".
    // This is slower
    {idx_aloop2(isrc, src, T, idst, dst, T) { *idst = *isrc; }}
  }
}

template<class T1, class T2> void idx_copy(const idx<T1> &src, idx<T2> &dst){
  idx_checknelems2_all(src, dst);
  // loop and copy
  idx_aloop2(isrc, src, T1, idst, dst, T2) { *idst = (T2)(*isrc); }
}

template<class T1, class T2> idx<T1> idx_copy(const idx<T2> &src){
  idx<T1> dst(src.get_idxdim());
  idx_copy(src, dst);
  return dst;
}

template <typename T> idx<T> idx_copy(const idx<T> &src){
  idx<T> dst(src.get_idxdim());
  idx_copy(src, dst);
  return dst;
}

template<class T1, class T2>
void idx_copy_clip(const idx<T1> &src, idx<T2> &dst) {
  idx_checknelems2_all(src, dst);
  // loop and copy
  idx_aloop2(isrc, src, T1, idst, dst, T2) {
    *idst = saturate(*isrc, T2);
  }
}

////////////////////////////////////////////////////////////////
// idx_clear

template <typename T> void idx_clear(idx<T> &inp) {
  if (inp.contiguousp())
    memset(inp.idx_ptr(), 0, inp.nelements() * sizeof (T));
  else
    idx_aloopf1(pinp, inp, T, { *pinp = 0; });
}

////////////////////////////////////////////////////////////////
// idx_fill

template<class T, class T2> void idx_fill(idx<T> & inp, T2 v) {
  idx_fill(inp, saturate(v, T));
}

template <typename T> void idx_fill(idx<T> &inp, T v) {
  idx_aloopf1(it, inp, T, {*it = v;})
      }

template <typename T> void idx_fill_index(idx<T> &inp) {
  T i = 0;
  idx_aloopf1(it, inp, T, {*it = i++;})
      }

////////////////////////////////////////////////////////////////
// idx_shuffle_*

// TODO: can n random swaps be as random? (it would be more efficient)
template <typename T> void idx_shuffle(idx<T> &in_, intg d, idx<T> *out_) {
  if (!drand_ini)
    eblwarn( "Warning: random not initialized, call dynamic_init_drand()"
             << std::endl);
  // if out exists, use it for output, otherwise create a temporary buffer
  // and put output back into in.
  idx<T> in, out;
  if (out_) { // use in_ as input and out_ as output
    // check that in_ and out_ are different
    if (&in_ == out_)
      eblerror("input and output idx should be different");
    idx_checknelems2_all(in_, *out_);
    in = in_;
    out = *out_;
  } else { // otherwise, use in_ as output and a copy of _in as input
    idxdim indims(in_);
    in = idx<T>(indims);
    idx_copy(in_, in);
    out = in_;
  }
  // for each element of in, put it randomly in out.
  // if there is a collision, loop until the next available slot
  idx<T> tmpi, tmpo;
  idx<bool> assigned(in.dim(d));
  idx_fill(assigned, false);
  intg pos;
  for (intg i = 0; i < in.dim(d); ++i) {
    pos = (intg) drand(0, in.dim(d) - 1);
    if (assigned.get(pos)) { // if already assigned, loop until free slot
      for (intg j = pos + 1; j != pos; ++j) {
        if (j >= in.dim(d)) j = 0;
        if (j == pos) eblerror("idx_shuffle: no available slot");
        if (!assigned.get(j)) {
          pos = j;
          break ;
        }
      }
    }
    // copy ith element of in into pos^th element of out
    tmpi = in.select(d, i);
    tmpo = out.select(d, pos);
    idx_copy(tmpi, tmpo);
    assigned.set(true, pos);
  }
}

// TODO: can n random swaps be as random? (it would be more efficient)
template<class T1, class T2>
void idx_shuffle_together(idx<T1> &in1_, idx<T2> &in2_, intg d,
                          idx<T1> *out1_, idx<T2> *out2_) {
  if (!drand_ini)
    eblwarn( "Warning: random not initialized, call dynamic_init_drand()"
             << std::endl);
  idx_checkdim2_all(in1_, in2_, d); // size of dim d must match of in1 and in2
  // if out exists, use it for output, otherwise create a temporary buffer
  // and put output back into in.
  idx<T1> in1, out1;
  idx<T2> in2, out2;
  if (out1_) { // use in_ as input and out_ as output
    // check that in_ and out_ are different
    if (&in1_ == out1_)
      eblerror("input and output idx should be different");
    idx_checknelems2_all(in1_, *out1_);
    in1 = in1_;
    out1 = *out1_;
  } else { // otherwise, use in_ as output and a copy of _in as input
    idxdim indims(in1_);
    in1 = idx<T1>(indims);
    idx_copy(in1_, in1);
    out1 = in1_;
  }
  if (out2_) { // use in_ as input and out_ as output
    if (&in2_ == out2_)
      eblerror("input and output idx should be different");
    idx_checknelems2_all(in2_, *out2_);
    in2 = in2_;
    out2 = *out2_;
  } else { // otherwise, use in_ as output and a copy of _in as input
    idxdim indims(in2_);
    in2 = idx<T2>(indims);
    idx_copy(in2_, in2);
    out2 = in2_;
  }
  // for each element of in, put it randomly in out.
  // if there is a collision, loop until the next available slot
  idx<T1> tmpi1, tmpo1;
  idx<T2> tmpi2, tmpo2;
  idx<bool> assigned(in1.dim(d));
  idx_fill(assigned, false);
  intg pos;
  for (intg i = 0; i < in1.dim(d); ++i) {
    pos = (intg) drand(0, in1.dim(d) - 1);
    if (assigned.get(pos)) { // if already assigned, loop until free slot
      for (intg j = pos + 1; j != pos; ++j) {
        if (j >= in1.dim(d)) j = 0;
        if (j == pos) eblerror("no available slot");
        if (!assigned.get(j)) {
          pos = j;
          break ;
        }
      }
    }
    // copy ith element of in into pos^th element of out
    tmpi1 = in1.select(d, i);
    tmpo1 = out1.select(d, pos);
    idx_copy(tmpi1, tmpo1);
    tmpi2 = in2.select(d, i);
    tmpo2 = out2.select(d, pos);
    idx_copy(tmpi2, tmpo2);
    assigned.set(true, pos);
  }
}

// TODO: can n random swaps be as random? (it would be more efficient)
template<class T1, class T2, class T3>
void idx_shuffle_together(idx<T1> &in1_, idx<T2> &in2_, idx<T3> &in3_,
                          intg d,
                          idx<T1> *out1_, idx<T2> *out2_, idx<T3> *out3_) {
  if (!drand_ini)
    eblwarn( "Warning: random not initialized, call dynamic_init_drand()"
             << std::endl);
  // size of dim d must match of in1 and in2 and in3
  idx_checkdim3_all(in1_, in2_, in3_, d);
  // if out exists, use it for output, otherwise create a temporary buffer
  // and put output back into in.
  idx<T1> in1, out1;
  idx<T2> in2, out2;
  idx<T3> in3, out3;
  if (out1_) { // use in_ as input and out_ as output
    if (&in1_ == out1_)
      eblerror("input and output idx should be different");
    idx_checknelems2_all(in1_, *out1_);
    in1 = in1_;
    out1 = *out1_;
  } else { // otherwise, use in_ as output and a copy of _in as input
    idxdim indims(in1_);
    in1 = idx<T1>(indims);
    idx_copy(in1_, in1);
    out1 = in1_;
  }
  if (out2_) { // use in_ as input and out_ as output
    if (&in2_ == out2_)
      eblerror("input and output idx should be different");
    idx_checknelems2_all(in2_, *out2_);
    in2 = in2_;
    out2 = *out2_;
  } else { // otherwise, use in_ as output and a copy of _in as input
    idxdim indims(in2_);
    in2 = idx<T2>(indims);
    idx_copy(in2_, in2);
    out2 = in2_;
  }
  if (out3_) { // use in_ as input and out_ as output
    if (&in3_ == out3_)
      eblerror("input and output idx should be different");
    idx_checknelems2_all(in3_, *out3_);
    in3 = in3_;
    out3 = *out3_;
  } else { // otherwise, use in_ as output and a copy of _in as input
    idxdim indims(in3_);
    in3 = idx<T3>(indims);
    idx_copy(in3_, in3);
    out3 = in3_;
  }
  // for each element of in, put it randomly in out.
  // if there is a collision, loop until the next available slot
  idx<T1> tmpi1, tmpo1;
  idx<T2> tmpi2, tmpo2;
  idx<T3> tmpi3, tmpo3;
  idx<bool> assigned(in1.dim(d));
  idx_fill(assigned, false);
  intg pos;
  for (intg i = 0; i < in1.dim(d); ++i) {
    pos = (intg) drand(0, in1.dim(d) - 1);
    if (assigned.get(pos)) { // if already assigned, loop until free slot
      for (intg j = pos + 1; j != pos; ++j) {
        if (j >= in1.dim(d)) j = 0;
        if (j == pos) eblerror("no available slot");
        if (!assigned.get(j)) {
          pos = j;
          break ;
        }
      }
    }
    // copy ith element of in into pos^th element of out
    tmpi1 = in1.select(d, i);
    tmpo1 = out1.select(d, pos);
    idx_copy(tmpi1, tmpo1);
    tmpi2 = in2.select(d, i);
    tmpo2 = out2.select(d, pos);
    idx_copy(tmpi2, tmpo2);
    tmpi3 = in3.select(d, i);
    tmpo3 = out3.select(d, pos);
    idx_copy(tmpi3, tmpo3);
    assigned.set(true, pos);
  }
}

////////////////////////////////////////////////////////////////////////
// idx_minus

template <typename T> void idx_minus(idx<T> &inp, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, { *pout = - *pinp; });
}

////////////////////////////////////////////////////////////////////////
// idx_minus_acc

template <typename T> void idx_minus_acc(idx<T> &inp, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, { *pout += - *pinp; });
}

////////////////////////////////////////////////////////////////////////
// idx_inv

template <typename T> void idx_inv(idx<T> &inp, idx<T> &out) {
#ifdef __DEBUG__
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      if (*pinp == 0) eblerror("division by zero");
      *pout = 1 / *pinp;
    });
#else
  idx_aloopf2(pinp, inp, T, pout, out, T, { *pout = 1 / *pinp; });
#endif
}

////////////////////////////////////////////////////////////////////////
// idx_add

template <typename T> void idx_add(idx<T> &in, idx<T> &out) {
  idx_aloopf2 (pin, in, T, pout, out, T, { *pout = *pout + *pin; });
}

template<typename T> void idx_add(idx<T> &i1, idx<T> &i2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, { *pout = *pi1 + *pi2; });
}

////////////////////////////////////////////////////////////////////////
// idx_sub

template <typename T> void idx_sub(idx<T> &i1, idx<T> &i2) {
  idx_aloopf2(pi1, i1, T, pi2, i2, T, { *pi1 = *pi1 - *pi2; });
}

template <typename T> void idx_sub(idx<T> &i1, idx<T> &i2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, {*pout = *pi1 - *pi2; });
}

template <typename T> void idx_subacc(idx<T> &i1, idx<T> &i2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, {*pout += *pi1 - *pi2; });
}

////////////////////////////////////////////////////////////////////////
// idx_spherical_sub

template <typename T> void idx_spherical_sub(idx<T> &i1, idx<T> &i2, idx<T> &out){
  T tmp, tmpabs, tmpabs2;
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, {
      tmp = *pi1 - *pi2;
      tmpabs = std::abs(tmp);
      tmpabs2 = TWOPI - tmpabs;
      if (tmpabs > tmpabs2) {
        if (tmp < 0)
          tmp = tmpabs2;
        else
          tmp = -tmpabs2;
      }
      *pout = tmp;
    });
}

////////////////////////////////////////////////////////////////////////
// idx_spherical_add

template <typename T> void idx_spherical_add(idx<T> &i1, idx<T> &i2, idx<T> &out){
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, {
      *pout = std::min(TWOPI - *pi1, *pi1) + std::min(TWOPI - *pi2, - *pi2);
    });
}

////////////////////////////////////////////////////////////////////////
// idx_mul

template <typename T> void idx_mul(idx<T> &i1, idx<T> &i2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T,
              {*pout = (*pi1) * (*pi2); });
}

template <typename T, typename T2>
void idx_mul(idx<T> &i1, idx<T2> &i2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T2, pout, out, T,
              {*pout = (*pi1) * (T) (*pi2); });
}

template <typename T> void idx_mulacc(idx<T> &i1, idx<T> &i2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T,
              {*pout += (*pi1) * (*pi2); });
}

////////////////////////////////////////////////////////////////////////
// idx_div

template <typename T> void idx_div(idx<T> &i1, idx<T> &i2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, {
      *pout = (*pi1) / (*pi2);
    });
  // since we cannot put a ifdef in a macro, we repeat the code here
#ifdef __DEBUG__
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, {
      if (*pi2 == 0)
        eblerror("division by zero");
      *pout = (*pi1) / (*pi2);
    });
#endif
}

////////////////////////////////////////////////////////////////////////
// idx_addc

template<class T, class T2> void idx_addc(idx<T> & inp, T2 c, idx<T> & out) {
  idx_addc(inp, saturate(c, T), out);
}

template <typename T> void idx_addc(idx<T> &inp, T c, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, { *pout = *pinp + c; });
}

template <typename T> void idx_addc(idx<T> &inp, T c) {
  idx_aloopf1(pinp, inp, T, { *pinp += c; });
}

////////////////////////////////////////////////////////////////////////
// idx_addc_bounded

template<class T, class T2>
void idx_addc_bounded(idx<T> & inp, T2 c, idx<T> & out) {
  idx_addc_bounded(inp, saturate(c, T), out);
}


template <typename T> void idx_addc_bounded(idx<T> &inp, T c, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      *pout = saturate(*pinp + c, T);
    });
}

////////////////////////////////////////////////////////////////////////
// idx_subc_bounded

template<class T, class T2>
void idx_subc_bounded(idx<T> &inp, T2 c, idx<T> &out) {
  idx_subc_bounded(inp, saturate(c, T), out);
}


template <typename T> void idx_subc_bounded(idx<T> &inp, T c, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      *pout = saturate(*pinp - c, T);
    });
}

////////////////////////////////////////////////////////////////////////
// idx_addcacc

template<class T, class T2>
void idx_addcacc(idx<T> & inp, T2 c, idx<T> & out) {
  idx_addcacc(inp, saturate(c, T), out);
}

template <typename T> void idx_addcacc(idx<T> &inp, T c, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, { *pout += *pinp + c; });
}

////////////////////////////////////////////////////////////////////////
// idx_dotc

template<class T, class T2> void idx_dotc(idx<T> &inp, T2 c, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, { *pout = (T)(*pinp * c); });
}

////////////////////////////////////////////////////////////////////////
// idx_dotc_bounded

template<class T, class T2>
void idx_dotc_bounded(idx<T> &inp, T2 c, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      *pout = saturate(*pinp * c, T);
    });
}

////////////////////////////////////////////////////////////////////////
// idx_dotcacc

template<class T, class T2> void idx_dotcacc(idx<T> &inp, T2 c, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, { *pout += (T)(*pinp * c); });
}

////////////////////////////////////////////////////////////////////////
// idx_signdotc

template<class T, class T2>
void idx_signdotc(idx<T> &inp, T2 c, idx<T> &out) {
  idx_digndotc(inp, saturate(c, T), out);
}

template <typename T> void idx_signdotc(idx<T> &inp, T c, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      *pout = (*pinp < 0) ? -c : c;
    });
}

////////////////////////////////////////////////////////////////////////
// idx_signdotcacc

template<class T, class T2>
void idx_signdotcacc(idx<T> &inp, T2 c, idx<T> &out) {
  idx_digndotcacc(inp, saturate(c, T), out);
}

template <typename T> void idx_signdotcacc(idx<T> &inp, T c, idx<T> &out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      *pout += (*pinp < 0) ? -c : c;
    });
}

////////////////////////////////////////////////////////////////////////
// idx_subsquare

template <typename T> void idx_subsquare(idx<T> &i1, idx<T> &i2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, {
      T d = *pi1 - *pi2;
      *pout = d*d;
    });
}

template <typename T> void idx_subsquareacc(idx<T> &i1, idx<T> &i2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, {
      T d = *pi1 - *pi2;
      *pout += d*d;
    });
}

// idx_lincom ////////////////////////////////////////////////////////////////

// not very efficient. There must be a more parallel way of doing this
template <typename T>
void idx_lincomb(idx<T> &i1, T k1, idx<T> &i2, T k2, idx<T> &out) {
  idx_aloopf3(pi1, i1, T, pi2, i2, T, pout, out, T, {
      *pout = k1 * (*pi1) + k2 * (*pi2);
    });
}

// idx_tanh //////////////////////////////////////////////////////////////////

template <typename T> void idx_tanh(idx<T> &inp, idx<T> &out) {
  idx_checknelems2_all(inp, out);
#ifdef __OPENMP__
  T* inptr = inp.idx_ptr();
  T* outptr = out.idx_ptr();
#pragma omp parallel for
  for(int i=0; i< inp.nelements(); ++i) {
    outptr[i] = (T)(tanh((double)inptr[i]));
  }
#else
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      *pout = (T)(tanh((double)*pinp));
    });
#endif
}

// idx_dtanh /////////////////////////////////////////////////////////////////

template <typename T> void idx_dtanh(idx<T> &inp, idx<T> &out) {
  idx_checknelems2_all(inp, out);
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      *pout = (T) (dtanh((double)*pinp));
    });
}

// idx_stdsigmoid ////////////////////////////////////////////////////////////

template <typename T> void idx_stdsigmoid(idx<T> &inp, idx<T> &out) {
  idx_checknelems2_all(inp, out);
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      *pout = (T) (stdsigmoid((double)*pinp));
    });
}

// idx_dstdsigmoid ///////////////////////////////////////////////////////////

template <typename T> void idx_dstdsigmoid(idx<T> &inp, idx<T> &out) {
  idx_checknelems2_all(inp, out);
  idx_aloopf2(pinp, inp, T, pout, out, T, {
      *pout = (T) (dstdsigmoid((double)*pinp));
    });
}

// idx_abs ///////////////////////////////////////////////////////////////////

template <typename T> inline T abs2 (T a) {
  return (T)std::abs((float64)a);
}
template<> inline float abs2 (float a) {
  return std::fabs(a);
}
template<> inline byte abs2 (byte a) {
  if (a >= 0)
    return a;
  return (a == -128) ? 127 : -a;
}
template<> inline int16 abs2 (int16 a) {
  if (a >= 0)
    return a;
  return (a == -32768) ? 32767 : -a;
}

template <typename T> void idx_abs(idx<T>& inp, idx<T>& out) {
  idx_aloopf2(pinp, inp, T, pout, out, T, { *pout = abs2<T>(*pinp); });
}


////////////////////////////////////////////////////////////////////////
// idx_threshold_acc

template<class T, class T2, class T3>
void idx_thresdotc_acc(idx<T>& in, T2 c, T3 th, idx<T>& out) {
  idx_thresdotc_acc(in, saturate(c, T), saturate(th, T), out);
}

template <typename T> void idx_thresdotc_acc(idx<T>& in, T c, T th, idx<T>& out) {
  idx_aloopf2(pin, in, T, pout, out, T, {
      *pout += (*pin < -th) ? -c : (*pin > th) ? c : 0;
    });
}

////////////////////////////////////////////////////////////////////////
// idx_threshold (in-place)

template<class T, class T2> void idx_threshold(idx<T> & in, T2 th) {
  idx_threshold(in, saturate(th, T));
}

template <typename T> void idx_threshold(idx<T>& in, T th) {
  idx_aloopf1(pin, in, T, { if (*pin < th) *pin = th; });
}

template <typename T> void idx_threshold2(idx<T>& in, T th) {
  idx_aloopf1(pin, in, T, { if (*pin > th) *pin = th; });
}

////////////////////////////////////////////////////////////////////////
// idx_threshold (not-in-place)

template<class T, class T2>
void idx_threshold(idx<T> & in, T2 th, idx<T> & out) {
  idx_threshold(in, saturate(th, T), out);
}

template <typename T> void idx_threshold(idx<T>& in, T th, idx<T>& out) {
  idx_checknelems2_all(in, out);
  idx_aloopf2(pin, in, T, pout, out, T, {
      if (*pin < th)
        *pout = th;
      else
	*pout = *pin;
    });
}

////////////////////////////////////////////////////////////////////////
// idx_threshold (in-place, with value)

template<class T, class T2, class T3>
void idx_threshold(idx<T> & in, T2 th, T3 value) {
  idx_threshold(in, (T) th, (T) value);
  //    idx_threshold(in, saturate(th, T), saturate(value, T));
}

template <typename T> void idx_threshold(idx<T>& in, T th, T value) {
  idx_aloopf1(pin, in, T, { if (*pin < th) *pin = value; });
}

////////////////////////////////////////////////////////////////////////
// idx_threshold (not-in-place, with value)

template<class T, class T2, class T3>
void idx_threshold(idx<T> & in, T2 th, T3 value, idx<T> & out) {
  idx_threshold(in, (T) th, (T) value, out);
  //    idx_threshold(in, saturate(th, T), saturate(value, T), out);
}

template <typename T> void idx_threshold(idx<T>& in, T th, T value, idx<T>& out) {
  idx_aloopf2(pin, in, T, pout, out, T, {
      if (*pin < th)
        *pout = value;
      else
        *pout = *pin;
    });
}

////////////////////////////////////////////////////////////////////////
// idx_threshold (with above and below)

template<class T, class T2>
void idx_threshold(idx<T>& in, T th, T2 below, T2 above, idx<T2>& out) {
  idx_checknelems2_all(in, out);
  idx_aloopf2(pin, in, T, pout, out, T2, {
      *pout = (*pin < th) ? below : above;
    });
}

////////////////////////////////////////////////////////////////////////
// idx_sqrt

template <typename T> void idx_sqrt(idx<T>& in, idx<T>& out) {
  idx_checknelems2_all(in, out);
  idx_aloopf2(pin, in, T, pout, out, T, {
      *pout = (T)sqrt((float64)(*pin));
    });
}

////////////////////////////////////////////////////////////////////////
// idx_power

template<class T, class T2> void idx_power(idx<T> & in, T2 p, idx<T> & out) {
  idx_checknelems2_all(in, out);
  idx_power(in, saturate(p, T), out);
}

template <typename T> void idx_power(idx<T>& in, T p, idx<T>& out) {
  idx_checknelems2_all(in, out);
  idx_aloopf2(pin, in, T, pout, out, T, {
      *pout = saturate(pow((double)(*pin), (double)p), T);
    });
}

////////////////////////////////////////////////////////////////////////
// idx_sum
//#ifndef __OPENMP__

// there is a much faster and parallel way
// of doing this using a tree.
template<typename Tout, typename T> Tout idx_sum(idx<T> &inp) {
  Tout z = 0;
  idx_aloopf1(pinp, inp, T, { z += (Tout)(*pinp); });
  return z;
}

// there is a much faster and parallel way
// of doing this using a tree.
template <typename T> T idx_sum(idx<T> &inp, T *out) {
  T z = idx_sum(inp);
  if (out != NULL)
    *out = saturate(z, T);
  return z;
}

// there is a much faster and parallel way
// of doing this using a tree.
template <typename T> T idx_sum(idx<T> &inp) {
  T z = 0;
  idx_aloopf1(pinp, inp, T, { z += (T)(*pinp); });
  return z;
}

// #endif /* ifndef __OPENMP__ */

////////////////////////////////////////////////////////////////////////
// idx_sumabs

// there is a much faster and parallel way
// of doing this using a tree.
template <typename T> float64 idx_sumabs(idx<T> &inp, T *out) {
  float64 z = 0;
  idx_aloopf1(pinp, inp, T, { z += std::abs((float64)(*pinp)); });
  if (out != NULL) {
    *out = saturate(z, T);
    return *out;
  }
  return z;
}

////////////////////////////////////////////////////////////////////////
// idx_sumsqr

// there is a much faster and parallel way
// of doing this using a tree.
template <typename T> float64 idx_sumsqr(idx<T> &inp) {
  float64 z = 0;
  idx_aloopf1(pinp, inp, T, { z += ((float64)(*pinp))*((float64)(*pinp)); });
  return z;
}

////////////////////////////////////////////////////////////////////////
// idx_sumacc

template <typename T> float64 idx_sumacc(idx<T> &inp, idx<T> &acc) {
  // acc must be of order 0.
  if (acc.order() != 0)
    eblerror("expecting an idx0 as output");
  float64 sum = (float64)acc.get() + idx_sum(inp);
  acc.set(saturate(sum, T));
  return sum;
  //return idx_sum(inp, acc.ptr());
}

////////////////////////////////////////////////////////////////////////
// idx_sumabs (with acc)

template <typename T> float64 idx_sumabs(idx<T> &inp, idx<T> &acc) {
  // acc must be of order 0.
  if (acc.order() != 0)
    eblerror("expecting an idx0 as output");
  float64 sum = (float64)acc.get() + idx_sumabs(inp);
  acc.set(saturate(sum, T));
  return sum;
  //return idx_sumabs(inp, acc.ptr());
}

////////////////////////////////////////////////////////////////////////
// idx_l1

template <typename T> float64 idx_l1(idx<T> &m1, idx<T> &m2) {
  idx_checknelems2_all(m1, m2);
  float64 z = 0;
  idx_aloopf2(pm1, m1, T, pm2, m2, T,
              { z += (float64) std::fabs((float64) (*pm1 - *pm2)); });
  return z;
}

////////////////////////////////////////////////////////////////////////
// idx_l2norm

template <typename T> float64 idx_l2norm(idx<T> &in) {
  return sqrt((T) idx_sumsqr(in));
}

template <typename T> float64 idx_l2(idx<T> &in) {
  return sqrt((T) idx_sumsqr(in));
}

template <typename T> float64 idx_l2(idx<T> &m1, idx<T> &m2) {
  idx_checknelems2_all(m1, m2);
  float64 z = 0;
  float64 tmp;
  idx_aloopf2(pm1, m1, T, pm2, m2, T,
              { tmp = *pm1 - (float64) *pm2;
                z += tmp * tmp; });
  return sqrt(z);
}

template <typename T> float64 idx_l2(midx<T> &m1, midx<T> &m2) {
  idx_checknelems2_all(m1, m2);
  float64 z = 0;
  idx_aloopf2(pe1, ((idx<idx<T>*>&) m1), idx<T>*,
              pe2, ((idx<idx<T>*>&) m2), idx<T>*, {
                idx<T> *e1 = *pe1; idx<T> *e2 = *pe2;
                if (e1 && e2)
                  z += idx_l2(*e1, *e2);
                else if (e1 != e2)
                  eblerror("expected both matrices to be empty in " << m1
                           << " and " << m2);
              });
  return sqrt(z);
}

template <typename T> float64 idx_l2common(midx<T> &m1, midx<T> &m2) {
  idx_checknelems2_all(m1, m2);
  float64 z = 0;
  idx_aloopf2(pe1, ((idx<idx<T>*>&) m1), idx<T>*,
              pe2, ((idx<idx<T>*>&) m2), idx<T>*, {
                idx<T> *e11 = *pe1; idx<T> *e22 = *pe2;
                if (e11 && e22) {
                  idx<T> e1 = *e11; idx<T> e2 = *e22;
                  idx_checkorder2_all(e1, e2);
                  for (intg i = 0; i < e1.order(); ++i) {
                    e1 = e1.narrow(i, std::min(e1.dim(i), e2.dim(i)), 0);
                    e2 = e2.narrow(i, std::min(e1.dim(i), e2.dim(i)), 0);
                  }
                  z += idx_l2(e1, e2);
                }
              });
  return sqrt(z);
}

// idx_mean ////////////////////////////////////////////////////////////////////

template <typename T2, typename T> T2 idx_mean(idx<T> &in) {
  return (T2) (idx_sum<T2>(in) / (float64)in.nelements());
}

template <typename T> T idx_mean(idx<T> &in, T *out) {
  T mean = idx_mean(in);
  if (out != NULL) *out = (T) mean;
  return mean;
}

template <typename T> T idx_mean(idx<T> &in) {
  return (T) (idx_sum(in) / (float64)in.nelements());
}

////////////////////////////////////////////////////////////////////////
// idx_std_normalize

template<typename T>
void idx_std_normalize(idx<T> &in, idx<T> &out, T *mean_) {
  T mean = mean_ ? *mean_ : idx_mean(in);
  idx_addc(in, (T)-mean, out); // remove mean
  // std deviation
#ifdef __WINDOWS__
  T coeff = (T) sqrt((double) (idx_sumsqr(out) / out.nelements()));
#else
  T coeff = (T) sqrt((T) (idx_sumsqr(out) / out.nelements()));
#endif
  if (coeff != 0)
    idx_dotc(out, 1 / coeff, out);
}

////////////////////////////////////////////////////////////////////////
// idx_flip

template <class T> idx<T> idx_flip(idx<T> &m, uint n, idx<T> *m2) {
  if (m2 == NULL) {
    idx<T> flipped(m.get_idxdim());
    return idx_flip(m, n, &flipped);
  } else {
    if (!m.same_dim(m2->get_idxdim())) {
      eblerror("expected same dim idx in idx_flip, but got " << m
               << " and " << *m2);
    } else {
      if (n == 0) {
        // we reached the dimension we want to flip, flip it
        for (intg i = 0; i < m.dim(0); ++i) {
          idx<T> a = m.select(0, i);
          idx<T> b = m2->select(0, m.dim(0) - i - 1);
          idx_copy(a, b);
        }
      } else {
        // we didn't reach the dimension to flip, call another recursion
        idx_bloop2(mm, m, T, mm2, *m2, T) {
          idx_flip(mm, n - 1, &mm2);
        }
      }
    }
    return *m2;
  }
}

template <class T> midx<T> idx_flip(midx<T> &m, uint n, midx<T> *m2) {
  if (m2 == NULL) {
    midx<T> flipped(m.get_idxdim());
    // allocate all elements
    flipped.resize(m);
    return idx_flip(m, n, &flipped);
  } else {
    if (!m.same_dim(*m2)) {
      eblerror("expected same dim idx in idx_flip, but got " << m
               << " and " << *m2);
    } else {
      // loop on each object of m and flip it
      if (m.order() == 1) {
        for (intg i = 0; i < m.dim(0); ++i) {
          idx<T> tmp = m.mget(i);
          tmp = idx_flip(tmp, n);
          m2->mset(tmp, i);
        }
      } else if (m.order() == 2) {
        for (intg i = 0; i < m.dim(0); ++i)
          for (intg j = 0; j < m.dim(1); ++j) {
            idx<T> tmp = m.mget(i, j);
            tmp = idx_flip(tmp, n);
            m2->mset(tmp, i, j);
          }
      }
    }
    return *m2;
  }
}

template <typename T> void rev_idx2 (idx<T> &m) {
  CHECK_CONTIGUOUS1(m);
  if (m.order() != 2)
    idx_compatibility_error1(m, "expecting idx of order 2");
  T tmp, *p = m.idx_ptr();
  intg size = m.dim(0) * m.dim(1);
  intg i;

  for (i = 0; i < size/2; i++) {
    tmp = p[i];
    p[i] = p[size-i-1];
    p[size-i-1] = tmp;
  }
}

template <typename T> void rev_idx2_tr (idx<T> &m, idx<T> &n) {
  CHECK_CONTIGUOUS2(m, n);
  if ((m.order() != 2) || (n.order() != 2))
    idx_compatibility_error2(m, n, "expecting idx of order 2");
  intg size = m.dim(0) * m.dim(1);
  T *p = m.idx_ptr() + size - 1;
  T *q = n.idx_ptr();
  for (int i = 0; i < size; i++) {
    *(q++) = *(p--);
  }
}

////////////////////////////////////////////////////////////////////////
// idx's products

/* multiply M2 by M1, result in M1 */
/* matrix - vector product */
template <typename T>
void idx_m2dotm1(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  T *c1, *c2, *c1_0, *ker;
  intg c1_m1 = i1.mod(1), c2_m0 = i2.mod(0);
  intg j, jmax = i2.dim(0);
  intg c1_m0 = i1.mod(0), d1_m0 = o1.mod(0);
  T *d1, f;
  intg i, imax = o1.dim(0);
  c1_0 = i1.idx_ptr();
  ker = i2.idx_ptr();
  d1 = o1.idx_ptr();
  for (i=0; i<imax; i++){
    f = 0;
    c1 = c1_0;
    c2 = ker;
    if(c1_m1==1 && c2_m0==1)
      for (j=0; j<jmax; j++)
        f += (*c1++)*(*c2++);
    else
      for (j=0; j<jmax; j++) {
        f += (*c1)*(*c2);
        c1 += c1_m1;
        c2 += c2_m0;
      }
    *d1 = f;
    d1 += d1_m0;
    c1_0 += c1_m0;
  }
}

/* multiply M2 by M1, result in M1 */
template <class T>
void idx_m2dotm1acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  T *c1, *c2, *c1_0, *ker;
  intg c1_m1 = i1.mod(1), c2_m0 = i2.mod(0);
  intg j, jmax = i2.dim(0);
  intg c1_m0 = i1.mod(0), d1_m0 = o1.mod(0);
  T *d1, f;
  intg i, imax = o1.dim(0);
  c1_0 = i1.idx_ptr();
  ker = i2.idx_ptr();
  d1 = o1.idx_ptr();
  for (i=0; i<imax; i++){
    f = *d1;
    c1 = c1_0;
    c2 = ker;
    for (j=0; j<jmax; j++) {
      f += (*c1)*(*c2);
      c1 += c1_m1;
      c2 += c2_m0;
    }
    *d1 = f;
    d1 += d1_m0;
    c1_0 += c1_m0;
  }
}

template <class T>
void idx_m2dotm3(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  idx_bloop2(ii2, i2, T, oo1, o1, T) {
    idx_bloop2(iii2, ii2, T, ooo1, oo1, T) {
      idx_m2dotm1(i1, iii2, ooo1);
    }
  }
}

// m2dotm2 ///////////////////////////////////////////////////////////////////

template <class T>
void idx_m2dotm2(idx<T> &i1, idx<T> &i2, idx<T> &o) {
  idx_checkorder3(i1, 2, i2, 2, o, 2);
  if (i1.dim(1) != i2.dim(0) || i1.dim(0) != o.dim(0) ||
          o.dim(1) != i2.dim(1))
    eblerror("incompatible dimensions for matrix-matrix multiplication of "
             << i1 << " . " << i2 << " -> " << o);
  T *c1, *c2, *c1_0, *ker;
  intg c1_m1 = i1.mod(1), c2_m0 = i2.mod(0);
  intg j, jmax = i2.dim(0);
  intg c1_m0 = i1.mod(0), d1_m0 = o.mod(0);
  T *d1, f;
  intg i, imax = o.dim(0);
  intg k, kmax = o.dim(1);
  // loop on o.dim(1)
  for (k = 0; k < kmax; ++k) {
    c1_0 = i1.idx_ptr();
    d1 = o.idx_ptr() + k;
    ker = i2.idx_ptr() + k;
    // loop on o.dim(0)
    for (i=0; i<imax; i++){
      f = 0;
      c1 = c1_0;
      c2 = ker;
      // loop on
      if(c1_m1==1 && c2_m0==1)
        for (j=0; j<jmax; j++)
          f += (*c1++)*(*c2++);
      else
        for (j=0; j<jmax; j++) {
          f += (*c1)*(*c2);
          c1 += c1_m1;
          c2 += c2_m0;
        }
      *d1 = f;
      d1 += d1_m0;
      c1_0 += c1_m0;
    }
  }
}

// m4dotm2 ///////////////////////////////////////////////////////////////////

// TODO-0 write specialized blas version in cpp
template <typename T> void idx_m4dotm2(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  idx_checkorder3(i1, 4, i2, 2, o1, 2); // check for compatible orders
  if ((i1.dim(0) != o1.dim(0)) || (i1.dim(1) != o1.dim(1))
          || (i1.dim(2) != i2.dim(0)) || (i1.dim(3) != i2.dim(1)))
    idx_compatibility_error3(i1, i2, o1, "incompatible dimensions");
  T *c1, *c1_2;
  T *c2, *c2_0;
  T *c1_0, *c1_1;
  T *ker;
  intg c1_m2 = (i1).mod(2), c2_m0 = (i2).mod(0);
  intg c1_m3 = (i1).mod(3), c2_m1 = (i2).mod(1);
  intg k,l, kmax = (i2).dim(0), lmax = (i2).dim(1);
  T *d1_0, *d1, f;
  intg c1_m0 = (i1).mod(0), d1_m0 = (o1).mod(0);
  intg c1_m1 = (i1).mod(1), d1_m1 = (o1).mod(1);
  intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
  c1_0 = i1.idx_ptr();
  ker = i2.idx_ptr();
  d1_0 = o1.idx_ptr();
  for (i=0; i<imax; i++) {
    c1_1 = c1_0;
    d1 = d1_0;
    for (j=0; j<jmax; j++) {
      f = 0;
      c1_2 = c1_1;
      c2_0 = ker;
      for (k=0; k<kmax; k++) {
        c1 = c1_2;
        c2 = c2_0;
        for (l=0; l<lmax; l++) {
          f += (*c1)*(*c2);
          c1 += c1_m3;
          c2 += c2_m1;
        }
        c1_2 += c1_m2;
        c2_0 += c2_m0;
      }
      *d1 = f;
      d1 += d1_m1;
      c1_1 += c1_m1;
    }
    d1_0 += d1_m0;
    c1_0 += c1_m0;
  }
}

// TODO-0 write specialized blas version in cpp
template <typename T> void idx_m4dotm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  idx_checkorder3(i1, 4, i2, 2, o1, 2); // check for compatible orders
  if ((i1.dim(0) != o1.dim(0)) || (i1.dim(1) != o1.dim(1))
          || (i1.dim(2) != i2.dim(0)) || (i1.dim(3) != i2.dim(1)))
    idx_compatibility_error3(i1, i2, o1, "incompatible dimensions");
  T *c1, *c1_2;
  T *c2, *c2_0;
  T *c1_0, *c1_1;
  T *ker;
  intg c1_m2 = (i1).mod(2), c2_m0 = (i2).mod(0);
  intg c1_m3 = (i1).mod(3), c2_m1 = (i2).mod(1);
  intg k,l, kmax = (i2).dim(0), lmax = (i2).dim(1);
  T *d1_0, *d1, f;
  intg c1_m0 = (i1).mod(0), d1_m0 = (o1).mod(0);
  intg c1_m1 = (i1).mod(1), d1_m1 = (o1).mod(1);
  intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
  c1_0 = i1.idx_ptr();
  ker = i2.idx_ptr();
  d1_0 = o1.idx_ptr();
  for (i=0; i<imax; i++) {
    c1_1 = c1_0;
    d1 = d1_0;
    for (j=0; j<jmax; j++) {
      f = *d1;
      c1_2 = c1_1;
      c2_0 = ker;
      for (k=0; k<kmax; k++) {
        c1 = c1_2;
        c2 = c2_0;
        for (l=0; l<lmax; l++) {
          f += (*c1)*(*c2);
          c1 += c1_m3;
          c2 += c2_m1;
        }
        c1_2 += c1_m2;
        c2_0 += c2_m0;
      }
      *d1 = f;
      d1 += d1_m1;
      c1_1 += c1_m1;
    }
    d1_0 += d1_m0;
    c1_0 += c1_m0;
  }
}

template <typename T> void idx_m4squdotm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  idx_checkorder3(i1, 4, i2, 2, o1, 2);
  T *c1, *c1_2;
  T *c2, *c2_0;
  T *c1_0, *c1_1;
  T *ker;
  intg c1_m2 = (i1).mod(2), c2_m0 = (i2).mod(0);
  intg c1_m3 = (i1).mod(3), c2_m1 = (i2).mod(1);
  intg k,l, kmax = (i2).dim(0), lmax = (i2).dim(1);
  T *d1_0, *d1, f;
  intg c1_m0 = (i1).mod(0), d1_m0 = (o1).mod(0);
  intg c1_m1 = (i1).mod(1), d1_m1 = (o1).mod(1);
  intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
  c1_0 = i1.idx_ptr();
  ker = i2.idx_ptr();
  d1_0 = o1.idx_ptr();
  for (i=0; i<imax; i++) {
    c1_1 = c1_0;
    d1 = d1_0;
    for (j=0; j<jmax; j++) {
      f = *d1;
      c1_2 = c1_1;
      c2_0 = ker;
      for (k=0; k<kmax; k++) {
        c1 = c1_2;
        c2 = c2_0;
        for (l=0; l<lmax; l++) {
          f += (*c1)*(*c1)*(*c2);
          c1 += c1_m3;
          c2 += c2_m1;
        }
        c1_2 += c1_m2;
        c2_0 += c2_m0;
      }
      *d1 = f;
      d1 += d1_m1;
      c1_1 += c1_m1;
    }
    d1_0 += d1_m0;
    c1_0 += c1_m0;
  }
}

template <typename T> void idx_m2extm2(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  idx_checkorder3(i1, 2, i2, 2, o1, 4);
  T *c2_0, *c2_1;
  T *d1_2, *d1_3;
  T *d1_0, *d1_1;
  T *c1_0, *c1_1;
  T *ker;
  intg c2_m0 = (i2).mod(0), c2_m1 = (i2).mod(1);
  intg d1_m2 = (o1).mod(2), d1_m3 = (o1).mod(3);
  intg c1_m0 = (i1).mod(0), c1_m1 = (i1).mod(1);
  intg d1_m0 = (o1).mod(0), d1_m1 = (o1).mod(1);
  intg k,l, lmax = (o1).dim(3), kmax = (o1).dim(2);
  intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
  c1_0 = i1.idx_ptr();
  ker = i2.idx_ptr();
  d1_0 = o1.idx_ptr();
  for (i=0; i<imax; i++) {
    d1_1 = d1_0;
    c1_1 = c1_0;
    for (j=0; j<jmax; j++) {
      d1_2 = d1_1;
      c2_0 = ker;
      for (k=0; k<kmax; k++) {
        d1_3 = d1_2;
        c2_1 = c2_0;
        for (l=0; l<lmax; l++) {
          *d1_3 = (*c1_1)*(*c2_1);
          d1_3 += d1_m3;
          c2_1 += c2_m1;
        }
        d1_2 += d1_m2;
        c2_0 += c2_m0;
      }
      d1_1 += d1_m1;
      c1_1 += c1_m1;
    }
    d1_0 += d1_m0;
    c1_0 += c1_m0;
  }
}

template <typename T> void idx_m2extm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  idx_checkorder3(i1, 2, i2, 2, o1, 4);
  T *c2_0, *c2_1;
  T *d1_2, *d1_3;
  T *d1_0, *d1_1;
  T *c1_0, *c1_1;
  T *ker;
  intg c2_m0 = (i2).mod(0), c2_m1 = (i2).mod(1);
  intg d1_m2 = (o1).mod(2), d1_m3 = (o1).mod(3);
  intg c1_m0 = (i1).mod(0), c1_m1 = (i1).mod(1);
  intg d1_m0 = (o1).mod(0), d1_m1 = (o1).mod(1);
  intg k,l, lmax = (o1).dim(3), kmax = (o1).dim(2);
  intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
  c1_0 = i1.idx_ptr();
  ker = i2.idx_ptr();
  d1_0 = o1.idx_ptr();
  for (i=0; i<imax; i++) {
    d1_1 = d1_0;
    c1_1 = c1_0;
    for (j=0; j<jmax; j++) {
      d1_2 = d1_1;
      c2_0 = ker;
      for (k=0; k<kmax; k++) {
        d1_3 = d1_2;
        c2_1 = c2_0;
        for (l=0; l<lmax; l++) {
          *d1_3 += (*c1_1)*(*c2_1);
          d1_3 += d1_m3;
          c2_1 += c2_m1;
        }
        d1_2 += d1_m2;
        c2_0 += c2_m0;
      }
      d1_1 += d1_m1;
      c1_1 += c1_m1;
    }
    d1_0 += d1_m0;
    c1_0 += c1_m0;
  }
}

template <typename T> void idx_m2squextm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  idx_checkorder3(i1, 2, i2, 2, o1, 4);
  T *c2_0, *c2_1;
  T *d1_2, *d1_3;
  T *d1_0, *d1_1;
  T *c1_0, *c1_1;
  T *ker;
  intg c2_m0 = (i2).mod(0), c2_m1 = (i2).mod(1);
  intg d1_m2 = (o1).mod(2), d1_m3 = (o1).mod(3);
  intg c1_m0 = (i1).mod(0), c1_m1 = (i1).mod(1);
  intg d1_m0 = (o1).mod(0), d1_m1 = (o1).mod(1);
  intg k,l, lmax = (o1).dim(3), kmax = (o1).dim(2);
  intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
  c1_0 = i1.idx_ptr();
  ker = i2.idx_ptr();
  d1_0 = o1.idx_ptr();
  for (i=0; i<imax; i++) {
    d1_1 = d1_0;
    c1_1 = c1_0;
    for (j=0; j<jmax; j++) {
      d1_2 = d1_1;
      c2_0 = ker;
      for (k=0; k<kmax; k++) {
        d1_3 = d1_2;
        c2_1 = c2_0;
        for (l=0; l<lmax; l++) {
          *d1_3 += (*c1_1)*(*c2_1)*(*c2_1);
          d1_3 += d1_m3;
          c2_1 += c2_m1;
        }
        d1_2 += d1_m2;
        c2_0 += c2_m0;
      }
      d1_1 += d1_m1;
      c1_1 += c1_m1;
    }
    d1_0 += d1_m0;
    c1_0 += c1_m0;
  }
}

// Fu Jie Huang, May 20, 2008
template<typename T> void idx_m2squdotm2(idx<T>& i1, idx<T>& i2, idx<T>& o) {
  idx_checkorder3(i1, 2, i2, 2, o, 0);
  idx_checkdim2(i1, 0, i2.dim(0), i1, 1, i2.dim(1));
  intg imax = i1.dim(0), jmax = i2.dim(1);
  intg c1_m0 = i1.mod(0), c2_m0 = i2.mod(0);
  intg c1_m1 = i1.mod(1), c2_m1 = i2.mod(1);

  T *c1_0 = i1.idx_ptr();
  T *c2_0 = i2.idx_ptr();
  T *d1 = o.idx_ptr();
  T *c1, *c2;

  T f = 0;
  for(int i=0; i<imax; ++i) {
    c1 = c1_0;
    c2 = c2_0;
    for(int j=0; j<jmax; ++j) {
      f += (*c1) * (*c1) * (*c2);   // only difference
      c1 += c1_m1;
      c2 += c2_m1;
    }
    c1_0 += c1_m0;
    c2_0 += c2_m0;
  }
  *d1 = f;
}

// Fu Jie Huang, May 20, 2008
template<typename T>
void idx_m2squdotm2acc(idx<T>& i1, idx<T>& i2, idx<T>& o) {
  idx_checkorder3(i1, 2, i2, 2, o, 0);
  idx_checkdim2(i1, 0, i2.dim(0), i1, 1, i2.dim(1));
  intg imax = i1.dim(0), jmax = i2.dim(1);
  intg c1_m0 = i1.mod(0), c2_m0 = i2.mod(0);
  intg c1_m1 = i1.mod(1), c2_m1 = i2.mod(1);

  T *c1_0 = i1.idx_ptr();
  T *c2_0 = i2.idx_ptr();
  T *d1 = o.idx_ptr();
  T *c1, *c2;

  T f = *d1;          //   only difference: accumulate
  for(int i=0; i<imax; ++i) {
    c1 = c1_0;
    c2 = c2_0;
    for(int j=0; j<jmax; ++j) {
      f += (*c1) * (*c1) * (*c2);
      c1 += c1_m1;
      c2 += c2_m1;
    }
    c1_0 += c1_m0;
    c2_0 += c2_m0;
  }
  *d1 = f;
}

////////////////////////////////////////////////////////////////////////
// idx_m2oversample

// Fu Jie Huang, May 20, 2008
template<typename T>
void idx_m2oversample(idx<T>& small, intg nlin, intg ncol, idx<T>& big) {
  idx<T> uin  = big.unfold(0, nlin, nlin);
  idx<T> uuin = uin.unfold(1, ncol, ncol);
  idx_eloop1(z1, uuin, T) {
    idx_eloop1(z2, z1, T) {
      idx_copy(small, z2);
    }
  }
}

template<typename T>
void idx_m2oversampleacc(idx<T>& small, intg nlin, intg ncol, idx<T>& big) {
  idx<T> uin  = big.unfold(0, nlin, nlin);
  idx<T> uuin = uin.unfold(1, ncol, ncol);
  idx_eloop1(z1, uuin, T) {
    idx_eloop1(z2, z1, T) {
      idx_add(small, z2, z2);
    }
  }
}

////////////////////////////////////////////////////////////////////////
// idx_max

template <class T> T idx_max(idx<T> &m) {
  T v = *(m.idx_ptr());
  idx_aloopf1(i, m, T, { if (*i > v) v = *i; });
  return v;
}

// idx_max (between 2 idx's, in-place)
template <class T> void idx_max(idx<T> &in1, idx<T> &in2) {
  idx_aloopf2(i1, in1, T, i2, in2, T, { *i2 = std::max(*i1, *i2); });
}

// idx_max (between 2 idx's, not-in-place)
template <class T> void idx_max(idx<T> &in1, idx<T> &in2, idx<T> &out) {
  idx_aloopf3(i1, in1, T, i2, in2, T, o, out, T, { *o = std::max(*i1, *i2);});
}

////////////////////////////////////////////////////////////////////////
// idx_min

template <class T> T idx_min(idx<T> &m) {
  T v = *(m.idx_ptr());
  idx_aloopf1(i, m, T, { if (*i < v) v = *i; });
  return v;
}

// idx_min (between 2 idx's, in-place)
template <class T> void idx_min(idx<T> &in1, idx<T> &in2) {
  idx_aloopf2(i1, in1, T, i2, in2, T, { *i2 = std::min(*i1, *i2); });
}

////////////////////////////////////////////////////////////////////////
// idx_indexmax

template <typename T> intg idx_indexmax(idx<T> &m) {
  intg i = 0, imax = 0;
  T v = *(m.idx_ptr());
  idx_aloopf1(me, m, T, {
      if (*me > v) {
        v = *me;
        imax = i;
      }
      i++;
    });
  return imax;
}

////////////////////////////////////////////////////////////////////////
// idx_indexmin

template <typename T> intg idx_indexmin(idx<T> &m) {
  intg i = 0, imin = 0;
  T v = *(m.idx_ptr());
  idx_aloopf1(me, m, T, {
      if (*me < v) {
        v = *me;
        imin = i;
      }
      i++;
    });
  return imin;
}

////////////////////////////////////////////////////////////////////////
// idx_sortdown

template<class T1, class T2> void idx_sortdown(idx<T1> &m, idx<T2> &p) {
  idx_checkorder2(m, 1, p, 1);
  if (m.mod(0) != 1) eblerror("idx_sortdown: vector is not contiguous");
  if (p.mod(0) != 1) eblerror("idx_sortdown: vector is not contiguous");
  intg n = m.dim(0);
  intg z = p.dim(0);
  if (n != z) eblerror("idx_sortdown: vectors have different sizes");
  if (n > 1) {
    int l,j,ir,i;
    T1 *ra, rra;
    T2 *rb, rrb;

    ra = (T1*)m.idx_ptr() -1;
    rb = (T2*)p.idx_ptr() -1;

    l = (n >> 1) + 1;
    ir = n;
    for (;;) {
      if (l > 1) {
        rra=ra[--l];
        rrb=rb[l];
      } else {
        rra=ra[ir];
        rrb=rb[ir];
        ra[ir]=ra[1];
        rb[ir]=rb[1];
        if (--ir == 1) {
          ra[1]=rra;
          rb[1]=rrb;
          return ; } }
      i=l;
      j=l << 1;
      while (j <= ir)	{
        if (j < ir && ra[j] > ra[j+1]) ++j;
        if (rra > ra[j]) {
          ra[i]=ra[j];
          rb[i]=rb[j];
          j += (i=j);
        } else j=ir+1; }
      ra[i]=rra;
      rb[i]=rrb;
    }
  }
}

//! Sorts in-place elements of (continuous) vector <m>
//! in ascending order.
template <typename T> void idx_sortup(idx<T> &m) {
  idx_checkorder1(m, 1);
  CHECK_CONTIGUOUS1(m);
  intg n = m.dim(0);
  if (n > 1) {
    int l,j,ir,i;
    T *ra, rra;
    ra = m.idx_ptr() - 1;
    l=(n >> 1)+1;
    ir= n;
    for (;;) {
      if (l > 1)
	rra=ra[--l];
      else {
	rra=ra[ir];
	ra[ir]=ra[1];
	if (--ir == 1) {
          ra[1]=rra;
          goto end;
	}
      }
      i=l;
      j=l << 1;
      while (j <= ir) {
	if (j < ir && ra[j] < ra[j+1]) ++j;
	if (rra < ra[j]) {
          ra[i]=ra[j];
          j += (i=j);
	} else j=ir+1;
      }
      ra[i]=rra;
    }
  }
end:;
}

//! Sorts in-place elements of (continuous) vector <m>
//! in ascending order.
template<class T1, class T2> void idx_sortup(idx<T1> &m, idx<T2> &m2) {
  idx_checkorder2(m, 1, m2, 1);
  CHECK_CONTIGUOUS2(m, m2);
  idx_checknelems2_all(m, m2); // they must have same number of elements
  intg n = m.dim(0);
  if (n > 1) {
    int l,j,ir,i;
    T1 *ra, rra;
    T2 *ra2, rra2;

    ra = m.idx_ptr() - 1;
    ra2 = m2.idx_ptr() - 1;
    l=(n >> 1)+1;
    ir= n;
    for (;;) {
      if (l > 1) {
	rra=ra[--l];
	rra2=ra2[l];
      } else {
	rra=ra[ir];
	rra2=ra2[ir];
	ra[ir]=ra[1];
	ra2[ir]=ra2[1];
	if (--ir == 1) {
          ra[1]=rra;
          ra2[1]=rra2;
          goto end;
	}
      }
      i=l;
      j=l << 1;
      while (j <= ir) {
	if (j < ir && ra[j] < ra[j+1]) ++j;
	if (rra < ra[j]) {
          ra[i]=ra[j];
          ra2[i]=ra2[j];
          j += (i=j);
	} else j=ir+1;
      }
      ra[i]=rra;
      ra2[i]=rra2;
    }
  }
end:;
}

//! Sorts in-place elements of (continuous) vector <m>
//! in ascending order.
template<class T1, class T2, class T3> void idx_sortup(idx<T1> &m, idx<T2> &m2,
                                                       idx<T3> &m3) {
  idx_checkorder3(m, 1, m2, 1, m3, 1);
  CHECK_CONTIGUOUS3(m, m2, m3);
  idx_checknelems3_all(m, m2, m3); // they must have same number of elements
  intg n = m.dim(0);
  if (n > 1) {
    int l,j,ir,i;
    T1 *ra, rra;
    T2 *ra2, rra2;
    T3 *ra3, rra3;

    ra = m.idx_ptr() - 1;
    ra2 = m2.idx_ptr() - 1;
    ra3 = m3.idx_ptr() - 1;
    l=(n >> 1)+1;
    ir = n;
    for (;;) {
      if (l > 1) {
	rra=ra[--l];
	rra2=ra2[l];
	rra3=ra3[l];
      } else {
	rra=ra[ir];
	rra2=ra2[ir];
	rra3=ra3[ir];
	ra[ir]=ra[1];
	ra2[ir]=ra2[1];
	ra3[ir]=ra3[1];
	if (--ir == 1) {
          ra[1]=rra;
          ra2[1]=rra2;
          ra3[1]=rra3;
          goto end;
	}
      }
      i=l;
      j=l << 1;
      while (j <= ir) {
	if (j < ir && ra[j] < ra[j+1]) ++j;
	if (rra < ra[j]) {
          ra[i]=ra[j];
          ra2[i]=ra2[j];
          ra3[i]=ra3[j];
          j += (i=j);
	} else j=ir+1;
      }
      ra[i]=rra;
      ra2[i]=rra2;
      ra3[i]=rra3;
    }
  }
end:;
}

template <typename T> void idx_sortdown(idx<T> &m) {
  idx_checkorder1(m, 1);
  CHECK_CONTIGUOUS1(m);
  intg n = m.dim(0);
  if (n > 1) {
    int l,j,ir,i;
    T *ra, rra;

    ra = m.idx_ptr() - 1;
    l=(n >> 1)+1;
    ir= n;
    for (;;) {
      if (l > 1)
	rra=ra[--l];
      else {
	rra=ra[ir];
	ra[ir]=ra[1];
	if (--ir == 1) {
          ra[1]=rra;
          goto end; }}
      i=l;
      j=l << 1;
      while (j <= ir) {
	if (j < ir && ra[j] > ra[j+1]) ++j;
	if (rra > ra[j]) {
          ra[i]=ra[j];
          j += (i=j);
	} else j=ir+1; }
      ra[i]=rra;
    }
  }
end:;
}

////////////////////////////////////////////////////////////////////////
// idx_sqrdist

template <typename T> float64 idx_sqrdist(idx<T> &i1, idx<T> &i2) {
  idx_checknelems2_all(i1, i2);
  float64 z = 0;
  float64 tmp;
  idx_aloopf2(pi1, i1, T, pi2, i2, T, {
      tmp = (float64)(*pi1) - (float64)(*pi2);
      z += tmp * tmp;
    });
  return z;
}

////////////////////////////////////////////////////////////////////////
// idx_sqrdist (with idx out)

template <typename T> void idx_sqrdist(idx<T> &i1, idx<T> &i2, idx<T> &out) {
  idx_checknelems2_all(i1, i2);
  if (out.order() != 0) eblerror("idx_sqrdist: expecting an idx of order 0");
  out.set((T)idx_sqrdist(i1, i2));
}

////////////////////////////////////////////////////////////////////////
// idx_spherical_sqrdist

template <typename T> float64 idx_spherical_sqrdist(idx<T> &i1, idx<T> &i2) {
  idx_checknelems2_all(i1, i2);
  float64 z = 0;
  float64 tmp, tmpabs, tmpabs2;
  idx_aloopf2(pi1, i1, T, pi2, i2, T, {
      tmp = fmod((float64)(*pi1) - (float64)(*pi2), TWOPI);
      tmpabs = std::fabs(tmp);
      tmpabs2 = TWOPI - tmpabs;
      if (tmpabs > tmpabs2)
        tmp = tmpabs2;
      z += tmp * tmp;
    });
  return z;
}

////////////////////////////////////////////////////////////////////////
// idx_gaussian

template <typename T>
void idx_gaussian(idx<T> &in, double m, double sigma, idx<T> &out) {
  idx_aloopf2(i, in, T, o, out, T, {
      *o = (T) gaussian((double) (*i), m, sigma);
    });
}

////////////////////////////////////////////////////////////////////////
// idx_modulo

template <class T> void idx_modulo(idx<T> &m, T mod) {
  idx_aloopf1(e, m, T, { *e = *e % mod; });
}

////////////////////////////////////////////////////////////////////////
// idx_exp

template <class T> void idx_exp(idx<T> &m) {
  // note: we use float32 instead of 64 because float64 has its own
  // specialized template, therefore, calls to this generic template
  // will be with types of lower precision than float32, no need for
  // float64 precision.
  idx_aloopf1(i, m, T, { *i = saturate(exp((float32)*i), T); });
}

////////////////////////////////////////////////////////////////////////
// idx_log

template <class T> void idx_log(idx<T> &m) {
#ifdef __WINDOWS__
  idx_aloopf1(i, m, T, { *i = (T)log((double)*i); });
#else
  idx_aloopf1(i, m, T, { *i = (T)log(*i); });
#endif
}

template <typename T> EXPORT void idx_log(idx<T>& in, idx<T> &out) {
#ifdef __WINDOWS__
	idx_aloopf2(i, in, T, o, out, T, { *o = (T)log((double)*i); });
#else
	idx_aloopf2(i, in, T, o, out, T, { *o = (T)log(*i); });
#endif
}

////////////////////////////////////////////////////////////////////////
// idx_dot

template <class T> T idx_dot(idx<T> & in1, idx<T> & in2) {
  T z = 0;
#if USING_FAST_ITERS == 0
  { idx_aloop2(pi1, in1, T, pi2, in2, T) {
      z += *pi1 * *pi2;
    }
  }
#else
  idx_aloopf2(pi1, in1, T, pi2, in2, T, {
      z += *pi1 * *pi2;
    });
#endif
  return z;
}

// TODO: this doesn't compile on newest gcc
//   template <class T> float64 idx_dot(idx<T> & in1, idx<T> & in2) {
//     float64 z = 0;
// #if USING_FAST_ITERS == 0
//     { idx_aloop2(pi1, in1, T, pi2, in2, T) {
// 	z += ((float64)(*pi1)) * ((float64)(*pi2));
//       }
//     }
// #else
//     idx_aloopf2(pi1, in1, T, pi2, in2, T, {
// 	z += ((float64)(*pi1)) * ((float64)(*pi2));
//       });
// #endif
//     return z;
//   }

////////////////////////////////////////////////////////////////////////
// idx_dotacc

template <typename T> void idx_dotacc(idx<T> &i1, idx<T> &i2, idx<T> &o) {
  if (o.order() != 0)
    eblerror("output idx should have order 0");
  o.set(o.get() + (T) idx_dot(i1, i2));
}

////////////////////////////////////////////////////////////////////////
// idx_clip

template <typename T> void idx_clip(idx<T> &i1, T m, idx<T> &o1) {
  idx_checknelems2_all(i1, o1);
#if USING_FAST_ITERS == 0
  { idx_aloop2(i, i1, T, o, o1, T) {
      *o = (std::max)(m, *i);
    }}
#else
  idx_aloopf2(i, i1, T, o, o1, T, { *o = std::max(m, *i); });
#endif
}

////////////////////////////////////////////////////////////////////////
// idx_2dconvol

template <typename T> void idx_2dconvol(idx<T> &in, idx<T> &kernel, idx<T> &out) {
  idx_checkorder3(in, 2, kernel, 2, out, 2);
  idx<T> uin(in.unfold(0, kernel.dim(0), 1));
  uin = uin.unfold(1, kernel.dim(1), 1);
  idx_m4dotm2(uin, kernel, out);
}

template <typename T> void idx_3dconvol(idx<T> &in, idx<T> &kernel, idx<T> &out) {
  idx_bloop2(i, in, T, o, out, T) {
    idx_2dconvol(i, kernel, o);
  }
}

template <typename T>
void idx_m1extm1(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  T *c2, *d1, *c1, *c2_0, *d1_0;
  intg c2_m0 = i2.mod(0), d1_m1 = o1.mod(1);
  intg c1_m0 = i1.mod(0), d1_m0 = o1.mod(0);
  intg j, jmax = o1.dim(1);
  intg i, imax = o1.dim(0);
  c1 = i1.idx_ptr();
  c2_0 = i2.idx_ptr();
  d1_0 = o1.idx_ptr();
  for (i=0; i<imax; i++) {
    d1 = d1_0;
    c2 = c2_0;
    for (j=0; j<jmax; j++) {
      *d1 = (*c1)*(*c2);
      d1 += d1_m1;
      c2 += c2_m0;
    }
    d1_0 += d1_m0;
    c1 += c1_m0;
  }
}

template <typename T>
void idx_m1extm1acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
  T *c2, *d1, *c1, *c2_0, *d1_0;
  intg c2_m0 = i2.mod(0), d1_m1 = o1.mod(1);
  intg c1_m0 = i1.mod(0), d1_m0 = o1.mod(0);
  intg j, jmax = o1.dim(1);
  intg i, imax = o1.dim(0);
  c1 = i1.idx_ptr();
  c2_0 = i2.idx_ptr();
  d1_0 = o1.idx_ptr();
  for (i=0; i<imax; i++) {
    d1 = d1_0;
    c2 = c2_0;
    for (j=0; j<jmax; j++) {
      *d1 += (*c1)*(*c2);
      d1 += d1_m1;
      c2 += c2_m0;
    }
    d1_0 += d1_m0;
    c1 += c1_m0;
  }
}

template <typename T>
void idx_m2squdotm1(idx<T> &a, idx<T> &x, idx<T> &y) {
  eblerror("not implemented");
}

template <typename T>
void idx_m2squdotm1acc(idx<T> &a, idx<T> &x, idx<T> &y) {
  eblerror("not implemented");
}

template <typename T>
void idx_m1squextm1(idx<T> &x, idx<T> &y, idx<T> &a) {
  eblerror("not implemented");
}

template <typename T>
void idx_m1squextm1acc(idx<T> &x, idx<T> &y, idx<T> &a) {
  eblerror("not implemented");
}

template <typename T>
T idx_trace(idx<T> &m2) {
  idx_checkorder1(m2, 2); // must be 2D
  if (m2.dim(0) != m2.dim(1))
    eblerror("Expected a square matrix, but found: " << m2);
  T sum = 0;
  for (uint i = 0; i < m2.dim(0); ++i)
    sum += m2.get(i, i);
  return sum;
}

////////////////////////////////////////////////////////////////
// concatenation

template <typename T>
idx<T> idx_concat(idx<T> &m1, idx<T> &m2, intg dim) {
  idxdim d(m1);
  // m1 and m2 must have the same order
  idx_checkorder2(m1, d.order(), m2, d.order());
  // check that all dimensions have the same size except for
  for (intg i = 0; i < d.order(); ++i) {
    if (i != dim && m1.dim(i) != m2.dim(i))
      eblerror("expected same dimensions except for dim " << dim
               << " but found differences: " << m1 << " and " << m2);
  }
  // allocate and copy
  d.setdim(dim, m1.dim(dim) + m2.dim(dim));
  idx<T> m3(d);
  idx<T> tmp = m3.narrow(dim, m1.dim(dim), 0);
  idx_copy(m1, tmp);
  tmp = m3.narrow(dim, m2.dim(dim), m1.dim(dim));
  idx_copy(m2, tmp);
  return m3;
}

// randomization ////////////////////////////////////////////////////////////

template <typename T>
void idx_random(idx<T> &m, double v0, double v1) {
  idx_aloopf1(mm, m, T, { *mm = (T) drand(v0, v1); });
}

template <typename T>
void idx_random(idx<T> &m, double v) {
  idx_aloopf1(mm, m, T, { *mm = (T) drand(v); });
}

} // end namespace ebl

/*
  #ifdef __OPENMP__
  #include "idxops_openmp.hpp"
  #endif
*/

#endif /* IDXOPS_HPP */
