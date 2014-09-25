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

#ifndef IDXSPEC_H
#define IDXSPEC_H

#include "defines.h"

#ifndef __NOSTL__
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#endif

#include <stdio.h>
#include "stl.h"
#include "smart.h"

namespace ebl {

const int MAXDIMS=8;

// forward declarations ////////////////////////////////////////////////////////

template <typename T = intg> class idxd;
typedef idxd<intg> idxdim;
typedef idxd<float> fidxdim;

// idxspec /////////////////////////////////////////////////////////////////////

//! idxspec contains all the characteristics of an idx,
//! except the storage. It includes the order (number of dimensions)
//! the offset, and the dim and mod arrays.
//! having an idxspec class separate from idx allows us to write
//! generic manipulation functions that do not depend on the
//! type of the storage.
class EXPORT idxspec {
public:
  //! the destructor of idxspec deallocates the dim and mod arrays.
  virtual ~idxspec();
  //! assignment operator overloading.
  const idxspec& operator=(const idxspec& src);
  //! copy idxspec src into current idxspec
  void copy(const idxspec& src);
  //! copy constructor from idxspec src
  idxspec(const idxspec& src);

  // constructors //////////////////////////////////////////////////////////////

  //! This creates an idxspec0 with offset 0.
  //! This can be used to build an empty/blank idxspec.
  idxspec();
  //! Creates an idxspec0 with offset o.
  idxspec(intg o);
  //! Creates an idxspec1 with offset o.
  idxspec(intg o, intg size0);
  //! Creates an idxspec2 with offset o.
  idxspec(intg o, intg size0, intg size1);
  //! Creates an idxspec3 with offset o.
  idxspec(intg o, intg size0, intg size1, intg size2);
  //! Generic constructor with offset.
  idxspec(intg o, intg s0, intg s1, intg s2, intg s3,
          intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);
  //! Creates an idxspec of order n from arrays of dims and mods.
  //! The arrays are copied.
  idxspec(intg o, int n, intg *ldim, intg *lmod);
  //! construct an idxspec from an array of dimensions contained in an idxdim,
  //! with offset o
  idxspec(intg o, const idxdim &d);

  // access methods //////////////////////////////////////////////////////////

  //! return the offset of idxspec
  intg getoffset() const;
  //! Adds 'n' to current offset.
  void add_offset(intg n);
  //! return the order (number of dimensions).
  int getndim() const;
  //! return the memory footprint, including the offset.
  //! The storage of an idx containing this idxspec must
  //! be have at least this size.
  intg footprint() const;
  //! total number of elements accessed by idxspec
  intg nelements() const;
  //! returns true if the idxspec elements are
  //! in a continuous chunk of memory. This is useful
  //! to optimize iterators over the data.
  bool contiguousp() const;
  //! Returns the "true" order of this tensor, e.g. 10, 10x1, 10x1x1
  //! are in effect 1 dimensional tensors even though their order differ.
  intg true_order() const;

  // print methods /////////////////////////////////////////////////////////////

  //! pretty-prints the idxspec on the specified file.
  void pretty(FILE *f) const;
  void pretty(std::ostream& out) const;

  // manipulation methods //////////////////////////////////////////////////////

  //! Returns a new idxspec corresponding to
  //! a slice of the current idxspec with slice i
  //! in dimension d. In other words, if m is an
  //! idxspec of order 2 of size (10,4), the call
  //! idxspec p = m.select(0,3) will set p to
  //! an idxspec or order 1 containing the 4-th
  //! row of m.
  idxspec select(int d, intg i);
  //! select_into: same as select, but modifies an existing
  //! idxspec instead of returning a new one.
  intg select_into(idxspec *dst, int d, intg n) const;
  //! select_inplace: same as select, but modifies the
  //! current idxspec.
  intg select_inplace(int d, intg i);

  //! narrow: return a new idxspec in which the d-th
  //! dimension has been reduced to size s, starting
  //! at item o. In other words, if m is an
  //! idxspec of order 2 of size (10,4), the call
  //! idxspec p = m.narrow(0,6,2) will set p to
  //! an idxspec or order 2 of size (6,4) whose rows
  //! are rows 2 to 7 of m.
  idxspec narrow(int d, intg s, intg o);
  //! narrow_into: same as narrow, but modifies an existing
  //! idxspec instead of returning a new one.
  intg narrow_into(idxspec *dst, int d, intg s, intg o);
  //! narrow_inplace: same as narrow, but modifies the
  //! current idxspec.
  intg narrow_inplace(int d, intg s, intg o);

  //! transpose: transpose dimensions d1 and d2.
  idxspec transpose(int d1, int d2);
  //! transpose all dimensions through permutation matrix p.
  idxspec transpose(int *p);
  //! same as transpose, but modifies an existing
  //! idxspec instead of returning a new one.
  int transpose_into(idxspec *dst, int d1, int d2);
  //! same as transpose, but modifies an existing
  //! idxspec instead of returning a new one.
  int transpose_into(idxspec *dst, int *p);
  //! transpose_inplace: same as transpose, but modifies the
  //! current idxspec.
  int transpose_inplace(int d1, int d2);
  //! transpose_inplace: same as transpose, but modifies the
  //! current idxspec.
  int transpose_inplace(int *p);

  //! unfold: prepare an idxspec for a convolution.
  //! Returns an idx on the same storage as m (pointing to the
  //! same data) with an added dimension at the end obtained by
  //! "unfolding" the n -th dimension. The size of the new dimension
  //! is k. This essentially manipulates the mod array to make
  //! convolutions look like matrix-vector multiplies.
  idxspec unfold(int d, intg k, intg s);
  //! same as unfold, but modifies an existing
  //! idxspec instead of returning a new one.
  intg unfold_into(idxspec *dst, int d, intg k, intg s);
  //! unfold_into: same as unfold, but modifies the
  //! current idxspec.
  intg unfold_inplace(int d, intg k, intg s);

  // friends ///////////////////////////////////////////////////////////////////

  friend class idxd<>;
  template <class T> friend class idxiter;
  template <class T> friend class idxlooper;
  template <class T> friend class contiguous_idxiter;
  template <class T> friend class noncontiguous_idxiter;
  template <class T> friend class idx;

  friend bool same_dim(idxspec &s1, idxspec &s2);
  //! idxspec print operator.
  friend EXPORT std::ostream& operator<<(std::ostream& out, const idxspec& d);
  //! idxspec print operator.
  friend EXPORT std::ostream& operator<<(std::ostream& out, idxspec& d);
  //! idxspec print operator.
  friend EXPORT std::string& operator<<(std::string& out, const idxspec& d);
  //! idxspec print operator.
  friend EXPORT std::string& operator<<(std::string& out, idxspec& d);

private:
  //! private method to set the order (number of dims) to n.
  //! Calling this with n = 0 deallocates the dim and mod arrays.
  //! The dim and mod arrays are reallocated as necessary when
  //! the order is increased.
  int setndim(int n);

  //! private method to set the order to n, and the dim and
  //! mod arrays to pre-allocated arrays ldim and lmod.
  int setndim(int n, intg *ldim, intg *lmod);

  //! number of dimensions
  int ndim;
  //! offset in the storage
  intg offset;
  //! array of sizes in each dimension
  intg *dim;
  //! array of strides in each dimension
  intg *mod;

  //! resize the spec and return the new footprint.
  //! this is private because only idx can call this
  //! so that the storage gets properly resized.
  //! We do not allow the order to be changed with this,
  //! only the size of each dimension can be modified.
  //! The resized spec will be contiguous but will have
  //! the same offset as the original.
  intg resize(intg s0=-1, intg s1=-1, intg s2=-1, intg s3=-1,
              intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);
  //! resize the spec and return the new footprint,
  //! using dimensions contained in an idxdim.
  //! The order is not allowed to change.
  intg resize(const idxdim &d);
  //! resize dimension <dimn> with size <size> and return new footprint.
  //! only already allocated dimensions can be resized
  //! (order is not allowed to change).
  intg resize1(intg dimn, intg size);
  /* template<typename SizeIter> */
  /*   intg resize(SizeIter& dimsBegin, SizeIter& dimsEnd); */
  //! set the offset and return the new value
  intg setoffset(intg o) { return offset = o; }
  //! initialize a spec with offset o and 8 dimensions
  void init_spec(intg o, intg s0, intg s1, intg s2, intg s3,
                 intg s4, intg s5, intg s6, intg s7);
  //! Initialize a spec with offset o and n dimensions.
  //! This is more efficient that other init_spec().
  void init_spec(intg o, intg s0, intg s1, intg s2, intg s3,
                 intg s4, intg s5, intg s6, intg s7, uint n);
};

//! return true if two idxspec have the same dimensions,
//! i.e. if all their dimensions are equal (regardless
//! of strides).
bool same_dim(idxspec &s1, idxspec &s2);

// idxd ////////////////////////////////////////////////////////////////////////

//! This class allows to extract dimensions information from existing idx
//! objects in order to create other idx objects with the same order without
//! knowning their order in advance. One can modify the order, each dimensions
//! and their offsets. Offsets are 0 by default, but can be used to define
//! bounding boxes for a n-dimensional tensor.
template <typename T> class idxd : public smart_pointer {
 public:
  // constructors //////////////////////////////////////////////////////////////

  //! Empty constructor, creates an empty idxd.
  idxd();
  //! Create an idxd based on the information found in an idxspec.
  idxd(const idxspec &s);
  //! Create an idxd based on the information found in an idx<T2>.
  template <class T2> idxd(const idx<T2> &i);
  //! Create an idxd based on the information found in an idxd<T2>.
  template <class T2> idxd(const idxd<T2> &i);
  //! Create an idxd based on the information found in an idxd.
  idxd(const idxd<T> &s);
  //! Generic constructor.
  idxd(T s0, T s1=-1, T s2=-1, T s3=-1, T s4=-1, T s5=-1, T s6=-1, T s7=-1);
  //! Destructor.
  virtual ~idxd();

  // set dimensions ////////////////////////////////////////////////////////////

  //! Change the dimensions dimn to size size. One cannot change the
  //! order of an idxd, only existing dimensions can be changed.
  void setdim(intg dimn, T size);
  //! Set sames dimensions as an idx.
  template <class Tidx> void setdims(const idx<Tidx> &i);
  //! Set sames dimensions as an idxd.
  void setdims(const idxd<T> &s);
  //! Set sames dimensions as an idxspec.
  void setdims(const idxspec &s);
  //! Set all existing dimensions to n.
  void setdims(T n);
  //! Insert a dimension of size dim_size at position pos, shifting
  //! all dimensions after pos and incrementing order by 1.
  //! This is valid only if all dimensions up to pos (excluded) are > 0.
  void insert_dim(intg pos, T dim_size);
  //! Remove dimension at position pos and return it, shifting
  //! all dimensions after pos and decrementing order by 1.
  //! This is valid only if all dimensions up to pos (excluded) are > 0.
  T remove_dim(intg pos);
  //! Remove all dimensions of size 1 starting from the highest order.
  void remove_trailing_dims();
  //! Set the offset of dimensions 'dimn' to 'offset'.
  void setoffset(intg dimn, T offset);
  //! Returns true if this object contains offsets or not.
  bool has_offsets() const;
  //! Set each dimension of this idxd to the max of this one and 'other'.
  //! Note: 'other' and this idx are expected to have the same order.
  void set_max(const idxd<T> &other);
  //! Shift dimension 'd' to position 'pos'.
  void shift_dim(int d, int pos);

  // get dimensions ////////////////////////////////////////////////////////////

  //! Returns true if this idxd has not been set at all.
  bool empty() const;
  //! Returns the order.
  intg order() const;
  //! Returns the size of dimension 'dimn'.
  T dim(intg dimn) const;
  //! Returns the size of biggest dimension.
  T maxdim() const;
  //! Returns the offset of dimension 'dimn'.
  T offset(intg dimn) const;
  //! Return true if dimensions and order are equal (regardless of offsets).
  bool operator==(const idxd<T>& other);
  //! Return true if dimensions and/or order are different
  //! (regardless of offsets).
  bool operator!=(const idxd<T>& other);
  //! Return total number of elements.
  intg nelements();

  // operators /////////////////////////////////////////////////////////////////

  /* //! Assign idxd 'd2' into current idxd (copies offsets if they exist). */
  /* idxd<T>& operator=(const idxd<T> &d2); */
  //! Assign idxd 'd2' into current idxd (copies offsets if they exist).
  idxd<T>& operator=(idxd<T> d2);
  //! Assign idxd 'd2' with different type into current idxd
  //! (copies offsets if they exist).
  template <typename T2> idxd<T>& operator=(const idxd<T2> &d2);
  //! Return an idxd who's each dimension and offset is multiplied
  //! with each dimension of idxd 'd2'.
  idxd<T> operator*(const idxd<T> &d2) const;
  //! Return an idxd who's each dimension and offset is multiplied
  //! with each dimension of idxd 'd2'.
  template <typename T2> idxd<T> operator*(const idxd<T2> &d2) const;
  //! Return an idxd who's each dimension and offset is multiplied
  //! with each dimension of idxd 'd2'.
  template <typename T2> idxd<T> operator*(idxd<T2> &d2);
  //! Return an idxd who's dimensions and offsets are multiplied by d.
  template <typename T2> idxd<T> operator*(T2 d);
  //! Return an idxd who's dimensions are added d.
  template <typename T2> idxd<T> operator+(T2 d);
  //! Return the addition of this idxd with another one 'd2' with same order.
  idxd<T> operator+(idxd<T> &d2);
  //! Return true if all of this idxd's dimensions are <= to corresponding
  //! dimensions in d2.
  bool operator<=(idxd<T> &d2);
  //! Return true if all of this idxd's dimensions are >= to corresponding
  //! dimensions in d2.
  bool operator>=(idxd<T> &d2);

  // friends ///////////////////////////////////////////////////////////////////

  template <typename T2>
  friend std::string& operator<<(std::string& out, const idxd<T2>& d);
  friend class idxspec;

  // member variables //////////////////////////////////////////////////////////
 protected:
  T	dims[MAXDIMS];                  //!< Size of each dimension.
  intg 	ndim;                           //!< Order, i.e. number of dimensions.
  T    *offsets;                        //!< Offsets (optional).
};

// stream operators ////////////////////////////////////////////////////////////

//! idxd print operator.
template <typename T>
EXPORT std::ostream& operator<<(std::ostream& out, const idxd<T>& d);
//! idxd string concatenation operator.
template <typename T>
EXPORT std::string& operator<<(std::string& out, const idxd<T>& d);

// midxd ///////////////////////////////////////////////////////////////////////

//! This class is a container for multiple idxdim.
template <typename T> class midxd : public svector<idxd<T> > {
 public:
  // constructors ////////////////////////////////////////////////////////////

  //! Empty constructor, creates an empty vector of idxd.
  midxd();
  //! Creates a vector of 'n' empty idxd elements.
  midxd(uint n);
  //! Create an midxd with 's' as first element.
  midxd(const idxd<T> &s);
  //! Create an midxd containing the same elements as 's'.
  template <typename T2> midxd(const midxd<T2> &s);
  //! Destructor.
  virtual ~midxd();

  // accessors ///////////////////////////////////////////////////////////////

  //! Returns an midxd with 'n' elements starting at 'offset'.
  virtual midxd<T> narrow_copy(uint n, uint offset);
  // //! Converts and appends idxd 'm' of different type to this idxd.
  template <typename T2> void push_back(const idxd<T2> &m);
  //! Copies and appends all idxd contained in 'm' to this idxd.
  virtual void push_back(const midxd<T> &m);
  /* //! Appends all idxd contained in 'm' to this idxd. */
  /* virtual void push_back(midxd<T> &m); */
  //! Returns an midxd with 1 idxd only, assuming they are all equal.
  //! An error will be issued if any elements is different.
  virtual midxd<T> merge_all();
  //! Assign another midxd of possibly different type to this midxd.
  template <typename T2> midxd<T>& operator=(const midxd<T2> &other);

  // member variables ////////////////////////////////////////////////////////
 protected:
};

// shortcut types
typedef midxd<intg> midxdim;
typedef midxd<float> mfidxdim;

// printing //////////////////////////////////////////////////////////////////

//! Prints a vector into a stream.
template<class T>
EXPORT std::string& operator<<(std::string &o, const midxd<T> &v);
//! Prints a vector into a stream.
template<class T>
EXPORT std::ostream& operator<<(std::ostream &o, const midxd<T> &v);

} // end namespace ebl

#include "idxspec.hpp"

#endif /* IDXSPEC_H_ */
