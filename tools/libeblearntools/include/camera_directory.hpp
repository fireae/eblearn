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

#ifndef CAMERA_DIRECTORY_HPP_
#define CAMERA_DIRECTORY_HPP_

#include <sstream>

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera_directory<Tdata>::camera_directory(const char *dir,
					    int height_, int width_,
					    bool randomize_, uint npasses_,
					    std::ostream &o, std::ostream &e,
					    const char *pattern,
					    const std::list<std::string> *files)
    : camera<Tdata>(height_, width_, o, e), indir(dir),
      randomize(randomize_), npasses(npasses_), file_pattern(pattern) {
    if (npasses == 0)
      eblerror("number of passes must be >= 1");
    out << "Initializing directory camera from: " << dir << std::endl;
    if (files && files->size() > 0) { // file names specified by hand
      // build list and check each file exists
      fl = new files_list;
      for (std::list<std::string>::const_iterator i = files->begin();
           i != files->end(); ++i) {
	std::string fullname;
	fullname << dir << "/" << *i;
	if (file_exists(fullname)) {
	  fl->push_back(stringpair(std::string(dir), *i));
	} else
	  e << "warning: file not found: " << fullname << std::endl;
      }
      if (fl->size() == 0) {
	err << "warning: No images in image list were found: "
	     << *files << std::endl;
	err << "Looking for other images..." << std::endl;
	if (!read_directory(dir)) eblerror( "No images found in " << dir);
      }
    } else { // search all files matching pattern
      if (!read_directory(dir)) eblerror( "No images found in " << dir);
    }
    std::cout << "Found " << fl->size() << " images in " << indir << std::endl;
    if (randomize)
      out << "Image list is randomized." << std::endl;
    if (npasses > 1)
      out << "Image list will be used " << npasses << " times." << std::endl;
    flsize = fl->size() * npasses;
    fli = fl->begin(); // initialize iterator to beginning
  }

  template <typename Tdata>
  camera_directory<Tdata>::camera_directory(int height_, int width_,
					    bool randomize_, uint npasses_,
					    std::ostream &o, std::ostream &e,
					    const char *pattern)
    : camera<Tdata>(height_, width_, o, e),
      randomize(randomize_), npasses(npasses_), file_pattern(pattern) {
    if (npasses == 0)
      eblerror("number of passes must be >= 1");
  }

  template <typename Tdata>
  bool camera_directory<Tdata>::read_directory(const char *dir) {
    out << "Image search pattern: " << file_pattern << " in "
        << dir << std::endl;
    std::string directory = dir;
    if (directory[directory.length() - 1] != '/')
      directory += '/';
    indir = directory;
    // // first count number of images, to allocate list and speed up
    // uint n = count_files(directory, file_pattern);
    // if (fl) delete fl;
    // fl = new files_list(n);
    // get all file names
    if (fl) delete fl;
    fl = find_files(directory, file_pattern, NULL,
		    randomize ? false : true, true, randomize);
    if (!fl) {
      err << "invalid directory: " << dir << std::endl;
      eblerror("invalid directory");
      return false;
    }
    return true;
  }

  template <typename Tdata>
  camera_directory<Tdata>::~camera_directory() {
    if (fl)
      delete fl;
  }

  ////////////////////////////////////////////////////////////////
  // frame grabbing

  template <typename Tdata>
  void camera_directory<Tdata>::next() {
    if (empty())
      eblerror("cannot grab images on empty list");
    fdir = fli->first; // directory
    fname = fli->second; // file name
    frame_name_ = fname;
    if (fdir.size() > indir.size()) {
      subdir = fdir.substr(indir.size());
      frame_name_ = "";
      frame_name_ << subdir << "/" << fname;
    }
    //std::cout << "fname: " << fname << " fdir: " << fdir << " subdir: "
    // << subdir << std::endl;
    //    ostringstream fn("");
    if (fdir[fdir.length() - 1] != '/')
      fdir += "/";
//     if (strcmp(fdir.c_str(), ""))
//       fn << fdir;
//     fn << fname; // << "_" << frame_id_;
//     frame_name_ = fn.str();
//     if (strcmp(fdir.c_str(), "")) {
//       size_t npos = frame_name_.length() - fdir.length();
//       frame_name_ = frame_name_.substr(fdir.length(), npos);
//     }
//     for (size_t i = 0; i < frame_name_.length(); ++i)
//       if (frame_name_[i] == '/')
// 	frame_name_[i] = '_';
    fli++; // move to next element
    frame_id_++;
  }

  template <typename Tdata>
  void camera_directory<Tdata>::previous() {
    if (empty())
      eblerror("cannot grab images on empty list");
    // move to previous element
    fli--;
    frame_id_--;
    // set names
    fdir = fli->first; // directory
    fname = fli->second; // file name
    frame_name_ = fname;
    if (fdir.size() > indir.size()) {
      subdir = fdir.substr(indir.size());
      frame_name_ = "";
      frame_name_ << subdir << "/" << fname;
    }
    if (fdir[fdir.length() - 1] != '/')
      fdir += "/";
  }

  template <typename Tdata>
  std::string camera_directory<Tdata>::grab_filename() {
    next();
    out << frame_id_ << "/" << flsize << ": grabbing ";
    out << fdir << fname << std::endl;
    oss.str(""); oss << fdir << "/" << fname;
    return oss.str();
  }

  template <typename Tdata>
  idx<Tdata> camera_directory<Tdata>::grab() {
    next();
    out << frame_id_ << "/" << flsize << ": grabbing ";
    out << fdir << fname << std::endl;
    oss.str(""); oss << fdir << "/" << fname;
    try {
      frame = load_image<Tdata>(oss.str());
    } catch (const std::string &e) {
      err << "failed to load image " << oss.str();
#ifndef __NOEXCEPTIONS__
      err << " (" << e << "). ";
#endif
      err << "Trying next image..." << std::endl;
      frame_id_++;
      return grab();
    }
    return this->postprocess();
  }

  template <typename Tdata>
  void camera_directory<Tdata>::skip(uint n) {
    if (n == 0) return ;
    uint i;
    for (i = 0; i < n; ++i) {
      if (empty()) {
	i--;
	break ;
      }
      fli++;
      frame_id_++;
    }
    std::cout << "Skipped " << i << " frames." << std::endl;
  }

  template <typename Tdata>
  bool camera_directory<Tdata>::empty() {
    if (!fl)
      eblerror("directory not initialized");
    if (fli == fl->end()) {
      if (npasses > 0) {
	npasses--;
	if (npasses > 0)
	  fli = fl->begin(); // reset to begining.
      }
      if (npasses == 0) // we did all passes, stop.
	return true;
    }
    return false;
  }

  template <typename Tdata>
  std::string camera_directory<Tdata>::frame_name() {
    return frame_name_;
  }

  template <typename Tdata>
  std::string camera_directory<Tdata>::frame_fullname() {
    std::string full;
    full << fdir << fname;
    return full;
  }

  template <typename Tdata>
  std::string camera_directory<Tdata>::get_subdir() {
    return subdir;
  }

  template <typename Tdata>
  int camera_directory<Tdata>::remaining() {
    return (int) (flsize - frame_id_);
  }

  template <typename Tdata>
  int camera_directory<Tdata>::size() {
    return (int) flsize;
  }

} // end namespace ebl

#endif /* CAMERA_DIRECTORY_HPP_ */
