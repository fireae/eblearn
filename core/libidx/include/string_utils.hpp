/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet   *
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

#ifndef STRING_UTILS_HPP_
#define STRING_UTILS_HPP_

#include <map>
#include <string>
#include <sstream>
#include "stl.h"

namespace ebl {

  template <typename T1, typename T2>
  std::string map_to_string(std::map<T1,T2> &m) {
    std::string s;
    typename std::map<T1,T2>::iterator j;
    for (j = m.begin(); j != m.end(); ++j)
      s << "(" << j->first << ", " << j->second << ") ";
    return s;
  }

  template <typename T1, typename T2>
  std::string map_to_string2(std::map<T1,T2> &m) {
    std::ostringstream s;
    typename std::map<T1,T2>::iterator j;
    for (j = m.begin(); j != m.end(); ++j)
      s << j->first << ": " << j->second << std::endl;
    return s.str();
  }

  template <typename T>
  void list_to_vector(std::list<T> &l, std::vector<T> &v) {
    v.resize(l.size());
    typename std::vector<T>::iterator iv = v.begin();
    typename std::list<T>::iterator il = l.begin();
    for ( ; il != l.end(); ++iv, ++il) {
      *iv = *il;
    }
  }

  template <typename T>
  idx<T> string_to_idx(const char *s_, char sep) {
    idx<T> d(1);
    std::string s = s_;
    int k = 0;
    bool found = false;
    while (s.size()) {
      uint j;
      for (j = 0; j < s.size(); ++j)
	if (s[j] == sep)
	  break ;
      std::string s0 = s.substr(0, j);
      if (j >= s.size())
	s = "";
      else
	s = s.substr(j + 1, s.size());
      if (!s0.empty()) {
	if (!found)
	  d.set(string_to_number<T>(s0.c_str()), 0); // 1st element
	else {
	  d.resize(d.dim(0) + 1);
	  d.set(string_to_number<T>(s0.c_str()), d.dim(0) - 1);
	}
	found = true;
	k++;
      }
    }
    if (!found)
      eblerror("expected at least 1 number in: " << s_);
    return d;
  }

  template <typename T>
  T string_to_number(const char *s_) {
    return (T) string_to_double(s_);
  }

} // end namespace ebl

#endif /* STRING_UTILS_HPP_ */
