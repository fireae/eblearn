/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun, Pierre Sermanet and Soumith Chintala*
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com, soumith@gmail.com  *
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

#ifndef EBL_POOLING_H_
#define EBL_POOLING_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_utils.h"
#include "ebl_preprocessing.h"

namespace ebl {

// subsampling_module //////////////////////////////////////////////////////////

//! This module applies 2D subsampling on dimensions 1 and 2
//! (0 contains different layers of information).
//! This module has a replicable order of 3, if the input has a bigger order,
//! use the replicable version of this module:
//! subsampling_module_replicable.
template <typename T> class subsampling_module: public module_1_1<T> {
 public:
  //! Constructor.
  //! \param p is used to store all parametric variables in a single place.
  //!        If p is null, a local buffer will be used.
  //! \param thickness The number of features.
  //! \param kernel Size of subsampling kernel (without thickness).
  //! \param stride Stride of subsampling kernel (without thickness).
  //! \param crop If true, crop input when it does not match with the kernel.
  //!          This allows to feed any input size to this module.
  //! \param pad If true, add padding to end of non-feature dimensions to
  //!   match the kernel when not matching.
  subsampling_module(parameter<T> *p, uint thickness, idxdim &kernel,
                     idxdim &stride, const char *name = "subsampling",
                     bool crop = true, bool pad = true);
  //! Destructor.
  virtual ~subsampling_module();

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This is useful for debugging.
  virtual void fprop1_dump(idx<T> &in, idx<T> &out);

  //! forgetting weights by replacing with random values
  virtual void forget(forget_param_linear &fp);
  //! order of operation
  virtual int replicable_order() { return 3; }
  //! resize the output based on input dimensions
  virtual bool resize_output(idx<T> &in, idx<T> &out, idxdim *ignore = NULL);
  //! resize the output based on input dimensions
  virtual bool resize_output(state<T> &in, state<T> &out, idxdim *ignore = NULL);
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

  // internal methods
 private:
  //! Generic forward resize.
  template <class Tstate> bool resize_forward(Tstate &in, Tstate &out);

  // members
 public:
  state<T> coeff;     //!< Learned averaging coefficients.
  state<T> sub;       //!< Temporary buffer to hold sum of neighborhood.
  uint     thickness; //!< Number of features.
  idxdim   kernel;    //!< Dimensions of subsampling kernel.
  idxdim   stride;    //!< Strides of subsampling.
 protected:
  bool     crop;      //!< Crop input when size mismatch or not.
  bool     pad;       //!< Pad output when size mismatch or not.
};

//! The replicable version of subsampling_module.
//! If the input has a bigger order than the replicable_order() of
//! subsampling_module, then this module loops on extra dimensions until
//! it reaches the replicable order, and then calls the base module
//! subsampling_module.
//! For example, if the base module works on an order of 3, an input with
//! dimensions <2x16x16x9x9> will produce a 9x9 grid where each box contains
//! the output of the processing of each <2x16x16> slice.
DECLARE_REPLICABLE_MODULE_1_1(
    subsampling_module_replicable, subsampling_module, T,
    (parameter<T> *p, uint thickness, idxdim &kernel, idxdim &strides,
     const char *name = "subsampling_replicable"),
    (p, thickness, kernel, strides, name));

// lppooling_module ////////////////////////////////////////////////////////////

//! This module takes the l2 norm of neighborhoods with a stride,
//! e.g. a 5x5 l2 pooling with a stride of 2x2.
template <typename T> class lppooling_module: public module_1_1<T> {
 public:
  //! Constructor.
  //! \param p is used to store all parametric variables in a single place.
  //!        If p is null, a local buffer will be used.
  //! \param thickness The number of features.
  //! \param kernel Size of subsampling kernel (without thickness).
  //! \param stride Stride of subsampling kernel (without thickness).
  //! \param crop If true, crop input when it does not match with the kernel.
  //!          This allows to feed any input size to this module.
  lppooling_module(uint thickness, idxdim &kernel, idxdim &stride,
                   uint lppower = 2, const char *name = "lppooling",
                   bool crop = true);
  //! destructor
  virtual ~lppooling_module();

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

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
  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This can be useful for debugging.
  virtual void fprop1_dump(idx<T> &in, idx<T> &out);

  // members /////////////////////////////////////////////////////////////////
 protected:
  uint                   thickness;     //!< Number of features.
  idxdim                 kernel;        //!< Dimensions of subsampling kernel.
  idxdim                 stride;        //!< Strides of subsampling.
  bool                   crop;          //!< Crop input if size mismatch or not.
  uint                   lp_pow;        //!< the P in LP Pooling
  convolution_module<T> *conv;
  power_module<T>        sqmod;
  power_module<T>        sqrtmod;
  state<T>               squared, convolved; //!< Intermediate buffer.
  parameter<T>           param;
};

//////////////////////////////////////////////////////////////////////////////
// wavg_pooling_module

//! This module takes a weighted average ofneighborhoods with a stride,
//! e.g. a 5x5 avg pooling with a stride of 2x2.
template <typename T>
class wavg_pooling_module: public module_1_1<T> {
 public:
  //! Constructor.
  //! \param p is used to store all parametric variables in a single place.
  //!        If p is null, a local buffer will be used.
  //! \param thickness The number of features.
  //! \param kernel Size of subsampling kernel (without thickness).
  //! \param stride Stride of subsampling kernel (without thickness).
  //! \param crop If true, crop input when it does not match with the kernel.
  //!          This allows to feed any input size to this module.
  wavg_pooling_module(uint thickness, idxdim &kernel, idxdim &stride,
                      const char *name = "wavg_pooling", bool crop = true);
  //! destructor
  virtual ~wavg_pooling_module();

  //! Forward propagation from 'in' tensor to 'out' tensor.
  //! Note: because a state object cast to idx is its forward tensor,
  //!   you can also pass state objects directly here.
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! 1st order backward propagation from out to in (first state tensor only).
  virtual void bprop1(state<T> &in, state<T> &out);
  //! 2nd order backward propagation from out to in (first state tensor only).
  virtual void bbprop1(state<T> &in, state<T> &out);

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
  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This can be useful for debugging.
  virtual void fprop1_dump(idx<T> &in, idx<T> &out);

  // members ////////////////////////////////////////////////////////
 protected:
  uint                thickness; //!< Number of features.
  idxdim              kernel; //!< Dimensions of subsampling kernel.
  idxdim              stride; //!< Strides of subsampling.
  bool                crop; //! Crop input when size mismatch or not.
  convolution_module<T> *conv;
  parameter<T>	param;
};

// pyramid_module //////////////////////////////////////////////////////////////

//! Creates a pyramid of the input.
template <typename T> class pyramid_module : public resizepp_module<T> {
 public:
  //! Constructor. Preprocessing module pp will be deleted upon destruction.
  //! \param nscales Number of scales of pyramid, starting from target
  //!   dimensions down with a subsampling ratio of 2.
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param pp_original Preprocessing for original channel.
  //! \param add_original If true, the first object in the output will
  //!   be the original preprocessed patch of target size, without Laplacian.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param size The target dimensions (heightxwidth)
  //! \param zpad Optional zero-padding is added on each side
  pyramid_module(uint nscales, float scaling_ratio, idxdim &dsize,
                 uint mode = MEAN_RESIZE, module_1_1<T> *pp = NULL,
                 bool own_pp = false, idxdim *dzpad = NULL,
                 const char *name = "pyramid_module");
  //! Constructor without target dimensions. set_dimensions should be called
  //! later. Preprocessing module pp will be deleted upon destruction.
  //! \param nscales Number of scales of pyramid, starting from target
  //!   dimensions down with a subsampling ratio of 2.
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param pp_original Preprocessing for original channel.
  //! \param add_original If true, the first object in the output will
  //!   be the original preprocessed patch of target size, without Laplacian.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param zpad Optional zero-padding is added on each side
  pyramid_module(uint nscales, float scaling_ratio, uint mode = MEAN_RESIZE,
                 module_1_1<T> *pp = NULL, bool own_pp = false,
                 idxdim *dzpad = NULL, const char *name = "pyramid_module");
  virtual ~pyramid_module();
  //! Process 'in' into 'out' which will contain all scales separated in
  //! each state of the multi-state 'out'.
  virtual void fprop(state<T> &in, state<T> &out);
  //! Process 'in' into 'out' which will contain an array of idx, where each
  //! idx has different scale with different dimensions.
  virtual void fprop(state<T> &in, midx<T> &out);
  //! backward propagation from in to out (empty)
  virtual void bprop(state<T> &in, state<T> &out);
  //! bbackward propagation from in to out (empty)
  virtual void bbprop(state<T> &in, state<T> &out);
  //! Returns input dimensions corresponding to output dimensions 'osize'.
  virtual mfidxdim bprop_size(mfidxdim &osize);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // members
 protected:
  uint  nscales;                  //!< Number of scales from target size down.
  float scaling_ratio;            //!< Ratio between scales.
};

// average_pyramid_module //////////////////////////////////////////////////////

//! Creates a pyramid of the input with average subsampling.
template <typename T> class average_pyramid_module : public module_1_1<T> {
 public:
  //! Creates a pyramid with 'strides.size()' scales, each of which
  //! is constructed with an average subsampling with each stride.
  //! \param p is used to store all parametric variables in a single place.
  //!        If p is null, a local buffer will be used.
  //! \param thickness The number of features.
  average_pyramid_module(parameter<T> *p, uint thickness,
                         midxdim &strides,
                         const char *name = "average_pyramid_module");
  virtual ~average_pyramid_module();

  //! Process 'in' into 'out' which will contain all scales separated in
  //! each state of the multi-state 'out'.
  virtual void fprop(state<T> &in, state<T> &out);
  //! backward propagation from in to out (empty)
  virtual void bprop(state<T> &in, state<T> &out);
  //! bbackward propagation from in to out (empty)
  virtual void bbprop(state<T> &in, state<T> &out);

  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns input dimensions corresponding to output dimensions 'osize'.
  virtual mfidxdim bprop_size(mfidxdim &osize);

  // members ////////////////////////////////////////////////////////
 protected:
  midxdim strides; //!< Strides for each scale.
  std::vector<subsampling_module<T>*> mods; //!< Subsampling modules.
  bool well_behaved; //!< True if strides increase by multiples of previous.
};

// maxss_module ////////////////////////////////////////////////////////////////

//! This module applies max subsampling.
template <typename T> class maxss_module : public subsampling_module<T> {
 public:
  //! Constructor.
  //! \param thickness The number of features.
  //! \param kernel Size of subsampling kernel (without thickness).
  //! \param stride Stride of subsampling kernel (without thickness).
  maxss_module(uint thickness, idxdim &kernel, idxdim &stride,
               const char *name = "maxss");
  //! Destructor.
  virtual ~maxss_module();

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

  // members
 protected:
  using subsampling_module<T>::thickness;
  using subsampling_module<T>::kernel;
  using subsampling_module<T>::stride;
  idx<int> switches;         //!< Remember max locations
  bool     float_precision;  //!< check the precision of the module
  bool     double_precision; //!< check the precision of the module
  //#ifdef __TH__
  idx<T>   indices;          //!< Remember max locations
  //#endif
};

} // namespace ebl {

#include "ebl_pooling.hpp"

#endif /* EBL_POOLING_H_ */
