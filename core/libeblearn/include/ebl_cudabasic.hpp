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

#ifndef EBL_CUDABASIC_HPP_
#define EBL_CUDABASIC_HPP_

#ifdef __CUDA__

namespace ebl {

  ///////////////////////////////////////////////////////////////////////////////
  ////         CUDA Basic Modules
  //////////////////////////////////////////////////////////////////////////////
  template <typename T, class Tstate>
  cuda_convolution_module<T, Tstate>::
  cuda_convolution_module(parameter<T,Tstate> *p, idxdim &ker_, idxdim &stride_,
                          idx<intg> &tbl, const char *name_, bool crop_, 
                          int gpu_id_)
    : convolution_module<T,Tstate>(p, ker_, stride_, tbl, name_, crop_),
      revtable(1,1,1), fanin(-1), gpu_id(gpu_id_) {
    gpu_support = true;
    this->_name << "_cuda";

    // check precision to decide if we use CUDA or not
    fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&kernel);
    if (!cont) 
      eblerror ("CUDA mode needs float precision");
      
    // check it the table has fixed fanin and construct the revtable
    if (!fulltable && (table.dim(0) % thickness == 0)) {
      fanin = table.dim(0) / thickness;
      revtable.resize(thickness, fanin, 2);
      // do a proper fanin check
      for (int i = 0; i < thickness; ++i) {
        int tempfan = fanin;
        for (int j=0; j < table.dim(0); ++j) {
          if( table.get(j,1) == i) {
            if(tempfan <= 0)
              break;
            revtable.set(table.get(j,0), i, tempfan - 1, 0);
            revtable.set(j, i, tempfan - 1, 1);
            tempfan--;
          }
        }
        if (tempfan != 0) {
          fanin = -1;
          break;
        }
      }
    }
  }

  template <typename T, class Tstate>
  cuda_convolution_module<T, Tstate>::~cuda_convolution_module() {
  }

  template <typename T, class Tstate>
  void cuda_convolution_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    if (!convolution_module<T, Tstate>::resize_output(in, out))
      return ; // do nothing if resizing failed
    EDEBUG_MAT(this->name() << ": kernel", kernel.x)
    EDEBUG_MAT(this->name() << ": table", table);
    // inx = input tensor over which the kernel has to be applied
    idx<T> inx = in.x;

    // check if all input channels are actually used
    if (in.x.dim(0) > tablemax + 1) {
      // case where input is 3-channel but table uses only 1-channel
      if (tablemax == 0) {
        inx = in.x.narrow(0,1,0);
      }
      else {
        cerr << "WARNING: CUDA WILL BE DISABLED FOR MODULE: " << this->name()
             << " because all inputs are not being used and tablemax is not 0 " 
             << " (i.e. if all inputs are not used, the only supported case"
             << "using CUDA is if tablemax is 0 " <<endl;
        convolution_module<T, Tstate>::fprop(in, out);
        return;
      }
    }

    // execute cuda kernel
    LOCAL_TIMING2_START();
    if (fulltable)
      cuda_convolution_3d(inx, kernel.x, out.x, stride.dim(0), stride.dim(1), 
                          gpu_id);
    else if (fanin != -1)
      cuda_convolution_3dmap(inx, kernel.x, out.x, stride.dim(0), 
                             stride.dim(1), revtable, fanin, gpu_id);
    else
      eblerror(" Only full tables or fixed fanin tables are supported" 
               << "in convolution_module with CUDA");
    LOCAL_TIMING2_REPORT("convgpu total execution time");
  }

  ////////////////////////////////////////////////////////////////////////
  ///// cuda_power_module
  ////////////////////////////////////////////////////////////////////////
  template <typename T, class Tstate>
  cuda_power_module<T,Tstate>::
  cuda_power_module(T p_, int gpu_id_, const char *name_)
    : power_module<T,Tstate>(p_, name_), gpu_id(gpu_id_) {
    gpu_support = true;
    
    // check precision to decide if we use CUDA or not
    fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&temp);
    if (!cont) eblerror ("CUDA mode needs float precision");
  }

  template <typename T, class Tstate>
  cuda_power_module<T,Tstate>::~cuda_power_module() {}

  template <typename T, class Tstate>
  void cuda_power_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize iff necessary
    cuda_power(in.x, out.x, p, gpu_id);
  }

  ////////////////////////////////////////////////////////////////////////
  ///// cuda_addc_module
  ////////////////////////////////////////////////////////////////////////
  template <typename T, class Tstate>
  cuda_addc_module<T,Tstate>::cuda_addc_module(parameter<T,Tstate> *p, intg size,
                                               const char *name_, int gpu_id_)
    : addc_module<T,Tstate> (p, size, name_), gpu_id(gpu_id_) {
    this->_name << "_cuda";
    // check precision to decide if we use CUDA or not
    fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&temp);
    if (!cont) 
      eblerror ("CUDA mode needs float precision");
  }

  template <typename T, class Tstate>
  cuda_addc_module<T,Tstate>::~cuda_addc_module() {}

    template <typename T, class Tstate>
    void cuda_addc_module<T,Tstate>::fprop(Tstate& in, Tstate& out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x.spec); // use same dimensions as in
      d.setdim(0, bias.x.dim(0)); // except for the first one
      this->resize_output(in, out, &d); // resize iff necessary
    }
    cuda_addc(in.x, bias.x, out.x, gpu_id);
  }
  template <typename T, class Tstate>
  void cuda_addc_module<T,Tstate>::load_x(idx<T> &weights) {
    eblerror("load_x not implemented for cuda_addc");
  }

  template <typename T, class Tstate>
  cuda_addc_module<T,Tstate>* 
  cuda_addc_module<T,Tstate>::copy(parameter<T,Tstate> *p) {
    // new module (with its own local parameter buffers)
    cuda_addc_module<T,Tstate> *l2 =
      new cuda_addc_module<T, Tstate>(p, bias.x.dim(0), this->name(), gpu_id);
    // assign same parameter state if no parameters were specified
    if (!p) l2->bias = bias;
    return l2;
  }


  // cuda_fsum_module ///////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  cuda_fsum_module<T,Tstate>::cuda_fsum_module(bool div_, float split_, int gpu_id_)
    : fsum_module<T,Tstate>(div_, split_), gpu_id(gpu_id_){
    this->_name << "_cuda";
    if (split != 1.0) eblerror("split != 1.0 not implemented in cuda_fsum");
  }

  template <typename T, class Tstate>
  cuda_fsum_module<T,Tstate>::~cuda_fsum_module() {
  }

  template <typename T, class Tstate>
  void cuda_fsum_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize iff necessary
    cuda_fsum(in.x, out.x, div, gpu_id);
    idx<T> outx0 = out.x[0];
    for (int i=1; i < out.x.dim(0); i++) {
      idx<T> outxi = out.x[i];
      idx_copy(outx0, outxi);
    }
    // idx_eloop2(inx2, in.x, T, outx2, out.x, T) {
    //   idx_eloop2(inx1, inx2, T, outx1, outx2, T) {
    //     sum = idx_sum(inx1);
    //     if (div) sum = sum / inx1.nelements();
    //     idx_fill(outx1, sum);
    //   }
    // }
  }

} // end ebl namespace

#endif // end __CUDA__
#endif // EBL_CUDABASIC_HPP_
