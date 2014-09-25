/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
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

// tell header that we are in the libidx scope
#define LIBIDX

#include "idxspec.h"
#include "idx.h"

namespace ebl {

// idxspec constructors //////////////////////////////////////////////////////

// destructor: deletes dim/mod arrays
idxspec::~idxspec() {
  DEBUG_LOW("idxspec::~idxspec: " << (intg)this);
  setndim(0);
}

// copy constructor
idxspec::idxspec(const idxspec& src) : ndim(-1), dim(NULL), mod(NULL)
{ copy(src); }

// constructor for idx0 with offset 0.
// Can be used to build an empty/blank idx.
idxspec::idxspec() {
  ndim = -1;
  offset = 0;
  dim = NULL; mod = NULL;
}

// constructor for idx0 with offset
idxspec::idxspec(intg o) {
  ndim = 0;
  offset = o;
  dim = NULL; mod = NULL;
}

// constructor for idx1
idxspec::idxspec(intg o, intg size0) {
  if (size0 < 0)
    eblerror("trying to construct idx1 with negative dimension " << size0);
  dim = NULL; mod = NULL;
  offset = o;
  ndim = -1; // required in constructors to avoid side effects in setndim
  setndim(1);
  dim[0] = size0;
  mod[0] = 1;
}

// constructor for idx2
idxspec::idxspec(intg o, intg size0, intg size1) {
  if (size0 < 0 || size1 < 0)
    eblerror("trying to construct idx2 with negative dimensions: "
             << size0 << "x" << size1 << " offset: " << o);
  dim = NULL; mod = NULL;
  offset = o;
  ndim = -1; // required in constructors to avoid side effects in setndim
  setndim(2);
  dim[0] = size0;
  mod[0] = size1;
  dim[1] = size1;
  mod[1] = 1;
}

// constructor for idx3
idxspec::idxspec(intg o, intg size0, intg size1, intg size2) {
  if (size0 < 0 || size1 < 0 || size2 < 0)
    eblerror("trying to construct idx3 with negative dimensions: "
             << size0 << "x" << size1 << "x" << size2 << " offset: " << o);
  dim = NULL; mod = NULL;
  offset = o;
  ndim = -1; // required in constructors to avoid side effects in setndim
  setndim(3);
  dim[0] = size0;
  mod[0] = size1 * size2;
  dim[1] = size1;
  mod[1] = size2;
  dim[2] = size2;
  mod[2] = 1;
}

// generic constructor for any dimension.
idxspec::idxspec(intg o, intg s0, intg s1, intg s2, intg s3,
                 intg s4, intg s5, intg s6, intg s7) {
  init_spec(o, s0, s1, s2, s3, s4, s5, s6, s7);
}

#define DIMS_ERROR(v)							\
  eblerror("incompatible dimensions (error " << v			\
	   << ") while trying to resize from (" << *this << ") to ("	\
	   << s0 << " x " << s1 << " x " << s2 << " x " << s3 << " x "	\
	   << s4 << " x " << s5 << " x " << s6 << " x " << s7 << ")");

// generic constructor for any dimension.
void idxspec::init_spec(intg o, intg s0, intg s1, intg s2, intg s3,
                        intg s4, intg s5, intg s6, intg s7) {
  bool ndimset = false;
  intg md = 1;
  dim = NULL; mod = NULL;
  offset = o;
  ndim = -1; // required in constructors to avoid side effects in setndim
  if (s7>=0) {
    if (!ndimset) { setndim(8); ndimset = true; }
    dim[7] = s7; mod[7] = md; md *= s7;
  } else { if (ndimset) { DIMS_ERROR(-8); } }
  if (s6>=0) {
    if (!ndimset) { setndim(7); ndimset = true; }
    dim[6] = s6; mod[6] = md; md *= s6;
  } else { if (ndimset) { DIMS_ERROR(-7); } }
  if (s5>=0) {
    if (!ndimset) { setndim(6); ndimset = true; }
    dim[5] = s5; mod[5] = md; md *= s5;
  } else { if (ndimset) { DIMS_ERROR(-6); } }
  if (s4>=0) {
    if (!ndimset) { setndim(5); ndimset = true; }
    dim[4] = s4; mod[4] = md; md *= s4;
  } else { if (ndimset) { DIMS_ERROR(-5); } }
  if (s3>=0) {
    if (!ndimset) { setndim(4); ndimset = true; }
    dim[3] = s3; mod[3] = md; md *= s3;
  } else { if (ndimset) { DIMS_ERROR(-4); } }
  if (s2>=0) {
    if (!ndimset) { setndim(3); ndimset = true; }
    dim[2] = s2; mod[2] = md; md *= s2;
  } else { if (ndimset) { DIMS_ERROR(-3); } }
  if (s1>=0) {
    if (!ndimset) { setndim(2); ndimset = true; }
    dim[1] = s1; mod[1] = md; md *= s1;
  } else { if (ndimset) { DIMS_ERROR(-2); } }
  if (s0>=0) {
    if (!ndimset) { setndim(1); ndimset = true; }
    dim[0] = s0; mod[0] = md;
  } else { if (ndimset) { DIMS_ERROR(-1); } }
  if (!ndimset) { setndim(0); }
}

// generic constructor for any dimension.
void idxspec::init_spec(intg o, intg s0, intg s1, intg s2, intg s3,
                        intg s4, intg s5, intg s6, intg s7, uint n) {
  intg md = 1;
  dim = NULL; mod = NULL;
  offset = o;
  ndim = -1; // required in constructors to avoid side effects in setndim
  setndim(n);
  switch (n) {
    case 8: dim[7] = s7; mod[7] = md; md *= s7; if (s7 < 0) DIMS_ERROR(-8);
    case 7: dim[6] = s6; mod[6] = md; md *= s6; if (s6 < 0) DIMS_ERROR(-7);
    case 6: dim[5] = s5; mod[5] = md; md *= s5; if (s5 < 0) DIMS_ERROR(-6);
    case 5: dim[4] = s4; mod[4] = md; md *= s4; if (s4 < 0) DIMS_ERROR(-5);
    case 4: dim[3] = s3; mod[3] = md; md *= s3; if (s3 < 0) DIMS_ERROR(-4);
    case 3: dim[2] = s2; mod[2] = md; md *= s2; if (s2 < 0) DIMS_ERROR(-3);
    case 2: dim[1] = s1; mod[1] = md; md *= s1; if (s1 < 0) DIMS_ERROR(-2);
    case 1: dim[0] = s0; mod[0] = md;  if (s0 < 0) DIMS_ERROR(-1);
  }
}

idxspec::idxspec(intg o, const idxdim &d) {
  init_spec(o, d.dims[0], d.dims[1], d.dims[2], d.dims[3], d.dims[4],
            d.dims[5], d.dims[6], d.dims[7], d.order());
}

// generic constructor for any dimension.
// The dim and mod arrays past as argument are copied.
idxspec::idxspec(intg o, int n, intg *ldim, intg *lmod) {
  DEBUG_LOW("idxspec::idxspec: " << (intg)this);
  dim = NULL; mod = NULL;
  offset = o;
  ndim = -1; // required in constructors to avoid side effects in setndim
  setndim(n);
  for (int i = 0; i < n; i++) {
    if (ldim[i] < 0) eblerror("negative dimension");
    dim[i] = ldim[i]; mod[i] = lmod[i];
  }
}

// accessors /////////////////////////////////////////////////////////////////

intg idxspec::getoffset() const {
  return offset;
}

void idxspec::add_offset(intg off) {
  offset += off;
}

int idxspec::getndim() const {
  return ndim;
}

intg idxspec::footprint() const {
  intg r = offset + 1;
  for (int i = 0; i < ndim; i++)
    r += mod[i] * (dim[i]-1);
  return r;
}

//! total number of elements accessed by idxspec
intg idxspec::nelements() const {
  intg r = 1;
  for (int i = 0; i < ndim; i++)
    r *= dim[i];
  return r;
}

bool idxspec::contiguousp() const {
  intg size = 1; bool r = true;
  for(int i=ndim-1; i>=0; i--){
    if (size != mod[i]) r = false;
    size *= dim[i];
  }
  return r;
}

intg idxspec::true_order() const {
  intg order = 0;
  for(int i = 0; i < ndim; i++) if (dim[i] > 1) order = i + 1;
  return order;
}

////////////////////////////////////////////////////////////////
// private methods

// set the order (number of dimensions).
// This is used to allocate/deallocate the dim/mod arrays.
// It is also used to deallocate them by passing a zero argument.
// Hence, if ndim is reduced, the arrays are preserved.
int idxspec::setndim(int n) {
  if ((n<0) || (n>MAXDIMS))
    eblerror("idx: cannot set ndim with n=" << n << " MAXDIMS=" << MAXDIMS);
  // if new ndim is zero or larger than before: deallocate arrays
  if ((n == 0) || (n > ndim)) {
    if (dim) { delete []dim; }
    if (mod) { delete []mod; }
    dim = NULL; mod = NULL;
  }
  // now allocate new arrays if necessary
  if (n > 0) {
    if (!dim) { dim = new intg[n]; }
    if (!mod) { mod = new intg[n]; }
  }
  // if the arrays allocated and ndim was larger
  // than new ndim, we don't do anything.
  ndim = n;
  return ndim;
}

// set the order (number of dimensions).
// using pre-allocated mod/dim arrays.
// Probably not useful.
int idxspec::setndim(int n, intg *ldim, intg *lmod) {
  if ((n<1) || (n>=MAXDIMS))
    eblerror("idx: cannot set ndim with n=" << n << " MAXDIMS=" << MAXDIMS);
  if (dim) { delete []dim; }
  if (mod) { delete []mod; }
  dim = ldim;
  mod = lmod;
  ndim = n;
  return ndim;
}

//intg idxspec::resize( intg* dimsBegin, intg* dimsEnd ){
//
//	const int nArgDims = std::distance(dimsBegin, dimsEnd);
//
//	// Error-check the supplied number of dims.
//	if( ndim == 0 ){
//		eblerror("Cannot call resize on a 0-dimensional idxspec.");
//	}
//	else if( ndim != nArgDims ){
//		std::ostringstream oss;
//		oss<<"Number of supplied dimension sizes ("<<nArgDims;
//            oss<<") doesn't match idxspec's number of dims ("<<ndim<<")";
//		eblerror(oss.str().c_str());
//	}
//
//	// copy dimensions to dim
//	std::copy(dimsBegin, dimsEnd, dim);
//
//	// set mod to be the partial sum of the dim sequence, in reverse order.
//	typedef std::reverse_iterator<intg*> RIter;
//	std::partial_sum(RIter(dimsEnd-1), RIter(dimsBegin-1),
//                     RIter(mod+(nArgDims-1)), std::multiplies<intg>());
//
//	// return the memory footprint
//	return mod[0] * dim[0] + offset;
//}

// resizing: order is not allowed to change
intg idxspec::resize(intg s0, intg s1, intg s2, intg s3,
                     intg s4, intg s5, intg s6, intg s7) {
  intg md = 1;
  // resizeing non-contiguous is forbiden to prevent nasty bugs
  if (!contiguousp())
    eblerror("Resizing non-contiguous idx is not allowed");
  if (ndim==0) { DIMS_ERROR(0); }  // can't resize idx0
  if (s7>=0) {
    if (ndim<8) DIMS_ERROR(8);
    dim[7] = s7; mod[7] = md; md *= s7;
  } else { if (ndim>7) DIMS_ERROR(-8); }
  if (s6>=0) {
    if (ndim<7) DIMS_ERROR(7);
    dim[6] = s6; mod[6] = md; md *= s6;
  } else { if (ndim>6) DIMS_ERROR(-7); }
  if (s5>=0) {
    if (ndim<6) DIMS_ERROR(6);
    dim[5] = s5; mod[5] = md; md *= s5;
  } else { if (ndim>5) DIMS_ERROR(-6); }
  if (s4>=0) {
    if (ndim<5) DIMS_ERROR(5);
    dim[4] = s4; mod[4] = md; md *= s4;
  } else { if (ndim>4) DIMS_ERROR(-5); }
  if (s3>=0) {
    if (ndim<4) DIMS_ERROR(4);
    dim[3] = s3; mod[3] = md; md *= s3;
  } else { if (ndim>3) DIMS_ERROR(-4); }
  if (s2>=0) {
    if (ndim<3) DIMS_ERROR(3);
    dim[2] = s2; mod[2] = md; md *= s2;
  } else { if (ndim>2) DIMS_ERROR(-3); }
  if (s1>=0) {
    if (ndim<2) DIMS_ERROR(2);
    dim[1] = s1; mod[1] = md; md *= s1;
  } else { if (ndim>1) DIMS_ERROR(-2); }
  if (s0>=0) {
    if (ndim<1) DIMS_ERROR(1);
    dim[0] = s0; mod[0] = md; md *= s0;
  } else { if (ndim>0) DIMS_ERROR(-1); }
  return md + offset; // return new footprint
}

intg idxspec::resize(const idxdim &d) {
  return resize(d.dims[0], d.dims[1], d.dims[2], d.dims[3],
                d.dims[4], d.dims[5], d.dims[6], d.dims[7]);
}

// resize one dimension <dimn> with size <size>.
// only already allocated dimensions can be resized
// (order is not allowed to change)
intg idxspec::resize1(intg dimn, intg size) {
  // resizeing non-contiguous is forbiden to prevent nasty bugs
  if (!contiguousp()) eblerror("Resizing non-contiguous idx is not allowed");
  if ((dimn >= ndim) || (dimn < 0))
    eblerror("idxspec::resize1: cannot resize an unallocated dimension");
  if (size < 0)
    eblerror("idxspec::resize1: cannot resize with a negative size");
  // since we know the current spec is valid, no need for error checking,
  // simply assign new dimension and propagate new mods.
  dim[dimn] = size;
  for (int i = dimn - 1; i >= 0; --i) {
    mod[i] = dim[i + 1] * mod[i + 1];
  }
  return mod[0] * dim[0] + offset; // return new footprint
}

////////////////////////////////////////////////////////////////
// public methods

// assignment operators: copies new dim/mod arrays
const idxspec& idxspec::operator=(const idxspec &src) {
  if (this != &src) { copy(src); }
  return *this;
}

// copy method: this allocates new dim/mod arrays
// and copies them from original
void idxspec::copy( const idxspec &src) {
  DEBUG_LOW("idxspec::copy: " << (intg)this);
  offset = src.offset;
  // we do not initialize ndim before setndim here because it may already
  // be initialized.
  setndim(src.ndim);
  if (ndim > 0) {
    memcpy(dim, src.dim, ndim * sizeof(intg));
    memcpy(mod, src.mod, ndim * sizeof(intg));
  }
}

////////////////////////////////////////////////////////////////

// pretty print
void idxspec::pretty(FILE *f) const {
  int i;
  fprintf(f,"  idxspec %ld\n",(intg)this);
  fprintf(f,"    ndim=%d\n",ndim);
  fprintf(f,"    offset=%ld\n",offset);
  if (ndim>0) {
    fprintf(f,"    dim=[ ");
    for (i=0; i<ndim-1; i++){ fprintf(f,"%ld, ",dim[i]); }
    fprintf(f,"%ld]\n",dim[ndim-1]);
    fprintf(f,"    mod=[ ");
    for (i=0; i<ndim-1; i++){ fprintf(f,"%ld, ",mod[i]); }
    fprintf(f,"%ld]\n",mod[ndim-1]);
  } else {
    fprintf(f,"    dim = %ld, mod = %ld\n",(intg)dim, (intg)mod);
  }
  fprintf(f,"    footprint= %ld\n",footprint());
  fprintf(f,"    contiguous= %s\n",(contiguousp())?"yes":"no");
}

void idxspec::pretty(std::ostream& out) const {
  int i;
  out << "  idxspec " << (intg)this << "\n";
  out << "    ndim= " << ndim << "\n";
  out << "    offset= " << offset << "\n";
  if (ndim>0) {
    out << "    dim=[ ";
    for (i=0; i<ndim-1; i++){ out << dim[i] << ", "; }
    out << dim[ndim-1] << "]\n";
    out << "    mod=[ ";
    for (i=0; i<ndim-1; i++){ out << mod[i] << ", "; }
    out << mod[ndim-1] << "]\n";
  } else {
    out << "    dim = " << (intg)dim << ", mod = " << (intg)mod <<"\n";
  }
  out << "    footprint= " << footprint() << "\n";
  out << "    contiguous= " << ((contiguousp())? "yes":"no") << "\n";
}
////////////////////////////////////////////////////////////////
// select, narrow, unfold, etc
// Each function has 3 version:
// 1. XXX_into: which writes the result
// into an existing idxspec apssed as argument.
// 2. XXX_inplace: writes into the current idxspec
// 3. XXX: creates a new idxspec and returns it.

intg idxspec::select_into(idxspec *dst, int d, intg n) const {
  if (ndim <= 0)
    eblerror("cannot select an empty idx idx that is a scalar ("
             << *this << ")");
  if ((n < 0) || (n >= dim[d]))
    eblerror("trying to select layer " << n
             << " of dimension " << d << " which is of size "
             << dim[d] << " in idx " << *this);
  // this preserves the dim/mod arrays if dst == this
  dst->setndim(ndim-1);
  dst->offset = offset + n * mod[d];
  if (ndim -1 > 0) { // dim and mod don't exist for idx0
    for (int j=0; j<d; j++) {
      dst->dim[j] = dim[j];
      dst->mod[j] = mod[j];
    }
    for (int j=d; j<ndim-1; j++) {
      dst->dim[j] = dim[j+1];
      dst->mod[j] = mod[j+1];
    }
  }
  return n;
}

intg idxspec::select_inplace(int d, intg n) {
  return select_into(this, d, n);
}

idxspec idxspec::select(int d, intg n) {
  // create new idxspec of order ndim-1
  idxspec r;
  select_into(&r, d, n);
  return r;
}

////////////////////////////////////////////////////////////////

intg idxspec::narrow_into(idxspec *dst, int d, intg s, intg o) {
  if (ndim <= 0)
    eblerror("cannot narrow a scalar");
  if ((d < 0) || (d>=ndim))
    eblerror("narrow: illegal dimension index " << d << " in " << *this);
  if ((o < 0)||(s < 1)||(s+o > dim[d]))
    eblerror("trying to narrow dimension " << d << " to size " << s
             << " starting at offset " << o << " in " << *this);
  // this preserves the dim/mod arrays if dst == this
  dst->setndim(ndim);
  dst->offset = offset + o * mod[d];
  for (int j=0; j<ndim; j++) {
    dst->dim[j] = dim[j];
    dst->mod[j] = mod[j];
  }
  dst->dim[d] = s;
  return s;
}

intg idxspec::narrow_inplace(int d, intg s, intg o) {
  return narrow_into(this, d, s, o);
}

idxspec idxspec::narrow(int d, intg s, intg o) {
  // create new idxspec of order ndim
  idxspec r;
  narrow_into(&r, d, s, o);
  return r;
}

////////////////////////////////////////////////////////////////
// transpose

// tranpose two dimensions into pre-existing idxspec
int idxspec::transpose_into(idxspec *dst, int d1, int d2) {
  if ((d1 < 0) || (d1 >= ndim) || (d2 < 0) || (d2 >= ndim))
    eblerror("illegal transpose of dimension " << d1
             << " to dimension " << d2);
  // this preserves the dim/mod arrays if dst == this
  dst->setndim(ndim);
  dst->offset = offset;
  for (int j=0; j<ndim; j++) {
    dst->dim[j] = dim[j];
    dst->mod[j] = mod[j];
  }
  intg tmp;
  // we do this in case dst = this
  tmp=dim[d1]; dst->dim[d1]=dim[d2]; dst->dim[d2]=tmp;
  tmp=mod[d1]; dst->mod[d1]=mod[d2]; dst->mod[d2]=tmp;
  return ndim;
}

// tranpose all dims with a permutation vector
int idxspec::transpose_into(idxspec *dst, int *p) {
  for (int i=0; i<ndim; i++) {
    if ((p[i] < 0) || (p[i] >= ndim))
      eblerror("illegal transpose of dimensions");
  }
  dst->setndim(ndim);
  dst->offset = offset;
  if (dst == this) {
    // we need temp storage if done in place
    intg tmpdim[MAXDIMS], tmpmod[MAXDIMS];
    for (int j=0; j<ndim; j++) {
      tmpdim[j] = dim[p[j]];
      tmpmod[j] = mod[p[j]];
    }
    for (int j=0; j<ndim; j++) {
      dst->dim[j] = tmpdim[j];
      dst->mod[j] = tmpmod[j];
    }
  } else {
    // not in place
    for (int j=0; j<ndim; j++) {
      dst->dim[j] = dim[p[j]];
      dst->mod[j] = mod[p[j]];
    }
  }
  return ndim;
}

int idxspec::transpose_inplace(int d1, int d2) {
  return transpose_into(this, d1, d2);
}

int idxspec::transpose_inplace(int *p) {
  return transpose_into(this, p);
}

idxspec idxspec::transpose(int d1, int d2) {
  idxspec r;
  transpose_into(&r, d1, d2);
  return r;
}

idxspec idxspec::transpose(int *p) {
  idxspec r;
  transpose_into(&r, p);
  return r;
}

////////////////////////////////////////////////////////////////
// unfold

// d: dimension; k: kernel size; s: stride.
intg idxspec::unfold_into(idxspec *dst, int d, intg k, intg s) {
  intg ns; // size of newly created dimension
  std::string err;
  if (ndim <= 0)
    err << "cannot unfold an idx of maximum order";
  else if ((d < 0) || (d>=ndim))
    err << "unfold: illegal dimension index";
  else if ((k < 1) || (s < 1))
    err << "unfold: kernel and stride must be >= 1";
  ns = 1+ (dim[d]-k)/s;
  if (!err.size() && ((ns <= 0) || ( dim[d] != s*(ns-1)+k )))
    err << "unfold: kernel and stride incompatible with size";
  if (err.size())
    eblerror(err << ", while unfolding dimension " << d << " to size " << k
             << " with step " << s << " from idx " << *this << " into idx "
             << *dst);
  // this preserves the dim/mod arrays if dst == this
  dst->setndim(ndim+1);
  dst->offset = offset;
  for (int i=0; i<ndim; i++) {
    dst->dim[i] = dim[i];
    dst->mod[i] = mod[i];
  }
  dst->dim[ndim] = k;
  dst->mod[ndim] = mod[d];
  dst->dim[d] = ns;
  dst->mod[d] = mod[d]*s;
  return ns;
}

intg idxspec::unfold_inplace(int d, intg k, intg s) {
  return unfold_into(this, d, k, s);
}

idxspec idxspec::unfold(int d, intg k, intg s) {
  idxspec r;
  unfold_into(&r, d, k, s);
  return r;
}

////////////////////////////////////////////////////////////////

// return true if two idxspec have the same dimensions,
// i.e. if all their dimensions are equal (regardless of strides).
bool same_dim(idxspec &s1, idxspec &s2) {
  if ( s1.ndim != s2.ndim ) return false;
  for (int i=0; i<s1.ndim; i++) { if (s1.dim[i] != s2.dim[i]) return false; }
  return true;
}

std::ostream& operator<<(std::ostream& out, const idxspec& d) {
  if (d.getndim() < 0) out << "<empty>";
  else if (d.getndim() == 0) out << "<scalar>";
  else {
    out << d.dim[0];
    for (int i = 1; i < d.getndim(); ++i)
      out << "x" << d.dim[i];
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, idxspec& d) {
  if (d.getndim() < 0) out << "<empty>";
  else if (d.getndim() == 0) out << "<scalar>";
  else {
    out << d.dim[0];
    for (int i = 1; i < d.getndim(); ++i)
      out << "x" << d.dim[i];
  }
  return out;
}

std::string& operator<<(std::string& out, const idxspec& d) {
  if (d.getndim() < 0) out << "<empty>";
  else if (d.getndim() == 0) out << "<scalar>";
  else {
    out << d.dim[0];
    for (int i = 1; i < d.getndim(); ++i)
      out << "x" << d.dim[i];
  }
  return out;
}

std::string& operator<<(std::string& out, idxspec& d) {
  if (d.getndim() < 0) out << "<empty>";
  else if (d.getndim() == 0) out << "<scalar>";
  else {
    out << d.dim[0];
    for (int i = 1; i < d.getndim(); ++i)
      out << "x" << d.dim[i];
  }
  return out;
}

} // end namespace ebl
