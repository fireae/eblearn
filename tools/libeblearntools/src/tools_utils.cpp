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

#include "tools_utils.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <errno.h>
#include "libidx.h"

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include <boost/exception/all.hpp>
#include <boost/exception/diagnostic_information.hpp>
#endif

using namespace std;

namespace ebl {

// directory utilities /////////////////////////////////////////////////////////

inline bool less_than(const stringpair& b1, const stringpair& b2) {
  if ((b1.first < b2.first) ||
      ((b1.first == b2.first) && (b1.second < b2.second)))
    return true;
  return false;
}

files_list *find_files(const std::string &dir, const char *pattern,
                       files_list *fl_, bool sort, bool recursive,
                       bool randomize) {
  files_list *fl = fl_;
#ifndef __BOOST__
  eblerror("boost not installed, install and recompile");
#else
  if (sort && randomize)
    eblerror("it makes no sense to sort and randomize at the same time");
  boost::regex r(pattern);
  boost::filesystem::path p(dir);
  if (!exists(p))
    return NULL; // return if invalid directory
  // allocate fl if null
  if (!fl)
    fl = new files_list();
  boost::filesystem::directory_iterator end_itr;
  // first process all elements of current directory
  for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
    if (!boost::filesystem::is_directory(itr->status()) &&
        boost::regex_match(itr->path().filename().string(), r)) {
      // found an match, add it to the list
      fl->push_back(pair<std::string,std::string>
		    (itr->path().branch_path().string(),
		     itr->path().filename().string()));
    }
  }
  // then explore subdirectories
  if (recursive) {
    for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
      if (boost::filesystem::is_directory(itr->status()))
        find_files(itr->path().string(), pattern, fl, sort,
                   recursive, false);
    }
  }
  // sort list
  if (sort)
    fl->sort(less_than);
  // randomize list
  if (randomize) {
    // list randomization is very inefficient, so first copy to vector,
    // randomize, then copy back (using vectors directly would be innefficient
    // too for big file lists because we constantly resize the list as we
    // find new files).
    std::vector<stringpair> v(fl->size());
    std::vector<stringpair>::iterator iv = v.begin();
    for (files_list::iterator i = fl->begin(); i != fl->end(); ++i, ++iv)
      *iv = *i;
    delete fl;
    fl = new files_list();
    random_shuffle(v.begin(), v.end());
    for (iv = v.begin(); iv != v.end(); ++iv)
      fl->push_back(*iv);
  }
#endif
  return fl;
}

std::list<std::string>*
find_fullfiles(const std::string &dir, const char *pattern,
	       std::list<std::string> *fl_, bool sorted, bool recursive,
	       bool randomize, bool finddirs, bool fullpattern) {
  std::list<std::string> *fl = fl_;
#ifndef __BOOST__
  eblerror("boost not installed, install and recompile");
#else
  boost::regex r(pattern);
  boost::filesystem::path p(dir);
  if (!boost::filesystem::exists(p))
    return NULL; // return if invalid directory
  // allocate fl if null
  if (!fl)
    fl = new std::list<std::string>();
  boost::filesystem::directory_iterator end_itr;
  // first process all elements of current directory
  for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
    try {
      bool match;
      // apply pattern on full name or leaf only
      if (fullpattern)
	match = regex_match(itr->path().string().c_str(), r);
      else match = regex_match(itr->path().filename().string(), r);
      if ((finddirs || !is_directory(itr->status())) && match) {
        // found an match, add it to the list
        fl->push_back(itr->path().string());
      }
    } catch(boost::exception &e) { eblwarn(boost::diagnostic_information(e));}
  }
  // then explore subdirectories
  if (recursive) {
    for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
      try {
        if (boost::filesystem::is_directory(itr->status()))
          find_fullfiles(itr->path().string(), pattern, fl, sorted,
                         recursive, randomize, finddirs, fullpattern);
      } catch(boost::exception &e) { eblwarn(boost::diagnostic_information(e));}
    }
  }
  // sort list
  if (sorted)
    fl->sort();
  // randomize list
  if (randomize) {
    // list randomization is very inefficient, so first copy to vector,
    // randomize, then copy back (using vectors directly would be innefficient
    // too for big file lists because we constantly resize the list as we
    // find new files).
    std::vector<std::string> v(fl->size());
    std::vector<std::string>::iterator iv = v.begin();
    for (std::list<std::string>::iterator i = fl->begin(); i != fl->end(); ++i, ++iv)
      *iv = *i;
    fl->clear();
    random_shuffle(v.begin(), v.end());
    for (iv = v.begin(); iv != v.end(); ++iv)
      fl->push_back(*iv);
  }
#endif
  return fl;
}

uint count_files(const std::string &dir, const char *pattern) {
  uint total = 0;
#ifndef __BOOST__
  eblerror("boost not installed, install and recompile");
#else
  boost::regex r(pattern);
  boost::filesystem::path p(dir);
  if (!exists(p))
    return 0; // return if invalid directory
  boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
  for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
    if (boost::filesystem::is_directory(itr->status()))
      total += count_files(itr->path().string(), pattern);
    else if (boost::regex_match(itr->path().filename().string(), r)) {
      // found file matching pattern, increment counter
      total++;
    }
  }
#endif
  return total;
}

std::string increment_filename(const char *fullname, const uint stride) {
  std::vector<std::string> path_vector = string_to_stringvector(fullname, '/');
  std::string filename = path_vector[path_vector.size() - 1];
  //split filename into name and extension
  std::string filename_head = noext_name(filename.c_str());
  //since file can contain . apart from the extension, make sure of that case
  std::string filename_str = strip_last_num(filename_head);
  std::string filename_num = return_last_num(filename_head);
  intg filenum = string_to_intg(filename_num);
  filenum = filenum + stride;
  std::string outnum;
  std::stringstream tempstream;
  tempstream << filenum;
  tempstream >> outnum;
  if(outnum.size() < filename_num.size())
    for(uint i=0; i < (1 + filename_num.size() - outnum.size()); ++i)
      outnum = "0" + outnum;
  //now add back the filename
  filename_head = filename_str + outnum;
  filename = filename_head + ext_name(filename.c_str());
  std::string ret = "";
  for(uint i = 0; i < (path_vector.size() - 1); ++i)
    ret = ret + "/" + path_vector[i];
  ret = ret+ "/" + filename;
  return ret;
}

bool tar(const std::string &dir, const std::string &tgtdir) {
#ifdef __BOOST__
  std::string cmd;
  boost::filesystem::path p(dir);
  cmd << "tar cz -C " << dir << "/../ -f " << tgtdir << "/"
      << p.filename().string()
      << ".tgz " << p.filename().string();// << " 2> /dev/null";
  int ret = std::system(cmd.c_str());
  if (ret < 0) {
    eblwarn("tar failed." << std::endl);
    return false;
  }
#endif
  return true;
}

bool tar_pattern(const std::string &dir, const std::string &tgtdir,
                 const std::string &tgtfilename, const char *pattern) {
#ifdef __BOOST__
  // find all files matching pattern
  std::list<std::string> *files = find_fullfiles(dir, pattern);
  if (!files) {
    eblwarn("No files matching pattern \"" << pattern
            << "\" were found." << std::endl);
    return false;
  }
  // tar them
  std::string cmd;
  boost::filesystem::path p(dir);
  cmd << "tar czf " << tgtdir << "/" << tgtfilename << " ";
  for (std::list<std::string>::iterator i = files->begin();
       i != files->end(); ++i) {
    cmd << *i << " ";
  }
  int ret = std::system(cmd.c_str());
  if (ret < 0) {
    eblwarn("tar failed." << std::endl);
    return false;
  }
#endif
  return true;
}

} // namespace ebl
