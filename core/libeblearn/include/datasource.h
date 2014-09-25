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

#ifndef DATASOURCE_H_
#define DATASOURCE_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_state.h"
#include "ebl_arch.h"
#include <map>

namespace ebl {

// datasource //////////////////////////////////////////////////////////////////

//! A class handling a data source. This datasource does can not contain
//! labels, see 'labeled_datasource' or 'class_datasource' for
//! regression and classification tasks.
template<typename T, typename Tdata> class datasource {
 public:
  typedef std::map<uint,idx<Tdata> > t_pick_map;

  //! CAUTION: This empty constructor requires a subsequent call to init().
  datasource();
  //! Construct a datasource from 'data', where data
  //! is a multi-matrix matrix (midx type). This allows for dynamic loading
  //! of data and avoids the need to fit all data in memory.
  //! The first dimension of each data sample
  //! is expected to be the samples dimension, and the second one the
  //! feature dimension, or the channel dimension in case of images.
  //! \param name An optional name for this dataset.
  datasource(midx<Tdata> &data, const char *name = NULL);
  //! Construct a datasource from 'data'. The first dimension of 'data'
  //! is expected to be the samples dimension, and the second one the
  //! feature dimension, or the channel dimension in case of images.
  //! \param name An optional name for this dataset.
  datasource(idx<Tdata> &data, const char *name = NULL);
  //! Construct a datasource from the matrix found in 'data_fname'.
  //! The first dimension of the data
  //! is expected to be the samples dimension, and the second one the
  //! feature dimension, or the channel dimension in case of images.
  //! \param name An optional name for this dataset.
  datasource(const char *data_fname, const char *name = NULL);
  //! destructor
  virtual ~datasource();

  // intialization ///////////////////////////////////////////////////////////

  //! Initialize from a multi-matrix data.
  void init(midx<Tdata> &data, const char *name);
  //! Initialize.
  void init(idx<Tdata> &data, const char *name);

  // data access methods /////////////////////////////////////////////////////

  //! Copies current sample's data into s (1 tensor only).
  virtual void fprop1_data(idx<T> &s);
  //! Copies all of current sample's data into s.
  virtual void fprop_data(state<T> &s);

  //! Return original sample's idx at this index.
  virtual idx<Tdata> get_sample(intg index);
  //! Returns an idx of the last raw output of current sample.
  //! This supposes raw outputs have been previously initialized via
  //! set_sample_energy().
  //! \param index If -1, return current sample's, otherwise sample's at
  //! 'index' position.
  virtual idx<T> get_raw_output(intg index = -1);

  // iterating methods ///////////////////////////////////////////////////////

  //! Set the internal iterators such that a call to fprop() will return
  //! the data associated with this index.
  virtual void select_sample(intg index);
  //! shuffle dataset, based on the number of classes
  //! assume the same nb of samples in each class
  virtual void shuffle();
  //! Move to the next datum (in the original order of the dataset).
  //! Returns false if we reached the end.
  //! This should be used during testing.
  //! It will always return the data in the same order with the same
  //! probability of 1. See next_train() for data returned with
  //! variable probability, balance, etc. (used for training only).
  virtual bool next();
  //! Move to the next datum, in a way suited for training (_not_ for testing,
  //! for testing see next()): depending on the configuration, this will
  //! return samples in a class-balanced way, i.e. showing each class
  //! sequentially, with different probabilities based on sample's difficulty,
  //! or/and in a random order after each pass.
  //! When all samples of a class have been shown, it loops back to the first
  //! sample of that class. This should be used during training only.
  //! If a sample was not selected because of a low probability, this will
  //! return false, if it was selected it returns true. In any case,
  //! internal iterators will always be set to the next sample, regardless
  //! if it was selected or not. It is up to the caller, to train on
  //! the sample if selected, or only test and update its energy if not
  //! selected.
  virtual bool next_train();

  // accessors ///////////////////////////////////////////////////////////////

  //! Set the bias to add to the data.
  virtual void set_data_bias(T bias);
  //! Set the coefficient to multiply the data with.
  virtual void set_data_coeff(T coeff);
  //! Returns the number of data instances contained in this data source.
  virtual unsigned int size();
  //! Returns an idxdim object describing the order (number of dimensions)
  //! and the size of each dimension of a single sample outputed by fprop.
  virtual idxdim sample_dims();
  //! Returns an idxdim object describing the order (number of dimensions)
  //! and the size of each dimension of a single sample outputed by fprop.
  virtual mfidxdim sample_mfdims();

  //! Set the distance (or energy) between the answer of the model to train
  //! and the
  //! true answer. This is used to give more or less probability for a
  //! sample to be used for training. At the beginning of training, all
  //! samples start with a probability of 1, thus all samples are used,
  //! but as training goes, easy samples are given lower probability while
  //! harder samples are given higher probability.
  //! The absolute distance is directly mapped into a probability, i.e.
  //! a distance of 0 will give probability 0 to be used, and distance of
  //! 1 and higher give probability 1 to be used. Therefore distance
  //! should be normalized so that a distance of 1 represents an offending
  //! answer.
  //! This is used only by next_train(), not by next().
  //! \param correct True if the answer was correct, false otherwise.
  //!          This is used to ignore samples for which we already get
  //!          the correct answer.
  //! \param raw_outputs The raw outputs of the network for current sample.
  //! \param answers All the estimated answers for that sample
  //!   (class, confidence, etc).
  //! \param target The target answer, i.e. the groundtruth equivalent of
  //!    'estimates'.
  virtual void set_sample_energy(double e, bool correct, idx<T> &raw_outputs,
                                 idx<T> &answers, idx<T> &target);
  //! If 'keep' is true, then we keep for each sample the 'raw_outputs',
  //! the 'answers' and the 'target' of the model (see set_sample_energy()).
  //! This may be expensive in memory.
  virtual void keep_outputs(bool keep = true);
  //! Save current answers as CSV file.
  virtual void save_answers(std::string &filename);
  //! Normalize picking probabilities globally with maximum probability.
  virtual void normalize_all_probas();
  //! Normalize picking probabilities globally with maximum probability.
  virtual void normalize_probas(std::vector<intg> *cindinces = NULL);
  //! Move to the beginning of the data, for the test iterators only,
  //! i.e. only next() is affected, next_train() is unaffected.
  virtual void seek_begin();
  //! Move to the beginning of the data, for the train iterators only,
  //! i.e. only next_train() is affected, next() is unaffected.
  virtual void seek_begin_train();
  //! Activate or deactivate shuffling of list of samples for each class
  //! after reaching the end of the sample list. This has an effect only
  //! when set_balanced() is set.
  //! This is activated by default.
  //! This is used only by next_train(), not by next().
  virtual void set_shuffle_passes(bool activate);
  //! Add a feature dimension with size 1 in front of data.
  virtual void add_features_dimension();

  //! Activate or deactivate weighing of samples based on classification
  //! results. Wrong answers give a higher probability for a sample
  //! to be used for training, correct answers a lower probability.
  //! This is activated by default.
  //! This is used only by next_train(), not by next().
  //! \param hardest_focus If true, focus on hardest samples, otherwise
  //!          focus on easiest ones.
  //! \param perclass_norm  Set the normalization of the sample probabilities
  //!   to be per class or global. If perclass is true, the probabilities for
  //!   each sample of a same class are normalized from [0..max_class]
  //!   to [0..1]
  //!   otherwise from [0..max_global]. Perclass can be used (or not) for
  //!   discrete classification problems, but should be global for continuous
  //!   labels. This normalization avoids looping on samples rarely picking
  //!   any if all probabilities tend to zero for example.
  //!   This is used only by next_train(), not by next().
  //! \param min_proba Set the minimum probaility of a sample to be picked
  //!   by next_train(). By default, this is zero.
  //!   Acceptable range is [0 .. 1].
  virtual void set_weigh_samples(bool activate, bool hardest_focus = false,
                                 bool perclass_norm = true,
                                 double min_proba = 0.0);
  //! Set this datasource to be a test datasource. This is optional but
  //! useful for reporting and to verify that no training only methods are
  //! called on this datasrouces (e.g. next_train()).
  virtual void set_test();
  //! Returns true if this datasource is a test datasource only.
  virtual bool is_test();
  //! Returns the number of samples to train on for one epoch.
  //! By default, it returns the size of the smallest class times
  //! the number of classes (see get_lowest_common_size()).
  //! Default behavior can be overriden with set_epoch_size().
  virtual intg get_epoch_size();
  //! Return the number of samples this epoch has processed.
  virtual intg get_epoch_count();
  //! Set the number of samples to train on for one epoch.
  //! If not called, default number used is the one returned
  //! by get_lowest_common_size().
  virtual void set_epoch_size(intg sz);
  //! Set the epoch mode, i.e. how samples are presented for training.
  //! 0: show a fixed number of samples (set by set_epoch_size()).
  //! 1: show all samples at least once (samples may be shown multiple times
  //!    if a class is unbalanced and the balance mode is activated).
  virtual void set_epoch_mode(uint mode);
  //! Return true if current epoch is finished. Call init_epoch() to
  //! restart a new epoch.
  virtual bool epoch_done();
  //! Restarts a new epoch, i.e. resets counters but do not reset iterators
  //! positions.
  virtual void init_epoch();
  //! Output statistics of samples picking, i.e. the number of times each
  //! sample has been picked for training.
  virtual void save_pickings(const char *name = NULL);
  //! Return true if counting of pickings is enabled.
  virtual bool get_count_pickings();
  //! Enable or disable the counting of pickings.
  virtual void set_count_pickings(bool count = true);
  //! Return name of dataset.
  virtual std::string& name();
  //! Print training count every module samples.
  virtual void set_epoch_show(uint modulo);
  //! Do not train on correctly classified examples if ignore is true
  virtual void ignore_correct(bool ignore = true);
  //! Each sample contains multiple states or not.
  virtual bool mstate_samples();

  // state saving ////////////////////////////////////////////////////////////

  //! Save internal iterators. Calling restore_state() will return to the
  //! current sample.
  virtual void save_state();
  //! Restore previously saved internal iterators.
  virtual void restore_state();

  // pretty methods //////////////////////////////////////////////////////////

  //! Print info about the datasource on the standard output.
  virtual void pretty();
  //! Pretty the progress of current epoch.
  //! \param newline If true, end pretty with a new line.
  virtual void pretty_progress(bool newline = true);

  ////////////////////////////////////////////////////////////////
 protected:

  // intialization ///////////////////////////////////////////////////////////

  //! Initialize. Should be called only after data matrix has been assigned.
  void init2(const char *name);

  // picking methods /////////////////////////////////////////////////////////

  //! Draw a random number between 0 and 1 and return true if higher
  //! than current sample's probability.
  virtual bool pick_current();
  //! Return a vector of sample indices, sorted by their picking counts.
  virtual std::map<uint,intg>& get_pickings();

  // members /////////////////////////////////////////////////////////////////
 public:
  T                   bias;
  T                   coeff;
  // data
  idx<Tdata>	      data;             // samples
  midx<Tdata>	      datas;            // samples (multi-matrix).
  idx<double>	      probas;           //!< sample probabilities
  bool                add_features_dimension_;
  // predictions
  idx<double>	      energies;         //!< sample energies
  idx<ubyte>	      correct;          //!< sample correctness
  idx<ubyte>	      incorrect;          //!< sample correctness
  idx<T>	      raw_outputs;      //!< The raw outputs for each sample.
  idx<T>	      answers;          //!< all answers
  idx<T>	      targets;          //!< all targets
  // picking /////////////////////////////////////////////////////////////////
  idx<uint>           pick_count;	//!< count pickings.
  bool                count_pickings;   //!< Count which samples are picked.
  bool                count_pickings_save;
  unsigned int        height;
  unsigned int        width;
  std::string	      _name;
 protected:
  std::vector<intg>   counts;           // # of samples / class
  std::map<uint,intg> picksmap;
  bool                multimat;         //!< True if data is a multi-matrix matrix.
  bool                bkeep_outputs;    //!< Keep model outputs for each sample.
  ////////////////////////////////////////////////////////////////////////////
  // (unbalanced) iterating indices
  intg                it;               //!< Index of current sample in data matrix.
  intg                it_test;          //!< Current test index in data matrix.
  intg                it_train;         //!< Current train index in vector 'indices'.
  idx<intg>           indices;          //!< Vector of indices to the data matrix.
  ////////////////////////////////////////////////////////////////////////////
  // state saving
  bool                state_saved;      //!< State has been saved or not.
  intg                it_saved;         //!< Saving current iterator it.
  intg                it_test_saved;    //!< Saving current test iterator.
  intg                it_train_saved;   //!< Saving current train iterator.
  idx<intg>           indices_saved;    //!< Saving sample indices.
  intg                epoch_cnt_saved;
  intg                epoch_pick_cnt_saved; //!< # pickings
  std::vector<intg>   epoch_done_counters_saved;
  ////////////////////////////////////////////////////////////////////////////
  // features switches
  bool                shuffle_passes;   //!< Shuffle at end of each pass.
  bool                test_set;         //!< This set is a test set or not.
  // epoch variables ///////////////////////////////////////////////////////////
  std::vector<intg>   epoch_done_counters;
  intg                epoch_sz;
  intg                epoch_cnt;
  intg                epoch_pick_cnt;	//!< # pickings
  uint                epoch_show;	// show modulo
  intg                epoch_show_printed;
  uint                epoch_mode;       //!< 0: fixed number, 1: all at least once
  timer               epoch_timer;
  timer               test_timer;
  uint                not_picked;
  bool                hardest_focus;    //!< Focus training on hardest samples.
  bool                _ignore_correct;  //!< Do not train on correct samples.
  ////////////////////////////////////////////////////////////////////////////
  // sample picking with probabilities
  bool                weigh_samples;    //!< Use probas to pick samples.
  bool                perclass_norm;    //!< Normalize probas per class.
  double              sample_min_proba; //!< Minimum proba of each sample.
  idxdim              sampledims;       //!< Dimensions of a data sample.
  mfidxdim            samplemfdims;     //!< Dimensions of a data sample.
};

// labeled_datasource //////////////////////////////////////////////////////////

//! A datasource associating samples with their corresponding label.
//! A label is not necessarly a discrete class label, it can be continuous
//! values in a regression setting. For discrete classification purpose,
//! use 'class_datasource'.
template <typename T, typename Tdata, typename Tlabel>
class labeled_datasource : public datasource<T, Tdata> {
 public:
  //! CAUTION: This empty constructor requires a subsequent call to init().
  labeled_datasource();
  //! Construct a datasource associating 'data' and its 'labels' where data
  //! is a multi-matrix matrix (midx type). This allows for dynamic loading
  //! of data and avoids the need to fit all data in memory.
  //! \param name An optional name for this dataset.
  labeled_datasource(midx<Tdata> &data, idx<Tlabel> &labels,
                     const char *name = NULL);
  //! Construct a datasource associating 'data' and its 'labels'.
  //! \param name An optional name for this dataset.
  labeled_datasource(idx<Tdata> &data, idx<Tlabel> &labels,
                     const char *name = NULL);
  //! Constructor from the full general name (i.e. root/dataset) but without
  //! the individual file names such as "_data.mat". This appends names and
  //! extension of each subfile of a dataset created with the dscompile tool.
  //! \param name An optional name for this dataset.
  labeled_datasource(const char *root_ds, const char *name = NULL);
  //! Constructor from data root and names for data, labels and classes files
  //! (appending names and extension of each subfile of a dataset created
  //!  with the dscompile tool).
  //! \param name An optional name for this dataset.
  labeled_datasource(const char *root, const char *data_name,
                     const char *labels_name, const char *jitters_name = NULL,
                     const char *scales_name = NULL, const char *name = NULL);
  //! Destructor.
  virtual ~labeled_datasource();

  // init methods ////////////////////////////////////////////////////////////

  //! Initialize from matrices, where data is a multi-matrix.
  void init(midx<Tdata> &data, idx<Tlabel> &labels, const char *name);
  //! Initialize from matrices.
  void init(idx<Tdata> &data, idx<Tlabel> &labels, const char *name);
  //! Intialize from matrices filenames.
  //! \param max_size If > 0, limit the number of samples to this value.
  void init(const char *data_fname, const char *labels_fname,
            const char *jitters_fname = NULL, const char *name = NULL,
            const char *scales_fname = NULL, uint max_size = 0);
  //! Intialize from root and partial matrices filenames.
  void init_root(const char *root, const char *data_fname,
                 const char *labels_fname, const char *jitters_fname = NULL,
                 const char *scales_fname = NULL, const char *name = NULL);
  //! Initialize from data root and names for data, labels and classes files
  //! (appending names and extension of each subfile of a dataset created
  //!  with the dscompile tool).
  void init_root(const char *root_dsname, const char *name = NULL);

  // data access /////////////////////////////////////////////////////////////

  //! Fills data and label.
  virtual void fprop(state<T> &data, state<Tlabel> &label);
  //! Copy current sample's label into s.
  virtual void fprop_label(state<Tlabel> &s);
  //! Copy current sample's label into s (name has to be different than
  //! fprop_label in case T == Tlabel).
  virtual void fprop_label_net(state<T> &s);
  //! Copy current sample's jitter into s.
  virtual void fprop_jitter(state<T> &s);
  //! Returns a scale id for this sample.
  virtual intg fprop_scale();

  // accessors ///////////////////////////////////////////////////////////////

  //! This returns true if sample with 'index' is considered for training.
  virtual bool included_sample(intg index);
  //! This returns the number of samples actually considered for training.
  virtual intg count_included_samples();
  //! Print info about the datasource on the standard output.
  virtual void pretty();
  //! Print info about the scales data on the standard output.
  virtual void pretty_scales();
  //! Returns the dimensions of a single label.
  virtual idxdim label_dims();
  //! Set the bias to add to the labels (before multiplying by coeff).
  virtual void set_label_bias(T bias);
  //! Set the coefficient to multiply the labels with (after adding bias).
  virtual void set_label_coeff(T coeff);
  //! Returns true if this dataset contains scale information.
  virtual bool has_scales();
  //! Remember if sample was correctly classified.
  virtual void set_sample_correctness(bool correct);

  // export ////////////////////////////////////////////////////////////////////

  //! Save correct subset of this dataset as a new dataset whose name
  //! and path start with prefix.
  //! The correctness information can be set via the set_correctness() method.
  virtual void save_correct(const char *prefix);
  //! Save incorrect subset of this dataset as a new dataset whose name
  //! and path start with prefix.
  //! The correctness information can be set via the set_correctness() method.
  virtual void save_incorrect(const char *prefix);

  // friends ///////////////////////////////////////////////////////////////////
  template <typename T1, typename T2, typename T3>
  friend class labeled_datasource_gui;
  template <typename T1, typename T2, typename T3>
  friend class supervised_trainer;
  template <typename T1, typename T2, typename T3>
  friend class supervised_trainer_gui;

  // protected methods ///////////////////////////////////////////////////////
 protected:

  //! Initialize labels. This should only be called by init().
  void init_labels(idx<Tlabel> &labels, const char *name);

  // members /////////////////////////////////////////////////////////////////
 protected:
  using datasource<T,Tdata>::_name;
  // data
  T	      label_bias;
  T	      label_coeff;
  using datasource<T,Tdata>::data;
  using datasource<T,Tdata>::correct;
  using datasource<T,Tdata>::incorrect;
  idx<Tlabel> labels;                   // labels
  idx<intg>   scales;                   // scales
  midx<float> jitters;                  //!< Jitter information.
  bool        scales_loaded;            //!< Scales info was loaded or not.
  // iterating
  using datasource<T,Tdata>::it;
  using datasource<T,Tdata>::epoch_sz;
  // dimensions
  using datasource<T,Tdata>::sampledims;
  idxdim      jitters_maxdim;           //!< Maximum dimensions in jitters matrices.
  idxdim      labeldims;                //!< Dimensions of a label.
  using datasource<T,Tdata>::multimat;
};

// class_datasource ////////////////////////////////////////////////////////////

//! A datasource associating samples with a discrete class.
//! The order of samples distributed to training can be balanced to
//! present the same amount of samples for each class if these are unbalanced.
template <typename T, typename Tdata, typename Tlabel>
class class_datasource : public labeled_datasource<T, Tdata, Tlabel> {
 public:
  //! CAUTION: This empty constructor requires a subsequent call to init().
  class_datasource();

  //! Construct dataset with 'data' and its corresponding 'labels', where data
  //! is a multi-matrix matrix (midx type). This allows for dynamic loading
  //! of data and avoids the need to fit all data in memory.
  //! \param lblstr An optional vector of strings describing each class.
  //! \param name An optional name for this dataset.
  class_datasource(midx<Tdata> &data, idx<Tlabel> &labels,
                   std::vector<std::string*> *lblstr = NULL, const char *name = NULL);
  //! Construct dataset with 'data' and its corresponding 'labels'.
  //! \param lblstr An optional vector of strings describing each class.
  //! \param name An optional name for this dataset.
  class_datasource(idx<Tdata> &data, idx<Tlabel> &labels,
                   std::vector<std::string*> *lblstr = NULL, const char *name = NULL);
  //! Construct dataset with 'data' and its corresponding 'labels', where data
  //! is a multi-matrix matrix (midx type). This allows for dynamic loading
  //! of data and avoids the need to fit all data in memory.
  //! \param classes A vector of strings describing each class.
  //! \param name An optional name for this dataset.
  class_datasource(midx<Tdata> &data, idx<Tlabel> &labels,
                   idx<ubyte> &classes, const char *name = NULL);
  //! Construct dataset with 'data' and its corresponding 'labels'.
  //! \param classes A vector of strings describing each class.
  //! \param name An optional name for this dataset.
  class_datasource(idx<Tdata> &data, idx<Tlabel> &labels,
                   idx<ubyte> &classes, const char *name = NULL);
  //! Constructor from full names for each dataset file.
  //! Note: jitters and classes files are optional.
  //! \param name An optional name for this dataset.
  class_datasource(const char *data_name,
                   const char *labels_name, const char *jitters_name = NULL,
                   const char *scales_name = NULL,
                   const char *classes_name = NULL, const char *name = NULL);
  //! (Shallow) copy constructor (only the class strings are deeply copied).
  class_datasource(const class_datasource<T, Tdata, Tlabel> &ds);
  //! Destructor.
  virtual ~class_datasource();

  // init methods ////////////////////////////////////////////////////////////

  //! Initialize variables to their default value.
  void defaults();
  //! Allocate class strings vector from ubyte matrix 'classes'.
  virtual void init_strings(idx<ubyte> &classes);
  //! Initialize things specific to this class. The rest can be initialized
  //! with parent init methods.
  void init_local(std::vector<std::string*> *lblstr);
  //! Initialize from matrices where the data is a multi-matrix matrix.
  void init(midx<Tdata> &data, idx<Tlabel> &labels,
            std::vector<std::string*> *lblstr, const char *name);
  //! Initialize from matrices.
  void init(idx<Tdata> &data, idx<Tlabel> &labels,
            std::vector<std::string*> *lblstr, const char *name);
  //! Intialize from matrices filenames.
  //! \param max_size If > 0, limit the number of samples to this value.
  void init(const char *data_fname, const char *labels_fname,
            const char *jitters_fname = NULL, const char *scales_fname = NULL,
            const char *classes_fname = NULL, const char *name = NULL,
            uint max_size = 0);
  //! Intialize from root and partial matrices filenames.
  void init_root(const char *root, const char *data_fname,
                 const char *labels_fname, const char *jitters_fname = NULL,
                 const char *scales_fname = NULL,
                 const char *classes_fname = NULL, const char *name = NULL);
  //! Constructor from directory 'root' and individual names for data,
  //! labels, jitters and classes files (appending names and extension of
  //! each subfile of a dataset created with dscompile tool).
  void init_root(const char *root_dsname, const char *name = NULL);
  //! Fills 'clabels' and 'clblstr' which are the true class labels if
  //! labels are not consecutive values starting from 0. If they are,
  //! then 'clabels' is equivalent to 'labels',
  //! same for 'clblstr' and 'lblstr'.
  virtual void init_class_labels();

  // data access /////////////////////////////////////////////////////////////

  //! Return the value contained in current sample's label. This supposes
  //! a label is only constituted of 1 element and will produce and error
  //! otherwise.
  virtual Tlabel get_label();

  // iterating ///////////////////////////////////////////////////////////////

  //! This returns true if sample with 'index' is considered for training.
  virtual bool included_sample(intg index);
  //! This returns the number of samples actually considered for training.
  virtual intg count_included_samples();
  //! Move to the beginning of the data, for the test iterators only,
  //! i.e. only next() is affected, next_train() is unaffected.
  virtual void seek_begin();
  //! Move to the beginning of the data, for the train iterators only,
  //! i.e. only next_train() is affected, next() is unaffected.
  virtual void seek_begin_train();
  //! Move to the next datum (in the original order of the dataset).
  //! Returns false if we reached the end.
  //! This should be used during testing.
  //! It will always return the data in the same order with the same
  //! probability of 1. See next_train() for data returned with
  //! variable probability, balance, etc. (used for training only).
  virtual bool next();
  //! Move to the next datum in a way suited for training (_not_ for testing,
  //! for testing see next()).  If balance is activated (see set_balanced())
  //! this will return samples in a class-balanced way,
  //! i.e. showing each class
  //! sequentially, with different probabilities based on sample's difficulty,
  //! or/and in a random order after each pass.
  //! When all samples of a class have been shown, it loops back to the first
  //! sample of that class. This should be used during training only.
  //! If a sample was not selected because of a low probability, this will
  //! return false, if it was selected it returns true. In any case,
  //! internal iterators will always be set to the next sample, regardless
  //! if it was selected or not. It is up to the caller, to train on
  //! the sample if selected, or only test and update its energy if not
  //! selected.
  virtual bool next_train();
  //! This move class iterator 'class_it' to the next class.
  //! If reaching the end of the classes, then the class order is shuffled
  //! and iterator starts at first class again.
  virtual void next_balanced_class();
  //! If 'random_class_order' is true, this will randomly change to order
  //! in which classes are presented for balanced training, otherwise
  //! it will do nothing and leave them in the original order.
  virtual void reset_class_order();
  //! If 'ran' is true, then the order in which classes are presented during
  //! balanced training is randomized.
  virtual void set_random_class_order(bool ran);
  //! Specifies probabilities for each class to be picked. If not specified
  //! and training is balanced, each class is shown one after the other.
  //! If probabilities are specified, each class has its own probability to
  //! be picked. The vector of probabilities does not need to sum to 1,
  //! all probabilities will be normalized. E.g. with [1,1,2], classes 0 and
  //! 1 have 25% chance and class 2 has 50% chance to be picked.
  virtual void set_class_probabilities(std::vector<float> &probas);
  //! Excludes use of classes other than ones in [offset, offset + n] range.
  //! \param random If true, keep only n classes at random.
  virtual void limit_classes(intg n, intg offset = 0, bool random = false);
  //! If 'bal' is true, make the next_train() method call sequentially one
  //! sample of each class instead of following the dataset's distribution.
  //! This is important to use when the dataset is unbalanced.
  //! This is set to true by default.
  //! Balance is used only by next_train(), not by next().
  virtual void set_balanced(bool bal = true);
  //! Return true if current epoch is finished. Call init_epoch() to
  //! restart a new epoch.
  virtual bool epoch_done();
  //! Restarts a new epoch, i.e. resets counters but do not reset iterators
  //! positions.
  virtual void init_epoch();
  //! Normalize picking probabilities by maximum probability for all classes
  //! if perclass_norm is true, or globally otherwise.
  virtual void normalize_all_probas();
  //! Normalize picking probabilities by maximum probability of classid if
  //! perclass_norm is true, or globally otherwise.
  virtual void normalize_probas_by_class(int classid = -1);

  // accessors ///////////////////////////////////////////////////////////////

  //! Return the number of classes.
  virtual intg get_nclasses();
  //! Return the label id corresponding to name, or -1 if not found.
  virtual int get_class_id(const char *name);
  //! Return the label string for index id.
  virtual std::string& get_class_name(int id);
  //! Returns a reference to a vector of each label string.
  virtual std::vector<std::string*>& get_label_strings();
  //! Return the lowest (non-zero) size per class, multiplied by the number
  //! of classes.
  //! e.g. if a dataset has 10 classes with 100 examples and 5 classes with
  //! 50 examples, it will return 50 * (10 + 5) = 750, whereas size()
  //! will return 1250.
  //! This is useful to keep iterations to a meaningful size when a class
  //! has many more examples than another.
  virtual intg get_lowest_common_size();

  // picking methods /////////////////////////////////////////////////////////

  //! Output statistics of samples picking, i.e. the number of times each
  //! sample has been picked for training.
  virtual void save_pickings(const char *name = NULL);
  //! Write plot of m organized by class and correctness
  template <typename T2>
  void write_classed_pickings(idx<T2> &m, idx<ubyte> &correct,
                              std::string &name_, const char *name2_ = NULL,
                              bool plot_correct = true,
                              const char *ylabel = "");

  // state saving ////////////////////////////////////////////////////////////

  //! Save internal iterators. Calling restore_state() will return to the
  //! current sample.
  virtual void save_state();
  //! Restore previously saved internal iterators.
  virtual void restore_state();

  // pretty methods //////////////////////////////////////////////////////////

  //! Print info about the datasource on the standard output.
  virtual void pretty();
  //! Print info about the scales data on the standard output.
  virtual void pretty_scales();
  //! Pretty the progress of current training epoch.
  //! \param newline If true, end pretty with a new line.
  virtual void pretty_progress(bool newline = true);

  // friends //////////////////////////////////////////////////////////////
  template <typename T1, typename T2, typename T3>
  friend class class_datasource_gui;
  template <typename T1, typename T2, typename T3>
  friend class supervised_trainer;

 protected:

  //! Draw a random number between 0 and 1 and return true if higher
  //! than current sample's probability.
  virtual bool pick_current();

  // members /////////////////////////////////////////////////////////////////
 protected:
  using datasource<T,Tdata>::_name;
  // classes
  intg		 nclasses;	//!< Number of classes.
  std::vector<std::string*> *lblstr;	//!< Name of each class.
  std::vector<std::string*> clblstr;	//!< Name of each class, may differ.
  bool           bexclusion;    //!< Exclusion is used.
  std::vector<bool> excluded;   //!< Vector of excluded classes.
  intg           included;      //!< Number of included classes.
  // data
  using datasource<T,Tdata>::data;
  using labeled_datasource<T,Tdata,Tlabel>::labels;
  idx<Tlabel> olabels; //!< Original class labels (may differ from labels).
  using datasource<T,Tdata>::correct;
  using datasource<T,Tdata>::energies;
  using datasource<T,Tdata>::probas;
  // iterating
  using datasource<T,Tdata>::it;
  using datasource<T,Tdata>::epoch_mode;
  using datasource<T,Tdata>::epoch_show;
  using datasource<T,Tdata>::epoch_sz;
  using datasource<T,Tdata>::epoch_timer;
  using datasource<T,Tdata>::epoch_show_printed;
  // class-balanced iterating indices
  using datasource<T,Tdata>::epoch_done_counters;
  bool		 balance;	//!< Balance iterating or not.
  std::vector<std::vector<intg> > bal_indices;	//!< Balanced iterating indices.
  std::vector<uint> bal_it;	//!< Sample iterators for each class.
  std::vector<uint> class_order;	//!< Order in which to balance classes.
  std::vector<float> class_probabilities;
  bool           random_class_order;
  uint		 class_it;	//!< Iterator on classes.
  uint		 class_it_it;	//!< Iterator on classes iterator.
  // sample picking with probabilities
  bool		 perclass_norm;	//!< Normalize probas per class.
  using datasource<T,Tdata>::epoch_pick_cnt;	//!< # pickings
  using datasource<T,Tdata>::epoch_cnt;	//!< # sample seen this epoch.
  using datasource<T,Tdata>::count_pickings;
  using datasource<T,Tdata>::pick_count;
  using datasource<T,Tdata>::counts;
  using datasource<T,Tdata>::weigh_samples;
  // state saving
  std::vector<std::vector<intg> > bal_indices_saved;
  std::vector<uint>        bal_it_saved;
  uint                class_it_saved;
  uint                class_it_it_saved;
  using datasource<T,Tdata>::state_saved;
  using datasource<T,Tdata>::count_pickings_save;
  using datasource<T,Tdata>::it_saved;
  using datasource<T,Tdata>::it_test;
  using datasource<T,Tdata>::it_test_saved;
  using datasource<T,Tdata>::it_train;
  using datasource<T,Tdata>::it_train_saved;
  // misc
  using datasource<T,Tdata>::sampledims;
  using datasource<T,Tdata>::test_set;
  using datasource<T,Tdata>::shuffle_passes;
  using datasource<T,Tdata>::not_picked;
};

// class_node //////////////////////////////////////////////////////////////////

//! A node class describing a class hierarchy.
//! This class allows recursive balanced selection of samples.
template <typename Tlabel> class class_node {
 public:
  //! Construct a class_node with label 'id'. Call set_parent() to
  //! establish parent relationship.
  class_node(Tlabel id, std::string &name);
  //! Destructor.
  virtual ~class_node();
  //! Returns false if this node or its children contain at least 1 sample,
  //! true otherwise. If false, next() can be called.
  virtual bool empty();
  //! Returns false if this node contains at least 1 sample, true otherwise.
  virtual bool internally_empty();
  //! This returns the index of the next sample of this class in a balanced
  //! way, i.e. it will alternate between each direct children and itself.
  //! e.g. if this class id is 1, and has 3 children with ids 2,3,4, samples
  //! classes id will have this order: 1,2,3,4,1,2,3,4,1,etc.
  virtual intg next();
  //! Add 'child' to the vector of children and check that child's parent
  //! is indeed this node.
  virtual void add_child(class_node *child);
  //! Add a sample index to this node, i.e. this sample has the same label.
  virtual void add_sample(intg index);
  //! Returns the number of samples contained in this node.
  inline virtual intg nsamples();
  //! Returns node label.
  inline virtual Tlabel label();
  //! Returns node label at depth 'depth'. If node has lower depth than d,
  //! this returns its leaf label.
  inline virtual Tlabel label(uint depth);
  //! Returns the depth of this node.
  inline virtual uint depth();
  //! Set the depth of this node to d, and recursively set depth of children
  //! with d + 1, and return the maximum depth of all children.
  virtual uint set_depth(uint d);
  //! Returns the name of this node.
  virtual std::string& name();
  //! Returns the parent pointer (NULL if no parent).
  virtual class_node<Tlabel>* get_parent();
  //! Returns true if label 'lab' is a parent of this node.
  virtual bool is_parent(Tlabel lab);

 protected:
  //! Set this node to non-empty. This can be called by children when they
  //! receive samples, so that the information propagates back to the top.
  //! Later, parents know if they can attempt to get samples from their
  //! children or not.
  virtual void set_non_empty();
  //! Set the parent to 'parent'.
  virtual void set_parent(class_node *parent);

  // members /////////////////////////////////////////////////////////////////
 protected:
  Tlabel		 _label; //!< This class' label.
  std::string               &_name;
  // hierarchy variables /////////////////////////////////////////////////////
  class_node		*parent;	//!< Parent node.
  std::vector<class_node<Tlabel>*>	 children;	//!< Children nodes.
  typename std::vector<class_node<Tlabel>*>::iterator it_children;
  bool bempty; //!< This node or its children have samples or not.
  bool iempty; //!< This node has internal samples or not.
  uint _depth; //!< Depth of this node.
  // samples corresponding to this node //////////////////////////////////////
  std::vector<intg>	 samples;	//!< Sample ids with this label.
  std::vector<intg>::iterator it_samples;	//!< Iterator in this class' samples.

  // friends /////////////////////////////////////////////////////////////////
  template <typename T1, typename Tdata1, typename Tlabel1>
  friend class hierarchy_datasource;
};

// hierarchy_datasource ////////////////////////////////////////////////////////

//! A datasource associating samples with a discrete class which can are
//! organized as a hierarchy.
template <typename T, typename Tdata, typename Tlabel>
class hierarchy_datasource : public class_datasource<T, Tdata, Tlabel> {
 public:
  //! CAUTION: This empty constructor requires a subsequent call to init().
  hierarchy_datasource();
  //! Construct dataset with 'data' and its corresponding 'labels',
  //! where data is a multi-matrix (useful for dynamic loading, avoiding
  //! the need to fit all data in memory).
  //! \param parents The parent label associated with each label. A Nx2 matrix
  //! with N the number of classes and 2 the pair child/parent.
  //! \param lblstr An optional vector of strings describing each class.
  //! \param name An optional name for this dataset.
  hierarchy_datasource(midx<Tdata> &data, idx<Tlabel> &labels,
                       idx<Tlabel> *parents = NULL,
                       std::vector<std::string*> *lblstr = NULL,
                       const char *name = NULL);
  //! Construct dataset with 'data' and its corresponding 'labels'.
  //! \param parents The parent label associated with each label. A Nx2 matrix
  //! with N the number of classes and 2 the pair child/parent.
  //! \param lblstr An optional vector of strings describing each class.
  //! \param name An optional name for this dataset.
  hierarchy_datasource(idx<Tdata> &data, idx<Tlabel> &labels,
                       idx<Tlabel> *parents = NULL,
                       std::vector<std::string*> *lblstr = NULL,
                       const char *name = NULL);
  //! Construct dataset with 'data' and its corresponding 'labels'.
  //! \param parents The parent label associated with each label. A Nx2 matrix
  //! with N the number of classes and 2 the pair child/parent.
  //! \param classes A vector of strings describing each class.
  //! \param name An optional name for this dataset.
  hierarchy_datasource(idx<Tdata> &data, idx<Tlabel> &labels,
                       idx<Tlabel> *parents = NULL,
                       idx<ubyte> *classes = NULL, const char *name = NULL);
  //! Constructor from full names for each dataset file.
  //! Note: jitters and classes files are optional.
  //! \param parents_name The parent label associated with each label.
  //! A Nx2 matrix with N the number of classes and 2 the pair child/parent.
  //! \param name An optional name for this dataset.
  //! \param max_size If > 0, limit the number of samples to this value.
  hierarchy_datasource(const char *data_name, const char *labels_name,
                       const char *parents_name = NULL,
                       const char *jitters_name = NULL,
                       const char *scales_name = NULL,
                       const char *classes_name = NULL,
                       const char *name = NULL,
                       uint max_size = 0);
  //! Destructor.
  virtual ~hierarchy_datasource();

  // init methods ////////////////////////////////////////////////////////////

  //! Initialize parents and check their validity.
  void init_parents(idx<Tlabel> *parents = NULL);
  //! Fills 'clabels' and 'clblstr' which are the true class labels if
  //! labels are not consecutive values starting from 0. If they are,
  //! then 'clabels' is equivalent to 'labels',
  //! same for 'clblstr' and 'lblstr'.
  virtual void init_class_labels();

  // data access /////////////////////////////////////////////////////////////

  //! Return the parent's label of current sample. This supposes
  //! a label is only constituted of 1 element and will produce and error
  //! otherwise.
  virtual Tlabel get_parent();
  //! Returns true if 'lab1' is a parent of 'lab2'.
  virtual bool is_parent_of(Tlabel lab1, Tlabel lab2);
  //! Returns a reference to all nodes of the hierarchy.
  virtual std::vector<class_node<Tlabel>*>& get_nodes();
  //! Returns a reference to all nodes ordered by depth, i.e. the following
  //! node is guaranteed to be of equal or higher depth.
  virtual std::vector<class_node<Tlabel>*>& get_nodes_by_depth();

  //! Copy current sample's label into s.
  virtual void fprop_label(state<Tlabel> &s);
  //! Copy current sample's label into s (name has to be different than
  //! fprop_label in case T == Tlabel).
  virtual void fprop_label_net(state<T> &s);
  //! Returns the label corresponding to current iterator and current depth.
  //! If current sample has a deeper label, it will return its parent's
  //! label at depth 'current_depth' (see set_current_depth()).
  virtual Tlabel get_label();
  //! Returns the label corresponding to current iterator and 'depth'.
  //! If current sample has a deeper label, it will return its parent's
  //! label at depth 'depth'.
  //! \param index If -1, return current sample's, otherwise sample's at
  //! 'index' position.
  virtual Tlabel get_label(uint depth, intg index = -1);
  //! Returns the matrix of all labels at current depth, i.e. labels deeper
  //! than current depth, are given their parent's label at current depth.
  idx<Tlabel>& get_depth_labels();
  //! Returns the number of brothers of node 'n'.
  uint get_nbrothers(class_node<Tlabel> &n);

  // iterating ///////////////////////////////////////////////////////////////

  //! Balance samples selection by current depth if 'bal' is true.
  void set_depth_balanced(bool bal);
  //! Set the current depth to 'depth' (for testing or training),
  //! i.e. the training or testing will set samples to have this depth at
  //! most.
  //! The label will be set to this depth's node's label, even if the sample
  //! come from children.
  virtual void set_current_depth(uint depth);
  //! Returns current depth.
  virtual uint get_current_depth();
  //! Increment current depth (see set_current_depth() for more details).
  virtual void incr_current_depth();
  //! Move to the next datum in a way suited for training (_not_ for testing,
  //! for testing see next()).  If balance is activated (see set_balanced())
  //! this will return samples in a class-balanced way,
  //! i.e. showing each class
  //! sequentially, with different probabilities based on sample's difficulty,
  //! or/and in a random order after each pass.
  //! Samples coming from children are also recursively
  //! balanced, so that even if a child has lot of samples compared to other
  //! children, others will be shown as many times.
  //! When all samples of a class have been shown, it loops back to the first
  //! sample of that class. This should be used during training only.
  //! If a sample was not selected because of a low probability, this will
  //! return false, if it was selected it returns true. In any case,
  //! internal iterators will always be set to the next sample, regardless
  //! if it was selected or not. It is up to the caller, to train on
  //! the sample if selected, or only test and update its energy if not
  //! selected.
  virtual bool next_train();

  /* //! If 'bal' is true, make the next_train() method call sequentially one */
  /* //! sample of each class instead of following the dataset's distribution. */
  /* //! This is important to use when the dataset is unbalanced. */
  /* //! This is set to true by default. */
  /* //! Balance is used only by next_train(), not by next(). */
  /* virtual void set_balanced(bool bal = true); */
  /* //! Return true if current epoch is finished. Call init_epoch() to */
  /* //! restart a new epoch. */
  /* virtual bool epoch_done(); */
  /* //! Restarts a new epoch, i.e. resets counters but do not reset iterators */
  /* //! positions. */
  /* virtual void init_epoch(); */
  /* //! Normalize picking probabilities by maximum probability for all classes */
  /* //! if perclass_norm is true, or globally otherwise. */
  /* virtual void normalize_all_probas(); */
  /* //! Normalize picking probabilities by maximum probability of classid if */
  /* //! perclass_norm is true, or globally otherwise. */
  /* virtual void normalize_probas(int classid = -1); */

  /* // accessors /////////////////////////////////////////////////////////////// */

  /* //! Return the number of classes. */
  /* virtual intg get_nclasses(); */
  /* //! Return the label id corresponding to name, or -1 if not found. */
  /* virtual int get_class_id(const char *name); */
  /* //! Return the label string for index id. */
  /* virtual string& get_class_name(int id); */
  /* //! Returns a reference to a vector of each label string. */
  /* virtual std::vector<string*>& get_label_strings(); */
  /* //! Return the lowest (non-zero) size per class, multiplied by the number */
  /* //! of classes. */
  /* //! e.g. if a dataset has 10 classes with 100 examples and 5 classes with */
  /* //! 50 examples, it will return 50 * (10 + 5) = 750, whereas size() */
  /* //! will return 1250. */
  /* //! This is useful to keep iterations to a meaningful size when a class */
  /* //! has many more examples than another. */
  /* virtual intg get_lowest_common_size(); */

  /* // picking methods ///////////////////////////////////////////////////////// */

  /* //! Output statistics of samples picking, i.e. the number of times each */
  /* //! sample has been picked for training. */
  /* virtual void save_pickings(const char *name = NULL);     */
  /* //! Write plot of m organized by class and correctness */
  /* template <typename T> */
  /*   void write_classed_pickings(idx<T> &m, idx<ubyte> &correct, */
  /* 				  string &name_, const char *name2_ = NULL, */
  /* 				  bool plot_correct = true, */
  /* 				  const char *ylabel = "");     */

  /* // state saving //////////////////////////////////////////////////////////// */

  /* //! Save internal iterators. Calling restore_state() will return to the */
  /* //! current sample. */
  /* virtual void save_state();     */
  /* //! Restore previously saved internal iterators. */
  /* virtual void restore_state(); */

  // pretty methods //////////////////////////////////////////////////////////

  //! Print info about the datasource on the standard output.
  virtual void pretty();
  /* //! Pretty the progress of current epoch. */
  /* //! \param newline If true, end pretty with a new line. */
  /* virtual void pretty_progress(bool newline = true); */

  //! Print path reaching class 'l'.
  virtual void print_path(Tlabel l);

  // friends //////////////////////////////////////////////////////////////
  template <typename T1, typename T2, typename T3>
  friend class class_datasource_gui;

  // members /////////////////////////////////////////////////////////////////
 protected:
  // hierarchy
  std::vector<class_node<Tlabel>*> all_nodes; //!< All hierarchy nodes.
  std::vector<class_node<Tlabel>*> all_nodes_by_depth; //!< All, ordered by depth.
  std::vector<std::vector<class_node<Tlabel>*>*> all_depths; //!< All nodes by depth.
  //! All nodes by depth, plus ones with lower depth that contain samples
  //! internally. When iterating at one depth ensures samples at lower depth
  //! are also used.
  std::vector<std::vector<class_node<Tlabel>*>*> complete_depths;
  // data
  idx<Tlabel> *parents; //!< Parents matrix.
  idx<Tlabel> depth_labels; //!< All labels given a certain depth.
  using labeled_datasource<T,Tdata,Tlabel>::labels;
  using class_datasource<T,Tdata,Tlabel>::olabels;
  using datasource<T,Tdata>::_name;
  // classes
  using class_datasource<T,Tdata,Tlabel>::nclasses;//!< Number of classes.
  using class_datasource<T,Tdata,Tlabel>::lblstr;	//!< Name of each class.
  using class_datasource<T,Tdata,Tlabel>::clblstr;	//!< Name of each class.
  using datasource<T,Tdata>::data;
  using datasource<T,Tdata>::correct;
  using datasource<T,Tdata>::energies;
  using datasource<T,Tdata>::probas;
  // iterating
  uint current_depth; //!< Maximum depth at which labels are set.
  std::vector<uint> it_depths; //!< Depths iterators.
  using datasource<T,Tdata>::it;
  /* using datasource<T,Tdata>::epoch_mode; */
  /* using datasource<T,Tdata>::epoch_show; */
  using datasource<T,Tdata>::epoch_sz;
  /* using datasource<T,Tdata>::epoch_timer; */
  /* using datasource<T,Tdata>::epoch_show_printed; */
  // class-balanced iterating indices
  /* using datasource<T,Tdata>::epoch_done_counters; */
  bool depth_balance; //!< Balance samples by depth.
  using class_datasource<T,Tdata,Tlabel>::balance;	//!< Balance iterating or not.
  /* std::vector<std::vector<intg> > bal_indices;	//!< Balanced iterating indices. */
  /* std::vector<uint>	 bal_it;	//!< Sample iterators for each class. */
  /* uint		 class_it;	//!< Iterator on classes. */
  /* // sample picking with probabilities */
  /* bool		 perclass_norm;	//!< Normalize probas per class. */
  using datasource<T,Tdata>::epoch_pick_cnt;	//!< # pickings
  using datasource<T,Tdata>::epoch_cnt;	//!< # sample seen this epoch.
  /* using datasource<T,Tdata>::count_pickings; */
  using datasource<T,Tdata>::pick_count;
  /* using datasource<T,Tdata>::counts; */
  using datasource<T,Tdata>::weigh_samples;
  /* // state saving */
  /* std::vector<std::vector<intg> > bal_indices_saved; */
  /* std::vector<uint>        bal_it_saved; */
  /* uint                class_it_saved; */
  /* using datasource<T,Tdata>::state_saved; */
  /* using datasource<T,Tdata>::count_pickings_save; */
  /* using datasource<T,Tdata>::it_saved; */
  /* using datasource<T,Tdata>::it_test; */
  /* using datasource<T,Tdata>::it_test_saved; */
  /* using datasource<T,Tdata>::it_train; */
  /* using datasource<T,Tdata>::it_train_saved; */
  // misc
  using datasource<T,Tdata>::sampledims;
  using datasource<T,Tdata>::test_set;
  /* using datasource<T,Tdata>::shuffle_passes; */
  /* using datasource<T,Tdata>::not_picked; */
};

// labeled_pair_datasource /////////////////////////////////////////////////////

//! A class for input pairs datasource.
template <typename T, typename Tdata, typename Tlabel>
class labeled_pair_datasource
    : public labeled_datasource<T, Tdata, Tlabel> {
 public:
  idx<intg>					pairs;
  /* typename idx<intg>::dimension_iterator	pairsIter; */

  //! Constructor from dataset file names.
  labeled_pair_datasource(const char *data_fname, const char *labels_fname,
                          const char *classes_fname, const char *pairs_fname,
                          const char *name_ = NULL,
                          Tdata bias = 0, float coeff = 1.0);
  //! Constructor from dataset matrices.
  labeled_pair_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
                          idx<ubyte> &classes_, idx<intg> &pairs_,
                          const char *name_ = NULL,
                          Tdata bias = 0, float coeff = 1.0);
  //! destructor.
  virtual ~labeled_pair_datasource();

  //! Copies the current datum to a state and label.
  virtual void fprop(state<T> &d1, state<T> &d2, state<Tlabel> &label);
  //! Move to the next datum. Returns false if we reached the end.
  virtual bool next();
  //! Move to the beginning of the data, for the test iterators only,
  //! i.e. only next() is affected, next_train() is unaffected.
  virtual void seek_begin();
  //! Returns the number of pairs contained in this data source.
  virtual unsigned int size();
};

// mnist_datasource ////////////////////////////////////////////////////////////

//! A datasource specific to the MNIST dataset. The specific operations
//! for MNIST are padding the data from 28x28 to 32x32 and multiplying it
//! by a coefficient of .01 (brining the input range from [0,255] to [0,2.55]
//! (the coefficient and bias can be overriden via the inherited
//! set_data_coeff() and set_data_bias() methods).
template <typename T, typename Tdata, typename Tlabel>
class mnist_datasource : public class_datasource<T, Tdata, Tlabel> {
 public:
  //! Create an MNIST dataset using the original MNIST filenames,
  //! in 'root' directory. Set 'train_data' to true to load the training data,
  //! false to load the testing data.
  //! \param size The number of samples to use, can go up to 10k for testing
  //!    and 60k for training.
  mnist_datasource(const char *root, bool train_data, uint size);
  //! Create an MNIST dataset using files found in root directory.
  //! Name can be used to differentiate between training, testing and
  //! validataion data. The file name will be constructed as follow:
  //! {root}/{name}_{data|labels}.mat
  //! \param size The number of samples to use, can go up to 10k for testing
  //!    and 60k for training.
  mnist_datasource(const char *root, const char *name, uint size);
  //! Destructor.
  virtual ~mnist_datasource();

  //! Copy current sample's data into s.
  virtual void fprop1_data(idx<T> &s);

 protected:
  //! Initialize with 'data' and 'labels', and an optional 'name'.
  virtual void init(idx<Tdata> &data, idx<Tlabel> &labels, const char *name);

  // members /////////////////////////////////////////////////////////////////
 public:
  using datasource<T,Tdata>::bias;
  using datasource<T,Tdata>::coeff;
  using datasource<T,Tdata>::sampledims;
  using datasource<T,Tdata>::height;
  using datasource<T,Tdata>::width;
  using datasource<T,Tdata>::data;
  using datasource<T,Tdata>::datas;
  using datasource<T,Tdata>::multimat;
  using labeled_datasource<T,Tdata,Tlabel>::labels;
  using datasource<T,Tdata>::it;
};

// Helper functions ////////////////////////////////////////////////////////////

//! Fill a matrix with 1-of-n code.
//! Return an idx<double> with <nclasses> targets in it, where the target
//! value for the correct answer is target and -target for the rest.
template <typename Tdata>
idx<Tdata> create_target_matrix(intg nclasses, Tdata target);

} // end namespace ebl

#include "datasource.hpp"

#endif /* DATASOURCE_H_ */
