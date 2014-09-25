/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun, Pierre Sermanet, Koray Kavukcuoglu *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com, koray@cs.nyu.edu *
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

#ifndef EBL_TESTER_H_
#define EBL_TESTER_H_

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <ostream>

#include "libidx.h"
#include "ebl_state.h"
#include "ebl_basic.h"
#include "ebl_arch.h"

namespace ebl {

//! Tests any module that is descendant of module_1_1. (includes layers)
//! bprop to input and to parameters are validated with finite difference
//! approximations.
//!
//! This class is not meant to replace the testing framework for eblearn
//! developers but it should be useful for eblearn users that create new
//! modules using this library as a binary reference
template <class T> class module_tester {
 public:
  //! out    : reference to ostream to push results
  //! thres  : threshold to decide bprop and finite diff jacobian are same or
  //!          different this might be tricky because if the modules
  //!          accumulates derivatives, then the difference will inevitably
  //!          grow, so user has to have a sense of what is valid
  //!          default : 1e-8
  //! rrange : range that is used to randomize the input state and parameter
  //!          objects. if this value is very small, then the jacobians will
  //!          always be very similar
  module_tester(double thres = 1e-6, T rrange_min = -1.0, T rrange_max = 1.0,
                FILE* out = stdout);
  virtual ~module_tester();

  //! Test the 1st derivatives of 'in' w.r.t. 'out' with module 'm'.
  //! This returns an idx with 2 elements, 0: maximum error 1: total error.
  idx<double> test_jacobian(module_1_1<T> &m, state<T> &in, state<T> &out);
  //! Test the 1st derivatives of 'out' w.r.t. parameters 'p' of module 'm'.
  //! \param p The parameter object that was used to allocate the
  //!          parameters of module. this parameter object should contain the
  //!          parameter of only this module.
  //! This returns an idx with 2 elements, 0: maximum error 1: total error.
  idx<double> test_jacobian_param(parameter<T> &p, module_1_1<T>& m,
                                  state<T> &in, state<T> &out);
  //! Test the 2nd derivatives of 'in' w.r.t. 'out' with module 'm'.
  //! This returns an idx with 2 elements, 0: maximum error 1: total error.
  idx<double> test_hessian(module_1_1<T> &m, state<T> &in, state<T> &out);
  //! Test the 2nd derivatives of 'out' w.r.t. parameters 'p' of module 'm'.
  //! \param p The parameter object that was used to allocate the
  //!          parameters of module. this parameter object should contain the
  //!          parameter of only this module.
  //! This returns an idx with 2 elements, 0: maximum error 1: total error.
  idx<double> test_hessian_param(parameter<T> &p, module_1_1<T>& m,
                                 state<T> &in, state<T> &out);
  //! get accuracy threshold
  double get_acc_thres() const;
  //! set accuracy threshold
  void set_acc_thres(double acc_thres);
  //! get random range
  T get_rrange() const;
  //! set random range
  void set_rrange(T rrange);
  //! get ostream used
  FILE* get_out() const;
  //! set ostream
  void set_out(FILE* out);

  // protected methods ///////////////////////////////////////////////////////
 protected:

  //! Get jacobian using 1st order central finite differnce approximation for
  //! derivative of 'out' w.r.t. 'in' using module 'm'.
  void get_jacobian_fprop(module_1_1<T> &m, state<T> &in,
                          state<T> &out, idx<T>& jac);
  //! Get jacobian using 1st order central finite differnce approximation for
  //! derivative of 'out' w.r.t. 'p' using module 'm'.
  //! \param p The parameter object that was used to allocate the parameters
  //!   of module. This should contain the parameter of this module only.
  void get_jacobian_fprop_param(parameter<T> &p, module_1_1<T> &m, state<T> &in,
                                state<T> &out, idx<T>& jac);
  //! Get jacobian using bprop for derivative of 'out' w.r.t. 'in'.
  void get_jacobian_bprop(module_1_1<T> &m, state<T> &in,
                          state<T> &out, idx<T>& jac);
  //! Get jacobian using bprop for derivative of 'out' w.r.t. 'p'.
  //! \param p The parameter object that was used to allocate the parameters
  //!   of module. This should contain the parameter of this module only.
  void get_jacobian_bprop_param(parameter<T> &p, module_1_1<T> &m, state<T> &in,
                                state<T> &out, idx<T>& jac);

  //! Get hessian using 1st order central finite differnce approximation for
  //! derivative of 'out' w.r.t. 'in' using module 'm'.
  void get_hessian_fprop(module_1_1<T> &m, state<T> &in,
                         state<T> &out, idx<T>& jac);
  //! Get hessian using 1st order central finite differnce approximation for
  //! derivative of 'out' w.r.t. 'p' using module 'm'.
  //! \param p The parameter object that was used to allocate the parameters
  //!   of module. This should contain the parameter of this module only.
  void get_hessian_fprop_param(parameter<T> &p, module_1_1<T> &m, state<T> &in,
                               state<T> &out, idx<T>& jac);
  //! Get hessian using bprop for derivative of 'out' w.r.t. 'in'.
  void get_hessian_bprop(module_1_1<T> &m, state<T> &in,
                         state<T> &out, idx<T>& jac);
  //! Get hessian using bprop for derivative of 'out' w.r.t. 'p'.
  //! \param p The parameter object that was used to allocate the parameters
  //!   of module. This should contain the parameter of this module only.
  void get_hessian_bprop_param(parameter<T> &p, module_1_1<T> &m, state<T> &in,
                               state<T> &out, idx<T>& jac);

  //! Print error between 'a' and 'b' with message 'msg'.
  void report_err(idx<T>& a, idx<T>& b, const char* msg);
  //! Returns a 2-elements idx with 0: max error 1: total error.
  idx<double> get_errs(idx<T>& a, idx<T>& b);

  // protected members
 protected:
  double acc_thres; //!< accuracy threshold
  T rrange_min, rrange_max; //!< random initialization range
  FILE* out; //!< output stream
  idx<T> jac_fprop, jac_bprop, jac_bbprop; //!< Jacobians from *prop.
  idx<T> jac_pfprop, jac_pbprop, jac_pbbprop; //!< Jacobians wrt params.
  idx<T> hes_fprop, hes_bprop, hes_bbprop; //!< Hessians from *prop.
  idx<T> hes_pfprop, hes_pbprop, hes_pbbprop; //!< Hessians wrt params.
};

////////////////////////////////////////////////////////////////////////////////

//! Generic Jacobian tester.
template <class T> class Jacobian_tester {
 public:
  Jacobian_tester() {};
  ~Jacobian_tester() {};
  // this function take any module_1_1 with a fprop et bprop implemented,
  // and tests if the jacobian is correct (by pertubation)
  // (on a fstate_idx with 3 dimensions)
  void test(module_1_1<T> &module);
};

////////////////////////////////////////////////////////////////////////////////

//! Generic BBprop tester tester.
template <class T> class Bbprop_tester {
 public:
  Bbprop_tester() {};
  ~Bbprop_tester() {};
  // this function take any module_1_1 with a fprop et bbprop implemented,
  // and tests if the Bbprop is correct (by pertubation)
  // (on a fstate_idx with 3 dimensions)
  void test(module_1_1<T> &module);
};

////////////////////////////////////////////////////////////////////////////////

//! Generic Bprop tester.
template <class T> class Bprop_tester {
 public:
  Bprop_tester() {};
  ~Bprop_tester() {};
  // this function take any module_1_1 with a fprop et bprop implemented,
  // and tests if the bprop is correct (by pertubation)
  // (on a fstate_idx with 3 dimensions)
  void test(module_1_1<T> &module);
};

} // namespace ebl {

#include "ebl_tester.hpp"

#endif /* EBL_TESTER_H_ */
