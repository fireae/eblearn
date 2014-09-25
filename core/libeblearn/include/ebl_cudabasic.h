/***************************************************************************
 *   Copyright (C) 2011 by Soumith Chintala*
 *   soumith@gmail.com  *
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

#ifndef EBL_CUDABASIC_H_
#define EBL_CUDABASIC_H_

#include "ebl_basic.h"
#include "ebl_cudaops.h"

#ifdef __CUDA__

namespace ebl {

  ///////////////////////////////////////////////////////////////////////////////
  ////         CUDA Basic Modules
  //////////////////////////////////////////////////////////////////////////////

  // cuda_convolution_module
  //! CUDA version of the convolution module
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_convolution_module : public convolution_module<T, Tstate> {
  public:
    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    //!        If p is null, a local buffer will be used.
    //! \param ker The convolution kernel sizes.
    //! \param stride The convolution strides.
    //! \param table is the convolution connection table.
    //! \param crop If true, crop input when it does not match with the kernel.
    //!          This allows to feed any input size to this module.
    //! \param gpu_id Specifies which GPU to use (in case of multiple GPUs)
    cuda_convolution_module(parameter<T,Tstate> *p, idxdim &ker, idxdim &stride,
		       idx<intg> &table, const char *name = "convolution",
		       bool crop = true, int gpu_id = -1);
    //! destructor
    virtual ~cuda_convolution_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    // members /////////////////////////////////////////////////////////////////
    idx<intg>		revtable; //!< table of connections btw output and input
    int                 fanin;  //!< the fanin of the connection table
    using module_1_1<T, Tstate>::gpu_support;
    using convolution_module<T, Tstate>::fulltable;
    using convolution_module<T, Tstate>::table;
    using convolution_module<T, Tstate>::thickness;
    using convolution_module<T, Tstate>::tablemax;
    using convolution_module<T, Tstate>::kernel;
    using convolution_module<T, Tstate>::stride;
  protected:
    // GPU members /////////////////////////////////////////////////////////////
    int                 gpu_id; //!< Whether to use gpu or not
  };

  //////////////////////////////////////////////////////////////////////////////
  // cuda_power_module
  //! x^p module. p can be nay real number
  //! the derivatives are implemented using
  //! polynomial derivative rules, so they are exact
  //! The derivative implementation divides output by input
  //! to get x^(p-1), therefore this module assumes that
  //! the :input:x and :output:x is not modified until bprop
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_power_module : public power_module<T,Tstate> {
  public:
    //! <p> is double number, every element of input is raised to
    //! its <p>th power.
    cuda_power_module(T p, int gpu_id = -1,
		      const char *name = "cuda_power");
    //! destructor
    virtual ~cuda_power_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);

    // members /////////////////////////////////////////////////////////////////
  protected:
    using power_module<T, Tstate>::p;
    fstate_idx<T> temp;
  public:
    // GPU members /////////////////////////////////////////////////////////////
    int                 gpu_id; //!< Whether to use gpu or not
    using module_1_1<T, Tstate>::gpu_support;
  };

  //////////////////////////////////////////////////////////////////////////////
  // cuda_addc_module
  //! The constant add module adds biases to the first dimension of the input
  //! and puts the results in the output. This module is spatially replicable
  //! (the input can have an order greater than 1 and the operation will apply
  //! to all elements).
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_addc_module: public addc_module<T, Tstate> {
  public:
    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    //!        If p is null, a local buffer will be used.
    //! \param size is the number of biases, or the size of dimensions 0 of
    //! inputs and outputs.
  cuda_addc_module(parameter<T,Tstate> *p, intg size,
                   const char *name = "addc", int gpu_id_=-1);
    //! destructor
    virtual ~cuda_addc_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Copy passed weights into x component of internal weights.
    virtual void load_x(idx<T> &weights);
    //! Returns a deep copy of this module.
    //! \param p If NULL, reuse current parameter space, otherwise allocate new
    //!   weights on parameter 'p'.
    virtual cuda_addc_module<T,Tstate>* copy(parameter<T,Tstate> *p = NULL);
    // members ////////////////////////////////////////////////////////
  public:
  using addc_module<T, Tstate>::bias;
  // GPU members /////////////////////////////////////////////////////////////
  int                 gpu_id; //!< Whether to use gpu or not
  using module_1_1<T, Tstate>::gpu_support;
  Tstate temp;
  };

  ////////////////////////////////////////////////////////////////
  // cuda_fsum_module
  //! This modules iterates of the last two dimenions and takes
  //! the sum of the remaining dimensions.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_fsum_module : public fsum_module<T,Tstate> {
  public:
    //! constructor.
    //! \param div If true, divide the sum by the number of elements used.
    //! \param split If non-1, sum every consecutive groups of size
    //!   (number of features) * split.
  cuda_fsum_module(bool div = false, float split = 1.0, int gpu_id_ = -1);
    //! destructor
    virtual ~cuda_fsum_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
  protected:
  using fsum_module<T, Tstate>:: div; //!< Normalize by number of elements used for sum.
  //! Sum by groups of n elements, n = features * split.
  using fsum_module<T, Tstate>::split;

  // GPU members /////////////////////////////////////////////////////////////
  int                 gpu_id; //!< Whether to use gpu or not
  using module_1_1<T, Tstate>::gpu_support;
  };

  ////////////////////////////////////////////////////////////////
  // cuda_thres_module
  //! A thresholding module that filters the input and any entry that is
  //! smaller then a given threshold is set to a specified value.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_thres_module : public thres_module<T,Tstate> {
  public:
  using thres_module<T,Tstate>::thres;
  using thres_module<T,Tstate>::val;

  public:
    //! <thres> is the threshold value that is used to filter the
    //! input.
    //! <val> is the value that is used to replace any input entry.
    //! smaller than <thres>.
  cuda_thres_module(T thres, T val, int gpu_id_= -1);
    //! destructor
    virtual ~cuda_thres_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
  // GPU members /////////////////////////////////////////////////////////////
  int                 gpu_id; //!< Whether to use gpu or not
  using module_1_1<T, Tstate>::gpu_support;
  };



} // namespace ebl

#include "ebl_cudabasic.hpp"

#endif // __CUDA__

#endif /* EBL_CUDABASIC_H_ */
