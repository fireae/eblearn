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

#include "pascal_xml.h"
#include "tools_utils.h"

#ifdef __BOOST__
#define BOOST_FILESYSTEM_VERSION 3
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#endif

using namespace std;

namespace ebl {

  pascal_xml::pascal_xml() {
  }

  pascal_xml::~pascal_xml() {
  }

  // bboxes extraction /////////////////////////////////////////////////////////

  bboxes pascal_xml::get_bboxes(const std::string &xml_fname,
				vector<std::string>& labels) {
    std::string imgroot = dirname(xml_fname.c_str());
    std::string fname, fullname, folder;
    int height, width, depth;
    vector<object*> objs;
    bboxes bb;
    // get objects
    if (!get_properties(imgroot, xml_fname, fname, fullname, folder,
		       height, width, depth, objs, NULL, false))
      eblthrow("failed to load xml from " << xml_fname);
    // convert to bb
    for (uint i = 0; i < objs.size(); ++i) {
      object &o = *(objs[i]);
      bbox b(o.h0, o.w0, o.height, o.width);
      for (uint l = 0; l < labels.size(); ++l)
	if (!strcmp(labels[l].c_str(), o.name.c_str()))
	  b.class_id = l;
      bb.push_back_new(b);
    }
    // delete all objects
    for (uint i = 0; i < objs.size(); ++i) delete objs[i];
    objs.clear();
    return bb;
  }

  bboxes pascal_xml::get_filtered_bboxes
  (const std::string &xml_fname, float minvisibility, float min_aspect_ratio,
   float max_aspect_ratio, idxdim &mindims, idxdim &minborders,
   vector<std::string> &included, bboxes &filtered, vector<std::string> *labels) {
    std::string imgroot = dirname(xml_fname.c_str());
    std::string fname, fullname, folder;
    int height = -1, width = -1, depth = -1;
    vector<object*> objs;
    bboxes bb;
    // get objects
    if (!get_properties(imgroot, xml_fname, fname, fullname, folder,
		       height, width, depth, objs, NULL, false))
      eblerror("failed to load xml from " << xml_fname);
    // filter objects
    for (uint i = 0; i < objs.size(); ++i) {
      object &o = *(objs[i]);
      bool remove = false;
      // check that minimum visibility ratio is respected
      if (minvisibility > 0.0 && o.visible &&
	  (o.match(*o.visible) < minvisibility)) remove = true;
      // check that minimum/maximum aspect ratio respected
      if (o.height == 0) remove = true;
      else {
	float ar = o.width / (float) o.height;
	if (min_aspect_ratio >= 0.0 && ar < min_aspect_ratio) remove = true;
	if (max_aspect_ratio >= 0.0 && ar > max_aspect_ratio) remove = true;
	// check that object is larger than minimum size
	if (o.height < mindims.dim(0) || o.width < mindims.dim(1)) remove =true;
	// check that bbox is not below image border distances
	if (!minborders.empty()) {
	  if (height < 0 || width < 0) eblerror("image sizes not found in xml");
	  int hmax = height - minborders.dim(0);
	  int wmax = width - minborders.dim(1);
	  if (o.h0 < (int) minborders.dim(0) || o.w0 < minborders.dim(1)
	      || o.h0 + o.height > hmax || o.w0 + o.width > wmax) remove = true;
	}
	// check that class is included
	if (included.size() > 0) {
	  bool found = false;
	  for (uint j = 0; j < included.size(); ++j)
	    if (o.name.compare(included[j]) == 0) {
	      found = true;
	      break ;
	    }
	  if (!found) remove = true;
	}
      }
      // remove
      if (remove && objs[i] != NULL) {
	// add object to filtered vector
	object &o = *(objs[i]);
	bbox b(o.h0, o.w0, o.height, o.width);
	if (labels) {
	  for (uint l = 0; l < labels->size(); ++l) {
	    if (!strcmp((*labels)[l].c_str(), o.name.c_str()))
	      b.class_id = l;
	  }
	} else b.class_id = -1;
	filtered.push_back_new(b);
	// then remove it from current list
	delete objs[i];
	objs[i] = NULL;
      }
    }
    // convert to bb
    for (uint i = 0; i < objs.size(); ++i) {
      if (objs[i] != NULL) {
	object &o = *(objs[i]);
	bbox b(o.h0, o.w0, o.height, o.width);
	if (labels) {
	  for (uint l = 0; l < labels->size(); ++l) {
	    if (!strcmp((*labels)[l].c_str(), o.name.c_str()))
	      b.class_id = l;
	  }
	} else b.class_id = -1;
	bb.push_back_new(b);
      }
    }
    // delete all objects
    for (uint i = 0; i < objs.size(); ++i) if (objs[i]) delete objs[i];
    objs.clear();
    return bb;
  }

  // xml extraction ////////////////////////////////////////////////////////////

  bool pascal_xml::get_properties(const std::string &imgroot,
				  const std::string &xmlfile, std::string &image_filename,
				  std::string &image_fullname, std::string &folder,
				  int &height, int &width, int &depth,
				  vector<object*> &objs, rect<int> **cropr,
				  bool ignore) {
#ifdef __XML__ // disable some derived methods if XML not available
    int cxmin, cymin, cxmax, cymax; // crop bbox
    // parse xml file
    try {
      DomParser parser;
      //    parser.set_validate();
      parser.parse_file(xmlfile);
      if (parser) {
	// initialize root node and list
	const Node* pNode = parser.get_document()->get_root_node();
	Node::NodeList list = pNode->get_children();
	// get image filename and folder
	for(Node::NodeList::iterator iter = list.begin();
	    iter != list.end(); ++iter) {
	  if (!strcmp((*iter)->get_name().c_str(), "filename")) {
	    xml_get_string(*iter, image_filename);
	  } else if (!strcmp((*iter)->get_name().c_str(), "folder")) {
	    xml_get_string(*iter, folder);
	  } else if (!strcmp((*iter)->get_name().c_str(), "size")) {
	    Node::NodeList blist = (*iter)->get_children();
	    for(Node::NodeList::iterator biter = blist.begin();
		biter != blist.end(); ++biter) {
	      // get sizes
	      if (!strcmp((*biter)->get_name().c_str(), "height"))
		height = xml_get_int(*biter);
	      if (!strcmp((*biter)->get_name().c_str(), "width"))
		width = xml_get_int(*biter);
	      if (!strcmp((*biter)->get_name().c_str(), "depth"))
		depth = xml_get_int(*biter);
	    }
	  } else if (!strcmp((*iter)->get_name().c_str(), "crop")) {
	    Node::NodeList blist = (*iter)->get_children();
	    for(Node::NodeList::iterator biter = blist.begin();
		biter != blist.end(); ++biter) {
	      // save xmin, ymin, xmax and ymax
	      if (!strcmp((*biter)->get_name().c_str(), "xmin"))
		cymin = xml_get_int(*biter);
	      else if (!strcmp((*biter)->get_name().c_str(), "ymin"))
		cxmin = xml_get_int(*biter);
	      else if (!strcmp((*biter)->get_name().c_str(), "xmax"))
		cymax = xml_get_int(*biter);
	      else if (!strcmp((*biter)->get_name().c_str(), "ymax"))
		cxmax = xml_get_int(*biter);
	    }
	    if (cropr)
	      *cropr = new rect<int>(cymin, cxmin, cymax - cymin, cxmax -cxmin);
	  }
	}
	image_fullname = imgroot;
	if (!folder.empty())
	  image_fullname << "/" << folder << "/";
	image_fullname << image_filename;
	get_objects(list, objs, cropr ? *cropr : NULL, ignore);
      }
    } catch (const std::exception& ex) {
      cerr << "error: Xml exception caught: " << ex.what() << endl;
      return false;
    } catch (const char *err) {
      cerr << "error: " << err << endl;
      return false;
    }
    return true;
#else
    eblerror("libxml not found, recompile");
    return false;
#endif
  }

#ifdef __XML__
  void pascal_xml::get_objects(Node::NodeList &list, vector<object*> &objs,
			       rect<int> *cropr, bool ignore) {
    uint n = 0;
    // parse all objects in image
    for(Node::NodeList::iterator iobjs = list.begin();
	iobjs != list.end(); ++iobjs) {
      if (!strcmp((*iobjs)->get_name().c_str(), "object")) {
	// add a new object
	objs.push_back(new object(n++));
	object &o = *(objs.back());
	o.ignored = ignore;
	// get object attributes
	uint ipart = 0;
	Node::NodeList list = (*iobjs)->get_children();
	for(Node::NodeList::iterator iter = list.begin();
	    iter != list.end(); ++iter) {
	  // parse bounding box
	  if (!strcmp((*iter)->get_name().c_str(), "bndbox")) {
	    Node::NodeList blist = (*iter)->get_children();
	    int xmin = -1, ymin = -1, xmax = -1, ymax = -1;
	    for(Node::NodeList::iterator biter = blist.begin();
		biter != blist.end(); ++biter) {
	      // save xmin, ymin, xmax and ymax
	      if (!strcmp((*biter)->get_name().c_str(), "xmin"))
		xmin = xml_get_int(*biter);
	      else if (!strcmp((*biter)->get_name().c_str(), "ymin"))
		ymin = xml_get_int(*biter);
	      else if (!strcmp((*biter)->get_name().c_str(), "xmax"))
		xmax = xml_get_int(*biter);
	      else if (!strcmp((*biter)->get_name().c_str(), "ymax"))
		ymax = xml_get_int(*biter);
	    }
	    // set bbox
	    o.set_rect(xmin, ymin, xmax, ymax);
	    // adjusting coordinates if cropping image
	    if (cropr) {
	      o.h0 -= cropr->h0;
	      o.w0 -= cropr->w0;
	    }
	  }
	  // parse visible bounding box
	  else if (!strcmp((*iter)->get_name().c_str(), "visible")) {
	    Node::NodeList blist = (*iter)->get_children();
	    int vxmin = -1, vymin = -1, vxmax = -1, vymax = -1;
	    for(Node::NodeList::iterator biter = blist.begin();
		biter != blist.end(); ++biter) {
	      // save xmin, ymin, xmax and ymax
	      if (!strcmp((*biter)->get_name().c_str(), "xmin"))
		vxmin = xml_get_int(*biter);
	      else if (!strcmp((*biter)->get_name().c_str(), "ymin"))
		vymin = xml_get_int(*biter);
	      else if (!strcmp((*biter)->get_name().c_str(), "xmax"))
		vxmax = xml_get_int(*biter);
	      else if (!strcmp((*biter)->get_name().c_str(), "ymax"))
		vymax = xml_get_int(*biter);
	    }
	    o.set_visible(vxmin, vymin, vxmax, vymax);
	    // adjusting coordinates if cropping image
	    if (cropr) {
	      o.visible->h0 -= cropr->h0;
	      o.visible->w0 -= cropr->w0;
	    }
	  }
	  // parse centroid
	  else if (!strcmp((*iter)->get_name().c_str(), "center")) {
	    try {
	      Node::NodeList blist = (*iter)->get_children();
	      int x = -1, y = -1;
	      for(Node::NodeList::iterator biter = blist.begin();
		  biter != blist.end(); ++biter) {
		if (!strcmp((*biter)->get_name().c_str(), "x"))
		  x = xml_get_uint(*biter);
		else if (!strcmp((*biter)->get_name().c_str(), "y"))
		  y = xml_get_uint(*biter);
	      }
	      o.set_centroid(x, y);
	      // adjusting coordinates if cropping image
	      if (cropr) {
		o.centroid->first -= cropr->h0;
		o.centroid->second -= cropr->w0;
	      }
	    } catch(ebl::eblexception &e) {
	      cerr << "warning: no centroid found." << endl; }
	  } // else get object class name
	  else if (!strcmp((*iter)->get_name().c_str(), "name"))
	    xml_get_string(*iter, o.name);
	  else if (!strcmp((*iter)->get_name().c_str(), "difficult"))
	    o.difficult = xml_get_uint(*iter);
	  else if (!strcmp((*iter)->get_name().c_str(), "truncated"))
	    o.truncated = xml_get_uint(*iter);
	  else if (!strcmp((*iter)->get_name().c_str(), "occluded"))
	    o.occluded = xml_get_uint(*iter);
	  else if (!strcmp((*iter)->get_name().c_str(), "pose")) {
	    xml_get_string(*iter, o.pose);
	  }
	  ////////////////////////////////////////////////////////////////
	  // parts
	  else if (!strcmp((*iter)->get_name().c_str(), "part")) {
	    Node::NodeList plist = (*iter)->get_children();
	    for(Node::NodeList::iterator piter = plist.begin();
		piter != plist.end(); ++piter) {
	      // add new part object
	      o.parts.push_back(new object(ipart++));
	      object &p = *(o.parts.back());
	      // parse bounding box
	      if (!strcmp((*piter)->get_name().c_str(), "bndbox")) {
		Node::NodeList blist = (*piter)->get_children();
		int xmin = -1, ymin = -1, xmax = -1, ymax = -1;
		for(Node::NodeList::iterator biter = blist.begin();
		    biter != blist.end(); ++biter) {
		  // save xmin, ymin, xmax and ymax
		  if (!strcmp((*biter)->get_name().c_str(), "xmin"))
		    xmin = xml_get_uint(*biter);
		  else if (!strcmp((*biter)->get_name().c_str(), "ymin"))
		    ymin = xml_get_uint(*biter);
		  else if (!strcmp((*biter)->get_name().c_str(), "xmax"))
		    xmax = xml_get_uint(*biter);
		  else if (!strcmp((*biter)->get_name().c_str(), "ymax"))
		    ymax = xml_get_uint(*biter);
		}
		// set bbox
		p.set_rect(xmin, ymin, xmax, ymax);
		// adjusting coordinates if cropping image
		if (cropr) {
		  p.h0 -= cropr->h0;
		  p.w0 -= cropr->w0;
		}
	      } // else get object class name
	      else if (!strcmp((*piter)->get_name().c_str(), "name")) {
		xml_get_string(*piter, p.name);
	      }
	    }
	  }
	}
      }
    }
  }

#endif /* __XML__ */

} // end namespace ebl
