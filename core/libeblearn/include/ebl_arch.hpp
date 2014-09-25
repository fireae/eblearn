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

// layers //////////////////////////////////////////////////////////////////////

template <typename T> layers<T>::layers(bool oc, const char *name_)
    : module_1_1<T>(name_), hi(NULL), ho(NULL), htmp(NULL) {
  this->own_contents = oc;
  this->default_name("layers");
}

// Clean vectors. Module doesn't have ownership of sub-modules
template <typename T> layers<T>::~layers() {
  if (this->own_contents) {
    for (uint i = 0; i < modules.size(); i++) delete modules[i];
    if (!this->memoptimized) {
      for(uint i = 0; i < hiddens.size(); i++) delete hiddens[i];
    }
  }
}

template <typename T>
void layers<T>::add_module(module_1_1<T>* module) {
  // regular addition
  modules.push_back(module);
  hiddens.push_back(NULL);
  // update what type of input/output are expected
  this->bmstate_input = modules[0]->mstate_input();
  this->bmstate_output = modules[modules.size() - 1]->mstate_output();
}

// fprop1 //////////////////////////////////////////////////////////////////////

template <typename T>
void layers<T>::fprop1(idx<T>& in, idx<T>& out) {
  state<T> sin(in), sout(out);
  forward(sin, sout, T_FPROP1);
  out = sout;
}

template <typename T>
void layers<T>::bprop1(state<T>& in, state<T>& out) {
  backward(in, out, T_BPROP1);
}

template <typename T>
void layers<T>::bbprop1(state<T>& in, state<T>& out) {
  bbackward(in, out, T_BPROP1);
}

// fprop ///////////////////////////////////////////////////////////////////////

template <typename T>
void layers<T>::fprop(state<T>& in, state<T>& out) {
  forward(in, out, T_FPROP);
}

template <typename T>
void layers<T>::bprop(state<T>& in, state<T>& out) {
  backward(in, out, T_BPROP);
}

template <typename T>
void layers<T>::bbprop(state<T>& in, state<T>& out) {
  bbackward(in, out, T_BPROP);
}

// dumping /////////////////////////////////////////////////////////////////////

template <typename T>
void layers<T>::fprop_dump(state<T>& in, state<T>& out) {
  forward(in, out, T_FPROP_DUMP);
}

// optimization ////////////////////////////////////////////////////////////////

// TODO: fix optimize fprop

// template <typename T>
// bool layers<T>::optimize_fprop(Mstate& in, Mstate& out){
//   this->memoptimized = true;
//   if (modules.empty())
//     eblerror("trying to fprop through empty layers");
//   // initialize buffers
//   hi = &in;
//   ho = &out;
//   // parallelism: do not modify input nor output
//   if (branch) {
//     // create our internal buffers with all dimensions set to 1
//     intern_h0 = new Mstate(in);
//     intern_h1 = new Mstate(in);
//     ho = intern_h0;
//   }
//   // loop over modules
//   for (uint i = 0; i < modules.size(); i++) {
//     hiddens[i] = ho;
//     // parallelism: for first module, do not allow optim with in buffer
//     if (branch && i == 0) {
// 	hi = intern_h1; // now we use only internal buffers
// 	swap_buffers(); // swap hi and ho
//     } else {
// 	// call optimization on submodules, and remember if they put
// 	// the output in ho (swap == true) or not (swap == false).
// 	bool swap = modules[i]->optimize_fprop(*hi,*ho);
// 	// if output is truly in ho, swap buffers, otherwise do nothing.
// 	// if module was a branch, it di
// 	if (swap)
// 		swap_buffers();
//     }
//   }
//   // parallelism: remember which buffer contains the output
//   if (branch) {
//     intern_out = hiddens[modules.size() - 1];
//     // a branch does not output to current track, so the output for the
//     // mother branch is actually the branch's input, which is left in in
//     return false; // output is in in
//   }
//   // tell the outside if the output is in in or out
//   if (hiddens[modules.size() - 1] == &out)
//     return true; // output is in out
//   return false; // output is in in
// }

//////////////////////////////////////////////////////////////////////////////

template <typename T>
void layers<T>::forget(forget_param_linear& fp){
  if (modules.empty()) eblerror("trying to forget through empty layers");
  for (uint i = 0; i < modules.size(); i++)
    modules[i]->forget(fp);
}

template <typename T>
void layers<T>::normalize(){
  if (modules.empty()) eblerror("trying to normalize through empty layers");
  for (uint i = 0; i < modules.size(); i++)
    modules[i]->normalize();
}

template <typename T>
fidxdim layers<T>::fprop1_size(fidxdim &isize) {
  fidxdim os(isize);
  //! Loop through all the layers of the module, and update output
  //! size accordingly.
  for (uint i = 0; i < modules.size(); i++)
    os = modules[i]->fprop1_size(os);
  //! Recompute the input size to be compliant with the output
  isize = bprop1_size(os);
  return os;
}

template <typename T>
fidxdim layers<T>::bprop1_size(const fidxdim &osize) {
  fidxdim isize(osize);
  //! Loop through all the layers of the module, from the end to the beg.
  for (int i = (int) modules.size() - 1; i >= 0; i--)
    isize = modules[i]->bprop1_size(isize);
  return isize;
}

template <typename T>
mfidxdim layers<T>::fprop_size(mfidxdim &isize) {
  mfidxdim os(isize);
  //! Loop through all the layers of the module, and update output
  //! size accordingly.
  for (unsigned int i = 0; i < modules.size(); i++)
    os = modules[i]->fprop_size(os);
  //! Recompute the input size to be compliant with the output
  isize = bprop_size(os);
  this->ninputs = isize.size();
  this->noutputs = os.size();
  return os;
}

template <typename T>
mfidxdim layers<T>::bprop_size(mfidxdim &osize) {
  mfidxdim isize(osize);
  //! Loop through all the layers of the module, from the end to the beg.
  for (int i = (int) modules.size() - 1; i >= 0; i--)
    isize = modules[i]->bprop_size(isize);
  return isize;
}

template <typename T>
module_1_1<T>* layers<T>::copy(parameter<T> *p) {
  layers<T> *l2 = new layers<T>(true);
  //! Loop through all the modules and buffers and copy them
  int niter = this->modules.size();
  for (int i = 0; i < niter; i++) {
    l2->add_module(this->modules[i]->copy(p));
    if (this->hiddens[i] != NULL) {
      l2->hiddens[i] = new state<T>(*(this->hiddens[i]));
      l2->hiddens[i]->deep_copy(*(l2->hiddens[i]));
    }
  }
  return (module_1_1<T>*) l2;
}

template <typename T>
void layers<T>::swap_buffers() {
  htmp = hi;
  hi = ho;
  ho = htmp;
}

template <typename T>
uint layers<T>::size() {
  return modules.size();
}

template <typename T>
std::string layers<T>::pretty(idxdim &isize) {
  mfidxdim is(isize);
  return this->pretty(is);
}

template <typename T>
std::string layers<T>::pretty(mfidxdim &isize) {
  std::string s;
  mfidxdim is(isize);
  //! Loop through all the layers of the module, and update output
  //! size accordingly.
  for (unsigned int i = 0; i < modules.size(); i++) {
    s << modules[i]->pretty(is);
    mfidxdim mis(is);
    mis = modules[i]->fprop_size(mis);
    is = mis;
  }
  return s;
}

template <typename T>
void layers<T>::zero_dx() {
  // clear hidden states
  for (uint i = 0; i < hiddens.size(); i++)
    if (hiddens[i]) {
      hiddens[i]->resize_dx(); // make sure it has the right size
      hiddens[i]->zero_dx();
    }
  // clear hidden in each module
  for (uint i = 0; i < modules.size(); i++)
    if (modules[i]) {
      modules[i]->zero_dx(); // make sure it has the right size
    }
}

template <typename T>
void layers<T>::zero_ddx() {
  // clear hidden states
  for (uint i = 0; i < hiddens.size(); i++)
    if (hiddens[i]) {
      hiddens[i]->resize_ddx(); // make sure it has the right size
      hiddens[i]->zero_ddx();
    }
  // clear hidden in each module
  for (uint i = 0; i < modules.size(); i++)
    if (modules[i]) {
      modules[i]->zero_ddx(); // make sure it has the right size
    }
}

template <typename T>
module_1_1<T>* layers<T>::find(const char *name) {
  for (uint i = 0; i < modules.size(); ++i) {
    module_1_1<T>* m = modules[i];
    if (!strcmp(name, m->name()))
      return m;
  }
  return NULL; // not found
}

template <typename T>
module_1_1<T>* layers<T>::last_module() {
  if (modules.size() == 0)
    eblerror("requires at least 1 module");
  return modules[modules.size() - 1]->last_module();
}

template <typename T>
std::string layers<T>::describe_indent(uint indent) {
  std::string desc;
  desc << "Module " << this->name() << " contains "
       << (int) modules.size() << " modules:\n";
  for (uint i = 0; i < modules.size(); ++i) {
    for (uint j = 0; j < indent; ++j) desc << "\t";
    desc << i << ": " << modules[i]->describe();
    if (i != modules.size() - 1) desc << "\n";
  }
  return desc;
}

template <typename T>
std::string layers<T>::info() {
  std::string s;
  s << this->name() << " info";
  for (uint i = 0; i < modules.size(); ++i) {
    s << ", modules[" << i << "] ";
    if (modules[i]) s << modules[i]->name();
    else s << "null";
    s << " hiddens[" << i << "] ";
    if (hiddens[i]) s << hiddens[i]->info();
    else s << "null";
  }
  return s;
}

template <typename T>
bool layers<T>::mstate_input() {
  if (modules.size())
    return modules[0]->mstate_input();
  return this->bmstate_input;
}

template <typename T>
bool layers<T>::mstate_output() {
  if (modules.size())
    return modules[modules.size() - 1]->mstate_output();
  return this->bmstate_output;
}

template <typename T>
void layers<T>::set_output_streams(std::ostream &out,
                                   std::ostream &err) {
  for(uint i = 0; i < modules.size(); ++i)
    modules[i]->set_output_streams(out, err);
}

// internal methods ////////////////////////////////////////////////////////////

template <typename T>
void layers<T>::forward(state<T>& in, state<T>& out, fprop_type fp) {
  if (modules.empty()) eblerror("trying to fprop through empty layers");
  // initialize buffers
  hi = &in;
  ho = &out;
  // loop over modules
  for (int i = 0; i < (int) modules.size(); i++) {
    LOCAL_TIMING_START(); // timing debugging
    // if last module, output into out
    if (i == (int) modules.size() - 1) ho = &out;
    else { // not last module, use hidden buffers
      ho = (state<T>*) hiddens[i];
      // allocate hidden buffer if necessary
      if (ho == NULL) {
        // allocate states with only 1 state.
        hiddens[i] = new state<T>(in, 1, 1);
        ho = (state<T>*) hiddens[i];
      }
    }
    // run module
    module_1_1<T> *mod = modules[i];
    EDEBUG_MAT(mod->name() << ": in", *hi);
    switch (fp) {
      case T_FPROP1: mod->fprop1(*hi, *ho); break ;
      case T_FPROP: mod->fprop(*hi, *ho); break ;
      case T_FPROP_DUMP: mod->fprop_dump(*hi, *ho); break ;
      default: eblerror("unknown type");
    }
    EDEBUG_MAT(mod->name() << ": out", *ho);
    hi = ho;
    LOCAL_TIMING_REPORT(mod->name()); // timing debugging
  }
  // remember number of input/outputs
  this->ninputs = in.x.size();
  this->noutputs = out.x.size();
}


template <typename T>
void layers<T>::backward(state<T>& in, state<T>& out, bprop_type bp) {
  if (this->memoptimized)
    eblerror("cannot bprop while using dual-buffer memory optimization");
  if (modules.empty()) eblerror("trying to bprop through empty layers");
  // clear hidden states
  zero_dx();
  EDEBUG(this->name() << ": in " << in);
  // init buffers
  hi = &out;
  ho = &out;
  // last will be manual
  for (int i = (int) modules.size() - 1; i >= 0; i--){
    LOCAL_TIMING_START(); // timing debugging
    // set input
    if (i == 0) hi = &in;
    else hi = hiddens[i - 1];
    // make sure input's backward tensors exist
    hi->resize_dx();
    // run module
    module_1_1<T> *mod = modules[i];
    EDEBUG_MAT(mod->name() << ": ho.dx ", ho->dx[0]);
    switch (bp) {
      case T_BPROP1: mod->bprop1(*hi, *ho); break ;
      case T_BPROP: mod->bprop(*hi, *ho); break ;
      default: eblerror("unknown type");
    }
    EDEBUG_MAT(mod->name() << ": hi.dx ", hi->dx[0]);
    ho = hi;
    LOCAL_TIMING_REPORT(mod->name() << " bprop"); // timing debugging
  }
}

template <typename T>
void layers<T>::bbackward(state<T>& in, state<T>& out, bprop_type bp) {
  if (this->memoptimized)
    eblerror("cannot bbprop while using dual-buffer memory optimization");
  if (modules.empty()) eblerror("trying to bbprop through empty layers");
  // clear hidden states
  zero_ddx();

  hi = &out;
  ho = &out;
  // last will be manual
  for(int i = (int) modules.size() - 1; i >= 0; i--){
    LOCAL_TIMING_START(); // timing debugging
    // set input
    if (i == 0) hi = &in;
    else hi = hiddens[i-1];
    // make sure input's backward tensors exist
    hi->resize_ddx();
    // run module
    module_1_1<T> *mod = modules[i];
    EDEBUG_MAT(mod->name() << ": ho.ddx ", ho->ddx[0]);
    switch (bp) {
      case T_BPROP1: mod->bbprop1(*hi, *ho); break ;
      case T_BPROP: mod->bbprop(*hi, *ho); break ;
      default: eblerror("unknown type");
    }
    EDEBUG_MAT(mod->name() << ": hi.ddx ", hi->ddx[0]);
    // shift output pointer to input
    ho = hi;
    LOCAL_TIMING_REPORT(mod->name() << " bbprop"); // timing debugging
  }
}

// narrow_module ///////////////////////////////////////////////////////////////

template <typename T>
narrow_module<T>::narrow_module(int dim_, intg size_, intg offset_,
                                bool narrow_tensors_)
    : module_1_1<T>("narrow_module"), dim(dim_), size(size_),
      narrow_tensors(narrow_tensors_) {
  this->bmstate_input = true; // this module takes multi-state inputs
  this->bmstate_output = true; // this module takes multi-state outputs
  offsets.push_back(offset_);
}

template <typename T>
narrow_module<T>::narrow_module(int dim_, intg size_,
                                std::vector<intg> &offsets_, bool states_,
                                const char *name_)
    : module_1_1<T>(name_),
      dim(dim_), size(size_), offsets(offsets_), narrow_tensors(states_) {
  this->bmstate_input = true; // this module takes multi-state inputs
  this->bmstate_output = true; // this module takes multi-state outputs
}

template <typename T>
narrow_module<T>::~narrow_module() {
}

template <typename T>
void narrow_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  // TODO: handle multiple offsets by copying narrows next to each other
  intg offset = offsets[0];
  out = in.narrow(dim, size, offset);
}

template <typename T>
void narrow_module<T>::fprop(state<T> &in, state<T> &out) {
  // narrow each tensor of multi-tensor in
  if (narrow_tensors) {
    if (out.order() != in.order()) out.reset(in);
    else out.resize(in);
    for (uint i = 0; i < in.x.size(); ++i)
      fprop1(in.x[i], out.x[i]);
  } else { // narrow multi-state itself
    if (dim == 0) { // narrow on states
      out.resize(in, offsets.size() * size);
      for (uint o = 0; o < offsets.size(); ++o) {
        intg offset = offsets[o];
        if ((intg) in.x.size() < offset + size)
          eblerror("expected at least " << offset + size
                   << " states in narrow of dimension "
                   << dim << " at offset " << offset << " to size " << size
                   << " but found only " << in.x.size() << " states");
        for (intg i = offset; i < offset + size; ++i)
          out.x[i - offset + o * size] = in.x[i];
      }
    } else eblerror("not implemented");
  }
  this->ninputs = in.x.size();
  this->noutputs = out.x.size();
  EDEBUG("narrowed " << in << " to " << out);
}

template <typename T>
void narrow_module<T>::bprop(state<T> &in, state<T> &out) {
  // TODO: assign states back to their input location?
}

template <typename T>
void narrow_module<T>::bbprop(state<T> &in, state<T> &out){
  // TODO: assign states back to their input location?
}

template <typename T>
std::string narrow_module<T>::describe() {
  std::string s;
  s << "narrow_module " << this->name() << " narrowing dimension " << dim
    << " to size " << size << " starting at offset(s) " << offsets;
  return s;
}

template <typename T>
module_1_1<T>* narrow_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*)
      new narrow_module<T>(dim, size, offsets,narrow_tensors);
}

template <typename T>
mfidxdim narrow_module<T>::fprop_size(mfidxdim &isize) {
  EDEBUG(this->name() << ": " << isize << " f-> ...");
  mfidxdim osize;
  if (narrow_tensors) {
    for (uint i = 0; i < isize.size(); ++i) {
      if (isize.exists(i)) {
        fidxdim d = isize[i];
        d.setdim(dim, size);
        osize.push_back_new(d);
      } else osize.push_back_empty();
    }
  } else {
    if (dim == 0) { // narrow on states
      osize.resize_default(offsets.size() * size);
      for (uint o = 0; o < offsets.size(); ++o) {
        intg offset = offsets[o];
        if ((intg) isize.size() < offset + size)
          eblerror("expected at least " << offset + size
                   << " states in narrow of dimension "
                   << dim << " at offset " << offset << " to size " << size
                   << " but found only " << isize.size() << " states");
        for (intg i = offset; i < offset + size; ++i)
          if (isize.exists(i)) osize.set(isize[i], i - offset + o * size);
      }
    } else eblerror("not implemented");
  }
  this->ninputs = isize.size();
  this->noutputs = osize.size();
  EDEBUG(this->name() << ": " << isize << " f-> " << osize);
  return osize;
}

template <typename T>
mfidxdim narrow_module<T>::bprop_size(mfidxdim &osize) {
  EDEBUG(this->name() << ": " << osize << " b-> ...");
  // eblwarn("temporary no bpropsize in narrow");
  // return osize;
  mfidxdim isize;
  uint offset = offsets[0];
  for (uint i = 0; i < offset; ++i)
    isize.push_back_empty();
  isize.push_back(osize);
  for (uint i = offset + size; i < this->ninputs; ++i)
    isize.push_back_empty();
  EDEBUG(this->name() << ": " << osize << " b-> " << isize);
  return isize;
}

// table_module ////////////////////////////////////////////////////////////////

template <typename T>
table_module<T>::table_module(std::vector<intg> &tbl, intg tot,
                              const char *name_)
    : module_1_1<T>(name_), table(tbl), total(tot) {
  this->bmstate_input = true; // this module takes multi-state inputs
  this->bmstate_output = true; // this module takes multi-state outputs
}

template <typename T>
table_module<T>::~table_module() {
}

template <typename T>
void table_module<T>::fprop(state<T> &in, state<T> &out) {
  out.x.clear();
  for (uint i = 0; i < table.size(); ++i) {
    intg k = table[i];
    if (k < 0 || k >= (intg) in.x.size())
      eblerror("trying to access index " << k << " in inputs " << in);
    out.x.push_back(in.x[k]);
  }
  this->ninputs = in.x.size();
  this->noutputs = out.x.size();
  EDEBUG(this->name() << ": mapped " << in << " to " << out);
}

template <typename T>
void table_module<T>::bprop(state<T> &in, state<T> &out) {
  EDEBUG(this->name() << " bprop: in: " << in);
  EDEBUG(this->name() << " bprop: out: " << out);
  // TODO: assign states back to their input location?
}

template <typename T>
void table_module<T>::bbprop(state<T> &in, state<T> &out){
  // TODO: assign states back to their input location?
}

template <typename T>
std::string table_module<T>::describe() {
  std::string s;
  s << "table_module " << this->name() << " with input list " << table;
  return s;
}

template <typename T>
module_1_1<T>* table_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*) new table_module<T>(table, total, this->name());
}

template <typename T>
mfidxdim table_module<T>::fprop_size(mfidxdim &isize) {
  mfidxdim osize;
  for (uint i = 0; i < table.size(); ++i) {
    intg k = table[i];
    if (k < 0 || k >= (intg) isize.size())
      eblerror("trying to access index " << k << " in inputs " << isize);
    osize.push_back(isize[k]);
  }
  return osize;
}

template <typename T>
mfidxdim table_module<T>::bprop_size(mfidxdim &osize) {
  mfidxdim isize;
  uint n = total;
  for (uint i = 0; i < table.size(); ++i)
    if (table[i] + 1 > n) n = table[i] + 1;
  for (uint i = 0; i < n; ++i)
    isize.push_back_empty();
  for (uint i = 0; i < table.size(); ++i) {
    intg k = table[i];
    if (osize.exists(i)) isize.set(osize[i], k);
  }
  EDEBUG(this->name() << ": " << osize << " b-> " << isize);
  return isize;
}

// network sizes methods ///////////////////////////////////////////////////////

template <typename T>
idxdim network_mindims(module_1_1<T> &m, uint order) {
  idxdim d;
  for (uint i = 0; i < order; ++i)
    d.insert_dim(0, 1);
  fidxdim fd = d;
  mfidxdim mm;
  mm.push_back_new(fd);
  mm = m.bprop_size(mm);
  if (mm.exists(0)) d = mm[0];
  return d;
}

template <typename T>
idxdim network_strides(module_1_1<T> &m, uint order) {
  fidxdim d;
  for (uint i = 0; i < order; ++i) d.insert_dim(0, 1);
  fidxdim d1 = d;
  for (uint i = 0; i < d1.order(); ++i) d1.setoffset(i, 1);
  mfidxdim mm, mm1;
  mm.push_back_new(d);
  mm1.push_back_new(d1);
  EDEBUG("netstride bprop 0: " << mm);
  mm = m.bprop_size(mm);
  EDEBUG("netstride bprop 1: " << mm1);
  mm1 = m.bprop_size(mm1);
  if (!mm.exists(0) || !mm1.exists(0))
    eblerror("expected dimensions in first element of " << mm
             << " and " << mm1);
  EDEBUG("bprop size results for netstride: " << mm << " and " << mm1);
  d = mm[0];
  d1 = mm1[0];
  idxdim s;
  for (uint i = 1; i < d.order(); ++i)
    s.insert_dim(i - 1, (intg) (d1.offset(i) - d.offset(i)));
  return s;
}

} // end namespace ebl
