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

#ifndef IDXITER_HPP_
#define IDXITER_HPP_

namespace ebl {

// state_idx error checking macros ///////////////////////////////////////////

//! check that 2 state_idxs (references) are different, if not, call eblerror
#define state_idx_check_different(s1, s2)				\
  if (&s1 == &s2) eblerror("in and out state_idx must be different");
//! check that 3 state_idxs (references) are different, if not, call eblerror
#define state_idx_check_different3(s1, s2, s3)			\
  if ((&s1 == &s2) || (&s1 == &s3)) 				\
    eblerror("in1, in2 and out state_idx must be different");

// idx error macros //////////////////////////////////////////////////////////

//! Print incompatible idx1 and idx2 and call eblerror with errmsg.
#define idx_compatibility_error2(idx1, idx2, errmsg) {			\
    eblerror(idx1 << " and " << idx2 << " are incompatible: " << errmsg); }
//! Print incompatible idx1, idx2, idx3 and call eblerror with errmsg.
#define idx_compatibility_error3(idx1, idx2, idx3, errmsg) {    \
    eblerror(idx1 << ", " << idx2 << " and " << idx3            \
	     << " are incompatible: " << errmsg); }
//! Print incompatible idx1, idx2, idx3 and call eblerror with errmsg.
#define idx_compatibility_error4(idx1, idx2, idx3, idx4, errmsg) {	\
    eblerror(idx1 << ", " << idx2 << ", " << idx3 << " and "		\
	     << idx4 << " are incompatible: " << errmsg); }
//! Print incompatible idx1, idx2, idx3 and call eblerror with errmsg.
#define idx_compatibility_error5(idx1, idx2, idx3, idx4, idx5, errmsg) { \
    eblerror("error: " << idx1 << ", " << idx2 << ", " << idx3 << ", " << idx4 \
	     << " and " << idx5 << " are incompatible: " << errmsg); }
//! Print incompatible idx1, idx2, idx3 and call eblerror with errmsg.
#define idx_compatibility_error6(idx1, idx2, idx3, idx4, idx5, idx6, errmsg) { \
    eblerror("error: " << idx1 << ", " << idx2 << ", " << idx3 << ", " << idx4 \
	     << ", " << idx5 << " and " << idx6				\
	     << " are incompatible: " << errmsg); }

// idx elements and dimensions error checking macros /////////////////////////

#ifdef __DEBUG__
#define DEBUG_CHECK_SAME_DIMS(m1, m2)					\
  if (m1.get_idxdim() != m2.get_idxdim())				\
    eblerror("expected same dimensions but got " << m1 << " and " << m2);

#else
#define DEBUG_CHECK_SAME_DIMS(m1, m2)
#endif


// contiguity checks ///////////////////////////////////////////////////////////

#define CHECK_CONTIGUOUS1(m1) {                                         \
    if (!(m1).contiguousp()) eblerror("expected contiguous tensor " << m1); \
  }
#define CHECK_CONTIGUOUS2(m1, m2) {                                     \
    if (!(m1).contiguousp()) eblerror("expected contiguous tensor " << m1); \
    if (!(m2).contiguousp()) eblerror("expected contiguous tensor " << m2); \
  }
#define CHECK_CONTIGUOUS3(m1, m2, m3) {                                 \
    if (!(m1).contiguousp()) eblerror("expected contiguous tensor " << m1); \
    if (!(m2).contiguousp()) eblerror("expected contiguous tensor " << m2); \
    if (!(m3).contiguousp()) eblerror("expected contiguous tensor " << m3); \
  }

//! Calls eblerror if src0 and src1 have different number of elements.
#define idx_checknelems2_all(src0, src1)                        \
  if ((src0).nelements() != (src1).nelements()) {               \
    eblerror(src0 << " and " << src1 <<                         \
	     " should have the same number of elements"); }
//! Calls eblerror if src0 and src1 and src2 have different number of
//! elements.
#define idx_checknelems3_all(src0, src1, src2)                  \
  if (((src0).nelements() != (src1).nelements()) ||             \
          ((src0).nelements() != (src2).nelements())) {         \
    eblerror(src0 << ", " << src1 << " and " << src2            \
             << " should have the same number of elements"); }
//! Calls eblerror if src0 and src1 and src2 have different number of
//! elements.
#define idx_checknelems4_all(src0, src1, src2, src3)			\
  if (((src0).nelements() != (src1).nelements()) ||			\
          ((src0).nelements() != (src2).nelements()) ||			\
      ((src0).nelements() != (src3).nelements())) {			\
    eblerror(src0 << ", " << src1 << " and " << src2 << " and " << src3	\
	     << " should have the same number of elements"); }
//! Calls eblerror if src0 and o0 do not match.
#define idx_checkorder1(src0, o0)                       \
  if ((src0).order() != o0) {                           \
    eblerror(src0 << " does not have order " << o0); }
//! Calls eblerror if src0,src1 and o0,o1 do not match.
#define idx_checkorder2(src0, o0, src1, o1)				\
  if (((src0).order() != o0) || ((src1).order() != o1)) {		\
    std::string err;							\
    if ((src0).order() != o0) err << src0 <<" should have order "<< o0<<". "; \
    if ((src1).order() != o1) err << src1 <<" should have order "<< o1<<". "; \
    eblerror("incompatible orders: " << err); }
//! Calls eblerror if src0,src1,src2 and o0,o1,o2 do not match.
#define idx_checkorder3(src0, o0, src1, o1, src2, o2)			\
  if (((src0).order() != o0) || ((src1).order() != o1)			\
          || ((src2).order() != o2)) {					\
    std::string err;							\
    if ((src0).order() != o0) err << src0 <<" should have order "<< o0<<". "; \
    if ((src1).order() != o1) err << src1 <<" should have order "<< o1<<". "; \
    if ((src2).order() != o2) err << src2 <<" should have order "<< o2<<". "; \
    eblerror("incompatible orders: " << err); }
//! Calls eblerror if src0.order() and src1.order() differ
#define idx_checkorder2_all(src0, src1)					\
  if ((src0).order() != (src1).order())					\
    idx_compatibility_error2(src0, src1, "idx have incompatible orders");
//! Calls eblerror if src0.order(), src1.order() and src2.order() differ
#define idx_checkorder3_all(src0, src1, src2)				\
  if (((src0).order() != (src1).order())				\
          || ((src0).order() != (src2).order()))                        \
    idx_compatibility_error3(src0, src1, src2, "idx have incompatible orders");
//! Calls eblerror if src0.dim(d0) != e0
#define idx_checkdim1(src0, d0, e0)					\
  if ((src0).dim(d0) != e0)						\
    eblerror("expected dim " << d0 << " to be " << e0 << " in " << src0);
//! Calls eblerror if src0.dim(d0) and src1.dim(d1) don't match e0,e1
#define idx_checkdim2(src0, d0, e0, src1, d1, e1)			\
  if ((src0).dim(d0) != e0)						\
    eblerror("expected dim " << d0 << " to be " << e0 << " in " << src0); \
  if ((src1).dim(d1) != e1)						\
    eblerror("expected dim " << d1 << " to be " << e1 << " in " << src1);
//! Calls eblerror if src0.dim(d) and src1.dim(d) don't match
#define idx_checkdim2_all(src0, src1, d)                                \
  if ((src0).dim(d) != (src1).dim(d))                                   \
    idx_compatibility_error2(src0, src1,"expected same sizes in dimension "<<d);
//! Calls eblerror if src0.dim(d) and src1.dim(d) and src2.dim(d) don't match
#define idx_checkdim3_all(src0, src1, src2, d)                          \
  if ((src0.order() != 0) && (((src0).dim(d) != (src1).dim(d)) ||	\
          ((src0).dim(d) != (src2).dim(d))))                            \
    idx_compatibility_error3(src0,src1,src2,				\
			     "expected same sizes in dimension "<<d);
//! Calls eblerror if src0.dim(d) and src1.dim(d) and src2.dim(d)
//! and src3.dim(d) don't match
#define idx_checkdim4_all(src0, src1, src2, src3, d)                    \
  if (((src0).dim(d) != (src1).dim(d)) ||                               \
          ((src0).dim(d) != (src2).dim(d)) ||                           \
      ((src0).dim(d) != (src3).dim(d)))                                 \
    idx_compatibility_error4(src0,src1,src2,src3,                       \
			     "expected same sizes in dimension "<<d);
//! Calls eblerror if src0.dim(d) and src1.dim(d) and src2.dim(d)
//! and src3.dim(d) and src4.dim(d) don't match
#define idx_checkdim5_all(src0, src1, src2, src3, src4, d)              \
  if (((src0).dim(d) != (src1).dim(d)) ||                               \
          ((src0).dim(d) != (src2).dim(d)) ||                           \
      ((src0).dim(d) != (src3).dim(d)) ||                               \
      ((src0).dim(d) != (src4).dim(d)))                                 \
    idx_compatibility_error5(src0,src1,src2,src3,src4,			\
			     "expected same sizes in dimension "<<d);
//! Calls eblerror if src0.dim(d) and src1.dim(d) and src2.dim(d)
//! and src3.dim(d) and src4.dim(d) amd src5.dim(d) don't match
#define idx_checkdim6_all(src0, src1, src2, src3, src4, src5, d)	\
  if (((src0).dim(d) != (src1).dim(d)) ||                               \
          ((src0).dim(d) != (src2).dim(d)) ||                           \
      ((src0).dim(d) != (src3).dim(d)) ||                               \
      ((src0).dim(d) != (src4).dim(d)) ||                               \
      ((src0).dim(d) != (src5).dim(d)))                                 \
    idx_compatibility_error6(src0,src1,src2,src3,src4,src5,             \
			     "expected same sizes in dimension "<<d);

// looping macros ////////////////////////////////////////////////////////////

//! cidxN_bloopX: macros to loop simultaneously over elements
//! of X idx'es of order at least N. Can be used as follows:
//! { double *z0, *z1;
//!   intg i;
//!   cidx1_bloop2(i, z0, myidx0, z1, myidx1) { *z0 = *z1 + 4; }
//! }
//! { float *z0;
//!   intg i,j;
//!   cidx2_bloop1(i, j, z0, myidx0) { *z0 *= 2; }
//! }
//! Variable i is a loop index, myidx0 is an idx of any type whose
//! order must be at least 1, and and z0 is a pointer to the numerical
//! type of myidx0.
//! It is best to enclose each cidx1_bloopX in its own brace scope
//! because the macro creates temprary variables with a fixed name.
//! These macros should be used over the idx_aloopX macros whenever
//! possible, because they are considerably more efficient.
//! unlike the aloop macros, these macros manipulate regular pointers
//! with simple incrementation, as opposed to iterators with complicated
//! logic.
#define cidx1_bloop1(i,p0,src0)					\
  if ((src0).order() < 1) eblerror("idx has wrong order");	\
  intg _n0 = (src0).dim(0), _m0 = (src0).mod(0);		\
  for (i=0, p0=(src0).idx_ptr(); i<_n0; i++, p0+=_m0)

#define cidx1_bloop2(i,p0,src0,p1,src1)					\
  if (((src0).order() < 1)||((src1).order() < 1))			\
    eblerror("idx has wrong order");					\
  intg _n0 = (src0).dim(0), _m0 = (src0).mod(0); _m1 = (src1).mod(0);	\
  idx_checkdim2_all(src0,src1,0)					\
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr();			\
       i<_n0;								\
       i++, p0+=_m0, p1+=_m1)

#define cidx1_bloop3(i,p0,src0,p1,src1,p2,src2)				\
  intg _n0 = (src0).dim(0), _m0 = (src0).mod(0);			\
  intg _m1 = (src1).mod(0), _m2 = (src2).mod(0);			\
  idx_checkdim3_all(src0,src1,src2,0)					\
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr(), p2=(src2).idx_ptr(); \
       i<_n0;								\
       i++, p0+=_m0, p1+=_m1, p2+=_m2)

#define cidx1_bloop4(i,p0,src0,p1,src1,p2,src2,p3,src3)			\
  intg _n0 = (src0).dim(0), _m0 = (src0).mod(0), _m1 = (src1).mod(0);	\
  intg _m2 = (src2).mod(0), _m3 = (src3).mod(0);			\
  idx_checkdim4_all(src0,src1,src2,src3,0)				\
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr(),			\
           p2=(src2).idx_ptr(), p3=(src3).idx_ptr();			\
       i<_n0;								\
       i++, p0+=_m0, p1+=_m1, p2+=_m2, p3+=_m3)

#define cidx2_bloop1(i,j,p0,src0)					\
  if ((src0).order() < 2) eblerror("idx has wrong order");		\
  intg _n00 = (src0).dim(0), _m00 = (src0).mod(0);			\
  intg _n01 = (src0).dim(1), _m01 = (src0).mod(1);			\
  for (i=0, p0=(src0).idx_ptr(); i<_n00; i++, p0+=_m00-_n01*_m01)	\
    for (j=0; i<_n01; j++, p0+=_m01)

#define cidx2_bloop2(i,j,p0,src0,p1,src1)                       \
  if ((src0).order() < 2) eblerror("idx has wrong order");      \
  intg _n00 = (src0).dim(0), _m00 = (src0).mod(0);              \
  intg _n01 = (src0).dim(1), _m01 = (src0).mod(1);              \
  intg _n10 = (src1).dim(0), _m10 = (src1).mod(0);              \
  intg _n11 = (src1).dim(1), _m11 = (src1).mod(1);              \
  idx_checkdim2_all(src0,src1,0)                                \
  idx_checkdim2_all(src0,src1,1)                                \
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr();           \
       i<_n00;                                                  \
       i++, p0+=_m00-_n01*_m01, p1+=_m10-_n11*_m11)             \
    for (j=0; i<_n01; j++, p0+=_m01, p1+=_m11)

#define cidx2_bloop3(i,j,p0,src0,p1,src1,p2,src2)			\
  if ((src0).order() < 2) eblerror("idx has wrong order");		\
  intg _n00 = (src0).dim(0), _m00 = (src0).mod(0);			\
  intg _n01 = (src0).dim(1), _m01 = (src0).mod(1);			\
  intg _n10 = (src1).dim(0), _m10 = (src1).mod(0);			\
  intg _n11 = (src1).dim(1), _m11 = (src1).mod(1);			\
  intg _n20 = (src2).dim(0), _m20 = (src2).mod(0);			\
  intg _n21 = (src2).dim(1), _m21 = (src2).mod(1);			\
  idx_checkdim3_all(src0,src1,src2,0)					\
  idx_checkdim3_all(src0,src1,src2,1)					\
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr(), p2=(src2).idx_ptr(); \
       i<_n00;								\
       i++, p0+=_m00-_n01*_m01, p1+=_m10-_n11*_m11, p2+=_m20-_n21*_m21) \
    for (j=0; i<_n01; j++, p0+=_m01, p1+=_m11, p2+=_m21)

////////////////////////////////////////////////////////////////

//! call these macros like this:
//! { idx_bloop1(la, a) { r += la.get(); } }
//! exmaple: matrix-vector product  a x b -> c
//! { idx_bloop3(la, a, lb, b, lc, c) { dot_product(la,lb,lc); } }
//! It's advisable to encase every bloop in its own scope to
//! prevent name clashes if the same loop variable is used
//! multiple times in a scope.

// bloop/eloop macros

// Okay, either C++ really suxx0rz or I'm a really st00pid Lisp-head.
// Why can't we define syntax-changing macros like in Lisp? Basically,
// I can't encapsulate the allocation
// of temporary variable for a loop inside the macro, unless
// I define a "begin" macro and an "end" macro. I would like
// to define idx_bloop so I can do:
// idx_bloop2(lm, m, lv, v) { idx_dot(lm,lm,v); }
// but I can't do that because I have to allocate lm and lv
// inside the macro, hence I need to know the type.
// Now the call would be:
// idx_bloop2(lm, m, double, lv, v, double) { idx_dot(lm,lm,v); }
// But that still doesn't quite work because if I declare lm and lv
// then I can't reuse the same symbols for another loop in the same
// scope. The only way out is to force the user to encase every
// bloop call inside braces, or to not reuse the same synbol twice
// for a looping idx. I thought about generating a mangled name
// but couldn't find a way to make it useful.
// If a macro could define its own scope that would be great.

#define idx_bloop1(dst0,src0,type0)		\
  idxlooper<type0> dst0(src0,0);		\
  for ( ; dst0.notdone(); dst0.next())

#define idx_bloop2(dst0,src0,type0,dst1,src1,type1)	\
  idx_checkdim2_all(src0, src1, 0);			\
  idxlooper<type0> dst0(src0,0);			\
  idxlooper<type1> dst1(src1,0);			\
  for ( ; dst0.notdone(); dst0.next(), dst1.next())

#define idx_bloop3(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2)	\
  idx_checkdim3_all(src0, src1, src2, 0);				\
  idxlooper<type0> dst0(src0,0);					\
  idxlooper<type1> dst1(src1,0);					\
  idxlooper<type2> dst2(src2,0);					\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next())

#define idx_bloop4(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,	\
		   dst3,src3,type3)					\
  idx_checkdim4_all(src0, src1, src2, src3, 0);				\
  idxlooper<type0> dst0(src0,0);					\
  idxlooper<type1> dst1(src1,0);					\
  idxlooper<type2> dst2(src2,0);					\
  idxlooper<type3> dst3(src3,0);					\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next(), dst3.next())

#define idx_bloop5(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,	\
		   dst3,src3,type3,dst4,src4,type4)			\
  idx_checkdim5_all(src0, src1, src2, src3, src4, 0);			\
  idxlooper<type0> dst0(src0,0);					\
  idxlooper<type1> dst1(src1,0);					\
  idxlooper<type2> dst2(src2,0);					\
  idxlooper<type3> dst3(src3,0);					\
  idxlooper<type4> dst4(src4,0);					\
  for ( ; dst0.notdone();						\
	dst0.next(), dst1.next(), dst2.next(), dst3.next(), dst4.next())

#define idx_bloop6(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,	\
		   dst3,src3,type3,dst4,src4,type4,dst5,src5,type5)	\
  idx_checkdim6_all(src0, src1, src2, src3, src4, src5, 0);		\
  idxlooper<type0> dst0(src0,0);					\
  idxlooper<type1> dst1(src1,0);					\
  idxlooper<type2> dst2(src2,0);					\
  idxlooper<type3> dst3(src3,0);					\
  idxlooper<type4> dst4(src4,0);					\
  idxlooper<type5> dst5(src5,0);					\
  for ( ; dst0.notdone();						\
	dst0.next(), dst1.next(), dst2.next(), dst3.next(), dst4.next(), \
            dst5.next())

// 1loop macros: loop on all but the 1st dimension

#define idx_1loop2(dst0,src0,type0,dst1,src1,type1,code) {      \
    uint src0o = src0.order();                                  \
    if (src0o == 1) {                                           \
      idx<type0> dst0 = src0;                                   \
      idx<type1> dst1 = src1;                                   \
      code                                                      \
          } else if (src0o == 2) {                              \
      idxlooper<type0> dst0(src0, 1);                           \
      idxlooper<type1> dst1(src1, 1);                           \
      for ( ; dst0.notdone(); dst0.next(), dst1.next()) {       \
	code                                                    \
            }                                                   \
    } else if (src0o == 3) {                                    \
      idxlooper<type0> src00(src0, 2);                          \
      idxlooper<type1> src11(src1, 2);                          \
      for ( ; src00.notdone(); src00.next(), src11.next()) {    \
	idxlooper<type0> dst0(src00, 1);                        \
	idxlooper<type1> dst1(src11, 1);                        \
	for ( ; dst0.notdone(); dst0.next(), dst1.next()) {     \
	  code                                                  \
              }                                                 \
      }                                                         \
    } else                                                      \
      eblerror("order " << src0o << " not implemented");        \
  }

// eloop macros

#define idx_eloop1(dst0,src0,type0)		\
  idxlooper<type0> dst0(src0,src0.order()-1);	\
  for ( ; dst0.notdone(); dst0.next())

#define idx_eloop2(dst0,src0,type0,dst1,src1,type1)			\
  if ((src0).dim((src0).order() - 1) != (src1).dim((src1).order() - 1)) \
    eblerror("incompatible idxs for eloop\n");				\
  idxlooper<type0> dst0(src0,(src0).order()-1);				\
  idxlooper<type1> dst1(src1,(src1).order()-1);				\
  for ( ; dst0.notdone(); dst0.next(), dst1.next())

#define idx_eloop3(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2)     \
  if (((src0).dim((src0).order() - 1) != (src1).dim((src1).order() - 1)) \
          || ((src0).dim((src0).order() - 1) != (src2).dim((src2).order() - 1))) \
    eblerror("incompatible idxs for eloop\n");				\
  idxlooper<type0> dst0(src0,(src0).order()-1);				\
  idxlooper<type1> dst1(src1,(src1).order()-1);				\
  idxlooper<type2> dst2(src2,(src2).order()-1);				\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next())

#define idx_eloop4(dst0,src0,type0,dst1,src1,type1,			\
		   dst2,src2,type2,dst3,src3,type3)			\
  if (((src0).dim((src0).order() - 1) != (src1).dim((src1).order() - 1)) \
          || ((src0).dim((src0).order() - 1) != (src2).dim((src2).order() - 1)) \
      || ((src0).dim((src0).order() - 1) != (src3).dim((src3).order() - 1))) \
    eblerror("incompatible idxs for eloop\n");				\
  idxlooper<type0> dst0(src0,(src0).order()-1);				\
  idxlooper<type1> dst1(src1,(src1).order()-1);				\
  idxlooper<type2> dst2(src2,(src2).order()-1);				\
  idxlooper<type3> dst3(src3,(src3).order()-1);				\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next(), dst3.next())

////////////////////////////////////////////////////////////////
// aloop macros: loop over all elements

// Loops over all elements of an idx. This takes a pointer to
// the data type of idx elements, and a blank idxiter object:
// idx_aloop1(data_pointer,idxiter,&idx) { do_stuff(data_pointer); }
// Example of use: add 1 to all element of m:
//  idx<double> m(3,4);
//  idxiter<double> p;
//  idx_aloop1(p,&m) { *p += 1; }
#define idx_aloop1_on(itr0,src0)			\
  for ( itr0.init(src0); itr0.notdone(); itr0.next())

// this loops simultaneously over all elements of 2 idxs.
// The two idxs can have different structures as long as they have
// the same total number of elements.
#define idx_aloop2_on(itr0,src0,itr1,src1)	\
  idx_checknelems2_all(src0, src1);		\
  for ( itr0.init(src0), itr1.init(src1);	\
	itr0.notdone();				\
	itr0.next(), itr1.next())

#define idx_aloop3_on(itr0,src0,itr1,src1,itr2,src2)		\
  idx_checknelems3_all(src0, src1, src2);			\
  for (itr0.init(src0), itr1.init(src1), itr2.init(src2);	\
       itr0.notdone();						\
       itr0.next(), itr1.next(), itr2.next())

// high level aloop macros.
// These should be enclosed in braces, to avoid name clashes
#define idx_aloop1(itr0,src0,type0)		\
  idxiter<type0> itr0;				\
  idx_aloop1_on(itr0,src0)

#define idx_aloop2(itr0,src0,type0,itr1,src1,type1)	\
  idxiter<type0> itr0;					\
  idxiter<type1> itr1;					\
  idx_checknelems2_all(src0, src1);			\
  for (itr0.init(src0), itr1.init(src1);		\
       itr0.notdone();					\
       itr0.next(), itr1.next())

#define idx_aloop3(itr0,src0,type0,itr1,src1,type1,itr2,src2,type2)	\
  idxiter<type0> itr0;							\
  idxiter<type1> itr1;							\
  idxiter<type2> itr2;							\
  idx_checknelems3_all(src0, src1, src2);				\
  for (itr0.init(src0), itr1.init(src1), itr2.init(src2);		\
       itr0.notdone();							\
       itr0.next(), itr1.next(), itr2.next())

#define idx_aloop4(itr0,src0,type0,itr1,src1,type1,itr2,src2,type2,	\
		   itr3,src3,type3)					\
  idxiter<type0> itr0;							\
  idxiter<type1> itr1;							\
  idxiter<type2> itr2;							\
  idxiter<type3> itr3;							\
  idx_checknelems4_all(src0, src1, src2, src3);				\
  for (itr0.init(src0), itr1.init(src1), itr2.init(src2), itr3.init(src3); \
       itr0.notdone();							\
       itr0.next(), itr1.next(), itr2.next(), itr3.next())

////////////////////////////////////////////////////////////////
// an idxlooper is an iterator for midx.
// It is actually a subclass of idx.
// These are not C++ iterators in the classical sense.

class dummyt {  bool someunk; };

template <class T>
idxlooper<T>::idxlooper(idx<T> &m, int ld) : idx<T>((dummyt*)0) {
  if (m.order() == 0) // TODO: allow looping once on 0-order idx
    eblerror("cannot loop on idx with order 0. idx is: " << m);
  i = 0;
  dimd = m.spec.dim[ld];
  modd = m.spec.mod[ld];
  m.spec.select_into(&(this->spec), ld, i);
  this->storage = m.storage;
  this->storage->lock();
}

// like ++
// CAUTION: this doesn't do array bound checking
// because we coudn't use a for loop if it did.
template <class T> T *idxlooper<T>::next() {
  i++;
  this->spec.offset += modd;
  return this->storage->data + this->spec.offset;
}

// return true when done.
template <class T> bool idxlooper<T>::notdone() { return ( i < dimd ); }

////////////////////////////////////////////////////////////////
// a pointer that loops over all elements
// of an idx

// empty constructor;
template <class T> idxiter<T>::idxiter() { }

template <class T> T *idxiter<T>::init(const idx<T> &m) {
  iterand = &m;
  i = 0;
  j = iterand->spec.ndim;
  data = iterand->storage->data + iterand->spec.offset;
  n = iterand->spec.nelements();
  if (iterand->spec.contiguousp()) {
    d[0] = -1;
  } else {
    for(int i=0; i < iterand->spec.ndim; i++) { d[i] = 0; }
  }
  return data;
}

template <class T> T *idxiter<T>::next() {
  i++;
  if (d[0] < 0) {
    // contiguous idx
    data++;
  } else {
    // non-contiguous idx
    j--;
    do {
      if (j<0) {
        break;
      }
      if (++d[j] < iterand->spec.dim[j]) {
        data += iterand->spec.mod[j];
        j++;
      } else {
        data -= iterand->spec.dim[j] * iterand->spec.mod[j];
        d[j--] = -1;
      }
    } while (j < iterand->spec.ndim);
  }
  return data;
}

//template <class T> bool idxiter<T>::notdone() { return done; }
template <class T> bool idxiter<T>::notdone() { return (i < n); }

} // end namespace ebl

#endif /* IDXITER_HPP_*/
