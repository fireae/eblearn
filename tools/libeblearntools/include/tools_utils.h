/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
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

#ifndef TOOLS_UTILS_H_
#define TOOLS_UTILS_H_

#include <list>
#include <string>
#include <vector>
#include <map>

#include "defines.h"
#include "idx.h"

namespace ebl {

// directory utilities /////////////////////////////////////////////////////////

typedef std::pair<std::string, std::string> stringpair;
typedef std::list<stringpair> files_list;

inline bool less_than(const stringpair& b1, const stringpair& b2);

//! Returns a list of pairs of root directory and filename of all files
//! found recursively in directory 'dir'. The files are found using
//! the IMAGE_PATTERN regular expression by default.
//! If the directory does not exists, it returns NULL.
//! The user is responsible for deleting the returned list.
//! \param fl A file list where new found files will be apprended if not null.
//!           If null, a new list is allocated. This is used by the recursion.
//! \param pattern The regular expression describing the file name pattern.
//!           The default pattern matches images extensions.
//! \param randomize If true, randomize the returned list.
EXPORT files_list *find_files(const std::string &dir,
                              const char *pattern = IMAGE_PATTERN,
                              files_list *fl = NULL, bool sort = true,
                              bool recursive = true, bool randomize = false);

//! Returns a list of string of full paths to files recursively found in
//! directory dir and matching the pattern. The files are found using
//! the IMAGE_PATTERN regular expression by default.
//! If the directory does not exists, it returns NULL.
//! The user is responsible for deleting the returned list.
//! \param fl A file list where new found files will be appended if not null.
//!           If null, a new list is allocated. This is used by the recursion.
//! \param pattern The regular expression describing the file name pattern,
//!           e.g. ".*[.](mat|MAT)".
//!           The default pattern matches images extensions.
//! \param randomize If true, randomize the returned list.
//! \param finddir If true, include directories in results.
//! \param fullpattern If true, match pattern against entire path instead
//!   filename only.
EXPORT std::list<std::string>*
  find_fullfiles(const std::string &dir,
		 const char *pattern = IMAGE_PATTERN,
		 std::list<std::string> *fl = NULL,
		 bool sort = true, bool recursive = true,
		 bool randomize = false, bool finddir = false,
		 bool fullpattern = false);

//! Counts recursively the number of files matching the pattern (default is
//! an image extension pattern) in directory 'dir'.
//! \param pattern The regular expression describing the file name pattern.
//!           The default pattern matches images extensions.
EXPORT uint count_files(const std::string &dir,
                        const char *pattern = IMAGE_PATTERN);

//! Takes a filename and tries to return the next filename in the sequence
//! by assuming that the file is in "chars"+"number"."ext" format.
//! \param fullname The input filename including the full path
//! \param stride The increment value
EXPORT std::string increment_filename(const char *fullname, uint stride);

// system functions ////////////////////////////////////////////////////////////

//! Tar directory dir into a .tgz archive, using directory's rightmost name,
//! into target directory tgtdir.
//! Return false upon failure, true otherwise.
EXPORT bool tar(const std::string &dir, const std::string &tgtdir);
//! Tar all files found recursively in directory dir into a compressed tar
//! archive "tgtdir/tgtfilename", using matching pattern 'pattern'.
//! Return false upon failure, true otherwise.
EXPORT bool tar_pattern(const std::string &dir, const std::string &tgtdir,
                        const std::string &tgtfilename, const char *pattern);
 
} // end namespace ebl

#include "tools_utils.hpp"

#endif /* TOOLS_UTILS_ */
