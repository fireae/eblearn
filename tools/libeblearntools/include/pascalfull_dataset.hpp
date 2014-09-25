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

#ifndef PASCALFULL_DATASET_HPP_
#define PASCALFULL_DATASET_HPP_

#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>

#include "xml_utils.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <class Tdata>
  pascalfull_dataset<Tdata>::pascalfull_dataset(const char *name_,
						const char *inroot_,
						const char *outdir_,
						const char *annotations)
    : pascal_dataset<Tdata>(name_, inroot_, false, false, false, annotations) {
    outdir = outdir_;
    data_cnt = 0;
    this->allocated = true; // fool extract method
 }

  template <class Tdata>
  pascalfull_dataset<Tdata>::~pascalfull_dataset() {
  }

#ifdef __XML__ // disable some derived methods if XML not available

  ////////////////////////////////////////////////////////////////
  // process xml

  template <class Tdata>
  bool pascalfull_dataset<Tdata>::process_xml(const std::string &xmlfile) {
    std::string image_filename;
    std::string image_fullname;
    std::string obj_classname;

    // parse xml file
    try {
      DomParser parser;
      //    parser.set_validate();
      parser.parse_file(xmlfile);
      if (parser) {
	// initialize root node and list
	const Node* pNode = parser.get_document()->get_root_node();
	Node::NodeList list = pNode->get_children();
	// get image filename
	for(Node::NodeList::iterator iter = list.begin();
	    iter != list.end(); ++iter) {
	  if (!strcmp((*iter)->get_name().c_str(), "filename")) {
	    xml_get_string(*iter, image_filename);
	    iter = list.end(); iter--; // stop loop
	  }
	}
	image_fullname = imgroot;
	image_fullname += image_filename;
	// parse all objects in image
	for(Node::NodeList::iterator iter = list.begin();
	    iter != list.end(); ++iter) {
	  if (!strcmp((*iter)->get_name().c_str(), "object")) {
	    Node::NodeList olist = (*iter)->get_children();
	    for(Node::NodeList::iterator oiter = olist.begin();
		oiter != olist.end(); ++oiter) {
	      if (!strcmp((*oiter)->get_name().c_str(), "name")) {
		xml_get_string(*oiter, obj_classname);
		// if object's name matches an excluded class, stop this xml
		if (find(exclude.begin(), exclude.end(),
			 obj_classname) != exclude.end())
		  return false;
	      }
	    }
	  }
	}
      }
    } catch (const std::exception& ex) {
      std::cerr << "error: Xml exception caught: " << ex.what() << std::endl;
      return false;
    } catch (const char *err) {
      std::cerr << "error: " << err << std::endl;
      return false;
    }
    // copy image into output directory
    std::ostringstream cmd;
    std::ostringstream tgt;
    tgt << outdir << "/" << image_filename;
    cmd << "cp " << image_fullname << " " << tgt.str();
    if (std::system(cmd.str().c_str()))
      std::cerr << "warning: failed to execute: " << cmd.str() << std::endl;
    else {
      std::cout << data_cnt << ": copied " << tgt.str() << std::endl;
      data_cnt++;
    }
    return true;
  }

#endif /* __XML__ */

} // end namespace ebl

#endif /* PASCALFULL_DATASET_HPP_ */
