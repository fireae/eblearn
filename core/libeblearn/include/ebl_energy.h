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

#ifndef EBL_ENERGY_H_
#define EBL_ENERGY_H_

#include "libidx.h"
#include "ebl_ebm.h"
#include "ebl_arch.h"
#include "ebl_nonlinearity.h"

namespace ebl {

// l2_energy ///////////////////////////////////////////////////////////////////

//! A generalized euclidean distance energy between inputs,
//! i.e. the energy equals: .5 * || X - Y ||^2
template <typename T> class l2_energy : public ebm_2<T> {
 public:
	l2_energy(const char *name = "l2_energy");
	virtual ~l2_energy();

	//! Forward propagation from 'in1'/'in2' tensors to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in1, idx<T> &in2, idx<T> &energy);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in1, state<T> &in2, state<T> &energy);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in1, state<T> &in2, state<T> &energy);

	virtual void infer2_copy(state<T> &i1, state<T> &i2, state<T> &energy);
	//! Returns a string describing this module and its parameters.
	virtual std::string describe();
 protected:
	idx<T> tmp; //!< Buffer for temporary operations.
};

// l1_penalty ////////////////////////////////////////////////////////////////
//! An L1 penalty energy given a single input.
template<typename T> class l1_penalty : public ebm_1<T> {
 public:
	//! \param threshold A threshold defining a flat region around zero, for
	//!   derivatives only.
	//! \param coeff A coefficient applied to the energy of the fprop.
	l1_penalty(T threshold = 0, T coeff = 1);
	virtual ~l1_penalty();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in, idx<T> &energy);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in, state<T> &energy);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in, state<T> &energy);

	//! Returns a string describing this module and its parameters.
	virtual std::string describe();

	// member variables
 protected:
	T threshold;
	T coeff;
};

// cross_entropy_energy ////////////////////////////////////////////////////////

//! A cross-entropy energy module for multinomiol logistic regression.
//! Energy equals: - desired
//! where desired is a 1-of-n encoding of the groundtruth class.
template<typename T> class cross_entropy_energy : public ebm_2<T> {
 public:
	cross_entropy_energy(const char *name = "cross_entropy_energy");
	virtual ~cross_entropy_energy();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in1, idx<T> &in2, idx<T> &energy);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in1, state<T> &in2, state<T> &energy);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in1, state<T> &in2, state<T> &energy);

	virtual void infer2_copy(state<T> &i1, state<T> &i2, state<T> &energy);
	//! Returns a string describing this module and its parameters.
	virtual std::string describe();
 protected:
	state<T> soft_out; //!< Buffer for temporary operations.
	idx<T> tmp;
	softmax_module<T> softmax;
};

// scalerclass_energy //////////////////////////////////////////////////////////

template<typename T> class scalerclass_energy : public l2_energy<T> {
 public:
	//! constructor.
	//! \param predict_conf Add an extra feature for confidence prediction.
	//! \param predict_bconf If true, predicted confidence is binary (0,1).
	scalerclass_energy(bool apply_tanh = false, uint jsize = 1,
										 uint jitter_selection = 0, float dist_coeff = 1.0,
										 float scale_coeff = 1.0, bool predict_conf = false,
										 bool predict_bconf = false,
										 idx<T> *biases = NULL, idx<T> *coeffs = NULL,
										 const char *name = "scalerclass_energy");
	//! destructor.
	virtual ~scalerclass_energy();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in1, idx<T> &in2, idx<T> &energy);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in1, state<T> &in2, state<T> &energy);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in1, state<T> &in2, state<T> &energy);

	//! compute value of in2 that minimizes the energy, given in1
	virtual void infer2(state<T> &i1, state<T> &scale, infer_param &ip,
											state<T> *energy = NULL);
	//! Returns a string describing this module and its parameters.
	virtual std::string describe();
	// members
 public:
	idx<T>          last_target_raw;   //!< Un-normalized target.
	state<T>        last_target;       //!< Last target selected by fprop.
 protected:
	uint            jsize;             //!< Number of jitter elements.
	bool            apply_tanh;        //!< If true, apply tanh to inputs.
	uint            jitter_selection;  //!< Selection type.
	float           dist_coeff;        //!< Coefficient of distance score.
	float           scale_coeff;       //!< Coefficient of scale score.
	state<T>        tmp;               //!< Temporary buffer.
	state<T>        tmp2;              //!< Temporary buffer.
	state<T>        last_class_target; //!< Last class target selected.
	state<T>        last_jitt_target;  //!< Last jitter target selected.
	state<T>        last_conf_target;  //!< Last confidence target selected.
	idx<T>          best_target;       //!< The last target selected by fprop.
	tanh_module<T>  mtanh;             //!< A tanh module.
	bool            predict_conf;      //!< Predict confidence.
	bool            predict_bconf;     //!< Predicted confidence is binary or not.
	idx<T>         *biases;            //!< Normalization biases: scale,h,w,conf
	idx<T>         *coeffs;            //!< Normalization coeffs: scale,h,w,conf
};

// scaler_energy ///////////////////////////////////////////////////////////////

template<typename T> class scaler_energy : public ebm_2<T> {
 public:
	//! constructor.
	//! \param spatial Use spatial jitter or not.
	scaler_energy(const char *name = "scaler_energy");
	//! destructor.
	virtual ~scaler_energy();

	//! Forward propagation from 'in' tensor to 'out' tensor.
	//! Note: because a state object cast to idx is its forward tensor,
	//!   you can also pass state objects directly here.
	virtual void fprop1(idx<T> &in1, idx<T> &in2, idx<T> &energy);
	//! 1st order backward propagation from out to in (first state tensor only).
	virtual void bprop1(state<T> &in1, state<T> &in2, state<T> &energy);
	//! 2nd order backward propagation from out to in (first state tensor only).
	virtual void bbprop1(state<T> &in1, state<T> &in2, state<T> &energy);

	//! compute value of in2 that minimizes the energy, given in1
	virtual void infer2(state<T> &i1, state<T> &in2, infer_param &ip,
											state<T> *energy = NULL);
	//! Returns a string describing this module and its parameters.
	virtual std::string describe();
};

} // namespace ebl {

#include "ebl_energy.hpp"

#endif /* EBL_ENERGY_H_ */
