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

#ifndef EBL_MARCH_H_
#define EBL_MARCH_H_

#include "ebl_arch.h"

namespace ebl {

// ms_module ///////////////////////////////////////////////////////////////////

//! A container for one or multiple modules with one input and one output
//! that processes a pipeline between a single-to-multi-state module
//! and a multi-to-single-state module.
//! Multiple pipes can be defined for multiple inputs, or a single pipe
//! can be reused for all inputs.
template <typename T> class ms_module : public module_1_1<T> {
 public:
  // constructors ////////////////////////////////////////////////////////////

  //! Construct an empty multi-state module.
  ms_module(bool replicate_inputs = false, const char *name = "ms_module");
  //! Construct a multi-state module where 'pipe' is replicated 'n' times.
  ms_module(module_1_1<T> *pipe, uint n = 1,
            bool replicate_inputs = false, const char *name = "ms_module");
  //! Constructor a multi-state module where each state goes through
  //! one of 'pipes' pipe.
  ms_module(std::vector<module_1_1<T>*> &pipes,
            bool replicate_inputs = false, const char *name = "ms_module");
  //! Destructor.
  virtual ~ms_module();

  // full state propagation //////////////////////////////////////////////////

  //! Forward propagation of all tensors from 'in' tensors to 'out' tensors.
  virtual void fprop(state<T> &in, state<T> &out);
  //! 1st order backward propagation of all tensors from out to in.
  virtual void bprop(state<T> &in, state<T> &out);
  //! 2nd order backward propagation of all tensors from out to in.
  virtual void bbprop(state<T> &in, state<T> &out);

  //! The generic forward method where func is the forward function applied,
  //! i.e. 'fprop' or 'fprop_dump'.
  void forward(state<T>& in, state<T>& out, fprop_type type);

  // dumping ///////////////////////////////////////////////////////////////////

  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This is useful for debugging.
  virtual void fprop_dump(state<T> &in, state<T> &out);

  ////////////////////////////////////////////////////////////////////////////
  virtual void forget(forget_param_linear& fp);

  // sizes propagations //////////////////////////////////////////////////////
  //! given the input dimensions, modifies it to be compliant with module's
  //! architecture, and returns the output dimensions corresponding to
  //! modified input dimensions.
  //! the implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual fidxdim fprop1_size(fidxdim &i_size);
  //! Modifies multi-input dimensions 'isize' to be compliant with module's
  //! architecture, and returns corresponding output dimensions.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim fprop_size(mfidxdim &isize);
  //! given the output dimensions, returns the input dimensions.
  //! the implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Given the multi-output dimensions, returns the multi-input dimensions.
  //! the implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim bprop_size(mfidxdim &o_size);

  // printing ////////////////////////////////////////////////////////////////
  //! Prints the forward transformation of dimensions in a string and return
  //! it. This method calls fprop_size to determine the output size given
  //! the input.
  virtual std::string pretty(idxdim &isize);
  //! Prints the forward transformation of multi-dimensions in a string and
  //! return it. This method calls fprop_size to determine the output size
  //! given the input.
  virtual std::string pretty(mfidxdim &isize);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // accessors ///////////////////////////////////////////////////////////////
  //! Only propagate in 1 pipe based on the size of the first input state.
  virtual void set_switch(midxdim &sizes);
  //! Only propagate in pipe with index 'id'.
  virtual void set_switch(intg id);
  //! Returns the number of pipes.
  virtual uint npipes();
  //! Returns pointer to pipe 'i'.
  virtual module_1_1<T>* get_pipe(uint i);
  //! Returns the last module contained in this module, or itself if composed
  //! of only 1 module.
  virtual module_1_1<T>* last_module();

  // internal methods ////////////////////////////////////////////////////////
 protected:
  //! Initializations.
  virtual void init();
  //! Prepare pipes and buffer for fprop.
  virtual void init_fprop(state<T> &in, state<T> &out);
  //! Switch used_pipes on input size when switches are defined.
  virtual void switch_pipes(state<T> &in);

  // variable members ////////////////////////////////////////////////////////
 protected:
  std::vector<module_1_1<T>*> pipes;    //!< All pipes/
  std::vector<module_1_1<T>*> used_pipes; //!< Not always all pipes.
  std::vector<uint>  pipes_noutputs; //!< Number of outputs for each pipe.
  svector<state<T> > ins;      //!< Inputs of each pipe.
  svector<state<T> > mbuffers; //!< Multi-state buffers.
  bool    replicate_inputs;    //!< Replicate all inputs for each pipe.
  midxdim switches;            //!< Only propagate 1 pipe based on input sizes.
  bool    bindex;              //!< If true, use switch id to switch.
  intg    switch_id;           //!< Index of pipe to switch to.

  // friends /////////////////////////////////////////////////////////////////
  template <typename T1, class Tc>
  friend EXPORT Tc* arch_find(ms_module<T1> *m, Tc *c);
  template <typename T1, class Tc>
  friend EXPORT std::vector<Tc*> arch_find_all(ms_module<T1> *m, Tc *c,
                                               std::vector<Tc*> *);
  template <typename T1, class Tc>
  friend EXPORT ms_module<T1>*
  arch_narrow(ms_module<T1> *m, Tc *c, bool i, bool p, bool *f);
  friend class ms_module_gui;
};

// msc_module //////////////////////////////////////////////////////////////////

//! A "convolutional" version of ms_module, where pipes are applied to
//! a set of inputs with a certain stride.
template<typename T> class msc_module : public ms_module<T> {
 public:
  //! Constructor a multi-state module where each state goes through
  //! one of 'pipes' pipe,
  //! and each pipe sees groups of 'nsize' inputs with stride 'stride'.
  //! \param nsize2 If non-zero, this defines the size of higher level
  //!   groups of inputs, i.e. regular grouping will be applied to each of
  //!   these higher groups individually.
  msc_module(std::vector<module_1_1<T>*> &pipes, uint nsize = 1,
             uint stride = 1, uint nsize2 = 0,
             const char *name = "msc_module");
  //! Destructor.
  virtual ~msc_module();
  ////////////////////////////////////////////////////////////////////////////
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
  //! Given the multi-output dimensions, returns the multi-input dimensions.
  //! the implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim bprop_size(mfidxdim &o_size);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // internal methods ////////////////////////////////////////////////////////
 protected:
  //! Prepare pipes and buffer for fprop.
  virtual void init_fprop(state<T> &in, state<T> &out);

  // variable members //////////////////////////////////////////////////////////
 protected:
  using ms_module<T>::pipes;
  using ms_module<T>::used_pipes;
  using ms_module<T>::ins;
  using ms_module<T>::pipes_noutputs;
  uint nsize; //! Groups size.
  uint stride; //! Stride for regular groups.
  uint nsize2; //! High level groups size.
};

// arch find methods /////////////////////////////////////////////////////////

//! Returns first module contained in 'm',
//! whose dynamic_cast<Tcast*> works, NULL otherwise.
template <typename T, class Tcast>
EXPORT Tcast* arch_find(module_1_1<T> *m, Tcast *c);
//! Returns first module contained in 'm',
//! whose dynamic_cast<Tcast*> works, NULL otherwise.
template <typename T, class Tcast>
EXPORT Tcast* arch_find(layers<T> *m, Tcast *c);
//! Returns first module contained in 'm',
//! whose dynamic_cast<Tcast*> works, NULL otherwise.
template <typename T, class Tcast>
EXPORT Tcast* arch_find(ms_module<T> *m, Tcast *c);

// arch find_all methods /////////////////////////////////////////////////////

//! Returns all modules contained in 'm', whose dynamic_cast<Tcast*> match.
//! If v is not null, fill v and return it, otherwise return a new vector.
template <typename T, class Tcast>
EXPORT std::vector<Tcast*> arch_find_all(module_1_1<T> *m, Tcast *c,
                                         std::vector<Tcast*> *v = NULL);
//! Returns all modules contained in 'm', whose dynamic_cast<Tcast*> match.
//! If v is not null, fill v and return it, otherwise return a new vector.
template <typename T, class Tcast>
EXPORT std::vector<Tcast*> arch_find_all(layers<T> *m, Tcast *c,
                                         std::vector<Tcast*> *v = NULL);
//! Returns all modules contained in 'm', whose dynamic_cast<Tcast*> match.
//! If v is not null, fill v and return it, otherwise return a new vector.
template <typename T, class Tcast>
EXPORT std::vector<Tcast*> arch_find_all(ms_module<T> *m, Tcast *c,
                                         std::vector<Tcast*> *v = NULL);

// arch narrow methods ///////////////////////////////////////////////////////

//! Returns a copy of network 'm' which is narrowed up to the first
//! occurence of module of type 'Tcast' with same address 'c' (included).
//! If no occurence is found, a copy of the entire network is returned.
//! \param included If true, also include target module.
//! \param post If true, return the postfix network rather than prefix.
template <typename T, class Tcast>
EXPORT module_1_1<T>* arch_narrow(module_1_1<T> *m, Tcast *c,
                                  bool included = true, bool post = false,
                                  bool *found = NULL);
//! Returns a copy of network 'm' which is narrowed up to the first
//! occurence of module of type 'Tcast' with same address 'c' (included).
//! If no occurence is found, a copy of the entire network is returned.
//! \param included If true, also include target module.
//! \param post If true, return the postfix network rather than prefix.
template <typename T, class Tcast>
EXPORT layers<T>* arch_narrow(layers<T> *m, Tcast *c, bool included = true,
                              bool post = false, bool *found = NULL);
//! Returns a copy of network 'm' which is narrowed up to the first
//! occurence of module of type 'Tcast' with same address 'c' (included).
//! If no occurence is found, a copy of the entire network is returned.
//! \param included If true, also include target module.
//! \param post If true, return the postfix network rather than prefix.
template <typename T, class Tcast>
EXPORT ms_module<T>* arch_narrow(ms_module<T> *m, Tcast *c, bool included =true,
                                 bool post = false, bool *found = NULL);

} // namespace ebl {

#include "ebl_march.hpp"

#endif /* EBL_MARCH_H_ */
