/***************************************************************************
 *   Copyright (C) 2012 by Soumith Chintala and Pierre Sermanet *
 *   soumith@gmail.com, pierre.sermanet@gmail.com *
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

#ifndef EBL_CUDANORMALIZATION_HPP_
#define EBL_CUDANORMALIZATION_HPP_

#ifdef __CUDA__

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // cuda_divisive_norm_module

  template <typename T, class Tstate>
  cuda_divisive_norm_module<T,Tstate>::
  cuda_divisive_norm_module(idxdim &kerdim_, int nf, bool mirror_, bool threshold_,
			    parameter<T,Tstate> *param_, const char *name_, bool af,
			    double cgauss_, bool fsum_div_, float fsum_split_,
			    double epsilon_, double epsilon2_, int gpu_id_)
    : divisive_norm_module<T,Tstate>(name_), gpu_id(gpu_id_) {
    // common initializations
    init(kerdim_, nf, mirror_, threshold_, param_, af, cgauss_, fsum_div_,
	 fsum_split_, epsilon_, epsilon2_);
    // local initializations
    if (this->mirror) eblerror("Mirror padding not implemented for CUDA");
    divconv = new cuda_convolution_module<T,Tstate>
      (param_, kerdim_, stride, conv_table, name_, true, gpu_id);
    set_kernel(gaussian_kernel);
    convvar.add_module(divconv);
    //! feature sum module to sum along features 
    //! this might be implemented by making the table in above conv module
    //! all to all connection, but that would be very inefficient
    if (across_features)
      convvar.add_module(new cuda_fsum_module<T,Tstate>
			 (fsum_div, fsum_split, gpu_id_));
    // allocate power modules
    sqrtmod = new cuda_power_module<T,Tstate>((T) .5, gpu_id_);
    sqmod = new cuda_power_module<T,Tstate>((T) 2, gpu_id_);
  }

  template <typename T, class Tstate>
  cuda_divisive_norm_module<T,Tstate>::~cuda_divisive_norm_module() {
  }

  template <typename T, class Tstate>
  void cuda_divisive_norm_module<T,Tstate>::invert(Tstate &in, Tstate &thstd_, 
						   Tstate &out) {
    //! out = in / thstd
    this->resize_output(in, out);
    cuda_div(in.x, thstd_.x, out.x, gpu_id);
  }

  template <typename T, class Tstate>
  cuda_divisive_norm_module<T,Tstate>* cuda_divisive_norm_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    cuda_divisive_norm_module<T,Tstate> *d =
      new cuda_divisive_norm_module<T,Tstate>
      (kerdim, nfeatures, mirror, threshold, p, this->name(), across_features, 
       cgauss, fsum_div, fsum_split, epsilon, epsilon2, gpu_id);
    if (!p) // assign same parameter state if no parameters were specified      
      d->divconv->kernel = divconv->kernel;
    return d;
  }
  
  template <typename T, class Tstate>
  std::string cuda_divisive_norm_module<T, Tstate>::describe() {
    std::string desc;
    desc << "cuda_" << divisive_norm_module<T, Tstate>::describe();
    return desc;
  }

  ////////////////////////////////////////////////////////////////
  // cuda_subtractive_norm_module

  template <typename T, class Tstate>
  cuda_subtractive_norm_module<T,Tstate>::
  cuda_subtractive_norm_module(idxdim &kerdim_, int nf, bool mirror_,
			       bool global_norm_, parameter<T,Tstate> *param_,
			       const char *name_, bool af, double cgauss_,
			       bool fsum_div_, float fsum_split_, int gpu_id_,
			       bool valid)
    : subtractive_norm_module<T,Tstate>(name_), gpu_id(gpu_id_) {
    this->valid = valid;
    // common initializations
    init(kerdim_, nf, mirror_, global_norm_, param_, af, cgauss_, fsum_div_,
	 fsum_split_, valid);
    meanconv = new cuda_convolution_module<T,Tstate>
      (param, kerdim, stride, conv_table, name_, true, gpu_id);
    set_kernel(gaussian_kernel);
    convmean.add_module(meanconv);
    //! feature sum module to sum along features 
    //! this might be implemented by making the table in above conv module
    //! all to all connection, but that would be very inefficient
    if (across_features)
      convmean.add_module(new cuda_fsum_module<T,Tstate>
			  (fsum_div, fsum_split, gpu_id_));
  }

  template <typename T, class Tstate>
  cuda_subtractive_norm_module<T,Tstate>::~cuda_subtractive_norm_module() {
  }

  template <typename T, class Tstate>
  cuda_subtractive_norm_module<T,Tstate>* cuda_subtractive_norm_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    cuda_subtractive_norm_module<T,Tstate> *d = new cuda_subtractive_norm_module<T,Tstate>
      (kerdim, nfeatures, mirror, global_norm, p, this->name(),
       across_features, cgauss, fsum_div, fsum_split, gpu_id, this->valid);
    if (!p) // assign same parameter state if no parameters were specified
      d->meanconv->kernel = meanconv->kernel;
    return d;
  }
  
  template <typename T, class Tstate>
  std::string cuda_subtractive_norm_module<T, Tstate>::describe() {
    std::string desc;
    desc << "cuda_" << subtractive_norm_module<T,Tstate>::describe();
    return desc;
  }

  //////////////////////////////////////////////////////////////////////////////
  // cuda_contrast_norm_module

  template <typename T, class Tstate>
  cuda_contrast_norm_module<T,Tstate>::
  cuda_contrast_norm_module(idxdim &kerdim, int nf, bool mirror, bool threshold,
			    bool gnorm_, parameter<T,Tstate> *param,
			    const char *name_, bool af, bool lm, double cgauss,
                            bool fsum_div, float fsum_split, double epsilon, 
			    double epsilon2, int gpu_id_, bool valid)
    : contrast_norm_module<T,Tstate>(name_), gpu_id(gpu_id_) {
    // setting names
    string sname, dname;
    if (name_) sname << name_ << "_";
    if (name_) dname << name_ << "_";
    sname << "subnorm";
    dname << "divnorm";
    // allocating
    subnorm = new cuda_subtractive_norm_module<T,Tstate>
      (kerdim, nf, mirror, gnorm_, lm ? param : NULL, sname.c_str(), af, cgauss,
       fsum_div, fsum_split, gpu_id_, valid);
    divnorm = new cuda_divisive_norm_module<T,Tstate>
      (kerdim, nf, mirror, threshold, param, dname.c_str(), af, cgauss,
       fsum_div, fsum_split, epsilon, epsilon2, gpu_id_);
    learn_mean = lm;
    if (mirror) eblerror("Mirror padding not implemented for CUDA");
  }

  template <typename T, class Tstate>
  cuda_contrast_norm_module<T,Tstate>::~cuda_contrast_norm_module() {
  }
      
  template <typename T, class Tstate>
  cuda_contrast_norm_module<T,Tstate>* cuda_contrast_norm_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    cuda_contrast_norm_module<T,Tstate> *d = new cuda_contrast_norm_module<T,Tstate>
      (divnorm->kerdim, divnorm->nfeatures, divnorm->mirror, divnorm->threshold,
       global_norm, p, this->name(), divnorm->across_features, learn_mean, 
       divnorm->cgauss, divnorm->fsum_div, divnorm->fsum_split, divnorm->epsilon,
       divnorm->epsilon2, gpu_id, subnorm->valid);
    if (!p) { // assign same parameter state if no parameters were specified
      d->divnorm->divconv->kernel = divnorm->divconv->kernel;
      d->subnorm->meanconv->kernel = subnorm->meanconv->kernel;
    }
    return d;
  }
  
  template <typename T, class Tstate>
  std::string cuda_contrast_norm_module<T, Tstate>::describe() {
    std::string desc;
    desc << "cuda_" << contrast_norm_module<T, Tstate>::describe();
    return desc;
  }

} // end ebl namespace

#endif // end __CUDA__
#endif // EBL_CUDANORMALIZATION_HPP_
