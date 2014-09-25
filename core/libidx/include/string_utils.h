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

#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <list>
#include <string>
#include <vector>

#include "defines.h"
#include "idx.h"

namespace ebl {

//! Convert a string to a bool. Throws a const char * exception
//! upon failure.
EXPORT bool string_to_bool(const std::string &s);
//! Convert a string to an int. Throws a const char * exception
//! upon failure.
EXPORT int string_to_int(const std::string &s);
//! Convert a string to an int. Throws a const char * exception
//! upon failure.
EXPORT int string_to_int(const char *s);
//! Convert a string to an intg. Throws a const char * exception
//! upon failure.
EXPORT intg string_to_intg(const std::string &s);
//! Convert a string to an intg. Throws a const char * exception
//! upon failure.
EXPORT intg string_to_intg(const char *s);
//! Convert a string to an unsigned int. Throws a const char * exception
//! upon failure.
EXPORT uint string_to_uint(const std::string &s);
//! Convert a string to an unsigned int. Throws a const char * exception
//! upon failure.
EXPORT uint string_to_uint(const char *s);
//! Convert a string to an float. Throws a const char * exception
//! upon failure.
EXPORT float string_to_float(const std::string &s);
//! Convert a string to an float. Throws a const char * exception
//! upon failure.
EXPORT float string_to_float(const char *s);
//! Convert a string to an double. Throws a const char * exception
//! upon failure.
EXPORT double string_to_double(const std::string &s);
//! Convert a string to an double. Throws a const char * exception
//! upon failure.
EXPORT double string_to_double(const char *s);

//! Convert a string containing a list of uint separated by commas, e.g.
//! "1,2,3,4" into a list of uints.
EXPORT std::list<uint> string_to_uintlist(const std::string &s);
//! Convert a string containing a list of uint separated by commas, e.g.
//! "1,2,3,4" into a list of uints.
EXPORT std::list<uint> string_to_uintlist(const char *s);
//! Convert a string containing a list of int separated by commas, e.g.
//! "1,2,3,4" into a vector of ints.
//! \param sep Specifies the separating character, comma by default.
EXPORT std::vector<int> string_to_intvector(const std::string &s,
                                            char sep = ',');
//! Convert a string containing a list of int separated by commas, e.g.
//! "1,2,3,4" into a vector of ints.
//! \param sep Specifies the separating character, comma by default.
EXPORT std::vector<int> string_to_intvector(const char *s, char sep = ',');
//! Convert a string containing a list of uint separated by commas, e.g.
//! "1,2,3,4" into a vector of uints.
//! \param sep Specifies the separating character, comma by default.
EXPORT std::vector<uint> string_to_uintvector(const std::string &s,
                                              char sep = ',');
//! Convert a string containing a list of uint separated by commas, e.g.
//! "1,2,3,4" into a vector of uints.
//! \param sep Specifies the separating character, comma by default.
EXPORT std::vector<uint> string_to_uintvector(const char *s, char sep = ',');
//! Convert a string containing a list of intg separated by commas, e.g.
//! "1,2,3,4" into a vector of uints.
//! \param sep Specifies the separating character, comma by default.
EXPORT std::vector<intg> string_to_intgvector(const char *s, char sep = ',');
//! Convert a string containing a list of intg separated by commas, e.g.
//! "1,2,3,4" into a vector of floats.
//! \param sep Specifies the separating character, comma by default.
EXPORT std::vector<float> string_to_floatvector(const char *s, char sep = ',');

//! Convert a string containing a list of numbers separated by 'sep', e.g.
//! "1x2x3x4" into an idx<T>.
//! \param sep Specifies the separating character, 'x' by default.
template <typename T>
EXPORT idx<T> string_to_idx(const char *s, char sep = ',');
//! Convert the number in s into a value of type T and return it.
template <typename T>
EXPORT T string_to_number(const char *s);
//! Convert a string containing a list of uint separated by 'sep', e.g.
//! "1x2x3x4" into an idxdim.
//! \param sep Specifies the separating character, 'x' by default.
EXPORT idxdim string_to_idxdim(const char *s, char sep = 'x');
//! Convert a string containing a list of float separated by 'sep', e.g.
//! "1.5x2x3x4.5" into an fidxdim.
//! \param sep Specifies the separating character, 'x' by default.
EXPORT fidxdim string_to_fidxdim(const char *s, char sep = 'x');
//! Convert a string containing a list of uint separated by 'sep', e.g.
//! "1x2x3x4" into an idxdim.
//! \param sep Specifies the separating character, 'x' by default.
EXPORT idxdim string_to_idxdim(const std::string &s, char sep = 'x');
//! Convert a string containing a list of dimension strings separated by
//! commas, e.g. "10x10,20x20" into a vector of idxdim.
//! \param vecsep Specifies the vector separating character, ',' by default.
//! \param dimsep Specifies the dim separating character, 'x' by default.
EXPORT midxdim string_to_idxdimvector(const char *s, char vecsep = ',',
                                      char dimsep = 'x');
//! Convert a string containing a list of dimension strings separated by
//! commas, e.g. "10x10,20x20" into a vector of idxdim.
//! \param vecsep Specifies the vector separating character, ',' by default.
//! \param dimsep Specifies the dim separating character, 'x' by default.
EXPORT std::vector<midxdim> string_to_midxdimvector(const char *s,
                                                    char msep = ',',
                                                    char vecsep = ';',
                                                    char dimsep = 'x');
//! Convert a string containing a list of float dimension strings separated by
//! commas, e.g. "1.5x1.5,2.5x2.5" into a vector of fidxdim.
//! \param vecsep Specifies the vector separating character, ',' by default.
//! \param dimsep Specifies the dim separating character, 'x' by default.
EXPORT mfidxdim string_to_fidxdimvector(const char *s, char vecsep = ',',
                                        char dimsep = 'x');

//! Convert a string containing a list of strings separated by commas, e.g.
//! "errors,2,toto,4" into a list of strings.
//! \param sep Specifies the separating character, ',' by default.
EXPORT std::list<std::string> string_to_stringlist(const std::string &s,
                                                   char sep = ',');
//! Convert a string containing a list of strings separated by commas, e.g.
//! "errors,2,toto,4" into a list of strings.
//! \param sep Specifies the separating character, ',' by default.
EXPORT std::list<std::string> string_to_stringlist(const char *s,
                                                   char sep = ',');
//! Convert a string containing a list of strings separated by commas, e.g.
//! "errors,2,toto,4" into a vector of strings.
//! \param sep Specifies the separating character, ',' by default.
EXPORT std::vector<std::string> string_to_stringvector(const std::string &s,
                                                       char sep = ',');
//! Convert a string containing a list of strings separated by commas, e.g.
//! "errors,2,toto,4" into a vector of strings.
//! \param sep Specifies the separating character, ',' by default.
EXPORT std::vector<std::string> string_to_stringvector(const char *s,
                                                       char sep = ',');
//! Convert a string containing a list of double separated by commas, e.g.
//! "1,2,3.0,4.0" into a vector of doubles.
EXPORT std::vector<double> string_to_doublevector(const std::string &s);
//! Convert a string containing a list of double separated by commas, e.g.
//! "1,2,3.0,4.0" into a vector of doubles.
EXPORT std::vector<double> string_to_doublevector(const char *s);

//! Convert a map to a string representation.
template <typename T1, typename T2>
EXPORT std::string map_to_string(std::map<T1,T2> &m);
//! Convert a map to a string representation, printing a new line between
//! each variables/value pair.
template <typename T1, typename T2>
EXPORT std::string map_to_string2(std::map<T1,T2> &m);
//! Convert a string list to a string representation.
EXPORT std::string stringlist_to_string(std::list<std::string> &l);
//! Convert an idx of string to a vector of strings.
EXPORT std::vector<std::string> ubyteidx_to_stringvector(idx<ubyte> &u);
//! Replace all occurences of 's1' by 's2' in s and return the result.
EXPORT std::string string_replaceall(const std::string &s, const char *s1,
                                     const char *s2);

//! Reads file 'fname' into a string an returns it.
EXPORT std::string file_to_string(const std::string &fname);
//! Reads file 'fname' into a string an returns it.
EXPORT std::string file_to_string(const char *fname);
//! Reads a C file handle 'fd' with size 'size' into a string an returns it.
EXPORT std::string file_to_string(FILE *fd, long size);
//! Save string content as binary file 'fname'.
EXPORT bool string_to_file(const std::string &data, const std::string &fname);
//! Save string content as binary file 'fname'.
EXPORT bool string_to_file(const std::string &data, const char *fname);

//! Resize and fill vector v with values in l.
template <typename T>
void list_to_vector(std::list<T> &l, std::vector<T> &v);

//! Return the name of a file stripped from its root.
//! This is just looking for the first
//! directory separator from the right, this is not doing any system calls
//! to find the directory.
EXPORT std::string filename(const char *s);

//! Execute command 'cmd' and return the resulting string.
EXPORT std::string system_to_string(const std::string &cmd);

} // end namespace ebl

#include "string_utils.hpp"

#endif /* STRING_UTILS_H_ */
