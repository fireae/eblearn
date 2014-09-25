/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet *
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

#include "string_utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <istream>
#include <ostream>
#include <errno.h>

using namespace std;

namespace ebl {

uint string_to_uint(const std::string &s) {
  return string_to_uint(s.c_str());
}

uint string_to_uint(const char *s) {
  istringstream iss(s);
  uint d;
  iss >> d;
  if (iss.fail()) eblthrow("\"" << s << "\" is not an unsigned int");
  return d;
}

bool string_to_bool(const std::string &s) {
  return (bool) string_to_int(s.c_str());
}

int string_to_int(const std::string &s) {
  return string_to_int(s.c_str());
}

int string_to_int(const char *s) {
  istringstream iss(s);
  int d;
  iss >> d;
  if (iss.fail()) eblthrow("\"" << s << "\" is not an int");
  return d;
}

intg string_to_intg(const std::string &s) {
  return string_to_intg(s.c_str());
}

intg string_to_intg(const char *s) {
  istringstream iss(s);
  intg d;
  iss >> d;
  if (iss.fail()) eblthrow("\"" << s << "\" is not an intg");
  return d;
}

float string_to_float(const std::string &s) {
  return string_to_float(s.c_str());
}

float string_to_float(const char *s) {
  istringstream iss(s);
  float d;
  iss >> d;
  if (iss.fail()) eblthrow("\"" << s << "\" is not a float");
  return d;
}

double string_to_double(const std::string &s) {
  return string_to_double(s.c_str());
}

double string_to_double(const char *s) {
  istringstream iss(s);
  double d;
  iss >> d;
  if (iss.fail()) eblthrow("\"" << s << "\" is not a double");
  return d;
}

std::list<uint> string_to_uintlist(const std::string &s_) {
  return string_to_uintlist(s_.c_str());
}

std::list<uint> string_to_uintlist(const char *s_) {
  std::list<uint> l;
  std::string s = s_;
  int k = 0;
  while (s.size()) {
    uint j;
    for (j = 0; j < s.size(); ++j)
      if (s[j] == ',')
        break ;
    std::string s0 = s.substr(0, j);
    if (j >= s.size())
      s = "";
    else
      s = s.substr(j + 1, s.size());
    if (!s0.empty()) {
      l.push_back(string_to_uint(s0));
      k++;
    }
  }
  return l;
}

std::vector<int> string_to_intvector(const std::string &s_, char sep) {
  return string_to_intvector(s_.c_str(), sep);
}

std::vector<int> string_to_intvector(const char *s_, char sep) {
  std::vector<int> l;
  std::string s = s_;
  int k = 0;
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
      l.push_back(string_to_int(s0));
      k++;
    }
  }
  return l;
}

std::vector<uint> string_to_uintvector(const std::string &s_, char sep) {
  return string_to_uintvector(s_.c_str(), sep);
}

std::vector<uint> string_to_uintvector(const char *s_, char sep) {
  std::vector<uint> l;
  std::string s = s_;
  int k = 0;
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
      l.push_back(string_to_uint(s0));
      k++;
    }
  }
  return l;
}

std::vector<intg> string_to_intgvector(const char *s_, char sep) {
  std::vector<intg> l;
  std::string s = s_;
  int k = 0;
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
      l.push_back(string_to_intg(s0));
      k++;
    }
  }
  return l;
}

std::vector<float> string_to_floatvector(const char *s_, char sep) {
  std::vector<float> l;
  std::string s = s_;
  int k = 0;
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
      l.push_back(string_to_float(s0));
      k++;
    }
  }
  return l;
}

idxdim string_to_idxdim(const std::string &s_, char sep) {
  return string_to_idxdim(s_.c_str(), sep);
}

idxdim string_to_idxdim(const char *s_, char sep) {
  idxdim d;
  std::string s = s_;
  int k = 0;
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
      d.insert_dim(k, string_to_uint(s0));
      k++;
    }
  }
  return d;
}

fidxdim string_to_fidxdim(const char *s_, char sep) {
  fidxdim d;
  std::string s = s_;
  int k = 0;
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
      d.insert_dim(k, string_to_float(s0));
      k++;
    }
  }
  return d;
}

midxdim string_to_idxdimvector(const char *s_, char vecsep, char dimsep) {
  midxdim v;
  std::list<std::string> l = string_to_stringlist(s_, vecsep);
  std::list<std::string>::iterator i;
  for (i = l.begin(); i != l.end(); ++i)
    v.push_back(string_to_idxdim(i->c_str()));
  return v;
}

std::vector<midxdim> string_to_midxdimvector(const char *s_, char msep, char vecsep, char dimsep) {
  std::vector<midxdim> v;
  std::list<std::string> l = string_to_stringlist(s_, vecsep);
  std::list<std::string>::iterator i;
  for (i = l.begin(); i != l.end(); ++i)
    v.push_back(string_to_idxdimvector(i->c_str(), msep, dimsep));
  return v;
}

mfidxdim string_to_fidxdimvector(const char *s_, char vecsep, char dimsep) {
  mfidxdim v;
  std::list<std::string> l = string_to_stringlist(s_, vecsep);
  std::list<std::string>::iterator i;
  for (i = l.begin(); i != l.end(); ++i)
    v.push_back(string_to_fidxdim(i->c_str()));
  return v;
}

std::list<std::string> string_to_stringlist(const std::string &s_, char sep) {
  return string_to_stringlist(s_.c_str(), sep);
}

std::list<std::string> string_to_stringlist(const char *s_, char sep) {
  std::list<std::string> l;
  std::string s = s_;
  int k = 0;
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
      l.push_back(s0);
      k++;
    }
  }
  return l;
}

std::vector<std::string> string_to_stringvector(const std::string &s_, char sep) {
  return string_to_stringvector(s_.c_str(), sep);
}

std::vector<std::string> string_to_stringvector(const char *s_, char sep) {
  std::vector<std::string> l;
  std::string s = s_;
  int k = 0;
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
      l.push_back(s0);
      k++;
    }
  }
  return l;
}

std::vector<double> string_to_doublevector(const std::string &s_) {
  return string_to_doublevector(s_.c_str());
}

std::vector<double> string_to_doublevector(const char *s_) {
  std::vector<double> l;
  std::string s = s_;
  int k = 0;
  while (s.size()) {
    uint j;
    for (j = 0; j < s.size(); ++j)
      if (s[j] == ',')
        break ;
    std::string s0 = s.substr(0, j);
    if (j >= s.size())
      s = "";
    else
      s = s.substr(j + 1, s.size());
    if (!s0.empty()) {
      l.push_back(string_to_double(s0));
      k++;
    }
  }
  return l;
}

std::string stringlist_to_string(std::list<std::string> &l) {
  std::string s;
  std::list<std::string>::iterator j;
  for (j = l.begin(); j != l.end(); ++j)
    s << *j << " ";
  return s;
}

std::vector<std::string> ubyteidx_to_stringvector(idx<ubyte> &u) {
  if (u.order() != 2)
    eblerror("expected an idx with order 2 but found: " << u);
  std::vector<std::string> v;
  idx_bloop1(uu, u, ubyte) {
    v.push_back((const char *) uu.idx_ptr());
  }
  return v;
}

std::string string_replaceall(const std::string &s, const char *s1, const char *s2) {
  std::string res = s;
  size_t pos = res.find(s1);
  while (pos != std::string::npos) {
    // found an occurence, replace it with s2
    res = res.replace(pos, strlen(s1), s2);
    // look for other occurences
    pos = res.find(s1);
  }
  return res;
}

std::string file_to_string(const std::string &fname) {
  return file_to_string(fname.c_str());
}

std::string file_to_string(const char *fname) {
  std::ifstream in(fname);
  if (!in) eblthrow("error: failed to open " << fname);
  std::string s((std::istreambuf_iterator<char>(in)),
                std::istreambuf_iterator<char>());
  in.close();
  return s;
}

 std::string file_to_string(FILE* fd, long size) { 
   char *buf = (char *)malloc(size);
   size_t read = fread(buf, 1, size, fd);
   if (read != size)
     eblerror("Read error");
   std::stringstream s;
   s << buf;
   free(buf);
   return s.str();  
}


bool string_to_file(const std::string &data, const std::string &fname) {
  return string_to_file(data, fname.c_str());
}

bool string_to_file(const std::string &data, const char *fname) {
  ofstream out;
  out.open(fname, ios::binary);
  if (!out) {
    eblthrow("error: failed to open " << fname);
    return false;
  }
  out.write(data.c_str(), data.size() + 1);
  out.close();
  return true;
}

//////////////////////////////////////////////////////////////////////////////

std::string filename(const char *s_) {
#ifdef __LINUX__
  char c = '/';
#else /* __WINDOWS__ */
  char c = '\\';
#endif
  std::string s = s_;
  size_t pos = s.find_last_of(c);
  return s.substr(pos + 1);
}

std::string system_to_string(const std::string &cmd) {
#ifdef __DEBUG__
  eblprint("system call: " << cmd << ", answer: " << flush);
#endif
  std::string res;
  char buf[256];
#ifdef __WINDOWS__
  FILE* fp = POPEN(cmd.c_str(), "rb");
#else
  FILE* fp = POPEN(cmd.c_str(), "r");
#endif
  while (fgets(buf, 256, fp))
    res << buf;
  if (PCLOSE(fp) != 0) {
    eblwarn("Warning: pclose failed (errno: " << errno
            << ") after calling command: " << cmd << std::endl);
  }
#ifdef __DEBUG__
  eblprint(res << std::endl);
#endif
  return res;
}

} // namespace ebl
