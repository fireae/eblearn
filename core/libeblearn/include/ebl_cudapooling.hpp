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

#ifndef EBL_CUDAPOOLING_HPP_
#define EBL_CUDAPOOLING_HPP_

#ifdef __CUDA__

namespace ebl {
  
  //////////////////////////////////////////////////////////////////////////////
  // cuda_lppooling_module
  template <typename T, class Tstate>
  cuda_lppooling_module<T, Tstate>::
  cuda_lppooling_module(uint thick, idxdim &kernel_, idxdim &stride_, 
                        uint lppower_, const char *name_, bool crop_, 
                        int gpu_id_)
    :lppooling_module<T, Tstate> (thick, kernel_, stride_, lppower_, name_, crop_),
      gpusqmod((T)lppower_, gpu_id_), 
     gpusqrtmod((T)(1.0/(T)lppower_), gpu_id_),
     gpu_id(gpu_id_) {
    // prepare convolution
    idx<intg> table = one2one_table(thick);
    gpuconv = new cuda_convolution_module<T,Tstate>
      (&param2, kernel, stride, table, "lppooling_convolution", crop, gpu_id);
    // gaussian kernel
    idx<T> filter = create_gaussian_kernel<T>(kernel);
    idx_bloop1(k, gpuconv->kernel.x, T) {
      idx_copy(filter, k); }

  }

  template <typename T, class Tstate>
  cuda_lppooling_module<T,Tstate>::~cuda_lppooling_module() {
    delete gpuconv;
  }

    template <typename T, class Tstate>
  void cuda_lppooling_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    // avoid div by 0 when bproping
    idx_addc(in.x, 1e-6, in.x); // TODO: temporary
    // special case for l1pool
    if (lp_pow == 1) {
      // gaussian-weighted neighborhood of in^2
      gpuconv->fprop(in, out);
      return;
    }
    // if it is an odd lp, example l3, l5 take an abs of the input
    if ((lp_pow % 2) == 1) eblerror("Odd lpPowers not supported");
    // in^p
    gpusqmod.fprop(in, squared);
    // gaussian-weighted neighborhood of in^p
    gpuconv->fprop(squared, convolved);
    // in^1/p(gaussian-weighted neighborhood of in^p)
    gpusqrtmod.fprop(convolved, out);
    // update last output size
    this->update_outdims(out);
  }

  template <typename T, class Tstate>
  cuda_lppooling_module<T,Tstate>* cuda_lppooling_module<T,Tstate>::copy() {
    cuda_lppooling_module<T,Tstate> *l2 =
      new cuda_lppooling_module<T, Tstate>(thickness, kernel, stride, lp_pow,
                                           this->name(), crop, gpu_id);
    return l2;
  }


} // end ebl namespace

#endif // end __CUDA__

#endif // EBL_CUDAPOOLING_HPP_
