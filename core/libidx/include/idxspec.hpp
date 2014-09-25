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

#ifndef IDXSPEC_HPP_
#define IDXSPEC_HPP_

namespace ebl {

////////////////////////////////////////////////////////////////
// idxdim: constructors

template <typename T>
idxd<T>::~idxd() {
  if (offsets) {
    delete[] offsets;
  }
}

template <typename T>
idxd<T>::idxd() : offsets(NULL) {
  ndim = -1;
  memset(dims, -1, MAXDIMS * sizeof (T));
}

template <typename T> template <class T2>
idxd<T>::idxd(const idx<T2> &i) : offsets(NULL) {
  setdims(i.spec);
}

template <typename T> template <class T2>
idxd<T>::idxd(const idxd<T2> &i) : offsets(NULL) {
  *this = i;
}

template <typename T>
idxd<T>::idxd(const idxspec &s) : offsets(NULL) {
  setdims(s);
}

template <typename T>
idxd<T>::idxd(const idxd<T> &s) : offsets(NULL) {
  setdims(s);
}

template <typename T>
idxd<T>::idxd(T s0, T s1, T s2, T s3, T s4, T s5, T s6, T s7)
    : offsets(NULL) {
  dims[0] = s0; dims[1] = s1; dims[2] = s2; dims[3] = s3;
  dims[4] = s4; dims[5] = s5; dims[6] = s6; dims[7] = s7;
  ndim = 0;
  for (int i = 0; i < MAXDIMS; i++)
    if (dims[i] >= 0) ndim++;
    else break;
}

////////////////////////////////////////////////////////////////
// idxdim: set dimensions

template <typename T> template <class Tidx>
void idxd<T>::setdims(const idx<Tidx> &i) {
  setdims(i.spec);
}

template <typename T>
void idxd<T>::setdims(const idxspec &s) {
  ndim = s.ndim;
  // copy existing dimensions
  for (int i = 0; i < ndim; ++i)
    dims[i] = (T) s.dim[i];
  // set remaining to -1
  int ord = std::max((int) 0, s.ndim);
  memset(dims + ord, -1, (MAXDIMS - ord) * sizeof (T));
}

template <typename T>
void idxd<T>::setdims(const idxd<T> &s) {
  ndim = s.order();
  // copy existing dimensions
  for (int i = 0; i < s.order(); ++i)
    dims[i] = s.dim(i);
  // set remaining to -1
  intg ord = std::max((intg) 0, s.order());
  memset(dims + ord, -1, (MAXDIMS - ord) * sizeof (T));
  // copy all offsets if exist
  if (s.offsets) {
    if (!offsets)
      offsets = new T[MAXDIMS];
    memcpy(offsets, s.offsets, MAXDIMS * sizeof(T));
  } else if (offsets) { // no offsets, delete existing
    delete[] offsets;
    offsets = NULL;
  }
}

template <typename T>
void idxd<T>::setdims(T n) {
  for (int i = 0; i < ndim; ++i)
    dims[i] = n;
}

template <typename T>
void idxd<T>::insert_dim(intg pos, T dim_size) {
  if (ndim + 1 > MAXDIMS)
    eblerror("error: cannot add another dimension to dim."
             << " Maximum number of dimensions (" << MAXDIMS << ") reached.");
        // check that dim_size is valid
        if (dim_size <= 0)
          eblerror("cannot set negative or zero dimension");
  // check that all dimensions up to pos (excluded) are > 0.
  for (uint i = 0; i < pos; ++i)
    if (dims[i] <= 0)
      eblerror("error: cannot insert dimension " << pos
               << " after empty dimensions: " << *this);
  // add order of 1
  ndim++;
  if (ndim == 0) // one more if it was empty
    ndim++;
  // shift all dimensions until position pos
  for (uint i = ndim - 1; i > pos && i >= 1; i--)
    dims[i] = dims[i - 1];
  if (offsets)
    for (uint i = ndim - 1; i > pos && i >= 1; i--)
      offsets[i] = offsets[i - 1];
  // insert new dim
  dims[pos] = dim_size;
  if (offsets)
    offsets[pos] = 0;
}

template <typename T>
T idxd<T>::remove_dim(intg pos) {
  // check that dim_size is valid
  if (ndim == 0)
    eblerror("not enough dimensions for removing one in " << *this);
  T rdim = dim(pos);
  // shift all dimensions until position pos
  for (uint i = pos; i < ndim - 1; i++)
    dims[i] = dims[i + 1];
  dims[ndim - 1] = -1; // empty last dimension
  if (offsets) {
    for (uint i = pos; i < ndim - 1; i++)
      offsets[i] = offsets[i + 1];
    offsets[ndim - 1] = 0; // empty last offset
  }
  // decrease order by 1
  ndim--;
  return rdim;
}

template <typename T>
void idxd<T>::remove_trailing_dims() {
  for (int i = ndim-1; i >= 0; --i)
    if (dim(i) == 1) remove_dim(i);
    else break ;
}

template <typename T>
void idxd<T>::setdim(intg dimn, T size) {
  if (dimn >= ndim)
    eblerror("error: trying to set dimension " << dimn << " to size "
             << size << " but idxidm has only " << ndim
             << " dimension(s): " << *this);
  dims[dimn] = size;
}

template <typename T>
void idxd<T>::setoffset(intg dimn, T offset) {
  if (dimn >= ndim)
    eblerror("error: trying to set offset of dim " << dimn << " to "
             << offset << " but idxidm has only " << ndim
             << " dimension(s): " << *this);
  // allocate if not allocated
  if (!offsets) {
    offsets = new T[MAXDIMS];
    memset(offsets, 0, MAXDIMS * sizeof (T));
  }
  offsets[dimn] = offset;
}

template <typename T>
bool idxd<T>::has_offsets() const {
  return offsets != NULL;
}

template <typename T>
void idxd<T>::set_max(const idxd<T> &other) {
  if (other.order() != ndim)
    eblerror("expected same order in " << *this << " and " << other);
  for (uint i = 0; i < ndim; i++)
    dims[i] = std::max(dims[i], other.dim(i));
}

template <typename T>
void idxd<T>::shift_dim(int d, int pos) {
  T dims2[MAXDIMS];
  for (int i = 0, j = 0; i < MAXDIMS; ++i) {
    if (i == pos)
      dims2[i] = dims[d];
    else {
      if (j == d)
        j++;
      dims2[i] = dims[j++];
    }
  }
  memcpy(dims, dims2, MAXDIMS * sizeof (T));
  if (offsets)
    eblerror("not implemented (TODO)");
}

//////////////////////////////////////////////////////////////////////////////
// get dimensions

template <typename T>
intg idxd<T>::order() const {
  return ndim;
}

template <typename T>
bool idxd<T>::empty() const {
  return ndim == -1;
}

template <typename T>
T idxd<T>::dim(intg dimn) const {
  if (dimn >= ndim)
    eblerror("trying to access size of dimension " << dimn
             << " but idxdim's maximum dimensions is " << ndim);
  return dims[dimn];
}

template <typename T>
T idxd<T>::maxdim() const {
  T m = 0;
  for (intg i = 0; i < ndim; ++i)
    if (m < dims[i]) m = dims[i];
  return m;
}

template <typename T>
T idxd<T>::offset(intg dimn) const {
  if (dimn >= ndim)
    eblerror("trying to access size of dimension " << dimn
             << " but idxdim's maximum dimensions is " << ndim);
  if (offsets)
    return offsets[dimn];
  else
    return 0;
}

template <typename T>
bool idxd<T>::operator==(const idxd<T>& other) {
  if (other.ndim != ndim)
    return false;
  for (int i = 0; i < ndim; ++i)
    if (other.dim(i) != dim(i))
      return false;
  return true;
}

template <typename T>
bool idxd<T>::operator!=(const idxd<T>& other) {
  return !(*this == other);
}

//////////////////////////////////////////////////////////////////////////////
// operators

// template <typename T>
// idxd<T>& idxd<T>::operator=(const idxd<T> &d2) {
//   if ((void*)this != (void*)&d2) { // protect against invalid self-assignment
//     setdims(d2);
//   }
//   EDEBUG("this " << this << " " << *this << " <- d2 " << &d2 << " " << d2);
//   return *this;
// }

template <typename T>
idxd<T>& idxd<T>::operator=(idxd<T> d2) {
  setdims(d2);
  return *this;
}

template <typename T> template <typename T2>
idxd<T>& idxd<T>::operator=(const idxd<T2> &d2) {
  if ((void*)this != (void*)&d2) { // protect against invalid self-assignment
    ndim = d2.order();
    for (intg i = 0; i < ndim; ++i)
      dims[i] = (T) d2.dim(i);
    if (offsets && !d2.has_offsets()) {
      delete[] offsets;
      offsets = NULL;
    } else if (d2.has_offsets()) {
      if (!offsets)
        offsets = new T[MAXDIMS];
      for (intg i = 0; i < d2.order(); ++i)
        offsets[i] = (T) d2.offset(i);
    }
  }
  return *this;
}

template <typename T>
idxd<T> idxd<T>::operator*(const idxd<T> &d2) const {
  idxd<T> d = *this;
  if (d2.order() != d.order())
    eblerror("expected same order idxd but got " << d << " and " << d2);
  for (int i = 0; i < d.order(); ++i)
    d.setdim(i, (T) (d.dim(i) * d2.dim(i)));
  if (offsets)
    for (int i = 0; i < d.order(); ++i)
      d.setoffset(i, (T) (d.offset(i) * d2.dim(i)));
  return d;
}

template <typename T> template <typename T2>
idxd<T> idxd<T>::operator*(const idxd<T2> &d2) const {
  idxd<T> d = *this;
  if (d2.order() != d.order())
    eblerror("expected same order idxd but got " << d << " and " << d2);
  for (int i = 0; i < d.order(); ++i)
    d.setdim(i, (T) ((T2)(d.dim(i)) * d2.dim(i)));
  if (offsets)
    for (int i = 0; i < d.order(); ++i)
      d.setoffset(i, (T) ((T2)(d.offset(i)) * d2.dim(i)));
  return d;
}

template <typename T> template <typename T2>
idxd<T> idxd<T>::operator*(idxd<T2> &d2) {
  idxd<T> d = *this;
  if (d2.order() != d.order())
    eblerror("expected same order idxd but got " << d << " and " << d2);
  for (int i = 0; i < d.order(); ++i)
    d.setdim(i, (T) ((T2)(d.dim(i)) * d2.dim(i)));
  if (offsets)
    for (int i = 0; i < d.order(); ++i)
      d.setoffset(i, (T) ((T2)(d.offset(i)) * d2.dim(i)));
  return d;
}

template <typename T> template <typename T2>
idxd<T> idxd<T>::operator*(T2 f) {
  idxd<T> d(*this);
  for (int i = 0; i < d.order(); ++i)
    d.setdim(i, (T) ((T2)(d.dim(i)) * f));
  if (offsets)
    for (int i = 0; i < d.order(); ++i)
      d.setoffset(i, (T) ((T2)(d.offset(i)) * f));
  return d;
}

template <typename T> template <typename T2>
idxd<T> idxd<T>::operator+(T2 f) {
  idxd<T> d = *this;
  for (int i = 0; i < d.order(); ++i)
    d.setdim(i, (T) ((T2)(d.dim(i)) + f));
  return d;
}

template <typename T>
idxd<T> idxd<T>::operator+(idxd<T> &d2) {
  idxd<T> d = *this;
  if (d.order() != d2.order())
    eblerror("cannot add two idxdim with different orders: " << d << " and "
             << d2);
  for (int i = 0; i < d.order(); ++i)
    d.setdim(i, d.dim(i) + d2.dim(i));
  return d;
}

template <typename T>
bool idxd<T>::operator<=(idxd<T> &d2) {
  if (this->order() != d2.order())
    eblerror("cannot add two idxdim with different orders: " << *this
             << " and " << d2);
  for (int i = 0; i < this->order(); ++i)
    if (this->dim(i) > d2.dim(i))
      return false;
  return true;
}

template <typename T>
bool idxd<T>::operator>=(idxd<T> &d2) {
  if (this->order() != d2.order())
    eblerror("cannot add two idxdim with different orders: " << *this
             << " and " << d2);
  for (int i = 0; i < this->order(); ++i)
    if (this->dim(i) < d2.dim(i))
      return false;
  return true;
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
intg idxd<T>::nelements() {
  intg total = 1;
  for (int i = 0; i < ndim; ++i)
    total *= dim(i);
  return total;
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
std::ostream& operator<<(std::ostream& out, const idxd<T>& d) {
  std::string s;
  s << d;
  out << s;
  return out;
}

template <typename T>
std::string& operator<<(std::string& out, const idxd<T>& d) {
  std::stringstream outstring;
  //outstring << out;
  if (d.order() <= 0)
    outstring << "<empty>";
  else {
    if (d.offsets) {
      bool show = false;
      for (int i = 0; i < d.order(); ++i)
        if (d.offset(i) != 0) {
          show = true;
          break;
        }
      if (show) {
        outstring << "(";
        outstring << d.offset(0);
        for (int i = 1; i < d.order(); ++i)
          outstring << "," << d.offset(i);
        outstring << ")";
      }
    }
    outstring << d.dim(0);
    for (int i = 1; i < d.order(); ++i)
      outstring << "x" << d.dim(i);
  }
  out <<  outstring.str();
  return out;
}
// midxdim ///////////////////////////////////////////////////////////////////

template <typename T>
midxd<T>::midxd() {
}

template <typename T>
midxd<T>::midxd(uint n) : svector<idxd<T> >(n) {
}

template <typename T>
midxd<T>::~midxd() {
}

template <typename T>
midxd<T>::midxd(const idxd<T> &s) {
  push_back(s);
}

template <typename T> template <typename T2>
midxd<T>::midxd(const midxd<T2> &s) {
  *this = s;
}

template <typename T>
midxd<T> midxd<T>::narrow_copy(uint n, uint offset) {
  if (offset + n > this->size())
    eblerror("out-of-bounds narrow of size " << n << " starting at offset "
             << offset << " in " << *this);
  midxd<T> m;
  for (typename midxd<T>::iterator i = this->begin() + offset;
       i != this->begin() + offset + n; ++i)
    if (i.exists()) m.push_back(*i);
    else m.push_back_empty();
  return m;
}

template <typename T> template <typename T2>
void midxd<T>::push_back(const idxd<T2> &m) {
  svector<idxd<T> >::push_back(new idxd<T>(m));
}

template <typename T>
void midxd<T>::push_back(const midxd<T> &m) {
  for (typename midxd<T>::const_iterator i = m.begin(); i != m.end(); ++i) {
    if (i.exists())
      svector<idxd<T> >::push_back(new idxd<T>(*i));
    else
      this->push_back_empty();
  }
}

// template <typename T>
// void midxd<T>::push_back(midxd<T> &m) {
//   for (typename midxd<T>::iterator i = m.begin(); i != m.end(); ++i)
//     svector<idxd<T> >::push_back((*i);
// }

template <typename T>
midxd<T> midxd<T>::merge_all() {
  midxd<T> m;
  if (this->size() == 0) return m;
  // add first element
  idxd<T> &e0 = (*this)[0];
  m.push_back(e0);
  // check that remaining ones are the same
  for (typename midxd<T>::iterator i = this->begin(1); i != this->end(); ++i){
    if (((idxd<T>&)*i) != e0)
      eblerror("expected all elements to be the same in " << *this);
  }
  return m;
}

template <typename T> template <typename T2>
midxd<T>& midxd<T>::operator=(const midxd<T2> &other) {
  if ((void*) &other != (void*) this) {
    this->clear();
    for (typename midxd<T2>::const_iterator i = other.begin();
         i != other.end(); ++i) {
      if (i.exists()) this->push_back(*i);
      else this->push_back_empty();
    }
  }
  return *this;
}

// printing //////////////////////////////////////////////////////////////////

template<class T>
std::string& operator<<(std::string &o, const midxd<T> &v) {
  o << "[ ";
  for (typename midxd<T>::const_iterator i = v.begin();
       i != v.end(); ++i) {
    if (i.exists())
      o << *i << " ";
    else
      o << " null ";
  }
  o << "]";
  return o;
}

template<class T>
std::ostream& operator<<(std::ostream &o, const midxd<T> &v) {
  o << "[ ";
  for (typename midxd<T>::const_iterator i = v.begin();
       i != v.end(); ++i) {
    if (i.exists())
      o << *i << " ";
    else
      o << " null ";
  }
  o << "]";
  return o;
}

} // namespace ebl

#endif /* IDXSPEC_HPP_ */
