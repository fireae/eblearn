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

#ifndef EBL_EBM_H_
#define EBL_EBM_H_

#include "libidx.h"
#include "ebl_defines.h"
#include "ebl_module.h"
#include "ebl_state.h"
#include "ebl_parameters.h"

#ifndef __NOSTL__
#include <vector>
#endif

namespace ebl {

// ebm_1 ///////////////////////////////////////////////////////////////////////

//! An abstract class for a module with one inputs and one energy output.
template <typename T> class ebm_1 : public module_1_1<T> {
 public:
  ebm_1(const char *name = "ebm_1");
  virtual ~ebm_1();

  // first state tensor propagation //////////////////////////////////////////

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &energy);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &energy);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &energy);

  // full state propagation //////////////////////////////////////////////////

  //! Forward propagation of all tensors from 'in' tensors to 'out' tensors.
  virtual void fprop(state<T> &in, state<T> &out);
  //! 1st order backward propagation of all tensors from out to in.
  virtual void bprop(state<T> &in, state<T> &out);
  //! 2nd order backward propagation of all tensors from out to in.
  virtual void bbprop(state<T> &in, state<T> &out);

  virtual void forget(forget_param_linear &fp);
  virtual void normalize();
};

// ebm_module_1_1 //////////////////////////////////////////////////////////////

//! A module containing both a module_1_1 that takes 1 input and produces
//! 1 output but that also an ebm_1 that produces an energy given module_1_1's
//! output.
template<typename T> class ebm_module_1_1 : public module_1_1<T> {
 public:
  //! Construct with module 'm' and ebm 'e'.
  //! This module is responsible for deleting 'm' and 'e' at destruction.
  ebm_module_1_1(module_1_1<T> *m, ebm_1<T> *e,
                 const char *name = "ebm_module_1_1");
  virtual ~ebm_module_1_1();

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

  virtual void forget(forget_param_linear &fp);
  //! Returns a reference to the energy output of fprop throught the ebm_1.
  virtual state<T>& get_energy();
  //! Modifies input dimensions 'isize' to be compliant with module's
  //! architecture, and returns corresponding output dimensions.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual fidxdim fprop1_size(fidxdim &isize);
  //! Returns input dimensions corresponding to output dimensions 'osize'.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual fidxdim bprop1_size(const fidxdim &osize);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

 protected:
  module_1_1<T>	*module;
  ebm_1<T>	*ebm;
  state<T>       energy;
};

// ebm_2 ///////////////////////////////////////////////////////////////////////

//! An abstract class for a module with two inputs and one energy output.
template <typename T> class ebm_2 : public module_2_1<T> {
 public:
  ebm_2(const char *name = "ebm_2");
  virtual ~ebm_2();

  // first state tensor propagation //////////////////////////////////////////

  //! Forward propagation from 'in1'/'in2' tensors to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in1, idx<T> &in2, idx<T> &energy);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in1, state<T> &in2, state<T> &energy);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in1, state<T> &in2, state<T> &energy);

  // full state propagation //////////////////////////////////////////////////

  //! Forward propagation of all tensors from 'in' tensors to 'out' tensors.
  virtual void fprop(state<T> &in1, state<T> &in2, state<T> &out);
  //! 1st order backward propagation of all tensors from out to in.
  virtual void bprop(state<T> &in1, state<T> &in2, state<T> &out);
  //! 2nd order backward propagation of all tensors from out to in.
  virtual void bbprop(state<T> &in1, state<T> &in2, state<T> &out);

  virtual void bprop1_copy(state<T> &i1, state<T> &i2, state<T> &energy);
  virtual void bprop2_copy(state<T> &i1, state<T> &i2, state<T> &energy);
  virtual void bbprop1_copy(state<T> &i1, state<T> &i2, state<T> &energy);
  virtual void bbprop2_copy(state<T> &i1, state<T> &i2, state<T> &energy);
  virtual void forget(forget_param_linear &fp);
  virtual void normalize();
  //! compute value of in1 that minimizes the energy, given in2
  virtual double infer1(state<T> &i1, state<T> &i2, state<T> &energy,
                        infer_param &ip);
  //! Compute and sets in2 that minimizes the energy, given in1.
  virtual void infer2(state<T> &i1, state<T> &i2, infer_param &ip,
                      state<T> *energy = NULL);
  virtual void infer2_copy(state<T> &i1, state<T> &i2, state<T> &energy);
};

// fc_ebm1 /////////////////////////////////////////////////////////////////////

//! standard 1 input EBM with one module-1-1, and one ebm-1 on top.
//! fc stands for "function+cost".
template<typename T> class EXPORT fc_ebm1 : public ebm_1<T> {
public:
  fc_ebm1(module_1_1<T> &fm, state<T> &fo, ebm_1<T> &fc);
  virtual ~fc_ebm1();

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &energy);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &energy);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &energy);

  virtual void forget(forget_param_linear &fp);

public:
  module_1_1<T>	&fmod;
  state<T>	&fout;
  ebm_1<T>	&fcost;
};

// fc_ebm2 /////////////////////////////////////////////////////////////////////

//! standard 2 input EBM with one module-1-1, and one ebm-2 on top.
//! fc stands for "function+cost".
template<typename T> class EXPORT fc_ebm2 : public ebm_2<T> {
public:
  fc_ebm2(module_1_1<T> &fm, state<T> &fo, ebm_2<T> &fc);
  virtual ~fc_ebm2();

  //! Forward propagation from 'in1'/'in2' tensors to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in1, idx<T> &in2, idx<T> &energy);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in1, state<T> &in2, state<T> &energy);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in1, state<T> &in2, state<T> &energy);

  virtual void forget(forget_param_linear &fp);
  //! Compute and sets in2 that minimizes the energy, given in1.
  virtual void infer2(state<T> &i1, state<T> &i2, infer_param &ip,
                      state<T> *energy = NULL);
public:
  module_1_1<T>	&fmod;
  state<T>	&fout;
  ebm_2<T>	&fcost;
};

} // namespace ebl {

#include "ebl_ebm.hpp"

#endif /* EBL_EBM_H_ */
