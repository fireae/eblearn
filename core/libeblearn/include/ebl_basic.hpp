/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun, Pierre Sermanet and Soumith Chintala *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com, soumith@gmail.com  *
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
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

namespace ebl {

// linear_module ///////////////////////////////////////////////////////////////

template <typename T>
linear_module<T>::linear_module(parameter<T> *p, intg in, intg out,
                                const char *name_)
    : module_1_1<T>(name_), w(out, in, p) {
}

template <typename T>
linear_module<T>::~linear_module() {
}

template <typename T>
void linear_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  //!\note The in must be contiguous
  //!\note The out must be contiguous
  CHECK_CONTIGUOUS2(in, out);
  // flatten dimensions starting from second one
  idxdim d(in);
  d.remove_dim(0);
  idx<T> inx(in.getstorage(), 0, in.dim(0), d.nelements());
  d.insert_dim(0, w.dim(0));
  this->resize_output(in, out, &d); // resize (iff necessary)
  idx<T> outx(out.getstorage(), 0, out.dim(0), inx.dim(1));

  // linear combination
  idx_m2dotm2(w, inx, outx);
}

template <typename T>
void linear_module<T>::bprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  idx<T> inx = in.flat(), indx = in.dx[0].flat(), outdx = out.dx[0].flat();
  idx<T> twx(w.transpose(0, 1)); // switch dimensions 0 and 1
  if (outdx.nelements() != w.dx[0].dim(0))
    eblerror("output should have " << w.dx[0].dim(0) << " elements "
             << "but has " << outdx.nelements() << " (" << outdx << ")");

  // bprop
  idx_m1extm1acc(outdx, inx, w.dx[0]); // backprop to weights
  idx_m2dotm1acc(twx, outdx, indx); // backprop to input
}

template <typename T>
void linear_module<T>::bbprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  idx<T> inx = in.flat(), inddx = in.ddx[0].flat(), outddx = out.ddx[0].flat();
  idx<T> twx = w.transpose(0, 1); // switch dimensions 0 and 1
  if (outddx.nelements() != w.ddx[0].dim(0))
    eblerror("output should have " << w.ddx[0].dim(0) << " elements "
             << "but has " << outddx.nelements() << " (" << outddx << ")");

  // bbprop
  idx_m1squextm1acc(outddx, inx, w.ddx[0]); // backprop to weights
  idx_m2squdotm1acc(twx, outddx, inddx); // backprop to input
}

template <typename T>
void linear_module<T>::forget(forget_param_linear &fp) {
  double fanin_ = w.dim(1);
  double z = fp.value / pow(fanin_, fp.exponent);
  idx_aloop1(lx, w, T) {
    *lx = (T) fp.generator.drand(-z, z);
  }
}

template <typename T>
void linear_module<T>::normalize() {
#ifdef __CBLAS__
  norm_columns(w);
#else
  eblerror("norm_columns not implemented without cblas");
#endif
}

template <typename T>
fidxdim linear_module<T>::fprop1_size(fidxdim &isize) {
  //! Update output size based on weight dimensions
  fidxdim osize = isize;
  osize.setdim(0, w.dim(0));
  isize = bprop1_size(osize);
  return osize;
}

template <typename T>
fidxdim linear_module<T>::bprop1_size(const fidxdim &osize) {
  fidxdim isize = osize;
  isize.setdim(0, w.dim(1));
  return isize;
}

template <typename T>
module_1_1<T>* linear_module<T>::copy(parameter<T> *p) {
  // new module
  linear_module<T> *l2 =
      new linear_module<T>(p, w.dim(1), w.dim(0), this->name());
  // assign same parameter state if no parameters were specified
  if (!p) l2->w = w;
  return (module_1_1<T>*)l2;
}

template <typename T>
void linear_module<T>::load_x(idx<T> &weights) {
  if (!w.same_dim(weights)) {
    // if sizes are the same except for the feature size, load
    // into the corresponding slices with a warning
    // this allows to load grayscale pretrained weights only
    // in a grayscale + color net for example.
    idxdim d(w);
    d.setdim(0, weights.dim(0));
    if (d == weights.get_idxdim()) {
      eblwarn("loading weights partly (the first " << d.dim(0)
              << " features) from " << weights << " instead of entire weights ("
              << w.x[0] << ")");
      intg sz = std::min(w.dim(0), weights.dim(0));
      idx<T> slices = w.narrow(0, sz, 0);
      idx<T> w = weights.narrow(0, sz, 0);
      idx_copy(w, slices);
    } else
      eblthrow("expected same dimension weights but got " << w.x[0] << " and "
               << weights << " instead in " << this->name());
  } else
    idx_copy(weights, w);
}

template <typename T>
std::string linear_module<T>::describe() {
  std::string desc;
  desc << "linear module " << this->name() << " "
       << w.dim(1) << " -> " << w.dim(0);
  return desc;
}

template <typename T>
void linear_module<T>::fprop1_dump(idx<T> &in, idx<T> &out) {
  DUMP(in, this->name() << "_linear_module_in");
  fprop1(in, out);
  DUMP(w.x[0], this->name() << "_linear_module_weights");
}

// convolution_module //////////////////////////////////////////////////////////

template <typename T>
convolution_module<T>::
convolution_module(parameter<T> *p, idxdim &ker_, idxdim &stride_,
                   idx<intg> &tbl, const char *name_, bool crop_)
    : module_1_1<T>(name_), ker(ker_), stride(stride_), table(tbl),
      warnings_shown(false), float_precision(false), double_precision(false),
      crop(crop_), use_ipp(false)  {
  this->default_name("convolution");
  idxdim d(ker);
  d.insert_dim(0, tbl.dim(0));
  kernel = state<T>(d, p);
  // check sanity of connection table
  if (table.dim(1) != 2) // check table order
    eblerror("error: expecting table with dim 1 equal to 2 but found: "
             << table);
  check_table_duplicates(table);
  idx<intg> tbl0 = table.select(1, 0);
  tablemax = idx_max(tbl0);
  idx<intg> tbl1 = table.select(1, 1);
  thickness = idx_max(tbl1) + 1;
  // check table uses all inputs
  idx<bool> tblcount(tablemax + 1);
  idx_bloop1(tb, table, intg) {
    tblcount.set(true, tb.get(0));
  }
  bool not_using_all_inputs = false;
  for (int i = 0; i <= tablemax; ++i) {
    if (tblcount.get(i) == false) {
      eblwarn("input " << i
              << " not used by connection table in convolution module");
      not_using_all_inputs = true;
    }
  }
  fulltable = false;
  // check if its a full-table
  if ( (((tablemax + 1) * thickness) == table.dim(0)) && !not_using_all_inputs)
    fulltable = true;

#if __TH__
  // check precision to decide if we use TH or not
  fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&kernel);
  if (cont) {
    float_precision = true;
    outtmp = idx<T>(1, 1);
  }
  else {
    fstate_idx<double> *cont_d = dynamic_cast<fstate_idx<double>*>(&kernel);
    if(cont_d) {
      double_precision = true;
      outtmp = idx<T>(1, 1);
    }
  }
#else
#ifdef __IPP__
  // check precision to decide if we use IPP or not
  fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&kernel);
  if (cont) {
    float_precision = true;
    // allocate reversed kernel
    revkernel = idx<T>(kernel.dim(1), kernel.dim(2));
    outtmp = idx<T>(1, 1);
  }
  ipp_err_printed = false;
  use_ipp = true;
#endif
#endif
  // for external display classes, declare which internal buffers to display
  this->internals.push_back(kernel);
  this->internals_str.push_back("kernels");
}

template <typename T>
convolution_module<T>::~convolution_module() {
}

template <typename T>
void convolution_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  if (!convolution_module<T>::resize_output(in, out))
    return ; // do nothing if resizing failed
  EDEBUG_MAT(this->name() << ": kernel", kernel);
  EDEBUG_MAT(this->name() << ": table", table);
  // temporarly crop input if mismatch in size
  intg ki = kernel.dim(1), kj = kernel.dim(2);
  intg si = stride.dim(0), sj = stride.dim(1);
  intg oi = in.dim(1) - (ki - si), oj = in.dim(2) - (kj - sj);
  if (crop && oi % stride.dim(0) != 0)
    in = in.narrow(1, in.dim(1) - oi % si, 0);
  if (crop && oj % stride.dim(1) != 0)
    in = in.narrow(2, in.dim(2) - oj % sj, 0);
#ifdef __TH__
  // a direct 3D-map optimization
  if((float_precision || double_precision) && in.order()==3) {
#ifdef __OPENMP__
    svector< idx<T> > lk_list, suin_list, sout_list;
    svector<idx<T> > outtmp_list;
    std::vector< idx<intg> > lt_list;
    idx_bloop2(lk, kernel, T, lt, table, intg) {
      lk_list.push_back(new idx<T>(lk));
      lt_list.push_back(lt);
      suin_list.push_back(new idx<T>(in.select(0, lt.get(0))));
      sout_list.push_back(new idx<T>((out).select(0, lt.get(1))));
      // need own outtmp variable for parallelization
      outtmp_list.push_back(new idx<T>(outtmp.get_idxdim()));
    }
    intg i;
    intg num_outputs = lk_list.size();
#pragma omp parallel for private(i)
    for ( i = 0; i < num_outputs; ++i) {
      // 2D convolution
      th_convolution(suin_list[i], lk_list[i], outtmp_list[i],
                     stride.dim(0), stride.dim(1));
    } // end of for loop
#pragma omp parallel for private(i)
    for ( i = 0; i < out.dim(0); ++i) {
      for(int j=0; j < lt_list.size(); j++) {
        if(lt_list[j].get(1) == i)
          th_add(outtmp_list[i],sout_list[i]);
      }
    }
#else
    th_convolution_3dmap(in, kernel, out, table, stride.dim(0), stride.dim(1));
#endif // endif __OPENMP__
    return;
  }
  else {
    // unfolding input for a faster convolution operation
    idx<T> uuin(in.unfold(1, kernel.dim(1), stride.dim(0)));
    uuin = uuin.unfold(2, kernel.dim(2), stride.dim(1));
    idx_bloop2(lk, kernel, T, lt, table, intg) {
      idx<T> sout((out).select(0, lt.get(1)));
      idx<T> suin(uuin.select(0, lt.get(0)));
      idx_m4dotm2acc(suin, lk, sout); // 2D convolution
    }
    return;
  }
#endif // endif __TH__

  LOCAL_TIMING_START();
  // unfolding input for a faster convolution operation
  idx<T> uuin(in.unfold(1, kernel.dim(1), stride.dim(0)));
  uuin = uuin.unfold(2, kernel.dim(2), stride.dim(1));
  idx_clear(out);
  // convolve 2D slice for each convolution kernel
  { idx_bloop2(lk, kernel, T, lt, table, intg) {
      idx<T> sout((out).select(0, lt.get(1)));
#ifdef __IPP__
      if (float_precision && use_ipp) {
        rev_idx2_tr(lk, revkernel);
        //		idx_clear(outtmp);
        idx<T> suin(in.select(0, lt.get(0)));
        ipp_convolution(suin, revkernel, outtmp);
        ipp_add(outtmp, sout);
      } else { // not using IPP
        idx<T> suin(uuin.select(0, lt.get(0)));
        idx_m4dotm2acc(suin, lk, sout); // 2D convolution
      }
#else
      idx<T> suin(uuin.select(0, lt.get(0)));
      idx_m4dotm2acc(suin, lk, sout); // 2D convolution
#endif //endif __IPP__
    }
  }
  LOCAL_TIMING_REPORT("convcpu total time");
}

template <typename T>
void convolution_module<T>::bprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DX(kernel); // in debug mode, check backward tensors are allocated
  // temporarly crop input if mismatch in size
  idx<T> inx = in, indx = in.dx[0];
  intg ki = kernel.dim(1), kj = kernel.dim(2);
  intg si = stride.dim(0), sj = stride.dim(1);
  intg oi = inx.dim(1) - (ki - si), oj = inx.dim(2) - (kj - sj);
  if (crop && oi % stride.dim(0) != 0) {
    inx = inx.narrow(1, inx.dim(1) - oi % si, 0);
    indx = indx.narrow(1, inx.dim(1) - oi % si, 0);
  }
  if (crop && oj % stride.dim(1) != 0) {
    inx = inx.narrow(2, inx.dim(2) - oj % sj, 0);
    indx = indx.narrow(2, inx.dim(2) - oj % sj, 0);
  }
#ifdef __TH__
  if ((float_precision || double_precision) && in.order() == 3) {
    idx_clear(indx);
    th_convolution_3dmap_bprop(inx, kernel, out.dx[0], indx,
                               kernel.dx[0], table,
                               stride.dim(0), stride.dim(1));
    return;
  }
  else {
    // use the regular method
    // backprop through convolution
    idx<T> uuin(indx.unfold(1, kernel.dx[0].dim(1), stride.dim(0)));
    uuin = uuin.unfold(2, kernel.dx[0].dim(2), stride.dim(1));
    idx<T> uuinf(inx.unfold(1, kernel.dx[0].dim(1), stride.dim(0)));
    uuinf = uuinf.unfold(2, kernel.dx[0].dim(2), stride.dim(1));
    int transp[5] = { 0, 3, 4, 1, 2 };
    idx<T> borp(uuinf.transpose(transp));
    { idx_bloop3 (lk, kernel.dx[0], T, lkf, kernel, T,
                  lt, table, intg) {
        intg islice = lt.get(0);
        idx<T> suin(uuin.select(0, islice));
        idx<T> sborp(borp.select(0, islice));
        idx<T> sout(out.dx[0].select(0, lt.get(1)));
        idx_m2extm2acc(sout, lkf, suin); // backward convolution
        idx_m4dotm2acc(sborp, sout, lk); // compute gradient for kernel
      }
    }
    return;
  }
#else
  // backprop through convolution
  idx<T> uuin(indx.unfold(1, kernel.dx[0].dim(1), stride.dim(0)));
  uuin = uuin.unfold(2, kernel.dx[0].dim(2), stride.dim(1));
  idx<T> uuinf(inx.unfold(1, kernel.dx[0].dim(1), stride.dim(0)));
  uuinf = uuinf.unfold(2, kernel.dx[0].dim(2), stride.dim(1));
  int transp[5] = { 0, 3, 4, 1, 2 };
  idx<T> borp(uuinf.transpose(transp));
  { idx_bloop3 (lk, kernel.dx[0], T, lkf, kernel, T,
                lt, table, intg) {
      intg islice = lt.get(0);
      idx<T> suin(uuin.select(0, islice));
      idx<T> sborp(borp.select(0, islice));
      idx<T> sout(out.dx[0].select(0, lt.get(1)));
#ifdef __IPP__
      if (float_precision && use_ipp)
        idx_m2extm2acc(sout, revkernel, suin); // backward convolution
      else
        idx_m2extm2acc(sout, lkf, suin); // backward convolution
      idx_m4dotm2acc(sborp, sout, lk); // compute gradient for kernel
#else
      idx_m2extm2acc(sout, lkf, suin); // backward convolution
      idx_m4dotm2acc(sborp, sout, lk); // compute gradient for kernel
#endif //IPP
    }}
#endif //TH
}

template <typename T>
void convolution_module<T>::bbprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DDX(kernel); // in debug mode, check backward tensors are allocated
  EDEBUG_MAT(this->name() << ": kernel.ddx ", kernel.ddx[0]);
  // temporarly crop input if mismatch in size
  idx<T> inx = in, inddx = in.ddx[0];
  intg ki = kernel.dim(1), kj = kernel.dim(2);
  intg si = stride.dim(0), sj = stride.dim(1);
  intg oi = inx.dim(1) - (ki - si), oj = inx.dim(2) - (kj - sj);
  if (crop && oi % stride.dim(0) != 0) {
    inx = inx.narrow(1, inx.dim(1) - oi % si, 0);
    inddx = inddx.narrow(1, inx.dim(1) - oi % sj, 0);
  }
  if (crop && oj % stride.dim(1) != 0) {
    inx = inx.narrow(2, inx.dim(2) - oj % si, 0);
    inddx = inddx.narrow(2, inx.dim(2) - oj % sj, 0);
  }
  // backprop through convolution
  idx<T> uuin(inddx.unfold(1, kernel.ddx[0].dim(1), stride.dim(0)));
  uuin = uuin.unfold(2, kernel.ddx[0].dim(2), stride.dim(1));
  idx<T> uuinf(inx.unfold(1, kernel.ddx[0].dim(1), stride.dim(0)));
  uuinf = uuinf.unfold(2, kernel.ddx[0].dim(2), stride.dim(1));
  int transp[5] = { 0, 3, 4, 1, 2 };
  idx<T> borp(uuinf.transpose(transp));
  { idx_bloop3 (lk, kernel.ddx[0], T, lkf, kernel, T,
                lt, table, intg) {
      intg islice = lt.get(0);
      idx<T> suin(uuin.select(0, islice));
      idx<T> sborp(borp.select(0, islice));
      idx<T> sout((out.ddx[0]).select(0, lt.get(1)));

#ifdef __IPP__
      if (float_precision && use_ipp)
        idx_m2squextm2acc(sout, revkernel, suin); // backward convolution
      else
        idx_m2squextm2acc(sout, lkf, suin); // backward convolution
      idx_m4squdotm2acc(sborp, sout, lk); // compute gradient for kernel
#else
      idx_m2squextm2acc(sout, lkf, suin); // backward convolution
      idx_m4squdotm2acc(sborp, sout, lk); // compute gradient for kernel
#endif
    }}
  EDEBUG_MAT(this->name() << ": kernel.ddx ", kernel.ddx[0]);
}

template <typename T>
void convolution_module<T>::forget(forget_param_linear &fp) {
  idx<T> kx(kernel);
  intg vsize = kx.dim(1);
  intg hsize = kx.dim(2);
  idx<intg> ts(table.select(1, 1));
  idx<int> fanin_(1 + idx_max(ts));
  idx_clear(fanin_);
  { idx_bloop1(tab, table, intg)	{
      fanin_.set(1 + fanin_.get(tab.get(1)), tab.get(1)); }}
  { idx_bloop2(tab, table, intg, x, kx, T) {
      double s = fp.value / pow((vsize * hsize * fanin_.get(tab.get(1))),
                                fp.exponent);
      { idx_bloop1(lx, x, T) {
          { idx_bloop1(llx, lx, T) {
              T n = (T) fp.generator.drand(-s, s);
              llx.set(n);
            }}
        }}
    }}
}

template <typename T>
bool convolution_module<T>::resize_output(idx<T> &in, idx<T> &out,
                                          idxdim *ignore) {
  if (!this->bresize) return false;
  TIMING_RESIZING_ACCSTART(); // start accumulating resizing time
  intg ki = kernel.dim(1), kj = kernel.dim(2);
  intg si = stride.dim(0), sj = stride.dim(1);
  // check input size for table
  if (in.dim(0) < tablemax + 1)
    eblerror("error: expecting input with size " << tablemax + 1
             << " in dimension 0 but found: " << in);
  if (!warnings_shown && (in.dim(0) > tablemax + 1)) {
    warnings_shown = true;
    eblwarn( "warning: convolution connection table is not using all inputs "
              << "in layer " << this->name() << " the maximum input index used by "
              << "the table is " << tablemax << " but the input is "
             << in << std::endl);
  }
  // check sizes
  if (!crop && (((in.dim(1) - (ki - si)) % si != 0) ||
                ((in.dim(2) - (kj - sj)) % sj != 0)))
    eblerror("inconsistent input size, kernel size, and subsampling ratio");
#ifdef __IPP__
  if ((stride.dim(0) != 1) || (stride.dim(1) != 1)) {
    use_ipp = false;
    if (!ipp_err_printed) {
      // eblwarn( "Warning: not using IPP in " << this->name()
      //      << " because stride > 1 not implemented for IPP" << std::endl);
      ipp_err_printed = true;
    }
  } else use_ipp = true;
#endif
  idx<T> inx = in;
  idxdim d(in.spec); // use same dimensions as in
  intg oi = inx.dim(1) - (ki - si), oj = inx.dim(2) - (kj - sj);
  intg ii = inx.dim(1) - oi % si, jj = inx.dim(2) - oj % sj;
  // if kernel is smaller than input, just resize output to 1 blank pixel
  if (ii == 0 || jj == 0 || ki > inx.dim(1) || kj > inx.dim(2)) {
    d.setdims(1);
    d.setdim(0, thickness);
    out = idx<T>(d);
    return false;
  }
  // crop input if mismatch in size
  if (crop && oi % stride.dim(0) != 0)
    inx = inx.narrow(1, ii, 0);
  if (crop && oj % stride.dim(1) != 0)
    inx = inx.narrow(2, jj, 0);
  // unfolding input for a faster convolution operation
  idx<T> uuin(inx.unfold(1, ki, si));
  uuin = uuin.unfold(2, kj, sj);
  // resize output based in input dimensions
  d.setdim(0, thickness); // except for the first one
  d.setdim(1, uuin.dim(1)); // convolution trims dimensions a bit
  d.setdim(2, uuin.dim(2)); // convolution trims dimensions a bit
  if (out.get_idxdim() != d) { // resize only if necessary
    EDEBUG(this->name() << ": resizing output from " << out << " to " << d);
#ifdef __TH__
    if (float_precision || double_precision) {
      outtmp.resize(d.dim(1), d.dim(2));
    }
#else
#ifdef __IPP__
    if (float_precision && use_ipp) {
      outtmp.resize(d.dim(1), d.dim(2));
    }
#endif
#endif
    if (out.order() != d.order()) out = idx<T>(d);
    else out.resize(d);
  }
  TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
  return true;
}

template <typename T>
fidxdim convolution_module<T>::fprop1_size(fidxdim &isize) {
  fidxdim osize = isize;
  if (osize.empty()) return osize;
  // features dimension
  osize.setdim(0, thickness);
  // update spatial dimensions
  for (uint i = 1; i < isize.order(); ++i)
    osize.setdim(i, std::max((float) 1, isize.dim(i) - kernel.dim(i) + 1));
  //! Recompute the input size to be compliant with the output
  isize = bprop1_size(osize);
  return osize;
}

template <typename T>
fidxdim convolution_module<T>::bprop1_size(const fidxdim &osize) {
  fidxdim isize = osize;
  // features dimension
  isize.setdim(0, thickness);
  // spatial dimensions
  for (uint i = 1; i < osize.order(); ++i) {
    isize.setdim(i, osize.dim(i) + kernel.dim(i) - 1);
    //      isize.setoffset(i, osize.offset(i) - kernel.dim(i) / 2);
  }
  return isize;
}

template <typename T>
module_1_1<T>* convolution_module<T>::copy(parameter<T> *p) {
  convolution_module<T> *l2 =
      new convolution_module<T>(p, ker, stride, table, this->name());
  if (!p) // assign same parameter state if no parameters were specified
    l2->kernel = kernel;
  return (module_1_1<T>*) l2;
}

template <typename T>
void convolution_module<T>::load_x(idx<T> &weights) {
  if (!kernel.same_dim(weights)) {
    // if sizes are the same except for the feature size, load
    // into the corresponding slices with a warning
    // this allows to load grayscale pretrained weights only
    // in a grayscale + color net for example.
    idxdim d(kernel);
    d.setdim(0, weights.dim(0));
    if (d == weights.get_idxdim()) {
      eblwarn("loading weights partly (the first " << d.dim(0)
              << " features) from " << weights << " instead of entire weights ("
              << kernel.x[0] << ")");
      intg sz = std::min(kernel.dim(0), weights.dim(0));
      idx<T> slices = kernel.narrow(0, sz, 0);
      idx<T> w = weights.narrow(0, sz, 0);
      idx_copy(w, slices);
    } else
      eblthrow("expected same dimension weights but got " << kernel.x[0]
               << " and " << weights << " instead in " << this->name());
  } else
    idx_copy(weights, kernel);
}

template <typename T>
std::string convolution_module<T>::describe() {
  std::string desc;
  desc << "convolution module " << this->name() << " with " << kernel.dim(0)
       << " kernels with size " << ker << ", stride " << stride
       << " and table " << table << " (" << tablemax+1 << "->" << thickness
       << ")";
  return desc;
}

template <typename T>
void convolution_module<T>::fprop1_dump(idx<T> &in, idx<T> &out) {
  DUMP(in, this->name() << "_convolution_module_in");
  DUMP(kernel.x[0], this->name() << "_convolution_module_ker");
  DUMP(table, this->name() << "_convolution_module_table");
  fprop1(in, out);
  DUMP(out, this->name() << "_convolution_module_out");
}

// addc_module /////////////////////////////////////////////////////////////////

template <typename T>
addc_module<T>::addc_module(parameter<T> *p, intg size, const char *name_)
    : module_1_1<T>(name_), bias(size, p) {
}

template <typename T>
addc_module<T>::~addc_module() {
}

template <typename T>
void addc_module<T>::fprop1(idx<T>& in, idx<T>& out) {
  if (&in != &out) { // resize only when input and output are different
    idxdim d(in.spec); // use same dimensions as in
    d.setdim(0, bias.dim(0)); // except for the first one
    this->resize_output(in, out, &d); // resize iff necessary
  }
  // add each bias to entire slices cut from the first dimension
  idx_bloop3(inx, in, T, biasx, bias, T, outx, out, T) {
    idx_addc(inx, biasx.get(), outx);
  }
}

template <typename T>
void addc_module<T>::bprop1(state<T>& in, state<T>& out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  if (&in != &out) idx_checknelems2_all(in.dx[0], out.dx[0]);

  idx_bloop3(indx, in.dx[0], T, biasdx, bias.dx[0], T,
             outdx, out.dx[0], T) {
    if (&in != &out) // only pass on info if necessary
      idx_add(outdx, indx, indx); // accumulate gradients to input
    idx_sumacc(outdx, biasdx); // accumulate gradients to weights
  }
}

template <typename T>
void addc_module<T>::bbprop1(state<T>& in, state<T>& out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  if (&in != &out) idx_checknelems2_all(in.ddx[0], out.ddx[0]);

  idx_bloop3(inddx, in.ddx[0], T, biasddx, bias.ddx[0], T,
             outddx, out.ddx[0], T) {
    if (&in != &out) // only pass on info if necessary
      idx_add(outddx, inddx, inddx); // accumulate 2nd gradients to input
    idx_sumacc(outddx, biasddx); // accumulate 2nd gradients to weights
  }
}

template <typename T>
void addc_module<T>::forget(forget_param_linear& fp) {
  idx_clear(bias);
}

template <typename T>
module_1_1<T>* addc_module<T>::copy(parameter<T> *p) {
  // new module (with its own local parameter buffers)
  addc_module<T> *l2 =
      new addc_module<T>(p, bias.dim(0), this->name());
  // assign same parameter state if no parameters were specified
  if (!p) l2->bias = bias;
  return (module_1_1<T>*) l2;
}

template <typename T>
void addc_module<T>::load_x(idx<T> &weights) {
  if (!bias.same_dim(weights)) {
    // if sizes are the same except for the feature size, load
    // into the corresponding slices with a warning
    // this allows to load grayscale pretrained weights only
    // in a grayscale + color net for example.
    idxdim d(bias);
    d.setdim(0, weights.dim(0));
    if (d == weights.get_idxdim()) {
      eblwarn("loading weights partly (the first " << d.dim(0)
              << " features) from " << weights
              << " instead of entire weights (" << bias.x[0] << ")");
      intg sz = std::min(bias.dim(0), weights.dim(0));
      idx<T> slices = bias.narrow(0, sz, 0);
      idx<T> w = weights.narrow(0, sz, 0);
      idx_copy(w, slices);
    } else
      eblthrow("expected same dimension weights but got " << bias.x[0]
               << " and " << weights << " instead in " << this->name());
  } else
    idx_copy(weights, bias);
}

template <typename T>
std::string addc_module<T>::describe() {
  std::string desc;
  desc << "bias module " << this->name() << " with "
       << bias.dim(0) << " biases";
  return desc;
}

template <typename T>
void addc_module<T>::fprop1_dump(idx<T>& in, idx<T>& out) {
  DUMP(in, this->name() << "_addc_module_in");
  DUMP(bias.x[0], this->name() << "_addc_module_weights");
  fprop1(in, out);
  DUMP(out, this->name() << "_addc_module_out");
}

// power_module ////////////////////////////////////////////////////////////////

template <typename T>
power_module<T>::power_module(T p_, const char *name_)
    : module_1_1<T>(name_), p(p_) {}

template <typename T>
power_module<T>::~power_module() {}

template <typename T>
void power_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize iff necessary
  EDEBUG(this->name() << " fprop in " << in << " out " << out);
  // #ifdef __TH__
  //     th_pow(in, out, p);
  // #else
  idx_power(in, p, out);
  // #endif
}

template <typename T>
void power_module<T>::bprop1(state<T> &in, state<T> &out) {
  EDEBUG(this->name() << " bprop in " << in << " out " << out);
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  idx_checknelems2_all(in.dx[0], out.dx[0]); // must have same dimensions

  if (!tt.same_dim(in.get_idxdim())) { // resize temp buffer
    idxdim d(in);
    tt = idx<T>(d);
  }
  // compute derivative
  // #ifdef __TH__
  //     th_pow(in, tt, p - 1);
  // #else
  idx_power(in, p - 1, tt);
  //#endif
  idx_mul(out.dx[0], tt, tt); //! tt = outdx*x^(p-1)
  idx_dotcacc(tt, p, in.dx[0]);
}

template <typename T>
void power_module<T>::bbprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  idx_checknelems2_all(in.ddx[0], out.ddx[0]); // must have same dimensions

  if (!tt.same_dim(in.get_idxdim())) { // resize temp buffer
    idxdim d(in);
    tt = idx<T>(d);
  }
  // compute 2nd derivative
  // #ifdef __TH__
  //     th_pow(in, tt, p - 1);
  // #else
  idx_power(in, p - 1, tt);
  //#endif
  idx_mul(tt, tt, tt); //! tt = (x^(p-1))^2
  idx_mul(out.ddx[0], tt, tt); //! tt = outddx*(x^(p-1))^2
  idx_dotcacc(tt, (p * p), in.ddx[0]);
}

// diff_module /////////////////////////////////////////////////////////////////

template <typename T>
diff_module<T>::diff_module() : module_2_1<T>("diff") {
}

template <typename T>
diff_module<T>::~diff_module() {
}

template <typename T>
void diff_module<T>::fprop1(idx<T> &in1, idx<T> &in2, idx<T> &out) {
  this->resize_output(in1, in2, out); // resize iff necessary
  idx_sub(in1, in2, out);
}

template <typename T>
void diff_module<T>::bprop1(state<T> &in1, state<T> &in2, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DX(in2); // in debug mode, check backward tensors are allocated
  state_idx_check_different3(in1, in2, out); // forbid same in and out
  idx_checknelems3_all(in1.dx[0], in2.dx[0], out.dx[0]); // must have same dims
  // backprop
  idx_add(out.dx[0], in1.dx[0]); // derivative wrt in1
  idx_minus_acc(out.dx[0], in2.dx[0]); // derivative wrt in2
}

template <typename T>
void diff_module<T>::bbprop1(state<T> &in1, state<T> &in2, state<T> &out) {
  DEBUG_CHECK_DDX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DDX(in2); // in debug mode, check backward tensors are allocated
  state_idx_check_different3(in1, in2, out); // forbid same in and out
  idx_checknelems3_all(in1.ddx[0], in2.ddx[0], out.ddx[0]); // must have same dims
  // backprop
  idx_add(out.ddx[0], in1.ddx[0]); // derivative wrt in1
  idx_add(out.ddx[0], in2.ddx[0]); // derivative wrt in2
}

// mul_module //////////////////////////////////////////////////////////////////

template <typename T>
mul_module<T>::mul_module() : module_2_1<T>("mul") {
}

template <typename T>
mul_module<T>::~mul_module() {
}

template <typename T>
void mul_module<T>::fprop1(idx<T> &in1, idx<T> &in2, idx<T> &out) {
  this->resize_output(in1, in2, out); // resize iff necessary
  idx_mul(in1, in2, out);
}

template <typename T>
void mul_module<T>::bprop1(state<T> &in1, state<T> &in2, state<T> &out) {
  DEBUG_CHECK_DX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DX(in2); // in debug mode, check backward tensors are allocated
  state_idx_check_different3(in1, in2, out); // forbid same in and out
  idx_checknelems3_all(in1.dx[0], in2.dx[0], out.dx[0]);// must have same dimensions

  if (!tmp.same_dim(in1.get_idxdim())) { // resize temp buffer
    idxdim d(in1);
    tmp = idx<T>(d);
  }
  idx_mul(out.dx[0], in2, tmp);
  idx_add(tmp, in1.dx[0]);
  idx_mul(out.dx[0], in1, tmp);
  idx_add(tmp, in2.dx[0]);
}

template <typename T>
void mul_module<T>::bbprop1(state<T> &in1, state<T> &in2, state<T> &out) {
  DEBUG_CHECK_DDX(in1); // in debug mode, check backward tensors are allocated
  DEBUG_CHECK_DDX(in2); // in debug mode, check backward tensors are allocated
  state_idx_check_different3(in1, in2, out); // forbid same in and out
  idx_checknelems3_all(in1.ddx[0], in2.ddx[0], out.ddx[0]);// must have same dimensions

  if (!tmp.same_dim(in1.get_idxdim())) { // resize temp buffer
    idxdim d(in1);
    tmp = idx<T>(d);
  }
  idx_mul(in2, in2, tmp);
  idx_mul(out.ddx[0], tmp, tmp);
  idx_add(tmp, in1.ddx[0]);
  idx_mul(in1, in1, tmp);
  idx_mul(out.ddx[0], tmp, tmp);
  idx_add(tmp, in2.ddx[0]);
}

// thres_module ////////////////////////////////////////////////////////////////

template <typename T>
thres_module<T>::thres_module(T thres_, T val_, const char *name_)
    : module_1_1<T>(name_), thres(thres_), val(val_) {
}

template <typename T>
thres_module<T>::~thres_module() {
}

template <typename T>
void thres_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize iff necessary
  idx_aloop2(inx, in, T, outx, out, T) {
    *outx = (*inx > thres) ? *inx : val;
  }
}

template <typename T>
void thres_module<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  idx_checknelems2_all(in.dx[0], out.dx[0]); // must have same dimensions
  // backprop
  idx_aloop3(inx, in, T, indx, in.dx[0], T, outdx, out.dx[0], T) {
    if (*inx > thres)
      *indx += *outdx;
  }
}

template <typename T>
void thres_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  idx_checknelems2_all(in.ddx[0], out.ddx[0]); // must have same dimensions
  // backprop
  idx_add(out.ddx[0], in.ddx[0]);
}

template <typename T>
module_1_1<T>* thres_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*) new thres_module<T>(thres, val);
}

template <typename T>
std::string thres_module<T>::describe() {
  std::string desc;
  desc << "threshold module " << this->name() << " sets any value below "
       << thres << " to " << val;
  return desc;
}

// cutborder_module ////////////////////////////////////////////////////////////

template <typename T>
cutborder_module<T>::cutborder_module(int nr_, int nc_)
    : module_1_1<T>("cutborder"), nrow(nr_), ncol(nc_) {
}

template <typename T>
cutborder_module<T>::~cutborder_module() {
}

template <typename T>
void cutborder_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  intg inr = in.dim(1);
  intg inc = in.dim(2);
  intg outr = inr - 2 * nrow;
  intg outc = inc - 2 * ncol;
  idxdim d(in.dim(0), outr, outc);
  this->resize_output(in, out, &d); // resize iff necessary
  out.zero_all();
  idx<T> tmp = in.narrow(1, outr, nrow);
  tmp = tmp.narrow(2, outc, ncol);
  idx_copy(tmp, out);
}

template <typename T>
void cutborder_module<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  // backprop
  intg inr = out.dim(1);
  intg inc = out.dim(2);
  idx<T> tmp = in.dx[0].narrow(1, inr, nrow);
  tmp = tmp.narrow(2, inc, ncol);
  idx_add(out.dx[0], tmp);
}

template <typename T>
void cutborder_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  // backprop
  intg inr = out.dim(1);
  intg inc = out.dim(2);
  idx<T> tmp = in.ddx[0].narrow(1, inr, nrow);
  tmp = tmp.narrow(2, inc, ncol);
  idx_add(out.ddx[0], tmp);
}

// zpad_module /////////////////////////////////////////////////////////////////

template <typename T>
zpad_module<T>::zpad_module(const char *name_) : module_1_1<T>(name_) {
  set_paddings(0, 0, 0, 0);
}

template <typename T>
zpad_module<T>::zpad_module(int nr, int nc, const char *name_)
  : module_1_1<T>(name_) {
  set_paddings(nr, nc, nr, nc);
}

template <typename T>
zpad_module<T>::zpad_module(int top, int left, int bottom, int right)
    : module_1_1<T>("zpad") {
  set_paddings(top, left, bottom, right);
}

template <typename T>
zpad_module<T>::zpad_module(idxdim &kerdims, const char *name_)
    : module_1_1<T>(name_) {
  set_kernel(kerdims);
}

template <typename T>
zpad_module<T>::zpad_module(midxdim &kerdims, const char *name_)
    : module_1_1<T>(name_) {
  set_kernels(kerdims);
}

template <typename T>
zpad_module<T>::~zpad_module() {
}

template <typename T>
void zpad_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  int top = pad.dim(0), left = pad.dim(1);
  idx<T> input = in;
  if (&in == &out) { // same buffers, use a temporary copy
    if (!input.same_dim(in)) input = idx<T>(in.get_idxdim());
    idx_copy(in, input); // only copy forward
  }
  this->resize_output(in, out); // resize iff necessary
  idx<T> tmp = out.narrow(1, input.dim(1), top);
  tmp = tmp.narrow(2, input.dim(2), left);
  idx_copy(input, tmp);
}

template <typename T>
bool zpad_module<T>::resize_output(idx<T> &in, idx<T> &out, idxdim *ignore) {
  int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
  idxdim d(in.dim(0), in.dim(1) + top + bottom, in.dim(2) + left + right);
  bool ret = module_1_1<T>::resize_output(in, out, &d); // resize iff necessary
  idx_clear(out);
  return ret;
}

template <typename T>
void zpad_module<T>::bprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
  // if in and out are the same, we just want to crop the buffers
  // by the extra padding that was added by the fprop
  if (&in == &out) {
    // crop state
    state<T> tmp = in.narrow_state(1, out.dim(1) - top - bottom, top);
    tmp = tmp.narrow_state(2, out.dim(2) - left - right, left);
    in = tmp;
  } else { // different buffers, accumulate gradients to input
    idx<T> tmp = out.dx[0].narrow(1, in.dim(1), top);
    tmp = tmp.narrow(2, in.dim(2), left);
    idx_add(tmp, in.dx[0]);
  }
}

template <typename T>
void zpad_module<T>::bbprop1(state<T> &in, state<T> &out) {
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
  // if in and out are the same, we just want to crop the buffers
  // by the extra padding that was added by the fprop
  if (&in == &out) {
    // crop state
    state<T> tmp = in.narrow_state(1, out.dim(1) - top - bottom, top);
    tmp = tmp.narrow_state(2, out.dim(2) - left - right, left);
    in = tmp;
  } else { // different buffers, accumulate gradients to input
    idx<T> tmp = out.ddx[0].narrow(1, in.dim(1), top);
    tmp = tmp.narrow(2, in.dim(2), left);
    idx_add(tmp, in.ddx[0]);
  }
}

template <typename T>
idxdim zpad_module<T>::get_paddings() {
  return pad;
}

template <typename T>
idxdim zpad_module<T>::get_paddings(idxdim &ker) {
  int top = (int) (floor(ker.dim(0) / (float) 2.0));
  int left = (int) (floor(ker.dim(1) / (float) 2.0));
  int bottom = top;
  int right = left;
  // remove 1 pixel on right and bottom borders if even.
  if (ker.dim(0) % 2 == 0) bottom -= 1;
  if (ker.dim(1) % 2 == 0) right -= 1;
  return idxdim(top, left, bottom, right);
}

template <typename T>
midxdim zpad_module<T>::get_paddings(midxdim &kers) {
  midxdim p;
  for (uint i = 0; i < kers.size(); ++i) {
    idxdim &ker = kers[i];
    ker = get_paddings(ker);
    p.push_back_new(ker);
  }
  return p;
}

template <typename T>
void zpad_module<T>::set_paddings(int top, int left, int bottom,
                                  int right) {
  pads.clear();
  pad = idxdim(top, left, bottom, right);
  pads.push_back_new(pad);
}

template <typename T>
void zpad_module<T>::set_paddings(idxdim &pads_) {
  if (pads_.order() != 4)
    eblerror("expected a 4-dim idxdim but got: " << pads_);
  pads.clear();
  pads.push_back(pads_);
  pad = pads_;
}

template <typename T>
void zpad_module<T>::set_kernel(idxdim &ker) {
  if (ker.maxdim() == 0) {
    eblwarn("no padding for kernel " << ker);
    return ;
  }
  idxdim p = get_paddings(ker);
  set_paddings(p);
}

template <typename T>
void zpad_module<T>::set_kernels(midxdim &kers) {
  intg mx = 0;
  for (uint i = 0; i < kers.size(); ++i) {
    idxdim &k = kers[i];
    if (k.maxdim() > mx) mx = k.maxdim();
  }
  if (mx == 0) {
    eblwarn("no padding for kernels " << kers);
    return ;
  }
  pads = get_paddings(kers);
  pad = pads[0];
}

template <typename T>
fidxdim zpad_module<T>::fprop1_size(fidxdim &isize) {
  int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
  fidxdim osize = isize;
  osize.setdim(1, isize.dim(1) + top + bottom);
  osize.setdim(2, isize.dim(2) + left + right);
  //! Recompute the input size to be compliant with the output
  isize = bprop1_size(osize);
  return osize;
}

template <typename T>
fidxdim zpad_module<T>::bprop1_size(const fidxdim &osize) {
  int top = pad.dim(0), left = pad.dim(1);
  fidxdim isize = osize;
  isize.setoffset(1, osize.offset(1) - top);
  isize.setoffset(2, osize.offset(2) - left);
  return isize;
}

template <typename T>
mfidxdim zpad_module<T>::fprop_size(mfidxdim &isize) {
  mfidxdim osize;
  for (uint i = 0; i < isize.size(); ++i) {
    if (i < pads.size()) pad = pads[i];
    if (isize.exists(i)) {
      fidxdim s = fprop1_size(isize[i]);
      osize.push_back(s);
    } else osize.push_back_empty();
  }
  //EDEBUG(this->name() << ": " << isize << " f-> " << osize);
  return osize;
}

template <typename T>
mfidxdim zpad_module<T>::bprop_size(mfidxdim &osize) {
  mfidxdim isize;
  for (uint i = 0; i < osize.size(); ++i) {
    if (i < pads.size()) pad = pads[i];
    if (osize.exists(i)) {
      fidxdim s = bprop1_size(osize[i]);
      isize.push_back(s);
    } else isize.push_back_empty();
  }
  EDEBUG(this->name() << ": " << osize << " -> " << isize);
  return isize;
}

template <typename T>
std::string zpad_module<T>::describe() {
  std::string desc;
  desc << "zpad module " << this->name() << " is padding with: "
       << pads;
  return desc;
}

template <typename T>
module_1_1<T>* zpad_module<T>::copy(parameter<T> *p) {
  zpad_module<T> *z = new zpad_module<T>(this->name());
  z->pads = pads;
  return (module_1_1<T>*) z;
}

// mirrorpad_module ////////////////////////////////////////////////////////////

template <typename T>
mirrorpad_module<T>::mirrorpad_module(int nr, int nc, const char *name_)
    : zpad_module<T>(nr, nc, name_) {
}

template <typename T>
mirrorpad_module<T>::mirrorpad_module(idxdim &kernel, const char *name_)
    : zpad_module<T>(kernel, name_) {
}

template <typename T>
mirrorpad_module<T>::~mirrorpad_module() {
}

template <typename T>
void mirrorpad_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
  idx<T> input = in;
  idxdim d(input.dim(0), input.dim(1) + top + bottom,
           input.dim(2) + left + right);
  if (&in == &out) { // same buffers, use a temporary copy
    eblprint( "TEMPORARY COPY!!!!!!!!!!!!!" << std::endl);
    // FIXME
    input = idx<T>(input.get_idxdim());
    idx_copy(input, input); // only copy forward
  }
  this->resize_output(in, out, &d); // resize iff necessary
  idx<T> tmp, tmp2;
  int i;
  tmp = out.narrow(1, input.dim(1), top);
  tmp = tmp.narrow(2, input.dim(2), left);
  idx_copy(input, tmp);
  // mirror border left
  for (i = std::max(0, (int) (left - input.dim(1) / 2)); i < left; ++i) {
    tmp2 = input.narrow(1, 1, left - i - 1);
    tmp = out.narrow(1, 1, i);
    tmp = tmp.narrow(2, input.dim(2), left);
    idx_copy(tmp2, tmp);
  }
  // mirror border right
  for (i = std::max(0, (int) (right - input.dim(1) / 2)); i < right; ++i) {
    tmp2 = input.narrow(1, 1, input.dim(1) - right - 1 + i);
    tmp = out.narrow(1, 1, out.dim(1) - 1 - i);
    tmp = tmp.narrow(2, input.dim(2), right);
    idx_copy(tmp2, tmp);
  }
  // mirror border top using out as input
  for (i = std::max(0, (int) (top - input.dim(2) / 2)); i < top; ++i) {
    tmp2 = out.narrow(2, 1, top + top - i - 1);
    tmp = out.narrow(2, 1, i);
    idx_copy(tmp2, tmp);
  }
  // mirror border bottom using out as input
  for (i = std::max(0, (int) (bottom - input.dim(2) / 2)); i < bottom; ++i) {
    tmp2 = out.narrow(2, 1, out.dim(2) - bottom * 2 - 1 + i);
    tmp = out.narrow(2, 1, out.dim(2) - 1 - i);
    idx_copy(tmp2, tmp);
  }
}

template <typename T>
module_1_1<T>* mirrorpad_module<T>::copy(parameter<T> *p) {
  return (module_1_1<T>*) new mirrorpad_module<T>(pad.dim(0), pad.dim(1));
}

// fsum_module /////////////////////////////////////////////////////////////////

template <typename T>
fsum_module<T>::fsum_module(bool div_, float split_)
    : module_1_1<T>("fsum"), div(div_), split(split_) {
}

template <typename T>
fsum_module<T>::~fsum_module() {
}

template <typename T>
void fsum_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize iff necessary
  T sum;
  uint i, ngroup, size = in.dim(0);
  uint nsplit = std::max((uint) 2, (uint) (in.dim(0) * split));
  idx<T> igroup, ogroup;
  idx_eloop2(inx2, in, T, outx2, out, T) {
    idx_eloop2(inx1, inx2, T, outx1, outx2, T) {
      if (split != 1.0) { // sum in groups
        for (i = 0; i < size; ) {
          ngroup = std::min(nsplit, size - i);
          igroup = inx1.narrow(0, ngroup, i);
          ogroup = outx1.narrow(0, ngroup, i);
          sum = idx_sum(igroup);
          if (div) sum = sum / igroup.nelements();
          idx_fill(ogroup, sum);
          i += ngroup;
        }
      } else { // no splitting
        sum = idx_sum(inx1);
        if (div) sum = sum / inx1.nelements();
        idx_fill(outx1, sum);
      }
    }
  }
}

template <typename T>
void fsum_module<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  idx_checknelems2_all(in.dx[0], out.dx[0]); // must have same dimensions
  // backprop
  T sum;
  uint i, ngroup, size = in.dim(0);
  uint nsplit = std::max((uint) 2, (uint) (in.dim(0) * split));
  idx<T> igroup, ogroup;
  idx_eloop2(indx2, in.dx[0], T, outdx2, out.dx[0], T) {
    idx_eloop2(indx1, indx2, T, outdx1, outdx2, T) {
      if (split != 1.0) { // sum in groups
        for (i = 0; i < size; ) {
          ngroup = std::min(nsplit, size - i);
          igroup = indx1.narrow(0, ngroup, i);
          ogroup = outdx1.narrow(0, ngroup, i);
          sum = idx_sum(ogroup);
          if (div) sum = sum / igroup.nelements();
          idx_addc(igroup, sum, igroup);
          i += ngroup;
        }
      } else { // no splitting
        sum = idx_sum(outdx1);
        if (div) sum = sum / indx1.nelements();
        idx_addc(indx1, sum, indx1);
      }
    }
  }
}

template <typename T>
void fsum_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  state_idx_check_different(in, out); // forbid same in and out
  idx_checknelems2_all(in.ddx[0], out.ddx[0]); // must have same dimensions
  // backprop
  T sum;
  uint i, ngroup, size = in.dim(0);
  uint nsplit = std::max((uint) 2, (uint) (in.dim(0) * split));
  idx<T> igroup, ogroup;
  idx_eloop2(inddx2, in.ddx[0], T, outddx2, out.ddx[0], T) {
    idx_eloop2(inddx1, inddx2, T, outddx1, outddx2, T) {
      if (split != 1) { // sum in groups
        for (i = 0; i < size; ) {
          ngroup = std::min(nsplit, size - i);
          igroup = inddx1.narrow(0, ngroup, i);
          ogroup = outddx1.narrow(0, ngroup, i);
          sum = idx_sum(ogroup);
          if (div) sum = sum / igroup.nelements();
          idx_addc(igroup, sum, igroup);
          i += ngroup;
        }
      } else { // no splitting
        sum = idx_sum(outddx1);
        if (div) sum = sum / inddx1.nelements();
        idx_addc(inddx1, sum, inddx1);
      }
    }
  }
}

// binarize_module /////////////////////////////////////////////////////////////

template <typename T>
binarize_module<T>::binarize_module(T threshold_, T false_value_, T true_value_)
    : threshold(threshold_), false_value(false_value_),
      true_value(true_value_) {
}

template <typename T>
binarize_module<T>::~binarize_module() {
}

template <typename T>
void binarize_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize out iff necessary
  idx_aloop2(inx, in, T, outx, out, T) {
    if (*inx > threshold)
      *outx = true_value;
    else
      *outx = false_value;
  }
}

// range_lut_module ////////////////////////////////////////////////////////////

template <typename T>
range_lut_module<T>::range_lut_module(idx<T> *value_range_) {
  if (value_range_ == NULL)
    eblerror("expected non null range matrix");
  value_range = idx<T>(value_range_->get_idxdim());
  idx_copy(*value_range_, value_range);
}

template <typename T>
range_lut_module<T>::~range_lut_module() {
}

template <typename T>
void range_lut_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize out iff necessary
  idx_aloop2(inx, in, T, outx, out, T) {
    //      eblprint( "v0: " << *inx);
    idx_bloop1(vr, value_range, T) {
      if (*inx < vr.get(1)) {
        *outx = vr.get(0);
        break ;
      }
    }
    //      eblprint( " v1: " << *outx << std::endl);
  }
}

// diag_module /////////////////////////////////////////////////////////////////

template <typename T>
diag_module<T>::diag_module(parameter<T> *p, intg thick, const char *name_)
    : module_1_1<T>(name_), coeff(thick, p) {
  // initialize coeffs to 1
  idx_fill(coeff, (T)1.0);
}

template <typename T>
diag_module<T>::~diag_module() {
}

template <typename T>
void diag_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize out iff necessary
  idx_bloop3(c, coeff, T, i, in, T, o, out, T) {
    idx_dotc(i, c.get(), o);
  }
}

template <typename T>
void diag_module<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_bloop5(c, coeff, T, cd, coeff.dx[0], T, i, in, T, id, in.dx[0], T,
             od, out.dx[0], T) {
    idx_dotcacc(od, c.get(), id); // bprop to input
    idx_dotacc(i, od, cd); // bprop to weights
  }
}

template <typename T>
void diag_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_bloop5(c, coeff, T, cdd, coeff.ddx[0], T, i, in, T, idd, in.ddx[0], T,
             odd, out.ddx[0], T) {
    idx_dotcacc(odd, c.get() * c.get(), idd); // bprop to input
    idx_m2squdotm2acc(i, odd, cdd); // bprop to weights
  }
}

template <typename T>
void diag_module<T>::fprop1_dump(idx<T> &in, idx<T> &out) {
  DUMP(in, this->name() << "_diag_module_in");
  DUMP(coeff.x[0], this->name() << "_diag_module_coeff");
  fprop1(in, out);
  DUMP(out, this->name() << "_diag_module_out");
}

template <typename T>
bool diag_module<T>::resize_output(idx<T> &in, idx<T> &out, idxdim *ignore) {
  // resize output based on input dimensions
  idxdim d(in); // use same dimensions as in
  d.setdim(0, coeff.dim(0)); // except for the first one
  return module_1_1<T>::resize_output(in, out, &d);
}

template <typename T>
void diag_module<T>::load_x(idx<T> &weights) {
  if (!coeff.same_dim(weights)) {
    // if sizes are the same except for the feature size, load
    // into the corresponding slices with a warning
    // this allows to load grayscale pretrained weights only
    // in a grayscale + color net for example.
    idxdim d(coeff);
    d.setdim(0, weights.dim(0));
    if (d == weights.get_idxdim()) {
      eblwarn("Warning: loading weights partly (the first " << d.dim(0)
              << " features) from " << weights << " instead of entire weights ("
              << coeff.x[0] << ")");
      intg sz = std::min(coeff.dim(0), weights.dim(0));
      idx<T> slices = coeff.narrow(0, sz, 0);
      idx<T> w = weights.narrow(0, sz, 0);
      idx_copy(w, slices);
    } else
      eblthrow("expected same dimension weights but got " << coeff.x[0]
               << " and " << weights << " instead in " << this->name());
  } else
    idx_copy(weights, coeff);
}

template <typename T>
std::string diag_module<T>::describe() {
  std::string desc;
  desc << "diag module " << this->name() << " with " << to_string(coeff.x)
       << " coefficients";
  return desc;
}

template <typename T>
module_1_1<T>* diag_module<T>::copy(parameter<T> *p) {
  diag_module<T>* d = new diag_module<T>(p, coeff.dim(0));
  // assign same parameter state if no parameters were specified
  if (!p) d->coeff = coeff;
  return (module_1_1<T>*) d;
}

// copy_module /////////////////////////////////////////////////////////////////

template <typename T>
copy_module<T>::copy_module(const char *name_) : module_1_1<T>(name_) {
}

template <typename T>
copy_module<T>::~copy_module() {
}

template <typename T>
void copy_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize (iff necessary)
  idx_copy(in, out);
}

template <typename T>
void copy_module<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_copy(out.dx[0], in.dx[0]);
}

template <typename T>
void copy_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  idx_copy(out.ddx[0], in.ddx[0]);
}

template <typename T>
std::string copy_module<T>::describe() {
  std::string desc;
  desc << "copy module " << this->name();
  return desc;
}

// back_module /////////////////////////////////////////////////////////////////

#define BACK_MIN -10.0

template <typename T>
back_module<T>::back_module(const char *name_)
    : module_1_1<T>(name_), s0(NULL), s1(NULL), s2(NULL) {
}

template <typename T>
back_module<T>::~back_module() {
}

template <typename T>
void back_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize (iff necessary)
  // copy input to s0
  idx_copy(in, *s0);
  eblprint( "back: mins: so: " << idx_min(*s0) << " s1: " << idx_min(*s1) << " s2: "
            << idx_min(*s2) << std::endl);
  eblprint( "back: maxs: so: " << idx_max(*s0) << " s1: " << idx_max(*s1) << " s2: "
            << idx_max(*s2) << std::endl);
  // put max of all buffers in output
  //     idx_aloop3(x0, *s0, T, x1, *s1, T, o, out, T) {
  //       *o = std::max(*x0, *x1);
  //     }
  // put max of all buffers in output
  idx_aloop4(x0, *s0, T, x1, *s1, T, x2, *s2, T, o, out, T) {
    *o = std::max(*x0, std::max(*x1, *x2));
  }
}

template <typename T>
void back_module<T>::bb(std::vector<bbox*> &boxes) {
    eblprint( "back: " << boxes.size() << " boxes" << std::endl);
  // shift internal buffers and clear first one
  idx_copy(*s1, *s2);
  idx_fill(*s1, (T)BACK_MIN);
  // copy all boxes features to s1
  int height = s0->dim(1);
  int width = s0->dim(2);
  for (uint i = 0; i < boxes.size(); ++i) {
    bbox &b = *(boxes[i]);
    // find box's location at this stage
    float rho = b.o.h0 / (float) b.oheight;
    float rwo = b.o.w0 / (float) b.owidth;
    int h0 = (int) (height * rho);
    int w0 = (int) (width * rwo);
    int h = pixel_size.dim(1);
    int w = pixel_size.dim(2);
    // cut bbox if outside of buffers
    if (h0 < 0) { h -= h0; h0 = 0; }
    if (w0 < 0) { w -= w0; w0 = 0; }
    if (h0 + h > height) h -= h0 + h - height;
    if (w0 + w > width) w -= w0 + w - width;
    // max-copy box features from s0 to s1
    idx<T> b1 = s1->narrow(1, h, h0);
    b1 = b1.narrow(2, w, w0);
    idx<T> b0 = s0->narrow(1, h, h0);
    b0 = b0.narrow(2, w, w0);
    idx_max(b0, b1);
  }
  // shift buffers for horizontal motion
  int wshift = (int) (.02 * width);
  eblprint( "back: shift by " << wshift << " pixels (width: "
            << width << ")" << std::endl);
  idx<T> tmp(s1->get_idxdim());
  idx_fill(tmp, (T)BACK_MIN);
  idx<T> shifted = tmp.narrow(2, width - wshift, wshift);
  idx<T> original = s1->narrow(2, width - wshift, 0);
  idx_copy(original, shifted);
  idx_copy(tmp, *s1);
  // shift s2
  idx_fill(tmp, (T)BACK_MIN);
  shifted = tmp.narrow(2, width - wshift, wshift);
  original = s2->narrow(2, width - wshift, 0);
  idx_copy(original, shifted);
  idx_copy(tmp, *s2);
  // decay buffers
  //    idx_addc(*s1, (T) -0.2, *s1);
  //    idx_addc(*s2, (T) -0.2, *s2);
}

template <typename T>
bool back_module<T>::resize_output(idx<T> &in, idx<T> &out, idxdim *ignore) {
  TIMING_RESIZING_ACCSTART(); // start accumulating resizing time
  // resize output based on input dimensions
  idxdim d(in); // use same dimensions as in
  if (out.get_idxdim() != d) { // resize only if necessary
    EDEBUG(this->name() << ": resizing output from " << out << " to " << d);
    out.resize(d);
  }
  if (!s0 || s0->get_idxdim() != d) {
    eblprint( "back: resizing internal buffers to " << d << std::endl);
    if (s0) s0->resize(d); else s0 = new idx<T>(d);
    if (s1) s1->resize(d); else s1 = new idx<T>(d);
    if (s2) s2->resize(d); else s2 = new idx<T>(d);
    idx_fill(*s0, (T)BACK_MIN);
    idx_fill(*s1, (T)BACK_MIN);
    idx_fill(*s2, (T)BACK_MIN);
  }
  TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
  return true;
}

template <typename T>
fidxdim back_module<T>::bprop1_size(const fidxdim &osize) {
  pixel_size = osize;
  eblprint( "back_module: 1 output pixel corresponds here to " << pixel_size
            << std::endl);
  return osize;
}

template <typename T>
std::string back_module<T>::describe() {
  std::string desc;
  desc << "back module " << this->name();
  return desc;
}

// printer_module //////////////////////////////////////////////////////////////

template <typename T>
printer_module<T>::printer_module(const char* name_)
    : module_1_1<T>(name_) {
}

template <typename T>
printer_module<T>::~printer_module() {
}

template <typename T>
void printer_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  T max = idx_max(in);
  T min = idx_min(in);
  eblprint( "min: "<< min << "\tmax: "<< max << std::endl);
  idx_copy(in, out);
}

template <typename T>
void printer_module<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // backprop
  T max = idx_max(out.dx[0]);
  T min = idx_min(out.dx[0]);
  eblprint( "min: "<< min << "\tmax: "<< max << std::endl);
  idx_copy(out.dx[0], in.dx[0]);
}

template <typename T>
void printer_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // backprop
  T max = idx_max(out.ddx[0]);
  T min = idx_min(out.ddx[0]);
  eblprint( "min: "<< min << "\tmax: "<< max << std::endl);
  idx_copy(out.ddx[0], in.ddx[0]);
}

// dropout_module //////////////////////////////////////////////////////////////

template <typename T>
dropout_module<T>::dropout_module(double drop_proba_, bool test_time_,
                                  const char* name_)
    : module_1_1<T>(name_), drop_proba(drop_proba_), test_time(test_time_) {
  if (drop_proba < 0 || drop_proba >= 1)
    eblerror("expected dropping probability in [0, 1) range");
}

template <typename T>
dropout_module<T>::~dropout_module() {
}

template <typename T>
void dropout_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  // resize if necessary
  this->resize_output(in, out); // resize (iff necessary)
  if (!keep.same_dim(in)) keep = idx<ubyte>(in.get_idxdim());

  // during test time, only scale down outputs because
  // less outputs were passed during training
  if (test_time) {
    idx_dotc(in, 1 - drop_proba, out);
  } else {
    // decide which inputs to keep
    idx_aloopf1(k, keep, ubyte, {
        if (drand() > drop_proba) *k = 1; else *k = 0; });
    // copy and multiply by keep flag to output
    idx_mul(in, keep, out);
  }
}

template <typename T>
void dropout_module<T>::bprop1(state<T> &in, state<T> &out) {
  // checks
  if (test_time) eblerror("bprop not expected during test time");
  DEBUG_CHECK_DX(in); // in debug mode, check backward tensors are allocated
  // only pass back gradients of kept inputs
  idx_mul(out.dx[0], keep, in.dx[0]);
}

template <typename T>
void dropout_module<T>::bbprop1(state<T> &in, state<T> &out) {
  // checks
  if (test_time) eblerror("bprop not expected during test time");
  DEBUG_CHECK_DDX(in); // in debug mode, check backward tensors are allocated
  // only pass back gradients of kept inputs
  idx_mul(out.ddx[0], keep, in.ddx[0]);
}

template <typename T>
std::string dropout_module<T>::describe() {
  std::string desc;
  desc << "dropout module " << this->name() << " dropping inputs with "
       << drop_proba << " probability";
  return desc;
}

} // end namespace ebl
