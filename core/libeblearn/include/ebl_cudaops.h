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

#ifndef EBL_CUDAOPS_H_
#define EBL_CUDAOPS_H_

#include "ebl_defines.h"
#include "libidx.h"

#ifdef __CUDA__

namespace ebl {

  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <typename T>
  void cuda_convolution_3d(idx<T> &in, idx<T> &ker,
			      idx<T> &out, 
                        intg stride_x, intg stride_y, int devid);

  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
    EXPORT void cuda_convolution_3d(idx<float32> &in, idx<float32> &ker,
        		      idx<float32> &out,
                        intg stride_x, intg stride_y, int devid);

  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>, but the connections can be sparse
  template <typename T>
  void cuda_convolution_3dmap(idx<T> &in, idx<T> &ker,
			      idx<T> &out, 
                           intg stride_x, intg stride_y, 
                           idx<intg> table, int fanin, int devid);

  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>, but the connections can be sparse
  template <>
    EXPORT void cuda_convolution_3dmap(idx<float32> &in, idx<float32> &ker,
        		      idx<float32> &out,
                                  intg stride_x, intg stride_y,
                                  idx<intg> table, int fanin, int devid);

  //! Applies a tanh function to the input idx and copies it to the output idx
  template <typename T>
  void cuda_tanh(idx<T> &in, idx<T> &out,  int devid);

  //! Applies a tanh function to the input idx and copies it to the output idx
  template <>
    EXPORT void cuda_tanh(idx<float32> &in, idx<float32> &out,  int devid);

  //! Applies a power function to the input idx and copies it to the output idx
  template <typename T>
    void cuda_power(idx<T> &in, idx<T> &out,  
                           float pow, int devid);
  template <>
    EXPORT void cuda_power(idx<float32> &in, idx<float32> &out,  
                           float pow, int devid);

  //! Applies addc function to the input idx and copies it to the output idx
  template <typename T>
    void cuda_addc(idx<T> &in, idx<T> &bias, idx<T> &out, int devid);
  template <>
    EXPORT void cuda_addc(idx<float32> &in, idx<float32> &bias, 
                                idx<float32> &out, int devid);

  //! Applies element-wise divide function to the input idxes and copies 
  //  it to the output idx
  template <typename T>
    void cuda_div(idx<T> &in1, idx<T> &in2, idx<T> &out, int devid);
  template <>
    EXPORT void cuda_div(idx<float32> &in1, idx<float32> &in2, 
                                idx<float32> &out, int devid);

  //! Applies fsum function to the input idx and copies 
  //  it to the output idx
  template <typename T>
    void cuda_fsum(idx<T> &in, idx<T> &out, bool div, int devid);
  template <>
    EXPORT void cuda_fsum(idx<float32> &in, 
                          idx<float32> &out, bool div, int devid);

  //! Applies threshold function to the input idx and copies 
  //  it to the output idx
  template <typename T>
    void cuda_threshold(idx<T> &in, idx<T> &out, T thres, T val, int devid);
  template <>
    EXPORT void cuda_threshold(idx<float32> &in, 
                               idx<float32> &out, float32 thres, float32 val,
                               int devid);

} // namespace ebl

#include "ebl_cudaops.hpp"

#endif // __CUDA__

#endif /* EBL_CUDAOPS_H_ */
