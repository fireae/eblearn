/***************************************************************************
 *   Copyright (C) 2011 by Yann LeCun, Pierre Sermanet and Soumith Chintala*
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

#ifndef EBL_BASIC_H_
#define EBL_BASIC_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_utils.h"
#include "bbox.h"

namespace ebl {
//! \defgroup network_modules The modules made for stacking into a network.

// linear_module ///////////////////////////////////////////////////////////////

//! This module applies a linear combination of the input <in>
//! with its internal weight matrix w and puts the result in the output.
//! This module has a replicable order of 1, if the input has a bigger order,
//! use the replicable version of this module: linear_module_replicable.
//! \brief Module: \f$\vec{out} = W \vec{in}\f$.
//! \ingroup network_modules
template <typename T> class linear_module: public module_1_1<T> {
 public:
  //! Constructor.
  //! \param p is used to store all parametric variables in a single place.
  //!        If p is null, a local buffer will be used.
  //! \param in the size of the input to the linear combination.
  //! \param out the size of the output to the linear combination.
  linear_module(parameter<T> *p, intg in, intg out, const char *name ="linear");
  //! destructor
  virtual ~linear_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
  //! order of operation
  virtual int replicable_order() { return 1; }
  //! forgetting weights by replacing with random values
  virtual void forget(forget_param_linear &fp);
  //! normalize
  virtual void normalize();
  //! Return dimensions that are compatible with this module.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim fprop1_size(fidxdim &i_size);
  //! Return dimensions compatible with this module given output dimensions.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Returns a deep copy of this module.
  //! \param p If NULL, reuse current parameter space, otherwise allocate new
  //!   weights on parameter 'p'.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Copy passed weights into x component of internal weights.
  virtual void load_x(idx<T> &weights);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This can be useful for debugging.
  virtual void fprop1_dump(idx<T> &in, idx<T> &out);

  // members
 public:
  state<T> w;
};

/**
 The replicable version of linear_module.
 If the input has a bigger order than the replicable_order() of
 linear_module, then this module loops on extra dimensions until
 it reaches the replicable order, and then calls the base module
 linear_module.
 For example, if the base module works on an order of 1, an input with
 dimensions <42x9x9> will produce a 9x9 grid where each box contains
 the output of the processing of each <42> slice.
 \brief Module: \f$\vec{out}_i = W \vec{in}_i
                   \;\;\forall i = (i_2, i_3, ..., i_{k+1});
                   \vec{in} \in R^{m\times k+1}\f$
 \ingroup network_modules
 */
DECLARE_REPLICABLE_MODULE_1_1(linear_module_replicable,
                              linear_module, T,
                              (parameter<T> *p, intg in, intg out,
                               const char *name = "linear_replicable"),
                              (p, in, out, name));

// convolution_module //////////////////////////////////////////////////////////

/**
 * This module applies 2D convolutions on dimensions 1 and 2
 * (0 contains different layers of information).
 * This module has a replicable order of 3, if the input has a bigger order,
 * use the replicable version of this module:
 * convolution_module_replicable.
 * \ingroup network_modules
 * \brief Module: \f$ \vec{out}_i = (K \otimes \vec{in}_i)(::s,::s) \;\; \forall i = 1 ... m\f$,
 * \f$ \vec{in} \in R^{m\times m_1 \times  m_2}, s\in N \f$
 */
template <typename T> class convolution_module : public module_1_1<T> {
 public:
  //! Constructor.
  //! \param p is used to store all parametric variables in a single place.
  //!        If p is null, a local buffer will be used.
  //! \param ker The convolution kernel sizes. (\f$a \times b\f$)
  //! \param stride The convolution strides.
  //!        stride.dim(0) is used for dimension  1, stride.dim(1) for dimension
  //!        2. Strides must divide input dimensions with no reminder unless
  //!        crop is true.
  //! \param table is the convolution connection table.
  //!        This means a \f$ n \times 2 \f$ integer idx, where table[i,0] is
  //!        the source field and table[i,1] is the destinataion connection.
  //!  
  //! \param crop If true, crop input when it does not match with the kernel.
  //!          This allows to feed any input size to this module.
  convolution_module(parameter<T> *p, idxdim &ker, idxdim &stride,
                     idx<intg> &table, const char *name = "convolution",
                     bool crop = true);
  //! destructor
  virtual ~convolution_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
  //! forgetting weights by replacing with random values
  virtual void forget(forget_param_linear &fp);
  //! order of operation
  virtual int replicable_order() { return 3; }
  //! resize the output based on input dimensions
  //! This returns true if output was resized/reallocated, false otherwise.
  virtual bool resize_output(idx<T> &in, idx<T> &out, idxdim *ignore = NULL);
  //! Return dimensions that are compatible with this module.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim fprop1_size(fidxdim &i_size);
  //! Return dimensions compatible with this module given output dimensions.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Returns a deep copy of this module.
  //! \param p If NULL, reuse current parameter space, otherwise allocate new
  //!   weights on parameter 'p'.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Copy passed weights into x component of internal weights.
  virtual void load_x(idx<T> &weights);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This can be useful for debugging.
  virtual void fprop1_dump(idx<T> &in, idx<T> &out);

  // members /////////////////////////////////////////////////////////////////
 public:
  intg			tablemax;
  state<T>  kernel;
  intg			thickness;
  idxdim    ker;
  idxdim    stride;
  idx<intg> table;             //!< table of connections btw input and output
 protected:
  bool			warnings_shown;
  bool      fulltable;         //!< indicating if full-table or not
  bool      float_precision;   //!< used for IPP and TH
  bool      double_precision;  //!< used for TH
  bool      crop;              //! Crop input when size mismatch or not.
  // IPP members /////////////////////////////////////////////////////////////
  idx<T>    revkernel;         //!< a reversed kernel for IPP
  idx<T>    outtmp;            //!< a tmp buffer for IPP conv output
  bool      ipp_err_printed;   //!< Print an error msg only once.
  bool      use_ipp;           //!< IPP is useable or not.
};

//! The replicable version of convolution_module.
//! If the input has a bigger order than the replicable_order() of
//! convolution_module, then this module loops on extra dimensions until
//! it reaches the replicable order, and then calls the base module
//! convolution_module.
//! For example, if the base module works on an order of 3, an input with
//! dimensions <2x16x16x9x9> will produce a 9x9 grid where each box contains
//! the output of the processing of each <2x16x16> slice.
DECLARE_REPLICABLE_MODULE_1_1(
    convolution_module_replicable, convolution_module, T,
    (parameter<T> *p, idxdim &ker, idxdim &stride, idx<intg> &table,
     const char *name = "convolution_replicable"),
    (p, ker, stride, table, name));

// addc_module /////////////////////////////////////////////////////////////////

//! The constant add module adds biases to the first dimension of the input
//! and puts the results in the output. This module is spatially replicable
//! (the input can have an order greater than 1 and the operation will apply
//! to all elements).
template <typename T>
class addc_module: public module_1_1<T> {
 public:
  //! Constructor.
  //! \param p is used to store all parametric variables in a single place.
  //!        If p is null, a local buffer will be used.
  //! \param size is the number of biases, or the size of dimensions 0 of
  //! inputs and outputs.
  addc_module(parameter<T> *p, intg size, const char *name = "addc");
  //! destructor
  virtual ~addc_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
  //! forgetting weights by replacing with random values
  virtual void forget(forget_param_linear &fp);
  //! Returns a deep copy of this module.
  //! \param p If NULL, reuse current parameter space, otherwise allocate new
  //!   weights on parameter 'p'.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Copy passed weights into x component of internal weights.
  virtual void load_x(idx<T> &weights);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This can be useful for debugging.
  virtual void fprop1_dump(idx<T> &in, idx<T> &out);

  // members
 public:
  state<T>  bias; //!< the biases
};

// power_module ////////////////////////////////////////////////////////////////

//! x^p module. p can be nay real number
//! the derivatives are implemented using
//! polynomial derivative rules, so they are exact
//! The derivative implementation divides output by input
//! to get x^(p-1), therefore this module assumes that
//! the :input:x and :output:x is not modified until bprop
// TODO: write specialized modules square and sqrt to run faster
template <typename T> class power_module : public module_1_1<T> {
 public:
  //! <p> is double number, every element of input is raised to
  //! its <p>th power.
  power_module(T p, const char *name = "power");
  //! destructor
  virtual ~power_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);

  // members
 protected:
  T p;
  idx<T> tt; //!< temporary buffer
};

// diff_module /////////////////////////////////////////////////////////////////

//! Elementwise subtraction class.
//! Derived from module-2-1.
template <typename T> class diff_module : public module_2_1<T> {
 public:
  //! constructor.
  diff_module();
  //! destructor
  virtual ~diff_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in1, idx<T> &in2, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in1, state<T> &in2, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in1, state<T> &in2, state<T> &out);
};

// mul_module //////////////////////////////////////////////////////////////////

//! Elementwise multiplication class.
//! Derived from module-2-1.
template <typename T> class mul_module : public module_2_1<T> {
 private:
  idx<T> tmp; //!< temporary buffer

 public:
  //! constructor.
  mul_module();
  //! destructor
  virtual ~mul_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in1, idx<T> &in2, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in1, state<T> &in2, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in1, state<T> &in2, state<T> &out);
};

// thres_module ////////////////////////////////////////////////////////////////

//! A thresholding module that filters the input and any entry that is
//! smaller then a given threshold is set to a specified value.
template <typename T> class thres_module : public module_1_1<T> {
 public:
  //! <thres> is the threshold value that is used to filter the
  //! input.
  //! <val> is the value that is used to replace any input entry.
  //! smaller than <thres>.
  thres_module(T thres, T val, const char *name = "threshold");
  //! destructor
  virtual ~thres_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

 public:
  T thres;
  T val;
};

// cutborder_module ////////////////////////////////////////////////////////////

//! opposite of zero padding, sometimes one needs to
//! cut the borders of an input to make it usable with
//! a convolved output
template <typename T> class cutborder_module : module_1_1<T> {
 private:
  int nrow, ncol;

 public:
  //! <nrow> and <ncol> are the number of rows and colums
  //! that is going to be removed from borders.
  //! The output size is (nrow-2*nr and ncols-2*ncol)
  //! for each feature map.
  cutborder_module(int nr, int nc);
  //! destructor
  virtual ~cutborder_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
};

// zpad_module /////////////////////////////////////////////////////////////////

//! a simple zero padding module that is mostly usefull for doing
//! same size output convolutions.
template <typename T> class zpad_module : public module_1_1<T> {
 public:
  //! Empty constructor. User should set paddings via the set_paddings()
  //! method.
  zpad_module(const char *name = "zpad");
  //! Constructs a zpad that adds same size borders on each side.
  //! \param nrows The number of rows added on each side.
  //! \param ncolumns The number of cols added on each side.
  //! the output size is enlarged by 2*nrow in rows and 2*ncols in cols
  //! for each feature map.
  zpad_module(int nrows, int ncolumns, const char *name = "zpad");
  //! Constructs a zpad module that adds padding on each side of a 2D input.
  //! (the 1st (features) dimension is left unchanged).
  //! \param top The number of rows added on to the top side.
  //! \param left The number of rows added on to the left side.
  //! \param bottom The number of rows added on to the bottom side.
  //! \param right The number of rows added on to the right side.
  zpad_module(int top, int left, int bottom, int right);
  //! Constructor adding zero borders with same size on each size if the
  //! kernel had odd size, otherwise adding 1 pixel less on the right
  //! and bottom borders.
  //! \param kernel_size The sizes of the kernel for which to pad.
  zpad_module(idxdim &kernel_size, const char *name = "zpad");
  //! Constructor adding zero borders with same size on each size if the
  //! kernel had odd size, otherwise adding 1 pixel less on the right
  //! and bottom borders.
  //! \param kernels A kernel for each input in case of multi-state input.
  zpad_module(midxdim &kernels, const char *name = "zpad");
  //! destructor
  virtual ~zpad_module();

  //! forward propagation from in to out
  void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);

  //! Resizes 'out' to the same dimensions as 'in'. If dimensions already
  //! match, nothing is changed. If orders differ, then out is assigned a new
  //! 'Tout' buffer if the correct order and dimensions.
  //! \param d If not null, use these dimensions as target, otherwise use in.x
  //! This returns true if output was resized/reallocated, false otherwise.
  virtual bool resize_output(idx<T> &in, idx<T> &out, idxdim *d = NULL);

  //! Return all paddings in an idxdim: top,left,bottom,right.
  virtual idxdim get_paddings();
  //! Returns all paddings associated with a 'kernel': top,left,bottom,right.
  //! This does not set any internal parameters of this module.
  virtual idxdim get_paddings(idxdim &kernel);
  //! Returns all paddings associated with a 'kernel': top,left,bottom,right.
  //! This does not set any internal parameters of this module.
  virtual midxdim get_paddings(midxdim &kernels);
  //! Set all paddings for top, left, bottom and right sides.
  virtual void set_paddings(int top, int left, int bottom, int right);
  //! Set all paddings from 'pads' in this order: top,left,bottom,right.
  virtual void set_paddings(idxdim &pads);
  //! Set all paddings according to 'kernel'.
  virtual void set_kernel(idxdim &kernel);
  //! Set all paddings according to 'kernels' for multi-state inputs.
  virtual void set_kernels(midxdim &kernels);
  //! Return dimensions that are compatible with this module.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim fprop1_size(fidxdim &i_size);
  //! Return dimensions compatible with this module given output dimensions.
  //! See module_1_1_gen's documentation for more details.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Returns multiple input dimensions corresponding to output dims 'osize'.
  virtual mfidxdim fprop_size(mfidxdim &isize);
  //! Returns multiple input dimensions corresponding to output dims 'osize'.
  virtual mfidxdim bprop_size(mfidxdim &osize);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns a deep copy of this module.
  //! \param p If NULL, reuse current parameter space, otherwise allocate new
  //!   weights on parameter 'p'.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);

 protected:
  idxdim pad; //!< Current padding (top,left,bottom,right).
  midxdim pads; //!< Paddings (top,left,bottom,right) for all scales.
};

// mirrorpad_module ////////////////////////////////////////////////////////////

//! A simple mirror padding module that is mostly usefull for doing
//! same size output convolutions.
template <typename T> class mirrorpad_module : public zpad_module<T> {
 public:
  //! <nrow> is the number of rows in zero added border
  //! <ncol> is the number of cols in zero added border
  //! the output size is enlarged by 2*nrow in rows and 2*ncols in cols
  //! for each feature map.
  mirrorpad_module(int nr, int nc, const char *name = "mirrorpad");
  //! Constructor adding zero borders with same size on each size if the
  //! kernel had odd size, otherwise adding 1 pixel less on the right
  //! and bottom borders.
  //! \param kernel_size The sizes of the kernel for which to pad.
  mirrorpad_module(idxdim &kernel_size, const char *name = "mirrorpad");
  //! destructor
  virtual ~mirrorpad_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, reuse current parameter space, otherwise allocate new
  //!   weights on parameter 'p'.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
 protected:
  using zpad_module<T>::pad;
};

// fsum_module /////////////////////////////////////////////////////////////////

//! This modules iterates of the last two dimenions and takes
//! the sum of the remaining dimensions.
template <typename T> class fsum_module : public module_1_1<T> {
 public:
  //! constructor.
  //! \param div If true, divide the sum by the number of elements used.
  //! \param split If non-1, sum every consecutive groups of size
  //!   (number of features) * split.
  fsum_module(bool div = false, float split = 1.0);
  //! destructor
  virtual ~fsum_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
 protected:
  bool div; //!< Normalize by number of elements used for sum.
  float split; //!< Sum by groups of n elements, n = features * split.
};

// range_lut_module ////////////////////////////////////////////////////////////

//! This modules transforms its inputs to discrete values corresponding to a
//! range of values, as described a given lookup table (lut).
template <typename T> class range_lut_module : public module_1_1<T> {
 public:
  //! constructor.
  //! @param value_range A matrix containing a series of value / range pairs.
  //!        The matrix size is Nx2, with (n, 0) being the value, (n, 1)
  //!        the maximum of the range. E.g. any value below (0, 1), will be
  //!        assigned (0, 0), then anything below (1, 1), will be assigned
  //!        (1, 0), etc.
  //!        It is assumed that ranges are non overlapping and given in
  //!        increasing order.
  range_lut_module(idx<T> *value_range);
  //! destructor
  virtual ~range_lut_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);

 protected:
  idx<T>	value_range;
};

// binarize_module /////////////////////////////////////////////////////////////

//! This modules transforms its inputs to binary outputs based on a given
//! threshold.
template <typename T> class binarize_module : public module_1_1<T> {
 public:
  //! constructor.
  binarize_module(T threshold, T false_value, T true_value);
  //! destructor
  virtual ~binarize_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);

 protected:
  T	threshold;
  T	false_value;
  T	true_value;
};

// diag_module /////////////////////////////////////////////////////////////////

//! This module applies a gain per unit (like a diagonal linear module).
template <typename T> class diag_module : public module_1_1<T> {
 public:
  //! Constructor.
  //! \param p is used to store all parametric variables in a single place.
  //!        If p is null, a local buffer will be used.
  //! \param thickness The number of feature maps.
  diag_module(parameter<T> *p, intg thickness,
              const char *name = "diag");
  //! Destructor.
  virtual ~diag_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
  //! Calls fprop and then dumps internal buffers, inputs and outputs
  //! into files. This can be useful for debugging.
  virtual void fprop1_dump(idx<T> &in, idx<T> &out);
  //! resize the output based on input dimensions
  //! This returns true if output was resized/reallocated, false otherwise.
  virtual bool resize_output(idx<T> &in, idx<T> &out, idxdim *ignore = NULL);
  //! Copy passed weights into x component of internal weights.
  virtual void load_x(idx<T> &weights);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns a deep copy of this module.
  //! \param p If NULL, reuse current parameter space, otherwise allocate new
  //!   weights on parameter 'p'.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
 protected:
  state<T>	coeff;
};

// copy_module /////////////////////////////////////////////////////////////////

//! This module does nothing but copying the input into the output.
//! This can be useful for parallel branching.
template <typename T> class copy_module : public module_1_1<T> {
 public:
  //! Constructor.
  copy_module(const char *name = "copy");
  //! Destructor.
  virtual ~copy_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
};

// back_module /////////////////////////////////////////////////////////////////

template <typename T> class back_module : public module_1_1<T> {
 public:
  //! Constructor.
  back_module(const char *name = "back");
  //! Destructor.
  virtual ~back_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! resize the output based on input dimensions
  //! This returns true if output was resized/reallocated, false otherwise.
  virtual bool resize_output(idx<T> &in, idx<T> &out, idxdim *ignore = NULL);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Return dimensions compatible with this module given output dimensions.
  //! See module_1_1_gen's documentation for more details.
  //! This module doesn't actually change the size, but we use it to know
  //! the corresponding size of 1 output pixel at this point.
  virtual fidxdim bprop1_size(const fidxdim &o_size);
  //! Apply boxes.
  void bb(std::vector<bbox*> &boxes);

 protected:
  idx<T> *s0;
  idx<T> *s1;
  idx<T> *s2;
  idxdim  pixel_size;
};

// printer_module //////////////////////////////////////////////////////////////

//! It prints input and output states to standard output.
//! useful for debugging
template <typename T> class printer_module : module_1_1<T> {
 public:
  //! Constructor.
  printer_module(const char *name = "printer");
  //! Destructor.
  virtual ~printer_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
};

// dropout_module //////////////////////////////////////////////////////////////

//! This randomly drops input to force input decorrelation and noise robustness.
template <typename T> class dropout_module : module_1_1<T> {
 public:
  //! Constructor.
  //! \param drop_proba Probability [0, 1) that an input is dropped.
  //! \param test_time If true, do not drop but instead divide outputs by
  //!   (1-drop_proba) because during training less outputs were passed.
  dropout_module(double drop_proba, bool test_time,
                 const char *name = "dropout");
  //! Destructor.
  virtual ~dropout_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! backward propagation from out to in
  virtual void bprop1(state<T> &in, state<T> &out);
  //! second-derivative backward propagation from out to in
  virtual void bbprop1(state<T> &in, state<T> &out);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

 protected:
  idx<ubyte>		keep;						//!< Binary map of kept inputs.
  double				drop_proba; 		//!< Probability of dropping input.
  bool          test_time;
};

} // namespace ebl {

#include "ebl_basic.hpp"

#endif /* EBL_BASIC_H_ */
