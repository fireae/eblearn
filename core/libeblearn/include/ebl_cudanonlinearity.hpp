/***************************************************************************
 *   Copyright (C) 2012 by Soumith Chintala *
 *   soumith@gmail.com *
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

#ifndef EBL_CUDANONLINEARITY_HPP_
#define EBL_CUDANONLINEARITY_HPP_

#ifdef __CUDA__

namespace ebl {

  template <typename T, class Tstate>
  cuda_tanh_module<T,Tstate>::cuda_tanh_module(const char *name_, int gpu_id_) 
  : tanh_module<T,Tstate>(name_), gpu_id(gpu_id_) {
    this->_name << "_cuda";
    // check precision to decide if we use CUDA or not
    fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&temp);
    if (!cont) eblerror("cuda_tanh_module needs float precision");
  }

  template <typename T, class Tstate>
  cuda_tanh_module<T,Tstate>::~cuda_tanh_module() {}

  // cuda_tanh module
  template <typename T, class Tstate>
  void cuda_tanh_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize iff necessary
    cuda_tanh(in.x, out.x, gpu_id);
  }
  template <typename T, class Tstate>
  cuda_tanh_module<T,Tstate>* cuda_tanh_module<T,Tstate>::copy() {
    return new cuda_tanh_module<T,Tstate>();
  }


} // end ebl namespace

#endif // end __CUDA__

#endif // EBL_CUDANONLINEARITY_HPP_
