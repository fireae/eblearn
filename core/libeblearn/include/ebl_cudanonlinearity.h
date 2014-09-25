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

#ifndef EBL_CUDANONLINEARITY_H_
#define EBL_CUDANONLINEARITY_H_

#include "ebl_nonlinearity.h"
#include "ebl_cudaops.h"

#ifdef __CUDA__

namespace ebl {

  ////////////////////////////////////////////////////////////////
  //! a slab of cuda_tanh
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_tanh_module: public tanh_module<T,Tstate> {
  public:
    //! default constructor
    cuda_tanh_module(const char *name = "tanh", int gpu_id_ = -1);
    virtual ~cuda_tanh_module();
    //! fprop from in to out
    void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module.
    virtual cuda_tanh_module<T,Tstate>* copy();
  protected:
  fstate_idx<T> temp;
    // GPU members /////////////////////////////////////////////////////////////
    int                 gpu_id; //!< Whether to use gpu or not
  };
} // namespace ebl

#include "ebl_cudanonlinearity.hpp"

#endif // __CUDA__

#endif /* EBL_CUDANONLINEARITY_H_ */
