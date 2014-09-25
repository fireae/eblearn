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

#ifndef DETECTION_THREAD_HPP_
#define DETECTION_THREAD_HPP_

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>

#include "libeblearn.h"
#include "libeblearntools.h"
#include "pascal_xml.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

namespace ebl {

// detection thread ////////////////////////////////////////////////////////////

template <typename T>
detection_thread<T>::
detection_thread(configuration &conf_, mutex *om, const char *name_,
                 const char *arg2_, bool sync, t_chans tc)
    : thread(om, name_, sync), conf(conf_), arg2(arg2_), frame(120, 160, 1),
      mutex_in(), mutex_out(),
      in_updated(false), out_updated(false), bavailable(false), bfed(false),
      frame_name(""), frame_id(0), outdir(""), total_saved(0), color_space(tc),
      silent(false), boot(conf), frame_skipped(false),
      frame_loaded(false), pdetect(NULL) {
  silent = conf.exists_true("silent");
  outdir = get_output_directory(conf);
  mout << "Saving outputs to " << outdir << std::endl;
}

template <typename T>
detection_thread<T>::~detection_thread() {
}

template <typename T>
void detection_thread<T>::execute() {
  try {
    bool	display        = false;
#ifdef __GUI__
    display = conf.exists_true("display")
        && conf.exists_true("display_threads");
    bool	mindisplay     = conf.exists_true("minimal_display");
    bool       save_video     = conf.exists_true("save_video");
    bool	display_states = conf.exists_true("display_states");
    uint       wid	       = 0;	// window id
    uint       wid_states     = 0;	// window id
#endif
    uint	display_sleep  = conf.try_get_uint("display_sleep", 0);
    //      if (!display && save_video) {
    //        // we still want to output images but not show them
    //        display = true;
    // #ifdef __GUI__
    //        set_gui_silent();
    // #endif
    //      }
    // load network and weights in a forward-only parameter
    parameter<T> theparam;
    theparam.set_forward_only();
    idx<ubyte> classes(1,1);
    //try { // try loading classes names but do not stop upon failure
    load_matrix<ubyte>(classes, conf.get_cstring("classes"));
    // } catch(std::string &err) {
    //   merr << "warning: " << err;
    //   merr << std::endl;
    // }
    std::vector<std::string> sclasses = ubyteidx_to_stringvector(classes);
    answer_module<T> *ans = create_answer<T,T,T>(conf, classes.dim(0));
    uint noutputs = ans->get_nfeatures();
    intg thick = -1;
    module_1_1<T> *net = create_network<T>(theparam, conf, thick, noutputs,
                                           "arch", this->_id);
    // loading weights
    if (conf.exists("weights")) { // manual weights
      // concatenate weights if multiple ones
      std::vector<std::string> w =
          string_to_stringvector(conf.get_string("weights"));
      mout << "Loading weights from: " << w << std::endl;
      theparam.load_x(w);
      // permute weights by blocks
      if (conf.exists("weights_permutation")) {
        std::string sblocks = conf.get_string("weights_blocks");
        std::string spermut = conf.get_string("weights_permutation");
        std::vector<intg> blocks = string_to_intgvector(sblocks.c_str());
        std::vector<uint> permut = string_to_uintvector(spermut.c_str());
        theparam.permute_x(blocks, permut);
      }
    } else {
      if (conf.exists_true("manual_load")) { // manual load
        eblwarn("\"weights\" variable not defined, loading manually "
                << "if manual_load defined");
        manually_load_network(*((layers<T>*)net), conf);
      } else { // random weights
        int seed = dynamic_init_drand();
        eblwarn("No weights to load, randomizing weights with seed " << seed);
        forget_param_linear fgp(1, 0.5, seed);
        net->forget(fgp);
      }
    }
    DEBUGMEM_PRETTY("before detection");
    // detector
    detector<T> detect(*net, sclasses, ans, NULL, NULL, mout, merr);
    init_detector(detect, conf, outdir, silent);
    // keep pointer to detector
    pdetect = &detect;
    bootstrapping<T> boot(conf);

    // when a bbox file is given, ignore the processing, load the pre-computed
    // bboxes and feed them to the nms (non-maximum suppression).
    bboxes boxes(bbox_all, NULL, mout, merr);
    boxes.print_saving_type(); // inform user how we save boxes
    bool precomputed_boxes = false;
    if (conf.exists("bbox_file")) {
      precomputed_boxes = true;
      std::string bbfile = conf.get_string("bbox_file");
      boxes.load_eblearn(bbfile);
    }
    bool bmask_class = false;
    if (conf.exists("mask_class"))
      bmask_class = detect.set_mask_class(conf.get_cstring("mask_class"));

    std::string viddir = outdir;
    viddir += "video/";
    mkdir_full(viddir);
    // gui
#ifdef __GUI__
    uint display_wmax = conf.try_get_uint("display_max_width", 3000);
    T display_min = (T) conf.try_get_double("display_min", -1.7);
    T display_max = (T) conf.try_get_double("display_max", 1.7);
    T display_in_max = (T) conf.try_get_double("display_in_max", 255);
    T display_in_min = (T) conf.try_get_double("display_in_min", 0);
    float display_transp = conf.try_get_float("display_bb_transparency", 0);
    uint qstep1 = conf.try_get_uint("qstep1", 0);
    uint qheight1 = conf.try_get_uint("qheight1", 0);
    uint qwidth1 = conf.try_get_uint("qwidth1", 0);
    uint qstep2 = conf.try_get_uint("qstep2", 0);
    uint qheight2 = conf.try_get_uint("qheight2", 0);
    uint qwidth2 = conf.try_get_uint("qwidth2", 0);
    module_1_1_gui	netgui;
    wid_states  = display_states ? new_window("network states"):0;
    night_mode();
    std::string title = "EBLearn detector: ";
    title += _name;
    if (display) {
      wid = new_window(title.c_str());
      mout << "displaying in window " << wid << std::endl;
      night_mode();
    }
    float zoom = conf.try_get_float("display_zoom", 1);
    bool bbox_show_conf = !conf.exists_false("bbox_show_conf");
    bool bbox_show_class = !conf.exists_false("bbox_show_class");
    detector_gui<T>
        dgui(conf.try_get_uint("show_extracted", 0),
             conf.exists_bool("queue1"), qstep1, qheight1,
             qwidth1, conf.exists_bool("queue2"), qstep2, qheight2, qwidth2,
             bbox_show_class, bbox_show_conf);
    if (bmask_class)
      dgui.set_mask_class(conf.get_cstring("mask_class"),
                          (T) conf.try_get_double("mask_threshold", 0));
#endif
    // timing variables
    timer tpass, toverall;
    long ms;
    // loop
    toverall.start();
    // we're ready
    bavailable = true;
    while(!this->_stop) {
      // wait until a new image is made available
      while (!in_updated && !_stop) {
        millisleep(1);
      }
      tpass.restart();
      if (_stop) break ;
      // we got a new frame, reset new frame flag
      in_updated = false; // no need to lock mutex
      // check if this frame should be skipped
      if (boot.skip_frame(frame_name)) {
        skip_frame();
        continue ;
      } else if (!frame_loaded) {
        uframe = load_image<ubyte>(frame_fullname);
        mout << "loaded image " << frame_fullname << std::endl;
      }
      if (!silent) mout << "processing " << frame_name << std::endl;
      // check frame is correctly allocated, if not, allocate.
      if (frame.order() != uframe.order())
        frame = idx<T>(uframe.get_idxdim());
      else if (frame.get_idxdim() != uframe.get_idxdim())
        frame.resize(uframe.get_idxdim());
      // copy frame
      idx_copy(uframe, frame);
      // run detector
      if (!display) { // fprop without display
        if (precomputed_boxes) {
          try {
            bboxes *bb = boxes.get_group(frame_name);
            idxdim d = boxes.get_group_dims(frame_name);
            d.insert_dim(0, 1);
            bboxes pruned;
            detect.init(d);
            detect.fprop_nms(*bb, pruned);
            copy_bboxes(pruned); // make a copy of bounding boxes
            // resize frame so that caller knows the size of the frame
            idxdim framedim = frame.get_idxdim();
            if (d.dim(1) == -1 || d.dim(2) == -1)
              eblerror("pre-computed boxes must contain full image size, "
                       << "but found: " << d);
            framedim.setdim(0, d.dim(1));
            framedim.setdim(1, d.dim(2));
            frame.resize(framedim);
          } catch(eblexception &e) {
#ifdef __NOEXCEPTIONS__
            merr << "exception" << std::endl;
#else
            merr << e << std::endl;
#endif
          }
        } else {
          try {
            mout << "starting processing of frame " << frame_name << std::endl;
            bboxes &bb = detect.fprop(frame, frame_name.c_str(), frame_id);
            copy_bboxes(bb); // make a copy of bounding boxes
          } catch(ebl::eblexception &e) { // detection failed
#ifdef __NOEXCEPTIONS__
            eblwarn("detection failed");
#else
            eblwarn("detection failed: " << e);
#endif
            clear_bboxes();
          }
        }
      }
#ifdef __GUI__
      else { // fprop and display
        if (precomputed_boxes) eblerror("not implemented for nms only (TODO)");
        disable_window_updates();
        select_window(wid);
        clear_window();
        std::string title = _name;
        title << ": " << frame_name;
        set_window_title(title.c_str());
        //	 clear_resize_window();
        try {
          if (mindisplay) {
            bboxes &bb =
                dgui.display(detect, frame, frame_name.c_str(), frame_id,
                             0, 0, zoom, display_min, display_max,
                             wid, _name.c_str(), display_transp);
            copy_bboxes(bb); // make a copy of bounding boxes
          } else {
            // extract & display boxes
            bboxes &bb =
                dgui.display_inputs_outputs(detect, frame, frame_name.c_str(),
                                            frame_id, 0, 0, zoom,
                                            display_min, display_max, wid,
                                            _name.c_str(),
                                            display_in_min, display_in_max,
                                            display_transp, display_wmax);
            // make a copy of bounding boxes
            copy_bboxes(bb);
          }
        } catch(ebl::eblexception &e) { // detection failed
          eblwarn("detection failed: " << e);
          clear_bboxes();
        }
        enable_window_updates();
      }
      if (display_states) {
        dgui.display_current(detect, frame, wid_states, NULL, zoom);
        select_window(wid);
      }
      if (save_video && display) {
        std::string fname = viddir;
        fname += frame_name;
        save_window(fname.c_str());
        if (!silent) mout << "saved " << fname << std::endl;
      }
#endif
      if (!silent)
        mout << "processing done for frame " << frame_name << std::endl;
      // bootstrapping
      if (conf.exists_true("bootstrapping")) {
        boot.fprop(detect, frame_name);
        // add multiple scales if positives and scales are defined
        if (conf.exists("gt_scales") && boot.extract_positives()) {
          std::vector<double> scales =
              string_to_doublevector(conf.get_cstring("gt_scales"));
          for (uint s = 0; s < scales.size(); ++s) {
            double f = scales[s];
            // downsample input by f
            detect.set_resolution(f);
            detect.init(frame.get_idxdim(), frame_name.c_str());
            detect.fprop(frame, frame_name.c_str(), frame_id);
            boot.fprop(detect, frame_name, false, f);
          }
          detect.set_scaling_original();
          detect.init(frame.get_idxdim(), frame_name.c_str());
        }
        copy_bootstrapping(boot.get_all(), boot.get_bball());
#ifdef __GUI__
        // display groundtruth
        if (conf.exists_true("display_bootstrapping"))
          dgui.display_groundtruth(detect, frame, boot.get_gtall(),
                                   boot.get_gtclean(), boot.get_gtrest(),
                                   boot.get_bbpos(), boot.get_bbneg(),
                                   boot.get_pos(), boot.get_neg(), 0, 0, zoom,
                                   display_min, display_max);
#endif
      }
      total_saved = detect.get_total_saved();
      ms = tpass.elapsed_milliseconds();
      if (!silent) {
        mout << bbs.pretty_short(detect.get_labels());
        mout << "processing=" << ms << " ms ("
             << tpass.elapsed() << ")" << std::endl;
      }
      DEBUGMEM_PRETTY("after detection");
      // switch 'updated' flag on to warn we just added new data
      set_out_updated();
      // display sleep
      if (display_sleep > 0) {
        mout << "sleeping for " << display_sleep << "ms." << std::endl;
        millisleep(display_sleep);
      }
      if (conf.exists("save_max") &&
          detect.get_total_saved() > conf.get_uint("save_max"))
        break ; // limit number of detection saves
    }
    mout << "detection finished. Execution time: " << toverall.elapsed()<<std::endl;
    // free variables
    if (net) delete net;
    if (ans) delete ans;
  } eblcatcherror();
}

// thread communication ////////////////////////////////////////////////////////

template <typename T>
void detection_thread<T>::init_detector(detector<T> &detect,
                                        configuration &conf,
                                        std::string &odir, bool silent) {
  // multi-scaling parameters
  double maxs = conf.try_get_double("max_scale", 2.0);
  double mins = conf.try_get_double("min_scale", 1.0);
  t_scaling scaling_type =
      (t_scaling) conf.try_get_uint("scaling_type", SCALES_STEP);
  double scaling = conf.try_get_double("scaling", 1.4);
  std::vector<midxdim> scales;
  switch (scaling_type) {
    case MANUAL:
      if (!conf.exists("scales"))
	eblerror("expected \"scales\" variable to be defined in manual mode");
      scales = string_to_midxdimvector(conf.get_cstring("scales"));
      detect.set_resolutions(scales);
      break ;
    case ORIGINAL: detect.set_scaling_original(); break ;
    case SCALES_STEP:
      detect.set_resolutions(scaling, maxs, mins);
      break ;
    case SCALES_STEP_UP:
      detect.set_resolutions(scaling, maxs, mins);
      detect.set_scaling_type(scaling_type);
      break ;
    default:
      detect.set_scaling_type(scaling_type);
  }
  // remove pads from target scales if requested
  if (conf.exists_true("scaling_remove_pad")) detect.set_scaling_rpad(true);
  // optimize memory usage by using only 2 buffers for entire flow
  state<T> input(1, 1, 1), output(1, 1, 1);
  if (!conf.exists_false("mem_optimization"))
    detect.set_mem_optimization(input, output, true);
  // TODO: always keep inputs, otherwise detection doesnt work. fix this.
  // 				   conf.exists_true("save_detections") ||
  // 				   (display && !mindisplay));
  // zero padding
  float hzpad = conf.try_get_float("hzpad", 0);
  float wzpad = conf.try_get_float("wzpad", 0);
  detect.set_zpads(hzpad, wzpad);
  if (conf.exists("input_min")) // limit inputs size
    detect.set_min_resolution(conf.get_uint("input_min"));
  if (conf.exists("input_max")) // limit inputs size
    detect.set_max_resolution(conf.get_uint("input_max"));
  if (silent) detect.set_silent();
  if (conf.exists_bool("save_detections")) {
    std::string detdir = odir;
    detdir += "detections";
    uint nsave = conf.try_get_uint("save_max_per_frame", 0);
    bool diverse = conf.exists_true("save_diverse");
    detdir = detect.set_save(detdir, nsave, diverse);
  }
  detect.set_scaler_mode(conf.exists_true("scaler_mode"));
  if (conf.exists("bbox_decision"))
    detect.set_bbox_decision(conf.get_uint("bbox_decision"));
  if (conf.exists("bbox_scalings")) {
    mfidxdim scalings =
	string_to_fidxdimvector(conf.get_cstring("bbox_scalings"));
    detect.set_bbox_scalings(scalings);
  }

  // nms configuration //////////////////////////////////////////////////////
  t_nms nms_type = (t_nms) conf.try_get_uint("nms", 0);
  float pre_threshold = conf.try_get_float("pre_threshold", 0.0);
  float post_threshold = conf.try_get_float("post_threshold", 0.0);
  float pre_hfact = conf.try_get_float("pre_hfact", 1.0);
  float pre_wfact = conf.try_get_float("pre_wfact", 1.0);
  float post_hfact = conf.try_get_float("post_hfact", 1.0);
  float post_wfact = conf.try_get_float("post_wfact", 1.0);
  float woverh = conf.try_get_float("woverh", 1.0);
  float max_overlap = conf.try_get_float("max_overlap", 0.0);
  float max_hcenter_dist = conf.try_get_float("max_hcenter_dist", 0.0);
  float max_wcenter_dist = conf.try_get_float("max_wcenter_dist", 0.0);
  float vote_max_overlap = conf.try_get_float("vote_max_overlap", 0.0);
  float vote_mhd = conf.try_get_float("vote_max_hcenter_dist", 0.0);
  float vote_mwd = conf.try_get_float("vote_max_wcenter_dist", 0.0);
  detect.set_nms(nms_type, pre_threshold, post_threshold, pre_hfact,
                 pre_wfact, post_hfact, post_wfact, woverh, max_overlap,
                 max_hcenter_dist, max_wcenter_dist, vote_max_overlap,
                 vote_mhd, vote_mwd);
  if (conf.exists("raw_thresholds")) {
    std::string srt = conf.get_string("raw_thresholds");
    std::vector<float> rt = string_to_floatvector(srt.c_str());
    detect.set_raw_thresholds(rt);
  }
  if (conf.exists("outputs_threshold"))
    detect.set_outputs_threshold(conf.get_double("outputs_threshold"),
                                 conf.try_get_double("outputs_threshold_val",
                                                     -1));
  ///////////////////////////////////////////////////////////////////////////
  if (conf.exists("netdims")) {
    idxdim d = string_to_idxdim(conf.get_string("netdims"));
    detect.set_netdim(d);
  }
  if (conf.exists("smoothing")) {
    idxdim ker;
    if (conf.exists("smoothing_kernel"))
      ker = string_to_idxdim(conf.get_string("smoothing_kernel"));
    detect.set_smoothing(conf.get_uint("smoothing"),
                         conf.try_get_double("smoothing_sigma", 1),
                         &ker,
                         conf.try_get_double("smoothing_sigma_scale", 1));
  }
  if (conf.exists("background_name"))
    detect.set_bgclass(conf.get_cstring("background_name"));
  if (conf.exists_true("bbox_ignore_outsiders"))
    detect.set_ignore_outsiders();
  if (conf.exists("corners_inference"))
    detect.set_corners_inference(conf.get_uint("corners_inference"));
  if (conf.exists("input_gain"))
    detect.set_input_gain(conf.get_double("input_gain"));
  if (conf.exists_true("dump_outputs")) {
    std::string fname;
    fname << odir << "/dump/detect_out";
    detect.set_outputs_dumping(fname.c_str());
  }
}

template <typename T>
bool detection_thread<T>::get_data(bboxes &bboxes2, idx<ubyte> &frame2,
                                   uint &total_saved_, std::string &frame_name_,
                                   uint *id, svector<midx<T> > *samples,
                                   bboxes *bbsamples, bool *skipped) {
  // lock data
  mutex_out.lock();
  // only read data if it has been updated
  if (!out_updated) {
    // unlock data
    mutex_out.unlock();
    return false;
  }
  // data is updated, but just to tell we skipped the frame
  if (frame_skipped) {
    if (skipped) *skipped = true;
    frame_skipped = false;
    // reset updated flag
    out_updated = false;
    // declare thread as available
    bavailable = true;
    // unlock data
    mutex_out.unlock();
    return false;
  }
  if (skipped) *skipped = false;
  // clear bboxes
  bboxes2.clear();
  bboxes2.push_back_new(bbs);
  bbs.clear(); // no use for local bounding boxes anymore, clear them
  // check frame is correctly allocated, if not, allocate.
  if (frame2.order() != uframe.order())
    frame2 = idx<ubyte>(uframe.get_idxdim());
  else if (frame2.get_idxdim() != uframe.get_idxdim())
    frame2.resize(uframe.get_idxdim());
  // copy frame
  idx_copy(uframe, frame2);
  // set total of boxes saved
  total_saved_ = total_saved;
  // set frame name
  frame_name_ = frame_name;
  // set frame id
  if (id) *id = frame_id;
  // overwrite samples
  if (samples) {
    samples->clear();
    samples->push_back_new(returned_samples);
    returned_samples.clear();
  }
  if (bbsamples) {
    bbsamples->clear();
    bbsamples->push_back_new(returned_samples_bboxes);
    returned_samples_bboxes.clear();
  }
  // reset updated flag
  out_updated = false;
  // declare thread as available
  bavailable = true;
  // unlock data
  mutex_out.unlock();
  // confirm that we copied data.
  return true;
}

template <typename T>
bool detection_thread<T>::set_data(idx<ubyte> &frame2,
                                   std::string &fullname,
                                   std::string &name, uint id) {
  // lock data (non blocking)
  if (!mutex_in.trylock())
    return false;
  // check frame is correctly allocated, if not, allocate.
  if (frame2.order() != uframe.order())
    uframe = idx<ubyte>(frame2.get_idxdim());
  else if (frame2.get_idxdim() != uframe.get_idxdim())
    uframe.resize(frame2.get_idxdim());
  idx_copy(frame2, uframe);	// copy frame
  frame_loaded = true;        // frame is loaded
  frame_fullname = fullname;
  frame_name = name;		// copy name
  frame_id   = id;		// copy frame_id
  in_updated = true;		// reset updated flag
  bavailable = false;		// declare thread as not available
  bfed       = true;		// data has been fed at least once
  mutex_in.unlock();		// unlock data
  return true;		// confirm that we copied data.
}

template <typename T>
bool detection_thread<T>::set_data(std::string &fullname,
                                   std::string &name,uint id){
  // lock data (non blocking)
  if (!mutex_in.trylock())
    return false;
  // load image
  frame_fullname = fullname;
  frame_name = name;          // copy name
  frame_id = id;              // copy frame_id
  in_updated = true;		// reset updated flag
  frame_loaded = false;       // let it be loaded later
  bavailable = false;		// declare thread as not available
  bfed       = true;		// data has been fed at least once
  mutex_in.unlock();		// unlock data
  return true;
}

template <typename T>
bool detection_thread<T>::available() {
  return bavailable;
}

template <typename T>
bool detection_thread<T>::fed() {
  return bfed;
}

template <typename T>
std::string detection_thread<T>::
get_output_directory(configuration &conf) {
  std::string s;
  if (conf.exists("output_dir")) s << conf.get_string("output_dir");
  else s << conf.get_string("current_dir");
  if (conf.exists("detections_dir"))
    s << conf.get_string("detections_dir");
  else {
    s << "/detections";
    if (conf.exists_true("nms")) s << "_" << tstamp();
  }
  s << "/";
  mkdir_full(s);
  return s;
}

// protected methods /////////////////////////////////////////////////////////

template <typename T>
void detection_thread<T>::clear_bboxes() {
  // lock data
  mutex_out.lock();
  // clear bboxes
  bbs.clear();
  // unlock data
  mutex_out.unlock();
}

template <typename T>
void detection_thread<T>::copy_bboxes(bboxes &bb) {
  // lock data
  mutex_out.lock();
  bbs = bb;
  // unlock data
  mutex_out.unlock();
}

template <typename T>
void detection_thread<T>::copy_bootstrapping(svector<midx<T> > &samples,
                                             bboxes &bb) {
  // lock data
  mutex_out.lock();
  returned_samples = samples;
  returned_samples_bboxes = bb;
  std::cout << "samples: " << samples << std::endl;
  std::cout << "boxes: " << bb << std::endl;
  // unlock data
  mutex_out.unlock();
}

template <typename T>
void detection_thread<T>::set_out_updated() {
  // lock data
  mutex_out.lock();
  // set flag
  out_updated = true;
  // unlock data
  mutex_out.unlock();
}

template <typename T>
void detection_thread<T>::skip_frame() {
  // lock data
  mutex_out.lock();
  mutex_in.lock();
  mout << "frame skipping requested for " << frame_fullname << std::endl;
  // set flag
  out_updated = true;
  frame_skipped = true;
  // unlock data
  mutex_in.unlock();
  mutex_out.unlock();
}

} // end namespace ebl

#endif /* DETECTION_THREAD_HPP_ */
