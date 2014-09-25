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

#include "nms.h"

using namespace std;

namespace ebl {

// nms ///////////////////////////////////////////////////////////////////////

nms::nms(float threshold_, float max_overlap_, float max_hcenter_dist_,
         float max_wcenter_dist_, float pre_hfact_, float pre_wfact_,
         float post_hfact_, float post_wfact_, float woverh_,
         std::ostream &out, std::ostream &err)
    : mout(out), merr(err), threshold(threshold_), max_overlap(max_overlap_),
      max_hcenter_dist(max_hcenter_dist_), max_wcenter_dist(max_wcenter_dist_),
      pre_hfact(pre_hfact_), pre_wfact(pre_wfact_),
      post_hfact(post_hfact_), post_wfact(post_wfact_),
      woverh(woverh_), same_class_matching_only(false) {
}

nms::~nms() {
}

void nms::fprop(bboxes &in, bboxes &out) {
  bboxes tmp = in;
  // apply pre-process factors
  tmp.scale_centered(pre_hfact, pre_wfact);
  // do nms
  this->process(tmp, out);
  // sort bboxes by confidence (most confident first)
  out.sort_by_confidence();
  // normalize boxes
  if (woverh != 1.0) out.normalize_widths(woverh);
  // apply post-process factors
  out.scale_centered(post_hfact, post_wfact);
  // threshold boxes
  out.threshold(threshold);
  MEDEBUG("Pruned " << in.size() << " bboxes to " << out.size() << " bboxes.");
}

std::string nms::describe() {
  std::string s;
  s << "Traditional overlap NMS, ignoring matching bboxes "
    << "(intersection/union) beyond "
    << "the max_overlap threshold (" << max_overlap << ") and centers "
    << "closer than " << max_hcenter_dist << " * height and "
    << max_wcenter_dist << " * width.";
  return s;
}

// internal methods //////////////////////////////////////////////////////////

void nms::process(bboxes &in, bboxes &out) {
  traditional_nms(in, out);
}

void nms::traditional_nms(bboxes &in, bboxes &out) {
  if (&in == &out) eblerror("in and out should be different objects");
  out.clear(); // reset output vector
  // for each bbox, check that matching with other bboxes does not go beyond
  // the maximum authorized matching score (0 means no overlap, 1 is identity)
  // and only keep ones with highest score when overlapping.
  // prune
  for (size_t ib = 0; ib < in.size(); ++ib) {
    if (!in.exists(ib)) continue ;
    bbox &bb = in[ib];
    if (bb.confidence < threshold)
      continue ; // ignore if lower than threshold
    bool add = true;
    // check each other bbox
    for (size_t jb = 0; jb < in.size() && add; ++jb) {
      if (!in.exists(jb)) continue ;
      bbox &bb2 = in[jb];
      if (&bb != &bb2) {
        bool overlap = false;

        // 	    // horizontal matching
        // 	    float hmatch = (std::min(bb.h0 + bb.height, bb2.h0 + bb2.height) -
        // 			    std::max(bb.h0, bb2.h0)) /
        // 	      (float) (std::max(bb.height, bb2.height));
        // 	    float wmatch = (std::min(bb.w0 + bb.width, bb2.w0 + bb2.width) -
        // 			    std::max(bb.w0, bb2.w0)) /
        // 	      (float) (std::max(bb.width, bb2.width));
        // 	    if (hmatch > .8 && wmatch > max_overlap)
        // 	      overlap = true;

        // box matching
        float match = 1 - bb.match(bb2);
        if (match <= max_overlap)
          overlap = true; // there is overlap

        // forbid centers to be closer than min dist to each other in
        // each axis
        if ((bb.center_hdistance(bb2) < max_hcenter_dist
             && bb.center_wdistance(bb2) < max_wcenter_dist)
            || (bb2.center_hdistance(bb) < max_hcenter_dist
                && bb2.center_wdistance(bb) < max_wcenter_dist))
          overlap = true;
        // // forbid centers to be closer than min dist to each other in
        // // each axis
        // // for boxes originating from the same scale.
        // // similar to applying stride on output.
        // if (bb.scale_index == bb2.scale_index &&
        // 	((bb.center_hdistance(bb2) < same_scale_mhd
        // 	  && bb.center_wdistance(bb2) < same_scale_mwd)
        // 	 || (bb2.center_hdistance(bb) < same_scale_mhd
        // 	     && bb2.center_wdistance(bb) < same_scale_mwd)))
        //   overlap = true;
        // if same_class_only, allow pruning only if 2 bb are same class
        bool allow_pruning = !same_class_matching_only ||
	    (same_class_matching_only && bb.class_id == bb2.class_id);
        // keep only 1 bb if overlap and pruning is ok
        if (overlap && allow_pruning) {
          if (bb.confidence < bb2.confidence) {
	    // if (bb.area() < bb2.area()) {
            // it's not the highest confidence, stop here.
            add = false;
            break ;
          } else if (bb.confidence == bb2.confidence) {
            // we have a tie, keep the biggest one.
            if (bb.height * bb.width > bb2.height * bb2.width)
              in.clear(jb);
            else {
              in.clear(ib);
              add = false;
              break ;
            }
          }
        }
      }
    }
    // if bbox survived, add it
    if (add)
      out.push_back(bb);
  }
}

//////////////////////////////////////////////////////////////////////////////
// voting nms

voting_nms::voting_nms(float threshold_, float max_overlap_,
                       float max_hcenter_dist_, float max_wcenter_dist_,
                       float pre_hfact_, float pre_wfact_,
                       float post_hfact_, float post_wfact_, float woverh_,
                       std::ostream &out, std::ostream &err)
    : nms(threshold_, max_overlap_, max_hcenter_dist_, max_wcenter_dist_,
	  pre_hfact_, pre_wfact_, post_hfact_, post_wfact_, woverh_,
	  out, err), tnms(NULL) {
}

voting_nms::voting_nms(float threshold_, float max_overlap_,
                       float max_hcenter_dist_, float max_wcenter_dist_,
                       float pre_hfact_, float pre_wfact_,
                       float post_hfact_, float post_wfact_, float woverh_,
                       float vote_max_overlap_,float vote_mhd, float vote_mwd,
                       std::ostream &out, std::ostream &err)
    : nms(threshold_, vote_max_overlap_, vote_mhd, vote_mwd,
	  pre_hfact_, pre_wfact_, post_hfact_, post_wfact_, woverh_,
	  out, err),
      tnms(new nms(threshold_, max_overlap_, max_hcenter_dist_,
		   max_wcenter_dist_, pre_hfact_, pre_wfact_, post_hfact_,
		   post_wfact_, woverh_, out, err)) {
}

voting_nms::~voting_nms() {
  if (tnms) delete tnms;
}

std::string voting_nms::describe() {
  std::string s;
  s << "Voting NMS, matching windows when overlap is less than "
    << max_overlap << " and centers are closer than "
    << max_hcenter_dist << " * height and "
    << max_wcenter_dist << " * width";
  if (tnms) s << ", followed by " << tnms->describe();
  return s;
}

void voting_nms::process(bboxes &in, bboxes &out) {
  bboxes tmp;
  vote_nms(in, tmp);
  if (tnms) tnms->traditional_nms(tmp, out);
  else out = tmp;
}

void voting_nms::vote_nms(bboxes &in, bboxes &out) {
  bboxes tmp = in;
  merge_votes(tmp);
  //prune_votes(tmp, out);
  out = tmp;
}

void voting_nms::dfs(bboxes &bbs, vector<bool> &explored,
                     uint i, float match, float max_center_dist,
                     bboxes &comp) {
  if (explored[i] == true)
    return ;
  explored[i] = true;
  bbox &bb = bbs[i];
  comp.push_back(bb);
  for (uint j = 0; j < bbs.size(); ++j) {
    if (i != j && explored[j] == false) {
      bbox &bb2 = bbs[j];
      bool add = true;
      // a box is matched if matching by at least 'match'
      if (bb.match(bb2) < match)
        add = false;
      // and if its center is within 'center_dist' of all other boxes
      // in this component
      float jrad = bb2.radius();
      for (uint k = 0; add == true && k < comp.size(); ++k) {
        bbox &kb = comp[k];
        float krad = kb.radius();
        float maxdist = std::min(krad, jrad) * max_center_dist;
        if (kb.i.center_distance(bb2.i) > maxdist)
          add = false;
      }
      if (add)
        dfs(bbs, explored, j, match, max_center_dist, comp);
    }
  }
}

// // TODO: optimization: track connected components as boxes are added (faster)
// // cf: http://en.wikipedia.org/wiki/Connected_component_(graph_theory)
// template <typename T, class Tstate>
// void detector<T,Tstate>::cluster_bboxes(float match, float max_center_dist,
// 					  bboxes &bbs,
// 					  float threshold) {
//   vector<bboxes > components;
//   vector<bool> explored(bbs.size(), false);
//   for (uint i = 0; i < bbs.size(); ++i)
//     if (explored[i] == false) {
// 	bboxes comp;
// 	dfs(bbs, explored, i, match, max_center_dist, comp);
// 	components.push_back(comp);
//     }
//   MEDEBUG("merged " << bbs.size() << " boxes into " << components.size()
// 	  << " component boxes");
//   // merge all components
//   bbs.clear();
//   for (uint i = 0; i < components.size(); ++i) {
//     bboxes &comp = components[i];
//     bbox *bb = comp[0];
//     bbox *b = new bbox(*bb);
//     b->mul(b->confidence);
//     delete bb;
//     for (uint j = 1; j < comp.size(); ++j) {
// 	bb = comp[j];
// 	b->accumulate(*bb);
// 	delete bb;
//     }
//     b->mul(1 / b->confidence);
//     if (b->confidence >= threshold)
// 	bbs.push_back(b);
//   }
// }

// TODO: use lists, more efficient than vector (especially 'erase')
void voting_nms::merge_votes(bboxes &bbs) {
  // vector of all bbox center distances
  typedef std::pair<uint,uint> t_upair;
  typedef std::pair<float,t_upair> t_pair;
  vector<t_pair> alldist;
  float dist = 0;
  for (uint i = 0; i < bbs.size(); ++i) {
    bbox &a = bbs[i];
    for (uint j = i + 1; j < bbs.size(); ++j) {
      bbox &b = bbs[j];
      dist = a.center_distance(b);
      alldist.push_back(t_pair(dist, t_upair(i,j)));
    }
  }
  // stop if no more pairs
  if (alldist.size() == 0)
    return ;
  // sort by lowest distance first
  std::sort(alldist.begin(), alldist.end());
  // merge all
  bool added = false;
  pair<float,pair<uint,uint> > p;
  for (uint k = 0; k < alldist.size(); ++k) {
    p = alldist[k];
    // ignore if one of the boxes doesn't exist anymore
    if (!bbs.exists(p.second.first) || !bbs.exists(p.second.second)) continue;
    bbox &a = bbs[p.second.first];
    bbox &b = bbs[p.second.second];
    // ignore if boxes don't match or too far away
    //      float maxdist = std::min(a.radius(), b.radius())
    //                      * max_center_dist;
    float maxhdist = std::max((a.height / 2) * max_hcenter_dist,
                              (b.height / 2) * max_hcenter_dist);
    float maxwdist = std::max((a.width  / 2) * max_wcenter_dist,
                              (b.width  / 2) * max_wcenter_dist);
    float hcenter_dist = std::fabs(a.hcenter() - b.hcenter());
    float wcenter_dist = std::fabs(a.wcenter() - b.wcenter());
    float match = 1 - a.match(b);
    //      if (p.first > maxdist || a.match(*b) < match)
    if (hcenter_dist > maxhdist || wcenter_dist > maxwdist
        || match > max_overlap)
      continue ;
    // don't merge if different classes
    if (a.class_id != b.class_id) continue ;

    // // take only first 50 TODO: TMP FIX
    // if (a.nacc > 25 || b.nacc > 25) {
    // 	// delete bbox with least acc
    // 	if (a.nacc > b.nacc) {
    // 	  delete b;
    // 	  bbs[p.second.second] = NULL;
    // 	} else {
    // 	  delete a;
    // 	  bbs[p.second.first] = NULL;
    // 	}
    // 	continue ;
    // }
    // merge the pair into a new box
    // first add boxes as children for debug (only add non-composed boxes)
    // if (a.children.size() == 0)
    // 	a.children.push_back(a);

    // cout << "merging box " << endl;
    // cout << a << endl;
    // cout << "and" << endl;
    // cout << b << endl;
    // cout << ":" << endl;


    // remember original boxes that create composite boxes
    if (a.children.size() == 0) // a is a true box, add it
      a.children.push_back_new(a);
    if (b.children.size() == 0) // b is a true box, add it
      a.children.push_back(b);
    else // add all children of b
      for (uint bi = 0; bi < b.children.size(); ++bi)
        a.children.push_back(b.children[bi]);
    // merge
    //if (a.nacc > 25 || b.nacc > 25)
    //MEDEBUG("merging box " << a << " and " << b << ":");
    a.mul((float) a.nacc);
    a.accumulate(b);
    a.mul(1 / (float) a.nacc);
    //if (a.nacc > 6 || b.nacc > 6)
    //MEDEBUG(a);
    EDEBUG(a);
    bbs.push_back(a);
    bbs.clear(p.second.first);
    bbs.clear(p.second.second);
    added = true;
  }
  // remove all empty entries in bboxes
  bbs.remove_empty();
  if (!added) return ;
  // recursively call until it stops (when alldist is empty)
  merge_votes(bbs);
}

void voting_nms::prune_votes(bboxes &in, bboxes &out) {
  // for each bbox, check that matching with other bboxes does not go beyond
  // the maximum authorized matching score (0 means no overlap, 1 is identity)
  // and only keep ones with highest score when overlapping.
  size_t ib, jb;
  for (ib = 0; ib < in.size(); ++ib) {
    if (!in.exists(ib)) continue ;
    bbox &bb = in[ib];
    // center of the box
    rect<float> this_bbox(bb.h0, bb.w0, bb.height, bb.width);
    bool add = true;
    bboxes overlaps;
    overlaps.push_back(bb);
    // check each other bbox
    for (jb = 0; jb < in.size() && add; ++jb) {
      bbox &bb2 = in[jb];
      if (&bb != &bb2) {
        rect<float> other_bbox(bb2.h0, bb2.w0, bb2.height, bb2.width);
        float match = this_bbox.match(other_bbox);
        bool overlap = false;
        if (match >= max_overlap)
          overlap = true; // there is overlap
        // 	    // forbid centers to be closer than min dist to each other in each axis
        // 	    if ((this_bbox.center_hdistance(other_bbox) < min_hcenter_dist
        // 		 && this_bbox.center_wdistance(other_bbox) < min_wcenter_dist)
        // 		|| (other_bbox.center_hdistance(this_bbox) < min_hcenter_dist
        // 		    && other_bbox.center_wdistance(this_bbox) < min_wcenter_dist))
        // 	      overlap = true;
        // if same_class_only, allow pruning only if 2 bb are same class
        bool allow_pruning = !same_class_matching_only ||
	    (same_class_matching_only && bb.class_id == bb2.class_id);
        // keep only 1 bb if overlap and pruning is ok
        if (overlap && allow_pruning) {
          overlaps.push_back(bb2);
        }
      }
    }
    // ********** TODO: delete non kept bboxes

    // // take mean of overlaps
    // bbox mean = mean_bbox(overlaps, .01, same_class_matching_only ?
    // 		      bb.class_id : classid);
    // bbox_parts *p = new bbox_parts(mean);
    // for (uint k = 0; k < overlaps.size(); ++k)
    //   p->add_part(*(overlaps[k]));
    // out.push_back(p);
  }
}

} // end namespace ebl
