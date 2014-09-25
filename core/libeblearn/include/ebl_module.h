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

#ifndef EBL_MODULE_H_
#define EBL_MODULE_H_

#include "libidx.h"
#include "ebl_defines.h"
#include "ebl_state.h"
#include "ebl_parameters.h"

#ifndef __NOSTL__
#include <vector>
#endif

namespace ebl {

// module ////////////////////////////////////////////////////////////////////

//! A module class containing a name.
class EXPORT module {
public:
  //! Constructs a module.
  module(const char *name = "module");
  //! Destructor.
  virtual ~module();
  //! Return the name of this module.
  virtual const char* name();
  //! Set the name of this module to 'name'.
  virtual void set_name(const char *name);
  //! If _name is empty, set it to 'name'.
  virtual void default_name(const char *name);
  //! Replace standard output streams for this module.
  virtual void set_output_streams(std::ostream &out, std::ostream &err);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns a string describing this module and its parameters.
  virtual std::string describe_indent(uint indent);
  //! Allow this module to process.
  virtual void enable();
  //! Forbid this module to process.
  virtual void disable();

  // members
protected:
  std::string   _name;                //!< Name of this module.
  std::ostream *mout, *merr;          //!< Output streams.
  bool          silent;
  bool          _enabled;
  bool          gpu_support;          //!< Whether module is gpu-enabled
};

// module_1_1 ////////////////////////////////////////////////////////////////

//! An abstract class for a module with one input and one output.
template <typename T> class EXPORT module_1_1 : public module {
public:
  //! Constructor.
  //! \param bresize By default, resize output.
  module_1_1(const char *name = "module_1_1", bool bresize = true);
  //! Destructor.
  virtual ~module_1_1();

  // first state tensor propagation //////////////////////////////////////////

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

  // full state propagation //////////////////////////////////////////////////

  //! Forward propagation of all tensors from 'in' tensors to 'out' tensors.
  virtual void fprop(state<T> &in, state<T> &out);
  //! 1st order backward propagation of all tensors from out to in.
  virtual void bprop(state<T> &in, state<T> &out);
  //! 2nd order backward propagation of all tensors from out to in.
  virtual void bbprop(state<T> &in, state<T> &out);

  // dumping /////////////////////////////////////////////////////////////////

  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This is useful for debugging.
  virtual void fprop1_dump(idx<T> &in, idx<T> &out);
  //! Calls fprop_dump for internal buffers.
  virtual void fprop_dump(state<T> &in, state<T> &out);

  // weights manipulation ////////////////////////////////////////////////////

  //! (Re)initializes weights of this module.
  virtual void forget(forget_param_linear &fp);
  //! Normalization.
  virtual void normalize();
  //! Zero-out internal hidden's states backward tensors.
  virtual void zero_dx();
  //! Zero-out internal hidden's states 2nd order backward tensors.
  virtual void zero_ddx();

  //! Returns the order at which the module operates.
  virtual int replicable_order();

  // resizing ////////////////////////////////////////////////////////////////

  //! This tests if this module is enabled, if it is it returns false,
  //! otherwise simply copies data from in to out and returns true;
  virtual bool ignored1(idx<T> &in, idx<T> &out);
  //! Makes sure out has the intended number of tensors and the intended order
  //! for each of them.
  virtual void resize_output_orders(state<T> &in, state<T> &out);
  //! Resizes 'out' to the same dimensions as 'in'. If dimensions already
  //! match, nothing is changed. If orders differ, then out is assigned a new
  //! tensor with the correct order and dimensions.
  //! \param d If not null, use these dimensions as target, otherwise use in.x
  //! This returns true if output was resized/reallocated, false otherwise.
  virtual bool resize_output(idx<T> &in, idx<T> &out, idxdim *d = NULL);
  //! Modifies input dimensions 'isize' to be compliant with module's
  //! architecture, and returns corresponding output dimensions.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual fidxdim fprop1_size(fidxdim &isize);
  //! Returns input dimensions corresponding to output dimensions 'osize'.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual fidxdim bprop1_size(const fidxdim &osize);
  //! Modifies multi-input dimensions 'isize' to be compliant with module's
  //! architecture, and returns corresponding output dimensions.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim fprop_size(mfidxdim &isize);
  /* //! Returns input dimensions corresponding to multiple output dimensions */
  /* //! 'osize'. Implementation of this method helps automatic scaling of input */
  /* //! data but is optional. */
  /* virtual mfidxdim bprop_size(const mfidxdim &osize); */
  //! Returns input dimensions corresponding to multiple output dimensions
  //! 'osize'. Implementation of this method helps automatic scaling of input
  //! data but is optional.
  virtual mfidxdim bprop_size(mfidxdim &osize);
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
  //! Pre-determine the order of hidden buffers to use only 2 buffers
  //! in order to reduce memory footprint.
  //! This returns true if outputs is actually put in out, false if it's
  //! in in.
  virtual bool optimize_fprop(state<T> &in, state<T> &out);
  //! Load internal weights of module with passed weights w.
  //! TODO: there should be not idx specialization at this level.
  virtual void load_x(idx<T> &weights);
  //! Returns the last module contained in this module, or itself if composed
  //! of only 1 module.
  virtual module_1_1<T>* last_module();
  //! Returns true if this module takes a multi-state as input.
  virtual bool mstate_input();
  //! Returns true if this module produces a multi-state as output.
  virtual bool mstate_output();
  //! Returns the number of input states after last fprop.
  virtual uint get_ninputs();
  //! Returns the number of output states after last fprop.
  virtual uint get_noutputs();
  //! Returns dimensions of last output(s).
  virtual idxdim get_outdims();
  //! Update internal "outdims" dimensions of last output.
  virtual void update_outdims(idx<T> &out);

  // variable members ////////////////////////////////////////////////////////
public:
  // these variables describe internal buffers declared to be displayed
  // by external display objects.
  std::vector<idx<T> >     internals;		//!< Internal buffers to display
  std::vector<std::string> internals_str;	//!< Internal buffers desc.
protected:
  bool					bresize;				//!< Tells module to resize output.
  bool					memoptimized;		//!< Using mem optim or not.
  bool					bmstate_input;	//!< Input is multi-state.
  bool					bmstate_output;	//!< Output is multi-state.
  uint					ninputs, noutputs;			//!< Current # of i/o states.
  idxdim				outdims;				//!< Last out dimensions.
};

// module_2_1 //////////////////////////////////////////////////////////////////

//! An abstract class for a module with two inputs and one output.
template <typename T> class EXPORT module_2_1 : public module {
public:
  //! Constructor.
  module_2_1(const char *name = "module_2_1");
  //! Destructor.
  virtual ~module_2_1();

  // first state tensor propagation ////////////////////////////////////////////

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in1, idx<T> &in2, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in1, state<T> &in2, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in1, state<T> &in2, state<T> &out);

  // full state propagation ////////////////////////////////////////////////////

  //! Forward propagation of all tensors from 'in' tensors to 'out' tensors.
  virtual void fprop(state<T> &in1, state<T> &in2, state<T> &out);
  //! 1st order backward propagation of all tensors from out to in.
  virtual void bprop(state<T> &in1, state<T> &in2, state<T> &out);
  //! 2nd order backward propagation of all tensors from out to in.
  virtual void bbprop(state<T> &in1, state<T> &in2, state<T> &out);

  // weights manipulation //////////////////////////////////////////////////////

  virtual void forget(forget_param_linear &fp);
  virtual void normalize();

  // resizing //////////////////////////////////////////////////////////////////

  //! Resizes 'out' to the same dimensions as 'in'. If dimensions already
  //! match, nothing is changed. If orders differ, then out is assigned a new
  //! 'Tout' buffer if the correct order and dimensions.
  //! \param d If not null, use these dimensions as target, otherwise use in.x
  //! This returns true if output was resized/reallocated, false otherwise.
  virtual bool resize_output(idx<T> &in1, idx<T> &in2, idx<T> &out,
                             idxdim *d = NULL);

protected:
  bool	 bresize;	//!< Tells module to resize output or not.
};

// helper functions ////////////////////////////////////////////////////////////

//! check that m and in are compatible for replication
template <typename T>
void check_replicable_orders(module_1_1<T> &m, state<T>& in);

// generic replicable module classes ///////////////////////////////////////////

//! This modules loops over replicable dimensions and calls <module>'s
//! functions when reaching <module>'s replicable_order.
//! Note: Multiple inheritance could have been cleaner but would have required
//! frequent dynamic_casts from the user.
template <class Tmodule, typename T> class EXPORT module_1_1_replicable {
public:
  Tmodule &module;
  module_1_1_replicable(Tmodule &m);
  virtual ~module_1_1_replicable();
  virtual void fprop(state<T> &in, state<T> &out);
  virtual void bprop(state<T> &in, state<T> &out);
  virtual void bbprop(state<T> &in, state<T> &out);
};

//! A macro to declare a module_1_1<state_idx,state_idx> as replicable
//! over extra dimensions beyond the module's replicable_order.
//! \param replicable_module is the new class name
//! \param base_module is the module_1_1<state_idx,state_idx> to be replicated
//! \param type_arguments are the arguments to the base_module's constructor
//!   along with their type (use parenthesis)
//! \param arguments are the naked arguments (use parenthesis)
//! \example   DECLARE_REPLICABLE_MODULE_1_1(linear_module_replicable,
//!                                          linear_module,
//!                                          (parameter &p, intg in, intg ou),
//!  					     (p, in, out));
//! Note: Multiple inheritance could have been cleaner but would have required
//! frequent dynamic_casts from the user.
#define DECLARE_REPLICABLE_MODULE_1_1(replicable_module, base_module,	\
                                      T, types_arguments, arguments)	\
  template <typename T>                                                 \
  class EXPORT replicable_module : public base_module<T> {              \
 public:								\
    module_1_1_replicable<base_module<T>,T> *rep;                       \
    replicable_module types_arguments : base_module<T> arguments {      \
      rep = new module_1_1_replicable<base_module<T>,T>(*this);         \
      this->bresize = false;						\
      if (this->replicable_order() <= 0)				\
	eblerror("this module is not replicable"); }			\
    virtual ~replicable_module() { delete rep; }			\
    virtual void fprop(state<T> &in, state<T> &out) {                   \
      rep->fprop(in, out); }						\
    virtual void bprop(state<T> &in, state<T> &out) {                   \
      rep->bprop(in, out); }						\
    virtual void bbprop(state<T> &in, state<T> &out) {			\
      rep->bbprop(in, out); }						\
  }

} // namespace ebl {

#include "ebl_module.hpp"

#endif /* EBL_MODULE_H_ */
