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

#ifndef DATASET_HPP_
#define DATASET_HPP_

#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits>
#include <typeinfo>

#ifdef __GUI__
#include "libidxgui.h"
#endif

#include "defines_tools.h"
#include "sort.h"
#include "tools_utils.h"

#ifdef __BOOST__
#ifndef BOOST_FILESYSTEM_VERSION
#define BOOST_FILESYSTEM_VERSION 3
#endif
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#endif

namespace ebl {

// jitter methods //////////////////////////////////////////////////////////////

template <typename T>
rect<T> jitter::get_rect(const rect<T> &r, float ratio) {
  rect<T> j = r; // jittered box
  j.scale_centered(s, s); // apply scale jitter
  j.h0 += (T) (h * ratio); // apply height offset jitter
  j.w0 += (T) (w * ratio); // apply width offset jitter
  // EDEBUG("jittering rect " << r << " with spatial jitter " << h << "x" << w
  // 	  << " and spatial jitter ratio " << ratio << ": " << j);
  return j;
}

// constructors & initializations //////////////////////////////////////////////

template <typename Tdata>
dataset<Tdata>::dataset(const char *name_, const char *inroot_) {
  // initialize members
  allocated = false;
  std::string bname = ebl::basename(name_);
  set_name(bname);
  if (inroot_) {
    inroot = inroot_;
    inroot += "/";
  } else inroot = ebl::dirname(name_);
  no_outdims = true;
  outdims = idxdim(96, 96, 1);
  height = outdims.dim(0);
  width = outdims.dim(1);
  mindims = idxdim(1, 1);
  maxdims = idxdim(1, 1);
  maxdims_set = false;
  max_data = 0;
  max_data_set = false;
  total_samples = 0;
  display_extraction = false;
  display_result = false;
  bsave_display = false;
  dataset_loaded = false;
  // preprocessing
  extension = IMAGE_PATTERN_MAT;
  sleep_display = false;
  sleep_delay = 0;
  // assuming already processed data, but the user can still require
  // processing via the set_preprocessing method.
  do_preprocessing = false;
  scale_mode = false;
  scales.push_back(1); // initialize with scale 1
  data_cnt = 0;
  interleaved_input = true;
  load_planar = false;
  max_per_class_set = false;
  mpc = (std::numeric_limits<intg>::max)();
  dynamic_init_drand(); // initialize random seed
  usepose = false;
  useparts = false;
  usepartsonly = false;
  save_mode = DYNSET_SAVE;
  individual_save = true; // save individual files by default.
  separate_layers_save = false; // save layers in separate files or not.
  bbox_woverh = 0.0; // 0.0 means not set
  force_label = "";
  nclasses = 0;
  add_errors = 0;
  nopadded = false;
  wmirror = false;
  // jitter
  tjitter_step = 0;
  tjitter_hmin = 0;
  tjitter_hmax = 0;
  tjitter_wmin = 0;
  tjitter_wmax = 0;
  sjitter_steps = 0;
  sjitter_min = 0;
  sjitter_max = 0;
  rjitter_steps = 0;
  rjitter = 0;
  njitter = 0;
  minvisibility = 0;
  bjitter = false;
  nlayers = 1;
  //videobox
  do_videobox = false;
  videobox_nframes = 0;
  videobox_stride = 0;
  load_img = midx<Tdata>(1);
#ifndef __BOOST__
  eblerror(BOOST_LIB_ERROR);
#endif
}

template <typename Tdata>
dataset<Tdata>::~dataset() {
  for (uint i = 0; i < ppmods.size(); ++i)
    delete ppmods[i];
}

template <typename Tdata>
bool dataset<Tdata>::alloc(intg max) {
  // save maximum number of samples if specified
  if (max > 0) {
    max_data = max;
    max_data_set = true;
    std::cout << "Limiting dataset to " << max << " samples." << std::endl;
  }
  if (classes.size() == 0)
    eblerror("found 0 class");
  std::cout << "Found: "; print_classes(); std::cout << "." << std::endl;
  // (re)init max per class, knowing number of classes
  intg m = (std::numeric_limits<intg>::max)();
  if (max_per_class_set) { // mpc has already been set
    m = mpc;
    set_max_per_class(m);
  }
  // allocate
  if (!allocate(total_samples, outdims))
    eblerror("allocation failed");
  // alloc tallies
  add_tally = idx<intg>(classes.size());
  idx_clear(add_tally);
  return true;
}

// data extraction /////////////////////////////////////////////////////////////

template <typename Tdata>
bool dataset<Tdata>::extract() {
  // extract
#ifdef __BOOST__
  if (!allocated && !scale_mode && !strcmp(save_mode.c_str(), DATASET_SAVE))
    return false;
  boost::regex hidden_dir(".svn");
  bool found = false;
  xtimer.start(); // start timing extraction
  processed_cnt = 0;
  boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
  for (  boost::filesystem::directory_iterator itr(inroot); itr != end_itr; itr++) {
#if !defined(BOOST_FILESYSTEM_VERSION) || BOOST_FILESYSTEM_VERSION == 3
    if (boost::filesystem::is_directory(itr->status())
        && !boost::regex_match(itr->path().filename().string(), hidden_dir)) {
      process_dir(itr->path().string(), extension, itr->path().filename().string());
      found = true;
    }
#else
    if (boost::filesystem::is_directory(itr->status())
        && !boost::regex_match(itr->path().filename().c_str(), hidden_dir)) {
      process_dir(itr->path().string(), extension, itr->path().filename().string());
      found = true;
    }
#endif
  }
  if (found) {
    std::cerr << "Samples adding failures: " << add_errors << std::endl;
  } else {
    std::cerr << "No class found in " << inroot
              << ", extracting images in local directory with name \"unknown\""
              << std::endl;
    process_dir(inroot, extension, "unknown");
    return true;
  }
  std::cout << "Extraction time: " << xtimer.elapsed() << std::endl;
#endif /* __BOOST__ */
  return true;
}

template <typename Tdata>
void dataset<Tdata>::extract_statistics() {
  eblerror("not implemented");
}

////////////////////////////////////////////////////////////////
// data

template <typename Tdata>
bool dataset<Tdata>::split_max_and_save(const char *name1, const char *name2,
                                        intg max, const std::string &outroot) {
  dataset<Tdata> ds1(name1);
  dataset<Tdata> ds2(name2);
  ds1.set_outdims(outdims);
  ds2.set_outdims(outdims);
  // if excluded classes exist, set them in ds1 and ds2
  if (exclude.size() > 0) {
    ds1.set_exclude(exclude);
    ds2.set_exclude(exclude);
  }
  split_max(ds1, ds2, max);
  bool ret1 = ds1.save(outroot);
  bool ret2 = ds2.save(outroot);
  return ret1 || ret2;
}

template <typename Tdata>
void dataset<Tdata>::split_max(dataset<Tdata> &ds1, dataset<Tdata> &ds2,
                               intg max) {
  std::cout << "Splitting \"" << name << "\" into datasets \"";
  std::cout << ds1.name << "\" and \"" << ds2.name << "\", limiting dataset \"";
  std::cout << ds1.name << "\" to " << max << " samples per class, the rest ";
  std::cout << "going to \"" << ds2.name << "\"." << std::endl;

  // copy classes strings
  if (classes.size() > 0) {
    idx<ubyte> classidx = build_classes_idx(classes);
    ds1.set_classes(classidx, false);
    ds2.set_classes(classidx, false);
  }
  ds1.nclasses = nclasses;
  ds2.nclasses = nclasses;
  // set max samples per class for dataset 1 (ds2 takes whatever is left)
  ds1.set_max_per_class(max);
  // split
  split(ds1, ds2);
}

template <typename Tdata>
void dataset<Tdata>::shuffle() {
  std::cout << "Shuffling dataset \"" << name << "\"." << std::endl;
  dynamic_init_drand(); // initialize random seed
  idx<int64> offsets = data.get_offsets();
  if (offsets.order() != data.order())
    idx_shuffle_together(data, labels, 0);
  else
    idx_shuffle_together(data, offsets, labels, 0);
}

// data preprocessing //////////////////////////////////////////////////////////

template <typename Tdata>
void dataset<Tdata>::set_planar_loading() {
  std::cout << "Inputs are loaded as planar." << std::endl;
  load_planar = true;
}

// accessors ///////////////////////////////////////////////////////////////////

template <typename Tdata>
const idxdim& dataset<Tdata>::get_sample_outdim() {
  return outdims;
}

template <typename Tdata>
intg dataset<Tdata>::size() {
  return data_cnt;
}

template <typename Tdata>
t_label dataset<Tdata>::get_label_from_class(const std::string &class_name) {
  t_label label = 0;
  std::vector<std::string>::iterator res;
  res = find(classes.begin(), classes.end(), class_name);
  if (res == classes.end()) { // not found
    return -42; // excluded class
  }
  // found
  label = res - classes.begin();
  return label;
}

template <typename Tdata>
void dataset<Tdata>::set_display(bool display_) {
#ifdef __GUI__
  if (display_) {
    std::cout << "Enabling display." << std::endl;
    new_window("Dataset compiler");
    display_extraction = display_;
  }
#endif /* __GUI__ */
}

template <typename Tdata>
void dataset<Tdata>::set_sleepdisplay(uint delay) {
  if (delay > 0) {
    std::cout << "Enabling sleeping display for " << delay << "ms." << std::endl;
    sleep_display = true;
    sleep_delay = delay;
  }
}

template <typename Tdata>
void dataset<Tdata>::
set_preprocessing(std::vector<resizepp_module<Tdata>*> &p){
  if (p.size() == 0) {
    std::cout << "No preprocessing." << std::endl;
    return ;
  }
  pp_names = "";
  nlayers = 0;
  for (uint i = 0; i < p.size(); ++i) {
    ppmods.push_back(p[i]);
    pp_names << p[i]->name();
    if (i + 1 != p.size()) pp_names << ",";
    nlayers += p[i]->nlayers();
    if (p[i]->nlayers() == 0)
      eblerror("expected at least 1 layer in output of preprocessing module "
               << p[i]->name());
  }
  std::cout << "Setting preprocessing to: " << pp_names << std::endl;
  std::cout << "Preprocessing produces " << nlayers << " layers per sample."
            << std::endl;
  std::cout << "Preprocessing modules:" << std::endl;
  for (uint i = 0; i < p.size(); ++i)
    std::cout << i << ": " << ppmods[i]->describe() << std::endl;
  do_preprocessing = true;
}

template <typename Tdata>
bool dataset<Tdata>::full(t_label label) {
  if (!total_samples) // we did not count samples, no opinion
    return false;
  if ((max_data_set && (data_cnt >= max_data)) ||
          ((data.order() > 0) && (data_cnt >= data.dim(0))))
    return true;
  if (!scale_mode) {
    if (label == -42) // excluded class
      return true;
    if (max_per_class_set && (label >= 0) &&
            (add_tally.get(label) >= max_per_class.get(label)))
      return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////
// print methods

template <typename Tdata>
void dataset<Tdata>::print_classes() {
  std::cout << nclasses << " classe";
  if (nclasses > 1) std::cout << "s";
  if (classes.size() > 0) {
    std::cout << " (";
    uint i;
    for (i = 0; i < classes.size() - 1; ++i)
      std::cout << classes[i] << ", ";
    std::cout << classes[i] << ")";
  }
}

template <typename Tdata>
void dataset<Tdata>::print_stats() {
  compute_stats();
  // print stats
  std::cout << "Dataset \"" << name << "\" contains " << data_cnt;
  std::cout << " samples (of dimensions " << outdims << " and ";
  std::cout << typeid(Tdata).name() << " precision)";
  std::cout << ", distributed in " << nclasses << " classes";
  if (classes.size() > 0) {
    uint i;
    std::cout << ": ";
    for (i = 0; i < classes.size() - 1; ++i)
      std::cout << class_tally.get(i) << " \"" << classes[i] << "\", ";
    std::cout << class_tally.get(i) << " " << classes[i] << "\".";
  }
  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////
// I/O

template <typename Tdata>
bool dataset<Tdata>::load(const std::string &root) {
  std::string fname;
  std::string root1 = root;
  root1 += "/";
  std::cout << "Loading dataset " << name << " from " << root1;
  std::cout << name << "_*" << MATRIX_EXTENSION << std::endl;
  // load data
  data = midx<Tdata>(1,1);
  fname = root1; fname += data_fname;
  loading_error(data, fname);
  // load labels
  labels = idx<t_label>(1);
  fname = root1; fname += labels_fname;
  loading_warning<t_label>(labels, fname);
  // load ids
  ids = idx<intg>(labels.dim(0));
  idx_clear(ids);
  fname = ""; fname << root1 << "/" << ids_fname;
  if (loading_warning<intg>(ids, fname)) {
    if (ids.dim(0) != labels.dim(0))
      eblerror("expected " << labels.dim(0) << " samples in scales file "
               << "but found " << ids.dim(0));
  }
  // load jitter
  fname = root1; fname += jitters_fname;
  loading_warning<t_jitter>(jitters, fname);
  // load classes
  idx<ubyte> classidx;
  classidx = idx<ubyte>(1,1);
  fname = root1; fname += classes_fname;
  if (loading_warning(classidx, fname))
    set_classes(classidx, false);
  else {
    // if not allocated, assign index as class names
    set_index_classes();
    nclasses = idx_max(labels) + 1;
  }
  // load classpairs
  classpairs = idx<t_label>(1,1);
  fname = root1; fname += classpairs_fname;
  loading_nowarning(classpairs, fname);
  // load deformation pairs
  deformpairs = idx<t_label>(1,1);
  fname = root1; fname += deformpairs_fname;
  loading_nowarning(deformpairs, fname);
  // initialize some members
  data_cnt = data.dim(0);
  allocated = true;
  idx<Tdata> sample;
  if (data.order() == 2) sample = data.mget(0, 0);
  else sample = data.mget(0);
  outdims = sample.get_idxdim();
  print_stats();
  dataset_loaded = true;
  return true;
}

template <typename Tdata>
bool dataset<Tdata>::save(const std::string &root, bool save_data) {
  if (!allocated && !strcmp(save_mode.c_str(), DATASET_SAVE))
    return false;
  std::string root1 = root;
  root1 += "/";
  std::cout << "Saving " << name << " in " << save_mode << " mode." << std::endl;
  // if force label is on, replace labels by force_label
  if (strcmp(force_label.c_str(), "")) {
    std::cout << "Forcing all labels to: " << force_label << std::endl;
    classes.clear();
    add_class(force_label);
    idx_clear(labels); // setting all labels to 0
    idx_clear(ids);
  }
  // return false if no samples
  if (data_cnt <= 0) {
    std::cerr << "Warning: No samples were added to the dataset, nothing to save.";
    std::cerr << std::endl;
    return false;
  }
  // creating directory
  mkdir_full(root1);
  // remove for empty slots
  midx<Tdata> dat = data;
  idx<t_label> labs = labels;
  idx<intg> scals = ids;
  midx<t_jitter> jitts = jitters;
  bool bjitts = (jitters.order() >= 1 && idx_max(jitters) > 0);
  if (data_cnt < total_samples) {
    std::cerr << "Warning: not all samples were added to dataset (";
    std::cerr << total_samples - data_cnt << " missing out of " << total_samples;
    std::cerr << ")" << std::endl;
    std::cout << "Downsizing dataset to igore missing samples." << std::endl;
    if (!strcmp(save_mode.c_str(), DATASET_SAVE))
      dat = dat.narrow(0, data_cnt, 0);
    labs = labs.narrow(0, data_cnt, 0);
    scals = scals.narrow(0, data_cnt, 0);
    if (bjitts)
      jitts = jitts.narrow(0, data_cnt, 0);
  }
  // switch between saving modes
  if (!strcmp(save_mode.c_str(), DATASET_SAVE)
          || !strcmp(save_mode.c_str(), DYNSET_SAVE)) { // dataset mode
    std::cout << "Saving dataset " << name << " in " << root << "/";
    std::cout << name << "_*" << MATRIX_EXTENSION << std::endl;
    // save data
    std::string fname;
    if (save_data) {
      fname = ""; fname << root1 << data_fname;
      std::cout << "Saving " << fname << " (" << dat << ")" << std::endl;
      if (!strcmp(save_mode.c_str(), DATASET_SAVE)) { // save data matrix
        // 	// make sure dat can be transformed into a single idx
        // 	idx<Tdata> single = dat.pack();
        // 	if (!save_matrix(single, fname)) {

        // save from dynamic to static
        if (images_list.size() == 0 && dataset_loaded == true) {
          if (data.true_order() != 1)
            eblerror("In dataset mode, multiscale not supported");
          if (!save_matrix<Tdata>(data, fname)) {
            std::cerr << "error: failed to save " << fname << std::endl;
            return false;
          } else std::cout << "Saved " << fname << std::endl;
        }
        else {
          if (!save_matrix<Tdata>(images_list, fname)) {
            std::cerr << "error: failed to save " << fname << std::endl;
            return false;
          } else std::cout << "Saved " << fname << std::endl;
        }
      } else if (!strcmp(save_mode.c_str(), DYNSET_SAVE)) {
        // compile data file from existing files
        if (images_list.size() > 0) {
          std::cout << "Saving dynamic dataset from a list of "
                    << images_list.size() << " samples." << std::endl;
          if (!save_matrices<Tdata>(images_list, fname)) {
            std::cerr << "error: failed to save " << fname << std::endl;
            return false;
          }
        } else {
          std::cout << "Saving dynamic dataset from internal data matrix "
                    << data << std::endl;
          if (!save_matrices<Tdata>(data, fname)) {
            std::cerr << "error: failed to save " << fname << std::endl;
            return false;
          }
        }
        std::cout << "Saved " << fname << std::endl;
      }
    }
    // save labels
    fname = ""; fname << root1 << labels_fname;
    std::cout << "Saving " << fname << " (" << labs << ")"  << std::endl;
    if (!save_matrix(labs, fname)) {
      std::cerr << "error: failed to save labels into " << fname << std::endl;
      return false;
    } else std::cout << "Saved " << fname << std::endl;
    // save jitters
    if (bjitts) {
      fname = ""; fname << root1 << jitters_fname;
      std::cout << "Saving " << fname << " (" << jitts << ")"  << std::endl;
      if (!save_matrices(jitts, fname)) {
        std::cerr << "error: failed to save labels into " << fname << std::endl;
        return false;
      } else std::cout << "Saved " << fname << std::endl;
    }
    // save ids
    fname = ""; fname << root1 << ids_fname;
    std::cout << "Saving " << fname << " (" << scals << ")"  << std::endl;
    if (!save_matrix(scals, fname)) {
      std::cerr << "error: failed to save ids into " << fname << std::endl;
      return false;
    } else std::cout << "Saved " << fname << std::endl;
    // save classes
    if (classes.size() > 0) {
      fname = ""; fname << root1 << classes_fname;
      idx<ubyte> classes_idx = build_classes_idx(classes);
      std::cout << "Saving " << fname << " (" << classes_idx << ")"  << std::endl;
      if (!save_matrix(classes_idx, fname)) {
        std::cerr << "error: failed to save classes into " << fname << std::endl;
        return false;
      } else std::cout << "Saved " << fname << std::endl;
    }
  } else { // single file mode, use save as image extensions
    //       root1 += name; root1 += "/";
    //       mkdir_full(root1);
    //       // save all images
    //       ostringstream fname;
    //       intg id = 0;
    //       idx<Tdata> tmp;
    //       idx_bloop2(dat, data, Tdata, lab, labs, t_label) {
    // 	// make class directory if necessary
    // 	fname.str("");
    // 	fname << root1 << "/" << get_class_string(lab.get()) << "/";
    // 	mkdir_full(fname.str().c_str());
    // 	// save image
    // 	fname << get_class_string(lab.get()) << "_" << id++ << "." << save_mode;
    // 	tmp = dat.shift_dim(0, 2); // shift from planar to interleaved
    // 	// scale image to 0 255 if preprocessed
    // 	if (strcmp(ppconv_type.c_str(), "RGB")) {
    // 	  idx_addc(tmp, (Tdata) 1.0, tmp);
    // 	  idx_dotc(tmp, (Tdata) 127.5, tmp);
    // 	}
    // 	if (save_image(fname.str(), tmp, save_mode.c_str()))
    // 	  std::cout << id << ": saved " << fname.str() << std::endl;
    //      }
  }
  return true;
}

////////////////////////////////////////////////////////////////
// allocation

template <typename Tdata>
bool dataset<Tdata>::allocate(intg n, idxdim &d) {
  // allocate only once
  //if (allocated)
  //  return false;
  // initialize members
  outdims = d;
  data_cnt = 0;
  // if max_data has been set, max n with max_data
  if (max_data_set)
    n = MIN(n, max_data);
  std::cout << "Dataset \"" << name << "\" will have " << n;
  std::cout << " samples of size " << d << std::endl;
  // max with max_per_class
  if (max_per_class_set)
    n = (std::max)((intg) 0, MIN(n, idx_sum(max_per_class)));
  if (n <= 0) {
    std::cerr << "Cannot allocate " << n << " samples." << std::endl;
    return false;
  }
  // allocate data buffer (only in dataset_save mode)
  if (!strcmp(save_mode.c_str(), DATASET_SAVE)) {
    std::cout << "Allocating dataset \"" << name << "\" with " << n;
    std::cout << " samples of size " << d << " ("
              << (n * d.nelements() * sizeof (Tdata)) / (1024 * 1024)
              << " Mb) ..." << std::endl;
    // uint c = 0, h = 1, w = 2;
    // if (interleaved_input) {
    // 	c = 2; h = 0; w = 1;
    // }
    // idxdim datadims(outdims.dim(c), outdims.dim(h), outdims.dim(w));
    data = midx<Tdata>(n, nlayers);
    datadims = data.get_idxdim();
  }
  // allocate labels buffer
  labels = idx<t_label>(n);
  ids = idx<intg>(n);
  // allocate jitter buffer
  if (bjitter)
    jitters = midx<t_jitter>(n);
  allocated = true;
  // alloc tally
  if (nclasses > 0) {
    add_tally = idx<intg>(nclasses);
    idx_clear(add_tally);
  }
  std::cout << "data matrix is " << data << std::endl;
  total_samples = n;
  return true;
}

////////////////////////////////////////////////////////////////
// data

template <typename Tdata>
bool dataset<Tdata>::add_mdata(midx<Tdata> &original_sample,
                               const t_label label,
                               const std::string *class_name,
                               const char *filename, const rect<int> *r,
                               std::pair<int,int> *center,
                               const rect<int> *visr,
                               const rect<int> *cropr,
                               const std::vector<object*> *objs,
                               const jitter *jittforce) {
  idx<Tdata> dat = original_sample.mget(0);
#ifdef __DEBUG__
  std::cout << "Adding image " << dat << " with label " << label;
  if (class_name) std::cout << ", class name " << *class_name;
  if (r) std::cout << ", ROI " << *r;
  if (center) std::cout << ", center " << center->first << "," << center->second;
  std::cout << " from " << (filename?filename:"") << std::endl;
#endif
  try {
    // check for errors
    if (!allocated && !strcmp(save_mode.c_str(), DATASET_SAVE))
      eblthrow("dataset has not been allocated, cannot add data.");
    // check that input is bigger than minimum dimensions allowed
    if ((dat.dim(0) < mindims.dim(0))
            || (r && (r->height < mindims.dim(0)))
        || (dat.dim(1) < mindims.dim(1))
        || (r && (r->width < mindims.dim(1)))) {
      std::string err;
      err << "not adding " << *class_name << " from "
          << (filename?filename:"")
          << ": smaller than minimum dimensions (" << dat;
      if (r)
        err << " or " << *r;
      err << " is smaller than " << mindims << ")";
      eblthrow(err);
    }
    // check that input is smaller than maximum dimensions allowed
    if (maxdims_set && r && (r->height > maxdims.dim(0)
            || r->width > maxdims.dim(1))) {
      std::string err;
      err << "not adding " << *class_name << " from "
          << (filename?filename:"")
          << ": bigger than maximum dimensions (" << *r
          << " is bigger than " << maxdims << ")";
      eblthrow(err);
    }
    // check that class exists (may not exist if excluded)
    if (classes.size() > 0 &&
            find(classes.begin(), classes.end(), *class_name) == classes.end())
      eblthrow("not adding " << *class_name << " from "
               << (filename?filename:"") << ", this class is excluded.");
    // check that this class is included
    if (class_name && !included(*class_name))
      eblthrow("not adding " << *class_name << " from "
               << (filename?filename:"") << ", this class is excluded.");

    // do cropr if preprocessing is enabled.
    // you have to do this outside the jitter loop.
    if(do_preprocessing){
      // crop it if cropr is defined
      if (cropr) {
        // input region
        idx<Tdata> dat_cropped = dat;
        dat_cropped = dat_cropped.narrow(0, cropr->height, cropr->h0);
        dat_cropped = dat_cropped.narrow(1, cropr->width, cropr->w0);
        dat = dat_cropped;
        original_sample.mset(dat,0);
      }
    }
    // draw random jitter
    std::vector<jitter> jitt;
    if (njitter > 0 && !jittforce) {
      // draw last n random pairs
      for (uint i = 0; i < njitter; ++i) {
        if (random_jitter.size() == 0) // refill vector of random jitters
          this->compute_random_jitter();
        if (random_jitter.size() == 0) // no jitter possible in this image
          break ;
        jitt.push_back(random_jitter.back());
        random_jitter.pop_back();
      }
    } else // no jitter
      //      if (jitt.size() == 0)
      jitt.push_back(jitter(0,0,1.0,0.0)); // no jitter
    // add all jittered samples
    std::vector<jitter>::iterator ijit;
    for (ijit = jitt.begin(); ijit != jitt.end(); ++ijit) {
      // check for capacity
      if (full(label))
        //	if (!strcmp(save_mode.c_str(), DATASET_SAVE) && full(label))
        // reached full capacity
        eblthrow("not adding " << *class_name << " from "
                 << (filename?filename:"")
                 << ", reached full capacity for this class.");
      // copy data into target type
      idxdim d(dat);
      rect<int> inr;
      //
      // do preprocessing
      midx<Tdata> sample;
      if (do_preprocessing)
        sample = preprocess_data(original_sample, class_name, filename, r, 0,
                                 NULL, center, &(*ijit), visr, cropr, &inr);
      else sample = original_sample;
      dat = original_sample.mget(0);
      // ignore this sample if it is not visibile enough
      if (r && r->overlap_ratio(inr) < minvisibility)
        continue ;
      // remember all overlapping objects with current region
      idx<t_jitter> *js = NULL;
      rect<float> finr(inr.h0, inr.w0, inr.height, inr.width);
      finr = finr * (height / (float) inr.height);
      for (uint i = 0; objs && i < objs->size(); ++i) {
        const object &o = *(*objs)[i];
        if (o.min_overlap(inr) >= minvisibility) { // overlaps, add box
          // scale box into sample's scale
          rect<float> fo(o.h0, o.w0, o.height, o.width);
          fo = fo * (height / (float) inr.height);
          // convert box to jitter in sample's coordinate system
          jitter jj(finr, fo, height);
          const idx<t_jitter> &tjj = jj.get_jitter_vector();
          // add jitter
          if (!js) // allocate first time
            js = new idx<t_jitter>(1, tjj.dim(0));
          else // increase size by 1
            js->resize1(0, js->dim(0) + 1);
          idx<t_jitter> e = js->select(0, js->dim(0) - 1);
          idx_copy(tjj, e);
        }
      }
      // add mirrors
      if (wmirror) {
        // flip data using vertical axis
        midx<Tdata> flipped = idx_flip(sample, 1);
        // flip horizontal jitter
        ijit->w = -ijit->w;
        idx<t_jitter> *jsmirr = NULL;
        if (js) {
          jsmirr = new idx<t_jitter>(js->get_idxdim());
          idx_copy(*js, *jsmirr);
          idx<t_jitter> wj = jsmirr->select(1, 2); // select width component
          idx_minus(wj, wj); // negate all width components
        }
        std::cout << "(vertical-axis mirror) ";
        add_data2(flipped, label, class_name, filename,
                  jittforce ? jittforce : &(*ijit), jsmirr);
        // display
        if (do_preprocessing)
          display_added(flipped, dat, class_name, filename, &inr, r,
                        true, center, visr, cropr, objs,
                        jittforce ? jittforce : &(*ijit), jsmirr);
        // put horizontal jitter back
        ijit->w = -ijit->w;
        // TEMPORARY MEMORY LEAK FIX (use smart srg pointer to clear
        // automatically on object deletion)
        flipped.clear();
      }
      // add/save sample
      add_data2(sample, label, class_name, filename,
                jittforce ? jittforce : &(*ijit), js);
      // display
      if (do_preprocessing)
        display_added(sample, dat, class_name, filename, &inr, r,
                      true, center, visr, cropr, objs,
                      jittforce ? jittforce : &(*ijit), js);
      // delete temp objs
      if (js) delete js;
      // TEMPORARY MEMORY LEAK FIX (use smart srg pointer to clear
      // automatically on object deletion)
      sample.clear();
    }
    return true;
  } eblcatchwarn_extra(return false;);
}

template <typename Tdata>
void dataset<Tdata>::add_data2(midx<Tdata> &sample, t_label label,
                               const std::string *class_name,
                               const char *filename, const jitter *jitt,
                               idx<t_jitter> *js) {
  // check for dimensions
  idx<Tdata> sample0 = sample.mget(0);
  if (!sample0.same_dim(outdims) && !no_outdims) {
    idxdim d2(sample0);
    d2.setdim(2, outdims.dim(2)); // try with same # of channels
    if (d2 == outdims) {
      // same size except for the channel dimension, replicate it
      std::cout << "duplicating image channel (" << sample0;
      std::cout << ") to fit target (" << outdims << ")." << std::endl;
      idx<Tdata> sample2(d2);
      idx_bloop2(samp2, sample2, Tdata, samp, sample0, Tdata) {
        idx_bloop2(s2, samp2, Tdata, s, samp, Tdata) {
          for (intg i = 0, j = 0; i < sample2.dim(2); ++i, ++j) {
            if (j >= sample0.dim(2))
              j = 0;
            s2.set(s.get(j), i);
          }
        }
      }
      sample.mset(sample2, 0);
      // TODO: apply replication to all layers of sample, not just 0
    }
    //       else
    // 	eblthrow("expected data with dimensions " << outdims << " but found "
    // 		 << sample0.get_idxdim() << " in " << filename);
  }
  // put sample's channels dimensions first, if interleaved.
  if (interleaved_input) sample.shift_dim_internal(2, 0);
  // if save_mode is dataset, cpy to dataset, otherwise save individual file
  if (!strcmp(save_mode.c_str(), DATASET_SAVE)) { // dataset mode
    // assign sample to big matrix
    for (uint i = 0; i < sample.dim(0); ++i) {
      idx<Tdata> tmp = sample.mget(i);
      data.mset(tmp, data_cnt, i);
    }
    // copy label
    labels.set(label, data_cnt);
    // copy jitt if present
    if (jitters.order() > 0 && js) jitters.mset(*js, data_cnt);
    // declare that data is being loaded
    dataset_loaded = true;
  } else {
    std::string format = save_mode;
    if (!strcmp(save_mode.c_str(), DYNSET_SAVE)
            || !strcmp(save_mode.c_str(), DATASET_SAVE))
      format = "mat"; // force mat format for dynamic set
    std::string fname_tmp, fname;
    fname_tmp << outtmp << "/" << get_class_string(label) << "/";
    if (individual_save) mkdir_full(fname_tmp.c_str());
    // save image
    fname_tmp << get_class_string(label) << "_" << data_cnt;
    fname << fname_tmp << "." << format;
    //       // scale image to 0 255 if preprocessed
    //       if (strcmp(ppconv_type.c_str(), "RGB")) {
    // 	idx_addc(tmp, (Tdata) 1.0, tmp);
    // 	idx_dotc(tmp, (Tdata) 127.5, tmp);
    //       }
    if (individual_save) {
      if (separate_layers_save && sample.dim(0) > 1) {
        for (uint i = 0; i < sample.dim(0); ++i) {
          std::string fname2;
          idx<Tdata> sample2 = sample.mget(i);
          fname2 << fname_tmp << "_" << i << "." << format;
          save_image(fname2, sample2, format.c_str());
          std::cout << "  saved " << fname2 << " (" << sample2 << ")" << std::endl;
        }
      } else if (sample.dim(0) == 1) {
        idx<Tdata> tmp = sample.mget(0);
        if (!strcmp(format.c_str(), "mat"))
          save_matrix(tmp, fname);
        else {
          tmp = tmp.shift_dim(0, 2);
          save_image(fname, tmp, format.c_str());
        }
        std::cout << "  saved " << fname << " (" << sample << ")" << std::endl;
      } else {
        if (!strcmp(format.c_str(), "mat")) {
          save_matrices(sample, fname);
          std::cout << "  saved " << fname << " (" << sample << ")" << std::endl;
        } else
          eblerror("cannot save multi-layer image into format " << format);
      }
    }
    //      if (!strcmp(save_mode.c_str(), DYNSET_SAVE)) {
    if (individual_save) // keep new path
      images_list.push_back(fname); // add image to files list
    else // keep original path
      images_list.push_back(filename); // add image to files list
    //  }
  }
  add_label(label, class_name, filename, jitt, js);
  std::cout << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::add_label(t_label label, const std::string *class_name,
                               const char *filename, const jitter *jitt,
                               idx<t_jitter> *js) {
  std::string err;
  err << "not adding " << (class_name?*class_name:"sample")
      << " from " << (filename?filename:"");
  // check for capacity
  //    if (!strcmp(save_mode.c_str(), DATASET_SAVE) && full(label))
  if (full(label))
    eblthrow(err << ", reached full capacity for this class.");
  if (!included(label))
    eblthrow(err << ", class not included");
  // increase counter for that class
  add_tally.set(add_tally.get(label) + 1, label);
  // print info
  if (data_cnt % 50 == 0) {
    std::cout << data_cnt+1 << " / " << total_samples << ": add "
              << (filename ? filename : "sample" );
    if (class_name)
      std::cout << " as " << *class_name;
    std::cout << " (" << label << ")";
    //    std::cout << " eta: " << xtimer.eta(processed_cnt, total_samples);
    std::cout << " eta: " << xtimer.eta(data_cnt+1, total_samples);
    std::cout << " elapsed: " << xtimer.elapsed();
    if (jitt)
      std::cout << " (jitter " << jitt->h << "," << jitt->w << "," << jitt->s << ","
                << jitt->r << ")";
  }
  // copy label
  if (labels.dim(0) > data_cnt)
    labels.set(label, data_cnt);
  // increment data count
  data_cnt++;
}

template <typename Tdata>
void dataset<Tdata>::display_added(midx<Tdata> &added, idx<Tdata> &original,
                                   const std::string *class_name,
                                   const char *filename,
                                   const rect<int> *inr,
                                   const rect<int> *origr,
                                   bool active_sleepd,
                                   std::pair<int,int> *center,
                                   const rect<int> *visr,
                                   const rect<int> *cropr,
                                   const std::vector<object*> *objs,
                                   const jitter *jitt,
                                   idx<t_jitter> *js,
                                   uint *woriginal) {
  // display each step
#ifdef __GUI__
  if (display_extraction) {
    disable_window_updates();
    clear_window();
    uint h = 0, w = 0;
    uint dh = 0, dw = 1;
    ostringstream oss;
    // display resized
    oss.str("");
    h = 16;
    gui << gui_only() << black_on_white();
    //       // print output sizes
    //       std::string ssizes;
    //       for (uint i = 0; i < added.dim(0); ++i) {
    // 	idx<Tdata> tmp = added.get(i);
    // 	ssizes << tmp << " ";
    //       }
    //       gui << at(h, w) << ssizes;
    //       h += 16;
    //       // draw original output before channel formatting in RGB
    //       oss.str("");
    //       oss << "RGB";
    //       draw_matrix(original, oss.str().c_str(), h, w);
    //       h += original.dim(dh) + 5;
    Tdata minval, maxval;
    ppmods[0]->get_display_range(minval, maxval);
    // draw output in RGB
    //      gui << at(h, w) << pp_names; h += 15;
    idx<Tdata> added0 = added.mget(0);
    if ((added0.dim(0) == 3 || added0.dim(0) == 1) && fovea.size() == 0) {
      idx<Tdata> tmp = added0.shift_dim(0, 2);
      draw_matrix(tmp, h, w, 1, 1, minval, maxval);
      // draw crossing arrows at center
      draw_cross(h + tmp.dim(dh)/(float)2, w + tmp.dim(dw)/(float)2,10,0,0,0);
      // draw jitter boxes
      if (js) {
        idx_bloop1(jj, *js, t_jitter) {
          rect<float> jr(h + jj.get(1) * height, w + jj.get(2) * height,
                         height, width);
          float scale = jj.get(0);
          scale = 1 / scale;
          jr.scale_centered(scale, scale);
          draw_box(jr.h0, jr.w0, jr.height, jr.width, 0, 0, 255);
        }
      }
      h += tmp.dim(dh) + 5;
    }
    // display all channels
    uint wtmp = w, maxw = 0, layers = 0, ppi = 0;
    for (uint i = 0; i < added.dim(0); ++i, layers++) {
      w = wtmp;
      std::string name;
      if (layers == ppmods[ppi]->nlayers()) {
        if (ppi < ppmods.size() - 1)
          ppi++;
        layers = 0;
      }
      if (layers == 0) {
        gui << at(h, w) << ppmods[ppi]->name();
        h += 16;
        ppmods[ppi]->get_display_range(minval, maxval);
      }
      idx<Tdata> addedi = added.mget(i);
      gui << at(h, w) << addedi;
      idx<Tdata> tmp = addedi.shift_dim(0, 2);
      w += 100;
      draw_matrix(tmp, h, w, 1, 1, minval, maxval);
      idx_bloop1(chan, addedi, Tdata) {
        w += addedi.dim(dw + 1) + 5;
        draw_matrix(chan, h, w, 1, 1, minval, maxval);
        // 	// display channel's center
        // draw_box(h + chan.dim(0) / 2 - 5, w + chan.dim(1) / 2 - 5, 10, 10,
        // 		 0, 0, 255);
      }
      w += addedi.dim(dw + 1) + 5;
      h += addedi.dim(dh + 1) + 5;
      maxw = std::max(maxw, w);
    }
    h = 0;
    w = maxw + 5;
    if (woriginal)
      *woriginal = w;
    // display original
    gui << gui_only() << black_on_white();
    oss.str("");
    if (filename) {
      oss << "file: " << filename;
      gui << gui_only() << black_on_white();
      gui << at(h, w) << oss.str();
    }
    h += 16;
    oss.str("");
    oss << "adding sample #" << data_cnt+1 << " / " << total_samples;
    gui << at(h, w) << oss.str(); h += 16;
    std::string desc;
    desc << "input: " << original;
    if (jitt)
      desc << " jitter: spatial " << jitt->h << "x" << jitt->w
           << " scale " << jitt->s << " rot " << jitt->r;
    gui << at(h, w) << desc; h += 16;
    // if image was cropped, shift everything by cropping offsets
    if (cropr) {
      h += cropr->h0;
      w += cropr->w0;
    }
    // draw image
    draw_matrix(original, h, w);
    // draw object's center
    if (center)
      draw_cross(h + center->second, w + center->first, 10, 0, 0, 255);
    // draw all objects in picture
    if (objs) {
      for (uint i = 0; i < objs->size(); ++i) {
        const object &o = *(*objs)[i];
        if (o.ignored) // ignored object
          draw_box(h + o.h0, w + o.w0, o.height, o.width, 255, 255, 0);
        else // used object
          draw_box(h + o.h0, w + o.w0, o.height, o.width, 0, 255, 255);
      }
    }
    // draw object's original box
    if (origr) {
      draw_box(h + origr->h0, w + origr->w0, origr->height, origr->width,
               255, 0, 0);
      draw_cross(h + origr->hcenter(), w + origr->wcenter(), 20, 255, 0, 0);
    }
    // draw object's factored box if factor != 1.0
    if (inr) {
      draw_box(h + inr->h0, w + inr->w0, inr->height, inr->width, 0, 255, 0);
      draw_cross(h + inr->hcenter(), w + inr->wcenter(), 20, 0, 255, 0);
    }
    // draw all
    for (uint i = 0; i < ppmods.size(); ++i) {
      resizepp_module<Tdata> *resizepp = ppmods[i];
      const std::vector<rect<int> > &boxes = resizepp->get_input_bboxes();
      for (uint i = 0; i < boxes.size(); ++i) {
        const rect<int> &b = boxes[i];
        draw_box(h + b.h0, w + b.w0, b.height, b.width, 255, 0, 255);
      }
    }
    // draw object's label
    if (class_name && inr)
      gui << white_on_transparent() << at(h + inr->h0, w + inr->w0)
          << *class_name;
    // draw object's visible area if presnet
    if (visr)
      draw_box(h + visr->h0, w + visr->w0, visr->height, visr->width,
               0, 0, 255);
    //h += dat.dim(dh) + 5;
    w += original.dim(dw) + 5;
    oss.str("");
    // display object
    if (inr) {
      idx<Tdata> obj = original;
      int offh = std::max((int)0, inr->h0);
      int offw = std::max((int)0, inr->w0);
      obj = obj.narrow(dh, std::min((int)obj.dim(0) - offh, inr->height), offh);
      obj = obj.narrow(dw, std::min((int)obj.dim(1) - offw, inr->width), offw);
      // display object
      if (class_name)
        oss << *class_name << " ";
      oss << obj;
      draw_matrix(obj, oss.str().c_str(), h, w);
      // draw crossing arrows at center
      //       draw_box(h + inr.height/2, w + inr.width/2, inr.height/2,
      // 	       inr.width/2, 0,0,0);
      //       gui << black_on_white() << at(h + inr.height, w) << oss.str();
    }
    // paint
    enable_window_updates();
    if (sleep_display && active_sleepd)
      millisleep((long) sleep_delay);
    if (bsave_display) {
      ostringstream oss("");
      oss << save_display_dir << "frame" << setw(6) << setfill('0')
          << data_cnt;
      save_window(oss.str().c_str());
    }
  }
#endif
}

template <typename Tdata>
void dataset<Tdata>::set_unique_label(const std::string &class_name) {
  // allocate labels matrix based on images list size
  if (labels.order() != 1 && images_list.size() > 0)
    labels = idx<t_label>(images_list.size());
  idx_clear(labels);
  clear_classes();
  add_class(class_name);
}

template <typename Tdata>
void dataset<Tdata>::clear_classes() {
  nclasses = 0;
  classes.clear();
}

template <typename Tdata>
bool dataset<Tdata>::add_class(const std::string &class_name, bool sort) {
  std::vector<std::string>::iterator res;
  std::string name = class_name;
  res = find(classes.begin(), classes.end(), name);
  if (res == classes.end()) {// not found
    classes.push_back(name);
    nclasses++;
  } else { // found
    //t_label i = res - classes.begin();
    //      std::cout << "found class " << name << " at index " << i << std::endl;
  }
  // sort all classes
  if (sort) std::sort(classes.begin(), classes.end(), natural_compare_less);
  return true;
}

template <typename Tdata>
void dataset<Tdata>::set_classes(idx<ubyte> &classidx, bool sort) {
  // add classes to each dataset
  std::string s;
  idx_bloop1(classe, classidx, ubyte) {
    s = (const char *) classe.idx_ptr();
    add_class(s, sort);
  }
  // init max_per_class
  max_per_class = idx<intg>(classes.size());
  max_per_class_set = false;
  idx_fill(max_per_class, (std::numeric_limits<intg>::max)());
}

template <typename Tdata>
void dataset<Tdata>::set_outdims(const idxdim &d) {
  no_outdims = false;
  outdims = d;
  uint featdims = 0;
  if (outdims.order() > 1) {
    if (interleaved_input || (outdims.order() == 2)) {
      height = outdims.dim(0);
      width = outdims.dim(1);
      featdims = 2;
    } else {
      height = outdims.dim(1);
      width = outdims.dim(2);
      featdims = 0;
    }
  }
  // update outdims' feature size with fovea factor
  if (outdims.order() > 0 && fovea.size() > 0)
    outdims.setdim(featdims, outdims.dim(featdims) * fovea.size());
  std::cout << "Setting target dimensions to " << outdims << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_outdir(const char *s, const char *tmp) {
  outdir = s;
  if (tmp)
    outtmp = tmp;
  if (outtmp.empty())
    outtmp = outdir;
  std::cout << "Setting output directory to " << outdir << std::endl;
  std::cout << "Setting temporary output directory to " << outtmp << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_mindims(const idxdim &d) {
  std::cout << "Setting minimum input dimensions to " << d << std::endl;
  mindims = d;
}

template <typename Tdata>
void dataset<Tdata>::set_maxdims(const idxdim &d) {
  std::cout << "Setting maximum input dimensions to " << d << std::endl;
  maxdims = d;
  maxdims_set = true;
}

template <typename Tdata>
void dataset<Tdata>::set_scales(const std::vector<double> &sc, const std::string &od) {
  scales = sc;
  scale_mode = true;
  outtmp = od;
  std::cout << "Enabling scaling mode. Scales: ";
  for (std::vector<double>::iterator i = scales.begin(); i != scales.end(); ++i)
    std::cout << *i << " ";
  std::cout << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_fovea(const std::vector<double> &sc) {
  fovea = sc;
  std::cout << "Enabling fovea mode with scales: " << fovea << std::endl;
  // set the number of layers the pyramid will produce
  //HERE
  nlayers = fovea.size();
  // update outdims' feature size
  set_outdims(outdims);
}

template <typename Tdata>
void dataset<Tdata>::set_max_per_class(intg max) {
  if (max < 0)
    eblerror("cannot set max_per_class to < 0");
  if (max > 0) {
    mpc = max;
    max_per_class_set = true;
    max_per_class = idx<intg>(nclasses);
    idx_fill(max_per_class, mpc);
    std::cout << "Max number of samples per class: " << max << std::endl;
  }
}

template <typename Tdata>
void dataset<Tdata>::set_max_data(intg max) {
  if (max < 0)
    eblerror("cannot set max_data to < 0");
  if (max > 0) {
    max_data = max;
    max_data_set = true;
    std::cout << "Max number of samples: " << max << std::endl;
  }
}

template <typename Tdata>
void dataset<Tdata>::set_image_pattern(const std::string &p) {
  extension = p;
  std::cout << "Setting image pattern to " << extension << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_exclude(const std::vector<std::string> &ex) {
  if (ex.size()) {
    std::cout << "Excluded class(es): ";
    for (std::vector<std::string>::const_iterator i = ex.begin(); i != ex.end(); ++i) {
      exclude.push_back(*i);
      if (i != ex.begin()) std::cout << ",";
      std::cout << " " << *i;
    }
    std::cout << std::endl;
  }
}

template <typename Tdata>
void dataset<Tdata>::set_include(const std::vector<std::string> &inc) {
  if (inc.size()) {
    std::cout << "Included class(es): ";
    for (std::vector<std::string>::const_iterator i = inc.begin(); i != inc.end(); ++i){
      include.push_back(*i);
      if (i != inc.begin()) std::cout << ",";
      std::cout << " " << *i;
    }
    std::cout << std::endl;
  }
}

template <typename Tdata>
void dataset<Tdata>::set_save(const std::string &s) {
  save_mode = s;
  std::cout << "Setting saving mode to: " << save_mode << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_individual_save(bool b) {
  individual_save = b;
  std::cout << (individual_save ? "Enabling" : "Disabling")
            << " individual sample saving." << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_separate_layers_save(bool b) {
  separate_layers_save = b;
  std::cout << (separate_layers_save ? "Enabling" : "Disabling")
            << " saving sample layers separately." << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_name(const std::string &s) {
  name = s;
  build_fname(name, DATA_NAME, data_fname);
  build_fname(name, LABELS_NAME, labels_fname);
  build_fname(name, SCALES_NAME, ids_fname);
  build_fname(name, JITTERS_NAME, jitters_fname);
  build_fname(name, CLASSES_NAME, classes_fname);
  build_fname(name, CLASSPAIRS_NAME, classpairs_fname);
  build_fname(name, DEFORMPAIRS_NAME, deformpairs_fname);
  std::cout << "Setting dataset name to: " << name << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_label(const std::string &s) {
  force_label = s;
  //add_class(force_label);
  std::cout << "Forcing label for all samples to: " << s << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_bbox_woverh(float factor) {
  bbox_woverh = factor;
  std::cout << "Forcing width to be h * " << bbox_woverh << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_nopadded(bool nopadded_) {
  nopadded = nopadded_;
  if (nopadded)
    std::cout << "Ignoring samples that have padding areas, i.e. too"
              << " small for target size." << std::endl;
}
template <typename Tdata>
void dataset<Tdata>::set_videobox(uint nframes, uint stride) {
  do_videobox = true;
  videobox_nframes = nframes;
  videobox_stride = stride;
}

template <typename Tdata>
void dataset<Tdata>::
set_jitter(uint tjitter_step_, uint tjitter_hmin_, uint tjitter_hmax_,
           uint tjitter_wmin_, uint tjitter_wmax_, uint scale_steps,
           float scale_min, float scale_max, uint rotation_steps,
           float rotation_range, uint n) {
  bjitter = true;
  tjitter_step = (int) tjitter_step_;
  tjitter_hmin = (int) tjitter_hmin_;
  tjitter_hmax = (int) tjitter_hmax_;
  tjitter_wmin = (int) tjitter_wmin_;
  tjitter_wmax = (int) tjitter_wmax_;
  sjitter_steps = (int) scale_steps;
  sjitter_min = scale_min;
  sjitter_max = scale_max;
  if (sjitter_min > sjitter_max)
    eblerror("expected max > min but got " << sjitter_max << " < "
             << sjitter_min);
  rjitter_steps = (int) rotation_steps;
  rjitter = rotation_range;
  njitter = n;
  std::cout << "Adding " << n << " samples randomly jittered in a neighborhood "
            << " with height range (" << tjitter_hmin
            << "," << tjitter_hmax << "), width range (" << tjitter_wmin
            << "," << tjitter_wmax << ") and step " << tjitter_step << ", over "
            << scale_steps << " scales"
            << " within a [" << scale_min << "," << scale_max
            << "] scale range and "
            << rjitter_steps << " rotations within a " << rjitter
            << " degrees rotation range around original location"
            << "/scale/orientation" << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::set_minvisibility(float minvis) {
  std::cout << "Setting minimum visibility ratio (visible bbox overlap with "
            << "original bbox) to " << minvis << std::endl;
  minvisibility = minvis;
}

template <typename Tdata>
void dataset<Tdata>::set_wmirror() {
  wmirror = true;
  std::cout << "Adding vertical-axis mirror." << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::save_display(const std::string &dir, uint h, uint w) {
#ifdef __GUI__
  bsave_display = true;
  save_display_dir = dir;
  save_display_dir << "/";
  mkdir_full(dir.c_str());
  std::cout << "Saving display frames ";
  if (h != 0 && w != 0) {
    freeze_window_size(h, w);
    std::cout << "(fixed size: " << h << "x" << w << ") ";
  }
  std::cout << "to " << dir << std::endl;
#endif
}

template <typename Tdata>
void dataset<Tdata>::use_pose() {
  usepose = true;
  std::cout << "Using pose to separate classes." << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::use_parts() {
  useparts = true;
  std::cout << "Extracting parts." << std::endl;
}

template <typename Tdata>
void dataset<Tdata>::use_parts_only() {
  usepartsonly = true;
  std::cout << "Extracting parts only." << std::endl;
}

template <typename Tdata>
intg dataset<Tdata>::count_total() {
  // count samples
  this->count_samples();
  std::cout << "Found: " << total_samples << " total samples." << std::endl;
  if (!total_samples) eblerror("no samples found");
  if (njitter > 0) {
    total_samples *= njitter;
    std::cout << "Jitter is on with " << njitter << " jitters per sample, "
              << "bringing total samples to " << total_samples << std::endl;
  }
  if (wmirror) {
    total_samples *= 2;
    std::cout << "Vertical-axis mirroring is on, "
              << "bringing total samples to " << total_samples << std::endl;
  }
  return total_samples;
}

template <typename Tdata>
intg dataset<Tdata>::count_samples() {
#ifdef __BOOST__
  std::cout << "Counting number of samples in " << inroot << " ..." << std::endl;
  total_samples = 0;
  boost::regex hidden_dir(".svn");
  boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
  boost::filesystem::path p(inroot);
  if (!exists(p)) eblthrow("path " << inroot << " does not exist.");
  // loop over all directories
  std::vector<std::string> dirs;
  for (boost::filesystem::directory_iterator itr(inroot); itr != end_itr; itr++) {
#if !defined(BOOST_FILESYSTEM_VERSION) || BOOST_FILESYSTEM_VERSION == 3
    if (boost::filesystem::is_directory(itr->status())
            && !boost::regex_match(itr->path().filename().string(), hidden_dir)) {
      // ignore excluded classes and use included if defined
      if (included(itr->path().filename().string())) {
        dirs.push_back(itr->path().filename().string());
        // recursively search each directory
        total_samples += count_matches(itr->path().string(), extension);
      }
    }
#else
    if (boost::filesystem::is_directory(itr->status())
            && !boost::regex_match(itr->path().filename().c_str(), hidden_dir)) {
      // ignore excluded classes and use included if defined
      if (included(itr->path().filename().string())) {
        dirs.push_back(itr->path().filename().c_str());
        // recursively search each directory
        total_samples += count_matches(itr->path().string(), extension);
      }
    }
#endif
  }
  // sort directories
  std::sort(dirs.begin(), dirs.end(), natural_compare_less);
  // process subdirs to extract images into the single image idx
  for (uint i = 0; i < dirs.size(); ++i) {
    // add directory as new class
    add_class(dirs[i]);
  }
#endif /* __BOOST__ */
  return total_samples;
}

template <typename Tdata>
void dataset<Tdata>::split(dataset<Tdata> &ds1, dataset<Tdata> &ds2) {
  // data already preprocessed
  ds1.do_preprocessing = false;
  ds2.do_preprocessing = false;
  ds1.interleaved_input = false;
  ds2.interleaved_input = false;
  // enable jitter in target if present here
  bjitter = (jitters.order() > 0);
  if (bjitter) {
    ds1.bjitter = true;
    ds2.bjitter = true;
  }
  std::cout << "Input data samples: " << data << std::endl;
  // alloc each dataset
  if (!ds1.allocate(data.dim(0), outdims) ||
      !ds2.allocate(data.dim(0), outdims))
    eblerror("Failed to allocate new datasets");
  // add samples 1st dataset, if not add to 2nd.
  // if 1st has reached max per class, it will return false upon addition
  std::cout << "Adding data to \"" << ds1.name << "\" and \""
	    << ds2.name << "\"." << std::endl;
  // using the shuffle() method is a problem with big datasets because
  // it requires allocation of an extra dataset.
  // instead, we use a random list of indices to assign the first random
  // samples to dataset 1 and the remaining to dataset 2.
  std::vector<intg> ids;
  idx<t_jitter> jitt;
  t_label label;
  // prepare offset matrices
  idx<int64> offsets = data.get_offsets();
  idx<int64> off1(offsets.get_idxdim()), off2(offsets.get_idxdim());
  idx_clear(off1);
  idx_clear(off2);
  // init timers
  ds1.xtimer.start();
  ds2.xtimer.start();
  // loop on all samples
  for (intg i = 0; i < data.dim(0); ++i) ids.push_back(i);
  random_shuffle(ids.begin(), ids.end());
  for (std::vector<intg>::iterator i = ids.begin(); i != ids.end(); ++i) {
    label = labels.get(*i);
    std::string &class_name = classes[(size_t)label];
    std::cout << "(original index " << *i << ") ";
    // adding to ds1
    try {
      intg cnt = ds1.data_cnt;
      ds1.add_label(label, &class_name, NULL, NULL, NULL);
      std::cout << " (dataset 1)" << std::endl;
      // copy offsets
      idx<int64> offs = offsets[*i];
      idx<int64> offss = off1[cnt];
      idx_copy(offs, offss);
      if (bjitter && jitters.exists(*i)) {
        jitt = jitters.mget(*i);
        ds1.jitters.mset(jitt, cnt);
      }
    } catch(eblexception &e) { // adding to ds2
      try {
        intg cnt = ds2.data_cnt;
        ds2.add_label(label, &class_name, NULL, NULL, NULL);
        std::cout << " (dataset 2), not 1:";
#ifndef __NOEXCEPTIONS__
        std::cout << e << std::endl;
#endif
        // copy offsets
        idx<int64> offs = offsets[*i];
        idx<int64> offss = off2[cnt];
        idx_copy(offs, offss);
        if (bjitter && jitters.exists(*i)) {
          jitt = jitters.mget(*i);
          ds2.jitters.mset(jitt, cnt);
        }
      } eblcatchwarn_msg("failed adding sample");
    }
  }
  // set data matrices from original data and offset matrices
  // ds1.data_cnt = idx_sum(ds1.add_tally);
  // ds2.data_cnt = idx_sum(ds2.add_tally);
  if (off1.order() == 1) {
    off1.resize(ds1.data_cnt);
    off2.resize(ds2.data_cnt);
  }
  else {
    off1.resize(ds1.data_cnt, off1.dim(1));
    off2.resize(ds2.data_cnt, off2.dim(1));
  }
  ds1.data = midx<Tdata>(off1.get_idxdim(), data.get_file_pointer(), &off1);
  ds2.data = midx<Tdata>(off2.get_idxdim(), data.get_file_pointer(), &off2);

  print_stats();
  ds1.print_stats();
  ds2.print_stats();
}

template <typename Tdata>
void dataset<Tdata>::merge_and_save(const char *name1, const char *name2,
                                    const std::string &outroot) {
  dataset<Tdata> ds1(name1), ds2(name2);
  std::string inroot1 = ebl::dirname(name1);
  std::string inroot2 = ebl::dirname(name2);
  // load 2 datasets
  ds1.load(inroot1);
  ds2.load(inroot2);
  if (ds1.jitters.order() > 0 || ds2.jitters.order() > 0) {
    njitter = 1; // enable jitter allocation/saving
    bjitter = true;
  }
  interleaved_input = false;
  intg newsz = ds1.size() + ds2.size();
  idxdim d1 = ds1.get_sample_outdim(), d2 = ds2.get_sample_outdim();
  if (!(d1 == d2))
    eblwarn("sample sizes for dataset 1 and 2 are different: "
            << d1 << " and " << d2);
  // allocate new dataset
  allocate(newsz, d1);
  idx<t_label> labelsnew;
  idx<intg> idsnew;
  midx<t_jitter> jittnew;
  // clear jitters, in case a dataset doesn't have any
  idx_clear(jitters);
  // update classes
  idx<ubyte> classidx = ds1.build_classes_idx(ds1.classes);
  set_classes(classidx, false); // initialize with ds1's class names
  std::cout << "Added all classes to new dataset from " << ds1.name << std::endl;
  // for each ds2 class name, push on new class names vector if not found
  std::vector<std::string>::iterator res, i;
  for (i = ds2.classes.begin(); i != ds2.classes.end(); ++i){
    res = find(classes.begin(), classes.end(), *i);
    if (res == classes.end()) { // not found
      classes.push_back(*i); // add new class name
      nclasses++;
      std::cout << "Adding class " << *i << " from dataset " << ds2.name << std::endl;
    }
  }
  // update each ds2 label based on new class numbering
  idx_bloop1(lab, ds2.labels, t_label) {
    std::string s = ds2.get_class_string(lab.get());
    lab.set(get_class_id(s));
  }
  // copy data 1 into new dataset
  labelsnew = labels.narrow(0, ds1.size(), 0);
  idx_copy(ds1.labels, labelsnew);
  idsnew = ids.narrow(0, ds1.size(), 0);
  idx_copy(ds1.ids, idsnew);
  if (bjitter)
    jittnew = jitters.narrow(0, ds1.size(), 0);
  if (ds1.jitters.order() > 0)
    for (uint i = 0; i < ds1.jitters.dim(0); ++i) {
      if (ds1.jitters.exists(i)) {
        idx<t_jitter> tmp = ds1.jitters.mget(i);
        jittnew.mset(tmp, i);
      }
    }
  // copy data 2 into new dataset
  labelsnew = labels.narrow(0, ds2.size(), ds1.size());
  idx_copy(ds2.labels, labelsnew);
  idsnew = ids.narrow(0, ds2.size(), ds1.size());
  idx_copy(ds2.ids, idsnew);
  if (bjitter)
    jittnew = jitters.narrow(0, ds2.size(), ds1.size());
  if (ds2.jitters.order() > 0)
    for (uint i = 0; i < ds2.jitters.dim(0); ++i) {
      if (ds2.jitters.exists(i)) {
        idx<t_jitter> tmp = ds2.jitters.mget(i);
        jittnew.mset(tmp, i);
      }
    }
  // update counter
  data_cnt = newsz;
  std::cout << "Copied data from " << ds1.name << " and " << ds2.name;
  std::cout << " into new dataset." << std::endl;
  // print info
  print_stats();
  // save data
  std::string fname, fname_tmp;
  fname << outroot << "/" << data_fname;
  fname_tmp << fname << "_tmp";
  mkdir_full(outroot.c_str());
  std::cout << "Saving " << fname << std::endl;
  save_matrices(ds1.data, ds2.data, fname_tmp);
  if (!mv_file(fname_tmp.c_str(), fname.c_str()))
    eblerror("failed to move " << fname_tmp << " to " << fname);
  // move from tmp to target name
  std::cout << "Saving dataset in " << outroot << std::endl;
  // save rest
  save(outroot, false);
}

template <typename Tdata> template <class Toriginal>
bool dataset<Tdata>::save_scales(idx<Toriginal> &dat, const std::string &filename){
  // copy data into target type
  idxdim d(dat);
  idx<Tdata> sample(d);
  idx_copy(dat, sample);
  // do preprocessing for each scale, then save image
  std::ostringstream base_name, ofname;
  base_name << outtmp << "/" << filename << "_scale";
  std::string class_name = "noclass";
  for (std::vector<double>::iterator i = scales.begin(); i != scales.end(); ++i) {
    idx<Tdata> s = preprocess_data(sample, &class_name,
                                   filename.c_str(), NULL, *i);
    // put sample's channels dimensions first, if defined.
    //s = s.shift_dim(2, 0);
    // save image
    ofname.str(""); ofname << base_name.str() << *i << ".mat";
    if (save_matrix(s, ofname.str())) {
      std::cout << data_cnt++ << ": saved " << ofname.str();
      std::cout << "(" << s << ")" << std::endl;
    }
  }
  return true;
}

template <typename Tdata>
bool dataset<Tdata>::included(t_label &lab) {
  if (classes.size() == 0) return false;
  if ((size_t) lab >= classes.size()) return false;
  std::string &class_name = classes[(size_t)lab];
  if (!included(class_name)) return false;
  return true;
}

template <typename Tdata>
bool dataset<Tdata>::included(const std::string &class_name) {
  return // is not excluded
      find(exclude.begin(), exclude.end(), class_name) == exclude.end()
      // and is included
      && ((find(include.begin(), include.end(), class_name) != include.end())
          // or everything is included
          || (include.size() == 0));
}

////////////////////////////////////////////////////////////////
// data preprocessing

template <typename Tdata>
midx<Tdata> dataset<Tdata>::
preprocess_data(midx<Tdata> &dat, const std::string *class_name,
                const char *filename, const rect<int> *r, double scale,
                rect<int> *outr,
                std::pair<int,int> *center, jitter *jitt,
                const rect<int> *visr,
                const rect<int> *cropr,
                rect<int> *inr_out) {
  // input region
  idx<Tdata> dat0 = dat.mget(0);

  rect<int> inr(0, 0, dat0.dim(0), dat0.dim(1));
  if (r) inr = *r;
  // force width to be h * bbox_woverh
  if (bbox_woverh > 0)
    inr.scale_width(bbox_woverh);
  // resize image to target dims
  rect<int> out_region, cropped;
  idxdim d(outdims);
  // input data
  idx<Tdata> tmp = dat0.shift_dim(2, 0);
  state<Tdata> in(tmp.get_idxdim());
  // allocate sample with videobox options in mind
  midx<Tdata> sample(nlayers * (1 + videobox_nframes));
  uint nadded = 0;
  std::string next_file = filename;
  for (uint i = 0; i < videobox_nframes + 1; ++i) {
    //copy the current image into in.x
    idx_copy(tmp, in);
    // loop on all preprocessing modules
    for (uint i = 0; i < ppmods.size(); ++i) {
      resizepp_module<Tdata> *resizepp = ppmods[i];
      midx<Tdata> sampletmp(resizepp->nlayers());
      // add jitter
      if (jitt)
        resizepp->set_jitter((int)jitt->h, (int)jitt->w, jitt->s, jitt->r);
      if (scale > 0) // resize entire image at specific scale
        resizepp->set_dimensions((uint) (outdims.dim(0) * scale),
                                 (uint) (outdims.dim(1) * scale));
      resizepp->set_input_region(inr);
      // actual preprocessing
      resizepp->fprop(in, sampletmp);
      // remember bbox of original image in resized image
      original_bbox = resizepp->get_original_bbox();
      if (outr)
        *outr = original_bbox;
      if (inr_out)
        *inr_out = resizepp->get_input_bbox();
      for (uint j = 0; j < sampletmp.dim(0); ++j) {
        idx<Tdata> stmp = sampletmp.mget(j);
        sample.mset(stmp, j + nadded);
      }
      nadded += resizepp->nlayers();
    }
    if (do_videobox) {
      // Load the next image into tmp
      load_img.clear();
      // if file doesn't exist, load_data automatically throws exception
      // which makes the sample to be not added
      next_file = ebl::increment_filename(next_file.c_str(), videobox_stride);
      load_data(next_file);
      dat0 = load_img.mget(0);
      if (cropr) {
        dat0 = dat0.narrow(0, cropr->height, cropr->h0);
        dat0 = dat0.narrow(1, cropr->width, cropr->w0);
      }
      dat0.shift_dim(2, 0);
      tmp = dat0.shift_dim(2, 0);
    }
  }
  sample.shift_dim_internal(0, 2);
  // return preprocessed image
  return sample;
}

////////////////////////////////////////////////////////////////
// Helper functions

template <typename Tdata>
void dataset<Tdata>::compute_stats() {
  // collect stats
  if (nclasses > 0) {
    class_tally = idx<intg>(nclasses);
    idx_clear(class_tally);
    for (intg i = 0; i < data_cnt && i < labels.dim(0); ++i) {
      class_tally.set(class_tally.get(labels.get(i)) + 1,
                      (intg) labels.get(i));
    }
  }
}

template <typename Tdata>
idx<ubyte> dataset<Tdata>::
build_classes_idx(std::vector<std::string> &classes) {
  // determine max length of strings
  uint max = 0;
  std::vector<std::string>::iterator i = classes.begin();
  for ( ; i != classes.end(); ++i)
    max = (std::max)((size_t) max, i->length());
  // allocate classes idx
  idx<ubyte> classes_idx(classes.size(), max + 1);
  // copy classes strings
  idx_clear(classes_idx);
  idx<ubyte> tmp;
  for (i = classes.begin(); i != classes.end(); ++i) {
    tmp = classes_idx.select(0, i - classes.begin());
    memcpy(tmp.idx_ptr(), i->c_str(), i->length() * sizeof (ubyte));
  }
  return classes_idx;
}

template <typename Tdata>
void dataset<Tdata>::set_index_classes() {
  classes.clear();
  std::ostringstream o;
  int imax = (int) idx_max(labels);
  for (int i = 0; i <= imax; ++i) {
    o << i;
    classes.push_back(o.str());
    o.str("");
  }
}

template <typename Tdata>
std::string& dataset<Tdata>::get_class_string(t_label id) {
  if (((int) id < 0) || ((uint) id >= classes.size()))
    eblerror("trying to access a class with wrong id.");
  return classes[id];
}

template <typename Tdata>
t_label dataset<Tdata>::get_class_id(const std::string &name) {
  std::vector<std::string>::iterator res;
  res = find(classes.begin(), classes.end(), name);
  if (res == classes.end()) // not found
    eblerror("class not found");
  return (t_label) (res - classes.begin());
}

// Recursively goes through dir, looking for files matching extension ext.
template <typename Tdata>
uint dataset<Tdata>::count_matches(const std::string &dir,
                                   const std::string &pattern) {
  uint total = 0;
#ifdef __BOOST__
  boost::regex eExt(pattern);
  boost::filesystem::path p(dir);
  if (!exists(p))
    return 0;
  boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
  for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
    if (boost::filesystem::is_directory(itr->status()))
      total += count_matches(itr->path().string(), pattern);
    else if (boost::regex_match(itr->path().filename().string(), eExt))
      total++;
  }
#endif /* __BOOST__ */
  return total;
}

template <typename Tdata>
void dataset<Tdata>::process_dir(const std::string &dir,
                                 const std::string &ext,
                                 const std::string &class_name_) {
#ifdef __BOOST__
  std::string class_name = class_name_;
  t_label label = get_label_from_class(class_name);
  boost::regex r(ext);
  boost::filesystem::path p(dir);
  if (!exists(p))
    return ;
  boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
  for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
    if (boost::filesystem::is_directory(itr->status()))
      process_dir(itr->path().string(), ext, class_name);
    else if (boost::regex_match(itr->path().filename().string(), r)) {
	try {
	  processed_cnt++;
	  // if full for this class, skip this directory
	  if ((full(get_label_from_class(class_name)) || !included(class_name)))
	    break ;
	  // load data
	  load_data(itr->path().string());
	  // add sample data
          // 	  if (scale_mode) // saving image at different scales
          // 	    save_scales(load_img, itr->path().filename());
          // 	  else // adding data to dataset
	  this->add_mdata(load_img, label, &class_name,
                          itr->path().string().c_str());
	  load_img.clear();
	} catch(const char *err) {
	  std::cerr << "error: failed to add " << itr->path().string();
	  std::cerr << ": " << std::endl << err << std::endl;
	  add_errors++;
	} catch(std::string &err) {
	  std::cerr << "error: failed to add " << itr->path().string();
	  std::cerr << ": " << std::endl << err << std::endl;
	  add_errors++;
	}
      }}
#endif /* __BOOST__ */
  }

  template <typename Tdata>
      void dataset<Tdata>::load_data(const std::string &fname) {
    load_img.clear();
    if (has_multiple_matrices(fname.c_str())) {
      load_img = load_matrices<Tdata>(fname.c_str());
    } else {
      load_img.clear();
      idx<Tdata> tmp = load_image<Tdata>(fname.c_str());
      load_img.mset(tmp, 0);
    }
    if (load_planar)
      load_img.shift_dim_internal(0, 2);
  }

  template <typename Tdata>
      void dataset<Tdata>::compute_random_jitter() {
    // compute all possible jitters
    random_jitter.clear();
    // scale step
    float sstep = (sjitter_max - sjitter_min)
        / std::max((int) 0, (int) sjitter_steps - 1);
    // min/max rotation jitter
    float min_rjitt = 0.0 - rjitter / 2;
    float max_rjitt = 0.0 + rjitter / 2;
    // rotation step
    float rstep = rjitter / std::max((int) 0, (int) rjitter_steps - 1);
    // loop over possible rotations
    for (float rj = min_rjitt; rj <= max_rjitt; rj += rstep) {
      // loop over possible scales
      for (float sj = sjitter_min; sj <= sjitter_max; sj += sstep) {
        // loop over possible heights
        for (int hj = -tjitter_hmax; hj <= tjitter_hmax; hj += tjitter_step) {
          // loop over possible width
          for (int wj = -tjitter_wmax; wj <= tjitter_wmax; wj +=tjitter_step){
            if (std::abs(hj) >= tjitter_hmin || std::abs(wj) >= tjitter_wmin) {
              // add jitter
              // multiply by scale when > 1, to compensate for object being
              // smaller and thus not reaching the same extent.
              random_jitter.push_back(jitter(hj * std::max((float)1.0, sj),
                                             wj * std::max((float)1.0, sj),
                                             sj, rj, height));
            }
          }
        }
      }
    }
    // randomize possibilities
    random_shuffle(random_jitter.begin(), random_jitter.end());
    EDEBUG("computed " << random_jitter.size() << " random jitters");
  }

  ////////////////////////////////////////////////////////////////
  // loading errors

  //! required datasets, throw error.
  template <typename T>
      bool loading_error(idx<T> &mat, std::string &fname) {
    try {
      mat = load_matrix<T>(fname);
    } eblcatcherror_msg("failed to load dataset file");
    std::cout << "Loaded " << fname << " (" << mat << ")" << std::endl;
    return true;
  }

  //! required datasets, throw error.
  template <typename T>
  bool loading_error(midx<T> &mat, std::string &fname) {
    try {
      // in multi-matrix files, samples are separate
      if (has_multiple_matrices(fname.c_str()))
        mat = load_matrices<T>(fname);
      else { // single-matrix, all samples are together
        idx<T> single = load_matrix<T>(fname);
        // break up first dimension into multi-matrix
        mat = midx<T>(single.dim(0));
        for (uint i = 0; i < single.dim(0); ++i) {
          idx<T> sample = single.select(0, i);
          mat.mset(sample, i);
        }
      }
    } eblcatcherror_msg("failed to load dataset file");
    std::cout << "Loaded " << fname << " (" << mat << ")" << std::endl;
    return true;
  }

  //! optional datasets, issue warning.
  template <typename T>
      bool loading_warning(idx<T> &mat, std::string &fname) {
    try {
      mat = load_matrix<T>(fname);
    } catch (const std::string &err) {
      std::cerr << "warning: failed to load dataset file " << fname << std::endl;
      return false;
    }
    std::cout << "Loaded " << fname << " (" << mat << ")" << std::endl;
    return true;
  }

  //! optional datasets, issue warning.
  template <typename T>
      bool loading_warning(midx<T> &mat, std::string &fname) {
    try {
      mat = load_matrices<T>(fname);
    } catch (const std::string &err) {
      std::cerr << "warning: failed to load dataset file " << fname << std::endl;
      return false;
    }
    std::cout << "Loaded " << fname << " (" << mat << ")" << std::endl;
    return true;
  }

  //! optional datasets, issue warning.
  template <typename T>
      bool loading_nowarning(idx<T> &mat, std::string &fname) {
    try {
      mat = load_matrix<T>(fname);
    } catch (const std::string &err) {
      return false;
    }
    std::cout << "Loaded " << fname << " (" << mat << ")" << std::endl;
    return true;
  }

} // end namespace ebl

#endif /* DATASET_HPP_ */
