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

#ifndef IDX_H
#define IDX_H

#include <stdio.h>
#include "defines.h"
#include "srg.h"
#include "stl.h"
#include "idxspec.h"
#include "idxiter.h"
#include "smart.h"

namespace ebl {

// idx /////////////////////////////////////////////////////////////////////////

//! The tensor class. This can represent vectors, matrices, and tensors
//! up to 8 dimensions. An idx is merely an access structure that
//! points to the data. Several idx can point to the same data.
template <class T> class idx : public smart_pointer {
 public:
  // basic constructors/destructor /////////////////////////////////////////////

  //! destructor: unlocks the srg.
  virtual ~idx();
  //! generic constructor with dims and mods creates
  //! the storage and set offset to zero.
  idx(int n, intg *dims, intg *mods);
  //! Construct an idx from another one.
  idx(const idx<T>& other);

  // constructors initialized with an array //////////////////////////////////

  //! Allocates an idx2 of size (s0, s1) and fills it with mat, expected
  //! to be of size (s0, s1).
  idx(const T *mat, intg s0, intg s1);
  //! Allocates an idx3 of size (s0, s1, s2) and fills it with mat, expected
  //! to be of size (s0, s1, s2).
  idx(const T *mat, intg s0, intg s1, intg s2);

  // specific constructors for each number of dimensions /////////////////////

  //! creates an idx0 from scratch;
  idx();
  //! creates an idx1 of size size0.
  idx(intg size0);
  //! creates an idx2 of size (size0,size1).
  idx(intg size0, intg size1);
  //! creates an idx3 of size (size0,size1,size2).
  idx(intg size0, intg size1, intg size2);
  //! creates an idx of any order.
  idx(intg s0, intg s1, intg s2, intg s3, intg s4=-1, intg s5=-1, intg s6=-1,
      intg s7=-1);
  //! creates an idx from an array of dimensions contained in an idxdim
  idx(const idxdim &d);

  // constructors from existing srg and offset ///////////////////////////////

  //! generic constructor with idxspec.
  idx(srg<T> *srg, idxspec &s);
  //! constructor with existing storage and array pointers for dim and mod
  idx(srg<T> *srg, intg o, intg n, intg *dims, intg *mods);
  //! creates an idx0 with existing srg and offset.
  idx(srg<T> *srg, intg o);
  //! creates an idx1 of size size0, with existing srg and offset.
  idx(srg<T> *srg, intg o, intg size0);
  //! creates an idx2 of size (size0,size1),
  //! with existing srg and offset.
  idx(srg<T> *srg, intg o, intg size0, intg size1);
  //! creates an idx3 of size (size0,size1,size2),
  //! with existing srg and offset.
  idx(srg<T> *srg, intg o, intg size0, intg size1, intg size2);
  //! creates an idx of any order with existing srg and offset.
  idx(srg<T> *srg, intg o, intg s0, intg s1, intg s2, intg s3, intg s4=-1,
      intg s5=-1, intg s6=-1, intg s7=-1);
  //! Creates an idx with dimensions 'd' using storage 'srg' and offset 'o'.
  idx(srg<T> *srg, intg o, const idxdim &d);

  // operators ///////////////////////////////////////////////////////////////

  // TODO: Find out why code such as idx<float> = 1 compiles
  // (even without default operator below).
  // default operator below outputs an error that this is forbidden.
  idx<T>& operator=(T other);
  //! Copy an idx into another.
  idx<T>& operator=(const idx<T>& other);
  //! Return an idx that is the 'i'th slice in dimensions 0.
  //! Equivalent to select(0, i).
  virtual idx<T> operator[](const intg i);

  // resize methods //////////////////////////////////////////////////////////

  //! Adds 'n' to current offset to storage.
  virtual void add_offset(intg n);
  //! change the offset of an idx. The Storage is
  //! resized accordingly. Returns the new offset.
  virtual intg setoffset(intg o);
  //! resize an idx. The order (ndim) is not allowed to change.
  //! This is to prevent nasty bugs.
  //! As in realloc(), the content will be unchanged to the minimum of the old
  //! and new sizes.
  void resize(intg s0=-1, intg s1=-1, intg s2=-1, intg s3=-1,
              intg s4=-1, intg s5=-1, intg s6=-1, intg s7=-1);
  //! resize an idx with dimensions contained in an idxdim.
  //! The order is not allowed to change.
  //! As in realloc(), the content will be unchanged to the minimum of the old
  //! and new sizes.
  void resize(const idxdim &d);
  //! resize 1 dimension of an idx. The order is not allowed to change.
  //! As in realloc(), the content will be unchanged to the minimum of the old
  //! and new sizes.
  void resize1(intg dimn, intg size);
  //! same as resize, but the storage is enlarged by a step of s_chunk
  //! if needed.
  //! As in realloc(), the content will be unchanged to the minimum of the old
  //! and new sizes.
  virtual void resize_chunk(intg s_chunk, intg s0=-1, intg s1=-1, intg s2=-1,
                            intg s3=-1, intg s4=-1, intg s5=-1, intg s6=-1,
                            intg s7=-1);

  // idx manipulation methods //////////////////////////////////////////////////

  //! Returns a new idx corresponding to a slice of the current idx with
  //! slice i in dimension d. In other words, if m is an idx of order 2
  //! of size (10,4), the call idx p = m.select(0,3) will set p to
  //! an idx or order 1 containing the 4-th row of m.
  idx<T> select(int d, intg i);
  //! narrow: return a new idx in which the d-th
  //! dimension has been reduced to size s, starting
  //! at item o. In other words, if m is an
  //! idx of order 2 of size (10,4), the call
  //! idx p = m.narrow(0,6,2) will set p to
  //! an idx or order 2 of size (6,4) whose rows
  //! are rows 2 to 7 of m.
  idx<T> narrow(int d, intg s, intg o);
  //! Return an new idx in which dimensions
  //! d1 and d2 are transposed. No data is actually
  //! moved around, this merely manipulates the idx
  //! structure itself.
  idx<T> transpose(int d1, int d2);
  //! Return an new idx in which the dimensions
  //! are permuted using the permutation vector p.
  //! For example, if m is an idx of size (2,4,6),
  //! int p[] = {1,2,0}; m.transpose(p);
  //! returns an idx of size (4,6,2). No data is actually
  //! moved around, this merely manipulates the idx
  //! structure itself.
  idx<T> transpose(int *p);
  //! Return a new idx prepared for a convolution.
  //! Returns an idx on the same storage as m (pointing to the
  //! same data) with an added dimension at the end obtained by
  //! "unfolding" the n -th dimension. The size of the new dimension
  //! is k. This essentially manipulates the mod array to make
  //! convolutions look like matrix-vector multiplies.
  virtual idx<T> unfold(int d, intg k, intg s);
  //! Returns a new idx with an order n (the data must be contiguous,
  //! this will raise an exception otherwise).
  //! if n == 1, the data is viewed as a 1D idx, regardless of its
  //!   current order.
  //! if n > 1 and n > current order, then extra dimensions of size 1
  //!   are added.
  //! if n > 1 and n < current order, this is undefined, an error is raised.
  //! if n == current order, an identical idx is returned.
  // TODO: forbid this function as it conflicts with the forbidden
  // order-change rule? unfold also conflicts with this rule?
  idx<T> view_as_order(int n);
  //! Returns a 1D view of current idx.
  idx<T> flat();
  //! Return an new idx in which dimension d is shifted to position pos,
  //! keeping the ordering of other dimensions.
  //! For example, if m is an idx of size (2,4,6),
  //! m.shift_dim(2, 0); returns an idx of size (6,2,4). No data is actually
  //! moved around, this merely manipulates the idx structure itself.
  idx<T> shift_dim(int d, int pos);

  // field access methods //////////////////////////////////////////////////////

  //! return pointer to storage
  virtual srg<T> *getstorage();
  //! return size of idx in d-th dimension.
  virtual intg dim(int d) const;
  //! return const ptr to dims
  virtual const intg* dims();
  //! return stride of idx in d-th dimension.
  virtual intg mod(int d) const;
  //! return const ptr to mods
  virtual const intg* mods();
  //! return order of idx (number of dimensions).
  virtual int order() const;
  //! Returns the "true" order of this tensor, e.g. 10, 10x1, 10x1x1
  //! are in effect 1 dimensional tensors even though their order differ.
  virtual intg true_order() const;
  //! return offset of idx.
  virtual intg offset();
  //! return total number of elements
  virtual intg nelements() const;
  //! return total footprint in the storage
  //! (index after last cell occupied in the storage)
  virtual intg footprint() const;
  //! return true if elements of idx are
  //! contiguous in memory.
  virtual bool contiguousp() const;

  //! return true if this idx has same order and dimensions as idxdim d.
  //! i.e. if all their dimensions are equal (regardless of strides).
  bool same_dim(const idxdim &d);
  //! return true if this idx has same order and dimensions s0 .. s7
  //! i.e. if all their dimensions are equal (regardless of strides).
  bool same_dim(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
                intg s6, intg s7);
  //! returns a reference to an idxdim object (owned by this idx)
  //! containing the order and dimensions of this idx.
  virtual idxdim& get_idxdim();
  //! The 'const' version of get_idxdim(), i.e. does not modify internal
  //! members. This is less efficient in that it returns the full object
  //! and does not remember it internally. This is useful when dealing with
  //! a const idx, i.e. where members cannot be modified.
  virtual idxdim get_idxdim() const;

  // data access methods /////////////////////////////////////////////////////

  //! return pointer on data chunk (on first element)
  virtual T *idx_ptr();
  //! return pointer on data chunk (on first element), const version
  virtual const T *idx_ptr() const;
  //! Returns a pointer to the modulos array.
  virtual intg *mod_ptr();

  //! return a pointer to an element (idx0 version)
  virtual T *ptr();
  //! return a pointer to an element (idx1 version)
  virtual T *ptr(intg i0);
  //! return a pointer to an element (idx2 version)
  virtual T *ptr(intg i0, intg i1);
  //! return a pointer to an element (idx3 version)
  virtual T *ptr(intg i0, intg i1, intg i2);
  //! return a pointer to an element (generic version)
  virtual T *ptr(intg i0, intg i1, intg i2, intg i3, intg i4=-1, intg i5=-1,
                 intg i6=-1, intg i7=-1);

  //! return the value of an element (idx0 version)
  virtual T get() const;
  //! return the value of an element (idx1 version)
  virtual T& get(intg i0) const;
  //! return the value of an element (idx2 version)
  virtual T get(intg i0, intg i1) const;
  //! return the value of an element (idx3 version)
  virtual T get(intg i0, intg i1, intg i2) const;
  //! return the value of an element (generic version)
  virtual T get(intg i0, intg i1, intg i2, intg i3, intg i4=-1,
                intg i5=-1, intg i6=-1, intg i7=-1);
  //! Returns the value of an element (generic version). The difference with
  //! other 'get' methods is that the user can specify a different number of
  //! arguments than the order of the idx. All remaining dimensions will
  //! be assumed to have offset 0 or none if the dimension does not exist.
  //! This is useful when one does not know in advance the order of the idx
  //! but only needs to access some elements while remaining offsets are 0.
  virtual T gget(intg i0=0, intg i1=0, intg i2=0, intg i3=0, intg i4=0,
                 intg i5=0, intg i6=0, intg i7=0);

  //! sets the value of an element (idx0 version)
  virtual T set(T val);
  //! sets the value of an element (idx1 version)
  virtual T set(T val, intg i0);
  //! sets the value of an element (idx2 version)
  virtual T set(T val, intg i0, intg i1);
  //! sets the value of an element (idx3 version)
  virtual T set(T val, intg i0, intg i1, intg i2);
  //! sets the value of an element (generic version)
  virtual T set(T val, intg i0, intg i1, intg i2, intg i3, intg i4=-1,
                intg i5=-1, intg i6=-1, intg i7=-1);
  //! Sets the value of an element (generic version). The difference with
  //! other 'set' methods is that the user can specify a different number of
  //! arguments than the order of the idx. All remaining dimensions will
  //! be assumed to have offset 0 or none if the dimension does not exist.
  //! This is useful when one does not know in advance the order of the idx
  //! but only needs to access some elements while remaining offsets are 0.
  virtual T sset(T val, intg i0=0, intg i1=0, intg i2=0, intg i3=0, intg i4=0,
                 intg i5=0, intg i6=0, intg i7=0);

  // print methods /////////////////////////////////////////////////////////////

  //! Pretty-prints Idx metadata to std::cout.
  void pretty() const;
  //! Pretty-prints Idx metadata to a stream.
  void pretty(std::ostream& out) const;
  //! Pretty-prints Idx metadata to a file pointer.
  void pretty(FILE *) const;

  //! Pretty-prints elements to a stream.
  virtual void print() const;
  //! Returns a string with all elements.
  virtual std::string str() const;
  //! Returns a string with info about idx, such as min/max/mean.
  virtual std::string info();

  virtual void printElems() const;
  virtual void printElems(std::ostream& out, bool newline = true) const;
  virtual void printElems(std::string& out, bool newline = true) const;
  // void printElems( FILE* out );  doesn't work (cf implementation)

  //! print content of idx on stream
  virtual int fdump(std::ostream &f);

  // friends /////////////////////////////////////////////////////////////////
  template <class T2> friend class idxiter;
  template <class T2> friend class idxlooper;
  template <class T2> friend class contiguous_idxiter;
  template <class T2> friend class noncontiguous_idxiter;

  // protected methods ///////////////////////////////////////////////////////
 protected:
  //! increase size of storage to fit the current dimension
  void growstorage();
  //! increase size of storage to fit the current dimension + a given size
  void growstorage_chunk(intg s_chunk);
  //! Implementation of public printElems() method.
  template <class stream>
  void printElems_impl(int indent, stream&, bool newline = true) const;
  //! fake constructor that does nothing.
  //! This is called by the idxlooper constructor.
  idx(dummyt *dummy);

  // members variables ///////////////////////////////////////////////////////
 public:
  idxspec spec; //!< Contains the order, offset, dimensions and strides.
 protected:
  srg<T> *storage; //!< Pointer to the srg structure that contains the data.
  //! A pointer to a dimensions descriptor.
  //! this is allocated when get_idxdim() method is called and destroyed by
  //! destructor.
  idxdim *pidxdim;
  idxd<intg> idd;
};

// midx //////////////////////////////////////////////////////////////////////

//! An idx vector containing pointers to other idx. Pointed idx can either
//! be loaded all at once using 'load_matrices()' or on-demand using
//! 'load_matrices_ondemand()'. Loading on-demand can be useful if the
//! matrices are too big to all fit in memory at once.
template <typename T>
class midx : public idx<idx<T>*> {
 public:
  //! This creates a vector of matrices of size 'size'. Initially all elements
  //! are set to empty.
  //! \param fp If not null, use this file pointer to dynamically
  //!   load each matrix when requested.
  midx(intg size, std::file *fp = NULL, idx<int64> *offsets = NULL);
  //! This creates a matrix of matrices of size 'size0'x'size1'.
  //! Initially all elements are set to empty.
  //! \param fp If not null, use this file pointer to dynamically
  //!   load each matrix when requested.
  midx(intg size0, intg size1,
       std::file *fp = NULL, idx<int64> *offsets = NULL);
  //! This creates a matrix of matrices with dimensions 'd'.
  //! Initially all elements are set to empty.
  //! \param fp If not null, use this file pointer to dynamically
  //!   load each matrix when requested.
  midx(idxdim &d, std::file *fp = NULL, idx<int64> *offsets = NULL);
  //! Empty constructor.
  midx();
  //! Construct an midx given an idx of pointers to idx.
  //    midx(const idx<idx<T>*> &o);
  midx(const midx<T> &o);
  //! The destructor deletes all allocated idx pointers.
  virtual ~midx();

  //! Copy an midx into another.
  virtual midx<T>& operator=(const midx<T>& other);

  // resizing ////////////////////////////////////////////////////////////////

  //! Deletes all sub-tensors, resizes current vector to size 'i0' and
  //! sets all pointers to NULL.
  void resize(intg i0);
  //! Deletes all sub-tensors, resizes current vector to size ('i0', 'i1') and
  //! sets all pointers to NULL.
  void resize(intg i0, intg i1);
  //! Allocate/resizes all elements of this to be the same as 'other'.
  void resize(midx<T> &other);
  //! Removes highest order dimensions that are equal to 1.
  virtual void remove_trailing_dims();

  // deletions ///////////////////////////////////////////////////////////////

  //! Deletes idx at position 'i0' (if present).
  void remove(intg i0);
  //! Deletes idx at position ('i0', 'i1') (if present).
  void remove(intg i0, intg i1);
  //! Deletes all idx present and set all to NULL.
  void clear();

  // accessors /////////////////////////////////////////////////////////////////

  //! Set matrix 'e' at position 'i0'.
  void mset(idx<T> &e, intg i0);
  //! Set matrix 'e' at position ('i0', 'i1').
  void mset(idx<T> &e, intg i0, intg i1);
  //! Get matrix if this midx has order 0 (a single matrix).
  //! This throws an error if matrix does not exist.
  //! Use exists() to verify it does.
  idx<T> mget();
  //! Get matrix at position 'i0'. This throws an error if matrix does not
  //! exist. Use exists() to verify it does.
  idx<T> mget(intg i0);
  //! Get matrix at position ('i0', 'i1'). This throws an error if matrix
  //! does not exist. Use exists() to verify it does.
  idx<T> mget(intg i0, intg i1);

  //! Return a narrowed version of current midx.
  midx<T> narrow(int d, intg s, intg o);
  //! Return a slice 's' of this midx in dimension d.
  midx<T> select(int d, intg s);
  //! Returns true if a matrix is present at position 'pos', false otherwise.
  bool exists(intg pos) const;
  //! Returns true if a matrix is present at position ('i0', 'i1'),
  //! false otherwise.
  bool exists(intg i0, intg i1) const;
  //! Returns the dimensions of the matrix with maximum number of elements
  //! among all matrices contained in this midx.
  idxdim get_maxdim();
  //! Returns true if this midx has same order and dimensions for each
  //! submatrix of 'other'.
  bool same_dim(const midx<T> &other) const;
  //! Returns the offsets of the on-demand loading matrix.
  idx<int64> get_offsets();
  //! Returns the on-demande file pointer.
  std::file *get_file_pointer();
  //! Return total number of elements in all sub-matrices.
  virtual intg nelements_all() const;

  //! Pack all data into a single matrix. This function can only be called
  //! if all sub-matrices have the same size. An error will be thrown
  //! otherwise.
  //! This operation is expensive as it involves copying all data into
  //! a single idx.
  virtual idx<T> pack();

  //! For each sub-matrix, shift dimension 'd' to position 'pos'.
  virtual void shift_dim_internal(int d, int pos);

  //! Pretty all sub-matrices to std::cout.
  virtual void pretty() const;
  //! Pretty all sub-matrices.
  template <class stream> void pretty(stream &out, bool newline = true) const;
  //! Pretty all sub-matrices into a string and return it.
  virtual std::string str() const;

  // internal methods ////////////////////////////////////////////////////////
 protected:
  //! Lock all idx contained in this midx, so that they are not deleted if
  //! other midx referencing them unlock them.
  void lock_all();
  //! Unlock all idx contained in this midx.
  void unlock_all();

  // members /////////////////////////////////////////////////////////////////
 protected:
  std::file *fp; //!< Load matrices ondemand if not null.
  idx<int64> offsets; //!< File offsets for each element.
};

// idx stream printing /////////////////////////////////////////////////////////

//! idx print operator.
template <typename T> std::ostream& operator<<(std::ostream &out,
                                               const idx<T> &m);
//! idx print operator.
template <typename T> std::string& operator<<(std::string &out, idx<T> &m);
//! idx print operator.
template <typename T> std::string& operator<<(std::string &out,const idx<T> &m);
//! idx print operator.
template <typename T> std::string& operator<<(std::string &out, idx<T> *m);
//! svector of idx print operator.
template <typename T> std::string to_string(const svector<idx<T> > &m);

// midx stream printing ////////////////////////////////////////////////////////

//! midx print operator.
template <typename T, class stream> stream& operator<<(stream &out, midx<T> &m);
//! midx to string.
template <typename T> std::string to_string(const midx<T> &m);
//! midx print operator.
template <typename T> std::string& operator<<(std::string &out, midx<T> &m);
//! midx print operator.
template <typename T> std::string& operator<<(std::string &out,
                                              const midx<T> &m);

// debugging /////////////////////////////////////////////////////////////////

#ifdef __DEBUGMEM__

#define INIT_DEBUGMEM()							\
  /* memsize */								\
  template <typename T> ebl::intg ebl::srg<T>::memsize = 0;		\
  template <> EXPORT ebl::intg ebl::srg<double>::memsize = 0;		\
  template <> EXPORT ebl::intg ebl::srg<float>::memsize = 0;		\
  template <> EXPORT ebl::intg ebl::srg<int>::memsize = 0;		\
  template <> EXPORT ebl::intg ebl::srg<long>::memsize = 0;		\
  template <> EXPORT ebl::intg ebl::srg<unsigned long>::memsize = 0;	\
  template <> EXPORT ebl::intg ebl::srg<short>::memsize = 0;		\
  template <> EXPORT ebl::intg ebl::srg<unsigned char>::memsize = 0;	\
  template <> EXPORT ebl::intg ebl::srg<signed char>::memsize = 0;	\
  template <> EXPORT ebl::intg ebl::srg<unsigned short>::memsize = 0;	\
  template <> EXPORT ebl::intg ebl::srg<ebl::idx<float>*>::memsize = 0; \
  template <> EXPORT ebl::intg ebl::srg<ebl::idx<unsigned char>*>::memsize = 0; \
  template <> EXPORT ebl::intg ebl::srg<const char*>::memsize = 0;	\
  template <> EXPORT ebl::intg ebl::srg<void *>::memsize = 0;		\
  template <> EXPORT ebl::intg ebl::srg<bool>::memsize = 0;		\
  template <> EXPORT ebl::intg ebl::srg<uint>::memsize = 0;		\
  /* locks */								\
  EXPORT ebl::intg ebl::smart_pointer::locks = 0;			\
  EXPORT ebl::intg ebl::smart_pointer::spointers = 0;

//! In debugmem mode, print the total usage of memory.
EXPORT void pretty_memory(const char *prefix = "");

#define DEBUGMEM_PRETTY(prefix) {		\
    std::string pf; pf << prefix;		\
    pretty_memory(pf.c_str());			\
  }

#else
#define INIT_DEBUGMEM()
#define DEBUGMEM_PRETTY(prefix)
#endif


} // end namespace ebl

#include "idx.hpp"

#endif /* IDX_H_ */
