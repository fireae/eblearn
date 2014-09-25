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

#ifndef SMART_HPP_
#define SMART_HPP__

namespace ebl {

// smart vector //////////////////////////////////////////////////////////////

template <class T>
svector<T>::svector() {
}

template <class T>
svector<T>::svector(const svector<T> &other) {
  this->copy(other);
  //EDEBUG("copied " << other << " into this: " << *this);
}

template <class T>
svector<T>::svector(uint n) : std::vector<T*>(n, NULL) {
}

template <class T>
svector<T>::~svector() {
  clear();
}

template <class T>
void svector<T>::clear() {
  unlock_all();
  std::vector<T*>::clear();
}

template <class T>
void svector<T>::clear(uint i) {
  T *e = std::vector<T*>::operator[](i);
  if (e) {
    //EDEBUG("clearing element " << i << " " << *e);
    e->unlock();
    std::vector<T*>::operator[](i) = NULL;
  }
}

template <class T>
T& svector<T>::operator[](uint i) {
#ifdef __DEBUG__
  if (i >= this->size())
    eblerror("trying to access element " << i << " in vector of size "
             << this->size());
#endif
  return *(std::vector<T*>::operator[](i));
}

// template <class T>
// const T& svector<T>::operator[](uint i) const {
//   return *(std::vector<T*>::operator[](i));
// }

template <class T>
const T& svector<T>::at_const(uint i) const {
  return *(std::vector<T*>::operator[](i));
}

template <class T>
T* svector<T>::at_ptr(uint i) const {
  return std::vector<T*>::operator[](i);
}

template <class T>
bool svector<T>::empty() const {
  return this->size() == 0;
}

template <class T>
bool svector<T>::exists(uint i) const {
  if (i >= std::vector<T*>::size()) return false;
  return std::vector<T*>::operator[](i) != NULL;
}

template <class T>
uint svector<T>::size_existing() const {
  uint n = 0;
  typename std::vector<T*>::const_iterator cit = std::vector<T*>::begin();
  for (; cit != std::vector<T*>::end(); ++cit)
    if (*cit != NULL) n++;
  return n;
}

template <class T>
void svector<T>::remove(uint i) {
  if (i >= this->size())
    eblerror("cannot remove element " << i << " when vector has only "
             << this->size() << " elements");
  it = this->begin() + i;
  if (*it != NULL) (*it)->unlock();
  this->erase(it);
}

template <class T>
void svector<T>::remove(uint start, uint end) {
  if (end >= this->size())
    eblerror("cannot remove elements from " << start << " to " << end
             << " when vector has only " << this->size() << " elements");
  unlock_range(start, end);
  it = this->begin() + start;
  typename std::vector<T*>::iterator itend = this->begin() + end;
  itend++; // include last element
  this->erase(it, itend);
}

template <class T>
void svector<T>::remove_empty() {
  for (uint i = 0; i < this->size(); ) {
    if (!exists(i)) this->remove(i);
    else i++;
  }
}

template <class T>
void svector<T>::push_front(T &e) {
  e.lock();
  std::vector<T*>::insert(std::vector<T*>::begin(), 1, &e);
}

template <class T>
void svector<T>::push_front_new(T &e) {
  T *n = new T(e);
  n->lock();
  std::vector<T*>::insert(std::vector<T*>::begin(), 1, n);
}

template <class T>
void svector<T>::push_back(T *e) {
  if (e) e->lock();
  std::vector<T*>::push_back(e);
}

template <class T>
void svector<T>::push_back(T &e) {
  e.lock();
  std::vector<T*>::push_back(&e);
}

template <class T>
void svector<T>::push_back_new(const T &e) {
  T *n = new T(e);
  //EDEBUG("pushing back " << e << " into new element " << *n);
  n->lock();
  std::vector<T*>::push_back(n);
}

template <class T>
void svector<T>::push_back_new(const T *e) {
  T *n = new T(*e);
  //EDEBUG("pushing back " << e << " into new element " << *n);
  n->lock();
  std::vector<T*>::push_back(n);
}

template <class T>
void svector<T>::push_back(std::vector<T*> &v) {
  for (it = v.begin(); it != v.end(); ++it) {
    (*it)->lock();
    std::vector<T*>::push_back(*it);
  }
}

template <class T>
void svector<T>::push_back(svector<T> &v) {
  for (it = v.begin(); it != v.end(); ++it) {
    if (*it != NULL) {
      (*it)->lock();
      std::vector<T*>::push_back(*it);
    } else push_back_empty();
  }
}

template <class T>
void svector<T>::push_back_new(const svector<T> &v) {
  typename std::vector<T*>::const_iterator cit = v.begin();
  for ( ; cit != v.end(); ++cit) {
    if (*cit != NULL) push_back_new(*cit);
    else push_back_empty();
  }
}

template <class T>
void svector<T>::set(T &o, uint i) {
  T* &e = this->at(i);
  if (e) e->unlock();
  o.lock();
  e = &o;
}

template <class T>
void svector<T>::set_new(T &o, uint i) {
  T* &e = this->at(i);
  if (e) e->unlock();
  T *n = new T(o);
  n->lock();
  e = n;
}

// template <class T>
// svector<T>& svector<T>::operator=(svector<T> &other) {
//   if ((void*) &other != (void*) this) {
//     this->clear();
//     this->push_back(other);
//   }
//   return *this;
// }

template <class T>
void svector<T>::operator=(svector<T> other) {
  if ((void*) &other != (void*) this) {
    this->clear();
    this->push_back(other);
  }
  // return *this;
}

template <class T>
void svector<T>::push_back_empty() {
  std::vector<T*>::push_back(NULL);
}

template <class T>
void svector<T>::swap(uint i, uint j) {
  T *tmp = std::vector<T*>::at(j);
  std::vector<T*>::at(j) = std::vector<T*>::at(i);
  std::vector<T*>::at(i) = tmp;
}

template <class T>
void svector<T>::permute(std::vector<uint> &permutations) {
  if (permutations.size() > this->size())
    eblerror("expected permutation vector " << permutations
             << " to be smaller than vector of size " << this->size());
  std::vector<bool> swapped(permutations.size(), false);
  for (uint i = 0; i < permutations.size(); ++i)
    if (!swapped[i] && i != permutations[i]) {
      this->swap(i, permutations[i]);
      swapped[i] = true;
      swapped[permutations[i]] = true;
    }
}

template <class T>
svector<T> svector<T>::copy() {
  svector<T> c;
  for (it = std::vector<T*>::begin(); it != std::vector<T*>::end(); ++it)
    c.push_back(*it);
  return c;
}

template <class T>
void svector<T>::copy(const svector<T> &other) {
  this->clear();
  for (typename svector<T>::const_iterator i = other.begin();
       i != other.end(); ++i)
    if (i.exists()) this->push_back_new(*i);
    else this->push_back_empty();
}

template <class T>
void svector<T>::copy(svector<T> &other) {
  this->clear();
  for (typename svector<T>::iterator i = other.begin();
       i != other.end(); ++i)
    if (i.exists()) this->push_back(i.ptr());
    else this->push_back_empty();
}

template <class T>
svector<T> svector<T>::narrow(uint n, uint offset) {
  if (offset + n > this->size())
    eblerror("out-of-bounds narrow of size " << n << " starting at offset "
             << offset << " in vector of size " << this->size());
  svector<T> m;
  for (typename svector<T>::iterator i = this->begin() + offset;
       i != this->begin() + offset + n; ++i)
    if (i.exists()) m.push_back(*i);
    else m.push_back_empty();
  return m;
}

template <class T>
void svector<T>::resize_default(uint n) {
  for (uint i = this->size(); i < n; ++i)
    this->push_back(new T);
}

template <class T>
void svector<T>::randomize() {
  typename std::vector<T*>::iterator b = ((std::vector<T*>*)this)->begin();
  typename std::vector<T*>::iterator e = ((std::vector<T*>*)this)->end();
  std::random_shuffle(b, e);
}

template <class T>
typename svector<T>::iterator svector<T>::begin(uint off) {
  return iterator(std::vector<T*>::begin() + off);
}

template <class T>
typename svector<T>::const_iterator svector<T>::begin(uint off) const {
  return const_iterator(std::vector<T*>::begin() + off);
}

// internal methods //////////////////////////////////////////////////////////

template <class T>
void svector<T>::lock_all() {
  for (it = this->begin(); it != this->end(); ++it)
    if (*it != NULL) (*it)->lock();
}

template <class T>
void svector<T>::unlock_all() {
  for (it = this->begin(); it != this->end(); ++it)
    if (*it != NULL) (*it)->unlock();
}

template <class T>
void svector<T>::unlock_range(uint start, uint end) {
  for (it = this->begin() + start; it != this->begin() + end; ++it)
    if (*it != NULL) (*it)->unlock();
}

// svector iterator //////////////////////////////////////////////////////////

template <class T>
svector<T>::iterator::iterator() {
}

template <class T>
svector<T>::iterator::iterator(typename std::vector<T*>::iterator &it)
    : std::vector<T*>::iterator(it) {
}

template <class T>
svector<T>::iterator::iterator(typename std::vector<T*>::iterator it)
    : std::vector<T*>::iterator(it) {
}

template <class T>
svector<T>::iterator::~iterator() {
}

template <class T>
T& svector<T>::iterator::operator*() {
#if __WINDOWS__ == 1
  return **(this->_Ptr);
#else
  return **(this->_M_current);
#endif
}

template <class T>
T* svector<T>::iterator::operator->() {
#if __WINDOWS__ == 1
  return *(this->_Ptr);
#else
  return *(this->_M_current);
#endif
}

template <class T>
T* svector<T>::iterator::ptr() {
#if __WINDOWS__ == 1
  return *(this->_Ptr);
#else
  return *(this->_M_current);
#endif
}

template <class T>
bool svector<T>::iterator::exists() const {
#if __WINDOWS__ == 1
  return *(this->_Ptr) != NULL;
#else
  return *(this->_M_current) != NULL;
#endif
}

// svector const_iterator ////////////////////////////////////////////////////

template <class T>
svector<T>::const_iterator::const_iterator() {
}

template <class T>
svector<T>::const_iterator::
const_iterator(typename std::vector<T*>::const_iterator &it)
    : std::vector<T*>::const_iterator(it) {
}

template <class T>
svector<T>::const_iterator::
const_iterator(typename std::vector<T*>::const_iterator it)
    : std::vector<T*>::const_iterator(it) {
}

template <class T>
svector<T>::const_iterator::~const_iterator() {
}

template <class T>
const T& svector<T>::const_iterator::operator*() const {
#if __WINDOWS__ == 1
  return **(this->_Ptr);
#else
  return **(this->_M_current);
#endif
}

template <class T>
const T* svector<T>::const_iterator::operator->() const {
#if __WINDOWS__ == 1
  return *(this->_Ptr);
#else
  return *(this->_M_current);
#endif
}

template <class T>
const T* svector<T>::const_iterator::ptr() const {
#if __WINDOWS__ == 1
  return *(this->_Ptr);
#else
  return *(this->_M_current);
#endif
}

template <class T>
bool svector<T>::const_iterator::exists() const {
#if __WINDOWS__ == 1
  return *(this->_Ptr) != NULL;
#else
  return *(this->_M_current) != NULL;
#endif
}

// printing //////////////////////////////////////////////////////////////////

template <class T>
std::string svector<T>::str() const {
  std::string s = "[ ";
  for (typename svector<T>::const_iterator i = this->begin();
       i != this->end(); ++i) {
    if (i.exists()) s << (const T&)*i << " ";
    else s << " null ";
  }
  s << "]";
  return s;
}

template <class T, class stream>
stream& operator<<(stream &o, const svector<T> &v) {
  o << v.str();
  return o;
}

template <class T, class stream>
stream& operator<<(stream &o, svector<T> &v) {
  o << v.str();
  return o;
}

} // namespace ebl

#endif /* SMART_HPP_ */
