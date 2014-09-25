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

#ifndef EBL_ARCH_H_
#define EBL_ARCH_H_

#include "libidx.h"
#include "ebl_defines.h"
#include "ebl_module.h"
#include "ebl_parameters.h"

#ifndef __NOSTL__
#include <vector>
#endif

namespace ebl {

enum fprop_type { T_FPROP1, T_FPROP, T_FPROP_DUMP };
enum bprop_type { T_BPROP1, T_BPROP, T_BPROP_DUMP };

// layers //////////////////////////////////////////////////////////////////////

//! A stack of module_1_1 modules.
template <typename T> class layers : public module_1_1<T> {
 public:
  //! Constructor.
  layers(bool oc = true, const char *name = "layers");
  //! Destructor.
  virtual ~layers();

  //! Add a module to the stack of modules.
  virtual void add_module(module_1_1<T>* module);

  // first state tensor propagation //////////////////////////////////////////

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

  // propagation ///////////////////////////////////////////////////////////////

  //! Forward propagation from 'in' tensor to 'out' tensor.
  virtual void fprop(state<T> &in, state<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop(state<T> &in, state<T> &out);

  // dumping ///////////////////////////////////////////////////////////////////

  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This is useful for debugging.
  virtual void fprop_dump(state<T> &in, state<T> &out);

  // optimization //////////////////////////////////////////////////////////////

  //! Pre-determine the order of hidden buffers to use only 2 buffers
  //! in order to reduce memory footprint.
  //! This returns true if outputs is actually put in out, false if it's
  //! in in.
  // TODO: fix optimize fprop
  //virtual bool optimize_fprop(Mstate &in, Mstate &out);

  //////////////////////////////////////////////////////////////////////////////
  virtual void forget(forget_param_linear &fp);
  virtual void normalize();
  //! given the input dimensions, modifies it to be compliant with module's
  //! architecture, and returns the output dimensions corresponding to
  //! modified input dimensions.
  //! the implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual fidxdim fprop1_size(fidxdim &i_size);
  //! given the output dimensions, returns the input dimensions.
  //! the implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Modifies multi-input dimensions 'isize' to be compliant with module's
  //! architecture, and returns corresponding output dimensions.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim fprop_size(mfidxdim &isize);
  //! Given multiple output dimensions, returns the input dimensions.
  //! The implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim bprop_size(mfidxdim &o_size);
  //! Prints the forward transformation of dimensions in a string and return
  //! it. This method calls fprop_size to determine the output size given
  //! the input.
  virtual std::string pretty(idxdim &isize);
  //! Prints the forward transformation of multi-dimensions in a string and
  //! return it. This method calls fprop_size to determine the output size
  //! given the input.
  virtual std::string pretty(mfidxdim &isize);
  //! Returns a deep copy of current module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Swap the dual buffers used for memory optimization.
  virtual void swap_buffers();
  //! Return the number of layers contained in this object.
  virtual uint size();
  //! Zero-out internal hidden's states backward tensors.
  virtual void zero_dx();
  //! Zero-out internal hidden's states 2nd order backward tensors.
  virtual void zero_ddx();
  //! Find first module whose name matches 'name', return NULL if not found.
  module_1_1<T>* find(const char *name);
  //! Returns the last module contained in this module, or itself if composed
  //! of only 1 module.
  virtual module_1_1<T>* last_module();
  //! Returns a string containing describe() string of all modules.
  virtual std::string describe_indent(uint indent = 0);
  //! Returns a string with statistics about hidden states (min/max/mean).
  virtual std::string info();
  //! Returns true if this module takes a multi-state as input.
  virtual bool mstate_input();
  //! Returns true if this module produces a multi-state as output.
  virtual bool mstate_output();
  //! Replace standard output streams for this module.
  virtual void set_output_streams(std::ostream &out, std::ostream &err);

  // friends ///////////////////////////////////////////////////////////////////
  friend class layers_gui;

  // internal methods //////////////////////////////////////////////////////////
 protected:

  //! The generic forward method where func is the forward function applied,
  //! i.e. 'fprop' or 'fprop_dump'.
  virtual void forward(state<T>& in, state<T>& out, fprop_type type);
  //! The generic backward method where func is the forward function applied,
  //! i.e. 'fprop' or 'fprop_dump'.
  virtual void backward(state<T>& in, state<T>& out, bprop_type type);
  //! The generic bbackward method where func is the forward function applied,
  //! i.e. 'fprop' or 'fprop_dump'.
  virtual void bbackward(state<T>& in, state<T>& out, bprop_type type);

  // member variables //////////////////////////////////////////////////////////
 public:
  std::vector<module_1_1<T>*>	modules;
  std::vector<state<T>*>			hiddens;
 protected:
  bool own_contents;
  state<T>* hi; //! temporary buffer pointer
  state<T>* ho; //! temporary buffer pointer
  state<T>* htmp; //! temporary buffer pointer used for swapping
};

// narrow_module ///////////////////////////////////////////////////////////////

//! This module narrow's its input into its output. By default, it narrows
//! the number of tensors of multi-tensor inputs, but it can also narrow
//! each tensors themselves.
template<typename T> class EXPORT narrow_module : public module_1_1<T> {
public:
  //! Construct a narrow module, narrowing dimension 'd' of input to 'size',
  //! starting at 'offset'.
  //! \param narrow_tensors Narrow each tensor of state instead of
  //!   narrowing the state itself.
  narrow_module(int d, intg size, intg offset, bool narrow_tensors = false);
  //! Construct a narrow module, narrowing dimension 'd' of input to 'size',
  //! starting at multiple 'offsets'.
  //! \param narrow_tensors Narrow each tensor of state instead of
  //!   narrowing the state itself.
  narrow_module(int d, intg size, std::vector<intg> &offsets,
                bool narrow_tensors = false, const char *name = "narrow");
  //! Destructor.
  virtual ~narrow_module();

  // propagation ///////////////////////////////////////////////////////////////

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);

  // full state propagation //////////////////////////////////////////////////

  //! Forward propagation of all tensors from 'in' tensors to 'out' tensors.
  virtual void fprop(state<T> &in, state<T> &out);
  //! 1st order backward propagation of all tensors from out to in.
  virtual void bprop(state<T> &in, state<T> &out);
  //! 2nd order backward propagation of all tensors from out to in.
  virtual void bbprop(state<T> &in, state<T> &out);

  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns a deep copy of current module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Modifies multi-input dimensions 'isize' to be compliant with module's
  //! architecture, and returns corresponding output dimensions.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim fprop_size(mfidxdim &isize);
  //! Returns input dimensions corresponding to multiple output dimensions
  //! 'osize'. Implementation of this method helps automatic scaling of input
  //! data but is optional.
  virtual mfidxdim bprop_size(mfidxdim &osize);

protected:
  int dim; //!< Dimension to be narrowed.
  intg size; //!< Target size for narrowed dimension.
  std::vector<intg> offsets; //!< Offset in input for narrowing.
  bool narrow_tensors; //!< Narrow tensors instead of state.
};

// table_module ////////////////////////////////////////////////////////////////

//! This module connects inputs and outputs with a connection table.
template <typename T> class EXPORT table_module : public module_1_1<T> {
public:
  //! Outputs will be the inputs arranged according to the list of
  //! input indices 'inputs'.
  table_module(std::vector<intg> &inputs, intg total,
               const char *name = "table_module");
  //! Destructor.
  virtual ~table_module();

  // full state propagation //////////////////////////////////////////////////

  //! Forward propagation of all tensors from 'in' tensors to 'out' tensors.
  virtual void fprop(state<T> &in, state<T> &out);
  //! 1st order backward propagation of all tensors from out to in.
  virtual void bprop(state<T> &in, state<T> &out);
  //! 2nd order backward propagation of all tensors from out to in.
  virtual void bbprop(state<T> &in, state<T> &out);

  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns a deep copy of current module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Modifies multi-input dimensions 'isize' to be compliant with module's
  //! architecture, and returns corresponding output dimensions.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim fprop_size(mfidxdim &isize);
  //! Returns input dimensions corresponding to multiple output dimensions
  //! 'osize'. Implementation of this method helps automatic scaling of input
  //! data but is optional.
  virtual mfidxdim bprop_size(mfidxdim &osize);
protected:
  std::vector<intg> table; //!< List of inputs indices.
  intg total; //!< Total number of inputs.
};

// network sizes methods /////////////////////////////////////////////////////

//! Returns the minimum input size with order 'order'
//! that can be fed to network m.
template <typename T>
EXPORT idxdim network_mindims(module_1_1<T> &m, uint order);
//! Returns the strides of the network in the input space.
template <typename T>
EXPORT idxdim network_strides(module_1_1<T> &m, uint order);

} // namespace ebl {

#include "ebl_arch.hpp"

#endif /* EBL_ARCH_H_ */
