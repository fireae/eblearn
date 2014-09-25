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

#ifndef EBL_NORMALIZATION_H_
#define EBL_NORMALIZATION_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_basic.h"
#include "ebl_arch.h"
#include "ebl_nonlinearity.h"

namespace ebl {

//! Small value added in normalization to avoid 0-divisions.
#define NORM_EPSILON 1e-6
#define DEFAULT_GAUSSIAN_COEFF 2.0

// divisive_norm ///////////////////////////////////////////////////////////////

//! This module divides local neighborhoods by they standard deviation.
template <typename T> class divisive_norm_module : public module_1_1<T> {
 public:
  //! Empty constructor to be used by a child class.
  divisive_norm_module(const char *name = "divisive_norm");
  //! \param kerdim The neighborhood dimensions.
  //! \param nf The number of feature maps input to this module.
  //! \param mirror Use mirroring of the input to pad border if true,
  //!               or use zero-padding otherwise (default).
  //! \param p If specified, parameter p holds learned weights.
  //! \param across_features If true, normalize across feature dimensions
  //!   in addition to spatial dimensions.
  //! \param cgauss Gaussian kernel coefficient.
  divisive_norm_module(idxdim &kerdim, int nf, bool mirror = false,
                       bool threshold = true, parameter<T> *p = NULL,
                       const char *name = "divisive_norm",
                       bool across_features = true,
		       double cgauss = DEFAULT_GAUSSIAN_COEFF,
                       bool fsum_div = false, float fsum_split = 1.0,
                       double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~divisive_norm_module();

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
  //! Pre-determine the order of hidden buffers to use only 2 buffers
  //! in order to reduce memory footprint.
  //! This returns true if outputs is actually put in out, false if it's
  //! in in.
  virtual bool optimize_fprop(state<T> &in, state<T> &out);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Update epsilon value.
  virtual void set_epsilon(double eps);

  // friends
  friend class divisive_norm_module_gui;
  friend class contrast_norm_module_gui;
  template <typename T1> friend class contrast_norm_module;
  template <typename T1>
  friend class cuda_contrast_norm_module;

  // internal methods ////////////////////////////////////////////////////////
 protected:
  //! Common initializations.
  virtual void init(idxdim &kerdim_, int nf, bool mirror_, bool threshold_,
                    parameter<T> *param_, bool af, double cgauss_,
                    bool fsum_div_, float fsum_split_, double epsilon_,
                    double epsilon2_);
  //! Copies and replicates kernel into divconv.
  virtual void set_kernel(idx<T> &kernel);
  //! out = in / thstd
  virtual void invert(idx<T> &in, idx<T> &thstd, idx<T> &out);

  // members ////////////////////////////////////////////////////////
 public:
  bool                	 mirror;        //!< mirror input or not.
 protected:
  parameter<T>          *param;
  layers<T>              convvar;
  power_module<T>	*sqrtmod;	//!< square root module
  power_module<T>	 invmod;	//!< inverse module
  power_module<T>	*sqmod;         //!< square module
  thres_module<T>	 thres;         //!< threshold module
  mul_module<T>          mcw;
  convolution_module<T> *divconv;
  module_1_1<T>         *padding;
  idx<T>              	 gaussian_kernel;
  idx<intg>              conv_table;
  idxdim                 stride;
  state<T>  	         insq, invar, instd, thstd, invstd;
  bool                	 threshold;
  int                 	 nfeatures;
  idxdim              	 kerdim;
  bool 			 across_features; //!< Norm across feats.
  double                 epsilon;       //!< bias to avoid div by 0.
  double                 epsilon2;      //!< bias to avoid div by 0.
  double                 cgauss;        //!< Gaussian coefficient.
  bool                   fsum_div;
  float                  fsum_split;
};

// subtractive_norm ////////////////////////////////////////////////////////////

//! Subtractive normalization operation using a weighted expectation
//! over a local neighborhood. An input set of feature maps is locally
//! normalized to be zero mean.
template <typename T> class subtractive_norm_module : public module_1_1<T> {
 public:
  //! Empty constructor to be used by a child class.
  subtractive_norm_module(const char *name = "subtractive_norm");
  //! \param kerdim The kernel dimensions.
  //! \param nf The number of feature maps input to this module.
  //! \param mirror Use mirroring of the input to pad border if true,
  //!               or use zero-padding otherwise (default).
  //! \param global_norm If true, apply global normalization first.
  //! \param p If specified, parameter p holds learned weights.
  //! \param across_features If true, normalize across feature dimensions
  //!   in addition to spatial dimensions.
  //! \param learn_mean If true, learn mean weighting.
  //! \param cgauss Gaussian kernel coefficient.
  //! \param valid If true, perform a valid convolution rather than a 'same' one
  subtractive_norm_module(idxdim &kerdim, int nf, bool mirror = false,
                          bool global_norm = false,
                          parameter<T> *p = NULL,
                          const char *name = "subtractive_norm",
                          bool across_features = true,
                          double cgauss = DEFAULT_GAUSSIAN_COEFF,
			  bool fsum_div = false,
                          float fsum_split = 1.0, bool valid = false);
  //! destructor
  virtual ~subtractive_norm_module();

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
  //! Pre-determine the order of hidden buffers to use only 2 buffers
  //! in order to reduce memory footprint.
  //! This returns true if outputs is actually put in out, false if it's
  //! in in.
  virtual bool optimize_fprop(state<T> &in, state<T> &out);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // friends
  friend class contrast_norm_module_gui;
  template <typename T1> friend class contrast_norm_module;
  template <typename T1>
  friend class cuda_contrast_norm_module;

  // internal methods ////////////////////////////////////////////////////////
 protected:
  //! Common initializations.
  virtual void init(idxdim &kerdim_, int nf, bool mirror, bool global_norm,
                    parameter<T> *param_, bool af, double cgauss,
                    bool fsum_div, float fsum_split_, bool valid);
  //! Copies and replicates kernel into divconv.
  virtual void set_kernel(idx<T> &kernel);

  // members ////////////////////////////////////////////////////////
 protected:
  parameter<T>          *param;
  layers<T>              convmean;
  convolution_module<T> *meanconv;
  module_1_1<T>         *padding;
  idx<T>              	 gaussian_kernel;
  idx<intg>              conv_table;
  diff_module<T>	 difmod;        //!< difference module
  idxdim                 stride;
  state<T>  	         inmean;
  bool                	 global_norm;   //!< global norm first
  int                 	 nfeatures;
  idxdim              	 kerdim;
  bool 			 across_features; //!< Norm across feats.
  bool                	 mirror;        //!< mirror input or not.
  double                 cgauss;        //!< Gaussian coefficient.
  bool                   fsum_div;
  float                  fsum_split;
  bool                   valid;
};

// contrast_norm ///////////////////////////////////////////////////////////////

//! Local contrast normalization operation using a weighted expectation
//! over a local neighborhood. An input set of feature maps is locally
//! normalized to be zero mean and unit standard deviation.
template <typename T>
class contrast_norm_module : public module_1_1<T> {
 public:
  //! Empty constructor to be used by a child class.
  contrast_norm_module(const char *name = "contrast_norm");
  //! \param kerdim The kernel dimensions.
  //! \param nf The number of feature maps input to this module.
  //! \param mirror Use mirroring of the input to pad border if true,
  //!               or use zero-padding otherwise (default).
  //! \param global_norm If true, apply global normalization first.
  //! \param p If specified, parameter p holds learned weights.
  //! \param across_features If true, normalize across feature dimensions
  //!   in addition to spatial dimensions.
  //! \param learn_mean If true, learn mean weighting.
  //! \param cgauss Gaussian kernel coefficient.
  //! \param valid If true, perform a valid convolution rather than a 'same' one
  contrast_norm_module(idxdim &kerdim, int nf, bool mirror = false,
                       bool threshold = true, bool global_norm = false,
                       parameter<T> *p = NULL,
                       const char *name = "contrast_norm",
                       bool across_features = true, bool learn_mean = false,
                       double cnorm = DEFAULT_GAUSSIAN_COEFF,
		       bool fsum_div = false,
                       float fsum_split = 1.0, double epsilon = NORM_EPSILON,
                       double epsilon2 = 0.0, bool valid = false);
  //! destructor
  virtual ~contrast_norm_module();

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
  //! Pre-determine the order of hidden buffers to use only 2 buffers
  //! in order to reduce memory footprint.
  //! This returns true if outputs is actually put in out, false if it's
  //! in in.
  virtual bool optimize_fprop(state<T> &in, state<T> &out);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Update epsilon value.
  virtual void set_epsilon(double eps);

  // friends
  friend class contrast_norm_module_gui;

  // members ////////////////////////////////////////////////////////
 protected:
  subtractive_norm_module<T> *subnorm;
  divisive_norm_module<T>    *divnorm;
  state<T>  	              tmp;
  bool                	      global_norm; //!< global norm first
  bool                	      learn_mean; //!< Learn mean weighting.
};

// laplacian_module ////////////////////////////////////////////////////////////

//! Laplacian pyramid normalization: input is filtered with a 5x5 kernel
//! and the result is subtracted to the input, resulting in a high-pass
//! filtered output.
template <typename T> class laplacian_module : public module_1_1<T> {
 public:
  //! \param nf The number of feature maps input to this module.
  //! \param mirror Use mirroring of the input to pad border if true,
  //!               or use zero-padding otherwise (default).
  //! \param global_norm If true, apply global normalization first.
  laplacian_module(int nf, bool mirror = false, bool global_norm = false,
                   const char *name = "laplacian_module");
  //! destructor
  virtual ~laplacian_module();

  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);

  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // members ////////////////////////////////////////////////////////
 private:
  bool                   mirror;        //!< mirror input or not in borders
  convolution_module<T> *conv;
  module_1_1<T>         *pad;
  diff_module<T>         diff;
  state<T>               padded, filtered;
  parameter<T>           param;
  idx<T>                 filter;	//!< Burt-Adelson 5x5 filter.
  bool                   global_norm;   //!< perform global normalization first
  int                    nfeatures;
  idxdim                 kerdim;
};

} // namespace ebl {

#include "ebl_normalization.hpp"

#endif /* EBL_NORMALIZATION_H_ */
