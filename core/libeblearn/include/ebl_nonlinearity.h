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

#ifndef EBL_NONLINEARITY_H_
#define EBL_NONLINEARITY_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_basic.h"

namespace ebl {

// stdsigmoid //////////////////////////////////////////////////////////////////

//! The standard sigmoid.
template <typename T> class stdsigmoid_module: public module_1_1<T> {
 public:
	//! Constructor.
	//! \param linear_coeff If non-zero, a linear term is added to the output
	//!   with coefficient 'linear_coeff': y = tanh(x) + linear_coeff * x
	//!   A linear term can be good to avoid flat regions.
	stdsigmoid_module(double linear_coeff = 0, const char *name = "stdsigmoid");
	//! Destructor.
	virtual ~stdsigmoid_module();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in, idx<T> &out);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in, state<T> &out);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in, state<T> &out);

	//! Returns a deep copy of this module.
	//! \param p If NULL, the copy points to the same weights as this module.
	virtual module_1_1<T>* copy(parameter<T> *p = NULL);
	//! Returns a string describing this module and its parameters.
	virtual std::string describe();

 protected:
	idx<T>				tmp;						//!< Temporary buffer.
	double				alpha;					//!< Coefficient of the linear term.
};

// tanh ////////////////////////////////////////////////////////////////////////

//! The hyperbolic tangent.
template <typename T> class tanh_module: public module_1_1<T> {
 public:
	//! default constructor
	//! \param linear_coeff If non-zero, a linear term is added to the output
	//!   with coefficient 'linear_coeff': y = tanh(x) + linear_coeff * x
	//!   A linear term can be good to avoid flat regions.
	tanh_module(double linear_coeff = 0, const char *name = "tanh");
	virtual ~tanh_module();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in, idx<T> &out);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in, state<T> &out);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in, state<T> &out);

	//! Calls fprop and then dumps internal buffers, inputs and outputs
	//! into files. This can be useful for debugging.
	virtual void fprop1_dump(idx<T> &in, idx<T> &out);
	//! Returns a deep copy of this module.
	//! \param p If NULL, the copy points to the same weights as this module.
	virtual module_1_1<T>* copy(parameter<T> *p = NULL);
	//! Returns a string describing this module and its parameters.
	virtual std::string describe();

 protected:
	idx<T>				tmp;						//!< Temporary buffer.
	double				alpha;					//!< Coefficient of the linear term.
};

// softmax /////////////////////////////////////////////////////////////////////

//! softmax module
template <typename T> class softmax_module: public module_1_1<T> {
 public:
	//! Constructor.
	//! \param beta If large, turns softmax into a max, if 0 returns 1/N.
	softmax_module(double beta = 1);
	//! Destructor.
	~softmax_module();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in, idx<T> &out);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in, state<T> &out);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in, state<T> &out);

 public:
	double beta;
};

// abs /////////////////////////////////////////////////////////////////////////

//! This module takes the absolute value of its input.
//! This module is spatially replicable
//! (the input can have an order greater than 1 and the operation will apply
//! to all elements).
template <typename T> class abs_module: public module_1_1<T> {
 public:
	//! Constructor. threshold makes the derivative of abs flat around zero
	//! with radius threshold.
	abs_module(double thresh = 0.0);
	//! Destructor.
	virtual ~abs_module();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in, idx<T> &out);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in, state<T> &out);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in, state<T> &out);

	//! Returns a deep copy of this module.
	//! \param p If NULL, the copy points to the same weights as this module.
	virtual module_1_1<T>* copy(parameter<T> *p = NULL);

 private:
	double threshold;
};

// linear shrink ///////////////////////////////////////////////////////////////

//! A piece-wise linear shrinkage module that parametrizes
//! the location of the shrinkage operator. This function is
//! useful for learning since there is always gradients flowing
//! through it.
template <typename T> class linear_shrink_module: public module_1_1<T> {
 public:
	//! Constructor.
	//! \param nf The number of features.
	linear_shrink_module(parameter<T> *p, intg nf, T bias = 0);
	//! Destructor.
	virtual ~linear_shrink_module();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in, idx<T> &out);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in, state<T> &out);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in, state<T> &out);

	//! Returns a deep copy of this module.
	//! \param p If NULL, the copy points to the same weights as this module.
	virtual module_1_1<T>* copy(parameter<T> *p = NULL);
	//! Returns a string describing this module and its parameters.
	virtual std::string describe();

 protected:
	state<T> bias;
	T default_bias;
};

// smooth_shrink_module ////////////////////////////////////////////////////////

//! A smoothed shrinkage module that parametrizes the steepnes
//! and location of the shrinkage operator. This function is
//! useful for learning since there is always gradients flowing
//! through it.
template <typename T> class smooth_shrink_module: public module_1_1<T> {
 public:
	//! Constructor.
	//! \param nf The number of features.
	smooth_shrink_module(parameter<T> *p, intg nf, T beta = 10, T bias = .3);
	//! Destructor.
	virtual ~smooth_shrink_module();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in, idx<T> &out);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in, state<T> &out);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in, state<T> &out);

	//! Returns a deep copy of this module.
	//! \param p If NULL, the copy points to the same weights as this module.
	virtual module_1_1<T>* copy(parameter<T> *p = NULL);

 public:
	state<T> beta, bias;
 private:
	state<T> ebb, ebx, tin;
	abs_module<T> absmod;
	T default_beta, default_bias;
};

// tanh_shrink_module //////////////////////////////////////////////////////////

//! A smoothed shrinkage module using (x - tanh(x))
//! that parametrizes the steepnes of the shrinkage operator.
//! This function is useful for learning since there is always gradients
//! flowing through it.
template <typename T> class tanh_shrink_module: public module_1_1<T> {
 public:
	//! Constructor.
	//! \param nf The number of features.
	//! \param diags If true, alpha and beta coefficients are learned
	//!   such that the output is: a * x - tanh(b * x)
	tanh_shrink_module(parameter<T> *p, intg nf, bool diags = false);
	//! Destructor.
	virtual ~tanh_shrink_module();

	// first state tensor propagation //////////////////////////////////////////

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in, idx<T> &out);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in, state<T> &out);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in, state<T> &out);

	//! Returns a deep copy of this module.
	//! \param p If NULL, the copy points to the same weights as this module.
	virtual module_1_1<T>* copy(parameter<T> *p = NULL);
	//! Returns a string describing this module and its parameters.
	virtual std::string describe();
 protected:
	intg			 nfeatures;
	state<T>		 abuf, tbuf, bbuf;
	diag_module<T>	*alpha, *beta;
	tanh_module<T>	 mtanh;
	diff_module<T>	 difmod;	//!< difference module
	bool                         diags; //!< Use coefficients or not.
};

} // namespace ebl {

#include "ebl_nonlinearity.hpp"

#endif /* EBL_NONLINEARITY_H_ */
