/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef STL_HPP_
#define STL_HPP_

namespace std {

// string //////////////////////////////////////////////////////////////////////

#ifndef __NOSTL__ // extending STL

template<class T>
string& operator<<(string& e, const T v) {
  e += v;
  return e;
}

#endif

// ostream /////////////////////////////////////////////////////////////////////

#ifdef __NOSTL__

template<class T>
ostream& ostream::operator<<(const T &v) {
  eblerror("ostream not implemented for this type");
  return *this;
}

template<class T>
ostream& ostream::operator<<(T &v) {
  eblerror("ostream not implemented for this type");
  return *this;
}

#endif /* __NOSTL__ */

template<class T>
string& operator<<(string &o, const vector<T> &v) {
  o << "[ ";
  for (typename vector<T>::const_iterator i = v.begin();
       i != v.end(); ++i)
    o << *i << " ";
  o << "]";
  return o;
}

template<class T>
ostream& operator<<(ostream &o, const vector<T> &v) {
  o << "[ ";
  for (typename vector<T>::const_iterator i = v.begin();
       i != v.end(); ++i)
    o << *i << " ";
  o << "]";
  return o;
}

template<class T>
string& operator<<(string &o, const list<T> &v) {
  o << "[ ";
  for (typename list<T>::const_iterator i = v.begin();
       i != v.end(); ++i)
    o << *i << " ";
  o << "]";
  return o;
}

template<class T>
ostream& operator<<(ostream &o, const list<T> &v) {
  o << "[ ";
  for (typename list<T>::const_iterator i = v.begin();
       i != v.end(); ++i)
    o << *i << " ";
  o << "]";
  return o;
}

template <typename T1, typename T2>
string& operator<<(string &o, const map<T1,T2> &v) {
  o << "[ ";
  for (typename map<T1,T2>::const_iterator i = v.begin();
       i != v.end(); ++i)
    o << i->first << "=" << i->second << " ";
  o << "]";
  return o;
}

template <typename T1, typename T2>
ostream& operator<<(ostream &o, const map<T1,T2> &v) {
  o << "[ ";
  for (typename map<T1,T2>::const_iterator i = v.begin();
       i != v.end(); ++i)
    o << i->first << "=" << i->second << " ";
  o << "]";
  return o;
}

// vector //////////////////////////////////////////////////////////////////////

#ifdef __NOSTL__

template <typename T>
vector<T>::vector() : ebl::idx<T>((ebl::intg)0) {
}

template <typename T>
vector<T>::vector(size_t n, const T value) : ebl::idx<T>((ebl::intg)n) {
  idx_fill(*this, value);
}

template <typename T>
vector<T>::~vector() {
}

template <typename T>
size_t vector<T>::size() const {
  return (size_t) this->dim(0);
}

template <typename T>
size_t vector<T>::length() const {
  return (size_t) this->dim(0);
}

template <typename T>
bool vector<T>::empty() const {
  return (this->dim(0) == 0) ? true : false;
}

template <typename T>
void vector<T>::clear() {
  this->resize((ebl::intg)0);
}

// template <typename T>
// void vector<T>::push_back(T &e) {
//   ebl::intg i = this->dim(0);
//   this->resize(i + 1);
//   this->set(e, i);
// }

template <typename T>
void vector<T>::push_back(T e) {
  ebl::intg i = this->dim(0);
  this->resize(i + 1);
  this->set(e, i);
}

template <typename T>
T& vector<T>::operator[](size_t i) {
  ebl::intg j = (ebl::intg) i;
  if (j >= this->dim(0))
    eblerror(j << " is out of bounds in " << this->dim(0));
  return this->get(j);
}

template <typename T>
const T& vector<T>::operator[](size_t i) const {
  ebl::intg j = (ebl::intg) i;
  if (j >= this->dim(0))
    eblerror(j << " is out of bounds in " << this->dim(0));
  return this->get(j);
}

#endif

// min/max /////////////////////////////////////////////////////////////////////

#ifdef __NOSTL__

template <typename T>
T min(const T &e1, const T &e2) {
  if (e2 < e1)
    return e2;
  return e1;
}

template <typename T>
T max(const T &e1, const T &e2) {
  if (e2 > e1)
    return e2;
  return e1;
}

#endif

} // end namespace ebl

#endif /* STL_HPP_ */
