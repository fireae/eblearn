/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun, Pierre Sermanet *
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

// ebm_1 ///////////////////////////////////////////////////////////////////////

template <typename T>
ebm_1<T>::ebm_1(const char *n) : module_1_1<T>(n) {
}

template <typename T>
ebm_1<T>::~ebm_1() {
}

template <typename T>
void ebm_1<T>::fprop1(idx<T> &in, idx<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_1<T>::bprop1(state<T> &in, state<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_1<T>::bbprop1(state<T> &in, state<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_1<T>::fprop(state<T> &in, state<T> &energy) {
  if (in.x.size() > 1)
    eblerror("expected only 1 tensor in input state");
  fprop1(in, energy);
}

template <typename T>
void ebm_1<T>::bprop(state<T> &in, state<T> &energy) {
  bprop1(in, energy);
}

template <typename T>
void ebm_1<T>::bbprop(state<T> &in, state<T> &energy) {
  bbprop1(in, energy);
}

template <typename T>
void ebm_1<T>::forget(forget_param_linear &fp) {
  err_not_implemented(); }

template <typename T>
void ebm_1<T>::normalize() { err_not_implemented(); }

// ebm_module_1_1 //////////////////////////////////////////////////////////////

template <typename T>
ebm_module_1_1<T>::ebm_module_1_1(module_1_1<T> *m, ebm_1<T> *e,
                                  const char *name_)
    : module_1_1<T>(name_), module(m), ebm(e) {
  if (!m) eblerror("expected non-null module");
  if (!e) eblerror("expected non-null ebm");
  this->_name = "";
  this->_name << m->name() << "+" << e->name();
  energy.resize_dx();
  energy.resize_ddx();
  energy.dx[0].set((T)1.0); // d(E)/dE is always 1
  energy.ddx[0].set((T)0.0); // dd(E)/dE is always 0
}

template <typename T>
ebm_module_1_1<T>::~ebm_module_1_1() {
  delete module;
  delete ebm;
}

template <typename T>
void ebm_module_1_1<T>::fprop1(idx<T> &in, idx<T> &out) {
  EDEBUG(this->name() << ": " << module->name() << ": in " << in);
  module->fprop1(in, out);
  ebm->fprop1(out, energy);
}

template <typename T>
void ebm_module_1_1<T>::bprop1(state<T> &in, state<T> &out) {
  EDEBUG(this->name() << ": " << module->name() << ": bprop in " << in);
  ebm->bprop1(out, energy);
  module->bprop1(in, out);
}

template <typename T>
void ebm_module_1_1<T>::bbprop1(state<T> &in, state<T> &out) {
  ebm->bbprop1(out, energy);
  module->bbprop1(in, out);
}

template <typename T>
void ebm_module_1_1<T>::forget(forget_param_linear &fp) {
  module->forget(fp);
}

template <typename T>
state<T>& ebm_module_1_1<T>::get_energy() {
  return energy;
}

template <typename T>
fidxdim ebm_module_1_1<T>::fprop1_size(fidxdim &isize) {
  return module->fprop1_size(isize);
}

template <typename T>
fidxdim ebm_module_1_1<T>::bprop1_size(const fidxdim &osize) {
  return module->bprop1_size(osize);
}

template <typename T>
std::string ebm_module_1_1<T>::describe() {
  std::string desc;
  desc << "ebm_module_1_1 " << this->name() << " contains a module_1_1: "
       << module->describe() << ", and an ebm1: " << ebm->describe();
  return desc;
}

// ebm_2 ///////////////////////////////////////////////////////////////////////

template <typename T>
ebm_2<T>::ebm_2(const char *name_) : module_2_1<T>(name_) {
}

template <typename T>
ebm_2<T>::~ebm_2() {
}

template <typename T>
void ebm_2<T>::fprop1(idx<T> &i1, idx<T> &i2, idx<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_2<T>::bprop1(state<T> &i1, state<T> &i2, state<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_2<T>::bbprop1(state<T> &i1, state<T> &i2, state<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_2<T>::fprop(state<T> &i1, state<T> &i2, state<T> &energy) {
  if (i1.x.size() > 1 || i2.x.size() > 1)
    eblerror("expected only 1 tensor in input states");
  fprop1(i1, i2, energy);
}

template <typename T>
void ebm_2<T>::bprop(state<T> &i1, state<T> &i2, state<T> &energy) {
  bprop1(i1, i2, energy);
}

template <typename T>
void ebm_2<T>::bbprop(state<T> &i1, state<T> &i2, state<T> &energy) {
  bbprop1(i1, i2, energy);
}

template <typename T>
void ebm_2<T>::bprop1_copy(state<T> &i1, state<T> &i2, state<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_2<T>::bprop2_copy(state<T> &i1, state<T> &i2, state<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_2<T>::bbprop1_copy(state<T> &i1, state<T> &i2, state<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_2<T>::bbprop2_copy(state<T> &i1, state<T> &i2, state<T> &energy) {
  err_not_implemented(); }

template <typename T>
void ebm_2<T>::forget(forget_param_linear &fp) {
  err_not_implemented(); }

template <typename T>
void ebm_2<T>::normalize() {
  err_not_implemented(); }

template <typename T>
double ebm_2<T>::infer1(state<T> &i1, state<T> &i2, state<T> &energy,
                        infer_param &ip) {
  err_not_implemented(); return 0; }

template <typename T>
void ebm_2<T>::infer2(state<T> &i1, state<T> &i2, infer_param &ip,
                      state<T> *energy) {
  err_not_implemented(); }

template <typename T>
void ebm_2<T>::infer2_copy(state<T> &i1, state<T> &i2, state<T> &energy) {
  err_not_implemented(); }

// fc_ebm1 /////////////////////////////////////////////////////////////////////

template <typename T>
fc_ebm1<T>::fc_ebm1(module_1_1<T> &fm, state<T> &fo, ebm_1<T> &fc)
    : fmod(fm), fout(fo), fcost(fc) {
}

template <typename T>
fc_ebm1<T>::~fc_ebm1() {}

template <typename T>
void fc_ebm1<T>::fprop1(idx<T> &in, idx<T> &energy) {
  fmod.fprop1(in, fout);
  fcost.fprop1(fout, energy);
}

template <typename T>
void fc_ebm1<T>::bprop1(state<T> &in, state<T> &energy) {
  fout.zero_dx();
  fcost.bprop1(fout, energy);
  fmod.bprop1(in, fout);
}

template <typename T>
void fc_ebm1<T>::bbprop1(state<T> &in, state<T> &energy) {
  fout.zero_ddx();
  fcost.bbprop1(fout, energy);
  fmod.bbprop1(in, fout);
}

template <typename T>
void fc_ebm1<T>::forget(forget_param_linear &fp) {
  fmod.forget(fp);
  fcost.forget(fp);
}

// fc_ebm2 /////////////////////////////////////////////////////////////////////

template <typename T>
fc_ebm2<T>::fc_ebm2(module_1_1<T> &fm, state<T> &fo, ebm_2<T> &fc)
    : fmod(fm), fout(fo), fcost(fc) {
}

template <typename T>
fc_ebm2<T>::~fc_ebm2() {}

template <typename T>
void fc_ebm2<T>::fprop1(idx<T> &in1, idx<T> &in2, idx<T> &energy) {
  fmod.fprop1(in1, fout);
  fcost.fprop1(fout, in2, energy);
#ifdef __DUMP_STATES__ // used to debug
  save_matrix(energy, "dump_fc_ebm2_energy.x.mat");
  save_matrix(in1, "dump_fc_ebm2_cost_in1.x.mat");
#endif
}

template <typename T>
void fc_ebm2<T>::bprop1(state<T> &in1, state<T> &in2, state<T> &energy) {
  fout.clear_dx();
  // in2.clear_dx(); // TODO this assumes state<T> == fstate_idx
  fcost.bprop1(fout, in2, energy);
  fmod.bprop1(in1, fout);
}

template <typename T>
void fc_ebm2<T>::bbprop1(state<T> &in1, state<T> &in2, state<T> &energy){
  fout.clear_ddx();
  // in2.clear_ddx(); // TODO this assumes state<T> == fstate_idx
  fcost.bbprop1(fout, in2, energy);
  fmod.bbprop1(in1, fout);
}

template <typename T>
void fc_ebm2<T>::forget(forget_param_linear &fp) {
  fmod.forget(fp);
  fcost.forget(fp);
}

template <typename T>
void fc_ebm2<T>::infer2(state<T> &i1, state<T> &i2, infer_param &ip,
                        state<T> *energy) {
  fmod.fprop(i1, fout); // first propagate all the way up
  fcost.infer2(fout, i2, ip, energy); //then infer from energies
}

} // end namespace ebl
