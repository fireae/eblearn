/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com   *
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

#ifndef SRG_HPP_
#define SRG_HPP_

#include <stdlib.h>
#include <string.h>

#ifdef __DEBUG__
#include <typeinfo>
#endif

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // the creation of an idx should call lock() on the srg,
  // and its destruction should call unlock().

  // simplest constructor builds an empty srg
  template <typename T> srg<T>::srg() {
    //    refcount = 0;
    data = (T *)NULL;
    size_ = 0;
#ifdef __DEBUG__
    smart_pointer::debug_name << "srg<" << typeid(T).name() << ">";
#endif
  }

  // return size on success, and -1 on error
  template <typename T> srg<T>::srg(intg s) {
    intg r;
    //    refcount = 0;
    data = (T *)NULL;
    if ( ( r=this->changesize(s) ) > 0 ) this->clear();
    if (r < 0) { eblerror("can't allocate srg"); }
  }

  // destructor: can be called twice when the srg
  // is not dynamically allocated. Hence this must
  // make sure the data is not deallocated twice.
  template <typename T> srg<T>::~srg() {
    // DEBUG_LOW("srg::destructor: refcount = " << refcount);
    // if (refcount != 0) {
    //   eblerror("can't delete an srg with non zero refcount"); }

    if (data != NULL) {
      DEBUG_LOW("srg: freeing data " << (void*) data);
      free((void *) data);
      data = NULL;
#ifdef __DEBUGMEM__
      this->memsize -= size_ * sizeof (T);
#endif    
      size_ = 0;
    }
  }

  // return size
  template <typename T> intg srg<T>::size() { return size_; }

  // low-level resize: can grow and shrink
  // returns -1 on failure.
  // Self should be used with care, because shrinking
  // an srg that has idx pointing to it is very dangerous.
  // In most case, the grow() method should be used.
  template <typename T> intg srg<T>::changesize(intg s) {
#ifdef __DEBUGMEM__
    this->memsize -= size_ * sizeof (T);
#endif    
    if (s == 0) {
      free((void*) data);
      data = (T*) NULL;
      size_ = 0;
    } else {
      // TODO: malloc/realloc not thread safe
      // use google's tcalloc? (supposedly much faster:
      // http://google-perftools.googlecode.com/svn/trunk/doc/tcmalloc.html    
      data = (T*) realloc((void*) data, s * sizeof (T));
      if (data != NULL) {
	size_ = s;
#ifdef __DEBUGMEM__
	this->memsize += size_ * sizeof (T);
#endif
	DEBUG_LOW("(re)allocated data " << (void*) data);
	return size_;
      } else {
	size_ = 0;
	return -1;
      }
    }
#ifdef __DEBUGMEM__
    this->memsize += size_ * sizeof (T);
#endif    
    return size_;
  }

  // this grows the size of the srg if necessary.
  // This is called when a new idx is created on the srg.
  // returns -1 on failure.
  template <typename T> intg srg<T>::growsize(intg s) {
    if (s > size_) { return this->changesize(s); } else { return size_; }
  }

  template<typename T> intg srg<T>::growsize_chunk(intg s, intg s_chunk){
    if(s > size_) { return this->changesize(s + s_chunk); } else {return size_;}
  }

  // // decrement refcount and free if it reaches zero
  // template <typename T> int srg<T>::unlock() {
  //   refcount--;
  //   //if (refcount == 0) this->nopened--;
  //   DEBUG_LOW("srg::unlock: refcount = " << refcount << " srg " << this
  // 	      << " data " << (void*) data);
  //   if (refcount<0) {
  //     eblerror("srg negative reference counter: " << refcount);
  //     return refcount;
  //   }
  //   else {
  //     if (refcount == 0) {
  // 	delete this;
  // 	return 0;
  //     } else {
  // 	return refcount;
  //     }
  //   }
  // }

  // // increment refcount
  // template <typename T> int srg<T>::lock() {
  //   //  if (refcount == 0) this->nopened++;
  //   DEBUG_LOW("srg::lock: refcount=" << refcount+1 << " srg " << this
  // 	      << " data " << (void*) data);
  //   return ++refcount;
  // }

  // get i-th item
  template <typename T> T srg<T>::get(intg i) { return data[i]; }

  // get i-th item
  template <typename T> T* srg<T>::get_data() { return data; }
  // set data pointer
  template <typename T> void srg<T>::set_data(T* ptr) { data=ptr; }

  // set i-th item
  template <typename T> void srg<T>::set(intg i, T val) { data[i] = val; }

  // fill with a ubyte value
  template <typename T> void srg<T>::clear() {
    memset(data, 0, size_ * sizeof(T));
  }

  // prints innards
  template <typename T> void srg<T>::pretty(FILE *f) {
    fprintf(f,"srg at address %ld; ",(long)this);
    fprintf(f,"size=%ld; ",size_);
    fprintf(f,"data=%ld; ",(long)data);
    //    fprintf(f,"refcount=%d\n",refcount);
  }
  
} // end namespace ebl

#endif /* SRG_HPP_ */
