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

#ifndef SMART_H
#define SMART_H

#include <stdio.h>
#include "defines.h"
#include "stl.h"

namespace ebl {

// smart_pointer /////////////////////////////////////////////////////////////

//! An object that knows how many other objects are refering to it and
//! destroys itself once nobody does anymore.
class EXPORT smart_pointer {
public:
  // basic constructors/destructor ///////////////////////////////////////////

  smart_pointer();
  smart_pointer(const smart_pointer &other);
  virtual ~smart_pointer();

  // reference counting //////////////////////////////////////////////////////

  //! Decrements reference counter and deallocates this if it reaches zero.
  virtual int unlock();
  //! Increments reference counter.
  virtual int lock();
  //! Returns the number of references to this object.
  virtual int get_count();

  //! Sets the number of references to this object(careful)
  virtual void set_count(int count);

  // friends /////////////////////////////////////////////////////////////////
  template <class T> friend class svector;

  // members variables ///////////////////////////////////////////////////////
protected:
  int refcount; //!< Reference counters of objects pointing to this idx.

  // debug only //////////////////////////////////////////////////////////////
#ifdef __DEBUGMEM__
public:
  static intg locks; //!< Total number of locks in program.
  static intg spointers; //!< Total number of smart pointers in program.
#endif
  //#ifdef __DEBUG__
  std::string debug_name;
  //#endif
};

// svector ///////////////////////////////////////////////////////////////////

//! A smart vector that handles smart pointers, i.e. this vector must be given
//! dynamically allocated objects and will handle their deletion
//! automatically.
template <class T>
class EXPORT svector : public std::vector<T*>, public smart_pointer {
public:
  //! Default constructor.
  svector();
  //! Construct from another svector.
  svector(const svector<T> &other);
  //! Constructs a vector of n empty elements.
  svector(uint n);
  virtual ~svector();

  //! Unlocks all elements and clears the pointers in this vector.
  virtual void clear();
  //! Unlocks element 'i' and set it to empty (NULL).
  virtual void clear(uint i);

  //! Returns a reference to T at index 'i'.
  virtual T& operator[](uint i);
  /* //! Returns a reference to T at index 'i'. */
  /* virtual const T& operator[](uint i) const; */
  //! Returns a reference to T at index 'i'.
  virtual const T& at_const(uint i) const;
  //! Returns the object pointer at index i.
  virtual T* at_ptr(uint i) const;

  //! Returns true if empty, false otherwise.
  virtual bool empty() const;
  //! Returns true if element 'i' is empty, false otherwise.
  virtual bool exists(uint i) const;
  //! Returns the number of non-empty elements.
  virtual uint size_existing() const;

  //! Unlocks element 'i' and erase the vector element (shifting all above)
  //! down by 1 element and decreasing size() by 1.
  virtual void remove(uint i);
  //! Remove all elements between 'start' and 'end' (included).
  virtual void remove(uint start, uint end);
  //! Erases all elements that are empty (!exists()), reducing the size of
  //! this vector by n if n elements are empty.
  virtual void remove_empty();

  //! Lock 'e' and and push its address to the front of this.
  virtual void push_front(T &e);
  //! Allocates a copy of 'e', lock it and push it to the front of this.
  virtual void push_front_new(T &e);

  //! Lock and push an element 'e' to the end of this vector.
  void push_back(T *e);
  //! Lock and push an element 'e' to the end of this vector.
  void push_back(T &e);
  //! Allocates a copy of 'e', lock it and push it to the end of this vector.
  virtual void push_back_new(const T &e);
  //! Allocates a copy of 'e', lock it and push it to the end of this vector.
  virtual void push_back_new(const T *e);
  //! Lock and push all elements of 'v' to the end of this vector.
  void push_back(std::vector<T*> &v);
  //! Lock and push all elements of 'v' to the end of this vector.
  void push_back(svector<T> &v);
  //! Allocate new, lock and push all elements of 'v' to the end of this.
  virtual void push_back_new(const svector<T> &v);
  //! Push an empty element (null) to the end of this vector.
  virtual void push_back_empty();

  //! Unlocks element at index 'n' and replace it with 'e' (and lock it).
  virtual void set(T& e, uint n);
  //! Unlocks element at index 'n' and replace it with a new 'e'(and lock it).
  virtual void set_new(T& e, uint n);

  /* //! Clear all elements, then lock and push all elements of 'v' to the end. */
  /* virtual svector<T>& operator=(svector<T> &v); */
  //! Clear all elements, then lock and push all elements of 'v' to the end.
  void operator=(svector<T> v);

  //! Swap elements 'i' and 'j'.
  virtual void swap(uint i, uint j);
  //! Permute elements according to permutation vector.
  virtual void permute(std::vector<uint> &permutations);

  //! Returns a svector that contains copies of each element of this.
  virtual svector<T> copy();
  //! Clear this and allocate copies of all elements of 'other' into this one.
  virtual void copy(const svector<T> &other);
  //! Clear this and allocate copies of all elements of 'other' into this one.
  virtual void copy(svector<T> &other);
  //! Returns an midxd with 'n' elements starting at 'offset'.
  virtual svector<T> narrow(uint n, uint offset);
  //! Pushes new elements of type T with default empty constructor until
  //! reaching size of n.
  virtual void resize_default(uint n);

  //! Randomizes order of elements.
  virtual void randomize();

  // iterators ///////////////////////////////////////////////////////////////

  //! Custom iterator for this class.
  class iterator;
  class const_iterator;
  //! Returns an iterator pointing to the beginning of this vector.
  //! \param offset Start at begin + offset.
  virtual iterator begin(uint offset = 0);
  //! Returns an iterator pointing to the beginning of this vector.
  //! \param offset Start at begin + offset.
  virtual const_iterator begin(uint offset = 0) const;

  // TEMPORARLY OUT
  //! Lock all idx contained in this midx, so that they are not deleted if
  //! other midx referencing them unlock them.
  void lock_all();

  // printing ////////////////////////////////////////////////////////////////

  //! Returns a string describing this vector.
  std::string str() const;

  // internal methods ////////////////////////////////////////////////////////
protected:
  //! Unlock all idx contained in this midx.
  void unlock_all();
  //! Unlock all elements between 'start' and 'end' (included).
  void unlock_range(uint start, uint end);

  // members /////////////////////////////////////////////////////////////////
protected:
  typename std::vector<T*>::iterator it;
};

// iterator //////////////////////////////////////////////////////////////////

//! Custom iterator for svector.
template <class T>
class svector<T>::iterator : public std::vector<T*>::iterator {
 public:
  //! Construct iterator.
  iterator();
  iterator(typename std::vector<T*>::iterator &i);
  iterator(typename std::vector<T*>::iterator i);
  virtual ~iterator();

  //! Dereference operator, returns a reference.
  inline virtual T& operator*();
  //! Dereference operator, returns a pointer.
  inline virtual T* operator->();
  //! Returns a pointer to current element.
  inline virtual T* ptr();
  //! Returns true if currently pointed element exists.
  inline virtual bool exists() const;
};

// const_iterator ////////////////////////////////////////////////////////////

//! Custom iterator for svector.
template <class T>
class svector<T>::const_iterator : public std::vector<T*>::const_iterator {
 public:
  //! Construct iterator.
  const_iterator();
  const_iterator(typename std::vector<T*>::const_iterator &i);
  const_iterator(typename std::vector<T*>::const_iterator i);
  virtual ~const_iterator();

  //! Dereference operator, returns a reference.
  inline virtual const T& operator*() const;
  //! Dereference operator, returns a pointer.
  inline virtual const T* operator->() const;
  //! Returns a pointer to current element.
  inline virtual const T* ptr() const;
  //! Returns true if currently pointed element exists.
  inline virtual bool exists() const;
};

// printing //////////////////////////////////////////////////////////////////

//! Prints a vector into a stream.
template <class T, class stream>
EXPORT stream& operator<<(stream &o, const svector<T> &v);
//! Prints a vector into a stream.
template <class T, class stream>
EXPORT stream& operator<<(stream &o, svector<T> &v);

} // end namespace ebl

#include "smart.hpp"

#endif /* SMART_H_ */
