/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#ifndef EBL_ENERGY_HPP_
#define EBL_ENERGY_HPP_

namespace ebl {

// l2_energy ///////////////////////////////////////////////////////////////////

template <typename T>
l2_energy<T>::l2_energy(const char *name_) : ebm_2<T>(name_) {
}

template <typename T>
l2_energy<T>::~l2_energy() {
}

template <typename T>
void l2_energy<T>::fprop1(idx<T> &in1, idx<T> &in2, idx<T> &energy) {
  idx_sqrdist(in1, in2, energy); // squared distance between in1 and in2
  idx_dotc(energy, 0.5, energy); // multiply by .5
}

template <typename T>
void l2_energy<T>::bprop1(state<T> &in1, state<T> &in2, state<T> &energy) {
  // checks
  DEBUG_CHECK_DX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DX(in2); // in debug mode, check backward tensors are allocated
  idx_checkorder1(energy, 0); // energy must have an order of 0
  // backprop
  idx_sub(in1, in2, in1.dx[0]); // derivative with respect to in1
  idx_dotc(in1.dx[0], energy.dx[0].get(), in1.dx[0]); // multiply by energy deriv
  idx_minus(in1.dx[0], in2.dx[0]); // derivative with respect to in2
}

template <typename T>
void l2_energy<T>::bbprop1(state<T> &in1, state<T> &in2, state<T> &energy) {
  // checks
  DEBUG_CHECK_DDX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DDX(in2); // in debug mode, check backward tensors are allocated
  // backprop
  idx_addc(in1.ddx[0], energy.dx[0].get(), in1.ddx[0]);
  idx_addc(in2.ddx[0], energy.dx[0].get(), in2.ddx[0]);
}

template <typename T>
void l2_energy<T>::infer2_copy(state<T> &in1, state<T> &in2, state<T> &energy) {
  idx_copy(in1, in2);
  idx_clear(energy);
}

template <typename T>
std::string l2_energy<T>::describe() {
  std::string s;
  s << "energy " << this->name() << " is the euclidean distance between inputs";
  return s;
}

// l1_penalty //////////////////////////////////////////////////////////////////

template <typename T>
l1_penalty<T>::l1_penalty(T t, T c) : threshold(t), coeff(c) {
}

template <typename T>
l1_penalty<T>::~l1_penalty() {
}

template <typename T>
void l1_penalty<T>::fprop1(idx<T> &in, idx<T> &energy) {
  idx_sumabs(in, energy.idx_ptr());
  energy.set(energy.get() * coeff);
}

template <typename T>
void l1_penalty<T>::bprop1(state<T> &in, state<T> &energy) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_thresdotc_acc(in, energy.dx[0].get() * coeff, threshold, in.dx[0]);
}

template <typename T>
void l1_penalty<T>::bbprop1(state<T> &in, state<T> &energy) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_addc(in.ddx[0], energy.ddx[0].get() * coeff * coeff, in.ddx[0]);
}

template <typename T>
std::string l1_penalty<T>::describe() {
  std::string s;
  s << "l1 penalty " << this->name()
    << " with threshold " << threshold << " and coefficient " << coeff;
  return s;
}

// cross_entropy_energy ////////////////////////////////////////////////////////

template <typename T>
cross_entropy_energy<T>::cross_entropy_energy(const char *name_)
    : ebm_2<T>(name_) {
}

template <typename T>
cross_entropy_energy<T>::~cross_entropy_energy() {
}

template <typename T>
void cross_entropy_energy<T>::fprop1(idx<T> &in1, idx<T> &in2, idx<T> &energy) {
  if (!in1.same_dim(tmp)) tmp = idx<T>(in1.get_idxdim());
  // softmax
  softmax.fprop1(in1, soft_out);
  // assuming in2 is a target vector with, normalize it between 0 and 1
  idx_addc(in2, -idx_min(in2), in2);
  idx_dotc(in2, 1 / idx_max(in2), in2);
  // log(softmax)
  idx_log(soft_out, tmp);
  // E = -log(softmax)
  energy.sset(-idx_dot(tmp, in2));
}

template <typename T>
void cross_entropy_energy<T>::bprop1(state<T> &in1, state<T> &in2,
                                     state<T> &energy) {
  // checks
  DEBUG_CHECK_DX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DX(in2); // in debug mode, check backward tensors are allocated
  idx_checkorder1(energy, 0); // energy must have an order of 0

  // backprop to in1
  idx_dotc(soft_out, idx_sum(in2), tmp);
  idx_sub(tmp, in2);
  idx_dotcacc(tmp, energy.dx[0].gget(), in1.dx[0]);
  // backprop to in2
  idx_log(soft_out, tmp);
  idx_dotcacc(tmp, energy.dx[0].gget(), in2.dx[0]);
}

template <typename T>
void cross_entropy_energy<T>::bbprop1(state<T> &in1, state<T> &in2,
                                      state<T> &energy) {
  // checks
  DEBUG_CHECK_DDX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DDX(in2); // in debug mode, check backward tensors are allocated

  // backprop to in1
  idx_dotc(soft_out, idx_sum(in2), tmp);
  idx_sub(tmp, in2);
  idx_mul(tmp, tmp, tmp);
  idx_dotcacc(tmp, energy.ddx[0].gget(), in1.ddx[0]);
  // backprop to in2
  idx_log(soft_out, tmp);
  idx_mul(tmp, tmp, tmp);
  idx_dotcacc(tmp, energy.ddx[0].gget(), in2.ddx[0]);
}

template <typename T>
void cross_entropy_energy<T>::infer2_copy(state<T> &in1, state<T> &in2,
                                          state<T> &energy) {
  idx_copy(in1, in2);
  idx_clear(energy);
}

template <typename T>
std::string cross_entropy_energy<T>::describe() {
  std::string s;
  s << "cross-entropy energy " << this->name() << " aka softmax regression";
  return s;
}

// scalerclass_energy //////////////////////////////////////////////////////////

template <typename T>
scalerclass_energy<T>::
scalerclass_energy(bool apply_tanh_, uint jsize_, uint jselection,
                   float dist_coeff_, float scale_coeff_,
                   bool predict_conf_, bool predict_bconf_,
                   idx<T> *biases_, idx<T> *coeffs_,
                   const char *name_)
    : l2_energy<T>(name_), jsize(jsize_), apply_tanh(apply_tanh_),
      jitter_selection(jselection), dist_coeff(dist_coeff_),
      scale_coeff(scale_coeff_), predict_conf(predict_conf_),
      predict_bconf(predict_bconf_), biases(NULL), coeffs(NULL) {
  if (biases_) biases = new idx<T>(*biases_);
  if (coeffs_) coeffs = new idx<T>(*coeffs_);
}

template <typename T>
scalerclass_energy<T>::~scalerclass_energy() {
  if (biases) delete biases;
  if (coeffs) delete coeffs;
}

template <typename T>
void scalerclass_energy<T>::fprop1(idx<T> &in, idx<T> &in2, idx<T> &energy) {
  // determine sizes
  int nclass = in.dim(0) - jsize;
  if (predict_conf) nclass--;
  // sanity checks
  // if (in.get_idxdim() != in2.get_idxdim())
  //   eblerror("expected same dimensions but got " << in << " and " << in2);
  // narrow inputs for regular l2 energy: class inputs
  tmp = in.narrow(0, nclass, 0);
  // apply tanh if requested
  if (apply_tanh) {
    if (tmp.get_idxdim() != tmp2.get_idxdim())
      tmp2 = state<T>(tmp.get_idxdim());
    mtanh.fprop1(tmp, tmp2);
    tmp = tmp2;
  } else { // if no tanh, cap with -1/1 to avoid penalties beyond these
    idx_threshold(tmp, (T)-1); // cap below by -1
    idx_threshold2(tmp, (T)1); // cap above by 1
  }
  // select jitter target among all possible ones
  if (in2.dim(0) == 1) { // only 1 possible target
    best_target = in2.select(0, 0);
  } else { // multiple targets
    T minscore = limits<T>::max();
    switch (jitter_selection) {
      case 0: // select highest confidence target
	{ uint jindex = 0;
        idx<T> tgt;
        if (predict_conf) { // use predict conf feature
          tgt = in2.narrow(1, 1, in2.dim(1) - 1);
          jindex = idx_indexmax(tgt);
        } else { // use class target
          idx<T> tgt = in2.narrow(1, nclass, 0);
          uint i = 0;
          T max_val = limits<T>::min();
          { idx_bloop1(t, tgt, T) {
              T val = idx_max(t);
              if (val > max_val) {
                max_val = val;
                jindex = i;
              }
              i++;
            }}
        }
        // select the highest confidence target
        best_target = in2.select(0, jindex);
	}
	break ;
      case 1: // select closest to center and scale 1
	// loop on all possible jitter
	{ idx_bloop1(tgt, in2, T) {
            T s1 = tgt.gget(nclass);
            T h1 = tgt.gget(nclass + 1);
            T w1 = tgt.gget(nclass + 2);
            T score = std::fabs(s1 - 1) * scale_coeff // ~[.8,2.0]
                + sqrt(h1 * h1 + w1 * w1) * dist_coeff;
            if (score < minscore) { // we found a better match
              minscore = score;
              best_target = tgt;
            }
          }}
	break ;
      case 2: // select closest to current answer
	{ T s = in.gget(nclass); // predicted scale
          T h = in.gget(nclass + 1); // predicted h
          T w = in.gget(nclass + 2); // predicted w
          // loop on all possible jitter
          { idx_bloop1(tgt, in2, T) {
              T s1 = tgt.gget(nclass);
              T h1 = tgt.gget(nclass + 1);
              T w1 = tgt.gget(nclass + 2);
              T score = sqrt((h1 - h) * (h1 - h) + (w1 - w) * (w1 - w))
                  * dist_coeff + std::fabs(s1 - s) * scale_coeff;
              if (score < minscore) { // we found a better match
		minscore = score;
		best_target = tgt;
              }
            }}
	}
	break ;
      default:
	eblerror("unknown selection mode " << jitter_selection);
    }
  }
  // resize target buffer
  idxdim d(best_target.get_idxdim());
  if (last_target.get_idxdim() != d) {
    if (last_target.order() != d.order()) {
      last_target = state<T>(d);
      last_target_raw = idx<T>(d);
    } else {
      last_target.resize(d);
      last_target_raw.resize(d);
    }
    last_class_target = last_target.narrow(0, nclass,0);
    last_jitt_target = last_target.narrow(0, jsize, nclass);
    if (predict_conf)
      last_conf_target = last_target.narrow(0, 1, in.dim(0) - 1);
  }
  idx_copy(best_target, last_target);
  // make confidence target binary if required
  uint conf_offset = idx_indexmax(last_class_target);
  if (predict_conf)
    conf_offset = in.dim(0) - 1;
  if (predict_bconf) { // make confidence binary (0, 1)
    if (last_target.gget(conf_offset) > .5)
      last_target.sset((T)1, conf_offset);
    else
      last_target.sset((T)0, conf_offset);
  }
  // save raw target
  idx_copy(last_target, last_target_raw);
  T s = last_target_raw.gget(nclass); // scale target
  // normalize jitt with bias then coeff
  if (biases) {
    idx<T> tmpbias = biases->narrow(0, jsize, 0);
    idx_add(last_jitt_target, tmpbias, last_jitt_target);
  }
  if (coeffs) {
    idx<T> tmpcoeff = coeffs->narrow(0, jsize, 0);
    idx_mul(last_jitt_target, tmpcoeff, last_jitt_target);
  }
  // normalize prediction with bias then coeff, only if using extra component
  // (otherwise, target uses the full -1,1 range already
  if (predict_conf) {
    if (biases) {
      idx<T> tmpbias = biases->narrow(0, 1, jsize);
      idx_add(last_conf_target, tmpbias, last_conf_target);
    }
    if (coeffs) {
      idx<T> tmpcoeff = coeffs->narrow(0, 1, jsize);
      idx_mul(last_conf_target, tmpcoeff, last_conf_target);
    }
  }
  // l2 energy
  l2_energy<T>::fprop1(tmp, last_class_target, energy);
  // energy of scale component
  T e = 0;
  // penalize quadraticaly only if scale is > 0
  if (s > 0 && last_target_raw.gget(conf_offset) > .5) {
    // narrow inputs for jitter energy
    tmp = in.narrow(0, jsize, nclass);
    e = .5 * idx_sqrdist(tmp, last_jitt_target);
    energy.set(energy.get() + e);
  }
  // penalize predicted confidence only if positive (i.e. scale > 0)
  if (predict_conf && s > 0) {
    tmp = in.narrow(0, 1, conf_offset);
    // cap below by 0 and above by 1 (or corresponding normalized values)
    T low = 0, high = 1;
    if (biases) { low += biases->gget(3); high += biases->gget(3); }
    if (coeffs) { low *= biases->gget(3); high *= biases->gget(3); }
    idx_threshold(tmp, low);
    idx_threshold2(tmp, high);
    e = .5 * idx_sqrdist(tmp, last_conf_target);
    energy.set(energy.get() + e);
  }
  EDEBUG("energy: " << energy.get() << " in: " << in.str() << " norm tgt: "
         << last_target.str() << " raw tgt: " << last_target_raw.str()
         << " conf penalized: " << ((predict_conf && s > 0) ? "yes":"no")
         << " jitt penalized: "
         << ((s > 0 && last_target_raw.gget(conf_offset) > .5) ? "yes":"no"));
}

template <typename T>
void scalerclass_energy<T>::bprop1(state<T> &in, state<T> &in2,
                                   state<T> &energy) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DX(in2); // in debug mode, check backward tensors are allocated
  idx_checkorder1(energy, 0); // energy must have an order of 0
  // backprop
  int nclass = in.dim(0) - jsize;
  if (predict_conf) nclass--;
  uint conf_offset = idx_indexmax(last_class_target);
  if (predict_conf)
    conf_offset = in.dim(0) - 1;
  // narrow inputs for regular l2 energy
  tmp = in.narrow_state(0, nclass, 0);
  l2_energy<T>::bprop1(tmp, last_class_target, energy);
  // get values
  T s = last_target_raw.gget(nclass);
  // penalize quadraticaly only if scale is > 0
  if (s > 0 && last_target_raw.gget(conf_offset) > .5) {
    // narrow inputs for jitter energy
    tmp = in.narrow(0, jsize, nclass);
    idx_sub(tmp, last_jitt_target, tmp.dx[0]); // derivative w.r.t in1
    idx_dotc(tmp.dx[0], energy.dx[0].get(), tmp.dx[0]);// multiply by energy derivative
  }
  // penalize predicted confidence only if positive (i.e. scale > 0)
  if (predict_conf && s > 0) {
    tmp = in.narrow_state(0, 1, conf_offset);
    idx_sub(tmp, last_conf_target, tmp.dx[0]);
    idx_dotc(tmp.dx[0], energy.dx[0].get(), tmp.dx[0]);// multiply by energy derivative
  }
}

template <typename T>
void scalerclass_energy<T>::bbprop1(state<T> &in1, state<T> &in2,
                                    state<T> &energy) {
  // checks
  DEBUG_CHECK_DDX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DDX(in2); // in debug mode, check backward tensors are allocated
  // backprop
  last_target.zero_ddx();
  // derivatives are all the same for everybody
  l2_energy<T>::bbprop1(in1, last_target, energy);
}

template <typename T>
void scalerclass_energy<T>::infer2(state<T> &in, state<T> &in2,
                                   infer_param &ip, state<T> *energy) {
  idx_copy(in, in2);
  idx_clear(*energy);
}

template <typename T>
std::string scalerclass_energy<T>::describe() {
  std::string s;
  s << "energy " << this->name()
    << " is the squared distance with target for class components and the "
    << "squared distance to the scale component when scale > 0 and jsize "
    << jsize;
  if (predict_conf)
    s << ", predicting confidence";
  s << ", target confidence is " << (predict_bconf?"binary":"continuous");
  s << ", biases: ";
  if (biases)
    biases->printElems(s);
  else
    s <<"none";
  s << ", coeffs: ";
  if (coeffs)
    coeffs->printElems(s);
  else
    s <<"none";
  return s;
}

// scaler_energy ///////////////////////////////////////////////////////////////

template <typename T>
scaler_energy<T>::scaler_energy(const char *name_) : ebm_2<T>(name_) {
}

template <typename T>
scaler_energy<T>::~scaler_energy() {
}

template <typename T>
void scaler_energy<T>::fprop1(idx<T> &in, idx<T> &in2, idx<T> &energy) {
  // sanity checks
  idx_checknelems2_all(in, in2);
  // get values
  T i = in.gget();
  T s = in2.gget();
  T e = 0;
  // no scale case: penalize quadraticaly only if above -1
  if (s == 0) {
    e = std::max((T) 0, i + 1);
    e = e * e * .5;
  } else // positive case: penalize quadraticaly with distance to value
    e = .5 * idx_sqrdist(in, in2);
  energy.set(e);
}

template <typename T>
void scaler_energy<T>::bprop1(state<T> &in, state<T> &in2, state<T> &energy) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DX(in2); // in debug mode, check backward tensors are allocated
  idx_checkorder1(energy, 0); // energy must have an order of 0
  // backprop
  // get values
  T i = in.gget();
  T s = in2.gget();
  // no scale case: penalize quadraticaly only if above -1
  if (s == 0) {
    in.dx[0].sset(std::max((T) 0, i + 1));
    in.dx[0].sset(in.dx[0].gget() * energy.dx[0].get());
  } else { // scale case: penalize quadraticaly with distance to value
    idx_sub(in, in2, in.dx[0]);
    idx_dotc(in.dx[0], energy.dx[0].get(), in.dx[0]); // multiply by energy derivative
  }
}

template <typename T>
void scaler_energy<T>::bbprop1(state<T> &in1, state<T> &in2, state<T> &energy) {
  // checks
  DEBUG_CHECK_DDX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DDX(in2); // in debug mode, check backward tensors are allocated
  // backprop
  idx_addc(in1.ddx[0], energy.dx[0].get(), in1.ddx[0]);
}

template <typename T>
void scaler_energy<T>::infer2(state<T> &in, state<T> &scale,
                              infer_param &ip, state<T> *energy) {
  T i = in.gget();
  if (i <= 0) // negative class
    scale.sset((T) 0);
  else // positive class
    scale.sset(i);
}

template <typename T>
std::string scaler_energy<T>::describe() {
  std::string s;
  s << "energy " << this->name()
    << " is the squared distance to -1 when input is > -1 for the "
    << "negative class and the squared distance to scale target for the "
    << "positive class";
  return s;
}

} // end namespace ebl

#endif /* EBL_ENERGY_HPP */
