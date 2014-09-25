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

#ifndef PASCALBG_DATASET_HPP_
#define PASCALBG_DATASET_HPP_

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
pascalbg_dataset<Tdata>::pascalbg_dataset(const char *name_,
                                          const char *inroot_,
                                          const char *outdir_,
                                          uint max_folders_,
                                          bool ignore_diff, bool ignore_trunc,
                                          bool ignore_occl,
                                          const char *annotations,
                                          const char *outtmp_)
    : pascal_dataset<Tdata>(name_, inroot_, ignore_diff, ignore_trunc,
			    ignore_occl, annotations) {
  this->set_outdir(outdir_, outtmp_);
  max_folders = max_folders_;
  data_cnt = 0;
  save_mode = "mat";
}

template <class Tdata>
pascalbg_dataset<Tdata>::~pascalbg_dataset() {
}

////////////////////////////////////////////////////////////////
// data extraction

template <class Tdata>
bool pascalbg_dataset<Tdata>::extract() {
#ifdef __BOOST__
#ifdef __XML__
  std::cout << "Extracting samples from files into dataset..." << std::endl;
  // adding data to dataset using all xml files in annroot
  boost::filesystem::path p(annroot);
  if (!exists(p))
    eblerror("Annotation path " << annroot << " does not exist.");
  xtimer.start();
  processed_cnt = 0;
  // find all xml files recursively (and randomize list)
  std::list<std::string> *files = find_fullfiles(annroot, XML_PATTERN, NULL, false,
                                       true, true);
  if (!files || files->size() == 0)
    eblerror("no xml files found in " << annroot << " using file pattern "
             << XML_PATTERN);
  std::cout << "Found " << files->size() << " xml files." << std::endl;
  for (std::list<std::string>::iterator i = files->begin(); i != files->end(); ++i) {
    this->process_xml(*i);
    processed_cnt++;
    if (this->full())
      break;
  }
  std::cout << "Extracted and saved " << data_cnt;
  std::cout << " background patches from dataset." << std::endl;
  std::cout << "Extraction time: " << xtimer.elapsed() << std::endl;
  if (files) delete files;
#endif /* __XML__ */
#endif /* __BOOSt__ */
  return true;
}

#ifdef __BOOST__ // disable some derived methods if BOOST not available
#ifdef __XML__ // disable some derived methods if XML not available

////////////////////////////////////////////////////////////////
// process xml

// Note: the difficult flag is ignored, so that we don't take
// background patches even in difficult bounding boxes.
template <class Tdata>
bool pascalbg_dataset<Tdata>::process_xml(const std::string &xmlfile) {
  std::string image_filename, image_fullname, folder;
  std::vector<rect<int> > bboxes;
  std::string obj_classname, pose;
  bool pose_found = false;
  Node::NodeList::iterator oiter;

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
        if (!std::strcmp((*iter)->get_name().c_str(), "filename")) {
          xml_get_string(*iter, image_filename);
        } else if (!std::strcmp((*iter)->get_name().c_str(), "folder")) {
          xml_get_string(*iter, folder);
        }
      }
      image_fullname = imgroot;
      if (!folder.empty())
        image_fullname << "/" << folder << "/";
      image_fullname += image_filename;
      // include folder into filename to avoid conflicts
      if (!folder.empty()) {
        std::string tmp;
        tmp << folder << "_" << image_filename;
        tmp = std::string_replace(tmp, "/", "_");
        image_filename = tmp;
      }
      // parse all objects in image
      for(Node::NodeList::iterator iter = list.begin();
          iter != list.end(); ++iter) {
        if (!std::strcmp((*iter)->get_name().c_str(), "object")) {
          // get object's properties
          Node::NodeList olist = (*iter)->get_children();
          for(oiter = olist.begin(); oiter != olist.end(); ++oiter) {
            if (!std::strcmp((*oiter)->get_name().c_str(), "name"))
              xml_get_string(*oiter, obj_classname);
            else if (!std::strcmp((*oiter)->get_name().c_str(), "pose")) {
              xml_get_string(*oiter, pose);
              pose_found = true;
            }
          }
          // add object's bbox
          if (!usepartsonly) {
            // add object's class to dataset
            if (usepose && pose_found) { // append pose to class name
              obj_classname += "_";
              obj_classname += pose;
            }
            if (dataset<Tdata>::included(obj_classname)) {
              bboxes.push_back(get_object(*iter));
            }
          }
          ////////////////////////////////////////////////////////////////
          // parts
          if (useparts || usepartsonly) {
            std::string part_classname;

            // add part's class to dataset
            for(oiter = olist.begin();oiter != olist.end(); ++oiter) {
              if (!std::strcmp((*oiter)->get_name().c_str(), "part")) {
                // get part's name
                Node::NodeList plist = (*oiter)->get_children();
                for(Node::NodeList::iterator piter = plist.begin();
                    piter != plist.end(); ++piter) {
                  if (!std::strcmp((*piter)->get_name().c_str(), "name")) {
                    xml_get_string(*piter, part_classname);
                    // found a part and its name, add it
                    if (usepose && pose_found) {
                      part_classname += "_";
                      part_classname += pose;
                    }
                    if (dataset<Tdata>::included(part_classname)) {
                      bboxes.push_back(get_object(*oiter));
                    }
                  }
                }
              }
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
  try {
    // load image
    idx<ubyte> img = load_image<ubyte>(image_fullname);
    // extract patches given image and bounding boxes
    process_image(img, bboxes, image_filename);
  } catch(std::string &err) {
    std::cerr << "error: failed to add " << image_fullname;
    std::cerr << ": " << std::endl << err << std::endl;
    add_errors++;
  }
  return true;
}

////////////////////////////////////////////////////////////////
// process 1 object of an xml file

template <class Tdata>
rect<int> pascalbg_dataset<Tdata>::get_object(Node* onode) {
  unsigned int xmin = 0, ymin = 0, xmax = 0, ymax = 0;

  // parse object node
  Node::NodeList list = onode->get_children();
  for(Node::NodeList::iterator iter = list.begin();
      iter != list.end(); ++iter) {
    // parse bounding box
    if (!std::strcmp((*iter)->get_name().c_str(), "bndbox")) {
      Node::NodeList blist = (*iter)->get_children();
      for(Node::NodeList::iterator biter = blist.begin();
          biter != blist.end(); ++biter) {
        // save xmin, ymin, xmax and ymax
        if (!std::strcmp((*biter)->get_name().c_str(), "xmin"))
          xmin = xml_get_uint(*biter);
        else if (!std::strcmp((*biter)->get_name().c_str(), "ymin"))
          ymin = xml_get_uint(*biter);
        else if (!std::strcmp((*biter)->get_name().c_str(), "xmax"))
          xmax = xml_get_uint(*biter);
        else if (!std::strcmp((*biter)->get_name().c_str(), "ymax"))
          ymax = xml_get_uint(*biter);
      }
    } // else get object class name
  }
  rect<int> r(ymin, xmin, ymax - ymin, xmax - xmin);
  return r;
}

////////////////////////////////////////////////////////////////
// process object's image

template <class Tdata>
void pascalbg_dataset<Tdata>::
process_image(idx<ubyte> &img, std::vector<rect<int> >& bboxes,
              const std::string &image_filename) {
  std::vector<rect<int> > patch_bboxes;
  std::vector<rect<int> >::iterator ibb;
  idxdim d(img);
  std::ostringstream fname;
  bool overlap;

  // for each scale, find patches and save them
  for (std::vector<double>::iterator i = scales.begin(); i != scales.end(); ++i) {
    patch_bboxes.clear();
    // rescale original bboxes
    //       double ratio = std::max(outdims.dim(0) / (double) img.dim(0),
    // 			      outdims.dim(1) / (double) img.dim(1)) * *i;
    double ratio = *i;
    // extract all non overlapping patches with dimensions outdims that
    // do not overlap with bounding boxes
    rect<int> patch(0, 0, outdims.dim(0), outdims.dim(1));
    patch = patch * ratio;
    for (patch.h0 = 0; patch.h0 + patch.height <= img.dim(0);
         patch.h0 += patch.height) {
      for (patch.w0 = 0; patch.w0 + patch.width <= img.dim(1);
           patch.w0 += patch.width) {
        // test if patch overlaps with any bounding box or is outside of image
        overlap = false;
        for (ibb = bboxes.begin(); ibb != bboxes.end(); ++ibb) {
          if (patch.overlap(*ibb)) {
            overlap = true;
            break ;
          }
        }
        if (!overlap) {
          // push patch to list of extracted patches
          patch_bboxes.push_back(patch);
        }
      }
    }
    fname.str("");
    fname << image_filename << "_scale" << *i;
    if (patch_bboxes.size() == 0)
      std::cout << "No background patches could be extracted at scale "
           << *i << std::endl;
    else {
      save_patches(img, image_filename, patch_bboxes, bboxes,
                   outtmp, max_folders, fname.str());
    }
  }
}

////////////////////////////////////////////////////////////////
// save patches

template <class Tdata>
void pascalbg_dataset<Tdata>::save_patches(idx<ubyte> &im, const std::string &image_filename,
                                           std::vector<rect<int> > &patch_bboxes,
                                           std::vector<rect<int> > &objs_bboxes,
                                           const std::string &outd,
                                           uint max_folders,
                                           const std::string &filename) {
  std::ostringstream folder, fname;
  std::string cname = "background";
  rect<int> inr;
  // change image type from ubyte to Tdata
  idx<Tdata> img(im.get_idxdim());
  idx_copy(im, img);
  try {
    mkdir_full(outd.c_str());
    uint i;
    // shuffle randomly std::vector of patches to avoid taking top left corner
    // as first patch every time
    random_shuffle(patch_bboxes.begin(), patch_bboxes.end());
    // loop on patches
    for (i = 0; (i < patch_bboxes.size()) && (i < max_folders); ++i) {
      // extract patch
      rect<int> p = patch_bboxes[i];
      midx<Tdata> patch(1);
      patch.mset(img, 0);
      // TODO: fix nasty memory leak if assigning patch = pp(patch) by deleting
      // references in destructor of midx
      midx<Tdata> patch2 = this->preprocess_data(patch, &cname, image_filename.c_str(),
                                                 &p, 0, NULL, NULL,
                                                 NULL, NULL, NULL, &inr);
      patch.clear();
      // create folder if doesn't exist
      folder.str("");
      folder << outd << "/" << "bg" << i+1 << "/";
      mkdir_full(folder.str().c_str());
      folder << "/background/";
      mkdir_full(folder.str().c_str());
      // save patch in folder
      // switch saving behavior
      fname.str("");
      fname << folder.str() << filename << ".bg" << i+1;
      if (!std::strcmp(save_mode.c_str(), "mat")
          || !std::strcmp(save_mode.c_str(), "dynset")) { // lush matrix mode
        fname << MATRIX_EXTENSION;
        //	  idx<Tdata> patch2 = patch.shift_dim(2, 0);
        patch2.shift_dim_internal(2, 0);
        if (!save_matrices(patch2, fname.str())) {
          patch2.clear();
          throw fname.str();
        }
      } else { // image file mode
        eblerror("fix implementation." << save_mode.c_str());
        // fname << "." << save_mode;
        // idx<Tdata> tmp = patch;
        // // scale image to 0 255 if preprocessed
        // if (std::strcmp(ppconv_type.c_str(), "RGB")) {
        //   idx_addc(tmp, (Tdata) 1.0, tmp);
        //   idx_dotc(tmp, (Tdata) 127.5, tmp);
        // }
        // save_image(fname.str(), tmp, save_mode.c_str());
      }
      images_list.push_back(fname.str()); // add image to files list
      std::cout << data_cnt++ << ": saved " << fname.str().c_str()
           << " " << patch2 << ", eta: " << xtimer.eta(data_cnt, max_data)
           << ", elapsed: " << xtimer.elapsed() << std::endl;
      display_patch(patch2, img, image_filename, cname, p, inr,
                    objs_bboxes, patch_bboxes);
      // TEMPORARY MEMORY LEAK FIX (use smart srg pointer to clear
      // automatically on object deletion)
      patch2.clear();
    }
    //       if (i < patches.size()) // reached max_folders, fill-up last one
    // 	for ( ; i < patches.size(); ++i) {
    // 	  // save patch in folder
    // 	  fname.str("");
    // 	  fname << folder.str() << filename << ".bg" << i+1 << ".mat";
    // 	  if (!save_matrix(patches[i], fname.str()))
    // 	    throw fname.str();
    // 	  std::cout << data_cnt++ << ": saved " << fname.str().c_str() << std::endl;
    // 	}
  } catch (const std::string &err) {
    std::cerr << "error: failed to save patch in " << err << std::endl;
  }
}

template <class Tdata>
void pascalbg_dataset<Tdata>::
display_patch(midx<Tdata> &patch, idx<Tdata> &img,
              const std::string &image_filename, const std::string &cname,
              rect<int> &pbbox, rect<int> &r, std::vector<rect<int> > &objs_bboxes,
              std::vector<rect<int> > &patch_bboxes) {
#ifdef __GUI__
  if (display_extraction) {
    disable_window_updates();
    // display
    //idx<Tdata> im3 = patch.shift_dim(2, 0);
    uint h = 47, w = 0;
    this->display_added(patch, img, &cname, image_filename.c_str(), NULL,
                  &r, false, NULL, NULL, NULL, NULL, NULL, NULL, &w);
    // draw patch bboxes
    std::vector<rect<int> >::iterator ibb;
    for (ibb = patch_bboxes.begin(); ibb != patch_bboxes.end(); ++ibb)
      draw_box(h + ibb->h0, w + ibb->w0,
               ibb->height, ibb->width, 0, 255, 0);
    // draw objects bboxes
    for (ibb = objs_bboxes.begin(); ibb != objs_bboxes.end(); ++ibb)
      draw_box(h + ibb->h0, w + ibb->w0,
               ibb->height, ibb->width, 255, 255, 0);
    // draw requested patch bbox
    draw_box(h + pbbox.h0, w + pbbox.w0,
             pbbox.height, pbbox.width, 0, 0, 255);
    enable_window_updates();
    if (sleep_display)
      millisleep((long) sleep_delay);
  }
#endif
}


#endif /* __XML__ */
#endif /* __BOOST__ */

} // end namespace ebl

#endif /* PASCALBG_DATASET_HPP_ */
