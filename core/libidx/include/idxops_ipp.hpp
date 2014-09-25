#ifdef __IPP__

////////////////////////////////////////////////////////////////
// idx_copy

#define idx_copy_macro(T)					\
  template<> void idx_copy(const idx<T> &src, idx<T> &dst) {	\
    idx_checknelems2_all(src, dst);				\
    if ( (src.order() == 0) && (dst.order() == 0) ) {		\
      *(dst.idx_ptr()) = *(src.idx_ptr());			\
    } else if (dst.contiguousp() && src.contiguousp()) {	\
      ipp_copy(src, dst);					\
    } else {							\
      /*This is slower */					\
      idx_aloop2(isrc, src, T, idst, dst, T) { *idst = *isrc; }	\
    }								\
  }

////////////////////////////////////////////////////////////////
// idx_clear

#define idx_clear_macro(T)			\
  template<> void idx_clear(idx<T> & inp) {	\
    if (inp.contiguousp()) {			\
      ipp_clear(inp);				\
    } else {					\
      idxiter<T> pinp;				\
      idx_aloop1_on(pinp,inp) { *pinp = 0; }	\
    }						\
  }

////////////////////////////////////////////////////////////////
// idx_fill

#define idx_fill_macro(T)			\
  template<> void idx_fill(idx<T> & inp, T v) {	\
    if (inp.contiguousp()) {			\
      ipp_fill(inp, v);				\
    } else {					\
      idxiter<T> pinp;				\
      idx_aloop1_on(pinp,inp) { *pinp = v; }	\
    }						\
  }

////////////////////////////////////////////////////////////////////////
// idx_minus

#define idx_minus_macro(T)					\
  template<> void idx_minus(idx<T> &inp, idx<T> &out) {		\
    if (inp.contiguousp() && out.contiguousp()) {		\
      ipp_minus(inp, out);					\
    } else {							\
      idxiter<T> pinp; idxiter<T> pout;				\
      idx_aloop2_on(pinp,inp,pout,out) { *pout = - *pinp; }	\
    }								\
  }

////////////////////////////////////////////////////////////////////////
// idx_minus

#define idx_minus_acc_macro(T)					\
  template<> void idx_minus_acc(idx<T> &inp, idx<T> &out) {	\
    if (inp.contiguousp() && out.contiguousp()) {		\
      ipp_minus(inp, out);					\
    } else {							\
      idxiter<T> pinp; idxiter<T> pout;				\
      idx_aloop2_on(pinp,inp,pout,out) { *pout += - *pinp; }	\
    }								\
  }

////////////////////////////////////////////////////////////////////////
// idx_add

#define idx_add_macro_in_place(T)				\
  template<> void idx_add(idx<T> &in, idx<T> &out) {		\
    if (in.contiguousp() && out.contiguousp()) {		\
      ipp_add(in, out);						\
    } else {							\
      idxiter<T> pin, pout;					\
      idx_aloop2_on(pin,in,pout,out) { *pout = *pout + *pin; }	\
    }								\
  }

#define idx_add_macro(T)						\
  template<> void idx_add(idx<T> &in1, idx<T> & in2, idx<T> &out) {	\
    if (in1.contiguousp() && in2.contiguousp() && out.contiguousp()) {	\
      ipp_add(in1, in2, out);						\
    } else {								\
      idxiter<T> pi1, pi2, pout;					\
      idx_aloop3_on(pi1,in1,pi2,in2,pout,out) { *pout = *pi1 + *pi2; }	\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_sub

#define idx_sub_macro(T)						\
  template<> void idx_sub(idx<T> &in1, idx<T> & in2, idx<T> &out) {	\
    if (in1.contiguousp() && in2.contiguousp() && out.contiguousp()) {	\
      ipp_sub(in1, in2, out);						\
    } else {								\
      idxiter<T> pi1, pi2, pout;					\
      idx_aloop3_on(pi1,in1,pi2,in2,pout,out) { *pout = *pi1 - *pi2; }	\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_mul

#define idx_mul_macro(T)						\
  template<> void idx_mul(idx<T> &in1, idx<T> & in2, idx<T> &out) {	\
    if (in1.contiguousp() && in2.contiguousp() && out.contiguousp()) {	\
      ipp_mul(in1, in2, out);						\
    } else {								\
      idxiter<T> pi1, pi2, pout;					\
      idx_aloop3_on(pi1,in1,pi2,in2,pout,out) { *pout = (*pi1) * (*pi2); } \
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_m2dotm1

#define idx_m2dotm1_macro(T)						\
  template<> void idx_m2dotm1(idx<T> &in1, idx<T> & in2, idx<T> &out) {	\
    if (in1.contiguousp() && in2.contiguousp() && out.contiguousp()) {	\
      ipp_m2dotm1(in1, in2, out);					\
    } else {								\
      eblerror("not implemented for non contiguous");			\
      /*      idxiter<T> pi1, pi2, pout;			*/      \
      /* idx_aloop3_on(pi1,in1,pi2,in2,pout,out) { *pout = (*pi1) * (*pi2); }*/ \
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_m2dotm1

#define idx_m2dotm2_macro(T)						\
  template<> void idx_m2dotm2(idx<T> &in1, idx<T> & in2, idx<T> &out) {	\
    if (in1.contiguousp() && in2.contiguousp() && out.contiguousp()) {	\
      ipp_m2dotm2(in1, in2, out);					\
    } else {								\
      eblerror("not implemented for non contiguous");			\
      /*      idxiter<T> pi1, pi2, pout;			*/      \
      /* idx_aloop3_on(pi1,in1,pi2,in2,pout,out) { *pout = (*pi1) * (*pi2); }*/ \
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_div

#define idx_div_macro(T)						\
  template<> void idx_div(idx<T> &in1, idx<T> & in2, idx<T> &out) {	\
    if (in1.contiguousp() && in2.contiguousp() && out.contiguousp()) {	\
      ipp_div(in1, in2, out);						\
    } else {								\
      idxiter<T> pi1, pi2; idxiter<T> pout;				\
      idx_aloop3_on(pi1,in1,pi2,in2,pout,out) {				\
	/* TODO: remove this check in optimized version */		\
	/* inefficient check but really important for debugging */	\
	if (*pi2 == 0)							\
	  eblerror("division by zero");					\
	*pout = (*pi1) / (*pi2);					\
      }									\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_addc

#define idx_addc_macro(T)					\
  template<> void idx_addc(idx<T> &inp, T c, idx<T> &out) {	\
    if (inp.contiguousp() && out.contiguousp()) {		\
      ipp_addc(inp, c, out);					\
    } else {							\
      idxiter<T> pinp; idxiter<T> pout;				\
      idx_aloop2_on(pinp,inp,pout,out) { *pout = *pinp + c; }	\
    }								\
  }

////////////////////////////////////////////////////////////////////////
// idx_addc_bounded

#define idx_addc_bounded_macro(T)					\
  template<> void idx_addc_bounded(idx<T> &inp, T c, idx<T> &out) {	\
    if (inp.contiguousp() && out.contiguousp()) {			\
      ipp_addc(inp, c, out);						\
    } else {								\
      idxiter<T> pinp; idxiter<T> pout;					\
      idx_aloop2_on(pinp,inp,pout,out) {				\
	*pout = saturate(*pinp + c, T);					\
      }									\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_subc_bounded

#define idx_subc_bounded_macro(T)					\
  template<> void idx_subc_bounded(idx<T> &inp, T c, idx<T> &out) {	\
    if (inp.contiguousp() && out.contiguousp()) {			\
      ipp_subc(inp, c, out);						\
    } else {								\
      idxiter<T> pinp; idxiter<T> pout;					\
      idx_aloop2_on(pinp,inp,pout,out) {				\
	*pout = saturate(*pinp - c, T);					\
      }									\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_dotc

#define idx_dotc_macro(T)					\
  template<> void idx_dotc(idx<T> &inp, T c, idx<T> &out) {	\
    if (inp.contiguousp() && out.contiguousp()) {		\
      ipp_dotc(inp, c, out);					\
    } else {							\
      idxiter<T> pinp; idxiter<T> pout;				\
      idx_aloop2_on(pinp,inp,pout,out) { *pout = *pinp * c; }	\
    }								\
  }

////////////////////////////////////////////////////////////////////////
// idx_dotc_bounded

#define idx_dotc_bounded_macro(T)					\
  template<> void idx_dotc_bounded(idx<T> &inp, T c, idx<T> &out) {	\
    if (inp.contiguousp() && out.contiguousp()) {			\
      ipp_dotc(inp, c, out);						\
    } else {								\
      idxiter<T> pinp; idxiter<T> pout;					\
      idx_aloop2_on(pinp,inp,pout,out) {				\
	*pout = saturate(*pinp * c, T);					\
      }									\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_abs

#define idx_abs_macro(T)						\
  template<> void idx_abs(idx<T> &inp, idx<T> &out) {			\
    if (inp.contiguousp() && out.contiguousp()) {			\
      ipp_abs(inp, out);						\
    } else {								\
      idxiter<T> pinp; idxiter<T> pout;					\
      idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(std::abs(*pinp)); } \
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_threshold (in-place)

#define idx_threshold_in_place_macro(T)			\
  template<> void idx_threshold(idx<T> & in, T th) {	\
    if (in.contiguousp()) {				\
      ipp_threshold_lt(in, th);				\
    } else {						\
      idxiter<T> pin;					\
      idx_aloop1_on(pin,in) {				\
	if (*pin < th)					\
	  *pin = th;					\
      }							\
    }							\
  }

////////////////////////////////////////////////////////////////////////
// idx_threshold (not-in-place)

#define idx_threshold_macro(T)						\
  template<> void idx_threshold(idx<T> & in, T th, idx<T> & out) {	\
    if (in.contiguousp() && out.contiguousp()) {			\
      ipp_threshold_lt(in, th, out);					\
    } else {								\
      idxiter<T> pin; idxiter<T> pout;					\
      idx_aloop2_on(pin,in,pout,out) {					\
	if (*pin < th)							\
	  *pout = th;							\
	else								\
	  *pout = *pin;							\
      }									\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_threshold (in-place, with value)

#define idx_threshold_in_place_val_macro(T)			\
  template<> void idx_threshold(idx<T> & in, T th, T value) {	\
    if (in.contiguousp()) {					\
      ipp_threshold_lt(in, th, value);				\
    } else {							\
      idxiter<T> pin;						\
      idx_aloop1_on(pin,in) {					\
	if (*pin < th)						\
	  *pin = value;						\
      }								\
    }								\
  }

////////////////////////////////////////////////////////////////////////
// idx_threshold (not-in-place, with value)

#define idx_threshold_val_macro(T)					\
  template<> void idx_threshold(idx<T> & in, T th, T value, idx<T> & out) { \
    if (in.contiguousp() && out.contiguousp()) {			\
      ipp_threshold_lt(in, th, value, out);				\
    } else {								\
      idxiter<T> pin; idxiter<T> pout;					\
      idx_aloop2_on(pin,in,pout,out) {					\
	if (*pin < th)							\
	  *pout = value;						\
	else								\
	  *pout = *pin;							\
      }									\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_sqrt

#define idx_sqrt_macro(T)				\
  template<> void idx_sqrt(idx<T> &inp, idx<T> &out) {	\
    if (inp.contiguousp() && out.contiguousp()) {	\
      ipp_sqrt(inp, out);				\
    } else {						\
      idxiter<T> pin; idxiter<T> pout;			\
      idx_aloop2_on(pin,inp,pout,out) {			\
	*pout = (T)sqrt(*pin);				\
      }							\
    }							\
  }

////////////////////////////////////////////////////////////////////////
// idx_sum

#ifndef __OPENMP__

#define idx_sum_macro(T)				\
  template<> T idx_sum(idx<T> &inp, T* out) {		\
    if (inp.contiguousp()) {				\
      if (out != NULL) {				\
	*out = (T)ipp_sum(inp);				\
	return *out;					\
      } else {						\
	return (T)ipp_sum(inp);				\
      }							\
    } else {						\
      /* there is a much faster and parallel way */	\
      /* of doing this using a tree. */			\
      T z = 0;						\
      idxiter<T> pinp;					\
      idx_aloop1_on(pinp,inp) { z += *pinp; }		\
      if (out != NULL)					\
	*out = z;					\
      return z;						\
    }							\
  }

#endif

////////////////////////////////////////////////////////////////////////
// idx_sumabs

#define idx_sumabs_macro(T)                                             \
  template<> float64 idx_sumabs(idx<T> & in, T* out) {                  \
    if (in.contiguousp()) {                                             \
      if (out != NULL) {                                                \
	float64 sum = ipp_sumabs(in);                                   \
	*out = saturate(sum, T);                                        \
	return sum;                                                     \
      }                                                                 \
      return ipp_sumabs(in);                                            \
    } else {                                                            \
      /* there is a much faster and parallel way */                     \
      /* of doing this using a tree. */                                 \
      float64 z = 0;                                                    \
      idxiter<T> pinp;                                                  \
      idx_aloop1_on(pinp,in) { z += std::abs((float64)(*pinp)); }	\
      if (out != NULL)                                                  \
	*out = saturate(z, T);                                          \
      return z;                                                         \
    }                                                                   \
  }

////////////////////////////////////////////////////////////////////////
// idx_l2norm

#define idx_l2norm_macro(T)			\
  template<> float64 idx_l2norm(idx<T> & in) {	\
    if (in.contiguousp()) {			\
      return ipp_l2norm(in);			\
    } else {					\
      return sqrt(idx_sumsqr(in));		\
    }						\
  }

////////////////////////////////////////////////////////////////////////
// idx_mean

#define idx_mean_macro(T)					\
  template<> T idx_mean(idx<T> & in) {                          \
    if (in.contiguousp()) return (T)ipp_mean(in);               \
    else return (T)(idx_sum(in) / (float64)in.nelements());	\
  }

////////////////////////////////////////////////////////////////////////
// idx_max

#define idx_max_macro(T)			\
  template<> T idx_max(idx<T> & m) {		\
    if (m.contiguousp()) {			\
      return ipp_max(m);			\
    } else {					\
      T v = *(m.idx_ptr());			\
      { idx_aloop1(i, m, T) {			\
	  if (*i > v) v = *i;			\
	}}					\
      return v;					\
    }						\
  }

////////////////////////////////////////////////////////////////////////
// idx_min

#define idx_min_macro(T)			\
  template<> T idx_min(idx<T> & m) {		\
    if (m.contiguousp()) {			\
      return ipp_min(m);			\
    } else {					\
      T v = *(m.idx_ptr());			\
      { idx_aloop1(i, m, T) {			\
	  if (*i < v) v = *i;			\
	}}					\
      return v;					\
    }						\
  }

////////////////////////////////////////////////////////////////////////
// idx_indexmax

#define idx_indexmax_macro(T)			\
  template<> intg idx_indexmax(idx<T> & m) {	\
    if (m.contiguousp()) {			\
      return ipp_indexmax(m);			\
    } else {					\
      intg i = 0, imax = 0;			\
      T v = *(m.idx_ptr());			\
      { idx_aloop1(me, m, T) {			\
	  if (*me > v) {			\
	    v = *me;				\
	    imax = i;				\
	  }					\
	  i++;					\
	}}					\
      return imax;				\
    }						\
  }

////////////////////////////////////////////////////////////////////////
// idx_indexmin

#define idx_indexmin_macro(T)			\
  template<> intg idx_indexmin(idx<T> & m) {	\
    if (m.contiguousp()) {			\
      return ipp_indexmin(m);			\
    } else {					\
      intg i = 0, imin = 0;			\
      T v = *(m.idx_ptr());			\
      { idx_aloop1(me, m, T) {			\
	  if (*me < v) {			\
	    v = *me;				\
	    imin = i;				\
	  }					\
	  i++;					\
	}}					\
      return imin;				\
    }						\
  }

////////////////////////////////////////////////////////////////////////
// idx_max (between 2 idx's, in-place)

#define idx_maxevery_macro(T)				\
  template<> void idx_max(idx<T> & in1, idx<T> & in2) {	\
    if (in1.contiguousp() && in2.contiguousp()) {	\
      return ipp_maxevery(in1, in2);			\
    } else {						\
      idx_aloop2(i1, in1, T, i2, in2, T) {		\
	*i2 = std::max(*i1, *i2);			\
      }							\
    }							\
  }

////////////////////////////////////////////////////////////////////////
// idx_max (between 2 idx's, not-in-place)

#define idx_maxevery2_macro(T)						\
  template<> void idx_max(idx<T> & in1, idx<T> & in2, idx<T> & out) {	\
    if (in1.contiguousp() && in2.contiguousp() && out.contiguousp()) {	\
      return ipp_maxevery(in1, in2, out);				\
    } else {								\
      idx_aloop3(i1, in1, T, i2, in2, T, o, out, T) {			\
	*o = std::max(*i1, *i2);					\
      }									\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_sqrdist

#define idx_sqrdist_macro(T)					\
  template<> float64 idx_sqrdist(idx<T> & in1, idx<T> & in2) {	\
    if (in1.contiguousp() && in2.contiguousp()) {		\
      return ipp_sqrdist(in1, in2);				\
    } else {							\
      idx_checknelems2_all(in1, in2);				\
      float64 z = 0;						\
      float64 tmp;						\
      { idx_aloop2(pi1, in1, T, pi2, in2, T) {			\
	  tmp = (float64)(*pi1) - (float64)(*pi2);		\
	  z += tmp * tmp;					\
	}							\
      }								\
      return z;							\
    }								\
  }

////////////////////////////////////////////////////////////////////////
// idx_sqrdist (with idx out)

#define idx_sqrdist2_macro(T)						\
  template<> void idx_sqrdist(idx<T> & in1, idx<T> & in2, idx<T> & out) { \
    if (in1.contiguousp() && in2.contiguousp() && out.contiguousp()) {	\
      ipp_sqrdist(in1, in2, out);					\
    } else {								\
      idx_checknelems2_all(in1, in2);					\
      if (out.order() != 0)						\
	eblerror("idx_sqrdist: expecting an idx of order 0");		\
      out.set((T)idx_sqrdist(in1, in2));				\
    }									\
  }

////////////////////////////////////////////////////////////////////////
// idx_exp

#define idx_exp_macro(T)			\
  template<> void idx_exp(idx<T> &inp) {	\
    if (inp.contiguousp()) {			\
      ipp_exp(inp);				\
    } else {					\
      idx_aloop1(i, inp, T) {			\
	*i = (T)exp(*i);			\
      };					\
    }						\
  }

////////////////////////////////////////////////////////////////////////
// idx_dot

#ifdef __IPP_DOT__

#define idx_dot_macro(T)				\
  template<> T idx_dot(idx<T> & in1, idx<T> & in2) {	\
    if (in1.contiguousp() && in2.contiguousp()) {	\
      return ipp_dot(in1, in2);				\
    } else {						\
      T z = 0;						\
      { idx_aloop2(pi1, in1, T, pi2, in2, T) {		\
	  z += *pi1 * *pi2;				\
	}						\
      }							\
      return z;						\
    }							\
  }

#endif

// TODO: this does not compile
// #ifdef __IPP_DOT__

/*
  #define idx_dot_macro(T)						\
  template<> float64 idx_dot(idx<T> & in1, idx<T> & in2) {		\
  if (in1.contiguousp() && in2.contiguousp()) {			\
  return ipp_dot(in1, in2);					\
  } else {								\
  float64 z = 0;							\
  { idx_aloop2(pi1, in1, T, pi2, in2, T) {			\
  z += ((float64)(*pi1)) * ((float64)(*pi2));			\
  }								\
  }								\
  return z;							\
  }									\
  }
*/
// #endif

#endif /* #ifdef __IPP__ */
