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

#ifndef EBL_CUDAOPS_HPP_
#define EBL_CUDAOPS_HPP_

#ifdef __CUDA__

namespace ebl {

  template <typename T>
  void cuda_convolution_3d(idx<T> &in, idx<T> &ker, idx<T> &out, 
                        intg stride_x, intg stride_y, int devid) {
    eblerror("cuda_convolution_3d : type not implemented."
             << "Available types are float32");
  }

  template <typename T>
  void cuda_convolution_3dmap(idx<T> &in, idx<T> &ker, idx<T> &out, 
                           intg stride_x, intg stride_y,
                              idx<intg> table, int fanin, int devid) {
    eblerror("cuda_convolution_3dmap : type not implemented. " 
             << "Available types are float32");
  }

  template <typename T>
  void cuda_tanh(idx<T> &in, idx<T> &out,  int devid) {
    eblerror("cuda_tanh : type not implemented. Available types are float32");
  }

  template <typename T>
  void cuda_power(idx<T> &in, idx<T> &out, float pow, int devid) {
    eblerror("cuda_power : type not implemented. Available types are float32");
  }

  template <typename T>
  void cuda_addc(idx<T> &in, idx<T> &bias, idx<T> &out, int devid) {
    eblerror("cuda_addc : type not implemented. Available types are float32");
  }

  template <typename T>
  void cuda_div(idx<T> &in1, idx<T> &in2, idx<T> &out, int devid) {
    eblerror("cuda_div : type not implemented. Available types are float32");
  }

  template <typename T>
  void cuda_fsum(idx<T> &in, idx<T> &out, bool div, int devid) {
    eblerror("cuda_fsum : type not implemented. Available types are float32");
  }

  template <typename T>
  void cuda_threshold(idx<T> &in, idx<T> &out, T thres, T val, int devid) {
    eblerror("cuda_threshold : type not implemented. Available types are float32");
  }


} // end ebl namespace

#endif // end __CUDA__

#endif // EBL_CUDAOPS_HPP_


