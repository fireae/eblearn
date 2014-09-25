/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

// full_layer //////////////////////////////////////////////////////////////////

template <typename T>
full_layer<T>::full_layer(parameter<T> *p, intg indim0, intg noutputs,
                          bool btanh_, const char *name_)
    : module_1_1<T>(name_), btanh(btanh_), linear(p, indim0, noutputs, name_),
      adder(p, noutputs, name_),
      sigmoid(btanh_ ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
  // the order of sum is not yet known and this is just an internal buffer
  // that does not need to be save in the parameter, so we allocate it later
  sum = NULL;
  this->_name = name_;
}

template <typename T>
full_layer<T>::~full_layer() {
  if (sum) delete sum;
  if (sigmoid) delete sigmoid;
}

template <typename T>
void full_layer<T>::fprop1(idx<T> &in, idx<T> &out) {
  // resize output and sum
  idxdim d(in); // use same dimensions as in
  d.setdim(0, adder.bias.dim(0)); // except for the first one
  if (!sum) sum = new state<T>(d); // we now know the order of sum

  // fprop
  linear.fprop1(in, *sum);
  adder.fprop1(*sum, *sum);
  sigmoid->fprop1(*sum, out);
}

template <typename T>
void full_layer<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  sum->resize_dx(); // make sure backward tensors are allocated
  // backprpo
  idx_clear(sum->dx[0]);
  sigmoid->bprop1(*sum, out);
  adder.bprop1(*sum, *sum);
  linear.bprop1(in, *sum);
}

template <typename T>
void full_layer<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  sum->resize_ddx(); // make sure backward tensors are allocated
  // backprop
  idx_clear(sum->ddx[0]);
  sigmoid->bbprop1(*sum, out);
  adder.bbprop1(*sum, *sum);
  linear.bbprop1(in, *sum);
}

template <typename T>
void full_layer<T>::forget(forget_param_linear &fp) {
  linear.forget(fp);
  adder.forget(fp);
}

template <typename T>
fidxdim full_layer<T>::fprop1_size(fidxdim &isize) {
  return linear.fprop1_size(isize);
}

template <typename T>
fidxdim full_layer<T>::bprop1_size(const fidxdim &osize) {
  return linear.bprop1_size(osize);
}

template <typename T>
module_1_1<T>* full_layer<T>::copy(parameter<T> *p) {
  // allocate
  full_layer<T>* l2 = new full_layer<T>(NULL, linear.w.dim(1), linear.w.dim(0),
                                        btanh);
  // copy data
  idx_copy(linear.w, l2->linear.w);
  idx_copy(adder.bias, l2->adder.bias);
  return (module_1_1<T>*) l2;
}

template <typename T>
std::string full_layer<T>::describe() {
  std::string s;
  s << "fully connected layer " << this->name() << " composed of a linear "
    << "module: " << linear.describe() << ", a bias module: "
    << adder.describe();
  if (sigmoid) s << ", and a sigmoid module: " << sigmoid->describe();
  return s;
}

// convolution_layer ///////////////////////////////////////////////////////////

template <typename T>
convolution_layer<T>::
convolution_layer(parameter<T> *p, idxdim &ker, idxdim &stride,
                  idx<intg> &tbl, bool btanh_, const char *name_)
    : module_1_1<T>(name_), btanh(btanh_),
      convol(p, ker, stride, tbl, name_),
      adder(p, convol.thickness, name_),
      sigmoid(btanh_ ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
  sum = NULL;
  this->_name = name_;
}

template <typename T>
convolution_layer<T>::~convolution_layer() {
  if (sum) delete sum;
  if (sigmoid) delete sigmoid;
}

template <typename T>
void convolution_layer<T>::fprop1(idx<T> &in, idx<T> &out) {
  // 1. allocate sum
  idxdim d(in); // use same dimensions as in
  if (!sum) sum = new state<T>(d);

  // 2. fprop
  convol.fprop1(in, *sum);
  adder.fprop1(*sum, *sum);
  sigmoid->fprop1(*sum, out);
}

template <typename T>
void convolution_layer<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  sum->resize_dx(); // make sure backward tensors are allocated
  // backprop
  idx_clear(sum->dx[0]);
  sigmoid->bprop1(*sum, out);
  adder.bprop1(*sum, *sum);
  convol.bprop1(in, *sum);
}

template <typename T>
void convolution_layer<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  sum->resize_ddx(); // make sure backward tensors are allocated
  // backprop
  idx_clear(sum->ddx[0]);
  sigmoid->bbprop1(*sum, out);
  adder.bbprop1(*sum, *sum);
  convol.bbprop1(in, *sum);
}

template <typename T>
void convolution_layer<T>::forget(forget_param_linear &fp) {
  convol.forget(fp);
  adder.forget(fp);
}

template <typename T>
fidxdim convolution_layer<T>::fprop1_size(fidxdim &isize) {
  return convol.fprop1_size(isize);
}

template <typename T>
fidxdim convolution_layer<T>::bprop1_size(const fidxdim &osize) {
  return convol.bprop1_size(osize);
}

template <typename T>
module_1_1<T>* convolution_layer<T>::copy(parameter<T> *p) {
  // allocate
  convolution_layer<T> *l2 = new convolution_layer<T>
      (NULL, convol.ker, convol.stride, convol.table, btanh);
  // copy data
  idx_copy(convol.kernel, l2->convol.kernel);
  idx_copy(adder.bias, l2->adder.bias);
  return (module_1_1<T>*) l2;
}

// convabsnorm_layer ///////////////////////////////////////////////////////////

template <typename T>
convabsnorm_layer<T>::convabsnorm_layer(parameter<T> *p,
                                        intg kerneli, intg kernelj,
                                        intg stridei_, intg stridej_,
                                        idx<intg> &tbl, bool mirror, bool btanh_,
                                        const char *name_)
    : module_1_1<T>(name_), btanh(btanh_),
      lconv(p, kerneli, kernelj, stridei_, stridej_, tbl, btanh_, name_),
      abs(), norm(kerneli, kernelj, lconv.convol.thickness, mirror),
      tmp(NULL), tmp2(NULL) {
  this->_name = name_;
}

template <typename T>
convabsnorm_layer<T>::~convabsnorm_layer() {
  if (tmp) delete tmp;
  if (tmp2) delete tmp2;
}

template <typename T>
void convabsnorm_layer<T>::fprop1(idx<T> &in, idx<T> &out) {
  // 1. resize tmp
  idxdim d(in); // use same dimensions as in
  d.setdim(0, lconv.convol.thickness); // except for the first one
  if (!tmp) tmp = new state<T>(d);
  if (!tmp2) tmp2 = new state<T>(d);

  // 2. fprop
  // tmp->clear();
  // tmp2->clear();
  lconv.fprop1(in, *tmp);
  abs.fprop1(*tmp, *tmp2);
  norm.fprop1(*tmp2, out);
}

template <typename T>
void convabsnorm_layer<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  tmp->resize_dx(); // make sure backward tensors are allocated
  tmp2->resize_dx(); // make sure backward tensors are allocated
  // backprop
  idx_clear(tmp->dx[0]);
  idx_clear(tmp2->dx[0]);
  norm.bprop1(*tmp2, out);
  abs.bprop1(*tmp, *tmp2);
  lconv.bprop1(in, *tmp);
}

template <typename T>
void convabsnorm_layer<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  tmp->resize_ddx(); // make sure backward tensors are allocated
  tmp2->resize_ddx(); // make sure backward tensors are allocated
  // backprop
  idx_clear(tmp->ddx[0]);
  idx_clear(tmp2->ddx[0]);
  norm.bbprop1(*tmp2, out);
  abs.bbprop1(*tmp, *tmp2);
  lconv.bbprop1(in, *tmp);
}

template <typename T>
void convabsnorm_layer<T>::forget(forget_param_linear &fp) {
  lconv.forget(fp);
}

template <typename T>
fidxdim convabsnorm_layer<T>::fprop1_size(fidxdim &isize) {
  return lconv.fprop1_size(isize);
}

template <typename T>
fidxdim convabsnorm_layer<T>::bprop1_size(const fidxdim &osize) {
  return lconv.bprop1_size(osize);
}

template <typename T>
module_1_1<T>* convabsnorm_layer<T>::copy(parameter<T> *p) {
  // allocate
  convabsnorm_layer<T> *l2 = new convabsnorm_layer<T>
      (NULL, lconv.convol.ker, lconv.convol.stride, lconv.convol.table,
       norm.mirror, btanh);
  // copy data
  idx_copy(lconv.convol.kernel, l2->lconv.convol.kernel);
  idx_copy(lconv.adder.bias, l2->lconv.adder.bias);
  return (module_1_1<T>*) l2;
}

// subsampling_layer ///////////////////////////////////////////////////////////

template <typename T>
subsampling_layer<T>::
subsampling_layer(parameter<T> *p, uint thickness, idxdim &kernel,
                  idxdim &stride, bool btanh_, const char *name_)
    : module_1_1<T>(name_), btanh(btanh_),
      subsampler(p, thickness, kernel, stride, name_),
      adder(p, thickness, name_),
      sigmoid(btanh_ ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
  sum = NULL;
  this->_name = name_;
}

template <typename T>
subsampling_layer<T>::~subsampling_layer() {
  if (sum) delete sum;
  if (sigmoid) delete sigmoid;
}

template <typename T>
void subsampling_layer<T>::fprop1(idx<T> &in, idx<T> &out) {
  // 1. resize sum
  idxdim d(in); // use same dimensions as in
  d.setdim(0, subsampler.thickness); // except for the first one
  if (!sum) sum = new state<T>(d);

  // 2. fprop
  subsampler.fprop1(in, *sum);
  adder.fprop1(*sum, *sum);
  sigmoid->fprop1(*sum, out);
}

template <typename T>
void subsampling_layer<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  sum->resize_dx(); // make sure backward tensors are allocated
  // backprop
  idx_clear(sum->dx[0]);
  sigmoid->bprop1(*sum, out);
  adder.bprop1(*sum, *sum);
  subsampler.bprop1(in, *sum);
}

template <typename T>
void subsampling_layer<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  sum->resize_ddx(); // make sure backward tensors are allocated
  // backprop
  idx_clear(sum->ddx[0]);
  sigmoid->bbprop1(*sum, out);
  adder.bbprop1(*sum, *sum);
  subsampler.bbprop1(in, *sum);
}

template <typename T>
void subsampling_layer<T>::forget(forget_param_linear &fp) {
  subsampler.forget(fp);
  adder.forget(fp);
}

template <typename T>
fidxdim subsampling_layer<T>::fprop1_size(fidxdim &isize) {
  return subsampler.fprop1_size(isize);
}

template <typename T>
fidxdim subsampling_layer<T>::bprop1_size(const fidxdim &osize) {
  return subsampler.bprop1_size(osize);
}

template <typename T>
module_1_1<T>* subsampling_layer<T>::copy(parameter<T> *p) {
  // allocate
  subsampling_layer<T> *l2 =
      new subsampling_layer<T>(NULL, subsampler.thickness, subsampler.kernel,
                               subsampler.stride, btanh);
  // copy data
  idx_copy(subsampler.coeff, l2->subsampler.coeff);
  idx_copy(adder.bias, l2->adder.bias);
  return (module_1_1<T>*) l2;
}

template <typename T>
std::string subsampling_layer<T>::describe() {
  std::string desc;
  desc << "subsampling layer " << this->name() << " with thickness "
       << subsampler.thickness << ", kernel "
       << subsampler.kernel << ", stride " << subsampler.stride
       << ", bias " << adder.bias.x[0]
       << " and non linearity " << sigmoid->name();
  return desc;
}

} // end namespace ebl
