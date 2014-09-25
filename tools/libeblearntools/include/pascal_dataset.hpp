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

#ifndef PASCAL_DATASET_HPP_
#define PASCAL_DATASET_HPP_

#include <algorithm>

#include "xml_utils.h"
#include "tools_utils.h"

#ifdef __BOOST__
#ifndef BOOST_FILESYSTEM_VERSION
#define BOOST_FILESYSTEM_VERSION 3
#endif
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#endif

namespace ebl {

//////////////////////////////////////////////////////////////////////////////
// constructors & initializations

template <class Tdata>
pascal_dataset<Tdata>::pascal_dataset(const char *name_, const char *inroot_,
                                      bool ignore_diff, bool ignore_trunc,
                                      bool ignore_occl,
                                      const char *annotations,
                                      const char *ignore_path,
                                      bool ignore_bb)
    : dataset<Tdata>(name_, inroot_),
      min_aspect_ratio(-1), max_aspect_ratio(-1), minborders(),
      max_jitter_match(1.0) {
  // initialize pascal-specific members
  // if (inroot_) {
  //   annroot = inroot;
  //   annroot += "/Annotations/"; // look for xml files in annotations
  //   imgroot = inroot;
  //   imgroot += "JPEGImages/"; // image directory
  // }
  imgroot = inroot;
  if (annotations && strcmp(annotations, ""))
    annroot = annotations;
  else
    eblerror("expected annotations folder, please specify with -annotations");
  if (ignore_path && strcmp(ignore_path, ""))
    ignore_root = ignore_path;
  ignore_difficult = ignore_diff;
  ignore_bbox = ignore_bb;
  ignore_truncated = ignore_trunc;
  ignore_occluded = ignore_occl;
#ifndef __XML__ // return error if xml not enabled
  eblerror("XML libraries not available, install libxml++ and recompile");
#endif /* __XML__ */
  extension = XML_PATTERN;
  std::cout << "Image search extension pattern: " << extension << std::endl;
}

template <class Tdata>
pascal_dataset<Tdata>::~pascal_dataset() {
}

////////////////////////////////////////////////////////////////
// data extraction

template <class Tdata>
bool pascal_dataset<Tdata>::extract() {
#ifdef __BOOST__
#ifdef __XML__
  if (!allocated && !strcmp(save_mode.c_str(), DATASET_SAVE))
    return false;
  std::cout << "Extracting samples from PASCAL files into dataset..."
            << std::endl;
  // adding data to dataset using all xml files in annroot
  boost::filesystem::path p(annroot);
  if (!exists(p))
    eblerror("Annotation path " << annroot << " does not exist.");
  xtimer.start();
  processed_cnt = 0;
  // find all xml files recursively
  std::list<std::string> *files = find_fullfiles(annroot, XML_PATTERN, NULL,
                                                 true, true);
  if (!files || files->size() == 0)
    eblerror("no xml files found in " << annroot << " using file pattern "
             << XML_PATTERN);
  std::cout << "Found " << files->size() << " xml files." << std::endl;
  for (std::list<std::string>::iterator i = files->begin();
       i != files->end(); ++i) {
    this->process_xml(*i);
    processed_cnt++;
    if (full())
      break;
  }
  std::cout << "Extracted " << data_cnt << " elements into dataset." << std::endl;
  std::cout << "Extraction time: " << xtimer.elapsed() << std::endl;
  print_stats();
  if (files) delete files;
#endif /* __XML__ */
#endif /* __BOOST__ */
  return true;
}

template <class Tdata>
void pascal_dataset<Tdata>::set_min_aspect_ratio(float minar) {
  std::cout << "Setting minimum aspect ratio to " << minar << std::endl;
  min_aspect_ratio = minar;
}

template <class Tdata>
void pascal_dataset<Tdata>::set_max_aspect_ratio(float maxar) {
  std::cout << "Setting maximum aspect ratio to " << maxar << std::endl;
  max_aspect_ratio = maxar;
}

template <class Tdata>
void pascal_dataset<Tdata>::set_minborders(idxdim &d) {
  minborders = d;
  std::cout << "Setting minimum distances allowed to image borders to "
            << minborders << std::endl;
}

template <class Tdata>
void pascal_dataset<Tdata>::set_max_jitter_match(float match) {
  max_jitter_match = match;
  std::cout << "Setting maximum jitter match between jittered rects and other "
            << "objects in an image to " << max_jitter_match << std::endl;
}

////////////////////////////////////////////////////////////////
// data

#ifdef __XML__ // disable some derived methods if XML not available
#ifdef __BOOST__

template <class Tdata>
bool pascal_dataset<Tdata>::included_pascal(const std::string &class_name,
                                            uint difficult, uint truncated,
                                            uint occluded) {
  return dataset<Tdata>::included(class_name)
      && !(ignore_difficult && difficult)
      && !(ignore_truncated && truncated)
      && !(ignore_occluded && occluded);
}

template <class Tdata>
intg pascal_dataset<Tdata>::count_samples() {
  total_difficult = 0;
  total_truncated = 0;
  total_occluded = 0;
  total_ignored = 0;
  total_samples = 0;
  std::string xmlpath;
  boost::filesystem::path p(annroot);
  if (!boost::filesystem::exists(p))
    eblthrow("Annotation path " << annroot << " does not exist.");
  std::cout << "Counting number of samples in " << annroot << " ..."
            << std::endl;
  // find all xml files recursively
  std::list<std::string> *files = find_fullfiles(annroot, XML_PATTERN, NULL,
                                                 false, true);
  if (!files || files->size() == 0)
    eblthrow("no xml files found in " << annroot << " using file pattern "
             << XML_PATTERN);
  std::cout << "Found " << files->size() << " xml files." << std::endl;
  for (std::list<std::string>::iterator i = files->begin(); i != files->end(); ++i) {
    xmlpath = *i;
    // parse xml
    DomParser parser;
    parser.parse_file(xmlpath);
    if (parser) {
      // initialize root node and list
      const Node* pNode = parser.get_document()->get_root_node();
      Node::NodeList list = pNode->get_children();
      // parse all objects in image
      for(Node::NodeList::iterator iter = list.begin();
          iter != list.end(); ++iter) {
        if (!strcmp((*iter)->get_name().c_str(), "object")) {
          // check for difficult flag in object node
          Node::NodeList olist = (*iter)->get_children();
          count_sample(olist);
        }
      }
    }
  }
  if (files) delete files;
  std::cout << "Found: " << total_samples << " samples, including ";
  std::cout << total_difficult << " difficult, " << total_truncated;
  std::cout << " truncated and " << total_occluded << " occluded." << std::endl;
  ignore_difficult ? std::cout << "Ignoring" : std::cout << "Using";
  std::cout << " difficult samples." << std::endl;
  ignore_truncated ? std::cout << "Ignoring" : std::cout << "Using";
  std::cout << " truncated samples." << std::endl;
  ignore_occluded ? std::cout << "Ignoring" : std::cout << "Using";
  std::cout << " occluded samples." << std::endl;
  total_samples = total_samples - total_ignored;
  return total_samples;
}

template <class Tdata>
void pascal_dataset<Tdata>::count_sample(Node::NodeList &olist) {
  uint difficult = 0, truncated = 0, occluded = 0;
  std::string obj_classname, pose;
  bool pose_found = false;
  Node::NodeList::iterator oiter;

  for(oiter = olist.begin(); oiter != olist.end(); ++oiter) {
    if (!strcmp((*oiter)->get_name().c_str(), "difficult"))
      difficult = xml_get_uint(*oiter);
    else if (!strcmp((*oiter)->get_name().c_str(), "truncated"))
      truncated = xml_get_uint(*oiter);
    else if (!strcmp((*oiter)->get_name().c_str(), "occluded"))
      occluded = xml_get_uint(*oiter);
    else if (!strcmp((*oiter)->get_name().c_str(), "name"))
      xml_get_string(*oiter, obj_classname);
    else if (!strcmp((*oiter)->get_name().c_str(), "pose")) {
      xml_get_string(*oiter, pose);
      pose_found = true;
    }
  }

  ////////////////////////////////////////////////////////////////
  // object
  if (!usepartsonly) {
    // add object's class to dataset
    if (included_pascal(obj_classname, difficult, truncated, occluded)) {
      if (usepose && pose_found) { // append pose to class name
        obj_classname += "_";
        obj_classname += pose;
      }
      if (included_pascal(obj_classname, difficult, truncated, occluded))
        this->add_class(obj_classname);
    }
  }
  // increment samples numbers
  total_samples++;
  if (difficult) total_difficult++;
  if (truncated) total_truncated++;
  if (occluded) total_occluded++;
  if ((difficult && ignore_difficult)
      || (truncated && ignore_truncated)
      || (occluded && ignore_occluded))
    total_ignored++;

  ////////////////////////////////////////////////////////////////
  // parts
  if (useparts || usepartsonly) {
    std::string part_classname;

    // add part's class to dataset
    for(oiter = olist.begin();oiter != olist.end(); ++oiter) {
      if (!strcmp((*oiter)->get_name().c_str(), "part")) {
        // get part's name
        Node::NodeList plist = (*oiter)->get_children();
        for(Node::NodeList::iterator piter = plist.begin();
            piter != plist.end(); ++piter) {
          if (!strcmp((*piter)->get_name().c_str(), "name")) {
            xml_get_string(*piter, part_classname);
            // found a part and its name, add it
            if (included_pascal(part_classname, difficult, truncated, occluded))
            {
              if (usepose && pose_found) { // append pose to class name
                part_classname += "_";
                part_classname += pose;
              }
              if (dataset<Tdata>::included(part_classname)) {
                this->add_class(part_classname);
                // increment samples numbers
                this->total_samples++;
                if (difficult) total_difficult++;
                if (truncated) total_truncated++;
                if (occluded) total_occluded++;
                if ((difficult && ignore_difficult)
                    || (truncated && ignore_truncated)
                    || (occluded && ignore_occluded))
                  total_ignored++;
              }
            }
          }
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////
// process xml

template <class Tdata>
bool pascal_dataset<Tdata>::process_xml(const std::string &xmlfile) {
  std::string image_filename, image_fullname, folder;
  int height = -1, width = -1, depth = -1;
  rect<int> *cropr = NULL;

  // get image's properties
  if (!pascal_xml::get_properties(imgroot, xmlfile, image_filename,
                                  image_fullname, folder, height, width,
                                  depth, objects, &cropr, false))
    return false;
  // get ignored boxes if present
  if (!ignore_root.empty()) {
    std::string bname = ebl::basename(xmlfile.c_str());
    std::string dname = ebl::dirname(image_filename.c_str());
    std::string xml, filename, fullname, folder;
    int h = -1, w = -1, d = -1;
    xml << ignore_root << "/" << dname << "/" << bname;
    if (file_exists(xml)) {
      if (!pascal_xml::get_properties(imgroot, xml, filename, fullname,
                                      folder, h, w, d, objects, &cropr, true))
        return false;
      // check that ignored properties matches original properties
      if (fullname.compare(image_fullname) || (height != h) || (width != w))
        eblerror("mistmatch between orignal image (" << image_fullname << ", "
                 << height << "x" << width << ") and ignored infos ("
                 << fullname << ", " << h << "x" << w << ")");
    } else // ignore file doesnt exist
      std::cerr << "warning: ignore xml not found: " << xml << std::endl;
  }
  // process objects
  process_objects(objects, height, width, image_fullname,
                  image_filename, cropr);
  // delete all objects
  for (uint i = 0; i < objects.size(); ++i)
    delete objects[i];
  objects.clear();
  if (cropr) delete cropr;
  return true;
}

////////////////////////////////////////////////////////////////
// process all objects

template <class Tdata>
bool pascal_dataset<Tdata>::
process_objects(const std::vector<object*> &objs, int height, int width,
                const std::string &image_fullname,
                const std::string &image_filename,
                const rect<int> *cropr) {
  idx<Tdata> *img = NULL;
  // process all objects
  for (iobj = 0; iobj < objs.size(); ++iobj) {
    object &o = *(objs[iobj]);
    if (!o.ignored) { // only process non-ignored objects
      if (!usepartsonly) {
        // check that minimum visibility ratio is respected
        if (minvisibility > 0.0 && o.visible) {
          float match = o.match(*o.visible);
          if (match < minvisibility) {
            std::cout << "Rejecting object " << o.id << " from "
                      << image_filename << " because visible ratio ("
                      << match << ") is lower than "
                      << "limit (" << minvisibility << ")" << std::endl;
            continue ; // skip this object
          }
        }
        // check that minimum aspect ratio is respected
        if (o.height == 0) continue ; // skip this object
        float ar = o.width / (float) o.height;
        if (min_aspect_ratio >= 0.0 && ar < min_aspect_ratio) {
          std::cout << "Rejecting object " << o.id << " from " << image_filename
                    << " because aspect ratio of " << o << " (" << ar
                    << ") is lower than " << "limit (" << min_aspect_ratio << ")"
                    << std::endl;
          continue ; // skip this object
        }
        if (max_aspect_ratio >= 0.0 && ar > max_aspect_ratio) {
          std::cout << "Rejecting object " << o.id << " from " << image_filename
                    << " because aspect ratio of " << o << " (" << ar
                    << ") is bigger than " << "limit (" << max_aspect_ratio << ")"
                    << std::endl;
          continue ; // skip this object
        }
        // check that object is larger than minimum size
        if (o.height < mindims.dim(0) || o.width < mindims.dim(1)) {
          std::cout << "Rejecting object " << o.id << " from "
                    << image_filename << " because object's size ("
                    << o << ") is smaller than minimum size "
                    << "(" << mindims << ")" << std::endl;
          continue ; // skip this object
        }
        // check that bbox is not below image border distances
        if (!minborders.empty()) {
          if (height < 0 || width < 0) { // we don't know the image sizes
            eblerror("image sizes not found in xml");
          } // from now on assume all images have the same size (TODO)
          int hmax = height - minborders.dim(0);
          int wmax = width - minborders.dim(1);
          if (o.h0 < (int) minborders.dim(0) || o.w0 < minborders.dim(1)
              || o.h0 + o.height > hmax || o.w0 + o.width > wmax) {
            std::cout << "Rejecting object " << o.id << " from "
                      << image_filename << " because (cropped) bbox " << o
                      << " is closer to image borders (" << height << "x" << width
                      << ") than allowed (" << minborders << ")" << std::endl;
            continue ; // skip this object
          }
        }
        // process image
        if (included_pascal(o.name, o.difficult, o.truncated, o.occluded)) {
          // load image if not already loaded
          if (!img) {
            // check that image exists
            if (!file_exists(image_fullname)) {
              eblwarn("Error: image not found " << image_fullname);
              return false;
            }
            input_height = height;
            input_width = width;
            //load image
            load_data(image_fullname);
          }
          // remove jitters that overlap with other objects
          if (max_jitter_match > 0)
            remove_jitter_matches(objs, iobj, max_jitter_match);
          // extract object from image
          process_mimage(load_img, o, o.name, o.difficult,
                        image_fullname, o.centroid, o.visible, cropr);
          load_img.clear();
        }
      }
    }
  }
  if (img) delete img;
  return true;
}

template <class Tdata>
void pascal_dataset<Tdata>::load_data(const std::string &fname) {
  // load image
  dataset<Tdata>::load_data(fname);
  // check that image matches expected sizes
  if (load_img.mget(0).dim(0) != input_height
      || load_img.mget(0).dim(1) != input_width) {
    eblerror( "Error: expected image of size is different \
                 from loaded image size");
  }
}

////////////////////////////////////////////////////////////////
// process object's image

template <class Tdata>
void pascal_dataset<Tdata>::
process_mimage(midx<Tdata> &imgs, const rect<int> &r,
              std::string &obj_class, uint difficult,
              const std::string &image_filename,
              std::pair<int,int> *center, const rect<int> *visr,
              const rect<int> *cropr) {
  t_label label = this->get_label_from_class(obj_class);
  rect<int> rr = r;
  if (ignore_bbox)
    rr = rect<int>(0, 0, imgs.mget(0).dim(0), imgs.mget(0).dim(1));
  add_mdata(imgs, label, &obj_class, image_filename.c_str(), &rr, center,
           visr, cropr, &objects);
  imgs.clear();
}

////////////////////////////////////////////////////////////////
#endif /* __XML__ */
#endif /* __BOOST__ */

template <class Tdata>
void pascal_dataset<Tdata>::compute_random_jitter() {
  // recompute all possible jitters
  dataset<Tdata>::compute_random_jitter();
  // remove jitters overlapping with other boxes given current box
  remove_jitter_matches(objects, iobj, max_jitter_match);
}

template <class Tdata>
void pascal_dataset<Tdata>::
remove_jitter_matches(const std::vector<object*> &objs,
                      uint iobj, float max_match) {
  rect<int> ri = *objs[iobj];
  // apply w/h ratio to object
  if (this->bbox_woverh > 0)
    ri.scale_width(this->bbox_woverh);
  // find ratio between current box and target
  float ratio = std::max(ri.height / (float) this->height,
                         ri.width / (float) this->width);
  EDEBUG("jitter ratio is " << ratio);
  // check overlaps with other objects
  std::vector<jitter>::iterator i = random_jitter.begin();
  for ( ; i != random_jitter.end(); ++i) {
    rect<int> rijitt = i->get_rect(ri, ratio);
    for (uint j = 0; j < objs.size(); ++j) {
      rect<int> rj = *objs[j];
      // scale object the same way jitter has been scaled
      rj.scale_centered(i->s, i->s);
      // apply w/h ratio to object
      if (this->bbox_woverh > 0)
        rj.scale_width(this->bbox_woverh);
      if (rj.match(rijitt) > max_match) {
        // there is a match, remove this jitter
        // note: erasing in a vector is not efficient because it has to move
        // all elements, but this should be rare enough to be better than
        // creating a new vector to which we add all elements.
        std::cout << "(removing jitter) match with jitter is more than "
                  << max_match << ": " << rj << " and " << rijitt
                  << " match by " << rj.match(rijitt) << std::endl;
        i = random_jitter.erase(i);
        if (i == random_jitter.end())
          return ;
        break ; // stop looking for match
      }
    }
  }
}

template <class Tdata>
void pascal_dataset<Tdata>::extract_statistics() {
#ifdef __BOOST__
  boost::filesystem::path p(annroot);
  if (!exists(p))
    eblerror("Annotation path " << annroot << " does not exist.");
  // find all xml files recursively
  std::list<std::string> *files = find_fullfiles(annroot, XML_PATTERN, NULL,
                                                 true, true);
  if (!files || files->size() == 0)
    eblerror("no xml files found in " << annroot << " using file pattern "
             << XML_PATTERN);
  std::cout << "Found " << files->size() << " xml files." << std::endl;
  // open an output file
  std::string fname;
  mkdir_full(outdir.c_str());
  fname << outdir << "/stats.csv";
  std::ofstream *fp = new std::ofstream(fname.c_str());
  if (!fp) eblerror("failed to open " << fname);
  std::cout << "Extracting statistics into " << fname << " ..." << std::endl;
  // write header
  *fp << "filename; height; width; h/w ratio; bbox height; bbox width; bbox h/w ratio; "
      << "max context;" << std::endl;
  // write stats
  for (std::list<std::string>::iterator i = files->begin();
       i != files->end(); ++i)
    this->write_statistics(*i, *fp);
  if (files) delete files;
  delete fp;
#endif
}

template <class Tdata>
void pascal_dataset<Tdata>::write_statistics(std::string &xml,
                                             std::ofstream &fp) {
  std::string image_filename, image_fullname, folder;
  int height = -1, width = -1, depth = -1;
  rect<int> *cropr = NULL;

  // get image's properties
  if (!pascal_xml::get_properties(imgroot, xml, image_filename,
                                  image_fullname, folder, height, width,
                                  depth, objects, &cropr, false))
    eblerror("error while getting properties of " << xml);
  // loop on objects
  for (uint i = 0; i < objects.size(); ++i) {
    object &o = *objects[i];
    // compute maximum context ratio of object
    float maxh = (float) std::max(0, std::min(o.h0, height - o.h0 +o.height));
    float maxw = (float) std::max(0, std::min(o.w0, width - o.w0 + o.width));
    float cratio = std::min((maxh * 2 + o.height) / o.height,
                            (maxw * 2 + o.width) / o.width);
    fp << xml << "; " << height << "; " << width
       << "; " << height / (float) width << "; "
       << o.height << "; " << o.width << "; " << o.height / (float) o.width
       << "; " << cratio << ';' << std::endl;
  }
  // delete all objects
  for (uint i = 0; i < objects.size(); ++i)
    delete objects[i];
  objects.clear();
  if (cropr) delete cropr;
}

} // end namespace ebl

#endif /* PASCAL_DATASET_HPP_ */
