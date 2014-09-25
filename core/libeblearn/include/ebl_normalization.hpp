/***************************************************************************
 *   Copyright (C) 2011 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

namespace ebl {

// divisive_norm_module //////////////////////////////////////////////////////

template <typename T>
divisive_norm_module<T>::divisive_norm_module(const char *name_)
    : module_1_1<T>(name_), convvar(true, name_), invmod(-1),
      thres((T) 1.0, (T) 1.0) {
}

template <typename T>
divisive_norm_module<T>::
divisive_norm_module(idxdim &kerdim_, int nf, bool mirror_, bool threshold_,
                     parameter<T> *param_, const char *name_, bool af,
                     double cgauss_, bool fsum_div_, float fsum_split_,
                     double epsilon_, double epsilon2_)
    : module_1_1<T>(name_), convvar(true, name_), invmod(-1),
      thres((T) 1.0, (T) 1.0) {
  // common initializations
  init(kerdim_, nf, mirror_, threshold_, param_, af, cgauss_, fsum_div_,
       fsum_split_, epsilon_, epsilon2_);
  // local initializations
  divconv = new convolution_module<T>(param, kerdim, stride, conv_table,
                                      name_);
  set_kernel(gaussian_kernel);
  convvar.add_module(divconv);
  // feature sum module to sum along features
  // this might be implemented by making the table in above conv module
  // all to all connection, but that would be very inefficient
  if (across_features)
    convvar.add_module(new fsum_module<T>(fsum_div, fsum_split));
  // allocate power modules
  sqrtmod = new power_module<T>((T) .5);
  sqmod = new power_module<T>((T) 2);
}

template <typename T>
void divisive_norm_module<T>::
init(idxdim &kerdim_, int nf, bool mirror_, bool threshold_,
     parameter<T> *param_, bool af, double cgauss_, bool fsum_div_,
     float fsum_split_, double epsilon_, double epsilon2_) {
  this->default_name("dnorm");
  mirror = mirror_;
  param = param_;
  threshold = threshold_;
  nfeatures = nf;
  kerdim = kerdim_;
  across_features = af;
  epsilon = epsilon_;
  epsilon2 = epsilon2_;
  cgauss = cgauss_;
  fsum_div = fsum_div_;
  fsum_split = fsum_split_;
  // create weighting kernel
  if (kerdim.order() != 2)
    eblerror("expected kernel dimensions with order 2 but found order "
             << kerdim.order() << " in " << kerdim);
  gaussian_kernel = create_gaussian_kernel<T>(kerdim, cgauss);
  stride = idxdim(1, 1);
  // convolution table
  conv_table = one2one_table(nfeatures);
  // convvar
  if (mirror) // switch between zero and mirror padding
    padding = new mirrorpad_module<T>
	((gaussian_kernel.dim(0) - 1)/2, (gaussian_kernel.dim(1) - 1)/2);
  else padding = new zpad_module<T>(gaussian_kernel.get_idxdim());
  convvar.add_module(padding);
}

template <typename T>
void divisive_norm_module<T>::set_kernel(idx<T> &w) {
  // copy kernel to convolution param
  idx_bloop1(kx, divconv->kernel, T) idx_copy(w, kx);
  // normalize the kernel
  if (across_features)
    idx_dotc(divconv->kernel, 1 / idx_sum(divconv->kernel),
             divconv->kernel);
}

template <typename T>
divisive_norm_module<T>::~divisive_norm_module() {
  if (sqrtmod) delete sqrtmod;
  if (sqmod) delete sqmod;
}

template <typename T>
void divisive_norm_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  if (epsilon2 != 0)
    idx_addc(in, (T) epsilon2, in); // avoid div by 0
  // insq = in^2 TODO: would this be faster with idx_mul(in, in, insq)?
  sqmod->fprop1(in, insq);
  // invar = sum_j (w_j in^2)
  convvar.fprop1(insq, invar);
  if (epsilon != 0) idx_addc(invar, (T) epsilon, invar); // avoid div by 0
  // if learning filters, make sure they stay positive
  //    if (param) idx_abs(divconv->kernel, divconv->kernel);
  // instd = sqrt(sum_j (w_j in^2))
  sqrtmod->fprop1(invar, instd);
  // the threshold is the average of all the standard deviations over
  // the entire input. values below it will be set to the threshold.
  if (threshold) { // don't update threshold for inputs
    // update the threshold values in thres
    T mm = (T) (idx_sum(instd) / (T) instd.nelements());
    thres.thres = mm;
    thres.val = mm;
  }
  // std(std<mean(std)) = mean(std)
  thres.fprop1(instd, thstd);
  // out = in / thstd
  this->invert(in, thstd, out);
  // remember output dimensions
  this->update_outdims(out);
}

template <typename T>
void divisive_norm_module<T>::invert(idx<T> &in, idx<T> &thstd_, idx<T> &out) {
  // invstd = 1 / thstd
  invmod.fprop1(thstd_, invstd);
  // out = in / thstd
  mcw.fprop1(in, invstd, out);
}

template <typename T>
void divisive_norm_module<T>::bprop1(state<T> &in, state<T> &out) {
  // make sure backward tensors are allocated
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  insq.resize_dx();
  invar.resize_dx();
  instd.resize_dx();
  thstd.resize_dx();
  invstd.resize_dx();
  divconv->kernel.resize_dx(); // not actually learned, TODO: reduce computation
  // clear derivatives
  insq.zero_dx();
  invar.zero_dx();
  instd.zero_dx();
  thstd.zero_dx();
  invstd.zero_dx();
  convvar.zero_dx();
  // out = in/std
  mcw.bprop1(in, invstd, out);
  // 1/std
  invmod.bprop1(thstd, invstd);
  // std(std<mean(std)) = mean(std)
  thres.bprop1(instd, thstd);
  // sqrt(sum_j (w_j in^2))
  sqrtmod->bprop1(invar, instd);
  // sum_j (w_j in^2)
  convvar.bprop1(insq, invar);
  // in^2
  sqmod->bprop1(in, insq);
}

template <typename T>
void divisive_norm_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // make sure backward tensors are allocated
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  insq.resize_ddx();
  invar.resize_ddx();
  instd.resize_ddx();
  thstd.resize_ddx();
  invstd.resize_ddx();
  divconv->kernel.resize_ddx(); // not actually learned, TODO: reduce computation
  // clear derivatives
  insq.zero_ddx();
  invar.zero_ddx();
  instd.zero_ddx();
  thstd.zero_ddx();
  invstd.zero_ddx();
  convvar.zero_ddx();
  // out = in/std
  mcw.bbprop1(in, invstd, out);
  // 1/std
  invmod.bbprop1(thstd, invstd);
  // std(std<mean(std)) = mean(std)
  thres.bbprop1(instd, thstd);
  // sqrt(sum_j (w_j in^2))
  sqrtmod->bbprop1(invar, instd);
  // sum_j (w_j in^2)
  convvar.bbprop1(insq, invar);
  // in^2
  sqmod->bbprop1(in, insq);
}

template <typename T>
void divisive_norm_module<T>::fprop1_dump(idx<T> &in, idx<T> &out) {
  fprop1(in, out);
  convvar.fprop1_dump(insq, invar);
}

template <typename T>
module_1_1<T>* divisive_norm_module<T>::copy(parameter<T> *p) {
  divisive_norm_module<T> *d = new divisive_norm_module<T>
      (kerdim, nfeatures, mirror, threshold, p, this->name(), across_features,
       cgauss, fsum_div, fsum_split, epsilon, epsilon2);
  if (!p) // assign same parameter state if no parameters were specified
    d->divconv->kernel = divconv->kernel;
  return (module_1_1<T>*) d;
}

template <typename T>
bool divisive_norm_module<T>::optimize_fprop(state<T> &in, state<T> &out) {
  // memory optimization
  // if (false) {//hi && ho) { // dual buffers are provided, use them
  eblprint( "Using dual buffer memory optimization in divisive_norm_module"
            << std::endl);
  insq = out;
  invar = in;
  instd = out;
  thstd = in;
  invstd = out;
  return false;
}

template <typename T>
std::string divisive_norm_module<T>::describe() {
  std::string desc;
  desc << "divisive_norm module " << this->name() << " with kernel "
       << kerdim << ", using " << (mirror ? "mirror" : "zero") << " padding"
       << ", " << (across_features ? "" : "not ") << "across features"
       << ", using " << (param ? "learned" : "fixed")
       << " filter " << gaussian_kernel.info();
  return desc;
}

template <typename T>
void divisive_norm_module<T>::set_epsilon(double eps) {
  epsilon = eps;
}

// subtractive_norm_module /////////////////////////////////////////////////////

template <typename T>
subtractive_norm_module<T>::subtractive_norm_module(const char *name_)
    : module_1_1<T>(name_), convmean(true, name_) {
}

template <typename T>
subtractive_norm_module<T>::
subtractive_norm_module(idxdim &kerdim_, int nf, bool mirror_,
                        bool global_norm_, parameter<T> *param_,
                        const char *name_, bool af, double cgauss_,
                        bool fsum_div_, float fsum_split_, bool valid_)
    : module_1_1<T>(name_), convmean(true, name_), valid(valid_) {
  // common initializations
  init(kerdim_, nf, mirror_, global_norm_, param_, af, cgauss_, fsum_div_,
       fsum_split_, valid);
  // local initializations
  meanconv = new convolution_module<T>(param, kerdim, stride, conv_table,name_);
  set_kernel(gaussian_kernel);
  convmean.add_module(meanconv);
  // feature sum module to sum along features
  // this might be implemented by making the table in above conv module
  // all to all connection, but that would be very inefficient
  if (across_features)
    convmean.add_module(new fsum_module<T>(fsum_div, fsum_split));
}

template <typename T>
void subtractive_norm_module<T>::
init(idxdim &kerdim_, int nf, bool mirror_, bool global_norm_,
     parameter<T> *param_, bool af, double cgauss_,
     bool fsum_div_, float fsum_split_, bool valid) {
  this->default_name("snorm");
  param = param_;
  global_norm = global_norm_;
  nfeatures = nf;
  kerdim = kerdim_;
  across_features = af;
  mirror = mirror_;
  cgauss = cgauss_;
  fsum_div = fsum_div_;
  fsum_split = fsum_split_;
  // create weighting kernel
  if (kerdim.order() != 2)
    eblerror("expected kernel dimensions with order 2 but found order "
             << kerdim.order() << " in " << kerdim);
  gaussian_kernel = create_gaussian_kernel<T>(kerdim, cgauss);
  stride = idxdim(1, 1);
  // convolution table
  conv_table = one2one_table(nfeatures);
  // convvar
  if (mirror) // switch between zero and mirror padding
    padding = new mirrorpad_module<T>
	((gaussian_kernel.dim(0) - 1)/2, (gaussian_kernel.dim(1) - 1)/2);
  else padding = new zpad_module<T>(gaussian_kernel.get_idxdim());
  if (!valid) // do not pad if valid convolution
    convmean.add_module(padding);
}

template <typename T>
void subtractive_norm_module<T>::set_kernel(idx<T> &w) {
  // copy kernel to convolution param
  idx_bloop1(kx, meanconv->kernel, T) idx_copy(w, kx);
  // normalize the kernel
  if (across_features)
    idx_dotc(meanconv->kernel, 1 / idx_sum(meanconv->kernel),
             meanconv->kernel);
}

template <typename T>
subtractive_norm_module<T>::~subtractive_norm_module() {
}

template <typename T>
void subtractive_norm_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  if (global_norm) // global normalization
    idx_std_normalize(in, in, (T*) NULL);
  // inmean = sum_j (w_j * in_j)
  state<T> cin(in);
  convmean.fprop1(cin, inmean);
  // crop in if valid convolution
  if (valid) {
    cin = cin.narrow_state(1, inmean.dim(1), (in.dim(1) - inmean.dim(1)) / 2);
    cin = cin.narrow_state(2, inmean.dim(2), (in.dim(2) - inmean.dim(2)) / 2);
  }
  // inzmean = in - inmean
  difmod.fprop1(cin, inmean, out);
  // remember output dimensions
  this->update_outdims(out);
  EDEBUG(this->name() << ": out " << out << " min " << idx_min(out)
         << " max " << idx_max(out));
}

template <typename T>
void subtractive_norm_module<T>::bprop1(state<T> &in, state<T> &out) {
  // make sure backward tensors are allocated
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  inmean.resize_dx();
  meanconv->kernel.resize_dx(); // not actually learned, TODO: reduce computation
  // clear derivatives
  inmean.zero_dx();
  convmean.zero_dx();
  // crop in if valid convolution
  state<T> cin(in.x[0], in.dx[0]);
  if (valid) {
    cin = cin.narrow_state(1, inmean.dim(1), (in.dim(1) - inmean.dim(1)) / 2);
    cin = cin.narrow_state(2, inmean.dim(2), (in.dim(2) - inmean.dim(2)) / 2);
  }
  // in - mean
  difmod.bprop1(in, inmean, out);
  // sum_j (w_j * in_j)
  convmean.bprop1(in, inmean);
}

template <typename T>
void subtractive_norm_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // make sure backward tensors are allocated
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  inmean.resize_ddx();
  meanconv->kernel.resize_ddx(); // not actually learned, TODO: reduce comp
  // clear derivatives
  inmean.zero_ddx();
  convmean.zero_ddx();
  // crop in if valid convolution
  state<T> cin(in.x[0], in.dx[0]);
  // state<T> cin(in);
  if (valid) {
    cin = cin.narrow_state(1, inmean.dim(1), (in.dim(1) - inmean.dim(1)) / 2);
    cin = cin.narrow_state(2, inmean.dim(2), (in.dim(2) - inmean.dim(2)) / 2);
  }
  // in - mean
  difmod.bbprop1(in, inmean, out);
  // sum_j (w_j * in_j)
  convmean.bbprop1(in, inmean);
}

template <typename T>
void subtractive_norm_module<T>::fprop1_dump(idx<T> &in, idx<T> &out) {
  fprop1(in, out);
  convmean.fprop1_dump(in, inmean);
}

template <typename T>
module_1_1<T>* subtractive_norm_module<T>::copy(parameter<T> *p) {
  subtractive_norm_module<T> *d = new subtractive_norm_module<T>
      (kerdim, nfeatures, mirror, global_norm, p, this->name(),
       across_features, cgauss, fsum_div, fsum_split, valid);
  if (!p) // assign same parameter state if no parameters were specified
    d->meanconv->kernel = meanconv->kernel;
  return (module_1_1<T>*) d;
}

template <typename T>
bool subtractive_norm_module<T>::optimize_fprop(state<T> &in, state<T> &out) {
  // memory optimization
  // if (false) {//hi && ho) { // dual buffers are provided, use them
  eblprint( "Using dual buffer memory optimization in subtractive_norm_module"
            << std::endl);
  inmean = out;
  return true;
}

template <typename T>
std::string subtractive_norm_module<T>::describe() {
  std::string desc;
  desc << "subtractive_norm module with " << (param ? "learned" : "fixed")
       << " mean weighting and kernel " << meanconv->kernel.info()
       << ", " << (across_features ? "" : "not ") << "across features"
       << ", " << (global_norm ? "" : "not ")
       << "using global normalization";
  if (valid) desc << ", and valid";
  else desc << ", and same";
  desc << " convolution";
  return desc;
}

// contrast_norm_module ////////////////////////////////////////////////////////

template <typename T>
contrast_norm_module<T>::contrast_norm_module(const char *name_)
    : module_1_1<T>(name_), subnorm(NULL), divnorm(NULL) {
}

template <typename T>
contrast_norm_module<T>::
contrast_norm_module(idxdim &kerdim, int nf, bool mirror, bool threshold,
                     bool gnorm_, parameter<T> *param,
                     const char *name_, bool af, bool lm, double cgauss,
                     bool fsum_div, float fsum_split, double epsilon,
                     double epsilon2, bool valid)
    : module_1_1<T>(name_), learn_mean(lm) {
  this->default_name("cnorm");
  // setting names
  std::string sname, dname;
  if (name_) sname << name_ << "_";
  if (name_) dname << name_ << "_";
  sname << "subnorm";
  dname << "divnorm";
  // allocating
  subnorm = new subtractive_norm_module<T>
      (kerdim, nf, mirror, gnorm_, lm ? param : NULL, sname.c_str(), af, cgauss,
       fsum_div, fsum_split, valid);
  divnorm = new divisive_norm_module<T>
      (kerdim, nf, mirror, threshold, param, dname.c_str(), af, cgauss,
       fsum_div, fsum_split, epsilon, epsilon2);
}

template <typename T>
contrast_norm_module<T>::~contrast_norm_module() {
  if (divnorm) delete divnorm;
  if (subnorm) delete subnorm;
}

template <typename T>
void contrast_norm_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  // subtractive normalization
  subnorm->fprop1(in, tmp);
  // divisive normalization
  divnorm->fprop1(tmp, out);
  // remember output dimensions
  this->update_outdims(out);
}

template <typename T>
void contrast_norm_module<T>::bprop1(state<T> &in, state<T> &out) {
  // make sure backward tensors are allocated
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  tmp.resize_dx();
  // clear derivatives
  tmp.zero_dx();
  // divisive normalization
  divnorm->bprop1(tmp, out);
  // subtractive normalization
  subnorm->bprop1(in, tmp);
}

template <typename T>
void contrast_norm_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // make sure backward tensors are allocated
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  tmp.resize_ddx();
  // clear derivatives
  tmp.zero_ddx();
  // divisive normalization
  divnorm->bbprop1(tmp, out);
  // subtractive normalization
  subnorm->bbprop1(in, tmp);
}

template <typename T>
void contrast_norm_module<T>::fprop1_dump(idx<T> &in, idx<T> &out) {
  subnorm->fprop1_dump(in, tmp);
  divnorm->fprop1_dump(tmp, out);
}

template <typename T>
module_1_1<T>* contrast_norm_module<T>::copy(parameter<T> *p) {
  contrast_norm_module<T> *d = new contrast_norm_module<T>
      (divnorm->kerdim, divnorm->nfeatures, divnorm->mirror, divnorm->threshold,
       global_norm, p, this->name(), divnorm->across_features, learn_mean,
       divnorm->cgauss, divnorm->fsum_div, divnorm->fsum_split,
       divnorm->epsilon, divnorm->epsilon2, subnorm->valid);
  if (!p) { // assign same parameter state if no parameters were specified
    d->divnorm->divconv->kernel = divnorm->divconv->kernel;
    d->subnorm->meanconv->kernel = subnorm->meanconv->kernel;
  }
  return (module_1_1<T>*) d;
}

template <typename T>
bool contrast_norm_module<T>::optimize_fprop(state<T> &in, state<T> &out) {
  // memory optimization
  // if (false) {//hi && ho) { // dual buffers are provided, use them
  eblprint( "Using dual buffer memory optimization in contrast_norm_module"
            << std::endl);
  return true;
}

template <typename T>
std::string contrast_norm_module<T>::describe() {
  std::string desc;
  desc << "contrast_norm module with " << subnorm->describe()
       << " and " << divnorm->describe();
  return desc;
}

template <typename T>
void contrast_norm_module<T>::set_epsilon(double eps) {
  divnorm->set_epsilon(eps);
}

// laplacian_module ////////////////////////////////////////////////////////////

template <typename T>
laplacian_module<T>::
laplacian_module(int nf, bool mirror_, bool global_norm_, const char *name_)
    : module_1_1<T>(name_), mirror(mirror_),
      global_norm(global_norm_), nfeatures(nf) {
  // create filter
  filter = create_burt_adelson_kernel<T>();
  idxdim kerdim = filter.get_idxdim();
  idxdim stride(1,1);
  // prepare convolution
  idx<intg> table = one2one_table(nfeatures);
  conv =
      new convolution_module<T>(&param, kerdim, stride, table);
  idx_bloop1(kx, conv->kernel, T)
      idx_copy(filter, kx);
  // create modules
  if (mirror) // switch between zero and mirror padding
    pad = new mirrorpad_module<T>((kerdim.dim(0) - 1)/2, (kerdim.dim(1) - 1)/2);
  else
    pad = new zpad_module<T>(filter.get_idxdim());
}

template <typename T>
laplacian_module<T>::~laplacian_module() {
  delete conv;
  delete pad;
}

template <typename T>
void laplacian_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  if (global_norm) // global normalization
    idx_std_normalize(in, in, (T*) NULL);
  if (in.order() != padded.order()
      || in.order() != filtered.order()) {
    idxdim d(in.get_idxdim());
    d.setdims(1);
    padded = state<T>(d);
    filtered = state<T>(d);
  }
  pad->fprop1(in, padded);
  conv->fprop1(padded, filtered);
  diff.fprop1(in, filtered, out);
}

template <typename T>
module_1_1<T>* laplacian_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*)
      new laplacian_module<T>(nfeatures, mirror, global_norm, this->name());
}

template <typename T>
std::string laplacian_module<T>::describe() {
  std::string desc;
  desc << "laplacian module " << this->name()
       << ", using " << (mirror ? "mirror" : "zero") << " padding"
       << ", " << (global_norm ? "" : "not ")
       << "using global normalization, using filter " << filter;
  // << ": " << filter.str();
  return desc;
}

} // end namespace ebl
