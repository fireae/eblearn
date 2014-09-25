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

#ifndef PASCALBG_DATASET_H_
#define PASCALBG_DATASET_H_

#include "dataset.h"
#include "xml_utils.h"

namespace ebl {

//! The pascalbg_dataset class extract background patches from
//! a dataset of type PASCAL.
template <class Tdata> class pascalbg_dataset : public pascal_dataset<Tdata> {
 public:

  ////////////////////////////////////////////////////////////////
  // constructors

  //! Initialize the dataset's name and other internal variables, but does
  //! not allocate data matrices, user must call alloc for that effect.
  //! outdims are the target output dimensions of each sample.
  //! inroot is the root directory from which we extract data.
  //! ignore_diff ignores difficult objects if true.
  pascalbg_dataset(const char *name, const char *inroot, const char *outdir,
                   uint max_folders = 1, bool ignore_diff = true,
                   bool ignore_trunc = false, bool ignore_occl = false,
                   const char *annotations = NULL, const char *tmpout = NULL);

  //! Destructor.
  virtual ~pascalbg_dataset();

  ////////////////////////////////////////////////////////////////
  // data

  //! Extract data from files into dataset.
  virtual bool extract();

 protected:

#ifdef __BOOST__ // disable some derived methods if BOOST not available
#ifdef __XML__ // disable some derived methods if XML not available

  ////////////////////////////////////////////////////////////////
  // internal methods

  //! process an xml file.
  virtual bool process_xml(const std::string &xmlfile);

  //! return bounding box of object
  virtual rect<int> get_object(Node* onode);

  //! process image given all bounding boxes.
  virtual void process_image(idx<ubyte> &img, std::vector<rect<int> >& bboxes,
                             const std::string &image_filename);

  //! save patches into directory outdir, creating 1 subdirectory per patch
  //! until reaching max_folders, then filling last folder with remaining
  //! patches, using filename as base filename for patches filenames.
  void save_patches(idx<ubyte> &img, const std::string &image_filename,
                    std::vector<rect<int> > &patch_bboxes,
                    std::vector<rect<int> > &objs_bboxes, const std::string &outdir,
                    uint max_folders, const std::string &filename);

  void display_patch(midx<Tdata> &patch, idx<Tdata> &img,
                     const std::string &image_filename,
                     const std::string &cname,
                     rect<int> &pbbox, rect<int> &r,
                     std::vector<rect<int> > &objs_bboxes,
                     std::vector<rect<int> > &patch_bboxes);

#endif /* __BOOST__ */
#endif /* __XML__ */

 protected:
  uint max_folders; // maximum number of patch directories
  // base class members to be used ///////////////////////////////
  using pascal_dataset<Tdata>::annroot;
  using pascal_dataset<Tdata>::imgroot;
  using dataset<Tdata>::inroot;
  using dataset<Tdata>::display_extraction;
  using dataset<Tdata>::display_result;
  using dataset<Tdata>::outdims;
  using dataset<Tdata>::outdir;
  using dataset<Tdata>::sleep_display;
  using dataset<Tdata>::sleep_delay;
  using dataset<Tdata>::print_stats;
  using dataset<Tdata>::data_cnt;
  using dataset<Tdata>::extension;
  using dataset<Tdata>::scales;
  using dataset<Tdata>::max_data;
  using dataset<Tdata>::max_data_set;
  using dataset<Tdata>::useparts;
  using dataset<Tdata>::usepartsonly;
  using dataset<Tdata>::exclude;
  using dataset<Tdata>::include;
  using dataset<Tdata>::usepose;
  using dataset<Tdata>::save_mode;
  using dataset<Tdata>::original_bbox;
  using dataset<Tdata>::add_errors;
  using pascal_dataset<Tdata>::ignore_difficult;
  using pascal_dataset<Tdata>::ignore_truncated;
  using pascal_dataset<Tdata>::ignore_occluded;
  using dataset<Tdata>::xtimer;
  using dataset<Tdata>::processed_cnt;
  using dataset<Tdata>::fovea;
  using dataset<Tdata>::images_list;
  using dataset<Tdata>::outtmp;
};

} // end namespace ebl

#include "pascalbg_dataset.hpp"

#endif /* PASCALBG_DATASET_H_ */
