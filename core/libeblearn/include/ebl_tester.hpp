/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun, Pierre Sermanet, Koray Kavukcuoglu *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com, koray@cs.nyu.edu *
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

template <class T>
module_tester<T>::module_tester(double thres_, T rrange_min_, T rrange_max_,
                                FILE* out_)
    : acc_thres(thres_), rrange_min(rrange_min_), rrange_max(rrange_max_),
      out(out_),
      jac_fprop(1,1), jac_bprop(1,1), jac_pfprop(1,1), jac_pbprop(1,1),
      hes_fprop(1,1), hes_bprop(1,1), hes_pfprop(1,1), hes_pbprop(1,1) {
  dynamic_init_drand();
}

template <class T>
module_tester<T>::~module_tester() {
}

template <class T>
idx<double> module_tester<T>::
test_jacobian(module_1_1<T>  &module, state<T> &in, state<T> &out) {
  forget_param_linear fp(2, 0.5, 0 /* fixed seed */);
  idx_random(in, rrange_min, rrange_max); // randomize input for fprop
  module.fprop(in,out); // just to resize states
  module.forget(fp); // randomize parametes if there are any
  // clear all input and output
  in.zero_all();
  out.zero_all();
  idx_random(in, rrange_min, rrange_max); // randomize input for fprop
  // EDEBUG("in: " << in.str() << std::endl << " out: "
  // << out.str() << std::endl );
  get_jacobian_fprop(module, in, out, jac_fprop);
  in.resize_dx();
  out.resize_dx();
  in.zero_dx();
  get_jacobian_bprop(module, in, out, jac_bprop);
  // EDEBUG(" jac_fprop: " << jac_fprop.str() << std::endl
  //       << " jac_bprop " << jac_bprop.str() << std::endl);
  return get_errs(jac_fprop,jac_bprop);
}

template <class T>
idx<double> module_tester<T>::
test_jacobian_param(parameter<T> &p, module_1_1<T> &module,
                    state<T> &in, state<T> &out) {
  forget_param_linear fp(2, 0, 0 /* fixed seed */);
  idx_random(in, rrange_min, rrange_max); // randomize input for fprop
  module.fprop(in,out); // just to resize states
  module.forget(fp); // randomize parametes if there are any
  // clear all input and output
  in.zero_all();
  out.zero_all();
  idx_random(p, rrange_min, rrange_max); // randomize input for fprop
  idx_random(in, rrange_min, rrange_max);
  get_jacobian_fprop_param(p, module, in, out, jac_pfprop);
  in.resize_dx();
  out.resize_dx();
  in.zero_dx();
  get_jacobian_bprop_param(p, module, in, out, jac_pbprop);
  return get_errs(jac_pfprop, jac_pbprop);
}


template <class T>
idx<double> module_tester<T>::
test_hessian(module_1_1<T>  &module, state<T> &in, state<T> &out) {
  forget_param_linear fp(2, 0.5, 0 /* fixed seed */);
  idx_random(in, rrange_min, rrange_max); // randomize input for fprop
  module.fprop(in,out); // just to resize states
  module.forget(fp); // randomize parametes if there are any
  // clear all input and output
  in.zero_all();
  out.zero_all();
  idx_random(in, rrange_min, rrange_max); // randomize input for fprop
  get_hessian_fprop(module, in, out, hes_fprop);
  in.resize_dx();
  in.resize_ddx();
  out.resize_dx();
  out.resize_ddx();
  in.zero_dx();
  in.zero_ddx();
  get_hessian_bprop(module, in, out, hes_bprop);
  return get_errs(hes_fprop, hes_bprop);
}

template <class T>
idx<double> module_tester<T>::
test_hessian_param(parameter<T> &p, module_1_1<T> &module,
                   state<T> &in, state<T> &out) {
  forget_param_linear fp(2, 0, 0 /* fixed seed */);
  idx_random(in, rrange_min, rrange_max); // randomize input for fprop
  module.fprop(in,out); // just to resize states
  module.forget(fp); // randomize parametes if there are any
  // clear all input and output
  in.zero_all();
  out.zero_all();
  idx_random(p, rrange_min, rrange_max); // randomize input for fprop
  idx_random(in, rrange_min, rrange_max);
  get_hessian_fprop_param(p, module, in, out, hes_pfprop);
  in.resize_dx();
  in.resize_ddx();
  out.resize_dx();
  out.resize_ddx();
  in.zero_dx();
  in.zero_ddx();
  get_hessian_bprop_param(p, module, in, out, hes_pbprop);
  return get_errs(hes_pfprop, hes_pbprop);
}

template <class T>
double module_tester<T>::get_acc_thres() const {
  return acc_thres;
}

template <class T>
void module_tester<T>::set_acc_thres(double acc_thres_) {
  acc_thres = acc_thres_;
}

template <class T>
T module_tester<T>::get_rrange() const {
  return rrange_max;
}

template <class T>
void module_tester<T>::set_rrange(T rrange) {
  rrange_min = -rrange;
  rrange_max = rrange;
}

template <class T>
FILE* module_tester<T>::get_out() const {
  return out;
}

template <class T>
void module_tester<T>::set_out(FILE* out) {
  this->out = out;
}

// protected members /////////////////////////////////////////////////////////

template <class T>
void module_tester<T>::get_jacobian_fprop(module_1_1<T> &module, state<T> &in,
                                          state<T> &out, idx<T>& jac) {
  state<T> sina = in.deep_copy(); //x-small
  state<T> sinb = in.deep_copy(); //x+small
  state<T> souta = out.deep_copy(); //f(x-small)
  state<T> soutb = out.deep_copy(); //f(x+small)
  T small = 1e-6;
  int cnt = 0;
  // clear out jacobian matrix
  jac.resize(in.nelements(), out.nelements());
  idx_clear(jac);
  idx_aloop3(sx, in, T, sxa, sina, T, sxb, sinb, T) {
    idx_copy(in, sina);
    idx_copy(in, sinb);
    // perturb
    // std::setprecision(16);
    *sxa = *sx - small;
    *sxb = *sx + small;
    // cout << "sx,sxa,sxb:" << std::setprecision(16) << *sx
    // << "\t" << std::setprecision(16) << *sxa << "\t"
    // << std::setprecision(16) << *sxb << endl;
    // idx_addc(in, -small, sina);
    // idx_addc(in, small, sinb);
    // perturb
    // T t0 = *sx;
    // T t1 = t0 - small;
    // T t2 = t0 + small;
    // double t3 = (double) t2 - (double) t1;
    // cout << "t0 " << t0 << " t1 " << t1 << " t2 " << t2 << " t3 " << t3 << endl;
    // *sxa = *sx - small;
    //cout << "sina: " << std::setprecision(16) << sina.gget()
    // << " souta: " <<std::setprecision(16) << souta.gget() <<endl;
    //  cout << "sinb: " << sinb.gget() << " soutb: " << soutb.gget() <<endl;
    //      idx_sub(sinb, sina, soutb);
    // //      cout << "diff: " << soutb.gget() <<endl;
    // idx_clear(souta);
    // idx_clear(soutb);

    module.fprop(sina, souta);
    //      *sxb = *sxa + 2 * small;
    module.fprop(sinb, soutb);
    idx_sub(soutb, souta, soutb);
    // cout << "diff: " <<std::setprecision(16) << soutb.gget() <<endl;
    idx<T> j = jac.select(0, cnt);
    idx_dotc(soutb, (T) (1.0 / (2 * small)), j);
    // cout << "1/2small: " << (T) (1.0 / (2 * small)) <<endl;
    // cout << "soutbx: " << soutb.gget() <<endl;
    // cout << "soutbx/2small: " << (soutb.gget() / (2 * small)) <<endl;
    // cout << "jac: " << (T) jac.gget() << endl;
    cnt++;
  }
}

template <class T>
void module_tester<T>::
get_jacobian_fprop_param(parameter<T> &p, module_1_1<T> &module,
                         state<T> &in, state<T> &out, idx<T>& jac) {
  state<T> souta = out.deep_copy(); //f(x-small)
  state<T> soutb = out.deep_copy(); //f(x+small)
  T small = 1e-6;
  int cnt = 0;
  // clear out jacobian matrix
  idx_clear(jac);
  { idx_aloop1(px, p, T) {
      // perturb
      *px = *px - small;
      module.fprop(in, souta);
      *px = *px + 2*small;
      module.fprop(in, soutb);
      *px = *px - small;
      idx_sub(soutb,souta,soutb);
      idx<T> j = jac.select(0,cnt);
      idx_dotc(soutb,1.0/(2*small),j);
      cnt++;
    }}
}

template <class T>
void module_tester<T>::
get_jacobian_bprop(module_1_1<T> &module, state<T> &in, state<T> &out,
                   idx<T>& jac) {
  in.resize_dx();
  out.resize_dx();
  jac.resize(in.nelements(), out.nelements());
  idx_clear(jac);
  int cnt = 0;
  idx_aloop1(dx, out.dx[0],T) {
    idx_clear(out.dx[0]);
    idx_clear(in.dx[0]);
    *dx = (T) 1.0;
    module.bprop(in, out);
    idx<T> j = jac.select(1, cnt);
    idx_copy(in.dx[0], j);
    cnt++;
  }
  //cout << "jac bprop: " << (T) jac.gget() << endl;
}

template <class T>
void module_tester<T>::
get_jacobian_bprop_param(parameter<T> &p, module_1_1<T> &module,
                         state<T> &in, state<T> &out, idx<T>& jac) {
  eblerror("not implemented");
}

template <class T>
void module_tester<T>::
get_hessian_fprop(module_1_1<T> &module, state<T> &in,
                  state<T> &out, idx<T> &jac) {
  state<T> sina = in.deep_copy(); //x-small
  state<T> sinb = in.deep_copy(); //x+small
  state<T> souta = out.deep_copy(); //f(x-small)
  state<T> soutb = out.deep_copy(); //f(x+small)
  double small = 1e-6;
  int cnt = 0;
  // clear out hessian matrix
  jac.resize(in.nelements(), out.nelements());
  idx_clear(jac);
  idx_aloop3(sx, in, T, sxa, sina, T, sxb, sinb, T) {
    idx_copy(in, sina);
    idx_copy(in, sinb);
    // perturb
    *sxa = *sx - small;
    *sxb = *sx + small;
    module.fprop(sina, souta);
    module.fprop(sinb, soutb);
    idx<T> ad(souta.get_idxdim());
    idx<T> dot(souta.get_idxdim());
    idx<T> sub(souta.get_idxdim());
    idx_add(souta, soutb, ad);
    idx_dotc(out, 2, dot);
    idx_sub(ad, dot, sub);
    idx<T> j = jac.select(0, cnt);
    idx_dotc(sub, 1.0 / small, j);
    cnt++;
  }
}

template <class T>
void module_tester<T>::
get_hessian_fprop_param(parameter<T> &p, module_1_1<T> &module,
                        state<T> &in, state<T> &out, idx<T>& jac){
  state<T> souta = out.deep_copy(); //f(x-small)
  state<T> soutb = out.deep_copy(); //f(x+small)
  double small = 1e-6;
  int cnt = 0;
  // clear out hessian matrix
  idx_clear(jac);
  { idx_aloop1(px, p, T) {
      // perturb
      *px = *px - small;
      module.fprop(in, souta);
      *px = *px + 2*small;
      module.fprop(in, soutb);
      *px = *px - small;
      idx_sub(soutb,souta,soutb);
      idx<T> j = jac.select(0,cnt);
      idx_dotc(soutb,1.0/(2*small),j);
      cnt++;
    }}
}

template <class T>
void module_tester<T>::
get_hessian_bprop(module_1_1<T> &module, state<T> &in,
                  state<T> &out, idx<T>& jac) {
  in.resize_ddx();
  out.resize_ddx();
  jac.resize(in.nelements(), out.nelements());
  idx_clear(jac);
  //     module.fprop(in, out);
  //     module.bprop(in, out);
  //     module.bbprop(in, out);
  //     idx_copy(in.ddx[0], jac);

  int cnt = 0;
  idx_aloop1(ddx, out.ddx[0],T) {
    idx_clear(out.ddx[0]);
    idx_clear(in.ddx[0]);
    //      *ddx = 1.0;
    module.bbprop(in, out);
    idx<T> j = jac.select(1, cnt);
    idx_copy(in.ddx[0], j);
    cnt++;
  }
}

template <class T>
void module_tester<T>::
get_hessian_bprop_param(parameter<T> &p, module_1_1<T> &module,
                        state<T> &in, state<T> &out, idx<T>& jac){
  eblerror("not implemented");
}

template <class T>
idx<double> module_tester<T>::get_errs(idx<T>& a, idx<T>& b) {
  double maxdist;
  // max distance
  idx_sub(a,b,a);
  idx_abs(a,a);
  double totdist = idx_sum(a);
  maxdist = (double) idx_max(a);
  idx<double> errs(2);
  errs.set(maxdist, 0);
  errs.set(totdist, 1);
  return errs;
}

template <class T>
void module_tester<T>::report_err(idx<T>& a, idx<T>& b, const char* msg) {
  idx<double> errs = get_errs(a, b);
  std::stringstream ss(std::stringstream::in | std::stringstream::out);
  // report results
  ss << "Max " << msg << " distance";
  fprintf(this->out,"%-40s = %-15g %15s\n",ss.str().c_str(), errs.get(0),
          ((errs.get(0) < this->acc_thres)?"OK":"NOT OK"));
  ss.str("");
  ss << "Total " << msg << "distance";
  fprintf(this->out,"%-40s = %-15g %15s\n",ss.str().c_str(), errs.get(1),
          ((errs.get(1) < this->acc_thres)?"OK":"NOT OK"));
  fflush(this->out);
}

////////////////////////////////////////////////////////////////

template <class T>
void Jacobian_tester<T>::test(module_1_1<T> &module) {
  int insize = 16;
  state<T> in(insize, 1, 1);
  state<T> out(insize, 1, 1);

  //init
  dseed(2);  // 2 is chosen randomly... feel free to change it
  module.fprop(in, out); // used to resize the outputs
  { idx_bloop1( i, in, T)
    { idx_bloop1 (ii, i, T)
      { idx_bloop1( iii, ii, T)
        { iii.set(drand(2)); }
      }
    }
  }
  { idx_bloop1( o, out, T)
    { idx_bloop1 (oo, o, T)
      { idx_bloop1( ooo, oo, T)
        { ooo.set(drand(2)); }
      }
    }
  }

  // check the Jacobian
  int ndim_in = in.nelements();
  int ndim_out = in.nelements();
  // used to store the jacobian calculated via bprop
  idx<T> jac_fprop(ndim_in, ndim_out);
  //  used to store the jacobian calculated via prturbations
  idx<T> jac_bprop(ndim_in, ndim_out);

  // creation of jac_fprop
  module.fprop(in, out);
  in.resize_dx();
  out.resize_dx();
  int cnt = 0;
  { idx_bloop1(o, out, T)
    { idx_bloop1(oo, o, T)
      { idx_bloop1(ooo, oo, T)
        {
          out.zero_dx();
          in.zero_dx();
          ooo.set(1);
          module.bprop(in, out);
          idx<T> bla = jac_bprop.select(1, cnt);
          idx_copy(in.dx[0], bla);
          cnt++;
        }
      }
    }
  }

  // creation of jac_bprop
  cnt = 0;
  double small = pow(10.0, -6);
  state<T> in1(in.dim(0), in.dim(1), in.dim(2));
  state<T> in2(in.dim(0), in.dim(1), in.dim(2));
  state<T> out1(1, 1, 1);
  state<T> out2(1, 1, 1);
  for(int d1 = 0; d1 < in.dim(0); d1++){
    for(int d2 = 0; d2 < in.dim(1); d2++){
      for(int d3 = 0; d3 < in.dim(2); d3++){
        idx_copy(in, in1);
        idx_copy(in, in2);
        in1.set(in1.get( d1, d2, d3) + small, d1, d2, d3);
        in2.set(in2.get( d1, d2, d3) - small, d1, d2, d3);
        module.fprop(in1, out1);
        module.fprop(in2, out2);
        idx<T> sub(new srg<T>(), out1.spec);
        idx<T> dot(new srg<T>(), out1.spec);
        idx_sub(out1, out2, sub);
        idx_dotc(sub, 0.5/small, dot);
        idx<T> bla2 = jac_fprop.select(0, cnt);
        idx_copy(dot, bla2);
        cnt++;
      }
    }
  }

  // comparison
  printf("Jacobian error: %8.7e\n", idx_sqrdist(jac_fprop, jac_bprop));
}

////////////////////////////////////////////////////////////////////////

template <class T>
void Bbprop_tester<T>::test(module_1_1<T> &module){

  int insize = 16;
  state<T> in(insize, 1, 1);
  state<T> out(insize, 1, 1);

  //init
  dseed(2);  // 2 is chosen randomly... feel free to change it
  module.fprop(in, out); // used to resize the outputs
  { idx_bloop1( i, in, T)
    { idx_bloop1 (ii, i, T)
      { idx_bloop1( iii, ii, T)
        { iii.set(drand(2)); }
      }
    }
  }
  { idx_bloop1( o, out, T)
    { idx_bloop1 (oo, o, T)
      { idx_bloop1( ooo, oo, T)
        { ooo.set(drand(2)); }
      }
    }
  }

  module.fprop(in, out);
  in.resize_dx();
  out.resize_dx();
  module.bprop(in, out);
  in.resize_ddx();
  out.resize_ddx();
  module.bbprop(in, out);

  // used to store the bbprop calculated via perturbation
  idx<T> bbprop_p(in.dim(0), in.dim(1), in.dim(2));

  // creation of bbprop_p
  int cnt = 0;
  double small = pow(10.0, -6);
  state<T> in1(in.dim(0), in.dim(1), in.dim(2));
  state<T> in2(in.dim(0), in.dim(1), in.dim(2));
  state<T> out1( 1, 1, 1);
  state<T> out2( 1, 1, 1);
  for(int d1 = 0; d1 < in.dim(0); d1++){
    for(int d2 = 0; d2 < in.dim(1); d2++){
      for(int d3 = 0; d3 < in.dim(2); d3++){
        idx_copy(in, in1);
        idx_copy(in, in2);
        in1.set(in1.get( d1, d2, d3) + small, d1, d2, d3);
        in2.set(in2.get( d1, d2, d3) - small, d1, d2, d3);
        module.fprop(in1, out1);
        module.fprop(in2, out2);
        // here we calculate a in aX²+bX+c as a model for the 3 points
        // calculated via
        // fprop(...), fprop(...+small) and fprop(...-small). the second
        // derivative is then 2*a
        idx<T> ad(new srg<T>(), out1.spec);
        idx<T> sub(new srg<T>(), out1.spec);
        idx<T> dot(new srg<T>(), out1.spec);
        idx<T> dot2(new srg<T>(), out1.spec);
        idx_add(out1, out2, ad);
        idx_dotc(out, 2, dot);
        idx_sub(ad, dot, sub);
        idx_dotc(sub, 1/small, dot2);
        bbprop_p.set(dot2.get( d1, d2, d3), d1, d2, d3);
        cnt++;
      }
    }
  }

  // comparison
  printf("bbprop error: %8.7e \n", idx_sqrdist(in.ddx[0], bbprop_p));
}

////////////////////////////////////////////////////////////////////////

template <class T>
void Bprop_tester<T>::test(module_1_1<T> &module){

  int insize = 16;
  state<T> in(insize, 1, 1);
  state<T> out(insize, 1, 1);

  //init
  dseed(2);  // 2 is chosen randomly... feel free to change it
  module.fprop(in, out); // used to resize the outputs
  { idx_bloop1( i, in, T)
    { idx_bloop1 (ii, i, T)
      { idx_bloop1( iii, ii, T)
        { iii.set(drand(2)); }
      }
    }
  }
  { idx_bloop1( o, out, T)
    { idx_bloop1 (oo, o, T)
      { idx_bloop1( ooo, oo, T)
        { ooo.set(drand(2)); }
      }
    }
  }

  // used to store the bbprop calculated via perturbation
  idx<T> bprop_p(in.dim(0), in.dim(1), in.dim(2));

  // creation of bprop_p
  int cnt = 0;
  double small = pow(10.0, -6);
  state<T> in1(in.dim(0), in.dim(1), in.dim(2));
  state<T> in2(in.dim(0), in.dim(1), in.dim(2));
  state<T> out1(1, 1, 1);
  state<T> out2(1, 1, 1);
  for(int d1 = 0; d1 < in.dim(0); d1++){
    for(int d2 = 0; d2 < in.dim(1); d2++){
      for(int d3 = 0; d3 < in.dim(2); d3++){
        idx_copy(in, in1);
        idx_copy(in, in2);
        in1.set(in1.get( d1, d2, d3) + small, d1, d2, d3);
        in2.set(in2.get( d1, d2, d3) - small, d1, d2, d3);
        module.fprop(in1, out1);
        module.fprop(in2, out2);

        idx<T> sub(new srg<T>(), out1.spec);
        idx<T> dot(new srg<T>(), out1.spec);
        idx_sub(out1, out2, sub);
        idx_dotc(sub, 0.5/small, dot);
        bprop_p.set(dot.get( d1, d2, d3), d1, d2, d3);
        cnt++;
      }
    }
  }

  printf("Bprop error : %8.7e \n", idx_sqrdist(in.dx[0], bprop_p));
}

} // end namespace ebl
