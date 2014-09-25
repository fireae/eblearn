/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef EBL_LAYERS_H_
#define EBL_LAYERS_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_basic.h"
#include "ebl_pooling.h"
#include "ebl_arch.h"
#include "ebl_nonlinearity.h"
#include "ebl_normalization.h"

namespace ebl {

// full_layer //////////////////////////////////////////////////////////////////

//! a simple fully-connected neural net layer: linear + tanh non-linearity.
template <typename T> class full_layer : public module_1_1<T> {
 public:
  //! Constructor. Arguments are a pointer to a parameter
  //! in which the trainable weights will be appended,
  //! the number of inputs, and the number of outputs.
  //! \param indim0 The number of inputs
  //! \param noutputs The number of outputs.
  //! \param tanh If true, use tanh squasher, stdsigmoid otherwise.
  full_layer(parameter<T> *p, intg indim0, intg noutputs,
             bool tanh = true, const char *name = "full_layer");
  //! Destructor.
  virtual ~full_layer();

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

  //! initialize the weights to random values
  void forget(forget_param_linear &fp);
  //! Return dimensions that are compatible with this module.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim fprop1_size(fidxdim &i_size);
  //! Return dimensions compatible with this module given output dimensions.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // members ////////////////////////////////////////////////////////
 private:
  bool		    btanh;              //!< use tanh or stdsigmoid
 public:
  /* linear_module_replicable<T>	 linear;//!< linear module for weight */
  linear_module<T>  linear;             //!< linear module for weight
  addc_module<T>    adder;              //!< bias vector
  module_1_1<T>	   *sigmoid;            //!< the non-linear function
  state<T>	   *sum;                //!< weighted sum
};

// convolution_layer ///////////////////////////////////////////////////////////

//! a convolution neural net layer: convolution + tanh non-linearity.
template <typename T> class convolution_layer : public module_1_1<T> {
 public:
  //! constructor. Arguments are a pointer to a parameter
  //! in which the trainable weights will be appended,
  //! the number of inputs, and the number of outputs.
  //! \param p is used to store all parametric variables in a single place.
  //! \param ker The convolution kernel sizes.
  //! \param stride The convolution strides.
  //! \param table is the convolution connection table.
  //! \param tanh If true, use tanh squasher, stdsigmoid otherwise.
  convolution_layer(parameter<T> *p, idxdim &ker, idxdim &stride,
                    idx<intg> &tbl, bool tanh = true,
                    const char *name = "convolution_layer");
  virtual ~convolution_layer();

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

  //! initialize the weights to random values
  void forget(forget_param_linear &fp);
  //! Return dimensions that are compatible with this module.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim fprop1_size(fidxdim &i_size);
  //! Return dimensions compatible with this module given output dimensions.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);

  // members ////////////////////////////////////////////////////////
 private:
  bool			 btanh;         //!< tanh or stdsigmoid
 public:
  //    convolution_module_replicable<T>	 convol;//!< convolution module
  convolution_module<T>	 convol;        //!< convolution module
  addc_module<T>	 adder;         //!< bias vector
  module_1_1<T>		*sigmoid;       //!< non-linear funct
  state<T>		*sum;           //!< convolution result
};

// convabsnorm_layer ///////////////////////////////////////////////////////////

//! a convolution layer with absolute rectification and constrast
//! normalization
template <typename T> class convabsnorm_layer : public module_1_1<T> {
 public:
  //! constructor. Arguments are a pointer to a parameter
  //! in which the trainable weights will be appended,
  //! the number of inputs, and the number of outputs.
  //! \param p is used to store all parametric variables in a single place.
  //! \param kerneli is the height of the convolution kernel
  //! \param kernelj is the width of the convolution kernel
  //! \param stridei is the stride at which convolutions are done on
  //!        the height axis.
  //! \param stridej is the stride at which convolutions are done on
  //!        the width axis.
  //! \param table is the convolution connection table.
  //! \param tanh If true, use tanh squasher, stdsigmoid otherwise.

  convabsnorm_layer(parameter<T> *p, intg kerneli, intg kernelj,
                    intg stridei, intg stridej, idx<intg> &tbl,
                    bool mirror = false, bool tanh = true,
                    const char *name = "convabsnorm_layer");
  //! Destructor.
  virtual ~convabsnorm_layer();

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

  //! initialize the weights to random values
  void forget(forget_param_linear &fp);
  //! Return dimensions that are compatible with this module.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim fprop1_size(fidxdim &i_size);
  //! Return dimensions compatible with this module given output dimensions.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);

  // members ////////////////////////////////////////////////////////
 private:
  bool			   btanh;	//!< use tanh or stdsigmoid
 public:
  convolution_layer<T>	   lconv;	//!< convolution layer
  abs_module<T>            abs;         //!< absolute rectification
  contrast_norm_module<T>  norm;	//!< constrast normalization
  state<T>		  *tmp;         //!< temporary results
  state<T>		  *tmp2;	//!< temporary results
};

// subsampling_layer ///////////////////////////////////////////////////////////

//! a subsampling neural net layer: subsampling + tanh non-linearity.
template <typename T> class subsampling_layer : public module_1_1<T> {
 public:
  //! Constructor. Arguments are a pointer to a parameter
  //! in which the trainable weights will be appended,
  //! the number of inputs, and the number of outputs.
  //! \param thickness The number of features.
  //! \param kernel Size of subsampling kernel (without thickness).
  //! \param strides Strides of subsampling kernel (without thickness).
  //! \param tanh If true, use tanh squasher, stdsigmoid otherwise.
  subsampling_layer(parameter<T> *p, uint thickness,
                    idxdim &kernel, idxdim &stride,
                    bool tanh = true, const char *name = "subsampling_layer");
  //! Destructor.
  virtual ~subsampling_layer();


  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

  //! initialize the weights to random values
  void forget(forget_param_linear &fp);
  //! Return dimensions that are compatible with this module.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim fprop1_size(fidxdim &i_size);
  //! Return dimensions compatible with this module given output dimensions.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // members ////////////////////////////////////////////////////////
 private:
  bool				    btanh; //!< tanh or stdsigmoid
 public:
  subsampling_module_replicable<T>  subsampler; //!< subsampling
  addc_module<T>		    adder; //!< bias vector
  module_1_1<T>                    *sigmoid; //!< non-linear funct
  state<T>			   *sum; //!< subsampling result
};

} // namespace ebl {

#include "ebl_layers.hpp"

#endif /* EBL_LAYERS_H_ */
