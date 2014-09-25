/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
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

#ifndef PASCALCLEAR_DATASET_HPP_
#define PASCALCLEAR_DATASET_HPP_

#include <algorithm>

#include "xml_utils.h"

#ifdef __BOOST__
#ifndef BOOST_FILESYSTEM_VERSION
#define BOOST_FILESYSTEM_VERSION 3
#endif
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#endif

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <class Tdata>
  pascalclear_dataset<Tdata>::pascalclear_dataset(const char *name_,
						  const char *inroot_,
						  const char *outdir_,
						  const char *annotations)
    : pascalbg_dataset<Tdata>(name_, inroot_, outdir_, 0, false, false, false,
			      annotations) {
  }

  template <class Tdata>
  pascalclear_dataset<Tdata>::~pascalclear_dataset() {
  }

  ////////////////////////////////////////////////////////////////
  // data extraction

  template <class Tdata>
  bool pascalclear_dataset<Tdata>::extract() {
#ifdef __BOOST__
#ifdef __XML__
    std::cout << "Clearing objects from original pascal images..." << std::endl;
    mkdir_full(outdir);
    // adding data to dataset using all xml files in annroot
    boost::regex eExt(XML_PATTERN);
    boost::cmatch what;
    boost::filesystem::path p(annroot);
    if (!exists(p)) {
      std::cerr << "path " << annroot << " does not exist." << std::endl;
      return false;
    }
    boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
    for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
      if (!boost::filesystem::is_directory(itr->status()) &&
	  boost::regex_match(itr->path().filename().c_str(), what, eExt)) {
	this->process_xml(itr->path().string());
	if (this->full()) //max_data_set && (data_cnt >= max_data))
	  break ;
      }
    }
    std::cout << "Cleared and saved " << data_cnt;
    std::cout << " images." << std::endl;
#endif /* __XML__ */
#endif /* __BOOSt__ */
    return true;
  }

#ifdef __BOOST__ // disable some derived methods if BOOST not available
#ifdef __XML__ // disable some derived methods if XML not available

  ////////////////////////////////////////////////////////////////
  // process object's image

  template <class Tdata>
  void pascalclear_dataset<Tdata>::
  process_image(idx<ubyte> &img, std::vector<rect<int> >& bboxes,
		const std::string &image_filename) {
    std::vector<rect<int> >::iterator ibb;
    std::ostringstream fname;

#ifdef __GUI__
    uint h = 63, w = 0;
    if (display_extraction) {
	disable_window_updates();
	clear_window();
	// draw original image
	draw_matrix(img, h, w, 1.0, 1.0, (ubyte) 0, (ubyte) 255);
	h += img.dim(0);
    }
#endif
    // clear each bbox
    for (ibb = bboxes.begin(); ibb != bboxes.end(); ++ibb) {
      idx<ubyte> bb = img.narrow(0, ibb->height, ibb->h0);
      bb = bb.narrow(1, ibb->width, ibb->w0);
      idx_clear(bb);
    }
#ifdef __GUI__
    if (display_extraction) {
      draw_matrix(img, h, w, 1.0, 1.0, (ubyte) 0, (ubyte) 255);
      enable_window_updates();
      if (sleep_display)
	millisleep((long) sleep_delay);
    }
#endif
    fname.str("");
    fname << outdir << "/" << image_filename << "_" << data_cnt << ".mat";
    save_matrix(img, fname.str());
    std::cout << data_cnt++ << ": saved " << fname.str() << std::endl;
  }

#endif /* __XML__ */
#endif /* __BOOST__ */

} // end namespace ebl

#endif /* PASCALCLEAR_DATASET_HPP_ */
