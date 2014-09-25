/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
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

// state_forwardvector ////////////////////////////////////////////////////
template <class T>
state_forwardvector<T>::state_forwardvector() : svector<T>(), parent(NULL) {
}

template <class T>
state_forwardvector<T>::state_forwardvector(const 
					    state_forwardvector<T> &other) 
  : svector<T>(), parent(NULL) {
  this->copy(other);
}

template <class T>
state_forwardvector<T>::state_forwardvector(uint n) 
  : svector<T>(n), parent(NULL) {
}

template <class T>
state_forwardvector<T>::~state_forwardvector() {
  clear();
}

template <class T>
void state_forwardvector<T>::unlock_all() {
  for (this->it = this->begin(); this->it != this->end(); this->it++)
    if (*(this->it) != NULL && parent != *(this->it)) {
      (*(this->it))->unlock();
    }
}

template <class T>
void state_forwardvector<T>::clear() {
  this->unlock_all();
  std::vector<T*>::clear();
}

template <class T>
void state_forwardvector<T>::clear(uint i) {
  T *e = std::vector<T*>::operator[](i);
  if (e && (parent != this->at_ptr(i)))
    e->unlock();
  std::vector<T*>::operator[](i) = NULL;
}

template <class T>
void state_forwardvector<T>::remove_without_unlock(uint i) {
  if (i >= this->size())
    eblerror("cannot remove element " << i << " when vector has only "
             << this->size() << " elements");
  this->it = this->begin() + i;
  this->erase(this->it);
}

template <class T>
void state_forwardvector<T>::set(T &o, uint i) {
  T* &e = this->at(i);
  if (e && (parent != this->at_ptr(i)))
       e->unlock();
  if (parent != &o)
    o.lock();
  e = &o;
}

template <class T>
void state_forwardvector<T>::set_new(T &o, uint i) {
  T* &e = this->at(i);
  if (e && (parent != e))
    e->unlock();
  T *n = new T(o);
  if (parent != n)
    n->lock();
  e = n;
}

template <class T>
void state_forwardvector<T>::push_back(T *e) {
  if (e && (e != parent)) e->lock();
  std::vector<T*>::push_back(e);
}

template <class T>
void state_forwardvector<T>::push_back(T &e) {
  if (&e != parent)
    e.lock();
  std::vector<T*>::push_back(&e);
}


// state /////////////////////////////////////////////////////////////////////

template<typename T>
state<T>::~state() {
  if(x.at_ptr(0) == x.parent)
    x.remove_without_unlock(0);
}

template<typename T>
state<T>::state() : idx<T>() {
  init();
}

template<typename T>
state<T>::state(intg s0, parameter<T> *p) : idx<T>() {
  init();
  idxdim d(s0);
  allocate(d, p);
}

template<typename T>
state<T>::state(intg s0, intg s1, parameter<T> *p) : idx<T>() {
  init();
  idxdim d(s0, s1);
  allocate(d, p);
}

template<typename T>
state<T>::state(intg s0, intg s1, intg s2, parameter<T> *p) : idx<T>() {
  init();
  idxdim d(s0, s1, s2);
  allocate(d, p);
}

template<typename T>
state<T>::state(intg s0, intg s1, intg s2, intg s3, parameter<T> *p) : idx<T>() {
  init();
  idxdim d(s0, s1, s2, s3);
  allocate(d, p);
}

template<typename T>
state<T>::state(intg s0, intg s1, intg s2, intg s3, intg s4,
		intg s5, intg s6, intg s7, parameter<T> *p) : idx<T>() {
  init();
  idxdim d(s0, s1, s2, s3, s4, s5, s6, s7);
  allocate(d, p);
}

template<typename T>
state<T>::state(const idxdim &d, parameter<T> *p) : idx<T>() {
  init();
  allocate(d, p);
}

template <typename T>
state<T>::state(const state<T> &s) : idx<T>() {
  init();
  forward_only = s.forward_only;
  x.clear();
  // forward
  for (uint i = 0; i < s.x.size(); ++i)
    if (s.x.exists(i)) add_x_new(s.x.at_const(i));
  // backward
  for (uint i = 0; i < s.dx.size(); ++i)
    if (s.dx.exists(i)) dx.push_back_new(s.dx.at_const(i));
  // bbackward
  for (uint i = 0; i < s.ddx.size(); ++i)
    if (s.ddx.exists(i)) ddx.push_back_new(s.ddx.at_const(i));
}

template <typename T>
state<T>::state(state<T> &s, intg dim_size, uint n) : idx<T>() {
  init();
  forward_only = s.forward_only;
  reset_tensors(s.x, x, dim_size, n);
  if (!s.dx.empty()) reset_tensors(s.dx, dx, dim_size, n);
  if (!s.ddx.empty()) reset_tensors(s.ddx, ddx, dim_size, n);
  link_f0();
  zero_all();
}

template<typename T>
state<T>::state(const idx<T> &ft) : idx<T>() {
  init();
  x.clear();
  add_x_new(ft);
}

template<typename T>
state<T> & state<T>::operator=(const state<T> &s) {
  forward_only = s.forward_only;
  x.clear();
  // forward
  for (uint i = 0; i < s.x.size(); ++i)
    if (s.x.exists(i)) add_x_new(s.x.at_const(i));
  // backward
  for (uint i = 0; i < s.dx.size(); ++i)
    if (s.dx.exists(i)) dx.push_back_new(s.dx.at_const(i));
  // bbackward
  for (uint i = 0; i < s.ddx.size(); ++i)
    if (s.ddx.exists(i)) ddx.push_back_new(s.ddx.at_const(i));
  return *this;
}

template<typename T>
state<T> & state<T>::operator=(const idx<T> &other) {
  // clear existing stuff
  clear_all();
  forward_only = true;
  add_x_new(other);
  return *this;
}

template<typename T>
state<T>::state(const idx<T> &ft, const idx<T> &bt) : idx<T>() {
  init();
  x.clear();
  add_x_new(ft);
  dx.push_back_new(bt);
}

template<typename T>
state<T>::state(const idx<T> &xt, const idx<T> &dxt, const idx<T> &ddxt)
    : idx<T>() {
  init();
  x.clear();
  add_x_new(xt);
  dx.push_back_new(dxt);
  ddx.push_back_new(ddxt);
}

template <typename T>
void state<T>::set_forward_only() {
  forward_only = true;
}

// data manipulation methods /////////////////////////////////////////////////

template <typename T> void state<T>::zero_all() {
  zero_x();
  zero_dx();
  zero_ddx();
}

template <typename T> void state<T>::zero_x() {
  for (typename svector<idx<T> >::iterator i = x.begin(); i != x.end(); ++i)
    if (i.exists()) idx_clear(*i);
}

template <typename T> void state<T>::zero_dx() {
  if (!dx.empty()) {
    for (typename svector<idx<T> >::iterator i = dx.begin(); i != dx.end(); ++i)
      if (i.exists()) idx_clear(*i);
  }
}

template <typename T> void state<T>::zero_ddx() {
  if (!dx.empty()) {
    for (typename svector<idx<T> >::iterator i = ddx.begin(); i != ddx.end(); ++i)
      if (i.exists()) idx_clear(*i);
  }
}

template <typename T> void state<T>::clear_all() {
  x.clear();
  dx.clear();
  ddx.clear();
}

// resize methods ////////////////////////////////////////////////////////////

template <typename T>
void state<T>::resize(intg s0, intg s1, intg s2, intg s3,
                      intg s4, intg s5, intg s6, intg s7) {
  if (!idx<T>::same_dim(s0, s1, s2, s3, s4, s5, s6, s7)) {
    idx<T>::resize(s0, s1, s2, s3, s4, s5, s6, s7);
    if (!dx.empty()) dx[0].resize(s0, s1, s2, s3, s4, s5, s6, s7);
    if (!ddx.empty()) ddx[0].resize(s0, s1, s2, s3, s4, s5, s6, s7);
  }
}

template <typename T>
void state<T>::resize(const idxdim &d, intg n) {
  for (uint i = 0; i < std::max((intg)1, n); ++i) {
    if (!x.exists(i)) {
      x.push_back(new idx<T>(d));
      if (!dx.empty()) dx.push_back(new idx<T>(d));
      if (!ddx.empty()) ddx.push_back(new idx<T>(d));
    } else if (!x[i].same_dim(d)) {
      x[i].resize(d);
      if (dx.exists(i)) dx[i].resize(d);
      if (ddx.exists(i)) ddx[i].resize(d);
    }
  }
}

template <typename T>
void state<T>::resize1(intg dimn, intg size) {
  if (idx<T>::dim(dimn) != size) {
    idx<T>::resize1(dimn, size);
    if (!dx.empty()) dx[0].resize(dimn, size);
    if (!ddx.empty()) ddx[0].resize(dimn, size);
  }
}

template <typename T>
void state<T>::resize_as(state<T>& other) {
  resize_vidx_as(other.x, x);
  link_f0();
  resize_vidx_as(other.dx, dx);
  resize_vidx_as(other.ddx, ddx);
}

template <typename T>
void state<T>::resize_as_but1(state<T>& s, intg fixed_dim) {
  idxdim d(s.spec); // use same dimensions as s
  d.setdim(fixed_dim, idx<T>::dim(fixed_dim));
  state<T>::resize(d);
}

template <typename T> template <typename T2>
void state<T>::resize_forward_orders(state<T2> &other, intg dim_sizes, intg n) {
  if (forward_only && !other.dx.empty())
    eblerror("trying to resize current forward-only state " << *this
             << " as a non-forward-only state " << other);
  resize_vidx_orders(other.x, x, dim_sizes, n);
  link_f0();
}

template <typename T>
void state<T>::resize_dx() {
  resize_vidx_as(x, dx);
}

template <typename T>
void state<T>::resize_ddx() {
  resize_vidx_as(x, ddx);
}

template <typename T>
void state<T>::reset(const idxdim &d, intg n) {
  bool dx_exists = !dx.empty();
  bool ddx_exists = !ddx.empty();
  x.clear();
  dx.clear();
  ddx.clear();
  for (uint i = 0; i < std::max((intg)1, n); ++i) {
    x.push_back(new idx<T>(d));
    if (dx_exists) dx.push_back(new idx<T>(d));
    if (ddx_exists) ddx.push_back(new idx<T>(d));
  }
  link_f0();
  zero_all();
}

template <typename T>
bool state<T>::allocated_dx() {
  if (x.size() != dx.size()) return false;
  for (uint i = 0; i < x.size(); ++i) {
    if (!dx[i].same_dim(x[i])) return false;
  }
  return true;
}

template <typename T>
bool state<T>::allocated_ddx() {
  if (x.size() != ddx.size()) return false;
  for (uint i = 0; i < x.size(); ++i) {
    if (!ddx[i].same_dim(x[i])) return false;
  }
  return true;
}

// slicing methods ///////////////////////////////////////////////////////////

template <typename T>
state<T> state<T>::select_state(int dimension, intg slice_index) {
  state<T> s;
  s.x.clear();
  s.forward_only = forward_only;
  // forward
  for (uint i = 0; i < x.size(); i++)
    if (x.exists(i))
      s.x.push_back_new(s.x[i].select(dimension, slice_index));
  s.link_f0();
  // backward
  for (uint i = 0; i < dx.size(); i++)
    if (dx.exists(i))
      s.dx.push_back_new(s.dx[i].select(dimension, slice_index));
  // bbackward
  for (uint i = 0; i < ddx.size(); i++)
    if (ddx.exists(i))
      s.ddx.push_back_new(s.ddx[i].select(dimension, slice_index));
  return s;
}

template <typename T>
state<T> state<T>::narrow_state(int d, intg sz, intg o) {
  state<T> s;
  s.x.clear();
  s.forward_only = forward_only;
  // forward
  for (uint i = 0; i < x.size(); i++)
    if (x.exists(i))
      s.x.push_back_new(s.x[i].narrow(d, sz, o));
  s.link_f0();
  // backward
  for (uint i = 0; i < dx.size(); i++)
    if (dx.exists(i))
      s.dx.push_back_new(s.dx[i].narrow(d, sz, o));
  // bbackward
  for (uint i = 0; i < ddx.size(); i++)
    if (ddx.exists(i))
      s.ddx.push_back_new(s.ddx[i].narrow(d, sz, o));
  return s;
}

template <typename T>
state<T> state<T>::narrow_state(intg size, intg offset) {
  if ((uint) (size + offset) > x.size())
    eblerror("cannot narrow this vector of size " << x.size()
             << " to size " << size << " starting at offset " << offset);
  state<T> s;
  s.x.clear();
  s.forward_only = forward_only;
  for (uint i = 0; i < size; ++i) {
    s.add_x(x.at(i + offset));
    if (i < dx.size()) s.dx.push_back(dx.at(i + offset));
    if (i < ddx.size()) s.ddx.push_back(ddx.at(i + offset));
  }
  return s;
}

template <typename T>
state<T> state<T>::narrow_state(mfidxdim &dims) {
  midxdim d = dims;
  return this->narrow_state(d);
}

template <typename T>
state<T> state<T>::narrow_state(midxdim &dims) {
  if (dims.size() != x.size())
    eblerror("expected same size input regions and states but got: "
             << dims << " and " << *this);
  state<T> all;
  all.x.clear();
  all.forward_only = forward_only;
  for (uint i = 0; i < dims.size(); ++i) {
    idx<T> in = x[i];
    idxdim d = dims[i];
    // narrow input, ignoring 1st dim
    for (uint j = 1; j < d.order(); ++j)
      in = in.narrow(j, d.dim(j), d.offset(j));
    all.add_x_new(in);
  }
  return all;
}

template <typename T>
state<T> state<T>::narrow_state_max(mfidxdim &dims) {
  if (dims.size() != x.size())
    eblerror("expected same size input regions and states but got: "
             << dims << " and " << *this);
  state<T> all;
  all.forward_only = forward_only;
  all.x.clear();
  for (uint i = 0; i < dims.size(); ++i) {
    idx<T> in = x[i];
    fidxdim d = dims[i];
    // narrow input, ignoring 1st dim
    for (uint j = 1; j < d.order(); ++j)
      in = in.narrow(j, (intg) std::min(in.dim(j) - d.offset(j), d.dim(j)),
                     (intg) d.offset(j));
    all.add_x_new(in);
  }
  return all;
}

template <typename T>
state<T> state<T>::get_x(int i) {
  if (!x.exists(i))
    eblerror("requesting an f state but tensor(s) not found at index "
             << i << " in f: " << x);
  state s(x[i]);
  s.forward_only = forward_only;
  return s;
}

template <typename T>
state<T> state<T>::get_dx(int i) {
  if (!x.exists(i) || !dx.exists(i))
    eblerror("requesting an f/b state but tensor(s) not found at index "
             << i << " in f: " << x << " and b: " << dx);
  state s(x[i], dx[i]);
  s.forward_only = forward_only;
  return s;
}

template <typename T>
state<T> state<T>::get_ddx(int i) {
  if (!x.exists(i) || !dx.exists(i) || !ddx.exists(i))
    eblerror("requesting an x/dx/ddx state but tensor(s) not found at index "
             << i << " in f: " << x << " and dx: " << dx << " and ddx: " << ddx);
  state s(x[i], dx[i], ddx[i]);
  s.forward_only = forward_only;
  return s;
}

template <typename T>
void state<T>::add_x(idx<T> &m) {
  x.push_back(m);
  // make sure the first element of 0 is same as main tensor
  if (x.size() == 1) link_f0();
}

template <typename T>
void state<T>::add_x(idx<T> *m) {
  x.push_back(m);
  // make sure the first element of 0 is same as main tensor
  if (x.size() == 1) link_f0();
}

template <typename T>
void state<T>::add_x(svector<idx<T> > &m) {
  for (uint i = 0; i < m.size(); ++i)
    add_x(m[i]);
}

template <typename T>
void state<T>::add_x_new(const idx<T> &m) {
  x.push_back_new(m);
  // make sure the first element of 0 is same as main tensor
  if (x.size() == 1) link_f0();
}

template <typename T>
void state<T>::add_x_new(svector<idx<T> > &m) {
  for (uint i = 0; i < m.size(); ++i)
    add_x_new(m[i]);
}

// copy methods //////////////////////////////////////////////////////////////

template <typename T>
state<T> state<T>::deep_copy() {
  state<T> other;
  resize_vidx_as(x, other.x);
  other.link_f0();
  resize_vidx_as(dx, other.dx);
  resize_vidx_as(ddx, other.ddx);
  state_copy(*this, other);
  return other;
}

template <typename T>
void state<T>::deep_copy(state<T> &s) {
  state_copy(s, *this);
}

template <typename T> template <typename T2>
void state<T>::deep_copy(midx<T2> &s) {
  x.clear();
  idx<T2> tmp;
  int i;
  switch (s.order()) {
    case 0:
      tmp = s.mget();
      add_x(new idx<T>(tmp.get_idxdim()));
      idx_copy(tmp, *(x.at_ptr(0)));
      break ;
    case 1:
      for (i = 0; i < s.dim(0); ++i) {
        tmp = s.mget(i);
        add_x(new idx<T>(tmp.get_idxdim()));
        idx_copy(tmp, *(x.at_ptr(0)));
      }
      break ;
    default: eblerror("not implemented");
  }
}

template <typename T>
void state<T>::shallow_copy(midx<T> &s) {
  x.clear();
  for (int i = 0; i < s.dim(0); ++i) {
    idx<T> tmp = s.mget(i);
    x.push_back_new(tmp);
  }
}

template <typename T>
midx<T> state<T>::shallow_copy_midx() {
  midx<T> s(x.size());
  for (uint i = 0; i < x.size(); ++i) s.mset(x[i], i);
  return s;
}

template <typename T>
void state<T>::get_midx(mfidxdim &dims, midx<T> &all) {
  // first narrow state
  state<T> n = narrow_state(dims);
  // now set all x into an midx
  all = midx<T>(n.x.size());
  for (uint i = 0; i < n.x.size(); ++i)
    all.mset(n.x[i], i);
}

template <typename T>
void state<T>::get_max_midx(mfidxdim &dims, midx<T> &all) {
  // first narrow state
  state<T> n = narrow_state_max(dims);
  // now set all x into an midx
  all = midx<T>(n.x.size());
  for (uint i = 0; i < n.x.size(); ++i)
    all.mset(n.x[i], i);
}

template <typename T>
void state<T>::get_padded_midx(mfidxdim &dims, midx<T> &all) {
  if (dims.size() != x.size())
    eblerror("expected same size input regions and states but got: "
             << dims << " and " << *this);
  all.clear();
  all.resize(dims.size_existing());
  uint ooff, ioff, osize, n = 0;
  bool bcopy;
  for (uint i = 0; i < dims.size(); ++i) {
    if (dims.exists(i)) {
      idx<T> in = x[i];
      idxdim d(dims[i]);
      d.setdim(0, in.dim(0));
      idx<T> out(d);
      idx_clear(out);
      all.mset(out, n);
      bcopy = true;
      // narrow input, ignoring 1st dim
      for (uint j = 1; j < d.order(); ++j) {
	// if no overlap, skip this state
	if (d.offset(j) >= in.dim(j) || d.offset(j) + d.dim(j) <= 0) {
          bcopy = false;
          break ;
	}
	// determine narrow params
	ooff = (uint) std::max(0, (int) - d.offset(j));
	ioff = (uint) std::max(0, (int) d.offset(j));
	osize = (uint) std::min(d.dim(j) - ooff, in.dim(j) - ioff);
	// narrow
	out = out.narrow(j, osize, ooff);
	in = in.narrow(j, osize, ioff);
      }
      // copy
      if (bcopy) idx_copy(in, out);
      n++;
    }
  }
}

// info printing methods /////////////////////////////////////////////////////

template <typename T>
void state<T>::pretty() const {
  eblprint( pretty_str() << std::endl);
}

template <typename T>
std::string state<T>::pretty_str() const {
  std::string s;
  s << "(x:" << to_string(x);
  if (!dx.empty()) s << ",dx:" << to_string(dx);
  if (!dx.empty()) s << ",ddx:" << to_string(ddx);
  s << ")";
  return s;
}

template <typename T>
std::string state<T>::str() const {
  std::string s = "(";
  // forward
  s << "x:";
  for (uint i = 0; i < x.size(); i++) {
    if (i == 0) s << idx<T>::str();
    else s << " " << x.at_const(i).str();
  }
  // backward
  s << ",dx:";
  for (uint i = 0; i < dx.size(); i++) s << " " << dx.at_const(i).str();
  // bbackward
  s << ",ddx:";
  for (uint i = 0; i < ddx.size(); i++) s << " " << ddx.at_const(i).str();
  s << ")";
  return s;
}

template <typename T>
void state<T>::print() const {
  eblprint( str() << std::endl);
}

template <typename T>
std::string state<T>::info() {
  std::string s = "(";
  // forward
  s << "x:";
  for (uint i = 0; i < x.size(); i++) {
    if (i == 0) s << idx<T>::info();
    else s << " " << x[i].info();
  }
  // backward
  s << ",dx:";
  for (uint i = 0; i < dx.size(); i++)
    s << " " << dx[i].info();
  // bbackward
  s << ",ddx:";
  for (uint i = 0; i < ddx.size(); i++)
    s << " " << ddx[i].info();
  s << ")";
  return s;
}

// protected methods /////////////////////////////////////////////////////////

template<typename T>
void state<T>::init() {
  forward_only = false;
  // add main tensor as x[0]
  x.parent = this;
  x.push_back(this);
}

template<typename T>
void state<T>::allocate(const idxdim &d, parameter<T> *p) {
  // allocate main forward tensor
  ((idx<T>&)*this) = idx<T>(p ? p->getstorage() : NULL,
                            p ? p->footprint() : 0, d);
  // allocate dx and ddx tensors if present in p
  if (p) {
    if (!p->dx.empty())
      dx.push_back(new idx<T>(p->dx[0].getstorage(), p->dx[0].footprint(), d));
    if (!p->ddx.empty())
      ddx.push_back(new idx<T>(p->ddx[0].getstorage(), p->ddx[0].footprint(), d));
  }
  // resize parameter accordingly
  if (p) p->resize_parameter(p->footprint() + idx<T>::nelements());
  // clear all allocated tensors
  zero_all();
}

template <typename T>
void state<T>::link_f0() {
  // only link if not already the case
  if (x.at_ptr(0) != (idx<T>*) this) {
    ((idx<T>&)*this) = x[0];
    link_main_to_f0();
  }
}

template <typename T>
void state<T>::link_main_to_f0() {
  x.set((idx<T>&)*this, 0); // f0 is a reference to the main tensor
}

template <typename T>
void state<T>::resize_vidx_as(svector<idx<T> > &in, svector<idx<T> > &out) {
  bool reset = false;
  bool resize = false;
  // reallocate entirely if different number of tensors
  if (in.size() != out.size()) reset = true;
  // reallocate if some orders are different
  // only resize if some dimensions are different
  if (in.size() == out.size()) {
    for (uint i = 0; i < in.size() && !reset; ++i) {
      if (in[i].order() != out[i].order()) reset = true;
      if (!in[i].same_dim(out[i])) resize = true;
    }
  }
  // reset/resize if necessary
  if (reset) reset_tensors(in, out);
  else if (resize) {
    for (uint i = 0; i < in.size(); ++i)
      if (in.exists(i)) out[i].resize(in[i].get_idxdim());
  }
}

template <typename T>
void state<T>::resize_vidx_orders(svector<idx<T> > &in, svector<idx<T> > &out,
                                  intg dim_size, intg n) {
  bool reset = false;
  // check we have the right number of tensors
  if (n <= 0) n = in.size();
  if ((intg) in.size() > n && (intg) out.size() < n) reset = true;
  if ((intg) in.size() <= n && out.size() != in.size()) reset = true;
  // check that all tensors have the right orders
  for (uint i = 0; !reset && i < in.size() && (intg) i < n; ++i)
    if (in[i].order() != out[i].order()) reset = true;
  // re-allocate if necessary
  if (reset) reset_tensors(in, out, dim_size, n);
}

template <typename T>
void state<T>::reset_tensors(svector<idx<T> > &in, svector<idx<T> > &out,
                             intg dim_size, intg n) {
  if (in.empty()) eblerror("expected non-empty input");
  if (n <= 0) n = (intg) in.size();
  out.clear();
  idxdim d;
  for (intg i = 0; i < n; ++i) {
    if (i < (intg) in.size()) {
      d = in[i].get_idxdim();
      if (dim_size > 0) d.setdims(dim_size);
    }
    out.push_back(new idx<T>(d));
  }
}

// state operations //////////////////////////////////////////////////////////

template <typename T, typename T2>
void state_copy(state<T> &in, state<T2> &out) {
  if (in.x.empty()) eblerror("expected at least 1 forward tensor");
  out.forward_only = in.forward_only;
  // forward
  for (uint i = 0; i < in.x.size(); i++)
    if (in.x.exists(i)) {
      // increase number of output tensors if necessary
      if (i >= out.x.size()) out.add_x(new idx<T2>(in.x[i].get_idxdim()));
      idx_copy(in.x[i], out.x[i]);
    } else out.x.push_back_empty();
  // backward
  for (uint i = 0; i < in.dx.size(); i++)
    if (in.dx.exists(i)) {
      // increase number of output tensors if necessary
      if (i >= out.x.size()) out.x.push_back(new idx<T2>(in.x[i].get_idxdim()));
      idx_copy(in.dx[i], out.dx[i]);
    } else out.dx.push_back_empty();
  // bbackward
  for (uint i = 0; i < in.ddx.size(); i++)
    if (in.ddx.exists(i)) {
      // increase number of output tensors if necessary
      if (i >= out.x.size()) out.x.push_back(new idx<T2>(in.x[i].get_idxdim()));
      idx_copy(in.ddx[i], out.ddx[i]);
    } else out.ddx.push_back_empty();
}

// stream operators //////////////////////////////////////////////////////////

template <typename T, class stream>
EXPORT stream& operator<<(stream &out, const state<T> &s) {
  out << s.pretty_str();
  return out;
}

template <typename T, class stream>
EXPORT stream& operator<<(stream &out, const svector<state<T> > &v) {
  std::string s = "[ ";
  for (typename svector<state<T> >::const_iterator i = v.begin();
       i != v.end(); ++i) {
    if (i.exists()) s << i->str() << " ";
    else s << " null ";
  }
  s << "]";
  out << s;
  return out;
}

template <typename T, class stream>
EXPORT stream& operator<<(stream &out, svector<state<T> > &v) {
  std::string s = "[ ";
  for (typename svector<state<T> >::iterator i = v.begin();
       i != v.end(); ++i) {
    if (i.exists()) s << i->str() << " ";
    else s << " null ";
  }
  s << "]";
  out << s;
  return out;
}

// state looper //////////////////////////////////////////////////////////////

template <typename T>
state_looper<T>::state_looper(state<T> &s, int d)
    : lx(s.x[0], d), ldx(s.dx[0], d), lddx(s.ddx[0], d) {
  ((state<T>&)*this) = s.select(d, 0);
}

template <typename T>
state_looper<T>::~state_looper() {
}

template <typename T>
void state_looper<T>::next() {
  lx.next();
  ldx.next();
  lddx.next();
  state<T>::x[0] = lx;
  this->link_f0();
  state<T>::dx[0] = ldx;
  state<T>::ddx[0] = lddx;
}

template <typename T>
bool state_looper<T>::notdone() {
  return lx.notdone();
}

} // end namespace ebl
