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

#ifndef BOOTSTRAPPING_HPP_
#define BOOTSTRAPPING_HPP_

namespace ebl {

// bootstrapping ///////////////////////////////////////////////////////////////

template <typename T>
bootstrapping<T>::bootstrapping(configuration &conf_)
    : conf(conf_), silent(false) {
  gt_path = conf.try_get_string("gt_path", "");
  silent = conf.exists_true("silent");
  extract_pos = conf.exists_true("gt_extract_pos");
  extract_neg = conf.exists_true("gt_extract_neg");
  _activated = conf.exists_true("bootstrapping");
  _max_size = conf.try_get_intg("bootstrapping_max", limits<intg>::max());
  if (_max_size == 0) _max_size = limits<intg>::max();
  neg_gt_only = conf.exists_true("gt_neg_gt_only");
  neg_threshold = (T) conf.try_get_double("gt_neg_threshold", .0001);
  if (conf.exists("bbox_scalings"))
    bbox_scalings =string_to_fidxdimvector(conf.get_cstring("bbox_scalings"));
  mirror_pos = conf.exists_true("gt_mirror_pos");
}

template <typename T>
bootstrapping<T>::~bootstrapping() {
}

template <typename T>
void bootstrapping<T>::fprop(detector<T> &detect, std::string &fname,
                             bool reset, double scale) {
  if (reset) clear();
  try {
    try {
      // load groundtruth data
      gt_all = load_groundtruth(fname, detect.labels);
      gt_clean = load_clean_groundtruth(fname, conf, gt_rest, &detect.labels);
      // negs do not need groundtruth data
      if (extract_neg)
        bbneg = get_negatives
	    (detect.answers, gt_clean, gt_rest, detect.itl, detect.itr,
	     detect.ibl, detect.ibr,
	     conf.try_get_float("gt_neg_matching", .5),
	     conf.try_get_float("gt_negneg_matching", .5),
	     conf.try_get_uint("gt_neg_max", 5), detect.bgclass, neg_threshold);
      // get bootstrapping boxes
      if (extract_pos && gt_clean.size() > 0) {
        // scale groundtruth (for positives only)
        if (scale != 1.0) {
          gt_all.scale_centered(scale, scale);
          gt_clean.scale_centered(scale, scale);
        }
        bbpos = get_positives
	    (detect.outputs, gt_clean, detect.itl, detect.itr,
	     detect.ibl, detect.ibr,
	     conf.try_get_float("gt_pos_matching", .5),
	     conf.try_get_float("gt_min_context", 1.0));
      }
    } eblcatchwarn();
    // get preprocessed bootstrappings
    bboxes ppbout;
    spos = detect.get_preprocessed(bbpos, ppbout);
    sneg = detect.get_preprocessed(bbneg, ppbout);
    if (mirror_pos) add_mirrors(spos, bbpos);
    // fuse all results
    bball.push_back(bbpos);
    bball.push_back(bbneg);
    sall.push_back(spos);
    sall.push_back(sneg);
  } eblcatchwarn();
}

template <typename T>
void bootstrapping<T>::clear() {
  sall.clear();
  sneg.clear();
  spos.clear();
  gt_rest.clear();
  gt_clean.clear();
  gt_all.clear();
  bball.clear();
  bbneg.clear();
  bbpos.clear();
}

// accessors /////////////////////////////////////////////////////////////////

template <typename T>
bboxes& bootstrapping<T>::get_bbpos() {
  return bbpos;
}

template <typename T>
bboxes& bootstrapping<T>::get_bbneg() {
  return bbneg;
}

template <typename T>
bboxes& bootstrapping<T>::get_bball() {
  return bball;
}

template <typename T>
bboxes& bootstrapping<T>::get_gtall() {
  return gt_all;
}

template <typename T>
bboxes& bootstrapping<T>::get_gtclean() {
  return gt_clean;
}

template <typename T>
bboxes& bootstrapping<T>::get_gtrest() {
  return gt_rest;
}

template <typename T>
svector<midx<T> >& bootstrapping<T>::get_pos() {
  return spos;
}

template <typename T>
svector<midx<T> >& bootstrapping<T>::get_neg() {
  return sneg;
}

template <typename T>
svector<midx<T> >& bootstrapping<T>::get_all() {
  return sall;
}

template <typename T>
bool bootstrapping<T>::activated() {
  return _activated;
}

template <typename T>
bool bootstrapping<T>::extract_positives() {
  return extract_pos;
}

template <typename T>
bool bootstrapping<T>::groundtruth_found(std::string &frame_name) {
  std::string s = groundtruth_file(frame_name);
  return file_exists(s);
}

template <typename T>
bool bootstrapping<T>::skip_frame(std::string &frame_name) {
  if (!_activated) return false;
  bool gtfound = groundtruth_found(frame_name);
  if (_activated && !extract_neg && !gtfound) {
    if (!gtfound)
      std::cout << "groundtruth not found in "
                << groundtruth_file(frame_name) << std::endl;
    return true;
  }
  if (extract_neg && neg_gt_only && !gtfound) {
    std::cout << "frame skipped because no groundtruth found" << std::endl;
    return true; // extract negatives but only in images with groundtruth
  }
  if (gtfound) {
    // groundtruth is found, let's check how many objects are filtered
    bboxes b, rest;
    b = load_clean_groundtruth(frame_name, conf, rest);
    if (b.size() == 0 && !extract_neg) {
      std::cout << "Skipping frame " << frame_name
                << " because no clean objects were found." << std::endl;
      return true;
    }
  }
  return false;
}

template <typename T>
intg bootstrapping<T>::max_size() {
  return _max_size;
}

// dataset methods ///////////////////////////////////////////////////////////

template <typename T>
void bootstrapping<T>::save_dataset(svector<midx<T> > &samples,
                                           bboxes &bb, std::string &outdir,
                                           idx<ubyte> &classes) {
  if (samples.size() != bb.size())
    eblerror("expected same number of samples and boxes but got "
             << samples.size() << " and " << bb.size());
  if (samples.size() == 0) eblerror("expected at least 1 sample");
  // find max number of submatrices
  intg order = samples[0].order(), dim = samples[0].dim(0);
  if (order != 1) eblerror("expected order 1");
  for (intg i = 0; i < (intg) samples.size(); ++i) {
    if (samples[i].order() != order)
      eblerror("expected same order for all samples but found "
               << samples[i].order() << " and " << order);
    if (samples[i].dim(0) > dim) dim = samples[i].dim(0);
  }
  std::cout << "Max number of sub-matrices for each sample is: "
            << dim << std::endl;
  // create a single midx with all samples
  midx<T> all(samples.size(), dim);
  idx<intg> scales(samples.size()), labels(samples.size());
  all.clear();
  idx_clear(scales);
  // loop on samples
  for (intg i = 0; i < (intg) samples.size(); ++i) {
    midx<T> &sample = samples[i];
    bbox &b = bb[i];
    // loop on sample's submatrix
    for (intg j = 0; j < sample.dim(0); ++j) {
      idx<T> layer = sample.mget(j);
      all.mset(layer, i, j);
    }
    // set scale & label
    scales.set(b.oscale_index, i);
    labels.set(b.class_id, i);
  }
  // save files
  std::string name;
  name << "bootstrapping_" << conf.get_string("gt_name");
  std::string data_fname, scales_fname, labels_fname, classes_fname;
  std::cout << "saving dataset..." << std::endl;
  data_fname << outdir << "/" << name << "_" << DATA_NAME << MATRIX_EXTENSION;
  save_matrices(all, data_fname);
  std::cout << "saved " << data_fname << " (" << all << ")" << std::endl;
  scales_fname << outdir << "/" << name << "_" << SCALES_NAME
               << MATRIX_EXTENSION;
  save_matrix(scales, scales_fname);
  std::cout << "saved " << scales_fname << " (" << scales << ")" << std::endl;
  labels_fname << outdir << "/" << name << "_" << LABELS_NAME
               << MATRIX_EXTENSION;
  save_matrix(labels, labels_fname);
  std::cout << "saved " << labels_fname << " (" << labels << ")" << std::endl;
  classes_fname << outdir << "/" << name << "_" << CLASSES_NAME
                << MATRIX_EXTENSION;
  save_matrix(classes, classes_fname);
  std::cout << "saved " << classes_fname << " (" << classes << ")"
            << std::endl;
}

// internal methods //////////////////////////////////////////////////////////

template <typename T>
std::string bootstrapping<T>::groundtruth_file(std::string &frame_name) {
  std::string xml = "";
  xml << gt_path << "/" << noext_name(frame_name.c_str()) << "ml";
  return xml;
}

template <typename T>
bboxes bootstrapping<T>::load_groundtruth(std::string &frame_name,
                                          std::vector<std::string> &labels) {
  bboxes gt;
  // look for xml version
  xml_fullname = groundtruth_file(frame_name);
  if (file_exists(xml_fullname)) {
    if (!silent)
      std::cout << "Found groundtruth file: " << xml_fullname << std::endl;
    gt = pascal_xml::get_bboxes(xml_fullname, labels);
    EDEBUG("groundtruth boxes: " << gt);
  } else eblwarn("groundtruth file not found " << xml_fullname);
  return gt;
}

template <typename T>
bboxes bootstrapping<T>::load_clean_groundtruth
(std::string &frame_name, configuration &conf, bboxes &rest,
 std::vector<std::string> *labels) {
  rest.clear();
  // load filtering settings
  float minvisibility = conf.try_get_float("gt_minvisibility", 0);
  float min_ar = conf.try_get_float("gt_min_aspect_ratio", 0);
  float max_ar = conf.try_get_float("gt_max_aspect_ratio", 1);
  idxdim mindims(1, 1), minborders;
  std::vector<std::string> included;
  if (conf.exists("gt_mindims"))
    mindims = string_to_idxdim(conf.get_cstring("gt_mindims"));
  if (conf.exists("gt_minborders"))
    minborders = string_to_idxdim(conf.get_cstring("gt_minborders"));
  if (conf.exists("gt_included"))
    included = string_to_stringvector(conf.get_string("gt_included"));
  bboxes gt;
  // look for xml version
  xml_fullname = groundtruth_file(frame_name);
  if (file_exists(xml_fullname)) {
    if (!silent)
      std::cout << "Found groundtruth file: " << xml_fullname << std::endl
                << "Filtering based on min visibility " << minvisibility
                << ", aspect ratio min " << min_ar << " max " << max_ar
                << ", mindims " << mindims << ", minborders " << minborders
                << ", included classes: " << included << std::endl;
    gt = pascal_xml::get_filtered_bboxes(xml_fullname, minvisibility, min_ar,
                                         max_ar, mindims, minborders,
                                         included, rest, labels);
    EDEBUG("clean groundtruth boxes: " << gt);
  } else eblwarn("groundtruth file not found " << xml_fullname);
  return gt;
}

template <typename T>
bboxes bootstrapping<T>::get_positives
(svector<state<T> > &outputs, bboxes &groundtruth,
 svector<mfidxdim> &topleft, svector<mfidxdim> &topright,
 svector<mfidxdim> &bottomleft, svector<mfidxdim> &bottomright,
 float matching, float mincontext) {
  bboxes res;
  float min_overlap = .9;
  float closest_overlap = 0, closest_overlap2 = 0, closest_match = 0;
  uint closest_overlap_scale = 0, closest_overlap2_scale = 0,
      closest_match_scale = 0;
  std::cout << "groundtruth: " << groundtruth << std::endl;
  // loop on groundtruth boxes
  for (bboxes::iterator i = groundtruth.begin(); i != groundtruth.end(); ++i){
    bbox &gtraw = *i;
    bboxes candidates;
    // loop on outputs maps
    for (uint oo = 0; oo < outputs.size(); ++oo) {
      state<T> &output = outputs[oo];
      for (uint o = 0; o < output.x.size(); ++o) {
        idx<T> &out = output.x[o];
        // input space corners
        fidxdim &tl = topleft[oo][o], &tr = topright[oo][o],
	    &bl = bottomleft[oo][o];
        // steps in input space
        double hf = (bl.offset(1) - tl.offset(1)) / out.dim(1);
        double wf = (tr.offset(2) - tl.offset(2)) / out.dim(2);
        // box size for this map
        bbox b(0, 0, tl.dim(1), tl.dim(2));
        b.o.height = 1;
        b.o.width = 1;
        // normalize width of gt
        bbox gt = gtraw;
        gt.scale_width(b.width / b.height);
        bbox gtcontext = gt;
        gtcontext.scale_centered(mincontext, mincontext);
        if (gt.class_id >= out.dim(0))
          eblerror("expected dim 0 of " << out << " to be > than "
                   << gt.class_id);
        // // box has to be able to include groundtruth entirely, otherwise skip
        // rect<float> gt2(0, 0, gt.height, gt.width);
        // float gt2_overlap = gt2.overlap_ratio(b);
        // if (gt2_overlap > closest_overlap) {
        //   closest_overlap = gt2_overlap;
        //   closest_overlap_scale = o;
        // }
        // if (gt2_overlap < 1.0) {
        //   EDEBUG("ruling out scale " << o << " because box " << b
        //   << " doesn't overlap completely with gt " << gt2);
        //   continue ;
        // }
        // apply scalings to box
        float hscale = 1, wscale = 1;
        if (o < bbox_scalings.size()) {
          fidxdim &scaling = bbox_scalings[o];
          hscale = scaling.dim(0);
          wscale = scaling.dim(1);
        }
        // loop on width
        int wmax = (int) ceil((gt.w0 + gt.width) / wf);
        int w = (int) std::max((double)0, floor((gt.w0 - b.width/2
                                                 - tl.offset(2)) / wf));
        // force minimum region to explore to at least 4 pixels
        int wadd = std::max(wmax - w, 2);
        w -= wadd;
        wmax += wadd;
        for ( ; w < wmax; ++w) {
          // loop on height
          int hmax = (int) ceil((gt.h0 + gt.height) / hf);
          int h = (int) std::max((double)0, floor((gt.h0 - b.height/2
                                                   - tl.offset(1)) / hf));
          // force minimum region to explore to at least 4 pixels
          int hadd = std::max(hmax - h, 2);
          h -= hadd;
          hmax += hadd;
          for ( ; h < hmax; ++h) {
            b.h0 = tl.offset(1) + h * hf;
            b.w0 = tl.offset(2) + w * wf;
            bbox b2 = b;
            b2.scale_centered(hscale, wscale);
            EDEBUG("scale " << o << " gt " << (rect<float>&)gt
                   << "gtcontext " << (rect<float>&)gtcontext << " b "
                   << (rect<float>&)b2 << " matching: " << b2.match(gtcontext));
            // skip this box if not matching gt more than minimum match
            float bmatch = b2.match(gt);
            if (bmatch > closest_match) {
              closest_match = bmatch;
              closest_match_scale = o;
            }
            //if (o > 0 && o < outputs.size() -1 && bmatch < matching) continue ;
            if (bmatch < matching) continue ;

            // skip this box if not including gt entirely
            float gtoverlap = gtcontext.overlap_ratio(b);
            if (gtoverlap > closest_overlap2) {
              closest_overlap2 = gtoverlap;
              closest_overlap2_scale = o;
            }
            EDEBUG("overlap ratio "<<gtoverlap << " minoverlap "<< min_overlap);
            //if (o < outputs.size() - 1 && gtoverlap < min_overlap) continue ;
            if (gtoverlap < min_overlap) continue ;

            // bbox is a potential candidate, add it
            // // use output value as confidence
            // b.confidence = (float) out.get(gt.class_id, h, w);
            // use matching value as confidence
            b.confidence = bmatch;
            // b.iscale_index = scale_indices[a]; // scale index
            // b.oscale_index = a; // scale index
            // b.i.h0 = ptl.offset(1) + h * phf;
            // b.i.w0 = ptl.offset(2) + w * pwf;
            // b.i.height = ptl.dim(1);
            // b.i.width = ptl.dim(2);
            b.o.h0 = h; // answer height in output
            b.o.w0 = w; // answer height in output
            b.class_id = gt.class_id;
            b.iscale_index = o;
            b.oscale_index = o;
            b.output_index = oo;
            EDEBUG("adding candidate " << (rect<float>&)b);
            candidates.push_back_new(b);
          }
        }
      }
    }
    EDEBUG("candidates: " << candidates);
    // pick highest score candidates
    bboxes highests = candidates.get_most_confidents();
    // out of all highest, pick one with closest center to groundtruth
    float dist = limits<float>::max();
    int best = -1;
    for (uint i = 0; i < highests.size(); ++i) {
      if (highests[i].center_distance(gtraw) < dist) {
        best = (int) i;
        dist = highests[i].center_distance(gtraw);
      }
    }
    if (best >= 0) res.push_back_new(highests[best]);
#ifdef __DEBUG__
    std::cerr << "candidate with strongest confidence for grountruth " << gtraw
         << " is: ";
    if (best >= 0) std::cerr << highests[best] << std::endl;
    else std::cerr << "none" << std::endl;
#endif
    if (best < 0)
      eblwarn("no positive sample found for groundtruth bbox " << gtraw
              << " in file " << xml_fullname
              << ", highest overlap " << closest_overlap
              << " with scale " << closest_overlap_scale
              << ", highest overlap2 " << closest_overlap2
              << " with scale " << closest_overlap2_scale
              << " (min overlap " << min_overlap << ")"
              << ", closest matching " << closest_match
              << " with scale " << closest_match_scale
              << " (min match " << matching << ")");
    std::cout << "best: " << best << " gt " << gtraw << std::endl;
  }
  EDEBUG("positive bootstraps: " << res);
  return res;
}

template <typename T>
bboxes bootstrapping<T>::get_negatives
(svector<state<T> > &answers, bboxes &filtered, bboxes &nonfiltered,
 svector<mfidxdim> &topleft, svector<mfidxdim> &topright,
 svector<mfidxdim> &bottomleft, svector<mfidxdim> &bottomright,
 float matching, float neg_matching, uint nmax, int neg_id, T threshold) {
  bboxes res;
  // extract all non-negative windows
  // loop on outputs maps
  bboxes pos2, res2;
  for (uint oo = 0; oo < answers.size(); ++oo) {
    state<T> &answer = answers[oo];
    for (uint o = 0; o < answer.x.size(); ++o) {
      idx<T> &ans = answer.x[o];
      // input space corners
      fidxdim &tl = topleft[oo][o], &tr = topright[oo][o],
	  &bl = bottomleft[oo][o];
      // steps in input space
      double hf = (bl.offset(1) - tl.offset(1)) / ans.dim(1);
      double wf = (tr.offset(2) - tl.offset(2)) / ans.dim(2);
      // box size for this map
      bbox b(0, 0, tl.dim(1), tl.dim(2));
      b.o.height = 1;
      b.o.width = 1;
      // loop on width
      for (uint w = 0; w < ans.dim(2); ++w) {
        // loop on height
        for (uint h = 0; h < ans.dim(1); ++h) {
          b.class_id = (int) ans.get(0, h, w);
          // ignore negative answers
          if (b.class_id == neg_id) continue ;
          b.confidence = (float) ans.get(1, h, w);
          // confidence is below threshold, ignore
          if (b.confidence < threshold) continue ;
          // not negative, enqueue
          b.h0 = tl.offset(1) + h * hf;
          b.w0 = tl.offset(2) + w * wf;
          b.o.h0 = h; // answer height in output
          b.o.w0 = w; // answer height in output
          b.oscale_index = o;
          b.output_index = oo;
          pos2.push_back_new(b);
        }
      }
    }
  }
  // sort positives by confidence
  pos2.sort_by_confidence();
  // extract nmax most confident
  for (uint i = 0; i < pos2.size() && res2.size() < nmax; ++i) {
    bbox &b = pos2[i];
    bbox b2 = b;
    // apply scalings to box
    float hscale = 1, wscale = 1;
    int off = std::min(b2.output_index, (int) bbox_scalings.size() - 1);
    if (off >= 0) {
      fidxdim &scaling = bbox_scalings[off];
      hscale = scaling.dim(0);
      wscale = scaling.dim(1);
    }
    b2.scale_centered(hscale, wscale);
    bool accept = true;
    // check that b doesn't overlap more than matching with filtered gt
    for (bboxes::iterator j = filtered.begin();j != filtered.end();++j){
      if (b.class_id == j->class_id && b2.match(*j) > matching) {
        accept = false;
        break ;
      }
    }
    if (!accept) continue ;
    // check that b doesn't overlap at all with non-filtered gt
    for (bboxes::iterator j = nonfiltered.begin();
         j != nonfiltered.end(); ++j) {
      if (b.overlap(*j)) {
        accept = false;
        break ;
      }
    }
    if (!accept) continue ;
    // check that b doesn't overlap at all with other accepted positives
    for (bboxes::iterator j = res2.begin();j != res2.end();++j){
      if (b.match(*j) > neg_matching) {
        accept = false;
        break ;
      }
    }
    if (!accept) continue ;
    // all checks passed, keep this box
    if (accept) res2.push_back(b);
  }
  res.push_back(res2);
  //    if (res2.size() == 0) eblwarn("no negatives found for scale " << o);
  // set all boxes to negative id
  for (bboxes::iterator j = res.begin(); j != res.end(); ++j)
    j->class_id = neg_id;
  EDEBUG("negative bootstraps: " << res);
  return res;
}

template <typename T>
void bootstrapping<T>::add_mirrors(svector<midx<T> > &samples, bboxes &boxes) {
  svector<midx<T> > mirrors;
  bboxes bmirrors;
  for (uint i = 0; i < samples.size(); ++i) {
    bbox &b = boxes[i];
    midx<T> &s = samples[i];
    midx<T> flipped = idx_flip(s, 2);
    mirrors.push_back_new(flipped);
    bmirrors.push_back_new(b);
  }
  samples.push_back(mirrors);
  boxes.push_back(bmirrors);
}

} // end namespace ebl

#endif /* BOOTSTRAPPING_HPP_ */
