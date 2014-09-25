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

#include "idx.h"

namespace ebl {

#ifdef __DEBUGMEM__

void pretty_memory(const char *prefix) {
  // srg
  intg total = 0;
  total += srg<double>::memsize;
  total += srg<float>::memsize;
  total += srg<int>::memsize;
  total += srg<long>::memsize;
  total += srg<short>::memsize;
  total += srg<unsigned char>::memsize;
  total += srg<const char *>::memsize;
  total += srg<void*>::memsize;
  total += srg<uint>::memsize;
  total += srg<bool>::memsize;
  // locks
  intg locks = smart_pointer::locks;
  intg sp = smart_pointer::spointers;
  // print
  eblprint( prefix << " idx total_memory_usage="
            << total / (float) 1048576 << " Mb"
            << ", Locks: " << locks
            << ", smart pointers: " << sp << std::endl);
}

template <> std::string idx<double>::info() {
  std::string s;
  s << "(" << this->spec;
  s << " min " << idx_min(*this);
  s << " max " << idx_max(*this);
  s << " mean " << idx_mean(*this);
  s << " sum " << idx_sum(*this);
  s << ")";
  return s;
}

template <> std::string idx<float>::info() {
  std::string s;
  s << "(" << this->spec;
  s << " min " << idx_min(*this);
  s << " max " << idx_max(*this);
  s << " mean " << idx_mean(*this);
  s << " sum " << idx_sum(*this);
  s << ")";
  return s;
}

#endif

} // end namespace ebl
