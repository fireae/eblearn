/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun, Pierre Sermanet *
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

namespace ebl {

// module_1_1 ////////////////////////////////////////////////////////////////

template <typename T>
module_1_1<T>::module_1_1(const char *name, bool bresize_)
    : module(name), bresize(bresize_), memoptimized(false),
      bmstate_input(false), bmstate_output(false), ninputs(1),
      noutputs(1){
}

template <typename T>
module_1_1<T>::~module_1_1() {
  EDEBUG("deleting module_1_1: " << _name);
}

// single propagation methods ////////////////////////////////////////////////

template <typename T>
void module_1_1<T>::fprop1(idx<T> &in, idx<T> &out) {
  eblerror("not implemented, " << this->describe()); }

template <typename T>
void module_1_1<T>::bprop1(state<T> &in, state<T> &out) {
  eblerror("not implemented, " << this->describe()); }

template <typename T>
void module_1_1<T>::bbprop1(state<T> &in, state<T> &out) {
  eblerror("not implemented, " << this->describe()); }

// multi-state methods ///////////////////////////////////////////////////////

template <typename T>
void module_1_1<T>::fprop(state<T> &in, state<T> &out) {
  if (in.x.size() == 0) eblerror("input should have at least 1 tensor");
  // make sure out has same # of tensors and orders
  this->resize_output_orders(in, out);
  // run regular fprop on each states
  for (uint i = 0; i < in.x.size(); ++i) {
    EDEBUG(this->name() << ": fprop at tensor " << i << " in: "
           << in << " and out: " << out);
    fprop1(in.x[i], out.x[i]);
  }
  // remember number of input/outputs
  ninputs = in.x.size();
  noutputs = out.x.size();
}

template <typename T>
void module_1_1<T>::bprop(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // run regular bbprop on each states
  for (int i = (int) in.dx.size() - 1; i >= 0; --i) {
    state<T> sin = in.get_dx(i);
    state<T> sout = out.get_dx(i);
    EDEBUG(this->name() << ": bprop in.dx[" << i << " " << sin
           << " min " << idx_min(sin) << " max " << idx_max(sin)
           << " out.dx[" << i << "] " << sout
           << " min " << idx_min(sout) << " max " << idx_max(sout));
    bprop1(sin, sout);
  }
}

template <typename T>
void module_1_1<T>::bbprop(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // run regular bbprop on each states
  for (int i = (int) in.ddx.size() - 1; i >= 0; --i) {
    state<T> inbb = in.get_ddx(i);
    state<T> outbb = out.get_ddx(i);
    bbprop1(inbb, outbb);
  }
}

// dumping ///////////////////////////////////////////////////////////////////

template <typename T>
void module_1_1<T>::fprop1_dump(idx<T> &in, idx<T> &out) {
  fprop1(in, out); // no dumping by default, just fproping.
}

template <typename T>
void module_1_1<T>::fprop_dump(state<T> &in, state<T> &out) {
  if (in.x.size() == 0) eblerror("input should have at least 1");
  // make sure out has same # of tensors and orders
  resize_output_orders(in, out);
  // run regular fprop_dump on each states
  for (uint i = 0; i < in.x.size(); ++i) {
    EDEBUG(this->name() << ": in.x " << in << ", min " << idx_min(in)
           << " max " << idx_max(in));
    this->fprop1_dump(in, out);
  }
  // remember number of input/outputs
  ninputs = in.x.size();
  noutputs = out.x.size();
}

// weights manipulation ////////////////////////////////////////////////////

template <typename T>
void module_1_1<T>::forget(forget_param_linear& fp) {
}

template <typename T>
void module_1_1<T>::normalize() {
}

template <typename T>
void module_1_1<T>::zero_dx() {
}

template <typename T>
void module_1_1<T>::zero_ddx() {
}

template <typename T>
int module_1_1<T>::replicable_order() { return -1; }

template <typename T>
bool module_1_1<T>::ignored1(idx<T> &in, idx<T> &out) {
  if (this->_enabled) return false;
  idx_copy(in, out);
  return true;
}

// resizing //////////////////////////////////////////////////////////////////

template <typename T>
void module_1_1<T>::resize_output_orders(state<T> &in, state<T> &out) {
  out.resize_forward_orders(in, 1);
}

template <typename T>
bool module_1_1<T>::resize_output(idx<T> &in, idx<T> &out, idxdim *d) {
  //if (!this->bresize) return false; // no resizing
  if (&in == &out) {
    outdims = out.get_idxdim(); // remember out size
    return false; // resize only when in and out are different
  }
  TIMING_RESIZING_ACCSTART(); // start accumulating resizing time
  if (d) { // use d as target dims
    outdims = *d; // remember out size
    if (d->order() != out.order()) { // re-allocate buffer
      EDEBUG(this->name() << ": reallocating output from " << out << " to "<<d);
      out = idx<T>(*d);
    } else if (*d != out.get_idxdim()) { // resize buffer
      EDEBUG(this->name() << ": resizing output from " << out << " to " << *d);
      out.resize(*d);
    } else {
      TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
      return false;
    }
  } else { // use in as target dims
    outdims = in.get_idxdim(); // remember out size
    if (in.order() != out.order() || !out.contiguousp()) { // re-allocate buffer
      EDEBUG(this->name() << ": reallocating output from " << out
             << " to " << in.get_idxdim());
      out = idx<T>(in.get_idxdim());
    } else if (in.get_idxdim() != out.get_idxdim()) { // resize buffer
      EDEBUG(this->name() << ": resizing output from " << out << " to "
             << in.get_idxdim());
      out.resize(in.get_idxdim());
    } else {
      TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
      return false;
    }
  }
  TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
  return true;
}

template <typename T>
fidxdim module_1_1<T>::fprop1_size(fidxdim &isize) {
  return isize;
}

template <typename T>
fidxdim module_1_1<T>::bprop1_size(const fidxdim &osize) {
  //EDEBUG(this->name() << ": " << osize << " -> same");
  return osize;
}

template <typename T>
mfidxdim module_1_1<T>::fprop_size(mfidxdim &isize) {
  EDEBUG(this->name() << ": " << isize << " f-> ...");
  mfidxdim osize;
  for (uint i = 0; i < isize.size(); ++i)
    if (!isize.exists(i)) osize.push_back_empty();
    else osize.push_back(this->fprop1_size(isize[i]));
  EDEBUG(this->name() << ": " << isize << " f-> " << osize);
  return osize;
}

// template <typename T>
// mfidxdim module_1_1<T>::bprop_size(const mfidxdim &osize) {
//   EDEBUG(this->name() << ": " << osize << " -> ...");
//   mfidxdim isize;
//   for (mfidxdim::const_iterator i = osize.begin(); i != osize.end(); ++i) {
//     if (i.exists())
// 	isize.push_back(this->bprop1_size(*i));
//     else
// 	isize.push_back_empty();
//   }
//   EDEBUG(this->name() << ": " << osize << " -> " << isize);
//   return isize;
// }

template <typename T>
mfidxdim module_1_1<T>::bprop_size(mfidxdim &osize) {
  EDEBUG(this->name() << ": " << osize << " b-> ...");
  mfidxdim isize;
  for (mfidxdim::iterator i = osize.begin(); i != osize.end(); ++i) {
    if (i.exists())
      isize.push_back(this->bprop1_size(*i));
    else
      isize.push_back_empty();
  }
  EDEBUG(this->name() << ": " << osize << " b-> " << isize);
  return isize;
}

template <typename T>
std::string module_1_1<T>::pretty(idxdim &isize) {
  std::string s;
  fidxdim d = isize;
  s << " -> " << this->_name.c_str() << " -> " << fprop1_size(d);
  return s;
}

template <typename T>
std::string module_1_1<T>::pretty(mfidxdim &isize) {
  std::string s;
  midxdim d = fprop_size(isize);
  s << " -> " << this->_name.c_str() << " -> " << d;
  return s;
}

template <typename T>
module_1_1<T>* module_1_1<T>::copy(parameter<T> *p) {
  eblerror("deep copy not implemented in " << this->name());
  return NULL;
}

template <typename T>
bool module_1_1<T>::optimize_fprop(state<T> &in, state<T> &out){
  return true;
}

template <typename T>
void module_1_1<T>::load_x(idx<T> &weights) {
  err_not_implemented(); }

template <typename T>
module_1_1<T>* module_1_1<T>::last_module() {
  return this;
}

template <typename T>
bool module_1_1<T>::mstate_input() {
  return bmstate_input;
}

template <typename T>
bool module_1_1<T>::mstate_output() {
  return bmstate_output;
}

template <typename T>
uint module_1_1<T>::get_ninputs() {
  return ninputs;
}

template <typename T>
uint module_1_1<T>::get_noutputs() {
  return noutputs;
}

template <typename T>
idxdim module_1_1<T>::get_outdims() {
  return outdims;
}

template <typename T>
void module_1_1<T>::update_outdims(idx<T> &out) {
  outdims = out.get_idxdim();
}

// module_2_1 //////////////////////////////////////////////////////////////////

template <typename T> module_2_1<T>::module_2_1(const char *name_)
    : module(name_), bresize(true) {
}

template <typename T> module_2_1<T>::~module_2_1() {
}

// first state tensor propagation //////////////////////////////////////////////

template <typename T>
void module_2_1<T>::fprop1(idx<T> &in1, idx<T> &in2, idx<T> &out) {
  err_not_implemented(); }

template <typename T>
void module_2_1<T>::bprop1(state<T> &in1, state<T> &in2, state<T> &out) {
  err_not_implemented(); }

template <typename T>
void module_2_1<T>::bbprop1(state<T> &in1, state<T> &in2, state<T> &out) {
  err_not_implemented(); }

// full state propagation //////////////////////////////////////////////////////

template <typename T>
void module_2_1<T>::fprop(state<T> &in1, state<T> &in2, state<T> &out) {
  // check that in/out have at least 1 state and the same number of them.
  if (in1.x.size() == 0 || in2.x.size() == 0 || out.x.size() == 0
      || in1.x.size() != out.x.size() || in2.x.size() != out.x.size())
    eblerror("in1, in2 and out don't have at least 1 state or don't have the "
             << "same number of states: in1: " << in2.x.size()
             << " in2: " << in2.x.size() << " out: " << out.x.size());
  // run regular fprop on each state
  for (uint i = 0; i < in1.x.size(); ++i)
    fprop1(in1.x[i], in2.x[i], out.x[i]);
}

template <typename T>
void module_2_1<T>::bprop(state<T> &in1, state<T> &in2, state<T> &out) {
  DEBUG_CHECK_DX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DX(in2); // in debug mode, check backward tensors are allocated
  // check that in/out have at least 1 state and the same number of them.
  if (in1.dx.size() == 0 || in2.dx.size() == 0 || out.dx.size() == 0
      || in1.dx.size() != out.dx.size() || in2.dx.size() != out.dx.size())
    eblerror("in1, in2 and out don't have at least 1 state or don't have the "
             << "same number of states: in1: " << in2.dx.size()
             << " in2: " << in2.dx.size() << " out: " << out.dx.size());
  // run regular bprop on each state
  for (uint i = 0; i < in1.x.size(); ++i) {
    state<T> b1 = in1.get_dx(i);
    state<T> b2 = in2.get_dx(i);
    state<T> bo = out.get_dx(i);
    bprop1(b1, b2, bo);
  }
}

template <typename T>
void module_2_1<T>::bbprop(state<T> &in1, state<T> &in2, state<T> &out) {
  DEBUG_CHECK_DDX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DDX(in2); // in debug mode, check backward tensors are allocated
  // check that in/out have at least 1 state and the same number of them.
  if (in1.ddx.size() == 0 || in2.ddx.size() == 0 || out.ddx.size() == 0
      || in1.ddx.size() != out.ddx.size() || in2.ddx.size() != out.ddx.size())
    eblerror("in1, in2 and out don't have at least 1 state or don't have the "
             << "same number of states: in1: " << in2.ddx.size()
             << " in2: " << in2.ddx.size() << " out: " << out.ddx.size());
  // run regular bprop on each state
  for (uint i = 0; i < in1.x.size(); ++i) {
    state<T> b1 = in1.get_ddx(i);
    state<T> b2 = in2.get_ddx(i);
    state<T> bo = out.get_ddx(i);
    bbprop1(b1, b2, bo);
  }
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
void module_2_1<T>::forget(forget_param_linear &fp) { err_not_implemented(); }

template <typename T>
void module_2_1<T>::normalize() { err_not_implemented(); }

template <typename T>
bool module_2_1<T>::resize_output(idx<T> &in1, idx<T> &in2, idx<T> &out,
                                  idxdim *d) {
  if (!bresize) return false; // no resizing
  if (&in1 == &out) return false; // resize only when in and out are different
  if (d) { // use d as target dims
    if (d->order() != out.order()) { // re-allocate buffer
      EDEBUG(this->name() << ": reallocating output from " << out
             << " to " << d);
      out = idx<T>(*d);
    } else if (*d != out.get_idxdim()) { // resize buffer
      EDEBUG(this->name() << ": resizing output from " << out << " to "
             << *d);
      out.resize(*d);
    } else return false;
  } else { // use in as target dims
    if (in1.order() != out.order()) { // re-allocate buffer
      EDEBUG(this->name() << ": reallocating output from " << out
             << " to " << in1.get_idxdim());
      out = idx<T>(in1.get_idxdim());
    } else if (in1.get_idxdim() != out.get_idxdim()) { // resize buffer
      EDEBUG(this->name() << ": resizing output from " << out << " to "
             << in1.get_idxdim());
      out.resize(in1.get_idxdim());
    } else return false;
  }
  if (in2.get_idxdim() != in1.get_idxdim())
    eblerror("expected same size inputs " << in1 << " and " << in2);
  return true;
}

// generic replicable modules classes //////////////////////////////////////////

// check that orders of input and module are compatible
template <typename T>
void check_replicable_orders(module_1_1<T> &m, state<T>& in) {
  if (in.order() < 0)
    eblerror("module_1_1_replicable cannot replicate this module (order -1)");
  if (in.order() < m.replicable_order())
    eblerror("input order must be >= to module's operating order, input is "
             << in << " but module operates with order "<<m.replicable_order());
  if (in.order() > MAXDIMS)
    eblerror("cannot replicate using more dimensions than MAXDIMS");
}

//! recursively loop over the last dimensions of input in and out until
//! reaching the operating order, then call the original fprop of module m.
template <class Tmodule, typename T>
void module_eloop2_fprop(Tmodule &m, state<T> &in, state<T> &out) {
  if (m.replicable_order() == in.order()) {
    m.Tmodule::fprop(in, out);
  } else if (m.replicable_order() > in.order()) {
    eblerror("input order must be >= to module's operating order, input is "
             << in << " but module operates with order "
             << m.replicable_order());
  } else {
    state_eloop2(iin, in, T, oout, out, T) {
      module_eloop2_fprop<Tmodule,T>(m, (state<T>&) iin, (state<T>&) oout);
    }
  }
}

//! recursively loop over the last dimensions of input in and out until
//! reaching the operating order, then call the original bprop of module m.
template <class Tmodule, typename T>
void module_eloop2_bprop(Tmodule &m, state<T> &in, state<T> &out) {
  if (m.replicable_order() == in.order()) {
    m.Tmodule::bprop(in, out);
  } else if (m.replicable_order() > in.order()) {
    eblerror("the order of the input should be greater or equal to module's"
             << " operating order");
  } else {
    state_eloop2(iin, in, T, oout, out, T) {
      module_eloop2_bprop<Tmodule,T>(m, (state<T>&) iin, (state<T>&) oout);
    }
  }
}

//! recursively loop over the last dimensions of input in and out until
//! reaching the operating order, then call the original bbprop of module m.
template <class Tmodule, typename T>
void module_eloop2_bbprop(Tmodule &m, state<T> &in, state<T> &out) {
  if (m.replicable_order() == in.order()) {
    m.Tmodule::bbprop(in, out);
  } else if (m.replicable_order() > in.order()) {
    eblerror("the order of the input should be greater or equal to module's"
             << " operating order");
  } else {
    state_eloop2(iin, in, T, oout, out, T) {
      module_eloop2_bbprop<Tmodule,T>(m, (state<T>&) iin, (state<T>&) oout);
    }
  }
}

template <class Tmodule, typename T>
module_1_1_replicable<Tmodule,T>::module_1_1_replicable(Tmodule &m)
    : module(m) {
}

template <class Tmodule, typename T>
module_1_1_replicable<Tmodule,T>::~module_1_1_replicable() {
}

template <class Tmodule, typename T>
void module_1_1_replicable<Tmodule,T>::fprop(state<T> &in, state<T> &out) {
  check_replicable_orders(module, in); // check for orders compatibility
  module.resize_output(in, out); // resize output
  module_eloop2_fprop<Tmodule,T>(module, in, out);
}

template <class Tmodule, typename T>
void module_1_1_replicable<Tmodule,T>::bprop(state<T> &in, state<T> &out) {
  check_replicable_orders(module, in); // check for orders compatibility
  module_eloop2_bprop<Tmodule,T>(module, in, out);
}

template <class Tmodule, typename T>
void module_1_1_replicable<Tmodule,T>::bbprop(state<T> &in, state<T> &out){
  check_replicable_orders(module, in); // check for orders compatibility
  module_eloop2_bbprop<Tmodule,T>(module, in, out);
}

} // end namespace ebl
