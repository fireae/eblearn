/***************************************************************************
 *   Copyright (C) 2011 by Yann LeCun and Pierre Sermanet *
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

#ifndef IDXITER_H_
#define IDXITER_H_

#include <string.h>

namespace ebl {

// forward declarations
template <typename T> class idx;

// idxlooper ///////////////////////////////////////////////////////////////////

//! idxlooper: a kind of iterator used by bloop
//! and eloop macros. idxlooper is a subclass of idx,
//! It is used as follows:
//! for (idxlooper z(&idx,0); z.notdone(); z.next()) { .... }
template <class T> class idxlooper : public idx<T> {
 public:
	//! generic constructor loops over dimensin ld
	idxlooper(idx<T> &m, int ld);
	//! return true if loop is over
	bool notdone();
	//! increment to next item. Return pointer to data.
	T *next();
	void operator++();
	// Making hidden operator visible.
	using idx<T>::operator=;

 protected:
	intg i;  // loop index
	intg dimd;  // number of elements to iterated upon
	intg modd; // stride in dimension being iterated upon
};

////////////////////////////////////////////////////////////////
// idx Iterators: gives you a pointer to the actual data,
// unlike idxlooper which gives you another idx.

//! idxiter allows to iterate over all elements of an idx.
//! Although it can be used directly, it is easier to use
//! it with the idx_aloopX macros. Example:
//!  idxiter<double> idx;
//!  for ( idx.init(m); idx.notdone(); idx.next() ) {
//!    printf("%g ",*idx);
//!  }
//! Here is an example that uses the aloop macro to fill up
//! an idx with numbers corresponding to the loop index:
//! idxiter<double> idx;
//! idx_aloop_on(idx,m) { *idx = idx.i; }
//! At any point during the loop, the indices of the element
//! being worked on is stored in idx.d[k] for k=0
//! to idx.order()-1.
template <class T> class idxiter {
 public:
	//! empty constructor;
	idxiter();
	//! Initialize an idxiter to the start of
	//! the idx passed as argument.
	T *init(const idx<T> &m);
	//! Return true while the loop is not completed
	bool notdone();
	//! Increments idxiter to next element
	T *next();
	//! dereferencing operator: returns data item.
	T& operator*() { return *data; }
 protected:
	T *data; //!< pointer to current item
	intg i; //!< number of elements visited so far (loop index)
	intg n; //!< total number of elements in idx
	int j; //!< dimension being looped over
	intg d[MAXDIMS]; //!< loop index array for non-contiguous idx
	const idx<T> *iterand; //!< pointer to idx being looped over.
};

template <class T> class contiguous_idxiter {
 public:
	inline contiguous_idxiter() {}
	inline contiguous_idxiter(idx<T> & m)
			:current(m.storage->data + m.spec.offset), end(NULL) {
#ifdef __DEBUG__
		if (!m.contiguousp())
			eblerror("this idx should be contiguous");
#endif
		end = current + (unsigned)m.nelements();
	}
	//inline intg i ();
	inline bool notdone () const {return current < end;}
	inline void next() {++current;}
	inline T& operator * () {return *current;}
	inline void operator += (intg i) {current += i;}
 protected: // members
	T *current, *end;
};

template <class T> class noncontiguous_idxiter {
 public:
	T* data;
	intg d[MAXDIMS];
	intg i, n;
	int j, jmax;
	idx<T>* iterand;
	inline noncontiguous_idxiter() {}
	inline noncontiguous_idxiter(idx<T> & m)
			:data(m.storage->data + m.spec.offset), i(0), n(m.spec.nelements()),
			 j(m.spec.ndim - 1), jmax(m.spec.ndim - 1), iterand(&m) {
		memset(d, 0, MAXDIMS * sizeof(intg));
	}
	inline bool notdone () const {return i < n;}
	inline void next () {
		++i;
		while (j >= 0) {
			if (++d[j] >= iterand->spec.dim[j]) {
				data -= iterand->spec.dim[j] * iterand->spec.mod[j]; //TODO precalculate?
				d[j--] = -1;
				continue;
			}
			data += iterand->spec.mod[j];
			if (j != jmax)
				j++;
			else
				return;
		}
	}
	inline T& operator * () {return *data;}
	inline void operator += (intg k) {
		if (k + i >= n) {
			i = n;
			return;
		}
		i += k;
		intg m = 1;
		intg t;
		while ((t = (iterand->spec.dim[j] - d[j]) * m) < k) {
			k += d[j];
			d[j] = 0;
			data -= d[j] * iterand->spec.mod[j];
			m *= iterand->spec.dim[j];
			--j;
		}
		for(;;) {
			t = k/m;
			d[j] += t;
			data += iterand->spec.mod[j] * t;
			if (j == jmax)
				return;
			++j;
			k -= t*m;
			m /= iterand->spec.dim[j];
		}
	}
};

#define new_fast_idxiter(itr, src, type, code)  \
	if (src.contiguousp()) {                      \
		contiguous_idxiter<type> itr (src);         \
		{ code }                                    \
				} else {                                \
		noncontiguous_idxiter<type> itr (src);      \
		{ code }                                    \
				}

#define idx_aloopf1(itr0, src0, type0, code)				\
	if (src0.contiguousp()) {						\
		for (contiguous_idxiter<type0> itr0 (src0); itr0.notdone();		\
	 itr0.next()) { code }                                          \
		} else {                                                            \
		for (noncontiguous_idxiter<type0> itr0 (src0); itr0.notdone();	\
	 itr0.next()) { code }                                          \
					}

#define idx_aloopf2(itr0, src0, type0, itr1, src1, type1, code) \
	if (src0.contiguousp()) {                                     \
		if (src1.contiguousp()) {                                   \
			contiguous_idxiter<type0> itr0 (src0);                    \
			contiguous_idxiter<type1> itr1 (src1);                    \
			for (; itr0.notdone(); itr0.next(), itr1.next())          \
			{ code }                                                  \
						} else {                                            \
			contiguous_idxiter<type0> itr0 (src0);                    \
			noncontiguous_idxiter<type1> itr1 (src1);                 \
			for (; itr0.notdone(); itr0.next(), itr1.next())          \
			{ code }                                                  \
						}                                                   \
	} else {                                                      \
		if (src1.contiguousp()) {                                   \
			noncontiguous_idxiter<type0> itr0 (src0);                 \
			contiguous_idxiter<type1> itr1 (src1);                    \
			for (; itr0.notdone(); itr0.next(), itr1.next())          \
			{ code }                                                  \
						} else {                                            \
			noncontiguous_idxiter<type0> itr0 (src0);                 \
			noncontiguous_idxiter<type1> itr1 (src1);                 \
			for (; itr0.notdone(); itr0.next(), itr1.next())          \
			{ code  }                                                 \
						}                                                   \
	}

#define idx_aloopf3(itr0, src0, type0, itr1, src1, type1, itr2, src2,	\
				type2, code)					\
	new_fast_idxiter(itr0, src0, type0, {					\
			new_fast_idxiter(itr1, src1, type1, {				\
					new_fast_idxiter(itr2, src2, type2, {				\
							for (; itr0.notdone(); itr0.next(), itr1.next(), itr2.next()) \
							{ code }                                                  \
										})                                                  \
							})                                                        \
					})

} // end namespace ebl

#include "idxiter.hpp"

#endif /* IDXITER_H_ */
