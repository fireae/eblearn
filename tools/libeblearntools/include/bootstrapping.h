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

#ifndef BOOTSTRAPPING_H_
#define BOOTSTRAPPING_H_

#include <stdio.h>
#include <stdlib.h>

#include "defines_tools.h"
#include "detector.h"
#include "configuration.h"
#include "pascal_xml.h"

namespace ebl {

//! A class for driven sample extraction for retraining.
template <typename T> class bootstrapping {
 public:
  bootstrapping(configuration &conf);
  virtual ~bootstrapping();

  //! Extract bootstrapping samples for frame 'fname' into internal buffers.
  //! See accessors to retrieve data.
  //! \param reset If true, clears previous results, otherwise accumulate.
  //! \param scale Scale groundtruth boxes by this factor.
  void fprop(detector<T> &detect, std::string &fname, bool reset = true,
             double scale = 1.0);
  //! Clears all previous results.
  void clear();

  // accessors ///////////////////////////////////////////////////////////////

  //! Returns a reference to positive bounding boxes.
  bboxes& get_bbpos();
  //! Returns a reference to negative bounding boxes.
  bboxes& get_bbneg();
  //! Returns a reference to positive+negative bounding boxes.
  bboxes& get_bball();
  //! Returns a reference to all groundtruth bounding boxes.
  bboxes& get_gtall();
  //! Returns a reference to cleaned up groundtruth bounding boxes.
  bboxes& get_gtclean();
  //! Returns a reference to rest of cleaned up groundtruth bounding boxes.
  bboxes& get_gtrest();
  //! Returns a reference to positive samples last extracted.
  svector<midx<T> >& get_pos();
  //! Returns a reference to negative samples last extracted.
  svector<midx<T> >& get_neg();
  //! Returns a reference to positive+negative samples last extracted.
  svector<midx<T> >& get_all();

  //! Returns true if configuration variable 'bootstrapping' is set to 1.
  bool activated();
  //! Returns true if extracting positive samples.
  bool extract_positives();
  //! Returns true if groundtruth exists for frame 'frame_name'.
  bool groundtruth_found(std::string &frame_name);
  //! Returns true if frame 'frame_name' should be ignored,
  //! i.e. if bootstrapping is activated, negatives are not extracted
  //! and no groundtruth was found for this frame.
  bool skip_frame(std::string &frame_name);
  //! Returns the maximum number of bootstrapping to be extracted
  //! (min of max limit of intg and value of "bootstrapping_max").
  intg max_size();

  // dataset methods /////////////////////////////////////////////////////////

  //! Save samples into dataset-format files.
  void save_dataset(svector<midx<T> > &samples, bboxes &bb, std::string &outdir,
                    idx<ubyte> &classes);

  // internal methods ////////////////////////////////////////////////////////
 protected:

  //! Returns groundtruth filename given a frame name.
  std::string groundtruth_file(std::string &frame_name);
  //! Load groundtruth based on the image name.
  bboxes load_groundtruth(std::string &frame_name,
                          std::vector<std::string> &labels);
  //! Load groundtruth based on the image name, and filter it based
  //! on various criteria.
  bboxes load_clean_groundtruth(std::string &frame_name, configuration &conf,
                                bboxes &rest,
                                std::vector<std::string> *labels = NULL);

  bboxes get_positives(svector<state<T> > &outputs, bboxes &groundtruth,
                       svector<mfidxdim> &topleft,
                       svector<mfidxdim> &topright,
                       svector<mfidxdim> &bottomleft,
                       svector<mfidxdim> &bottomright,
                       float matching, float min_context);
  bboxes get_negatives(svector<state<T> > &answers, bboxes &filtered,
                       bboxes &nonfiltered,
                       svector<mfidxdim> &topleft,
                       svector<mfidxdim> &topright,
                       svector<mfidxdim> &bottomleft,
                       svector<mfidxdim> &bottomright,
                       float matching, float neg_matching,
                       uint nmax, int neg_id, T threshold);
  //! Push mirror versions of all samples and their corresponding boxes
  //! at the end of 'samples' and 'boxes'.
  void add_mirrors(svector<midx<T> > &samples, bboxes &boxes);

  // private members ///////////////////////////////////////////////////////////
 protected:
  configuration &conf;
  bool           silent;
  std::string    gt_path;       //!< Path of groundtruth files.
  bboxes         gt_all;        //!< All groundtruth boxes.
  bboxes         gt_clean;      //!< Cleaned up groundtruth.
  bboxes         gt_rest;       //!< Remaining of groundtruth after clean up.
  bboxes         bbpos;         //!< Bounding boxes of positive samples.
  bboxes         bbneg;         //!< Bounding boxes of negative samples.
  bboxes         bball;         //!< Bounding boxes of all samples (pos + neg).
  svector<midx<T> > spos;       //!< Positive samples extracted.
  svector<midx<T> > sneg;       //!< Negative samples extracted.
  svector<midx<T> > sall;       //!< Positive + negative samples.
  std::string    xml_fullname;  //!< Full path of last xml groundtruth.
  bool           extract_pos;   //!< Extract positive samples or not.
  bool           extract_neg;   //!< Extract negative samples or not.
  bool           _activated;
  intg           _max_size;
  bool           neg_gt_only;   //!< Only extract negatives in images with gt.
  T              neg_threshold; //!< Threshold for negatives extraction.
  mfidxdim       bbox_scalings; //!< Scaling bboxes for each scale.
  bool           mirror_pos;    //!< Mirror positive samples.
};

} // end namespace ebl

#include "bootstrapping.hpp"

#endif /* BOOTSTRAPPING_H_ */
