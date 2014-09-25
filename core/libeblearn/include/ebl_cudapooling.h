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

#ifndef EBL_CUDAPOOLING_H_
#define EBL_CUDAPOOLING_H_

#include "ebl_pooling.h"
#include "ebl_cudaops.h"

#ifdef __CUDA__

namespace ebl {
  ////////////////////////////////////////////////////////////////
  // cuda_lppooling_module

  //! This module takes the l2 norm of neighborhoods with a stride,
  //! e.g. a 5x5 l2 pooling with a stride of 2x2.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_lppooling_module: public lppooling_module<T, Tstate> {
  public:
  //! Constructor.
  //! \param p is used to store all parametric variables in a single place.
  //!        If p is null, a local buffer will be used.
  //! \param thickness The number of features.
  //! \param kernel Size of subsampling kernel (without thickness).
  //! \param stride Stride of subsampling kernel (without thickness).
  //! \param crop If true, crop input when it does not match with the kernel.
  //!          This allows to feed any input size to this module.
  cuda_lppooling_module(uint thickness, idxdim &kernel, idxdim &stride,
                        uint lppower = 2,
                        const char *name = "lppooling",
                        bool crop = true, int gpu_id=-1);
  //! destructor
  virtual ~cuda_lppooling_module();
  //! forward propagation from in to out
  virtual void fprop(Tstate &in, Tstate &out);
  //! Returns a deep copy of this module.
  virtual cuda_lppooling_module<T,Tstate>* copy();
  // members /////////////////////////////////////////////////////////////////
  protected:
  cuda_convolution_module<T,Tstate> *gpuconv;
  cuda_power_module<T,Tstate> gpusqmod;
  cuda_power_module<T,Tstate> gpusqrtmod;
  // GPU members /////////////////////////////////////////////////////////////
  int                 gpu_id; //!< Whether to use gpu or not
  using module_1_1<T, Tstate>::gpu_support;
  // using
  using lppooling_module<T, Tstate>::kernel;
  using lppooling_module<T, Tstate>::thickness;
  using lppooling_module<T, Tstate>::stride;
  using lppooling_module<T, Tstate>::crop;
  using lppooling_module<T, Tstate>::lp_pow;
  using lppooling_module<T, Tstate>::squared;
  using lppooling_module<T, Tstate>::convolved;
  using lppooling_module<T, Tstate>::param;
  parameter<T, Tstate> param2;

  };

} // namespace ebl

#include "ebl_cudapooling.hpp"

#endif // __CUDA__

#endif /* EBL_CUDAPOOLING_H_ */
