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

namespace ebl {

// flat_merge_module ///////////////////////////////////////////////////////////

template <typename T>
flat_merge_module<T>::flat_merge_module(midxdim &ins_, mfidxdim &strides_,
                                        const char *name_, mfidxdim *scales_)
    : module_1_1<T>(name_), reference_scale(0) {
  if (scales_) scales = *scales_;
  else default_scales(ins_.size());
  // check there are enough elements
  if (ins_.size() < 1 || strides_.size() < 1
      || ins_.size() != strides_.size())
    eblerror("expected at least 1 dim and stride (matching) but got: dims: "
             << ins_.size() << " strides: " << strides_.size());
  // separate first dim/strides from rest
  din = ins_[0];
  din2 = ins_[0];
  stride = strides_[0];
  // add remaining ones
  for (uint i = 0; i < ins_.size(); ++i) {
    dins.push_back(ins_[i]);
    dins2.push_back(ins_[i]);
    strides.push_back(strides_[i]);
  }
}

template <typename T>
flat_merge_module<T>::~flat_merge_module() {
}

template <typename T>
void flat_merge_module<T>::fprop(state<T> &in, state<T> &out) {
  LOCAL_TIMING_START(); // profiling
  EDEBUG(this->name() << ": " << in << ", wins: " << dins << ", strides: "
         << strides << ", scales: " << scales);
  //    strides.clear();

  // output size for reference scale
  idxdim dref = compute_output_sizes(in, reference_scale);
  // check/pad output sizes for all non-reference scales to match the
  // reference output size
  for (uint k = 0; k < in.x.size(); ++k)
    if (k != reference_scale)
      compute_output_sizes(in, k, &dref);

  LOCAL_TIMING_REPORT("merge padding");

  // if (inputs.size() == 0)
  //   eblerror("no inputs to merge");
  // feature size for main input
  idx<T> &in0 = in.x[0];
  intg fsize = din.dim(0) * din.dim(1) * in0.dim(0);
  // number of possible windows
  // intg nh = 1 + (intg) ((in0.dim(1) - din.dim(0)) / stride.dim(0));
  // intg nw = 1 + (intg) ((in0.dim(2) - din.dim(1)) / stride.dim(1));
  intg nh = dref.dim(0), nw = dref.dim(1);
  // compute new size and resize output if necessary
  for (uint i = 1; i < in.x.size(); ++i) {
    idxdim &d = dins[i];
    fidxdim &s = strides[i];
    idx<T> &input = in.x[i];
    fsize += d.nelements() * input.dim(0);
    // check that strides match possible windows
    intg nh2 = (intg) ceil((input.dim(1) - d.dim(0) + 1)
                           / std::max(10e-9, (double) s.dim(0)));
    intg nw2 = (intg) ceil((input.dim(2) - d.dim(1) + 1)
                           / std::max(10e-9, (double) s.dim(1)));
    if (nh2 < nh || nw2 < nw) {
      *(this->mout) << "COUT input " << input << " and window " << d
                    << " with stride " <<s << " produce " << nh2 << "x" << nw2
                    << " outputs but expected at least " << nh << "x" << nw
                    << std::endl;
      eblerror("input " << input << " and window " << d << " with stride " <<s
               << " produce " << nh2 << "x" << nw2
               << " outputs but expected at least " << nh << "x" << nw);
    } else if (nh2 != nh || nw2 != nw)
      EDEBUG("warning: input " << input << " and window " << d
             << " with stride " << s  << " produce " << nh2 << "x" << nw2
             << ", ignoring extra cells and using only " <<nh << "x" << nw);
    EDEBUG("input " << i << " " << input << ", min " << idx_min(input)
           << " max " << idx_max(input));
  }
  LOCAL_TIMING_REPORT("merge check");
  idxdim d(fsize, nh, nw);
  if (!out.same_dim(d)) {
    if (out.order() != d.order())
      out = state<T>(d);
    else
      out.resize(d);
  }
  LOCAL_TIMING_REPORT("merge resize");
  idx_clear(out);
  LOCAL_TIMING_REPORT("merge clear of " << out);

  intg offset = 0;
  int h = 0, w = 0;
  float fh, fw;
  uint uh = 0, uw = 0, uw0 = 0;
  idx<T> iw, ow, onarrowed, inarrowed;
  // copy inputs to out
  for (uint i = 0; i < in.x.size(); ++i) {
    idxdim dd = dins[i];
    intg dd0 = dd.dim(0), dd1 = dd.dim(1);
    fidxdim s = strides[i];
    float s0 = s.dim(0), s1 = s.dim(1);
    idx<T> &input = in.x[i];
    if (!input.contiguousp()) eblerror("expected contiguous");
    fsize = dd.nelements() * input.dim(0); // feature size from input
    onarrowed = out.narrow(0, fsize, offset);
    h = 0; w = 0;
    intg wmod = onarrowed.mod(2);
    // copy
    for (h = 0, fh = 0; h < nh; h++, fh += s0) {
      uh = (uint) fh;
      // select 1 output pixel in the correct feature range
      ow = onarrowed.select(2, 0);
      ow = ow.select(1, h);
      inarrowed = input.narrow(1, dd0, uh);
      intg iwmod = inarrowed.mod(2);
      uw = 0; uw0 = 0;
      iw = inarrowed.narrow(2, dd1, uw);
      for (w = 0, fw = 0; w < nw; ++w, fw += s1) {
        // integer positions
        uw = (uint) fw;
        // select input window
        if (uw != uw0)
          iw.add_offset(iwmod);
        // copy flat input to output
        // TODO: tmp buffer less efficient than direct copy which but requires
        // continuous data, make idx pointing to oo with flat's dims?
        // idx<T> tmp(iw.get_idxdim());
        // idx_copy(iw, tmp);
        // iw = tmp.view_as_order(1);
        idx_copy(iw, ow);

        uw0 = uw;
        ow.add_offset(wmod);
      }
    }
    offset += fsize;
  }

  LOCAL_TIMING_REPORT("merge copies");

#ifdef __DEBUG_PRINT__
  // eblprint( describe() << ": " << in0 << " (in " << din
  //      << " stride " << stride << ")");
  // for (uint i = 1; i < in.x.size(); ++i)
  //   eblprint( " + " << in[i]
  //        << " (in " << dins[i] << " stride " << strides[i] << ")");
  // eblprint( " -> " << out << std::endl);
  // eblprint( "output min: " << idx_min(out) << " max: " << idx_max(out)
  //      << std::endl);
#endif
}

template <typename T>
void flat_merge_module<T>::bprop(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx<T> o, input, o1 = out.dx[0].view_as_order(1);
  intg offset = 0;
  // copy out to inputs
  for (uint i = 0; i < in.x.size(); ++i) {
    input = in.dx[i];
    input = input.view_as_order(1);
    o = o1.narrow(0, input.nelements(), offset);
    idx_add(o, input, input);
    offset += input.nelements();
  }
}

template <typename T>
void flat_merge_module<T>::bbprop(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx<T> o, input, o1 = out.ddx[0].view_as_order(1);
  intg offset = 0;
  // copy out to inputs
  for (uint i = 0; i < in.x.size(); ++i) {
    input = in.ddx[i];
    input = input.view_as_order(1);
    o = o1.narrow(0, input.nelements(), offset);
    idx_add(o, input, input);
    offset += input.nelements();
  }
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
fidxdim flat_merge_module<T>::fprop_size(fidxdim &isize) {
  // feature size for main input
  intg fsize = (intg) (din2.dim(0) * din2.dim(1) * isize.dim(0));
  // number of possible windows
  intg nh = 1 + (intg) ((isize.dim(1) - din2.dim(0)) / stride.dim(0));
  intg nw = 1 + (intg) ((isize.dim(2) - din2.dim(1)) / stride.dim(1));
  //! Extract its dimensions, update output size
  fidxdim osize(fsize, std::max((intg) 1, nh),
                std::max((intg) 1, nw));
  isize = bprop_size(osize);
  return osize;
}

template <typename T>
mfidxdim flat_merge_module<T>::fprop_size(mfidxdim &isize) {
  EDEBUG(this->name() << ": " << isize << " f-> ...");
  mfidxdim osize;
  if (isize.exists(0)) {
    fidxdim d = isize[0];
    d = this->fprop_size(d);
    osize.push_back(d);
  }
  EDEBUG(this->name() << ": " << isize << " f-> " << osize);
  return osize;
}

template <typename T>
fidxdim flat_merge_module<T>::bprop_size(const fidxdim &osize) {
  EDEBUG(this->name() << ": " << osize << " -> ...");
  // feature size for main input
  intg fsize = (intg) (osize.dim(0) / din2.dim(0) / din2.dim(1));
  // number of possible windows
  intg ih = (intg) (((osize.dim(1) - 1) * stride.dim(0)) + din2.dim(0));
  intg iw = (intg) (((osize.dim(2) - 1) * stride.dim(1)) + din2.dim(1));
  // extract its dimensions, update output size
  fidxdim isize(fsize, ih, iw);
  // set offsets
  for (uint j = 1; j < isize.order(); ++j)
    isize.setoffset(j, (intg) (osize.offset(j) * stride.dim(j - 1)));
  return isize;
}

template <typename T>
mfidxdim flat_merge_module<T>::bprop_size(mfidxdim &osize) {
  EDEBUG(this->name() << ": " << osize << " b-> ...");
  if (osize.size() == 0) eblerror("expected at least 1 idxdim");
  mfidxdim isize;
  idxdim pa, d;
  fidxdim s;
  // all inputs
  //float pix1 = .5;
  for (uint i = 0; i < dins2.size(); ++i) {
    if (!osize.exists(0)) {
      isize.push_back_empty();
      continue ;
    }
    idxdim o0 = osize[0];
    d = dins2[i];
    s = strides[i];
    d.insert_dim(0, o0.dim(0)); // add feature dimension
    // set offsets
    fidxdim fd(d);
    for (uint j = 1; j < d.order(); ++j) {
      float o = (o0.offset(j) + d.offset(j)) * s.dim(j - 1);
      if (j-1 < pa.order()) o -= pa.dim(j-1);
      fd.setoffset(j, o);
    }
    isize.push_back(fd);
  }
  EDEBUG(this->name() << ": " << osize << " -> " << isize);
  return isize;
}

template <typename T>
std::string flat_merge_module<T>::describe() {
  std::string desc;
  desc << "flat_merge module " << this->name() << ", merging "
       << (int) dins.size() << " inputs: ";
  for (uint i = 0; i < dins.size(); ++i) {
    desc << " (in " << dins[i] << " stride " << strides[i];
    if (i < scales.size()) desc << " scale " << scales[i];
    desc << "), ";
  }
  return desc;
}

template <typename T>
uint flat_merge_module<T>::get_ninputs() {
  return (uint) dins.size();
}

template <typename T>
mfidxdim flat_merge_module<T>::get_strides() {
  return strides;
}

template <typename T>
mfidxdim flat_merge_module<T>::get_scales() {
  return scales;
}

template <typename T>
module_1_1<T>* flat_merge_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*)
      new flat_merge_module<T>(dins, strides, this->name(), &scales);
}

template <typename T>
void flat_merge_module<T>::set_offsets(std::vector<std::vector<int> > &off) {
  offsets = off;
  EDEBUG(this->name() << ": setting offsets to " << offsets);
}

template <typename T>
void flat_merge_module<T>::set_strides(mfidxdim &s) {
  strides = s;
  fidxdim ref(1, 1);
  for (uint i = 0; i < strides.size(); ++i)
    if (ref == strides[i]) reference_scale = i;
  eblprint( this->name() << ": setting strides to " << strides
            << " with reference scale " << reference_scale << " with stride: "
            << strides[reference_scale] << std::endl);
}

// protected /////////////////////////////////////////////////////////////////

template <typename T>
idxdim flat_merge_module<T>::
compute_pad(idxdim &window, float subsampling, float edge,
            float scale, fidxdim &stride) {
  float hoff = (edge * scale * stride.dim(0)) / subsampling + .5;
  float woff = (edge * scale * stride.dim(1)) / subsampling + .5;
  idxdim d = window;
  d.setdim(0, (int) (d.dim(0) + hoff * 2));
  d.setdim(1, (int) (d.dim(1) + woff * 2));
  return d;
}

template <typename T>
void flat_merge_module<T>::default_scales(uint n) {
  scales.clear();
  fidxdim f(1, 1);
  for (uint i = 0; i < n; ++i) scales.push_back_new(f);
}

template <typename T>
idxdim flat_merge_module<T>::compute_output_sizes(state<T> &in, uint k,
                                                  idxdim *dref) {
  state<T> i = in.x[k];
  idxdim d = dins[k];
  fidxdim &s = strides[k];
  // compute number of outputs for this kernel
  idxdim dout((intg) ((i.dim(1) - d.dim(0) + 1) / s.dim(0)),
              (intg) ((i.dim(2) - d.dim(1) + 1) / s.dim(1)));
  std::string msg;
  msg << this->name() << ": in " << to_string(i.x) << " (min: " << idx_min(i)
      << ", max: " << idx_max(i) << ") with window " << d
      << " and stride " << s << " -> " << dout;
  EDEBUG(msg);
  if (dout.dim(0) <= 0 || dout.dim(1) <= 0)
    eblerror("input is too small for this network: " << msg);
  return dout;
}

// linear_merge_module /////////////////////////////////////////////////////////

template <typename T>
linear_merge_module<T>::
linear_merge_module(parameter<T> *p, intg nout_, midxdim &ins_,
                    mfidxdim &strides_, const char *name_, mfidxdim *scales_)
    : flat_merge_module<T>(ins_, strides_, name_, scales_),
      dins_original(ins_), nout(nout_) {
  // allocate convolutions for each state
  idxdim cstride(1, 1);
  for (uint i = 0; i < ins_.size(); ++i) {
    idxdim in = ins_[i];
    intg f = in.remove_dim(0);
    idx<intg> table = full_table(f, nout);
    std::string cname;
    cname << "linear_merge_conv" << i;
    convs.push_back(new convolution_module<T>(p, in, cstride, table,
                                              cname.c_str()));
    buffers1.x.push_back(new idx<T>);
    // set original window sizes
    dins2[i] = in;
    if (i == 0) din2 = in;
    // set inputs windows of 1x1 for flat_merge
    in.setdims(1);
    dins[i] = in;
    if (i == 0) din = in;
  }
}

template <typename T>
linear_merge_module<T>::~linear_merge_module() {
  for (uint i = 0; i < convs.size(); ++i) delete convs[i];
}

template <typename T>
void linear_merge_module<T>::fprop(state<T> &in, state<T> &out) {
  LOCAL_TIMING_START(); // profiling
  EDEBUG(this->name() << " (linear merge): " << in << ", wins: " << dins2
         << ", strides: " << strides << ", scales: " << scales);

  if (in.x.size() != convs.size())
    eblerror("expected " << convs.size() << " inputs but got " << in.x.size());
  // linear combinations
  for (uint i = 0; i < convs.size(); ++i)
    convs[i]->fprop1(in.x[i], buffers1.x[i]);
  // combine each state
  flat_merge_module<T>::fprop(buffers1, buffer2);
  // add states together
  idxdim d(buffer2);
  intg thick = d.dim(0) / dins.size();
  d.setdim(0, thick);
  this->resize_output(in, out, &d);
  out.zero_x();
  for (uint i = 0; i < dins.size(); ++i) {
    idx<T> slice = buffer2.narrow(0, thick, i * thick);
    idx_add(slice, out);
  }
  LOCAL_TIMING_REPORT("linear merge");
}

template <typename T>
void linear_merge_module<T>::bprop(state<T> &in, state<T> &out) {
  eblerror("not implemented");
}

template <typename T>
void linear_merge_module<T>::bbprop(state<T> &in, state<T> &out) {
  eblerror("not implemented");
}

template <typename T>
void linear_merge_module<T>::fprop_dump(state<T> &in, state<T> &out) {
  for (uint i = 0; i < in.x.size(); ++i)
    DUMP(in.x[i], this->name() << "_linear_merge_module_in[" << i << "]");
  fprop(in, out);
  DUMP(out.x[0], this->name() << "_linear_merge_module_out");
}

template <typename T>
std::string linear_merge_module<T>::describe() {
  std::string desc;
  desc << "linear_merge module " << this->name() << ", merging "
       << (int) dins_original.size() << " inputs: ";
  for (uint i = 0; i < dins_original.size(); ++i) {
    desc << " (in " << dins_original[i] << " stride " << strides[i];
    if (i < scales.size()) desc << " scale " << scales[i];
    desc << "), ";
  }
  return desc;
}

template <typename T>
module_1_1<T>* linear_merge_module<T>::copy(parameter<T> *p) {
  linear_merge_module<T> *l2 =
      new linear_merge_module<T>(p, nout, dins_original, strides,
                                 this->name(), &scales);
  // assign same parameter state if no parameters were specified
  if (!p) {
    for (uint i = 0; i < convs.size(); ++i)
      l2->convs[i]->kernel = convs[i]->kernel;
  }
  return (module_1_1<T>*) l2;
}

// mstate_merge_module /////////////////////////////////////////////////////////

template <typename T>
mstate_merge_module<T>::
mstate_merge_module(midxdim &ins, mfidxdim &strides, const char *name_)
    : module_1_1<T>(name_), dins(ins), dstrides(strides) {
}

template <typename T>
mstate_merge_module<T>::~mstate_merge_module() {
}

template <typename T>
void mstate_merge_module<T>::fprop(state<T> &in, state<T> &out) {
  // use state 0 as base for sizes
  state<T> &in0 = in[0];
  state<T> o0 = out[0];
  idxdim &d0 = dins[0];
  fidxdim &s0 = dstrides[0];
  // number of possible windows
  intg nh = (intg) (1 + (in0.dim(1) - d0.dim(0)) / s0.dim(0));
  intg nw = (intg) (1 + (in0.dim(2) - d0.dim(1)) / s0.dim(1));
  // compute new size and resize output if necessary
  intg fsize = 0;
  for (uint i = 0; i < dins.size(); ++i) {
    idxdim &d = dins[i];
    fidxdim &s = dstrides[i];
    state<T> &tin = in[i];
    fsize += d.nelements() * tin.dim(0);
    // check that strides match possible windows
    if (tin.dim(1) / s.dim(0) != nh || tin.dim(2) / s.dim(1) != nw)
      eblerror("input " << tin << " with stride " << s
               << " does not produce " << nh << "x" << nw << " windows");
  }
  // resize output (only 1 state)
  idxdim d(fsize, nh, nw);
  if (out.size() != 1) {
    out.clear();
    out.push_back(d);
  } else {
    if (!o0.same_dim(d))
      o0.resize(d);
  }
  intg offset = 0;
  // copy all inputs to outputs
  for (uint i = 0; i < dins.size(); ++i) {
    idxdim &d = dins[i];
    fidxdim &s = dstrides[i];
    state<T> &tin = in[i];
    // feature size for this state
    fsize = d.nelements() * in0.dim(0);
    // loop on all possible windows for this state
    float fh, fw;
    uint uh, uw, h, w;
    for (h = 0, fh = 0; h < nh; h++, fh += s.dim(0)) {
      for (w = 0, fw = 0; w < nw; w++, fw += s.dim(1)) {
        // integer positions
        uh = (uint) h;
        uw = (uint) w;
        // select 1 output pixel in the corect feature range
        idx<T> o = o0.select(2, w);
        o = o.select(1, h);
        o = o.narrow(0, fsize, offset);
        // select input window
        idx<T> iw = tin.select(2, uw);
        iw = iw.select(1, uh);
        // copy flat input to output
        // TODO: tmp buffer less efficient than direct copy which but requires
        // continuous data, make idx pointing to oo with flat's dims?
        idx<T> tmp(iw.get_idxdim());
        idx_copy(iw, tmp);
        iw = tmp.view_as_order(1);
        idx_copy(iw, o);
      }
    }
    offset += fsize;
  }
#ifdef __DEBUG_PRINT__
  // eblprint( describe() << ": " << in << " (in " << din
  // 	 << " stride " << stride << ")");
  // for (uint i = 0; i < inputs.size(); ++i)
  //   eblprint( " + " << (*inputs[i])->x << " (in " << dins[i]
  // 		 << " stride " << strides[i] << ")");
  // eblprint( " -> " << out << std::endl);
#endif
}

template <typename T>
void mstate_merge_module<T>::bprop(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  // expect only 1 state in output
  if (out.size() != 1)
    eblerror("expected only 1 state in output but found " << out.size());
  state<T> &to = out[0];
  idx<T> o = to.dx[0].view_as_order(1);
  // copy out to inputs
  intg offset = 0;
  for (uint i = 0; i < in.size(); ++i) {
    state<T> &tin = in[i];
    idx<T> ii = tin.dx[0].view_as_order(1);
    idx<T> oo = o.narrow(0, ii.nelements(), offset);
    idx_add(oo, ii, ii);
    offset += ii.nelements();
  }
}

template <typename T>
void mstate_merge_module<T>::bbprop(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  // expect only 1 state in output
  if (out.size() != 1)
    eblerror("expected only 1 state in output but found " << out.size());
  state<T> &to = out[0];
  idx<T> o = to.ddx[0].view_as_order(1);
  // copy out to inputs
  intg offset = 0;
  for (uint i = 0; i < in.size(); ++i) {
    state<T> &tin = in[i];
    idx<T> ii = tin.ddx[0].view_as_order(1);
    idx<T> oo = o.narrow(0, ii.nelements(), offset);
    idx_add(oo, ii, ii);
    offset += ii.nelements();
  }
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
idxdim mstate_merge_module<T>::fprop_size(idxdim &isize) {
  // use state 0 as base for sizes
  idxdim &d0 = dins[0];
  fidxdim &s0 = dstrides[0];
  // number of possible windows
  intg nh = (intg) (1 + (isize.dim(1) - d0.dim(0)) / s0.dim(0));
  intg nw = (intg) (1 + (isize.dim(2) - d0.dim(1)) / s0.dim(1));
  // compute new size and resize output if necessary
  intg fsize = 0;
  for (uint i = 0; i < dins.size(); ++i) {
    idxdim &d = dins[i];
    fsize += d.nelements() * isize.dim(0);
  }
  //! Extract its dimensions, update output size
  idxdim osize(fsize, std::max((intg) 1, nh),
               std::max((intg) 1, nw));
  fidxdim os = osize;
  isize = bprop_size(os);
  return osize;
}

template <typename T>
fidxdim mstate_merge_module<T>::bprop_size(const fidxdim &osize) {
  // use state 0 as base for sizes
  fidxdim &d0 = dins[0];
  fidxdim &s0 = dstrides[0];
  // number of possible windows
  intg ih = (intg) (((osize.dim(1) - 1) * s0.dim(0)) + d0.dim(0));
  intg iw = (intg) (((osize.dim(2) - 1) * s0.dim(1)) + d0.dim(1));
  // compute new size and resize output if necessary
  intg fsize = osize.dim(0) / d0.dim(0) / d0.dim(1);
  //! Extract its dimensions, update output size
  fidxdim isize(fsize, ih, iw);
  return isize;
}

template <typename T>
std::string mstate_merge_module<T>::describe() {
  std::string desc;
  desc << "mstate_merge module " << this->name() << ", merging states ";
  for (uint i = 0; i < dins.size(); ++i)
    desc << " (in " << dins[i] << " stride " << dstrides[i] << "), ";
  return desc;
}

// merge ///////////////////////////////////////////////////////////////////////

template <typename T>
merge_module<T>::merge_module(std::vector<std::vector<uint> > &states,
                              intg concat_dim_, const char *name_)
    : module_1_1<T>(name_), states_list(states), concat_dim(concat_dim_){
  this->ninputs = states[0].size();
}

template <typename T>
merge_module<T>::~merge_module() {
}

template <typename T>
void merge_module<T>::fprop(state<T> &in, state<T> &out) {
  if (states_list.size() == 0) eblerror("expected non-empty states_list");
  // resize out if necessary
  if (out.order() != in.order()) out.reset(in.get_idxdim(), states_list.size());
  else out.resize(in.get_idxdim(), states_list.size());
  // loop on each merging
  for (uint i = 0; i < states_list.size(); ++i) {
    std::vector<uint> ids = states_list[i];
    state<T> mi;
    mi.clear_all();
    // create multi-state of states to merge
    for (uint j = 0; j < ids.size(); ++j) {
      uint id = ids[j];
      if (id >= in.x.size())
        eblerror("trying to access state " << id << " but multi-state only "
                 << "contains " << in.x.size() << " states: " << in);
      mi.add_x_new(in.x[id]);
    }
    EDEBUG("merging states with ids " << ids << ": " << mi);
    // merge them
    merge_f(mi, out, i);
  }
}

template <typename T>
void merge_module<T>::bprop(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  // loop on each merging
  for (uint i = 0; i < states_list.size(); ++i) {
    std::vector<uint> ids = states_list[i];
    state<T> mi;
    // create multi-state of states to merge
    for (uint j = 0; j < ids.size(); ++j) {
      uint id = ids[j];
      if (id >= in.x.size())
        eblerror("trying to access state " << id << " but multi-state only "
                 << "contains " << in.x.size() << " states: " << in);
      mi.dx.push_back(in.dx[id]);
    }
    // merge them
    merge_b(mi, out, i);
  }
}

template <typename T>
void merge_module<T>::bbprop(state<T> &in, state<T> &out){
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  // loop on each merging
  for (uint i = 0; i < states_list.size(); ++i) {
    std::vector<uint> ids = states_list[i];
    state<T> mi;
    // create multi-state of states to merge
    for (uint j = 0; j < ids.size(); ++j) {
      uint id = ids[j];
      if (id >= in.x.size())
        eblerror("trying to access state " << id << " but multi-state only "
                 << "contains " << in.x.size() << " states: " << in);
      mi.ddx.push_back(in.ddx[id]);
    }
    // merge them
    merge_bb(mi, out, i);
  }
}

template <typename T>
void merge_module<T>::fprop_dump(state<T> &in, state<T> &out) {
  fprop(in, out);
}

template <typename T>
std::string merge_module<T>::describe() {
  std::string desc;
  desc << "merge module " << this->name();
  return desc;
}

template <typename T>
module_1_1<T>* merge_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*)
      new merge_module<T>(states_list, concat_dim, this->name());
}

template <typename T>
mfidxdim merge_module<T>::fprop_size(mfidxdim &isize) {
  EDEBUG(this->name() << ": " << isize << " f-> ...");
  mfidxdim osize;
  if (isize.exists(0)) {
    fidxdim d = isize[0];
    d.setdim(concat_dim, 0);
    for (uint i = 0; i < isize.size(); ++i)
      d.setdim(concat_dim, d.dim(concat_dim) + isize[i].dim(concat_dim));
    osize.push_back(d);
  }
  this->ninputs = isize.size();
  this->noutputs = osize.size();
  EDEBUG(this->name() << ": " << isize << " f-> " << osize);
  return osize;
}

template <typename T>
mfidxdim merge_module<T>::bprop_size(mfidxdim &osize) {
  EDEBUG(this->name() << ": " << osize << " b-> ...");
  mfidxdim isize;
  for (uint i = 0; i < this->ninputs; ++i) isize.push_back_new(osize);
  EDEBUG(this->name() << ": " << osize << " b-> " << isize);
  return isize;
}

// internal members //////////////////////////////////////////////////////////

template <typename T>
void merge_module<T>::merge_f(state<T> &in, state<T> &out, uint iout) {
  if (in.x.size() == 0) eblerror("expected at least 1 state in input");
  idxdim d(in), dtmp(in);
  // check that all inputs are compatible and compute output size
  for (uint i = 1; i < in.x.size(); ++i) {
    idx<T> &s = in.x[i];
    dtmp.setdim(concat_dim, s.dim(concat_dim));
    if (!s.same_dim(dtmp))
      eblerror("expected same dimensions but got " << s.get_idxdim()
               << " and " << dtmp);
    // increment dimension
    d.setdim(concat_dim, d.dim(concat_dim) + s.dim(concat_dim));
  }
  // check that output has the right size, if not, resize
  if (out.get_idxdim() != d) out.resize(d);
  // copy inputs to out
  intg offset = 0;
  idx<T> o;
  for (uint i = 0; i < in.x.size(); ++i) {
    idx<T> &s = in.x[i];
    o = out.x[iout].narrow(concat_dim, s.dim(concat_dim), offset);
    idx_copy(s, o);
    offset += s.dim(concat_dim);
  }
#ifdef __DEBUG_PRINT__
  eblprint( describe() << ": " << in.x[0]);
  for (uint i = 1; i < in.x.size(); ++i) eblprint( " + " << in.x[i]);
  eblprint( " -> " << out.x[iout] << std::endl);
#endif
}

template <typename T>
void merge_module<T>::merge_b(state<T> &in, state<T> &out, uint iout) {
  // accumulate outputs to inputs
  intg offset = 0;
  idx<T> o;
  for (uint i = 0; i < in.dx.size(); ++i) {
    idx<T> &s = in.dx[i];
    o = out.dx[iout].narrow(concat_dim, s.dim(concat_dim), offset);
    idx_add(o, s, s);
    offset += s.dim(concat_dim);
  }
#ifdef __DEBUG_PRINT__
  eblprint( describe() << ": bprop " << in.dx[0]);
  for (uint i = 1; i < in.dx.size(); ++i) eblprint( " + " << in.dx[i]);
  eblprint( " <- " << out.dx[iout] << std::endl);
#endif
}

template <typename T>
void merge_module<T>::merge_bb(state<T> &in, state<T> &out, uint iout) {
  // accumulate outputs to inputs
  intg offset = 0;
  idx<T> o;
  for (uint i = 0; i < in.ddx.size(); ++i) {
    idx<T> &s = in.ddx[i];
    o = out.ddx[iout].narrow(concat_dim, s.dim(concat_dim), offset);
    idx_add(o, s, s);
    offset += s.dim(concat_dim);
  }
#ifdef __DEBUG_PRINT__
  eblprint( describe() << ": bbprop " << in.ddx[0]);
  for (uint i = 1; i < in.ddx.size(); ++i) eblprint( " + " << in.ddx[i]);
  eblprint( " <- " << out.ddx[iout] << std::endl);
#endif
}

// interlace ///////////////////////////////////////////////////////////////////

template <typename T>
interlace_module<T>::interlace_module(uint stride_, const char *name_)
    : module_1_1<T>(name_), stride(stride_) {
}

template <typename T>
interlace_module<T>::~interlace_module() {
}

template <typename T>
void interlace_module<T>::
fprop(state<T> &in, state<T> &out) {
  if (in.x.size() % stride != 0)
    eblerror("expected number of states to be a multiple of " << stride
             << " but got: " << in);
  out.x.clear();
  // interlace
  for (uint i = 0; i < stride; ++i) {
    for (uint j = 0; j < in.x.size() / stride; ++j) {
      out.x.push_back(in.x[j * stride + i]);
    }
  }
  EDEBUG(this->name() << ": " << in << " -> " << out);
}

template <typename T>
void interlace_module<T>::bprop(state<T> &in, state<T> &out) {
  not_implemented();
}

template <typename T>
void interlace_module<T>::bbprop(state<T> &in, state<T> &out) {
  not_implemented();
}

template <typename T>
mfidxdim interlace_module<T>::bprop_size(mfidxdim &osize) {
  if (osize.size() % stride != 0) {
    eblwarn(this->name() << ": expected midxdim size to be a multiple of "
            << stride << " but got " << osize);
    return osize;
  }
  mfidxdim isize;
  uint step = osize.size() / stride;
  // interlace
  for (uint i = 0; i < step; ++i) {
    for (uint j = 0; j < stride; ++j) {
      if (osize.exists(j * step + i))
        isize.push_back(osize[j * step + i]);
      else
        isize.push_back_empty();
    }
  }
  EDEBUG(this->name() << ": " << osize << " -> " << isize);
  return isize;
}

template <typename T>
std::string interlace_module<T>::describe() {
  std::string desc;
  desc << "interlacing module " << this->name() << " with stride " << stride;
  return desc;
}

template <typename T>
module_1_1<T>* interlace_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*) new interlace_module<T>(stride, this->name());
}

} // end namespace ebl
