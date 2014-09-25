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

// stdsigmoid //////////////////////////////////////////////////////////////////

template <typename T>
stdsigmoid_module<T>::stdsigmoid_module(double alpha_, const char *name_)
    : module_1_1<T>(name_), alpha(alpha_) {
}

template <typename T>
stdsigmoid_module<T>::~stdsigmoid_module() {
}

template <typename T>
void stdsigmoid_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize iff necessary
  this->resize_output(in, tmp); // resize iff necessary
  idx_stdsigmoid(in, out);
  if (alpha != 0) idx_dotcacc(out, (T) alpha, out);
}

template <typename T>
void stdsigmoid_module<T>::bprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_dstdsigmoid(in, tmp);
  if (alpha != 0) idx_addc(tmp, (T) alpha);
  idx_mulacc(tmp, out.dx[0], in.dx[0]);
}

template <typename T>
void stdsigmoid_module<T>::bbprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_dstdsigmoid(in, tmp);
  idx_mul(tmp, tmp, tmp);
  idx_mulacc(tmp, out.ddx[0], in.ddx[0]);
}

template <typename T>
module_1_1<T>* stdsigmoid_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*) new stdsigmoid_module<T>(alpha);
}

template <typename T>
std::string stdsigmoid_module<T>::describe() {
  std::string s;
  s << "stdsigmoid module " << this->name() << " with linear coefficient "
    << alpha;
  return s;
}

// tanh ////////////////////////////////////////////////////////////////////////

template <typename T>
tanh_module<T>::tanh_module(double alpha_, const char *name_)
    : module_1_1<T>(name_), alpha(alpha_) {
}

template <typename T>
tanh_module<T>::~tanh_module() {
}

template <typename T>
void tanh_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize iff necessary
  this->resize_output(in, tmp); // resize iff necessary
  idx_tanh(in, out);
  if (alpha != 0) idx_dotcacc(out, (T) alpha, out);
}

template <typename T>
void tanh_module<T>::bprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_dtanh(in, tmp);
  if (alpha != 0) idx_addc(tmp, (T) alpha);
  idx_mulacc(tmp, out.dx[0], in.dx[0]);
}

template <typename T>
void tanh_module<T>::bbprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_dtanh(in, tmp);
  idx_mul(tmp, tmp, tmp);
  idx_mulacc(tmp, out.ddx[0], in.ddx[0]);
}

template <typename T>
void tanh_module<T>::fprop1_dump(idx<T> &in, idx<T> &out) {
  DUMP(in, this->name() << "_tanh_module_in");
  fprop1(in, out);
  DUMP(out, this->name() << "_tanh_module_out");
}

template <typename T>
module_1_1<T>* tanh_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*) new tanh_module<T>(alpha);
}

template <typename T>
std::string tanh_module<T>::describe() {
  std::string s;
  s << "tanh module " << this->name() << " with linear coefficient " << alpha;
  return s;
}

// softmax /////////////////////////////////////////////////////////////////////

template <typename T>
softmax_module<T>::softmax_module(double b) : module_1_1<T>("softmax_module"), beta(b) {
}

template <typename T>
softmax_module<T>::~softmax_module() {
}

template <typename T>
void softmax_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out);
  // shift to avoid overflow
  idx_addc(in, -idx_max(in), out);
  // coefficient
  if (beta != 1) idx_dotc(out, beta, out);
  // exp of all
  idx_exp(out);
  // normalize by sum of exp
  idx_dotc(out, 1/idx_sum(out), out);
}

template <typename T>
void softmax_module<T>::bprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  idx<T> tmp(out.dx[0].get_idxdim());
  // backprop
  double dot = idx_dot(out.dx[0], out);
  idx_addc(out.dx[0], -dot, tmp);
  idx_mul(out, tmp, tmp);
  idx_dotcacc(tmp, beta, in.dx[0]);
}

template <typename T>
void softmax_module<T>::bbprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx<T> mul(out.get_idxdim());
  idx<T> dot(out.get_idxdim());
  idx<T> pp(out.get_idxdim());
  idx<T> mul2(out.get_idxdim());
  idx<T> pp2(out.get_idxdim());
  idx<T> mul3(out.get_idxdim());
  idx_mul(out, out, mul);
  idx_dotc(out, (T)-2, dot);
  idx_addc(dot, (T)1, pp);
  idx_mul(pp, out.ddx[0], mul2);
  idx_addc(mul2, idx_dot(out.ddx[0], mul), pp2);
  idx_mul(mul, pp2, mul3);
  idx_dotcacc(mul3, beta*beta, in.ddx[0]);
}

// abs_module //////////////////////////////////////////////////////////////////

template <typename T>
abs_module<T>::abs_module(double thres) : module_1_1<T>("abs") {
  threshold = thres;
}

template <typename T>
abs_module<T>::~abs_module() {
}

template <typename T>
void abs_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize iff necessary
  idx_abs(in, out);
}

template <typename T>
void abs_module<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  idx_checknelems2_all(in.dx[0], out.dx[0]); // must have same dimensions
  // backprop
  idx_aloopf3(inx, in, T, indx, in.dx[0], T, outdx, out.dx[0], T, {
      if (*inx > threshold)
	*indx = *indx + *outdx;
      else if (*inx < -threshold)
	*indx = *indx - *outdx;
    });
}

template <typename T>
void abs_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  idx_checknelems2_all(in.ddx[0], out.ddx[0]); // must have same dimensions
  // backprop
  idx_add(in.ddx[0], out.ddx[0], in.ddx[0]);
}

template <typename T>
module_1_1<T>* abs_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*) new abs_module<T>();
}

// linear_shrink_module ////////////////////////////////////////////////////////

template <typename T>
linear_shrink_module<T>::linear_shrink_module(parameter<T> *p, intg nf, T bs)
    : module_1_1<T>("linear_shrink"), bias(nf, p), default_bias(bs) {
  idx_fill(bias, bs);
}

template <typename T>
linear_shrink_module<T>::~linear_shrink_module(){
}

template <typename T>
void linear_shrink_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  // checks
  if (&in != &out) eblerror("in and out should be different buffers");
  this->resize_output(in, out); // resize iff necessary
  // fprop
  idx_bloop3(inx, in, T, outx, out, T, biasx, bias, T) {
    T b = biasx.get();
    idx_aloopf2(i, inx, T, o, outx, T, {
	if (*i > b) *o = *i - b;
	else if (*i < -b) *o = *i + b;
	else *o = 0; });
  }
}

template <typename T>
void linear_shrink_module<T>::bprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_bloop5(inx, in, T, indx, in.dx[0], T, outdx, out.dx[0], T,
             biasx, bias, T, biasdx, bias.dx[0], T) {
    T b = biasx.get();
    idx_aloopf3(i, inx, T, id, indx, T, od, outdx, T, {
	if (*i > b) {
          *id += *od;
          biasdx.set(biasdx.get() - *od);
	} else if (*i < -b) {
          *id += *od;
          biasdx.set(biasdx.get() - *od);
	}});
  }
}

template <typename T>
void linear_shrink_module<T>::bbprop1(state<T> &in, state<T> &out){
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_bloop5(inx, in, T, inddx, in.ddx[0], T, outddx, out.ddx[0], T,
             biasx, bias, T, biasddx, bias.ddx[0], T) {
    T b = biasx.get();
    idx_aloopf3(i, inx, T, idd, inddx, T, odd, outddx, T, {
	if (*i > b) {
          *idd += *odd;
          biasddx.set(biasddx.get() - *odd);
	} else if (*i < -b) {
          *idd += *odd;
          biasddx.set(biasddx.get() - *odd);
	}});
  }
}

template <typename T>
module_1_1<T>* linear_shrink_module<T>::copy(parameter<T> *p) {
  linear_shrink_module<T>* s2 =
      new linear_shrink_module<T>(NULL, bias.dim(0), default_bias);
  // assign same parameter state
  s2->bias = bias;
  return (module_1_1<T>*) s2;
}

template <typename T>
std::string linear_shrink_module<T>::describe() {
  std::string desc;
  desc << "linear_shrink module " << this->name()
       << " with biases: " << to_string(bias.x)
       << " min: " << idx_min(bias) << " max: " << idx_max(bias);
  return desc;
}

// smooth_shrink_module ////////////////////////////////////////////////////////

template <typename T>
smooth_shrink_module<T>::smooth_shrink_module(parameter<T> *p, intg nf, T bt,
                                              T bs)
    : module_1_1<T>("smooth_shrink"), beta(nf, p), bias(nf, p),
      ebb(1), ebx(1,1,1), tin(1,1,1), absmod(0.0),
      default_beta(bt), default_bias(bs) {
  idx_fill(beta, bt);
  idx_fill(bias, bs);
}

template <typename T>
smooth_shrink_module<T>::~smooth_shrink_module(){
}

template <typename T>
void smooth_shrink_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  // checks
  if (&in != &out) { // resize only when input and output are different
    idxdim d(in.spec); // use same dimensions as in
    out.resize(d);
  } else
    eblerror("in and out should be different buffers");
  // fprop
  absmod.fprop1(in,tin);
  // failsafe
  idx_aloopf1(x, in, T, {
      if (*x > 20)
	*x = 20;
    });
  ebb.resize(bias.dim(0));
  ebx.resize(in.get_idxdim());

  idx_mul(beta, bias, ebb);
  idx_exp(ebb);

  idx_bloop5(inx, tin, T, outx, out, T, ebbx, ebb, T,
             betax, beta, T, biasx, bias, T) {
    idx_dotc(inx, betax.get(), outx);
    idx_exp(outx);
    idx_addc(outx, ebbx.get()-1, outx);
    idx_log(outx);
    idx_dotc(outx, 1/betax.get(), outx);
    idx_addc(outx, -biasx.get(), outx);
  }
  idx_aloopf2(x, in, T, y, out, T, {
      if (std::abs((int)*x) > 20)
	*y = *x;
      if(*x < 0.0) {
	*y = -(*y);
      }
    });
}

template <typename T>
void smooth_shrink_module<T>::bprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // zeroing
  tin.zero_dx();
  beta.zero_dx();
  bias.zero_dx();
  // backprop
  absmod.fprop1(in,tin);
  // failsafe
  idx_aloopf1(x, in, T, {
      if (*x > 20)
	*x = 20;
    });

  // bb = exp (beta .* bias)
  idx_mul(beta, bias, ebb);
  idx_exp(ebb);
  intg nf = bias.dim(0);

  idx<T> ttx(ebx[0].spec);
  idx<T> tty(ebx[0].spec);
  for (intg i=0; i< nf; i++) {
    // ebx = exp(beta * x)
    idx<T> ebxxi = ebx[i];
    idx<T> ebxdxi = ebx.dx[0][i];
    idx<T> ebxddxi = ebx.ddx[0][i];
    idx<T> tinxi = tin[i];
    idx<T> tindxi = tin.dx[0][i];
    idx<T> outdxi = out.dx[0][i];

    idx_dotc(tinxi,beta[i].get(),ebxxi);
    idx_exp(ebxxi);

    // ebdx = exp(beta*x) + exp(beta*bias) -1
    idx_addc(ebxxi,ebb[i].get()-1,ebxdxi);
    // ebddx = exp (beta*x)/ (exp(beta*x) + exp(beta*bias)-1)
    idx_div(ebxxi,ebxdxi,ebxddxi);

    // df/dx
    idx_mul(ebxddxi,outdxi,tindxi);

    // eblprint(tinxi.get(0,0) << tindxi.get(0,0) << endl);

    // ebddx = 1/ebdx
    idx_inv(ebxdxi,ebxddxi);

    // df/dbias
    idx_dotc(ebxddxi,ebb[i].get(),ttx);
    idx_addc(ttx,(T)-1.0,ttx);
    bias.dx[0][i].set(idx_dot(outdxi,ttx));

    // df/dbeta
    idx_mul(tinxi,ebxxi,ttx);
    idx_addc(ttx, bias[i].get() * ebb[i].get(),ttx);
    idx_mul(ttx,ebxddxi,ttx);
    idx_dotc(ttx, 1/beta[i].get(),ttx);
    idx_log(ebxdxi);
    idx_dotc(ebxdxi,-1/(beta[i].get()*beta[i].get()),tty);
    idx_add(ttx,tty,ttx);
    beta.dx[0][i].set((T)idx_dot(outdxi,ttx));
  }
  idx_add(in.dx[0],tin.dx[0],in.dx[0]);
}

template <typename T>
void smooth_shrink_module<T>::bbprop1(state<T> &in, state<T> &out){
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // clearing
  tin.zero_ddx();
  beta.zero_ddx();
  bias.zero_ddx();
  // backprop
  absmod.fprop1(in,tin);
  // failsafe
  idx_aloopf1(x, in, T, {
      if (*x > 20)
	*x = 20;
    });

  // bb = exp (beta .* bias)
  idx_mul(beta, bias, ebb);
  idx_exp(ebb);
  intg nf = bias.dim(0);

  idx<T> ttx(ebx[0].spec);
  idx<T> tty(ebx[0].spec);
  for (intg i=0; i< nf; i++) {
    // ebx = exp(beta * x)
    idx<T> ebxxi = ebx[i];
    idx<T> ebxdxi = ebx.dx[0][i];
    idx<T> ebxddxi = ebx.ddx[0][i];
    idx<T> tinxi = tin[i];
    idx<T> tindxi = tin.ddx[0][i];
    idx<T> outdxi = out.ddx[0][i];

    idx_dotc(tinxi,beta[i].get(),ebxxi);
    idx_exp(ebxxi);

    // ebdx = exp(beta*x) + exp(beta*bias) -1
    idx_addc(ebxxi,ebb[i].get()-1,ebxdxi);
    // ebddx = exp (beta*x)/ (exp(beta*x) + exp(beta*bias)-1)
    idx_div(ebxxi,ebxdxi,ebxddxi);

    // df/dx
    idx_mul(ebxddxi,ebxddxi,ebxddxi);
    idx_mul(ebxddxi,outdxi,tindxi);

    //eblprint(tinxi.get(0,0) << tindxi.get(0,0) << endl);

    // ebddx = 1/ebdx
    idx_inv(ebxdxi,ebxddxi);

    // df/dbias
    idx_dotc(ebxddxi,ebb[i].get(),ttx);
    idx_addc(ttx,(T)-1.0,ttx);
    idx_mul(ttx,ttx,ttx);
    bias.ddx[0][i].set((T)idx_dot(outdxi,ttx));

    // df/dbeta
    idx_mul(tinxi,ebxxi,ttx);
    idx_addc(ttx, bias[i].get() * ebb[i].get(),ttx);
    idx_mul(ttx,ebxddxi,ttx);
    idx_dotc(ttx, 1/beta[i].get(),ttx);
    idx_log(ebxdxi);
    idx_dotc(ebxdxi,-1/(beta[i].get()*beta[i].get()),tty);
    idx_add(ttx,tty,ttx);
    idx_mul(ttx,ttx,ttx);
    beta.ddx[0][i].set((T)idx_dot(outdxi,ttx));
  }
  idx_add(in.ddx[0],tin.ddx[0],in.ddx[0]);
}

template <typename T>
module_1_1<T>* smooth_shrink_module<T>::copy(parameter<T> *p) {
  smooth_shrink_module<T>* s2 =
      new smooth_shrink_module<T>(NULL, beta.dim(0), default_beta,
                                  default_bias);
  // assign same parameter state
  s2->beta = beta;
  s2->bias = bias;
  return (module_1_1<T>*) s2;
}

// tanh_shrink_module //////////////////////////////////////////////////////////

template <typename T>
tanh_shrink_module<T>::tanh_shrink_module(parameter<T> *p, intg nf, bool diags_)
    : module_1_1<T>("tanh_shrink"),
      nfeatures(nf), alpha(NULL), beta(NULL), diags(diags_) {
  if (diags) {
    alpha = new diag_module<T>(p, nf);
    beta = new diag_module<T>(p, nf);
  }
}

template <typename T>
tanh_shrink_module<T>::~tanh_shrink_module() {
  if (alpha) delete alpha;
  if (beta) delete beta;
}

template <typename T>
void tanh_shrink_module<T>::fprop1(idx<T>& in, idx<T>& out) {
  // checks
  if (&in != &out) { // resize only when input and output are different
    this->resize_output(in, out); // resize iff necessary
  } else eblerror("in and out should be different buffers");
  // fprop
  if (diags) { // use coefficients
    // x * alpha
    alpha->fprop1(in, abuf);
    // tanh(x * alpha)
    mtanh.fprop1(abuf, tbuf);
    // (x * alpha) - tanh(x * alpha)
    difmod.fprop1(in, tbuf, bbuf);
    // beta * ((x * alpha) - tanh(x * alpha))
    beta->fprop1(bbuf, out);
  } else { // no coefficients
    // tanh(x)
    mtanh.fprop1(in, tbuf);
    // x - tanh(x)
    difmod.fprop1(in, tbuf, out);
  }
}

template <typename T>
void tanh_shrink_module<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  abuf.resize_dx();
  bbuf.resize_dx();
  tbuf.resize_dx();
  // zeroing
  abuf.zero_dx();
  bbuf.zero_dx();
  tbuf.zero_dx();
  // bprop
  if (diags) { // use coefficients
    // bprop
    beta->bprop1(bbuf, out);
    difmod.bprop1(in, tbuf, bbuf);
    mtanh.bprop1(abuf, tbuf);
    alpha->bprop1(in, abuf);
  } else { // no coefficients
    difmod.bprop1(in, tbuf, out);
    mtanh.bprop1(in, tbuf);
  }
}

template <typename T>
void tanh_shrink_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  abuf.resize_ddx();
  bbuf.resize_ddx();
  tbuf.resize_ddx();
  // zeroing
  abuf.zero_ddx();
  bbuf.zero_ddx();
  tbuf.zero_ddx();
  // bbprop
  if (diags) { // use coefficients
    // bprop
    beta->bbprop1(bbuf, out);
    difmod.bbprop1(in, tbuf, bbuf);
    mtanh.bbprop1(abuf, tbuf);
    alpha->bbprop1(in, abuf);
  } else { // no coefficients
    difmod.bbprop1(in, tbuf, out);
    mtanh.bbprop1(in, tbuf);
  }
}

template <typename T>
module_1_1<T>* tanh_shrink_module<T>::copy(parameter<T> *p) {
  tanh_shrink_module<T>* s2 = new tanh_shrink_module<T>(NULL, nfeatures);
  // assign same parameter state
  if (s2->alpha) delete s2->alpha;
  if (s2->beta) delete s2->beta;
  s2->alpha = (diag_module<T>*) alpha->copy();
  s2->beta = (diag_module<T>*) beta->copy();
  return (module_1_1<T>*) s2;
}

template <typename T>
std::string tanh_shrink_module<T>::describe() {
  std::string desc;
  desc << "tanh_shrink module " << this->name()
       << (diags ? " with" : " without") << " scaling coefficients";
  return desc;
}

} // end namespace ebl
