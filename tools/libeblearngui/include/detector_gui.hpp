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

#ifndef DETECTOR_GUI_HPP_
#define DETECTOR_GUI_HPP_

using namespace std;

#include <deque>

namespace ebl {

// detector_gui ////////////////////////////////////////////////////////////////

template <typename T>
detector_gui<T>::
detector_gui(uint draw_extracted_, bool show_detqueue_, uint step_,
             uint qheight_, uint qwidth_, bool show_detqueue2_, uint step2_,
             uint qheight2_, uint qwidth2_, bool show_class_, bool show_conf_)
    : display_wid(-1), display_wid_fprop(-1), display_wid_gt(-1),
      draw_extracted(draw_extracted_),
      show_detqueue(show_detqueue_), show_detqueue2(show_detqueue2_),
      step(step_), step2(step2_), qheight(qheight_), qwidth(qwidth_),
      qheight2(qheight2_), qwidth2(qwidth2_), detcnt(0),
      show_class(show_class_), show_conf(show_conf_) {
  cout << "detector_gui: " << (draw_extracted > 0 ? "" : "not ")
       << "showing extracted windows." << endl;
}

template <typename T>
detector_gui<T>::~detector_gui() {
}

template <typename T> template <typename Tin>
bboxes& detector_gui<T>::
display(detector<T> &cl, idx<Tin> &img, const char *frame_name, int frame_id,
        uint h0, uint w0, double dzoom,
        T vmin, T vmax, int wid, const char *wname, float transparency) {
  display_wid = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector"));
  select_window(display_wid);

  // run network
  bboxes& vb = cl.fprop(img, frame_name, frame_id);
  display_minimal(img, vb, cl.labels, h0, w0, dzoom, vmin, vmax, display_wid,
                  false, transparency, show_class, show_conf);
  // draw masks class
  if (!mask_class.empty()) {
    idx<T> mask = cl.get_mask(mask_class);
    draw_mask(mask, h0, w0, dzoom, dzoom,
              255, 0, 0, 127, mask_threshold);
  }
  return vb;
}

template <typename T> template <typename Tin>
void detector_gui<T>::
display_minimal(idx<Tin> &img, bboxes& vb, vector<string> &labels, uint &h0,
                uint &w0, double dzoom,  T vmin, T vmax, int wid,
                bool show_parts, float transparency, bool show_class,
                bool show_conf, bboxes *bb2) {
  // draw image
  draw_matrix(img, h0, w0, dzoom, dzoom, (Tin)vmin, (Tin)vmax);
  // draw bboxes (in reverse order to display best on top)
  for (int i = vb.size() - 1; i >= 0; --i) {
    bbox &bb = vb[(uint) i];
    // // draw parts
    // if (show_parts && dynamic_cast<bbox_parts&>(bb))
    // 	draw_bbox_parts(((bbox_parts&) bb), labels, h0, w0, dzoom);
    // draw box
    draw_bbox(bb, labels, h0, w0, dzoom, transparency, true, 0, show_class,
              show_conf);
  }
  if (bb2) {
    for (int i = bb2->size() - 1; i >= 0; --i) {
      bbox &bb = (*bb2)[(uint) i];
      draw_bbox(bb, labels, h0, w0, dzoom, transparency, true, 1, show_class,
                show_conf);
    }
  }
  h0 += img.dim(0) + 5;
}

template <typename T> template <typename Tin>
bboxes& detector_gui<T>::
display_input(detector<T> &cl, idx<Tin> &img,
              const char *frame_name, int frame_id, uint h0, uint w0, double dzoom,
              T vmin, T vmax, int wid, const char *wname, float transparency) {
  display_wid = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector: output"));
  select_window(display_wid);
  //    disable_window_updates();
  bboxes &bb = display(cl, img, frame_name, frame_id, h0, w0, dzoom, vmin,
                       vmax, wid, wname, transparency);
  uint w = w0 + (uint) (img.dim(1) * dzoom + 5);
  // draw input
  draw_matrix(img, "input", h0, w, dzoom, dzoom, (Tin) vmin, (Tin) vmax);
  return bb;
}

template <typename T> template <typename Tin>
void detector_gui<T>::
display_groundtruth(detector<T> &cl, idx<Tin> &img,
                    bboxes &groundtruth, bboxes &filtered,
                    bboxes &nonfiltered, bboxes &pos,
                    bboxes &neg, svector<midx<T> > &pp_pos,
                    svector<midx<T> > &pp_neg,
                    uint h0, uint w0, double dzoom, T vmin, T vmax, int wid) {
  if (wid >= 0) display_wid_gt = wid;
  else if (display_wid_gt < 0)
    display_wid_gt = new_window("detector: groundtruth");
  select_window(display_wid_gt);
  disable_window_updates();
  clear_window();
  night_mode();
  uint w = w0, h = h0;
  // draw image
  draw_matrix(img, "all groundtruth", h, w, dzoom, dzoom);
  // draw all groundtruth boxes
  for (uint i = 0; i < groundtruth.size(); ++i) {
    bbox &gt = groundtruth[i];
    draw_bbox(gt, cl.labels, h, w, dzoom, 0, true, 3, false, false);
  }
  w += (uint) (img.dim(1) * dzoom + 5);
  // draw image
  draw_matrix(img, "positive bootstrapping", h, w, dzoom, dzoom);
  // draw filtered groundtruth boxes
  for (uint i = 0; i < filtered.size(); ++i) {
    bbox &gt = filtered[i];
    draw_bbox(gt, cl.labels, h, w, dzoom, 0, true, 5, false, false);
  }
  // draw gt-matched positives
  for (uint i = 0; i < pos.size(); ++i) {
    bbox &p = pos[i];
    draw_bbox(p, cl.labels, h, w, dzoom, 0, true, 6, false, false);
  }
  w += (uint) (img.dim(1) * dzoom + 5);
  // draw image
  draw_matrix(img, "negative bootstrapping", h, w, dzoom, dzoom);
  // draw filtered boxes
  for (uint i = 0; i < filtered.size(); ++i) {
    bbox &gt = filtered[i];
    draw_bbox(gt, cl.labels, h, w, dzoom, 0, true, 5, false, false);
  }
  // draw non-filtered boxes
  for (uint i = 0; i < nonfiltered.size(); ++i) {
    bbox &gt = nonfiltered[i];
    draw_bbox(gt, cl.labels, h, w, dzoom, 0, true, 3, false, false);
  }
  // draw negatives
  for (uint i = 0; i < neg.size(); ++i) {
    bbox &p = neg[i];
    draw_bbox(p, cl.labels, h, w, dzoom, 0, true, 7, false, false);
  }
  // draw positive preprocessed bootstrappings
  w = w0;
  h += (uint) (img.dim(0) * dzoom + 5);
  display_preprocessed(pp_pos, pos, cl.labels, h, w, dzoom, vmin, vmax);
  // draw positive preprocessed bootstrappings
  w = w0;
  display_preprocessed(pp_neg, neg, cl.labels, h, w, dzoom, vmin, vmax);
  enable_window_updates();
}

template <typename T> template <typename Tin>
bboxes& detector_gui<T>::
display_inputs_outputs(detector<T> &cl, idx<Tin> &img,
                       const char *frame_name, int frame_id,
                       uint h0, uint w0, double dzoom,
                       T vmin, T vmax, int wid, const char *wname,
                       T in_vmin, T in_vmax, float transparency, uint wmax) {
  display_wid_fprop = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
  select_window(display_wid_fprop);

  // draw input and output
  bboxes& bb =
      display_input(cl, img, frame_name, frame_id,
		    h0, w0, dzoom, in_vmin, in_vmax, display_wid_fprop, wname,
		    transparency);
  // compute min and max of all outputs, to maximize intensity display range
  bool first_time = true;
  if (vmin == vmax) {
    for (uint i = 0; i < cl.outputs.size(); ++i) {
      state<T> &o = cl.outputs[i];
      for (uint j = 0; j < o.x.size(); ++j) {
        idx<T> outx = o[j];
        if (first_time) {
          vmin = idx_min(outx);
          vmax = idx_max(outx);
          first_time = false;
        } else {
          vmin = MIN(vmin, idx_min(outx));
          vmax = std::max(vmax, idx_max(outx));
        }
      }
    }
  }
  EDEBUG("Displaying outputs in range [" << vmin << ", " << vmax
         << "], outputs actual range is [" << idx_min(cl.outputs[0])
         << ", " << idx_max(cl.outputs[0]) << "]");
  h0 += (uint) (cl.indim.dim(1) * dzoom + 20);
  // draw extracted windows
  if (draw_extracted == 1) { // preprocessed
    bboxes bbs;
    // display all preprocessed
    svector<midx<T> >& pp = cl.get_preprocessed(bbs);
    display_preprocessed(pp, bbs, cl.labels, h0, w0, dzoom, vmin, vmax, wmax);
    // display diverse preprocessed when saving
    if (cl.save_mode > 0 || cl.diverse_ordering) {
      svector<midx<T> >& fpp =
	  cl.get_preprocessed(bbs, cl.save_max_per_frame, cl.diverse_ordering);
      display_preprocessed(fpp, bbs, cl.labels, h0, w0, dzoom, vmin, vmax,
                           wmax);
    }
  }
  else if (draw_extracted == 2) { // originals
    vector<idx<T> >& pp = cl.get_originals();
    idx<T> m;
    if (pp.size() > 0) {
      m = pp[0];
      h0 += 15 * 3;
      uint wpp = w0, hpp = h0;
      gui << white_on_transparent() << at(h0 - 15 * 4, w0) << pp.size()
          << " positive windows with dimensions " << m;
      ostringstream o;
      o.precision(3);
      uint hmax = 0;
      for (uint i = 0; i < pp.size() && wpp < 5000; ++i) {
        hpp = h0;
        m = pp[i];
        bbox &b = bb[i];
        // print bbox infos
        o.str(""); o << b.confidence; gui << at(hpp - 15 * 3, wpp) << o.str();
        gui << at(hpp - 15 * 2, wpp) << b.iscale_index;
        gui << at(hpp - 15 * 1, wpp) << ((uint)b.class_id < cl.labels.size() ?
                                         cl.labels[b.class_id].c_str():"***");
        m = m.shift_dim(0, 2);
        draw_matrix(m, hpp, wpp, dzoom, dzoom, (T)0, (T)255);
        hpp += m.dim(0) + 1;
        wpp += m.dim(1) + 2;
        hmax = std::max(hmax, hpp);
      }
      h0 = hmax + 20;
    }
  }
  // display input states
  uint htmp = h0;
  display_inputs(cl, h0, w0, bb, dzoom, vmin, vmax, transparency);
  h0 = htmp;
  w0 += 20;
  // display output states
  display_outputs(cl, h0, w0, bb, dzoom, vmin, vmax, transparency);

  // display queues of detections
  if (show_detqueue || show_detqueue2) {
    uint hh0 = h0;
    vector<idx<T> > &new_detections = cl.get_originals();
    if (show_detqueue)
      update_and_display_queue(detqueue, step, qheight, qwidth,
                               new_detections, detcnt, hh0, w0, dzoom);
    if (show_detqueue2)
      update_and_display_queue(detqueue2, step2, qheight2, qwidth2,
                               new_detections, detcnt, hh0, w0, dzoom);
    detcnt += new_detections.size();
  }
  // reactive window drawing
  enable_window_updates();
  return bb;
}

template <typename T> void detector_gui<T>::
display_inputs(detector<T> &cl, uint &h0, uint &w0, bboxes &bb,
               double dzoom, T vmin, T vmax, float transparency) {
  uint wmax = 0;
  ostringstream s;
  // display all outputs
  for (int scale = 0; scale < (int) cl.ppinputs.size(); ) {
    state<T> &ins = cl.ppinputs[scale];
    for (uint i = 0; i < ins.x.size(); ++i) {
      idx<T> &t = ins.x[i];
      // draw inputs
      string ss;
      ss << "scale #" << scale;
      int htmp = h0;
      ss << " " << t.dim(1) << "x" << t.dim(2);
      idx<T> tx = t.shift_dim(0, 2);
      draw_matrix(tx, htmp, w0, dzoom, dzoom, (T)vmin, (T)vmax);
      wmax = std::max(wmax, (uint) (w0 + tx.dim(1)));
      // // loop on all boxes of this scale and this state#
      // for (bboxes::iterator q = bb.begin(); q != bb.end(); ++q) {
      // 	bbox &b = *q;
      // 	if (scale == b.iscale_index) {
      // 	  // if (i >= b.mi.size()) eblerror("expected as many states as boxes");
      // 	  // rect<float> r(htmp + b.mi[i].h0, w0 + b.mi[i].w0,
      // 	  // 		b.mi[i].height, b.mi[i].width);
      // 	  rect<float> r(htmp + b.mi[0].h0, w0 + b.mi[0].w0,
      // 			b.mi[0].height, b.mi[0].width);
      // 	  draw_box(r, 0, 0, 255);
      // 	  draw_cross(r.hcenter(), r.wcenter(), 3, 0, 0, 255, 255);
      // 	}
      // 	htmp += t.dim(1) + states_separation;
      // }

      gui << white_on_transparent() << at(h0 - 18, w0) << ss;
      // draw bboxes on scaled input
      if (!cl.bboxes_off) {
        for (bboxes::iterator ibb = bb.begin(); ibb != bb.end(); ++ibb) {
          if (scale == ibb->iscale_index)
            draw_bbox(*ibb, cl.labels, h0, w0, dzoom, transparency, false, 0,
                      show_class, show_conf);
          // draw all sub boxes
          for (bboxes::iterator jbb = ibb->children.begin();
               jbb != ibb->children.end(); ++jbb) {
            if (scale == jbb->iscale_index)
              draw_bbox(*jbb, cl.labels, h0, w0, dzoom, transparency, false,
                        1, show_class, show_conf);
          }
        }
      }
      scale++;
      h0 += (uint) (t.dim(1) * dzoom + 20);
    }
  }
  w0 = wmax;
}

template <typename T> void detector_gui<T>::
display_outputs(detector<T> &cl, uint &h0, uint &w0, bboxes &bb,
                double dzoom, T vmin, T vmax, float transparency) {
  uint h = h0, maxw = 0;
  uint color = 0;
  // draw answers
  for (uint l = 0; l < cl.answers.size(); ++l) {
    state<T> &answer = cl.answers[l];
    for (uint k = 0; k < answer.x.size(); ++k) {
      double czoom = dzoom;//* 2.0;
      idx<T> &o = answer.x[k];

      //uint lab = 0;
      idx<T> out0 = o.select(0, 0);
      idx<T> out1 = o.select(0, 1);
      string s;
      uint w = w0;
      s << "classes " << out0.dim(0) << "x" << out0.dim(1);
      gui << at((uint) (h - 20), (uint) w) << white_on_transparent() << s;
      draw_matrix(out0, h, w, czoom, czoom, idx_min(out0), idx_max(out0));
      // // draw crosses for found boxes
      // for (bboxes::iterator i = bb.begin(); i != bb.end(); ++i) {
      // 	color = 0;
      // 	if (k == (uint) i->oscale_index && (uint) i->class_id == lab)
      // 	  draw_cross(i->o.h0 * czoom + h0, i->o.w0 * czoom + w, 3,
      // 		     color_list[color][0], color_list[color][1],
      // 		     color_list[color][2]);
      w += (uint) (out0.dim(1) * czoom + 10);
      s = "";
      s << "conf " << out0.dim(0) << "x" << out0.dim(1);
      gui << at((uint) (h - 20), (uint) w) << white_on_transparent() << s;
      draw_matrix(out1, h, w, czoom, czoom, (T) 0, (T) 1);
      h += (uint) (out0.dim(0) * czoom + 20);
      w += (uint) (out0.dim(1) * czoom + 10);
      if (w > maxw) maxw = w;
    }
  }
  w0 = maxw;
  // draw outputs
  for (uint scale = 0; scale < cl.outputs.size(); ++scale) {
    state<T> &oo = cl.outputs[scale];
    for (uint k = 0; k < oo.x.size(); ++k) {
      double czoom = dzoom;// * 2.0;
      idx<T> &o = oo.x[k];
      uint lab = 0;
      idx<T> outx = o;
      uint w = w0;
      { idx_bloop1(category, outx, T) {
          string s;
          if (lab < cl.labels.size()) s << cl.labels[lab] << " ";
          else s << "feature " << lab << " ";
          s << category.dim(0) << "x" << category.dim(1);
          gui << at((uint) (h0 - 20), (uint) w) << white_on_transparent() <<s;
          draw_matrix(category, h0, w, czoom, czoom, vmin, vmax);
          // draw crosses for found boxes
          for (bboxes::iterator i = bb.begin(); i != bb.end(); ++i) {
            color = 0;
            if (scale + k == (uint) i->oscale_index
                && (uint) i->class_id == lab)
              draw_cross(i->o.h0 * czoom + h0, i->o.w0 * czoom + w, 3,
                         color_list[color][0], color_list[color][1],
                         color_list[color][2]);
            // draw children too
            color = 1;
            for (bboxes::iterator j = i->children.begin();
                 j != i->children.end(); ++j) {
              if (scale + k == (uint) j->oscale_index
                  && (uint) j->class_id == lab)
                draw_cross(j->o.h0 * czoom + h0, j->o.w0 * czoom + w, 3,
                           color_list[color][0], color_list[color][1],
                           color_list[color][2]);
            }
          }
          lab++;
          w += (uint) (outx.dim(2) * czoom + 10);
        }}
      h0 += (uint) (outx.dim(1) * czoom + 20);
    }
  }
}

template <typename T>
void detector_gui<T>::
display_preprocessed(svector<midx<T> > &pp, bboxes &bbs,
                     vector<string> &labels, uint &h0, uint &w0, double dzoom,
                     T vmin, T vmax, uint wmax) {
  if (pp.size() != bbs.size())
    eblerror("expected same size pp and bbs but got " << pp.size()
             << " and " << bbs.size());
  idx<T> l;
  if (pp.size() > 0) {
    midx<T> &m = pp[0];
    h0 += 15 * 3;
    uint w = w0, h = h0;
    gui << white_on_transparent() << at(h0 - 15 * 4, w0) << pp.size()
        << " positive windows with dimensions " << m.str();
    ostringstream o;
    o.precision(3);
    uint hmax = 0;
    for (uint i = 0; i < pp.size() && w < wmax; ++i) {
      h = h0;
      m = pp[i];
      bbox &b = bbs[i];
      // print bbox infos
      o.str(""); o << b.confidence; gui << at(h - 15 * 3, w) << o.str();
      gui << at(h - 15 * 2, w) << b.oscale_index;
      gui << at(h - 15 * 1, w)
          << ((uint) b.class_id < labels.size() ?
              labels[b.class_id].c_str() : "***");
      // // draw input box
      // rect<float> r(h + b.i.h0 - b.i0.h0,
      // 		w + b.i.w0 - b.i0.w0, b.i.height, b.i.width);
      // draw_box(r, 0, 0, 255);

      // loop on each layer of input to get maximum width
      int maxw = 0;
      for (uint j = 0; j < m.dim(0); ++j) {
        l = m.mget(j);
        maxw = std::max(maxw, (int) l.dim(2));
      }
      // draw all layers of preprocessed region
      for (uint j = 0; j < m.dim(0); ++j) {
        l = m.mget(j);
        int ww = std::max(0, (int) (w + (maxw - l.dim(2)) / 2));
        l = l.shift_dim(0, 2);
        draw_matrix(l, h, ww, dzoom, dzoom, (T)vmin, (T)vmax);
        rect<float> r(h, ww, l.dim(0), l.dim(1));
        draw_cross(r.hcenter(), r.wcenter(), 3, 0, 0, 255, 255);
        h += l.dim(0) + 1;
      }
      w += maxw + 2;
      hmax = std::max(h, hmax);
    }
    h0 = hmax + 20;
  }
}

template <typename T> void detector_gui<T>::
update_and_display_queue(deque<idx<T> > &queue, uint step, uint qheight,
                         uint qwidth, vector<idx<T> > &new_detections,
                         uint detcnt, uint &h0, uint &w0, double dzoom) {
  // update queue
  uint queuesz = qheight * qwidth;
  // loop over all new detections and add new ones based on the step
  for (typename vector<idx<T> >::iterator i = new_detections.begin();
       i != new_detections.end(); ++i, detcnt++) {
    if (!(detcnt % std::max((uint) 1, step))) { // add when multiple of step
      if ((queue.size() >= queuesz) && (queue.size() > 0))
        queue.pop_front();
      queue.push_back(*i);
    }
  }
  // display queue
  uint w = 0, wn = 0, h = 0;
  intg hmax = 0, wmax = 0;
  h = h0;
  for (typename deque<idx<T> >::iterator i = queue.begin();
       i != queue.end(); ++i) {
    draw_matrix(*i, h, w0 + w, dzoom, dzoom, (T)0, (T)255);
    w += i->dim(1) + 2;
    wn++;
    hmax = std::max(hmax, i->dim(0));
    wmax = std::max(wmax, (intg) w);
    if (wn >= qwidth) {
      wn = 0;
      w = 0;
      h += hmax + 2;
      hmax = 0;
    }
  }
  // update h0 and w0
  h0 += hmax;
  w0 += wmax;
}

template <typename T> template <typename Tin>
bboxes& detector_gui<T>::
display_all(detector<T> &cl, idx<Tin> &img,
            const char *frame_name, int frame_id, uint h0, uint w0, double dzoom,
            T vmin, T vmax, int wid, const char *wname) {
  display_wid_fprop = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
  select_window(display_wid_fprop);

  // draw input and output
  bboxes& bb =
      display_inputs_outputs(cl, img, frame_name, frame_id,
			     h0, w0, dzoom, vmin, vmax, display_wid_fprop);

  // disable_window_updates();
  // draw internal states of first scale
  w0 = (cl.indim.dim(2) + 5) * 2 + 5;
  module_1_1_gui mg;
  // cl.prepare(img);
  // cl.prepare_scale(0);
  mg.display_fprop(*(module_1_1<T>*) &cl.thenet,
                   *cl.input, cl.output, h0, w0, (double) 1.0,
                   (T) -1.0, (T) 1.0, true, display_wid_fprop);
  //    enable_window_updates();
  return bb;
}

template <typename T> template <typename Tin>
void detector_gui<T>::display_current(detector<T> &cl,
                                             idx<Tin> &sample,
                                             int wid, const char *wname,
                                             double dzoom){
  display_wid_fprop = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
  select_window(display_wid_fprop);
  disable_window_updates();
  clear_window();
  // draw internal states of first scale
  module_1_1_gui mg;
  cl.prepare(sample);
  cl.prepare_scale(0);
  mg.display_fprop(*(module_1_1<T>*) &cl.thenet,
                   *cl.input, cl.output, (uint) 0, (uint) 0, dzoom,
                   (T) -1.0, (T) 1.0, true, display_wid_fprop);
  enable_window_updates();
}

template <typename T>
void detector_gui<T>::set_mask_class(const char *name, T threshold) {
  if (name) {
    mask_class = name;
    mask_threshold = threshold;
  }
}

} // end namespace ebl

#endif
