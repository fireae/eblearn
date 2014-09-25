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

#ifndef EBL_CUDAMERGE_H_
#define EBL_CUDAMERGE_H_

#include "ebl_merge.h"
#include "ebl_cudaops.h"

#ifdef __CUDA__

namespace ebl {

  // cuda_linear_merge ////////////////////////////////////////////////////////////////

  //! This module is equivalent to a flat_merge_module followed by a linear_module
  //! but executes much faster.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_linear_merge_module : public flat_merge_module<T, Tstate> {
  public:
    //! \param nout Number of outputs of linear combination.
    //! \param dins The input sizes for each state, including the features
    //!   dimension.
    //! \param scales Specifies to which input scale each input corresponds to.
    //! \param features Number of features of each input state.
  cuda_linear_merge_module(parameter<T,Tstate> *p, intg nout,
			midxdim &ins, mfidxdim &strides,
			const char *name_ = "linear_merge",
			mfidxdim *scales = NULL, int gpu_id_ = -1);
    virtual ~cuda_linear_merge_module();

    // multi-states methods ////////////////////////////////////////////////////
    //! forward propagation from in to out
    virtual void fprop(mstate<Tstate> &in, Tstate &out);
    //! backward propagation from out to in

    ///////////////////////////////////////////////////////////////////////////
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns a deep copy of current module.
    //! \param p If NULL, reuse current parameter space, otherwise allocate new
    //!   weights on parameter 'p'.
    virtual cuda_linear_merge_module<T,Tstate>* copy(parameter<T,Tstate> *p = NULL);

    // class variables /////////////////////////////////////////////////////////
  private:
    using flat_merge_module<T,Tstate>::din;
    using flat_merge_module<T,Tstate>::din2;
    using flat_merge_module<T,Tstate>::dins;
    using flat_merge_module<T,Tstate>::dins2;
    using flat_merge_module<T,Tstate>::stride;
    using flat_merge_module<T,Tstate>::strides;
    using flat_merge_module<T,Tstate>::scales;
    vector<cuda_convolution_module<T,Tstate>*> convs;
    mstate<Tstate> buffers1;
    midxdim dins_original;
    intg nout;
    Tstate buffer2;
  public:
  // GPU members /////////////////////////////////////////////////////////////
  int                 gpu_id; //!< Whether to use gpu or not
  using module_1_1<T, Tstate>::gpu_support;
  };


} // namespace ebl

#include "ebl_cudamerge.hpp"

#endif // __CUDA__

#endif /* EBL_CUDAMERGE_H_ */
