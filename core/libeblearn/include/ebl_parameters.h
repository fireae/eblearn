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

#ifndef EBL_PARAMETERS_H_
#define EBL_PARAMETERS_H_

#include "ebl_state.h"

namespace ebl {

// inference parameters //////////////////////////////////////////////////////

class infer_param {
};

// gradient parameters ///////////////////////////////////////////////////////

//! A class that contains all the parameters
//! for a stochastic gradient descent update,
//! including step sizes, regularizer coefficients...
class EXPORT gd_param: public infer_param {
public:
  //! global step size
  double eta;
  //! stopping criterion threshold
  double n;
  //! L1 regularizer coefficient
  double decay_l1;
  //! L2 regularizer coefficient
  double decay_l2;
  //! Time (in number of training samples) after which to decay values
  intg decay_time;
  //! momentum term
  double inertia;
  //! annealing coefficient for the learning rate
  double anneal_value;
  //! Number of training samples beetween two annealings.
  intg anneal_period;
  //! threshold on square norm of gradient for stopping
  double gradient_threshold;
  //! for debugging purpose
  int niter_done;

  //! Empty constructor, set all variables to zero.
  gd_param();

  //! Constructor.
  gd_param(double leta, double ln, double l1, double l2, intg dtime,
	   double iner, double a_v, intg a_p, double g_t);
};

EXPORT std::ostream& operator<<(std::ostream &out, const gd_param &p);

// initialization parameters /////////////////////////////////////////////////

//! abstract class for randomization parameters
class forget_param {
};

class EXPORT forget_param_linear: public forget_param {
public:
  //! Construct a random initializer from uniform distribution
  //! from [-v/(fanin**exponent), +v/(fanin**exponent)].
	//! \param random_seed If true, use time as random seed, otherwise
	//!   use a fixed seed of 0. For better randomization use seed().
  forget_param_linear(double v, double e, bool random_seed);

  //! Initialize seed from current time and the sum of all characters of s.
  //! This can be useful if several programs are called at the
  //! same time with different configurations with s being a print out of
	//! all configuration variables.
  void seed(std::string &s);

  // public members
public:
  //! each random value will be drawn uniformly
  //! from [-value/(fanin**exponent), +value/(fanin**exponent)]
  double value;
  double exponent;
  random generator; //!< Random number generator.
};

// parameter /////////////////////////////////////////////////////////////////

//! A forward set of parameters.
template <typename T> class parameter : public state<T> {
 public:
  //! initialize the bbparameter with size initial_size.
  parameter(intg initial_size = 100);
  //! initialize the bbparameter with a previously saved x component.
  parameter(const char *param_filename);
  //! destructor
  virtual ~parameter();

  //! Resize the current parameter to size 's0'.
  void resize_parameter(intg s0);

  // I/O methods /////////////////////////////////////////////////////////////

  //! Given a vector of matrix filenames, load each of them separately,
  //! concatenate it along their dimension 0, then load the resulting matrix
  //! into this parameter's x component.
  bool load_x(std::vector<std::string> &files);
  //! Load forward weights from a stored matrix.
  bool load_x(const char *filename);
  //! Load forward weights from matrix m.
  bool load_x(idx<T> &m);
  //! Saves the forward component to a file.
  bool save_x(const char *filename);
  //! Permute blocks in x following permutation vector.
  void permute_x(std::vector<intg> &blocks, std::vector<uint> &permutations);

  // weights manipulation ////////////////////////////////////////////////////

  //! Clear average derivatives 'deltax'.
  void clear_deltax();
  //! Clear average second-derivatives 'ddeltax'.
  void clear_ddeltax();
  //! Set all epsilons (each individual learning rate) to 'm'.
  void set_epsilon(T m);
  //! Compute each individual learning rates (epsilons)
  //! based on second derivatives.
  void compute_epsilons(T mu);
  //! Update the forward weights (f), using current gradients (b).
  virtual void update(gd_param &arg);
  //! Update average derivatives 'deltax'. This is called
  //! by update() method, whenever update() is called with
  //! a non-zero inertia parameter.
  void update_ddeltax(T knew, T kold);

  // protected methods ///////////////////////////////////////////////////////
 protected:
  //! Update weights and gradients.
  //! Depending on gradient parameters 'arg', an l1 and/or l2 regularization
  //! decay may first be applied to the gradients (b).
  //! The forward weights (f) are then updated given the current gradients
  //! and learning rate.
  void update_weights(gd_param &arg);
  //! Update average derivatives 'deltax'. This is called
  //! by update() method, whenever update() is called with
  //! a non-zero inertia parameter.
  void update_deltax(T knew, T kold);

  // members /////////////////////////////////////////////////////////////////
 public:
  idx<T> deltax;   //!< Average derivatives.
  idx<T> epsilons; //!< Individual learning rates.
  idx<T> ddeltax;  //!< Average second-derivatives.
  using state<T>::x;
  using state<T>::dx;
  using state<T>::ddx;
};

// dparameter ////////////////////////////////////////////////////////////////

//! A trainable set of parameter.
template <typename T> class dparameter : public parameter<T> {
 public:
  //! initialize the ddparameter with size initial_size.
  dparameter(intg initial_size = 100);
  //! initialize the ddparameter with a previously saved x component.
  dparameter(const char *param_filename);
  //! destructor
  virtual ~dparameter();
};

// ddparameter ////////////////////////////////////////////////////////////////

//! A 2nd-order trainable set of parameter.
template <typename T> class ddparameter : public parameter<T> {
 public:
  //! initialize the bddparameter with size initial_size.
  ddparameter(intg initial_size = 100);
  //! initialize the bddparameter with a previously saved x component.
  ddparameter(const char *param_filename);
  //! destructor
  virtual ~ddparameter();
};

} // namespace ebl {

#include "ebl_parameters.hpp"

#endif /* EBL_PARAMETERS_H_ */
