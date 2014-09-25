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

#ifndef NMS_H_
#define NMS_H_

#include "bbox.h"

namespace ebl {

  // nms types /////////////////////////////////////////////////////////////////

  //! Types of nms.
  enum t_nms { nms_none		  = 0,	// no nms
	       nms_overlap	  = 1,	// traditional nms
	       nms_voting	  = 2,	// voting nms
	       nms_voting_overlap = 3	// voting followed by traditional
  };

  // nms ///////////////////////////////////////////////////////////////////////

  //! A non-maximum suppression class for suppressing overlapping non-maximum
  //! answers in a detection task.
  class EXPORT nms {
  public:
    //! Construct an NMS that suppresses non-maximum bounding boxes,
    //! i.e. retaining only highest confidence boxes among all matching
    //! boxes. Matching criterion depends on parameters defined below.
    //! \param max_overlap Boxes with lower overlap are considered matching.
    //! \param max_hcenter_dist If distance between center heights of boxes
    //!   is below this, boxes are considered matching.
    //! \param max_wcenter_dist If distance between center widths of boxes
    //!   is below this, boxes are considered matching.
    //! \param pre_hfact Pre-processing height scaling.
    //! \param pre_wfact Pre-processing width scaling.
    //! \param post_hfact Post-processing height scaling.
    //! \param post_wfact Post-processing width scaling.
    //! \param woverh Normalize width to be height * woverh.
    nms(float threshold, float max_overlap, float max_hcenter_dist,
	float max_wcenter_dist, float pre_hfact, float pre_wfact,
	float post_hfact, float post_wfact, float woverh,
	std::ostream &out = std::cout, std::ostream &err = std::cerr);
    //! Destructor.
    virtual ~nms();

    //! Applies nms processing on 'in' boxes and puts results into 'out'.
    virtual void fprop(bboxes &in, bboxes &out);
    //! Returns a string decribing this nms.
    virtual std::string describe();

    // friends /////////////////////////////////////////////////////////////////
    friend class voting_nms;

  protected:
    // internal methods ////////////////////////////////////////////////////////

    //! Apply nms on 'in' into 'out'.
    virtual void process(bboxes &in, bboxes &out);
    //! A traditional nms, pruning 'matching' boxes and keeping only
    //! the one with highest confidence. The matching criterion is based
    //! on several factors (see constructor parameters).
    virtual void traditional_nms(bboxes &in, bboxes &out);

    //! Prune bounding boxes between scales into prune_bboxes.
    //! \param same_class_only If true, only classes from the same class
    //!   can prune each other, otherwise any bb can cancel any other bb.
    void prune_overlap(bboxes &raw_bboxes, bboxes &prune_bboxes,
		       float max_match,
		       bool same_class_only = false,
		       float min_hcenter_dist = 0.0,
		       float min_wcenter_dist = 0.0,
		       float threshold = 0.0,
		       float same_scale_mhd = 0.0, float same_scale_mwd = 0.0);

    // member variables ////////////////////////////////////////////////////////
  protected:
    std::ostream &mout;	//! output stream.
    std::ostream &merr;	//! error output stream.
    float        threshold;	//!< Ignore confidences below threshold.
    float        max_overlap;	//!< Maximum ratio of overlap authorized.
    float	 max_hcenter_dist;	//!< Max dist to center for matching.
    float	 max_wcenter_dist;	//!< Max dist to center for matching.
    float        pre_hfact; //!< Height bbox preprocessing factor.
    float        pre_wfact; //!< Width bbox preprocessing factor.
    float        post_hfact; //!< Height bbox postprocessing factor.
    float        post_wfact; //!< Width bbox postprocessing factor.
    float        woverh; //!< Width / height factor.
    bool         same_class_matching_only; //!< Only match same classes.
  };

  // voting nms ////////////////////////////////////////////////////////////////

  //! A type of NMS that accumulates bounding boxes.
  class EXPORT voting_nms : public nms {
  public:
    //! Constructor for voting nms only. See nms() constructor for
    //! parameters descriptions.
    voting_nms(float threshold, float max_overlap, float max_hcenter_dist,
	       float max_wcenter_dist, float pre_hfact, float pre_wfact,
	       float post_hfact, float post_wfact, float woverh,
	       std::ostream &out = std::cout, std::ostream &err = std::cerr);
    //! Constructor for voting nms followed by traditional nms.
    //! See nms() constructor for parameters descriptions.
    voting_nms(float threshold, float max_overlap, float max_hcenter_dist,
	       float max_wcenter_dist, float pre_hfact, float pre_wfact,
	       float post_hfact, float post_wfact, float woverh,
	       float vote_max_overlap, float vote_max_hcd, float vote_max_wcd,
	       std::ostream &out = std::cout, std::ostream &err = std::cerr);
    //! Destructor.
    virtual ~voting_nms();

    //! Returns a string decribing this nms.
    virtual std::string describe();

  protected:
    // internal methods ////////////////////////////////////////////////////////

    //! Apply nms on 'in' into 'out'.
    virtual void process(bboxes &in, bboxes &out);
    //! Voting nms.
    virtual void vote_nms(bboxes &in, bboxes &out);

    void dfs(bboxes &bb, std::vector<bool> &explored, uint i, float match,
	     float max_center_dist, bboxes &comp);
    //! Merge all boxes that vote for the same location.
    void merge_votes(bboxes &bb);
    //! Prune overlapping votes.
    void prune_votes(bboxes &in, bboxes &out);

    // member variables ////////////////////////////////////////////////////////
  protected:
    nms   *tnms; //!< Also call traditional nms if allocated.
  };

} // end namespace ebl

#endif /* NMS_H_ */
