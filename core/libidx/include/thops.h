/***************************************************************************
 *   Copyright (C) 2012 by Soumith Chintala   *
 *   soumith@gmail.com                        *
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


#ifndef THOPS_H_
#define THOPS_H_

#ifdef __TH__
#include "idx.h"

namespace ebl {


  // th_add ///////////////////////////////////////////////////////////////////
  
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in the second idx
  //! Overflows are handled by saturating
  //! Returns the th return code for the wrapped function
  template <typename T> void th_add(idx<T> &in1, idx<T> &in2);

  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in the second idx.
  //! Returns the th return code for the wrapped function
  template <> void th_add(idx<float32> &in1, idx<float32> &in2);

  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in the second idx.
  //! Returns the th return code for the wrapped function
  template <> void th_add(idx<float64> &in1, idx<float64> &in2);


  
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in out.
  //! Overflows are handled by saturating
  //! Returns the th return code for the wrapped function
  template <typename T> void th_add(idx<T> &in1,
					    idx<T> &in2, idx<T> &out);

  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in out.
  //! Returns the th return code for the wrapped function
  template <> void th_add(idx<float32> &in1,
				  idx<float32> &in2, idx<float32> &out);
  //! This does component-wise addition of 2 matrices. The result is 
  //! saved in out.
  //! Returns the th return code for the wrapped function
  template <> void th_add(idx<float64> &in1,
				  idx<float64> &in2, idx<float64> &out);

  // th_copy /////////////////////////////////////////////////////////////
  //! Copy in to out.
  template <typename T> void th_copy(idx<T> &in, idx<T> &out);
  //! Copy in to out.
    template <> void th_copy(idx<float32> &in, idx<float32> &out);
  //! Copy in to out.
    template <> void th_copy(idx<float64> &in, idx<float64> &out);

  // th_convolution ///////////////////////////////////////////////////////////

  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  //! <ker> is actually in reverse order, so you might
  //! want to reverse it first.
  template <typename T>
    void th_convolution(idx<T> &in, idx<T> &ker, idx<T> &out, 
                        intg stride_x=1, intg stride_y=1);

  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  //! <ker> is actually in reverse order, so you might
  //! want to reverse it first.
  template <>
  void th_convolution(idx<float32> &in, idx<float32> &ker,
                      idx<float32> &out, intg stride_x, intg stride_y);
  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  template <>
  void th_convolution(idx<float64> &in, idx<float64> &ker,
                      idx<float64> &out, intg stride_x, intg stride_y);

  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <typename T>
  void th_convolution_3d(idx<T> &in, idx<T> &ker,
			      idx<T> &out, 
                        intg stride_x=1, intg stride_y=1);

  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_convolution_3d(idx<float32> &in, idx<float32> &ker,
			      idx<float32> &out, 
                        intg stride_x, intg stride_y);


  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_convolution_3d(idx<float64> &in, idx<float64> &ker,
			      idx<float64> &out, 
                        intg stride_x, intg stride_y);
  ///////////////////////
  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <typename T>
  void th_convolution_3dmap(idx<T> &in, idx<T> &ker,
                            idx<T> &out, idx<intg> &table,
                            intg stride_x=1, intg stride_y=1);

  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_convolution_3dmap(idx<float32> &in, idx<float32> &ker,
                            idx<float32> &out, idx<intg> &table,
                            intg stride_x, intg stride_y);


  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_convolution_3dmap(idx<float64> &in, idx<float64> &ker,
			      idx<float64> &out, idx<intg> &table,
                            intg stride_x, intg stride_y);
  //////////////////////
  //! compute a 3D bprop convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <typename T>
  void th_convolution_3dmap_bprop(idx<T> &inx, idx<T> &kerx,
                                  idx<T> &outdx, idx<T> &indx, 
                                  idx<T> &kerdx, idx<intg> &table,
                            intg stride_w, intg stride_h);

  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_convolution_3dmap_bprop(idx<float32> &inx, idx<float32> &kerx,
                                  idx<float32> &outdx, idx<float32> &indx, 
                                  idx<float32> &kerdx,idx<intg> &table,
                            intg stride_w, intg stride_h);


  //! compute a 3D convolution of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_convolution_3dmap_bprop(idx<float64> &inx, idx<float64> &kerx,
                                  idx<float64> &outdx, idx<float64> &indx, 
                                  idx<float64> &kerdx, idx<intg> &table,
                            intg stride_w, intg stride_h);

  ////////////////////////////////////////////////////////////////
  //! compute tanh
  template <typename T>
    void th_tanh(idx<T> &in, idx<T> &out);
  template <>
    void th_tanh(idx<float32> &in, idx<float32> &out);
  template <>
    void th_tanh(idx<float64> &in, idx<float64> &out);

  ////////////////////////////////////////////////////////////////
  //! compute pow
  template <typename T>
    void th_pow(idx<T> &in, idx<T> &out, T p);
  template <>
    void th_pow(idx<float32> &in, idx<float32> &out, float32 p);
  template <>
    void th_pow(idx<float64> &in, idx<float64> &out, float64 p);


  ////////////////////////////////////////////////////////////////
  //! compute a 3D maxpool of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <typename T>
  void th_maxpool_3d(idx<T> &in, intg kernel_w, intg kernel_h,
                            idx<T> &out, 
                     intg stride_x, intg stride_y, idx<T> &indices_e);

  //! compute a 3D maxpool of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_maxpool_3d(idx<float32> &in, intg kernel_w, intg kernel_h,
                            idx<float32> &out, 
                            intg stride_x, intg stride_y, idx<float32> &indices_e);


  //! compute a 3D maxpool of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_maxpool_3d(idx<float64> &in, intg kernel_w, intg kernel_h,
			      idx<float64> &out, 
                            intg stride_x, intg stride_y, idx<float64> &indices_e);
  //////////////////////
  //! compute a 3D bprop maxpool of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <typename T>
  void th_maxpool_3d_bprop(idx<T> &inx, intg kernel_w, intg kernel_h,
                                  idx<T> &outdx, idx<T> &indx, 
                            intg stride_w, intg stride_h, idx<T> &indices_e);

  //! compute a 3D maxpool of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_maxpool_3d_bprop(idx<float32> &inx, intg kernel_w, intg kernel_h,
                                  idx<float32> &outdx, idx<float32> &indx, 
                            intg stride_w, intg stride_h, idx<float32> &indices_e);


  //! compute a 3D maxpool of 3D <in> with 4D kernel <ker>
  //! and write result into 3D <out>
  template <>
  void th_maxpool_3d_bprop(idx<float64> &inx, intg kernel_w, intg kernel_h,
                                  idx<float64> &outdx, idx<float64> &indx, 
                            intg stride_w, intg stride_h, idx<float64> &indices_e);


} // end namespace ebl

#include "thops.hpp"
#endif /* __TH__ */

#endif /* THOPS_H_ */
