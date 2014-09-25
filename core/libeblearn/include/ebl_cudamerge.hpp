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

#ifndef EBL_CUDAMERGE_HPP_
#define EBL_CUDAMERGE_HPP_

#ifdef __CUDA__

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // cuda_linear_merge_module

  template <typename T, class Tstate>
  cuda_linear_merge_module<T, Tstate>::
  cuda_linear_merge_module(parameter<T,Tstate> *p, intg nout_,
		      midxdim &ins_, mfidxdim &strides_,
                           const char *name_, mfidxdim *scales_, int gpu_id_)
    : flat_merge_module<T,Tstate>(ins_, strides_, name_, scales_),
      dins_original(ins_), nout(nout_), gpu_id(gpu_id_) {
    // allocate convolutions for each state
    idxdim cstride(1, 1);
    for (uint i = 0; i < ins_.size(); ++i) {
      idxdim in = ins_[i];
      intg f = in.remove_dim(0);
      idx<intg> table = full_table(f, nout);
      string cname;
      cname << "linear_merge_conv" << i;
      convs.push_back(new cuda_convolution_module<T,Tstate>
		      (p, in, cstride, table, cname.c_str(), true, gpu_id_));
      buffers1.push_back(new Tstate);
      // set original window sizes
      dins2[i] = in;
      if (i == 0) din2 = in;
      // set inputs windows of 1x1 for flat_merge
      in.setdims(1);
      dins[i] = in;
      if (i == 0) din = in;
    }
  }

  template <typename T, class Tstate>
  cuda_linear_merge_module<T, Tstate>::~cuda_linear_merge_module() {
    for (uint i = 0; i < convs.size(); ++i) delete convs[i];
  }

  template <typename T, class Tstate>
  void cuda_linear_merge_module<T, Tstate>::
  fprop(mstate<Tstate> &in, Tstate &out) {
    LOCAL_TIMING_START(); // profiling
    EDEBUG(this->name() << " (linear merge): " << in << ", wins: " << dins2 
	   << ", strides: " << strides << ", scales: " << scales);
#ifdef __DEBUG__
    for (uint i = 0; i < in.size(); ++i)
      EDEBUG_MAT("input " << i << ": ", (in[i].x));
#endif

    if (in.size() != convs.size())
      eblerror("expected " << convs.size() << " inputs but got "
	       << in.size());
    // linear combinations
    for (uint i = 0; i < convs.size(); ++i)
      convs[i]->fprop(in[i], buffers1[i]);
    // combine each state
    flat_merge_module<T,Tstate>::fprop(buffers1, buffer2);
    // add states together
    idxdim d(buffer2.x);
    intg thick = d.dim(0) / dins.size();
    d.setdim(0, thick);
    this->resize_output(in[0], out, &d);
    idx_clear(out.x);
    for (uint i = 0; i < dins.size(); ++i) {
      idx<T> slice = buffer2.x.narrow(0, thick, i * thick);
      idx_add(slice, out.x);
    }
    LOCAL_TIMING_REPORT("linear merge");
  }

  template <typename T, class Tstate>
  std::string cuda_linear_merge_module<T, Tstate>::describe() {
    std::string desc;
    desc << "cuda_linear_merge module " << this->name() << ", merging "
	 << (int) dins_original.size() << " inputs: ";
    for (uint i = 0; i < dins_original.size(); ++i) {
      desc << " (in " << dins_original[i] << " stride " << strides[i];
      if (i < scales.size()) desc << " scale " << scales[i];
      desc << "), ";
    }
    return desc;
  }

  template <typename T, class Tstate>
  cuda_linear_merge_module<T,Tstate>* cuda_linear_merge_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    cuda_linear_merge_module<T,Tstate> *l2 =
      new cuda_linear_merge_module<T,Tstate>(p, nout, dins_original, strides, 
                                             this->name(), &scales, gpu_id);
    // assign same parameter state if no parameters were specified
    if (!p) {
      for (uint i = 0; i < convs.size(); ++i)
	l2->convs[i]->kernel = convs[i]->kernel;
    }
    return l2;
  }

} // end ebl namespace

#endif // end __CUDA__

#endif // EBL_CUDAMERGE_HPP_
