/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet   *
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

#ifndef DATASOURCE_HPP_
#define DATASOURCE_HPP_

#include <ostream>
#include <algorithm>

namespace ebl {

// datasource //////////////////////////////////////////////////////////////////

template <typename T, typename Tdata>
datasource<T,Tdata>::datasource() {
}

template <typename T, typename Tdata>
datasource<T,Tdata>::
datasource(midx<Tdata> &data_, const char *name_) {
  multimat = true; // data matrix is composed of multiple matrices
  init(data_, name_);
  init_epoch();
  pretty(); // print information about the dataset
}

template <typename T, typename Tdata>
datasource<T,Tdata>::
datasource(idx<Tdata> &data_, const char *name_) {
  multimat = false; // data matrix is composed of multiple matrices
  init(data_, name_);
  init_epoch();
  pretty(); // print information about the dataset
}

template <typename T, typename Tdata>
datasource<T,Tdata>::
datasource(const char *data_fname, const char *name_) {
  try {
		// try to load as csv if file is not a matrix
		if (!is_matrix(data_fname)) {
      multimat = false;
      idx<Tdata> data_ = load_csv_matrix<Tdata>(data_fname);
      init(data_, name_);
		} else {
			// matrix format
			if (has_multiple_matrices(data_fname)) {
				multimat = true;
				midx<Tdata> datas_ = load_matrices<Tdata>(data_fname);
				init(datas_, name_);
			} else {
				multimat = false;
				idx<Tdata> data_ = load_matrix<Tdata>(data_fname);
				init(data_, name_);
			}
		}
    init_epoch();
    pretty(); // print information about the dataset
  } eblcatcherror();
}

template <typename T, typename Tdata>
datasource<T,Tdata>::~datasource() {
}

//////////////////////////////////////////////////////////////////////////////
// init methods

template<typename T, typename Tdata>
void datasource<T,Tdata>::
init(midx<Tdata> &datas_, const char *name_) {
  datas = datas_;
  data = (idx<Tdata>&) datas_;
  multimat = true;
  init2(name_);
}

template<typename T, typename Tdata>
void datasource<T,Tdata>::
init(idx<Tdata> &data_, const char *name_) {
  multimat = false;
  add_features_dimension_ = false;
  data = data_;
  init2(name_);
}

template<typename T, typename Tdata>
void datasource<T,Tdata>::
init2(const char *name_) {
  // init randomization
  if (!drand_ini) // only re-init if not initialized
    dynamic_init_drand(); // initialize random seed
  // no bias and coeff by default (0 and 1)
  bias = (T) 0;
  coeff = (T) 1.0;
  // iterating
  it = 0;
  it_test = 0;
  it_train = 0;
  shuffle_passes = false;
  test_set = false;
  epoch_sz = 0;
  epoch_cnt = 0;
  epoch_pick_cnt = 0;
  epoch_mode = 1; // default (1): all samples are seen at least once.
  hardest_focus = false;
  _ignore_correct = false;
  // state saving
  state_saved = false;
  // buffers assigments/allocations
  indices = idx<intg>(data.dim(0));
  indices_saved = idx<intg>(data.dim(0));
  probas = idx<double>(data.dim(0));
  energies = idx<double>(data.dim(0));
  raw_outputs = idx<T>(1, 1);
  pick_count = idx<uint>(data.dim(0));
  correct = idx<ubyte>(data.dim(0));
  incorrect = idx<ubyte>(data.dim(0));
  answers = idx<T>(1, 1);
  targets = idx<T>(1, 1);
  // pickings
  idx_clear(pick_count);
  count_pickings = true;
  sample_min_proba = 0.0;
  // intialize buffers
  idx_fill(correct, 0);
  idx_fill(incorrect, 0);
  idx_fill(answers, 0);
  idx_fill(targets, 0);
  idx_fill(probas, 1.0); // default picking probability for a sample is 1
  idx_fill(energies, -1.0);
  idx_fill(raw_outputs, 0);
  _name = (name_ ? name_ : "Unknown Dataset");
  // iterating
  set_shuffle_passes(true); // for next_train only
  set_weigh_samples(true, true, true, 0.0); // for next_train only
  seek_begin();
  seek_begin_train();
  epoch_sz = size(); //get_lowest_common_size();
  epoch_mode = 1;
  std::cout << _name << ": Each training epoch sees " << epoch_sz
            << " samples." << std::endl;
  not_picked = 0;
  epoch_show = 50; // print epoch count message every epoch_show
  epoch_show_printed = -1; // last epoch count we have printed
  // fill indices with original data order
  for (it = 0; it < data.dim(0); ++it)
    indices.set(it, it);
  // set sample dimensions
  if (multimat) {
    bool found = false;
    uint i = 0;
    idx<Tdata> e;
    if (datas.order() == 2) {
      for (intg i = 0; i < datas.dim(0) && !found; ++i) {
        for (intg j = 0; j < datas.dim(1); ++j) {
          if (datas.exists(i, j)) {
            found = true;
            e = datas.mget(i, j);
            samplemfdims.push_back_new(e.get_idxdim());
          }
        }
      }
    } else {
      while (!found && i < datas.dim(0)) {
        if (datas.exists(i)) {
          found = true;
          e = datas.mget(i);
          samplemfdims.push_back_new(e.get_idxdim());
        }
        std::cout << std::endl;
        i++;
      }
    }
    if (!found)
      eblerror("no sample found in multi-matrix data " << datas);
    sampledims = e.get_idxdim();
  } else
    sampledims = idxdim(data.select(0, 0));
  if (sampledims.order() == 2)
    sampledims.insert_dim(0, 1);
  if (sampledims.order() > 2) {
    height = sampledims.dim(1);
    width = sampledims.dim(2);
  }
  // initialize index to 0
  it = 0;
  // shuffle data indices
  shuffle();
  bkeep_outputs = false;
}

//////////////////////////////////////////////////////////////////////////////
// accessors

template <typename T, typename Tdata>
unsigned int datasource<T,Tdata>::size() {
  if (multimat)
    return datas.dim(0);
  return data.dim(0);
}

template <typename T, typename Tdata>
idxdim datasource<T,Tdata>::sample_dims() {
  return sampledims;
}

template <typename T, typename Tdata>
mfidxdim datasource<T,Tdata>::sample_mfdims() {
  return samplemfdims;
}

template <typename T, typename Tdata>
std::string& datasource<T,Tdata>::name() {
  return _name;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::set_test() {
  test_set = true;
  std::cout << _name << ": This is a testing set only." << std::endl;
}

template <typename T, typename Tdata>
bool datasource<T,Tdata>::is_test() {
  return test_set;
}

template <typename T, typename Tdata>
intg datasource<T,Tdata>::get_epoch_size() {
  return epoch_sz;
}

template <typename T, typename Tdata>
intg datasource<T,Tdata>::get_epoch_count() {
  return epoch_cnt;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::set_epoch_size(intg sz) {
  std::cout << _name << ": Setting epoch size to " << sz << std::endl;
  epoch_sz = sz;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::set_epoch_mode(uint mode) {
  epoch_mode = mode;
  std::cout << _name << ": Setting epoch mode to " << epoch_mode;
  switch (epoch_mode) {
    case 0: std::cout << " (fixed number of samples)" << std::endl; break ;
    case 1: std::cout << " (see all samples at least once)" << std::endl;break ;
    default: eblerror("unknown mode");
  }
}

// data access methods /////////////////////////////////////////////////////////

template <typename T, typename Tdata>
void datasource<T,Tdata>::fprop1_data(idx<T> &out) {
  // get sample
  idx<Tdata> dat;
  if (multimat) dat = datas.mget(it);
  else dat = data[it];
  // resize output if necessary
  idxdim d = dat.get_idxdim();
  if (add_features_dimension_) d.insert_dim(0, 1);
  if (out.order() != d.order()) out = idx<T>(d);
  else out.resize(d);
  // copy to output
  idx_copy(dat, out);
  // bias and scaling
  if (bias != 0.0) idx_addc(out, bias, out);
  if (coeff != 1.0) idx_dotc(out, coeff, out);
#ifdef __DEBUG__
  if (out.nelements() < 50) out.print();
#endif
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::fprop_data(state<T> &out) {
  // copy data
  if (multimat) { // multiple matrices per sample
    midx<Tdata> sample = datas.select(0, it);
    out.deep_copy(sample);
  } else this->fprop1_data(out); // single matrix per sample
  EDEBUG_MAT("datasource sample " << it << ":", out);
}

template <typename T, typename Tdata>
idx<Tdata> datasource<T,Tdata>::get_sample(intg index) {
  if (multimat) return datas.mget(index);
  else return data[index];
}

template <typename T, typename Tdata>
idx<T> datasource<T,Tdata>::get_raw_output(intg index) {
  if (index >= 0)
    return raw_outputs[index];
  return raw_outputs[it];
}

// iterating methods ///////////////////////////////////////////////////////////

template <typename T, typename Tdata>
void datasource<T,Tdata>::select_sample(intg index) {
  if (index < 0 || index >= data.dim(0))
    eblthrow("cannot select index " << index
             << " in datasource of dimensions " << data);
  it = index;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::shuffle() {
  // shuffle indices to the data
  idx_shuffle(indices);
}

template <typename T, typename Tdata>
bool datasource<T,Tdata>::next() {
  // increment test iterator
  it_test++;
  // reset if reached end
  if (it_test >= data.dim(0)) {
    seek_begin();
    return false;
  }
  // set main iterator used by fprop
  it = it_test;
  return true;
}

template <typename T, typename Tdata>
bool datasource<T,Tdata>::next_train() {
  // check that this datasource is allowed to call this method
  if (test_set)
    eblerror("forbidden call of next_train() on testing sets");
  bool pick = false;
  not_picked++;
  // increment iterator
  it_train++;
  // reset if reached end
  if (it_train >= indices.dim(0)) {
    if (shuffle_passes)
      shuffle(); // shuffle indices to the data
    // reset iterator
    seek_begin_train();
    // normalize probabilities, mapping [0..max] to [0..1]
    if (weigh_samples)
      normalize_probas();
  }
  it = indices.get(it_train); // set main iterator to the train iterator
  // recursively loop until we find a sample that is picked for this class
  pick = this->pick_current();
  epoch_cnt++;
  if (pick) {
#ifdef __DEBUG__
    std::cout << "Picking sample " << it << ", pickings: " << pick_count.get(it)
              << ", energy: " << energies.get(it) << ", correct: "
              << (int) correct.get(it);
    if (weigh_samples) std::cout << ", proba: " << probas.get(it) << ")";
    std::cout << std::endl;
#endif
    // increment pick counter for this sample
    if (count_pickings) pick_count.set(pick_count.get(it) + 1, it);
    // increment sample counter
    epoch_pick_cnt++;
    not_picked = 0;
    return true;
  } else {
    EDEBUG("Not picking sample " << it << ", pickings: " << pick_count.get(it)
           << ", energy: " << energies.get(it) << ", correct: "
           << (int) correct.get(it) << ", proba: " << probas.get(it) << ")");
    return false;
  }
}

// accessors ///////////////////////////////////////////////////////////////

template <typename T, typename Tdata>
void datasource<T,Tdata>::set_data_bias(T b) {
  bias = b;
  std::cout << _name << ": Setting data bias to " << bias << std::endl;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::set_data_coeff(T c) {
  coeff = c;
  std::cout << _name << ": Setting data coefficient to " << coeff << std::endl;
}

template <typename T, typename Tdata>
bool datasource<T,Tdata>::epoch_done() {
  switch (epoch_mode) {
    case 0: // fixed number of samples
      if (epoch_cnt >= epoch_sz)
	return true;
      break ;
    case 1: // see all samples at least once
      // TODO: same as case 0?
      if (epoch_cnt >= epoch_sz)
	return true;
      break ;
    default: eblerror("unknown epoch_mode");
  }
  return false;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::init_epoch() {
  epoch_cnt = 0;
  epoch_pick_cnt = 0;
  epoch_timer.restart();
  epoch_show_printed = -1; // last epoch count we have printed
  // if we have prior information about each sample energy and classification
  // let's use it to initialize the picking probabilities.
  if (weigh_samples)
    this->normalize_all_probas();
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::seek_begin() {
  it_test = 0; // reset test iterator
  it = it_test; // set main iterator to test iterator
  test_timer.restart();
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::seek_begin_train() {
  // reset train iterator
  it_train = 0;
  // set main iterator to train iterator
  it = indices.get(it_train);
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::set_shuffle_passes(bool activate) {
  shuffle_passes = activate;
  std::cout << _name
            << ": Shuffling of samples (training only) after each pass is "
            << (shuffle_passes ? "activated" : "deactivated") << "."
            << std::endl;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::add_features_dimension() {
  add_features_dimension_ = true;
}

// picking probability methods /////////////////////////////////////////////////

template <typename T, typename Tdata>
void datasource<T,Tdata>::normalize_all_probas() {
  if (weigh_samples)
    normalize_probas();
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::normalize_probas(std::vector<intg> *cindices) {
  double maxproba = 0, minproba = (std::numeric_limits<double>::max)();
  double maxenergy = 0, sum = 0; //, energy_ratio, maxenergy2;
  std::vector<intg> allindices;
  if (weigh_samples && !is_test()) {
    if (!cindices) { // use all samples
      std::cout << _name << ": Normalizing all probabilities";
      allindices.resize(energies.dim(0)); // allocate
      for (intg i = 0; i < energies.dim(0); ++i)
        allindices[i] = i;
      cindices = &allindices;
    }
    idx<double> sorted_energies(cindices->size());
    // normalize probas for this class, mapping [0..max] to [0..1]
    maxenergy = 0; sum = 0;
    intg nincorrect = 0, ncorrect = 0, i = 0;
    // get max and sum
    for (std::vector<intg>::iterator j = cindices->begin();
         j != cindices->end(); ++j) {
      // don't take correct ones into account
      if (energies.get(*j) < 0) // energy not set yet
        continue ;
      if (correct.get(*j) == 1) { // correct
        ncorrect++;
        if (_ignore_correct)
          continue ; // skip this one
      } else
        nincorrect++;
      // max and sum
      maxenergy = (std::max)(energies.get(*j), maxenergy);
      sum += energies.get(*j);
      sorted_energies.set(energies.get(*j), i++);
    }
    std::cout << ", nincorrect: " << nincorrect << ", ncorrect: " << ncorrect;
    // no incorrect set all to 1
    if (!nincorrect) {
      idx_fill(probas, 1.0);
      std::cout << std::endl;
      return ;
    }
    // We choose 2 pivot points in the sorted energies curve,
    // one will be used as maximum energy and the other as the minimum
    // energy. This helps to have a meaningful range of energies not
    // biased by single extrema.
    double e1, e2;
    sorted_energies.resize(nincorrect);
    idx_sortup(sorted_energies);
    intg pivot1 = (intg) (sorted_energies.dim(0) * (float) .25);
    intg pivot2 = std::min(sorted_energies.dim(0) - 1,
                           (intg) (sorted_energies.dim(0)*(float)1.0));//.75);
    if (sorted_energies.dim(0) == 0) {
      e1 = 0; e2 = 1;
    } else {
      e1 = sorted_energies.get(pivot1);
      e2 = sorted_energies.get(pivot2);
    }
    // the ratio of total energies over n times the max energy
    //energy_ratio = sum / (maxenergy * cindices->size());
    // the max probability will be proportional to the energy ratio
    // this balances the probabilities so that outliers don't take
    // all the probabilites
    //maxenergy2 = maxenergy * energy_ratio;
    std::cout << ", max energy: " << maxenergy;
    // << ", energy ratio " << energy_ratio
    // << " and normalized max energy " << maxenergy2;
    // normalize
    for (std::vector<intg>::iterator j = cindices->begin();
         j != cindices->end(); ++j) {
      double e = energies.get(*j);
      // set proba 0 for correct samples if we ignore correct ones
      if (e >= 0 && _ignore_correct && correct.get(*j) == 1)
        probas.set(0.0, *j);
      else {
        // compute probas
        double den = e2 - e1;
        if (e < 0 || maxenergy == 0 || den == 0) // energy not set yet
          probas.set(1.0, *j);
        else {
          probas.set((std::max)((double) 0, (std::min)((e - e1) / den,
                                                       (double) 1)), *j);
          if (!hardest_focus) // concentrate on easiest misclassified
            probas.set(1 - probas.get(*j), *j); // reverse proba
          // iprobas.at(*j)->set((std::max)(sample_min_proba,
          // 					 e / maxenergy2));
          // remember min and max proba
          maxproba = (std::max)(probas.get(*j), maxproba);
          minproba = (std::min)(probas.get(*j), minproba);
        }
      }
    }
    std::cout << ", Min/Max probas are: " << minproba << ", "
              << maxproba << std::endl;
  }
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::set_sample_energy(double e, bool correct_,
                                            idx<T> &raw_,
                                            idx<T> &answers_,
                                            idx<T> &target) {
  energies.set(e, it);
  correct.set(correct_ ? 1 : 0, it);

  // store model outputs for current sample
  if (bkeep_outputs) {
    // resize buffers if necessary
    idx<T> ans = answers_.view_as_order(1);
    idx<T> ra = raw_.view_as_order(1);
    idxdim d(ans), draw(ra);
    d.insert_dim(0, data.dim(0));
    draw.insert_dim(0, data.dim(0));
    if (raw_outputs.get_idxdim() != draw) {
      raw_outputs.resize(draw);
      idx_clear(raw_outputs);
    }
    if (answers.get_idxdim() != d) {
      answers.resize(d);
      idx_clear(answers);
    }
    if (targets.get_idxdim() != draw) {
      targets.resize(draw);
      idx_clear(targets);
    }
    // copy raw
    idx<T> raw = raw_outputs.select(0, it);
    idx_copy(ra, raw);
    // copy answers
    idx<T> answer = answers.select(0, it);
    idx_copy(ans, answer);
    // copy target
    idx<T> tgt = targets.select(0, it);
    idx_copy(target, tgt);
  }
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::keep_outputs(bool keep) {
  bkeep_outputs = keep;
  std::cout << (bkeep_outputs ? "Keeping" : "Not keeping")
            << " model outputs for each sample." << std::endl;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::save_answers(std::string &filename) {
  if (save_csv_matrix(answers, filename))
    eblprint("Saved answers to " << filename << std::endl);
  else eblerror("Failed to save answers to " << filename);  
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::save_pickings(const char *name_) {
  // plot file
  std::string name = "pickings";
  if (name_)
    name = name_;
  std::string fname = name;
  fname += ".plot";
  std::ofstream fp(fname.c_str());
  if (!fp) {
    std::cerr << "failed to open " << fname << std::endl;
    eblerror("failed to open file for writing");
  }
  eblerror("not implemented");
  // typename idx<uint>::dimension_iterator i = pick_count.dim_begin(0);
  // uint j = 0;
  // for ( ; i.notdone(); i++, j++)
  //   fp << j << " " << i->get() << std::endl;
  // fp.close();
  std::cout << _name << ": Wrote picking statistics in " << fname << std::endl;
  // p file
  std::string fname2 = name;
  fname2 += ".p";
  std::ofstream fp2(fname2.c_str());
  if (!fp2) {
    std::cerr << "failed to open " << fname2 << std::endl;
    eblerror("failed to open file for writing");
  }
  fp2 << "plot \"" << fname << "\" with impulse" << std::endl;
  fp2.close();
  std::cout << _name << ": Wrote gnuplot file in " << fname2 << std::endl;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::ignore_correct(bool ignore) {
  _ignore_correct = ignore;
  if (ignore)
    std::cout << (ignore ? "Ignoring" : "Using") <<
	" correctly classified samples for training." << std::endl;
}

template <typename T, typename Tdata>
bool datasource<T,Tdata>::mstate_samples() {
  return multimat;
}

template <typename T, typename Tdata>
bool datasource<T,Tdata>::get_count_pickings() {
  return count_pickings;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::set_count_pickings(bool count) {
  count_pickings = count;
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::
set_weigh_samples(bool activate, bool hardest_focus_, bool perclass_norm_,
                  double min_proba) {
  weigh_samples = activate;
  hardest_focus = hardest_focus_;
  perclass_norm = perclass_norm_;
  sample_min_proba = MIN(1.0, min_proba);
  std::cout << _name
            << ": Weighing of samples (training only) based on classification is "
            << (weigh_samples ? "activated" : "deactivated") << "." << std::endl;
  if (activate) {
    std::cout << _name << ": learning is focused on "
              << (hardest_focus ? "hardest" : "easiest")
              << " misclassified samples" << std::endl;
    if (!_ignore_correct && !hardest_focus)
      std::cerr << "Warning: correct samples are not ignored and focus is on "
                << "easiest samples, this may not be optimal" << std::endl;
    std::cout << "Sample picking probabilities are normalized "
              << (perclass_norm ? "per class" : "globally")
              << " with minimum probability " << sample_min_proba << std::endl;
  }
}

// pretty methods //////////////////////////////////////////////////////////////

template <typename T, typename Tdata>
void datasource<T,Tdata>::pretty_progress(bool newline) {
  // train pretty
  intg i = epoch_cnt, sz = epoch_sz;
  std::string pre = "training: ";
  // test pretty
  if (is_test()) {
    i = it_test;
    sz = this->size();
    pre = "testing: ";
  }
  // common code
  if (epoch_show > 0 && i % epoch_show == 0 && epoch_show_printed != i) {
    epoch_show_printed = i; // remember last time printed
    std::cout << pre << i << " / " << sz
              << ", elapsed: " << test_timer.elapsed() << ", ETA: "
              << test_timer.
	elapsed((long) ((sz - i) *
                        (test_timer.elapsed_seconds()
                         /(double)std::max((intg)1,i))));
    if (newline)
      std::cout << std::endl;
  }
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::pretty() {
  std::cout << _name << ": dataset \"" << _name << "\" contains " << data.dim(0)
            << " samples of dimension " << sampledims
            << " and defines an epoch as " << epoch_sz << " samples." << std::endl;
#ifdef __DEBUG__
  if (multimat) { EDEBUG("datasource::data : " << datas.info()); }
  else EDEBUG("datasource::data : " << data.info());
#endif
}

//////////////////////////////////////////////////////////////////////////////
// state saving

template <typename T, typename Tdata>
void datasource<T,Tdata>::save_state() {
  state_saved = true;
  count_pickings_save = count_pickings;
  it_saved = it; // save main iterator
  it_test_saved = it_test;
  it_train_saved = it_train;
  for (intg k = 0; k < indices.dim(0); ++k)
    indices_saved[k] = indices[k];
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::restore_state() {
  if (!state_saved)
    eblerror("state not saved, call save_state() before restore_state()");
  count_pickings = count_pickings_save;
  it = it_saved; // restore main iterator
  it_test = it_test_saved;
  it_train = it_train_saved;
  for (intg k = 0; k < indices.dim(0); ++k)
    indices[k] = indices_saved[k];
}

template <typename T, typename Tdata>
void datasource<T,Tdata>::set_epoch_show(uint modulo) {
  std::cout << _name << ": Print training count every " << modulo
            << " samples." << std::endl;
  epoch_show = modulo;
}

//////////////////////////////////////////////////////////////////////////////
// protected pickings methods

template <typename T, typename Tdata>
bool datasource<T,Tdata>::pick_current() {
  if (test_set) // check that this datasource is allowed to call this method
    eblerror("forbidden call of pick_current() on testing sets");
  if (!weigh_samples) // always pick sample when not using probabilities
    return true;
  // draw random number between 0 and 1 and return true if lower
  // than sample's probability.
  double r = drand(); // [0..1]
  if (r <= probas.get(it))
    return true;
  return false;
}

template <typename T, typename Tdata>
std::map<uint,intg>& datasource<T,Tdata>::get_pickings() {
  picksmap.clear();
  // typename idx<uint>::dimension_iterator i = pick_count.dim_begin(0);
  // uint j = 0;
  // for ( ; i.notdone(); i++, j++)
  //   picksmap[i->get()] = j;
  eblerror("not implemented");
  return picksmap;
}

////////////////////////////////////////////////////////////////
// labeled_datasource

template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T, Tdata, Tlabel>::labeled_datasource() {
}

template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T, Tdata, Tlabel>::
labeled_datasource(midx<Tdata> &data_, idx<Tlabel> &labels_,
                   const char *name_) {
  init(data_, labels_, name_);
  this->init_epoch();
  this->pretty(); // print information about this dataset
}

template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T, Tdata, Tlabel>::
labeled_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
                   const char *name_) {
  init(data_, labels_, name_);
  this->init_epoch();
  this->pretty(); // print information about this dataset
}

template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T, Tdata, Tlabel>::
labeled_datasource(const char *root_dsname, const char *name_) {
  init_root(root_dsname, name_);
  this->init_epoch();
  this->pretty(); // print information about this dataset
}

template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T, Tdata, Tlabel>::
labeled_datasource(const char *root, const char *data_name,
                   const char *labels_name, const char *jitters_name,
                   const char *scales_name, const char *name_) {
  init_root(root, data_name, labels_name, jitters_name, scales_name, name_);
  this->init_epoch();
  this->pretty(); // print information about this dataset
}

template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T, Tdata, Tlabel>::~labeled_datasource() {
}

//////////////////////////////////////////////////////////////////////////////
// init methods

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T, Tdata, Tlabel>::
init(midx<Tdata> &data_, idx<Tlabel> &labels_, const char *name) {
  scales_loaded = false;
  datasource<T,Tdata>::init(data_, name);
  init_labels(labels_, name);
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T, Tdata, Tlabel>::
init(idx<Tdata> &data_, idx<Tlabel> &labels_, const char *name) {
  scales_loaded = false;
  datasource<T,Tdata>::init(data_, name);
  init_labels(labels_, name);
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T, Tdata, Tlabel>::
init_labels(idx<Tlabel> &labels_, const char *name) {
  labels = labels_;
  label_bias = 0;
  label_coeff = 1;
  // set label dimensions
  labeldims = labels.get_idxdim();
  if (labeldims.order() > 1)
    labeldims.remove_dim(0);
  else
    labeldims.setdim(0, 1);
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T, Tdata, Tlabel>::
init(const char *data_fname, const char *labels_fname,
     const char *jitters_fname, const char *scales_fname, const char *name_,
     uint max_size) {
  // load jitters
  if (jitters_fname && strlen(jitters_fname) != 0) {
    try {
      jitters = load_matrices<float>(jitters_fname);
      jitters_maxdim = jitters.get_maxdim();
    } eblcatchwarn();
  } else std::cout << "No jitter information loaded." << std::endl;
  // load labels
  idx<Tlabel> lab;
  try {
		// try to load as csv if file is not a matrix
		if (!is_matrix(data_fname))
			lab = load_csv_matrix<Tlabel>(labels_fname);
		else // regular format
			lab = load_matrix<Tlabel>(labels_fname);
  } eblcatcherror_msg("Failed to load dataset file");
  // limit number of samples
  if (max_size > 0) {
    std::cout << "Limiting " << name_<< " to " << max_size << " samples." <<std::endl;
    //      lab = lab.narrow(0, std::min((intg) max_size, lab.dim(0)), 0);
  }
  // load data
  multimat = has_multiple_matrices(data_fname);
  try {
    if (multimat) {
      midx<Tdata> dat = load_matrices<Tdata>(data_fname);
      if (max_size > 0)
        dat = dat.narrow(0, std::min((intg) max_size, dat.dim(0)), 0);
      // init
      labeled_datasource<T, Tdata, Tlabel>::init(dat, lab, name_);
    } else {
      idx<Tdata> dat = load_matrix<Tdata>(data_fname);
      if (max_size > 0)
        dat = dat.narrow(0, std::min((intg) max_size, dat.dim(0)), 0);
      // init
      labeled_datasource<T, Tdata, Tlabel>::init(dat, lab, name_);
    }
  } eblcatcherror_msg("Failed to load dataset file");
  // load scales
  scales_loaded = false;
  if (scales_fname && strlen(scales_fname) != 0) {
    try {
      // TODO: Fix scale loading/saving
      //#ifndef __WINDOWS__
#if 0
      scales = load_matrix<intg>(scales_fname);
      scales_loaded = true;
#endif
    } eblcatchwarn();
  } else std::cout << "No scale information loaded." << std::endl;
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T, Tdata, Tlabel>::
init_root(const char *root, const char *data_name, const char *labels_name,
          const char *jitters_name, const char *scales_name,
          const char *name_) {
  std::string data_fname, labels_fname, classes_fname, jitters_fname, scales_fname;
  data_fname << root << "/" << data_name << "_" << DATA_NAME
             << MATRIX_EXTENSION;
  labels_fname << root << "/" << labels_name << "_" << LABELS_NAME
               << MATRIX_EXTENSION;
  if (jitters_name)
    jitters_fname << root << "/" << jitters_name
                  << "_" << JITTERS_NAME << MATRIX_EXTENSION;
  if (scales_name)
    scales_fname << root << "/" << scales_name
                 << "_" << SCALES_NAME << MATRIX_EXTENSION;
  init(data_fname.c_str(), labels_fname.c_str(),
       jitters_name ? jitters_fname.c_str() : NULL,
       scales_name ? scales_fname.c_str() : NULL, name_);
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T, Tdata, Tlabel>::
init_root(const char *root_dsname, const char *name_) {
  std::string data_fname, labels_fname, classes_fname, jitters_fname,
      scales_fname;
  data_fname << root_dsname << "_" << DATA_NAME << MATRIX_EXTENSION;
  labels_fname << root_dsname << "_" << LABELS_NAME << MATRIX_EXTENSION;
  classes_fname << root_dsname << "_" << CLASSES_NAME << MATRIX_EXTENSION;
  jitters_fname << root_dsname << "_" << JITTERS_NAME << MATRIX_EXTENSION;
  scales_fname << root_dsname << "_" << SCALES_NAME << MATRIX_EXTENSION;
  init(data_fname.c_str(), labels_fname.c_str(), jitters_fname.c_str(),
       scales_fname.c_str(), name_);
}

// data access /////////////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T, Tdata, Tlabel>::
fprop(state<T> &data, state<Tlabel> &label) {
  this->fprop_data(data);
  this->fprop_label(label);
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T,Tdata,Tlabel>::fprop_label(state<Tlabel> &label) {
  idx<Tlabel> lab = labels[it];
  idx_copy(lab, label);
  if (label_bias != 0) idx_addc(label, label_bias, label);
  if (label_coeff != 1) idx_dotc(label, label_coeff, label);
  EDEBUG("datasource label " << it << ":" << label.str());
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T,Tdata,Tlabel>::
fprop_label_net(state<T> &label) {
  idx<Tlabel> lab = labels[it];
  idx_copy(lab, label);
  if (label_bias != 0) idx_addc(label, label_bias, label);
  if (label_coeff != 1) idx_dotc(label, label_coeff, label);
  EDEBUG("datasource label " << it << ":" << label.str());
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T,Tdata,Tlabel>::
fprop_jitter(state<T> &jitt) {
  if (jitters.order() < 1) eblerror("jitter information was not loaded");
  if (jitters.exists(it)) {
    idx<float> j = jitters.mget(it);
    idxdim d(j.get_idxdim());
    if (!jitt.same_dim(d)) jitt.resize(d);
    idx_copy(j, jitt);
  } else { // fprop an empty jitter
    idxdim d(jitters.get_maxdim());
    d.setdim(0, 1);
    if (!jitt.same_dim(d)) jitt.resize(d);
    idx_clear(jitt);
  }
}

template <typename T, typename Tdata, typename Tlabel>
intg labeled_datasource<T,Tdata,Tlabel>::fprop_scale() {
  if (!scales_loaded) eblthrow("scales information not present");
  return scales.get(it);
}

// accessors /////////////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
bool labeled_datasource<T,Tdata,Tlabel>::included_sample(intg index) {
  if (index >= data.dim(0))
    eblerror("cannot check inclusion of sample " << index
             << ", only " << data.dim(0) << " samples");
  return true;
}

template <typename T, typename Tdata, typename Tlabel>
intg labeled_datasource<T,Tdata,Tlabel>::count_included_samples() {
  return this->size();
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T, Tdata, Tlabel>::pretty() {
  std::cout << _name << ": (regression) labeled dataset \"" << _name
            << "\" contains "
            << data.dim(0) << " samples of dimension " << sampledims
            << " and defines an epoch as " << epoch_sz << " samples.";
  pretty_scales();
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T, Tdata, Tlabel>::pretty_scales() {
  if (scales_loaded) {
    intg maxscale = idx_max(scales);
    std::vector<intg> tally(maxscale + 1, 0);
    idx_bloop1(scale, scales, intg) {
      intg s = scale.get();
      if (s < 0) eblerror("unexpected negative value");
      tally[s] = tally[s] + 1;
    }
    intg nscales = 0;
    for (intg i = 0; i < (intg) tally.size(); ++i)
      if (tally[i] > 0) nscales++;
    // print scales distribution for each class
    std::cout << _name << ": has " << nscales << " scales";
    for (intg i = 0; i < (intg) tally.size(); ++i)
      std::cout << ", " << i << ": " << tally[i];
    std::cout << std::endl;
  } else std::cout << _name << ": no scales information." << std::endl;
}

template <typename T, typename Tdata, typename Tlabel>
idxdim labeled_datasource<T, Tdata, Tlabel>::label_dims() {
  return labeldims;
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T,Tdata,Tlabel>::set_label_bias(T b) {
  label_bias = b;
  std::cout << _name << ": Setting labels bias to " << label_bias << std::endl;
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T,Tdata,Tlabel>::set_label_coeff(T c) {
  label_coeff = c;
  std::cout << _name << ": Setting labels coefficient to " << label_coeff
            << std::endl;
}

template <typename T, typename Tdata, typename Tlabel>
bool labeled_datasource<T,Tdata,Tlabel>::has_scales() {
  return scales_loaded;
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T,Tdata,Tlabel>::set_sample_correctness(bool bcorrect) {
  correct.set(bcorrect ? 1 : 0, it);
  incorrect.set(bcorrect ? 0 : 1, it);
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T,Tdata,Tlabel>::save_correct(const char *prefix) {
  std::string data_fname, labels_fname;
  data_fname << prefix << "_" << DATA_NAME << MATRIX_EXTENSION;
  labels_fname << prefix << "_" << LABELS_NAME << MATRIX_EXTENSION;
  // count number of incorrect samples
  intg n = idx_sum<intg>(correct);
  if (n == 0) {
    eblwarn("no samples were correctly classified, nothing to save");
    return ;
  }
  std::cout << _name << ": saving " << n << " correctly classified samples to "
            << prefix << std::endl;
  midx<Tdata> idata(n);
  idx<Tlabel> ilabels(n);
  intg j = 0;
  for (intg i = 0; i < this->size(); ++i) {
    if (correct.get(i) == 1) {
      idx<Tdata> tmp;
      if (multimat) tmp = this->datas.mget(i);
      else tmp = data.select(0, i);
      idata.mset(tmp, j);
      ilabels.set(labels.get(i), j);
      j++;
    }
  }
  // save
  save_matrices(idata, data_fname);
  std::cout << _name << ": saved " << data_fname << " " << idata
            << std::endl;
  save_matrix(ilabels, labels_fname);
  std::cout << _name << ": saved " << labels_fname << " " << ilabels.info()
            << std::endl;
}

template <typename T, typename Tdata, typename Tlabel>
void labeled_datasource<T,Tdata,Tlabel>::save_incorrect(const char *prefix) {
  std::string data_fname, labels_fname;
  data_fname << prefix << "_" << DATA_NAME << MATRIX_EXTENSION;
  labels_fname << prefix << "_" << LABELS_NAME << MATRIX_EXTENSION;
  // count number of incorrect samples
  intg n = idx_sum<intg>(incorrect);
  if (n == 0) {
    eblwarn("no samples were misclassified, nothing to save");
    return ;
  }
  std::cout << _name << ": saving " << n << " misclassified samples to "
            << prefix << std::endl;
  midx<Tdata> idata(n);
  idx<Tlabel> ilabels(n);
  intg j = 0;
  for (intg i = 0; i < this->size(); ++i) {
    if (incorrect.get(i) == 1) {
      idx<Tdata> tmp;
      if (multimat) tmp = this->datas.mget(i);
      else tmp = data.select(0, i);
      idata.mset(tmp, j);
      ilabels.set(labels.get(i), j);
      j++;
    }
  }
  // save
  save_matrices(idata, data_fname);
  std::cout << _name << ": saved " << data_fname << " " << idata
            << std::endl;
  save_matrix(ilabels, labels_fname);
  std::cout << _name << ": saved " << labels_fname << " " << ilabels.info()
            << std::endl;
}

////////////////////////////////////////////////////////////////
// class_datasource

template <typename T, typename Tdata, typename Tlabel>
class_datasource<T, Tdata, Tlabel>::class_datasource()
    : lblstr(NULL) {
  defaults();
}

template <typename T, typename Tdata, typename Tlabel>
class_datasource<T, Tdata, Tlabel>::
class_datasource(midx<Tdata> &data_, idx<Tlabel> &labels_,
                 std::vector<std::string*> *lblstr_, const char *name_) {
  defaults();
  init(data_, labels_, name_, lblstr_);
  this->init_epoch();
  this->pretty(); // print info about dataset
}

template <typename T, typename Tdata, typename Tlabel>
class_datasource<T, Tdata, Tlabel>::
class_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
                 std::vector<std::string*> *lblstr_, const char *name_) {
  defaults();
  init(data_, labels_, name_, lblstr_);
  this->init_epoch();
  this->pretty(); // print info about dataset
}

template <typename T, typename Tdata, typename Tlabel>
class_datasource<T, Tdata, Tlabel>::
class_datasource(midx<Tdata> &data_, idx<Tlabel> &labels_,
                 idx<ubyte> &classes, const char *name_) {
  defaults();
  init_strings(classes);
  init(data_, labels_, this->lblstr, name_);
  this->init_epoch();
  this->pretty(); // print info about dataset
}

template <typename T, typename Tdata, typename Tlabel>
class_datasource<T, Tdata, Tlabel>::
class_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
                 idx<ubyte> &classes, const char *name_) {
  defaults();
  init_strings(classes);
  init(data_, labels_, this->lblstr, name_);
  this->init_epoch();
  this->pretty(); // print info about dataset
}

template <typename T, typename Tdata, typename Tlabel>
class_datasource<T, Tdata, Tlabel>::
class_datasource(const char *data_name, const char *labels_name,
                 const char *jitters_name, const char *scales_name,
                 const char *classes_name, const char *name_) {
  defaults();
  init(data_name, labels_name, jitters_name, scales_name, classes_name,name_);
  this->init_epoch();
  this->pretty(); // print info about dataset
}

template <typename T, typename Tdata, typename Tlabel>
class_datasource<T, Tdata, Tlabel>::
class_datasource(const class_datasource<T, Tdata, Tlabel> &ds)
    : datasource<T,Tdata>((const datasource<T,Tdata>&) ds),
      lblstr(NULL) {
  defaults();
  if (ds.lblstr) {
    this->lblstr = new std::vector<std::string*>;
    for (unsigned int i = 0; i < ds.lblstr->size(); ++i) {
      this->lblstr->push_back(new std::string(*ds.lblstr->at(i)));
    }
  }
}

template <typename T, typename Tdata, typename Tlabel>
class_datasource<T, Tdata, Tlabel>::~class_datasource() {
  if (lblstr) { // this class owns lblstr and its content
    std::vector<std::string*>::iterator i = lblstr->begin();
    for ( ; i != lblstr->end(); ++i)
      if (*i)
        delete *i;
    delete lblstr;
  }
}

// init methods ////////////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::defaults() {
  balance = true;
  bexclusion = false;
  random_class_order = true;
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::
init_strings(idx<ubyte> &classes) {
  this->lblstr = NULL;
  // load classes strings
  if (classes.order() == 2) {
    this->lblstr = new std::vector<std::string*>;
    idx_bloop1(classe, classes, ubyte) {
      this->lblstr->push_back(new std::string((const char*) classe.idx_ptr()));
    }
  }
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::
init_local(std::vector<std::string*> *lblstr_) {
  nclasses = (intg) idx_max(labels) + 1;
  if (lblstr_) nclasses = std::max(nclasses, (intg) lblstr_->size());
  // assign classes strings
  this->lblstr = lblstr_;
  // if no names are given and discrete, use indices as names
  if (!this->lblstr) {
    this->lblstr = new std::vector<std::string*>;
    std::ostringstream o;
    int imax = (int) idx_max(this->labels);
    for (int i = 0; i <= imax; ++i) {
      o << i;
      this->lblstr->push_back(new std::string(o.str()));
      o.str("");
    }
  }
  init_class_labels();
  nclasses = std::max((intg) idx_max(labels) + 1, (intg) clblstr.size());
  // count number of samples per class
  counts.resize(nclasses);
  fill(counts.begin(), counts.end(), 0);
  idx_bloop1(lab, labels, Tlabel) {
    counts[(size_t)lab.gget()]++;
  }
  // balance
  set_balanced(true); // balance dataset for each class in next_train
  perclass_norm = false;
  included = nclasses;
  seek_begin();
  seek_begin_train();
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::
init(midx<Tdata> &data_, idx<Tlabel> &labels_, std::vector<std::string*> *lblstr_,
     const char *name_) {
  labeled_datasource<T, Tdata, Tlabel>::init(data_, labels_, name_);
  class_datasource<T, Tdata, Tlabel>::init_local(lblstr_);
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::
init(idx<Tdata> &data_, idx<Tlabel> &labels_, std::vector<std::string*> *lblstr_,
     const char *name_) {
  labeled_datasource<T, Tdata, Tlabel>::init(data_, labels_, name_);
  class_datasource<T, Tdata, Tlabel>::init_local(lblstr_);
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::
init(const char *data_fname, const char *labels_fname,
     const char *jitters_fname, const char *scales_fname,
     const char *classes_fname, const char *name_,
     uint max_size) {
  // load classes
  idx<ubyte> classes;
  bool classes_found = false;
  if (classes_fname && strlen(classes_fname) != 0) {
    try {
      classes = load_matrix<ubyte>(classes_fname);
      classes_found = true;
    } eblcatchwarn();
  } else
    std::cout << "No category names found, using numbers." << std::endl;
  // classes names are optional, use numbers by default if not specified
  if (classes_found) {
    this->lblstr = new std::vector<std::string*>;
    idx_bloop1(classe, classes, ubyte) {
      this->lblstr->push_back(new std::string((const char*) classe.idx_ptr()));
    }
  }
  // init
  labeled_datasource<T, Tdata, Tlabel>::
      init(data_fname, labels_fname, jitters_fname, scales_fname, name_,
           max_size);
  class_datasource<T, Tdata, Tlabel>::init_local(this->lblstr);
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::
init_root(const char *root, const char *data_name, const char *labels_name,
          const char *jitters_name, const char *scales_name,
          const char *classes_name, const char *name_) {
  std::string data_fname, labels_fname, classes_fname, jitters_fname, scales_fname;
  data_fname << root << "/" << data_name << "_" << DATA_NAME
             << MATRIX_EXTENSION;
  labels_fname << root << "/" << labels_name << "_" << LABELS_NAME
               << MATRIX_EXTENSION;
  classes_fname << root << "/" << classes_name << "_" << CLASSES_NAME
                << MATRIX_EXTENSION;
  jitters_fname << root << "/" << (jitters_name ? jitters_name :classes_name)
                << "_" << JITTERS_NAME << MATRIX_EXTENSION;
  scales_fname << root << "/" << (scales_name ? scales_name :classes_name)
               << "_" << SCALES_NAME << MATRIX_EXTENSION;
  init(data_fname.c_str(), labels_fname.c_str(), jitters_fname.c_str(),
       scales_fname.c_str(), classes_fname.c_str(), name_);
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::
init_root(const char *root_dsname, const char *name_) {
  std::string data_fname, labels_fname, classes_fname, jitters_fname, scales_fname;
  data_fname << root_dsname << "_" << DATA_NAME << MATRIX_EXTENSION;
  labels_fname << root_dsname << "_" << LABELS_NAME << MATRIX_EXTENSION;
  classes_fname << root_dsname << "_" << CLASSES_NAME << MATRIX_EXTENSION;
  jitters_fname << root_dsname << "_" << JITTERS_NAME << MATRIX_EXTENSION;
  scales_fname << root_dsname << "_" << SCALES_NAME << MATRIX_EXTENSION;
  init(data_fname.c_str(), labels_fname.c_str(), jitters_fname.c_str(),
       scales_fname.c_str(), classes_fname.c_str(), name_);
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::init_class_labels() {
  std::map<Tlabel,Tlabel> mlabs;
  if (olabels.get_idxdim() != labels.get_idxdim())
    olabels = idx<Tlabel>(labels.get_idxdim());
  idx_copy(labels, olabels); // keep original labels
  idx_sortup(labels);
  // add all classes into table
  intg i = 0;
  clblstr.clear();
  { idx_bloop1(lab, labels, Tlabel) {
      Tlabel l = lab.gget();
      if (mlabs.find(l) == mlabs.end() && (!bexclusion || !excluded[(uint)l])) {
        mlabs[l] = i++;
        clblstr.push_back((*lblstr)[(uint)l]);
      }
    }}
  // now add excluded classes into table
  { idx_bloop1(lab, labels, Tlabel) {
      Tlabel l = lab.gget();
      if (mlabs.find(l) == mlabs.end() && (bexclusion && excluded[(uint)l])) {
        mlabs[l] = i++;
      }
    }}
  // now replace all original labels by their new value
  idx_copy(olabels, labels);
  { idx_bloop1(lab, labels, Tlabel) {
      Tlabel l = lab.gget();
      if (mlabs.find(l) != mlabs.end())
        lab.sset(mlabs[l]);
      else
        eblerror("label " << (uint) l << " not found");
    }}
  // replace exclusion table
  if (bexclusion) {
    for (uint i = 0; i < excluded.size(); ++i)
      excluded[i] = i < clblstr.size() ? false : true;
  }
  if (balance)
    set_balanced(true); // balance dataset for each class in next_train
}

//////////////////////////////////////////////////////////////////////////////
// data access

template <typename T, typename Tdata, typename Tlabel>
Tlabel class_datasource<T,Tdata,Tlabel>::get_label() {
  idx<Tlabel> lab = labels[it];
  if (lab.order() == 0)
    return lab.get();
  else if (lab.order() == 1 && lab.dim(0) == 1)
    return lab.get(0);
  else
    eblerror("expected single-element labels");
  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// iterating

template <typename T, typename Tdata, typename Tlabel>
bool class_datasource<T,Tdata,Tlabel>::included_sample(intg index) {
  if (!bexclusion)
    return true;
  if (index >= data.dim(0))
    eblerror("cannot check inclusion of sample " << index
             << ", only " << data.dim(0) << " samples");
  return !excluded[(int) labels.gget(index)];
}

template <typename T, typename Tdata, typename Tlabel>
intg class_datasource<T,Tdata,Tlabel>::count_included_samples() {
  if (!bexclusion) return this->size();
  intg n = 0;
  idx_bloop1(lab, labels, Tlabel) {
    if (!excluded[(int) lab.gget()])
      n++;
  }
  return n;
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::seek_begin() {
  datasource<T,Tdata>::seek_begin();
  if (bexclusion) {
    while (excluded[(int)get_label()])
      this->next();
    EDEBUG("seek_begin to label: " << (int) get_label() << " it: " << it);
  }
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::seek_begin_train() {
  datasource<T,Tdata>::seek_begin_train();
  if (bexclusion) {
    while (excluded[(int)get_label()])
      this->next();
    EDEBUG("seek_begin to label: " << (int) get_label() << " it: " << it);
  }
}


template <typename T, typename Tdata, typename Tlabel>
bool class_datasource<T,Tdata,Tlabel>::next() {
  if (!bexclusion)
    return datasource<T,Tdata>::next();
  // handle excluded classes
  bool b = datasource<T,Tdata>::next();
  while (b && excluded[(int)get_label()])
    b = datasource<T,Tdata>::next();
  return b;
}

template <typename T, typename Tdata, typename Tlabel>
bool class_datasource<T,Tdata,Tlabel>::next_train() {
  // check that this datasource is allowed to call this method
  if (test_set)
    eblerror("forbidden call of next_train() on testing sets");
  if (!balance) // do not balance by class
    return datasource<T,Tdata>::next_train();
  bool pick = false;
  not_picked++;
  // balanced: return samples in class-balanced order
  // get pointer to first non empty class
  while (!bal_indices[class_it].size())
    next_balanced_class();
  it = bal_indices[class_it][bal_it[class_it]];
  bal_it[class_it] += 1;
  // decide if we want to select this sample for training
  pick = this->pick_current();
  // decrement epoch counter
  //      if (epoch_done_counters[class_it] > 0)
  epoch_done_counters[class_it] = epoch_done_counters[class_it] - 1;
  if (bal_it[class_it] >= bal_indices[class_it].size()) {
    // returning to begining of list for this class
    bal_it[class_it] = 0;
    // shuffling list for this class
    if (shuffle_passes) {
      std::vector<intg> &clist = bal_indices[class_it];
      random_shuffle(clist.begin(), clist.end());
    }
    if (weigh_samples)
      normalize_probas_by_class(class_it);
  }
  // recursion failsafe, allow 1000 max recursions
  if (!bexclusion &&
          not_picked > MIN(1000, (intg) bal_indices[class_it].size())) {
    // we called recursion on this method more than number of class samples
    // give up and show current sample
    pick = true;
  }
  if (pick) {
    // increment pick counter for this sample
    if (count_pickings) pick_count.set(pick_count.get(it) + 1, it);
#ifdef __DEBUG__
    std::cout << "Picking sample " << it << " (label: " << (int)get_label()
              << ", name: " << *(clblstr[(int)get_label()]) << ", pickings: "
              << pick_count.get(it) << ", energy: " << energies.get(it)
              << ", correct: " << (int) correct.get(it);
    if (weigh_samples) std::cout << ", proba: " << probas.get(it);
    std::cout << ")" << std::endl;
#endif
    epoch_cnt++;
    // increment sample counter
    epoch_pick_cnt++;
    // if we picked a sample, jump to next class
    next_balanced_class();
    not_picked = 0;
    this->pretty_progress();
    return true;
  } else {
#ifdef __DEBUG__
    if (!bexclusion)
      std::cout << "Not picking sample " << it << " (label: "
                << (int) labels.gget(it) << ", pickings: " << pick_count.get(it)
                << ", energy: " << energies.get(it)
                << ", correct: " << (int) correct.get(it)
                << ", proba: " << probas.get(it) << ")" << std::endl;
#endif
    this->pretty_progress();
    return false;
  }
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::next_balanced_class() {
  // classes have custom chances to be picked
  if (class_probabilities.size() > 0) {
    float r = (float) drand(); // random number in [0,1]
    float total = 0; 
    for (class_it = 0; r > total && class_it < class_probabilities.size(); 
	 ++class_it) {
      total += class_probabilities[class_it];
    }
    class_it--;
  } else { // all classes have equal chance
    class_it_it++;
    if (class_it_it >= class_order.size()) {
      class_it_it = 0;
      reset_class_order();
    }
    class_it = class_order[class_it_it];
  }
  if (bexclusion && excluded[class_it])
    return next_balanced_class();
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::reset_class_order() {
  if (random_class_order) // randomize classes order
    random_shuffle(class_order.begin(), class_order.end());
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::set_balanced(bool bal) {
  balance = bal;
  if (!balance) // unbalanced
    std::cout << _name << ": Setting training as unbalanced (not taking class "
              << "distributions into account)." << std::endl;
  else { // balanced
    std::cout << _name << ": Setting training as balanced (taking class "
              << "distributions into account)." << std::endl;
    // compute vector of sample indices for each class
    bal_indices.clear();
    bal_it.clear();
    epoch_done_counters.clear();
    class_it = 0;
    class_it_it = 0;
    for (intg i = 0; i < nclasses; ++i) {
      std::vector<intg> indices;
      bal_indices.push_back(indices);
      bal_it.push_back(0); // init iterators
      class_order.push_back(i); // init iterators
    }
    reset_class_order();
    // distribute sample indices into each vector based on label
    for (uint i = 0; i < this->size(); ++i)
      bal_indices[(intg) (labels.gget(i))].push_back(i);
    for (uint i = 0; i < bal_indices.size(); ++i) {
      // shuffle
      random_shuffle(bal_indices[i].begin(), bal_indices[i].end());
      // init epoch counters
      epoch_done_counters.push_back(bal_indices[i].size());
    }
  }
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::set_random_class_order(bool ran) {
  random_class_order = ran;
  std::cout << "Classes order is " << (random_class_order ? "" : "not")
            << " random." << std::endl;
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::
set_class_probabilities(std::vector<float> &probas) {
  // check that there are as many probas as classes
  if (probas.size() != nclasses) 
    eblerror("expected as many class probabilities as classes but got "
	     << probas.size() << " probabilities for " << nclasses
	     << " classes");
  // normalize probas
  float total = 0;
  for (uint i = 0; i < probas.size(); ++i) total += probas[i];
  class_probabilities.clear();
  for (uint i = 0; i < probas.size(); ++i) 
    class_probabilities.push_back(probas[i]/total);
  eblprint(_name << ": classes are picked with following probabilities: " 
	   << class_probabilities << std::endl);
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::
limit_classes(intg n, intg offset, bool random) {
  // enable exclusion except for the n classes after offset
  if ((offset == 0 && n >= nclasses) || offset >= nclasses) {
    eblwarn("ignoring attempt to limit classes to " << n
            << " starting at offset " << offset << " because there are only "
            << nclasses << " classes");
  } else {
    bexclusion = true;
    excluded.clear();
    for (intg i = 0; i < nclasses; ++i) {
      if (i < offset || i >= offset + n)
        excluded.push_back(true);
      else
        excluded.push_back(false);
    }
    if (random)
      random_shuffle(excluded.begin(), excluded.end());
    included = std::min(nclasses, n + offset) - offset;
    std::cout << "Excluded all but " << included
              << " classes (offset: " << offset << ", n: " << n << ")" << std::endl;
    init_class_labels();
  }
}

template <typename T, typename Tdata, typename Tlabel>
bool class_datasource<T,Tdata,Tlabel>::epoch_done() {
  switch (epoch_mode) {
    case 0: // fixed number of samples
      if (epoch_cnt >= epoch_sz)
	return true;
      break ;
    case 1: // see all samples at least once
      if (balance) {
	// check that all classes are done
	for (uint i = 0; i < epoch_done_counters.size(); ++i) {
          if (epoch_done_counters[i] > 0)
            return false;
	}
	return true; // all classes are done
      } else { // do not balance, use epoch_sz
	if (epoch_cnt >= epoch_sz)
          return true;
      }
      break ;
    default: eblerror("unknown epoch_mode");
  }
  return false;
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::init_epoch() {
  epoch_cnt = 0;
  epoch_pick_cnt = 0;
  epoch_timer.restart();
  epoch_show_printed = -1; // last epoch count we have printed
  if (balance) {
    uint maxsize = 0;
    // for balanced training, set each class to not done.
    for (uint k = 0; k < bal_indices.size(); ++k) {
      epoch_done_counters[k] = bal_indices[k].size();
      if (bal_indices[k].size() > maxsize)
        maxsize = bal_indices[k].size();
    }
    if (epoch_mode == 1) // for ETA estimation only, estimate epoch size
      epoch_sz = maxsize * bal_indices.size();
  }
  // if we have prior information about each sample energy and classification
  // let's use it to initialize the picking probabilities.
  if (weigh_samples)
    this->normalize_all_probas();
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::normalize_all_probas() {
  if (weigh_samples) {
    if (perclass_norm && balance) {
      for (uint i = 0; i < bal_indices.size(); ++i)
        normalize_probas_by_class(i);
    } else
      this->normalize_probas();
  }
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::normalize_probas_by_class(int classid) {
  std::vector<intg> *cindices = NULL;
  if (perclass_norm && balance) { // use only class_it class samples
    if (classid < 0)
      eblerror("class id cannot be negative");
    uint class_it = (uint) classid;
    cindices = &(bal_indices[class_it]);
    std::cout << _name << ": Normalizing probabilities for class" << class_it;
    datasource<T,Tdata>::normalize_probas(cindices);
  } else // use all samples
    datasource<T,Tdata>::normalize_probas();
}

//////////////////////////////////////////////////////////////////////////////
// accessors

template <typename T, typename Tdata, typename Tlabel>
intg class_datasource<T, Tdata, Tlabel>::get_nclasses() {
  if (bexclusion)
    return included;
  return nclasses;
}

template <typename T, typename Tdata, typename Tlabel>
int class_datasource<T, Tdata, Tlabel>::get_class_id(const char *name) {
  int id_ = -1;
  std::vector<std::string*>::iterator i = clblstr.begin();
  for (int j = 0; i != clblstr.end(); ++i, ++j) {
    if (!strcmp(name, (*i)->c_str()))
      id_ = j;
  }
  return id_;
}

template <typename T, typename Tdata, typename Tlabel>
std::string &class_datasource<T, Tdata, Tlabel>::get_class_name(int id) {
  if (id >= (int) clblstr.size())
    eblerror("requesting label string at index " << id
             << " but string vector has only " << clblstr.size()
             << " elements.");
  std::string *s = clblstr[id];
  if (!s)
    eblerror("empty label string");
  return *s;
}

template <typename T, typename Tdata, typename Tlabel>
std::vector<std::string*>& class_datasource<T, Tdata, Tlabel>::
get_label_strings() {
  return clblstr;
}

template <typename T, typename Tdata, typename Tlabel>
intg class_datasource<T,Tdata,Tlabel>::get_lowest_common_size() {
  intg min_nonzero = (std::numeric_limits<intg>::max)();
  for (std::vector<intg>::iterator i = counts.begin(); i != counts.end(); ++i) {
    if ((*i < min_nonzero) && (*i != 0))
      min_nonzero = *i;
  }
  if (min_nonzero == (std::numeric_limits<intg>::max)())
    eblerror("empty dataset");
  return min_nonzero * nclasses;
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::save_pickings(const char *name_) {
  // non-class plotting
  datasource<T,Tdata>::save_pickings(name_);
  std::string name = "pickings";
  if (name_)
    name = name_;
  // plot by class
  write_classed_pickings(pick_count, correct, name);
  write_classed_pickings(energies, correct, name, "_energies");
  idx<double> e = idx_copy(energies);
  idx<ubyte> c = idx_copy(correct);
  idx_sortup(e, c);
  write_classed_pickings(e, c, name, "_sorted_energies");
  idx<double> p = idx_copy(probas);
  c = idx_copy(correct);
  idx_sortup(p, c);
  write_classed_pickings(p, c, name, "_sorted_probas");
  p = idx_copy(probas);
  e = idx_copy(energies);
  c = idx_copy(correct);
  idx_sortup(e, c, p);
  write_classed_pickings(p, c, name, "_probas_sorted_by_energy", true,
                         "Picking probability");
  write_classed_pickings(p, c, name, "_probas_sorted_by_energy_wrong_only",
                         false, "Picking probability");
  write_classed_pickings(e, c, name, "_energies_sorted_by_energy_wrong_only",
                         false, "Energy");
}

template <typename T, typename Tdata, typename Tlabel>
template <typename T2>
void class_datasource<T,Tdata,Tlabel>::
write_classed_pickings(idx<T2> &m, idx<ubyte> &c, std::string &name_,
                       const char *name2_, bool plot_correct,
                       const char *ylabel) {
  std::string name = name_;
  if (name2_)
    name += name2_;
  name += "_classed";
  // sorted classed plot file
  if (labels.order() == 1) { // single label value
    std::string fname = name;
    fname += ".plot";
    std::ofstream fp(fname.c_str());
    if (!fp) {
      std::cerr << "failed to open " << fname << std::endl;
      eblerror("failed to open file for writing");
    }
    eblerror("not implemented");
    // typename idx<T>::dimension_iterator i = m.dim_begin(0);
    // typename idx<Tlabel>::dimension_iterator l = labels.dim_begin(0);
    // typename idx<ubyte>::dimension_iterator ic = c.dim_begin(0);
    // uint j = 0;
    // for ( ; i.notdone(); i++, l++, ic++) {
    // 	if (!plot_correct && ic->get() == 1)
    // 		continue ; // don't plot correct ones
    // 	fp << j++;
    // 	for (Tlabel k = 0; k < (Tlabel) nclasses; ++k) {
    // 		if (k == l.get()) {
    // 			if (ic->get() == 0) { // wrong
    // 				fp << "\t" << i->get();
    // 				if (plot_correct)
    // 		fp << "\t?";
    // 			} else if (plot_correct) // correct
    // 				fp << "\t?\t" << i->get();
    // 		} else {
    // 			fp << "\t?";
    // 			if (plot_correct)
    // 				fp << "\t?";
    // 		}
    // 	}
    // 	fp << std::endl;
    // }
    fp.close();
    std::cout << _name << ": Wrote picking statistics in " << fname << std::endl;
    // p file
    std::string fname2 = name;
    fname2 += ".p";
    std::ofstream fp2(fname2.c_str());
    if (!fp2) {
      std::cerr << "failed to open " << fname2 << std::endl;
      eblerror("failed to open file for writing");
    }
    fp2 << "set title \"" << name << "\"; set ylabel \"" << ylabel
        << "\"; plot \""
        << fname << "\" using 1:2 title \"class 0 wrong\" with impulse";
    if (plot_correct)
      fp2 << ", \""
          << fname << "\" using 1:3 title \"class 0 correct\" with impulse";
    for (uint k = 1; k < nclasses; ++k) {
      fp2 << ", \"" << fname << "\" using 1:" << k * (plot_correct?2:1) + 2
          << " title \"class " << k << " wrong\" with impulse";
      if (plot_correct)
        fp2 << ", \"" << fname << "\" using 1:" << k * 2 + 3
            << " title \"class " << k << " correct\" with impulse";
    }
    fp << std::endl;
    fp2.close();
    std::cout << _name << ": Wrote gnuplot file in " << fname2 << std::endl;
  }
}

//////////////////////////////////////////////////////////////////////////////
// state saving

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::save_state() {
  state_saved = true;
  count_pickings_save = count_pickings;
  it_saved = it; // save main iterator
  it_test_saved = it_test;
  it_train_saved = it_train;
  this->epoch_cnt_saved = epoch_cnt;
  this->epoch_pick_cnt_saved = epoch_pick_cnt;
  this->epoch_done_counters_saved = epoch_done_counters;
  if (!balance) // save (unbalanced) iterators
    datasource<T,Tdata>::save_state();
  else { // save balanced iterators
    bal_it_saved.clear();
    bal_indices_saved.clear();
    for (uint k = 0; k < bal_it.size(); ++k) {
      bal_it_saved.push_back(bal_it[k]);
      std::vector<intg> indices;
      for (uint l = 0; l < bal_indices[k].size(); ++l)
        indices.push_back(bal_indices[k][l]);
      bal_indices_saved.push_back(indices);
    }
    class_it_saved = class_it;
    class_it_it_saved = class_it_it;
  }
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::restore_state() {
  if (!state_saved)
    eblerror("state not saved, call save_state() before restore_state()");
  count_pickings = count_pickings_save;
  it = it_saved; // restore main iterator
  it_test = it_test_saved;
  it_train = it_train_saved;
  epoch_cnt = this->epoch_cnt_saved;
  epoch_pick_cnt = this->epoch_pick_cnt_saved;
  epoch_done_counters = this->epoch_done_counters_saved;
  if (!balance) // restore unbalanced
    datasource<T,Tdata>::restore_state();
  else { // restore balanced iterators
    for (uint k = 0; k < bal_it.size(); ++k) {
      bal_it[k] = bal_it_saved[k];
      for (uint l = 0; l < bal_indices[k].size(); ++l)
        bal_indices[k][l] = bal_indices_saved[k][l];
    }
    class_it = class_it_saved;
    class_it_it = class_it_it_saved;
  }
}

// pretty methods //////////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T,Tdata,Tlabel>::pretty_progress(bool newline) {
  if (this->is_test())
    datasource<T,Tdata>::pretty_progress(newline);
  else {
    if (epoch_show > 0 && epoch_pick_cnt % epoch_show == 0 &&
            epoch_show_printed != (intg) epoch_pick_cnt) {
      datasource<T,Tdata>::pretty_progress(false);
      if (balance && epoch_done_counters.size() < 50) {
        std::cout << ", remaining:";
        for (uint i = 0; i < epoch_done_counters.size(); ++i) {
          std::cout << " " << i << ": " << epoch_done_counters[i];
        }
      }
      if (newline)
        std::cout << std::endl;
    }
  }
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::pretty() {
  std::cout << _name << ": classification dataset \"" << _name
            << "\" contains "
            << data.dim(0) << " samples of dimension " << sampledims
            << " and defines an epoch as " << epoch_sz << " samples." << std::endl;
  std::cout << this->_name << ": It has " << nclasses << " classes: ";
  uint i;
  for (i = 0; i < this->counts.size(); ++i)
    if (!bexclusion || !excluded[i])
      std::cout << this->counts[i] << " \"" << *(clblstr[i]) << "\" ";
  std::cout << std::endl;
  pretty_scales();
#ifdef __DEBUG__
  if (datasource<T,Tdata>::multimat) {
    EDEBUG("datasource::data : " << (datasource<T,Tdata>::datas.info()));
  } else {
    EDEBUG("class_datasource::data : " << data.info());
  }
  EDEBUG("class_datasource::labels : " << labels.info());
#endif
}

template <typename T, typename Tdata, typename Tlabel>
void class_datasource<T, Tdata, Tlabel>::pretty_scales() {
  labeled_datasource<T,Tdata,Tlabel>::pretty_scales();
  if (this->scales_loaded) {
    intg maxscale = idx_max(this->scales);
    for (uint c = 0; c < this->counts.size(); ++c)
      if (!bexclusion || !excluded[c]) {
        std::vector<intg> tally(maxscale + 1, 0);
        idx_bloop2(scale, this->scales, intg, label, labels, Tlabel) {
          if (label.get() != (Tlabel) c) continue ;
          intg s = scale.get();
          if (s < 0) eblerror("unexpected negative value");
          tally[s] = tally[s] + 1;
        }
        intg nscales = 0;
        for (intg i = 0; i < (intg) tally.size(); ++i)
          if (tally[i] > 0) nscales++;
        std::cout << _name << ": " << *(clblstr[c]) << " has "
                  << nscales << " scales";
        for (intg i = 0; i < (intg) tally.size(); ++i)
          std::cout << ", " << i << ": " << tally[i];
        std::cout << std::endl;
      }
  }
}

// protected methods /////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
bool class_datasource<T,Tdata,Tlabel>::pick_current() {
  if (bexclusion && excluded[(int) get_label()])
    return false;
  return datasource<T,Tdata>::pick_current();
}

//////////////////////////////////////////////////////////////////////////////
//! class_node

template <typename Tlabel>
class_node<Tlabel>::class_node(Tlabel id, std::string &name_)
    : _label(id), _name(name_), parent(NULL), children(),
      it_children(children.begin()),
      bempty(true), iempty(true), _depth(0), it_samples(samples.begin()) {
  //EDEBUG("creating node " << this << " label: " << id << " name: "
  //<< _name);
}

template <typename Tlabel>
class_node<Tlabel>::~class_node() {
}

template <typename Tlabel>
bool class_node<Tlabel>::empty() {
  return bempty;
}

template <typename Tlabel>
bool class_node<Tlabel>::internally_empty() {
  return iempty;
}

template <typename Tlabel>
intg class_node<Tlabel>::next() {
  if (bempty) eblerror("cannot call next() on empty node");
  if (it_children < children.begin()) it_children = children.begin();
  if (it_samples < samples.begin()) it_samples = samples.begin();
  intg id = -1;
  if (it_children == children.end()) { // we reached the end of children
    // roll back children iterator
    it_children = children.begin();
    if (samples.size() > 0) { // return internal samples if present
      id = *it_samples;
      it_samples++;
      if (it_samples == samples.end()) // roll back samples iterator
        it_samples = samples.begin();
      return id;
    }
  }
  // return a sample from children
  if ((*it_children)->empty()) {
    it_children++;
    return next(); // skip current empty child
  } else // this child is not empty
    return (*it_children)->next();
}

template <typename Tlabel>
void class_node<Tlabel>::add_child(class_node *child) {
  // make sure child is not added twice
  for (typename std::vector<class_node<Tlabel>*>::iterator i = children.begin();
       i != children.end(); ++i)
    if (*i == child)
      eblerror("trying to push same child node twice");
  // push
  children.push_back(child);
  // set child's parent to this node
  child->set_parent(this);
  // propagate up if child is non-empty
  if (!child->empty())
    set_non_empty();
}

template <typename Tlabel>
void class_node<Tlabel>::add_sample(intg index) {
  samples.push_back(index);
  iempty = false;
  // propagate information back to parents
  set_non_empty();
}

template <typename Tlabel>
intg class_node<Tlabel>::nsamples() {
  // TODO: we might want to use idx here to handle large sets (intg)
  return samples.size();
}

template <typename Tlabel>
Tlabel class_node<Tlabel>::label() {
  return _label;
}

template <typename Tlabel>
Tlabel class_node<Tlabel>::label(uint depth) {
  // current depth is lower or equal to target depth, return current label
  if (_depth <= depth || !parent)
    return _label;
  // current depth is higher than target depth, call parent's label
  return parent->label(depth);
}

template <typename Tlabel>
uint class_node<Tlabel>::depth() {
  return _depth;
}

template <typename Tlabel>
uint class_node<Tlabel>::set_depth(uint d) {
  _depth = d;
  uint dmax = d;
  for (typename std::vector<class_node<Tlabel>*>::iterator i = children.begin();
       i != children.end(); ++i)
    dmax = std::max(dmax, (*i)->set_depth(d + 1));
  return dmax;
}

template <typename Tlabel>
std::string &class_node<Tlabel>::name() {
  return _name;
}

template <typename Tlabel>
class_node<Tlabel>* class_node<Tlabel>::get_parent() {
  return parent;
}

template <typename Tlabel>
bool class_node<Tlabel>::is_parent(Tlabel lab) {
  if (lab == _label)
    return true;
  if (parent)
    return parent->is_parent(lab);
  return false;
}

// protected methods /////////////////////////////////////////////////////////

template <typename Tlabel>
void class_node<Tlabel>::set_non_empty() {
  bempty = false;
  // propagate information back to parents
  if (parent)
    parent->set_non_empty();
}

template <typename Tlabel>
void class_node<Tlabel>::set_parent(class_node *p) {
  parent = p;
}

////////////////////////////////////////////////////////////////
// hierarchy_datasource

template <typename T, typename Tdata, typename Tlabel>
hierarchy_datasource<T, Tdata, Tlabel>::hierarchy_datasource()
    : class_datasource<T,Tdata,Tlabel>() {
}

template <typename T, typename Tdata, typename Tlabel>
hierarchy_datasource<T, Tdata, Tlabel>::
hierarchy_datasource(midx<Tdata> &data_, idx<Tlabel> &labels_,
                     idx<Tlabel> *parents_,
                     std::vector<std::string*> *lblstr_, const char *name_) {
  class_datasource<T,Tdata,Tlabel>::init(data_, labels_, name_, lblstr_);
  this->init_parents(parents_);
  this->init_epoch();
  this->pretty(); // print info about dataset
}

template <typename T, typename Tdata, typename Tlabel>
hierarchy_datasource<T, Tdata, Tlabel>::
hierarchy_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
                     idx<Tlabel> *parents_,
                     std::vector<std::string*> *lblstr_, const char *name_) {
  class_datasource<T,Tdata,Tlabel>::init(data_, labels_, name_, lblstr_);
  this->init_parents(parents_);
  this->init_epoch();
  this->pretty(); // print info about dataset
}

template <typename T, typename Tdata, typename Tlabel>
hierarchy_datasource<T, Tdata, Tlabel>::
hierarchy_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
                     idx<Tlabel> *parents_, idx<ubyte> *classes,
                     const char *name_) {
  if (classes)
    this->init_strings(*classes);
  class_datasource<T,Tdata,Tlabel>::init(data_, labels_, this->lblstr,
                                         name_);
  this->init_parents(parents_);
  this->init_epoch();
  this->pretty(); // print info about dataset
}

template <typename T, typename Tdata, typename Tlabel>
hierarchy_datasource<T, Tdata, Tlabel>::
hierarchy_datasource(const char *data_name, const char *labels_name,
                     const char *parents_name, const char *jitters_name,
                     const char *scales_name, const char *classes_name,
                     const char *name_, uint max_size) {
  class_datasource<T,Tdata,Tlabel>::
      init(data_name, labels_name, jitters_name, scales_name, classes_name,
           name_, max_size);
  // load parent
  idx<Tlabel> parents_;
  if (parents_name) {
    try {
      parents_ = load_matrix<Tlabel>(parents_name);
    } eblcatcherror();
  }
  // inits
  this->init_parents(parents_name ? &parents_ : NULL);
  this->init_epoch();
  this->pretty(); // print info about dataset
}

// template <typename T, typename Tdata, typename Tlabel>
// hierarchy_datasource<T, Tdata, Tlabel>::
// hierarchy_datasource(const hierarchy_datasource<T, Tdata, Tlabel> &ds)
//   : class_datasource<T,Tdata>((const class_datasource<T,Tdata>&) ds) {
// }

template <typename T, typename Tdata, typename Tlabel>
hierarchy_datasource<T, Tdata, Tlabel>::~hierarchy_datasource() {
  // delete all nodes
  for (typename std::vector<class_node<Tlabel>*>::iterator i = all_nodes.begin();
       i != all_nodes.end(); ++i) {
    class_node<Tlabel> *n = *i;
    if (n) delete n;
  }
  // delete depth vectors
  for (typename std::vector<std::vector<class_node<Tlabel>*>*>::iterator i =
           all_depths.begin(); i != all_depths.end(); ++i) {
    std::vector<class_node<Tlabel>*> *n = *i;
    if (n) delete n;
  }
  if (parents)
    delete parents;
}

//////////////////////////////////////////////////////////////////////////////
// init methods

template <typename T, typename Tdata, typename Tlabel>
void hierarchy_datasource<T, Tdata, Tlabel>::
init_parents(idx<Tlabel> *parents_) {
  if (parents_)
    parents = new idx<Tlabel>(*parents_);
  else {
    eblwarn("no parents hierarchy specified, initializing with a flat "
            << "hierarchy");
    parents = new idx<Tlabel>(nclasses, 2);
    for (Tlabel i = 0; i < (Tlabel) nclasses; ++i) {
      parents->set(i, i, 0);
      parents->set(-1, i, 1); // node i has no parent
    }
  }
  // check that parent id do not exceed number of classes
  if (parents) {
    if (idx_max(*parents) > nclasses)
      eblerror("maximum parent id (" << idx_max(*parents)
               << ") cannot exceed number of classes (" << nclasses << ")");
    if (parents->dim(1) != 2)
      eblerror("expected dim 1 to be size 2 (child/parent)");
  }
  if (!lblstr)
    eblerror("expected class strings to be defined");
  // create hierarchy tree for efficient balanced samples ordering
  all_nodes.resize(nclasses, NULL);
  // add a new node for each class
  Tlabel l = 0;
  for (std::vector<std::string*>::iterator i = lblstr->begin(); i != lblstr->end();++i){
    class_node<Tlabel> *node = all_nodes[l];
    if (!node) {
      node = new class_node<Tlabel>(l, *((*lblstr)[l]));
      all_nodes[l] = node;
    }
    l++;
  }
  // add samples to each node
  intg i = 0;
  idx_bloop1(lab, labels, Tlabel) {
    Tlabel l = lab.get();
    class_node<Tlabel> *node = all_nodes[l];
    if (!node) eblerror("node " << l << " is not defined");
    // add sample to node
    node->add_sample(i);
    i++;
  }
  // associate each class to its parent
  if (parents) {
    idx_bloop1(par, *parents, Tlabel) {
      class_node<Tlabel> *child = all_nodes[par.get(0)];
      if (!child) eblerror("no node with id " << par.get(0));
      if (par.get(1) >= 0) {
        class_node<Tlabel> *parent = all_nodes[par.get(1)];
        parent->add_child(child);
        //EDEBUG("adding "<< child->name() << " as child of " << parent->name());
      }
    }
  }
  // assign depth to all nodes, starting from all orphan nodes
  uint maxdepth = 0;
  for (typename std::vector<class_node<Tlabel>*>::iterator i = all_nodes.begin();
       i != all_nodes.end(); ++i) {
    class_node<Tlabel> *n = *i;
    if (n && !n->get_parent())
      maxdepth = std::max(maxdepth, n->set_depth(0));
  }
  // remember nodes arranged by depth
  all_depths.resize(maxdepth + 1, NULL);
  for (typename std::vector<class_node<Tlabel>*>::iterator i = all_nodes.begin();
       i != all_nodes.end(); ++i) {
    class_node<Tlabel> *n = *i;
    if (n) {
      std::vector<class_node<Tlabel>*> *d = all_depths[n->depth()];
      if (!d) {
        d = new std::vector<class_node<Tlabel>*>;
        all_depths[n->depth()] = d;
      }
      d->push_back(n);
      //EDEBUG("assigning " << n->name() << " to depth " << n->depth());
    }
  }
  // remember nodes arranged by depth and keep nodes with lower depth that
  // have samples internally. this garantees that all samples are used even
  // at lower depths than current one.
  complete_depths.resize(maxdepth + 1, NULL);
  for (typename std::vector<class_node<Tlabel>*>::iterator i = all_nodes.begin();
       i != all_nodes.end(); ++i) {
    class_node<Tlabel> *n = *i;
    if (n) {
      std::vector<class_node<Tlabel>*> *d = complete_depths[n->depth()];
      if (!d) {
        d = new std::vector<class_node<Tlabel>*>;
        complete_depths[n->depth()] = d;
      }
      d->push_back(n);
      // add node to all higher depths if it has internal samples
      if (!n->internally_empty()) {
        for (uint i = n->depth() + 1; i < complete_depths.size(); ++i) {
          std::vector<class_node<Tlabel>*> *dd = complete_depths[i];
          if (!dd) {
            dd = new std::vector<class_node<Tlabel>*>;
            complete_depths[i] = dd;
          }
          dd->push_back(n);
        }
      }
      //EDEBUG("assigning " << n->name() << " to depth " << n->depth());
    }
  }
  // order all by depth
  for (typename std::vector<std::vector<class_node<Tlabel>*>*>::iterator i =
           all_depths.begin(); i != all_depths.end(); ++i) {
    for (typename std::vector<class_node<Tlabel>*>::iterator j = (*i)->begin();
         j != (*i)->end(); ++j) {
      all_nodes_by_depth.push_back(*j);
    }
  }
  // initialize depths iterators
  it_depths.resize(complete_depths.size(), 0);
  // allocate depth labels
  depth_labels = idx<Tlabel>(labels.get_idxdim());
  set_current_depth(0);
  it = 0;
  depth_balance = false;
}

template <typename T, typename Tdata, typename Tlabel>
void hierarchy_datasource<T, Tdata, Tlabel>::init_class_labels() {
  if (olabels.get_idxdim() != labels.get_idxdim())
    olabels = idx<Tlabel>(labels.get_idxdim());
  idx_copy(labels, olabels); // keep original labels
  clblstr.clear();
  for (uint i = 0; i < lblstr->size(); ++i)
    clblstr.push_back((*lblstr)[i]);
}

//////////////////////////////////////////////////////////////////////////////
// data access

template <typename T, typename Tdata, typename Tlabel>
Tlabel hierarchy_datasource<T,Tdata,Tlabel>::get_parent() {
  // idx<Tlabel> lab = labels[it];
  // if (lab.order() != 1 && lab.dim(0) != 1)
  //   eblerror("expected single-element labels");
  // return parents.get(lab.get(0));
  return -1;
}

template <typename T, typename Tdata, typename Tlabel>
bool hierarchy_datasource<T,Tdata,Tlabel>::
is_parent_of(Tlabel l1, Tlabel l2) {
  class_node<Tlabel> *n = all_nodes[l2];
  if (!n) eblerror("node " << l2  << " not found");
  return n->is_parent(l1);
}

template <typename T, typename Tdata, typename Tlabel>
std::vector<class_node<Tlabel>*>& hierarchy_datasource<T,Tdata,Tlabel>::
get_nodes() {
  return all_nodes;
}

template <typename T, typename Tdata, typename Tlabel>
std::vector<class_node<Tlabel>*>&
hierarchy_datasource<T,Tdata,Tlabel>::get_nodes_by_depth() {
  return all_nodes_by_depth;
}

template <typename T, typename Tdata, typename Tlabel>
void hierarchy_datasource<T,Tdata,Tlabel>::
fprop_label(state<Tlabel> &label) {
  label.sset(get_label());
}

template <typename T, typename Tdata, typename Tlabel>
void hierarchy_datasource<T,Tdata,Tlabel>::
fprop_label_net(state<T> &label) {
  label.sset((T) get_label());
}

template <typename T, typename Tdata, typename Tlabel>
Tlabel hierarchy_datasource<T,Tdata,Tlabel>::get_label() {
  // select label based on current depth and current iterator
  Tlabel l = labels[it].gget();
  return all_nodes[l]->label(current_depth);
}

template <typename T, typename Tdata, typename Tlabel>
Tlabel hierarchy_datasource<T,Tdata,Tlabel>::get_label(uint d, intg index){
  // select label based on current depth and current iterator
  Tlabel l;
  if (index < 0)
    l = labels[it].gget();
  else
    l = labels[index].gget();
  return all_nodes[l]->label(d);
}

template <typename T, typename Tdata, typename Tlabel>
idx<Tlabel>& hierarchy_datasource<T,Tdata,Tlabel>::get_depth_labels() {
  return depth_labels;
}

template <typename T, typename Tdata, typename Tlabel>
uint hierarchy_datasource<T,Tdata,Tlabel>::
get_nbrothers(class_node<Tlabel> &n) {
  if (n.parent)
    return n.parent->children.size();
  std::vector<class_node<Tlabel>*>* d0 = all_depths[0];
  if (d0)
    return d0->size();
  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// iterating

template <typename T, typename Tdata, typename Tlabel>
void hierarchy_datasource<T,Tdata,Tlabel>::set_depth_balanced(bool bal) {
  depth_balance = bal;
  if (!depth_balance) // unbalanced
    std::cout << _name << ": Setting training as depth-unbalanced." << std::endl;
  else // balanced
    std::cout << _name << ": Setting training as depth-balanced." << std::endl;
}

template <typename T, typename Tdata, typename Tlabel>
void hierarchy_datasource<T,Tdata,Tlabel>::set_current_depth(uint depth) {
  if (depth >= all_depths.size())
    eblerror("cannot set current depth to " << depth << " because it is "
             << "more than maximum depth " << all_depths.size());
  current_depth = depth;
  // fill depth_labels matrix with labels of all samples given current depth
  idx_bloop2(l, labels, Tlabel, dl, depth_labels, Tlabel) {
    dl.set(all_nodes[l.get()]->label(current_depth));
  }
}

template <typename T, typename Tdata, typename Tlabel>
uint hierarchy_datasource<T,Tdata,Tlabel>::get_current_depth() {
  return current_depth;
}

template <typename T, typename Tdata, typename Tlabel>
void hierarchy_datasource<T,Tdata,Tlabel>::incr_current_depth() {
  if (current_depth + 1 >= all_depths.size())
    std::cout << "warning: cannot increment current depth beyond maximum ("
              << all_depths.size() << ")" << std::endl;
  else
    current_depth++;
  set_current_depth(current_depth);
}

template <typename T, typename Tdata, typename Tlabel>
bool hierarchy_datasource<T,Tdata,Tlabel>::next_train() {
  // check that this datasource is allowed to call this method
  if (test_set)
    eblerror("forbidden call of next_train() on testing sets");
  if (!depth_balance) // do not balance by depth
    return class_datasource<T,Tdata,Tlabel>::next_train();
  // balanced training
  std::vector<class_node<Tlabel>*> *nodes = complete_depths[current_depth];
  uint itd = it_depths[current_depth];
  // set it for further fprop calls
  class_node<Tlabel> *node = (*nodes)[itd];
  it = node->next();
  // increment depth iterator
  itd++;
  it_depths[current_depth] = itd;
  if (itd >= nodes->size())
    it_depths[current_depth] = 0;
  // increment epoch counters
  epoch_cnt++;
  epoch_pick_cnt++;
#ifdef __DEBUG__
  std::cout << "Picking sample " << it << " (label: " << (int)get_label();
  if (lblstr)
    std::cout << ", name: " << *((*lblstr)[(int)get_label()]);
  std::cout << ", pickings: " << pick_count.get(it) << ", energy: "
            << energies.get(it) << ", correct: " << (int) correct.get(it);
  if (weigh_samples) std::cout << ", proba: " << probas.get(it);
  std::cout << ")" << std::endl;
#endif
  this->pretty_progress();
  return true;
}

//   template <typename T, typename Tdata, typename Tlabel>
//   void hierarchy_datasource<T,Tdata,Tlabel>::set_balanced(bool bal) {
//     balance = bal;
//     if (!balance) // unbalanced
//       std::cout << _name << ": Setting training as unbalanced (not taking class "
// 		 << "distributions into account)." << std::endl;
//     else { // balanced
//       std::cout << _name << ": Setting training as balanced (taking class "
// 		 << "distributions into account)." << std::endl;
//       // compute vector of sample indices for each class
//       bal_indices.clear();
//       bal_it.clear();
//       epoch_done_counters.clear();
//       class_it = 0;
//       for (intg i = 0; i < nclasses; ++i) {
// 	std::vector<intg> indices;
// 	bal_indices.push_back(indices);
// 	bal_it.push_back(0); // init iterators
//       }
//       // distribute sample indices into each vector based on label
//       for (uint i = 0; i < this->size(); ++i)
// 	bal_indices[(intg) (labels.gget(i))].push_back(i);
//       for (uint i = 0; i < bal_indices.size(); ++i) {
// 	// shuffle
// 	random_shuffle(bal_indices[i].begin(), bal_indices[i].end());
// 	// init epoch counters
// 	epoch_done_counters.push_back(bal_indices[i].size());
//       }
//     }
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   bool hierarchy_datasource<T,Tdata,Tlabel>::epoch_done() {
//     switch (epoch_mode) {
//     case 0: // fixed number of samples
//       if (epoch_cnt >= epoch_sz)
// 	return true;
//       break ;
//     case 1: // see all samples at least once
//       if (balance) {
// 	// check that all classes are done
// 	for (uint i = 0; i < epoch_done_counters.size(); ++i) {
// 		if (epoch_done_counters[i] > 0)
// 			return false;
// 	}
// 	return true; // all classes are done
//       } else { // do not balance, use epoch_sz
// 	if (epoch_cnt >= epoch_sz)
// 		return true;
//       }
//       break ;
//     default: eblerror("unknown epoch_mode");
//     }
//     return false;
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   void hierarchy_datasource<T,Tdata,Tlabel>::init_epoch() {
//     epoch_cnt = 0;
//     epoch_pick_cnt = 0;
//     epoch_timer.restart();
//     epoch_show_printed = -1; // last epoch count we have printed
//     if (balance) {
//       uint maxsize = 0;
//       // for balanced training, set each class to not done.
//       for (uint k = 0; k < bal_indices.size(); ++k) {
// 	epoch_done_counters[k] = bal_indices[k].size();
// 	if (bal_indices[k].size() > maxsize)
// 		maxsize = bal_indices[k].size();
//       }
//       if (epoch_mode == 1) // for ETA estimation only, estimate epoch size
// 	epoch_sz = maxsize * bal_indices.size();
//     }
//     // if we have prior information about each sample energy and classification
//     // let's use it to initialize the picking probabilities.
//     this->normalize_all_probas();
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   void hierarchy_datasource<T,Tdata,Tlabel>::normalize_all_probas() {
//     if (weigh_samples) {
//       if (perclass_norm && balance) {
// 	for (uint i = 0; i < bal_indices.size(); ++i)
// 		normalize_probas(i);
//       } else
// 	normalize_probas();
//     }
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   void hierarchy_datasource<T,Tdata,Tlabel>::normalize_probas(int classid) {
//     std::vector<intg> *cindices = NULL;
//     if (perclass_norm && balance) { // use only class_it class samples
//       if (classid < 0)
// 	eblerror("class id cannot be negative");
//       uint class_it = (uint) classid;
//       cindices = &(bal_indices[class_it]);
//       std::cout << _name << ": Normalizing probabilities for class" << class_it;
//       datasource<T,Tdata>::normalize_probas(cindices);
//     } else // use all samples
//       datasource<T,Tdata>::normalize_probas();
//   }

//   //////////////////////////////////////////////////////////////////////////////
//   // accessors

//   template <typename T, typename Tdata, typename Tlabel>
//   intg hierarchy_datasource<T, Tdata, Tlabel>::get_nclasses() {
//     return nclasses;
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   int hierarchy_datasource<T, Tdata, Tlabel>::get_class_id(const char *name) {
//     int id_ = -1;
//     std::vector<std::string*>::iterator i = lblstr->begin();
//     for (int j = 0; i != lblstr->end(); ++i, ++j) {
//       if (!strcmp(name, (*i)->c_str()))
// 	id_ = j;
//     }
//     return id_;
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   std::string &hierarchy_datasource<T, Tdata, Tlabel>::get_class_name(int id) {
//     if (!lblstr)
//       eblerror("no label strings");
//     if (id >= (int) lblstr->size())
//       eblerror("requesting label string at index " << id
// 				 << " but string vector has only " << lblstr->size()
// 				 << " elements.");
//     std::string *s = (*lblstr)[id];
//     if (!s)
//       eblerror("empty label string");
//     return *s;
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   std::vector<std::string*>& hierarchy_datasource<T, Tdata, Tlabel>::
//   get_label_strings() {
//     if (!lblstr)
//       eblerror("expected label strings to be set");
//     return *lblstr;
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   intg hierarchy_datasource<T,Tdata,Tlabel>::get_lowest_common_size() {
//     intg min_nonzero = (std::numeric_limits<intg>::max)();
//     for (std::vector<intg>::iterator i = counts.begin(); i != counts.end(); ++i) {
//       if ((*i < min_nonzero) && (*i != 0))
// 	min_nonzero = *i;
//     }
//     if (min_nonzero == (std::numeric_limits<intg>::max)())
//       eblerror("empty dataset");
//     return min_nonzero * nclasses;
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   void hierarchy_datasource<T,Tdata,Tlabel>::save_pickings(const char *name_) {
//     // non-class plotting
//     datasource<T,Tdata>::save_pickings(name_);
//     std::string name = "pickings";
//     if (name_)
//       name = name_;
//     // plot by class
//     write_classed_pickings(pick_count, correct, name);
//     write_classed_pickings(energies, correct, name, "_energies");
//     idx<double> e = idx_copy(energies);
//     idx<ubyte> c = idx_copy(correct);
//     idx_sortup(e, c);
//     write_classed_pickings(e, c, name, "_sorted_energies");
//     idx<double> p = idx_copy(probas);
//     c = idx_copy(correct);
//     idx_sortup(p, c);
//     write_classed_pickings(p, c, name, "_sorted_probas");
//     p = idx_copy(probas);
//     e = idx_copy(energies);
//     c = idx_copy(correct);
//     idx_sortup(e, c, p);
//     write_classed_pickings(p, c, name, "_probas_sorted_by_energy", true,
// 				 "Picking probability");
//     write_classed_pickings(p, c, name, "_probas_sorted_by_energy_wrong_only",
// 				 false, "Picking probability");
//     write_classed_pickings(e, c, name, "_energies_sorted_by_energy_wrong_only",
// 				 false, "Energy");
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   template <typename T>
//   void hierarchy_datasource<T,Tdata,Tlabel>::
//   write_classed_pickings(idx<T> &m, idx<ubyte> &c, std::string &name_,
// 			 const char *name2_, bool plot_correct,
// 			 const char *ylabel) {
//     std::string name = name_;
//     if (name2_)
//       name += name2_;
//     name += "_classed";
//     // sorted classed plot file
//     if (labels.order() == 1) { // single label value
//       std::string fname = name;
//       fname += ".plot";
//       std::ofstream fp(fname.c_str());
//       if (!fp) {
// 	std::cerr << "failed to open " << fname << std::endl;
// 	eblerror("failed to open file for writing");
//       }
//       typename idx<T>::dimension_iterator i = m.dim_begin(0);
//       typename idx<Tlabel>::dimension_iterator l = labels.dim_begin(0);
//       typename idx<ubyte>::dimension_iterator ic = c.dim_begin(0);
//       uint j = 0;
//       for ( ; i.notdone(); i++, l++, ic++) {
// 	if (!plot_correct && ic->get() == 1)
// 		continue ; // don't plot correct ones
// 	fp << j++;
// 	for (Tlabel k = 0; k < (Tlabel) nclasses; ++k) {
// 		if (k == l.get()) {
// 			if (ic->get() == 0) { // wrong
// 				fp << "\t" << i->get();
// 				if (plot_correct)
// 		fp << "\t?";
// 			} else if (plot_correct) // correct
// 				fp << "\t?\t" << i->get();
// 		} else {
// 			fp << "\t?";
// 			if (plot_correct)
// 				fp << "\t?";
// 		}
// 	}
// 	fp << std::endl;
//       }
//       fp.close();
//       std::cout << _name << ": Wrote picking statistics in " << fname << std::endl;
//       // p file
//       std::string fname2 = name;
//       fname2 += ".p";
//       std::ofstream fp2(fname2.c_str());
//       if (!fp2) {
// 	std::cerr << "failed to open " << fname2 << std::endl;
// 	eblerror("failed to open file for writing");
//       }
//       fp2 << "set title \"" << name << "\"; set ylabel \"" << ylabel
// 		<< "\"; plot \""
// 		<< fname << "\" using 1:2 title \"class 0 wrong\" with impulse";
//       if (plot_correct)
// 	fp2 << ", \""
// 			<< fname << "\" using 1:3 title \"class 0 correct\" with impulse";
//       for (uint k = 1; k < nclasses; ++k) {
// 	fp2 << ", \"" << fname << "\" using 1:" << k * (plot_correct?2:1) + 2
// 			<< " title \"class " << k << " wrong\" with impulse";
// 	if (plot_correct)
// 		fp2 << ", \"" << fname << "\" using 1:" << k * 2 + 3
// 				<< " title \"class " << k << " correct\" with impulse";
//       }
//       fp << std::endl;
//       fp2.close();
//       std::cout << _name << ": Wrote gnuplot file in " << fname2 << std::endl;
//     }
//   }

//   //////////////////////////////////////////////////////////////////////////////
//   // state saving

//   template <typename T, typename Tdata, typename Tlabel>
//   void hierarchy_datasource<T,Tdata,Tlabel>::save_state() {
//     state_saved = true;
//     count_pickings_save = count_pickings;
//     it_saved = it; // save main iterator
//     it_test_saved = it_test;
//     it_train_saved = it_train;
//     if (!balance) // save (unbalanced) iterators
//       datasource<T,Tdata>::save_state();
//     else { // save balanced iterators
//       bal_it_saved.clear();
//       bal_indices_saved.clear();
//       for (uint k = 0; k < bal_it.size(); ++k) {
// 	bal_it_saved.push_back(bal_it[k]);
// 	std::vector<intg> indices;
// 	for (uint l = 0; l < bal_indices[k].size(); ++l)
// 		indices.push_back(bal_indices[k][l]);
// 	bal_indices_saved.push_back(indices);
//       }
//       class_it_saved = class_it;
//     }
//   }

//   template <typename T, typename Tdata, typename Tlabel>
//   void hierarchy_datasource<T,Tdata,Tlabel>::restore_state() {
//     if (!state_saved)
//       eblerror("state not saved, call save_state() before restore_state()");
//     count_pickings = count_pickings_save;
//     it = it_saved; // restore main iterator
//     it_test = it_test_saved;
//     it_train = it_train_saved;
//     if (!balance) // restore unbalanced
//       datasource<T,Tdata>::restore_state();
//     else { // restore balanced iterators
//       for (uint k = 0; k < bal_it.size(); ++k) {
// 	bal_it[k] = bal_it_saved[k];
// 	for (uint l = 0; l < bal_indices[k].size(); ++l)
// 		bal_indices[k][l] = bal_indices_saved[k][l];
//       }
//       class_it = class_it_saved;
//     }
//   }

//////////////////////////////////////////////////////////////////////////////
// pretty methods

// template <typename T, typename Tdata, typename Tlabel>
// void hierarchy_datasource<T,Tdata,Tlabel>::pretty_progress(bool newline) {
//   if (epoch_show > 0 && epoch_pick_cnt % epoch_show == 0 &&
// 	epoch_show_printed != (intg) epoch_pick_cnt) {
//     datasource<T,Tdata>::pretty_progress(false);
//     if (balance) {
// 	std::cout << ", remaining:";
// 	for (uint i = 0; i < epoch_done_counters.size(); ++i) {
// 		std::cout << " " << i << ": " << epoch_done_counters[i];
// 	}
//     }
//     if (newline)
// 	std::cout << std::endl;
//   }
// }

template <typename T, typename Tdata, typename Tlabel>
void hierarchy_datasource<T, Tdata, Tlabel>::pretty() {
  std::cout << _name << ": hierarchy class dataset \"" << _name
            << "\" contains "
            << data.dim(0) << " samples of dimension " << sampledims
            << " and defines an epoch as " << epoch_sz << " samples." << std::endl;
  if (lblstr) {
    std::cout << this->_name << ": It has " << nclasses << " classes: ";
    uint i;
    for (i = 0; i < this->counts.size() - 1; ++i)
      std::cout << this->counts[i] << " \"" << *(*lblstr)[i] << "\", ";
    std::cout << "and " << this->counts[i] << " \"" << *(*lblstr)[i] << "\".";
    std::cout << std::endl;
    // pretty hierarchy
    std::cout << "Hierarchy by depth:" << std::endl;
    for (typename std::vector<std::vector<class_node<Tlabel>*>*>::iterator
             j = all_depths.begin(); j != all_depths.end(); ++j) {
      std::vector<class_node<Tlabel>*> &d = **j;
      std::cout << "depth " << j - all_depths.begin() << " ("
                << d.size() << " nodes): ";
      for (typename std::vector<class_node<Tlabel>*>::iterator k = d.begin();
           k != d.end(); ++k)
        std::cout << (*k)->name() << " ";
      std::cout << std::endl;
    }
    // pretty complete-depth hierarchy
    std::cout << "Hierarchy by depth, keeping lower depth nodes with samples:"
              << std::endl;
    for (typename std::vector<std::vector<class_node<Tlabel>*>*>::iterator
             j = complete_depths.begin(); j != complete_depths.end(); ++j) {
      std::vector<class_node<Tlabel>*> &d = **j;
      std::cout << "depth " << j - complete_depths.begin() << " ("
                << d.size() << " nodes): ";
      for (typename std::vector<class_node<Tlabel>*>::iterator k = d.begin();
           k != d.end(); ++k)
        std::cout << (*k)->name() << " ";
      std::cout << std::endl;
    }
  }
}

template <typename T, typename Tdata, typename Tlabel>
void hierarchy_datasource<T, Tdata, Tlabel>::print_path(Tlabel l) {
  bool first = true;
  for (class_node<Tlabel> *n = all_nodes[l]; n != NULL; n = n->get_parent()) {
    if (first)
      first = false;
    else
      std::cout << " <- ";
    std::cout << n->name();
  }
}

////////////////////////////////////////////////////////////////
// labeled_pair_datasource

// constructor
template <typename T, typename Tdata, typename Tlabel>
labeled_pair_datasource<T, Tdata, Tlabel>::
labeled_pair_datasource(const char *data_fname, const char *labels_fname,
                        const char *classes_fname, const char *pairs_fname,
                        const char *name_, Tdata b, float c)
    : labeled_datasource<T, Tdata, Tlabel>(data_fname, labels_fname,
                                           classes_fname, name_, b, c),
      pairs(1, 1) { //, pairsIter(pairs, 0) {
  // init current class
  try {
    pairs = load_matrix<intg>(pairs_fname);
  } eblcatcherror_msg("failed to load dataset file " << pairs_fname);
  eblerror("not implemented");
  //    typename idx<intg>::dimension_iterator	 diter(pairs, 0);
  // pairsIter = diter;
}

// constructor
template <typename T, typename Tdata, typename Tlabel>
labeled_pair_datasource<T, Tdata, Tlabel>::
labeled_pair_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
                        idx<ubyte> &classes_, idx<intg> &pairs_,
                        const char *name_, Tdata b, float c)
    : labeled_datasource<T, Tdata, Tlabel>(data_, labels_, classes_, name_,
                                           b, c),
      pairs(pairs_) { //, pairsIter(pairs, 0) {
}

// destructor
template <typename T, typename Tdata, typename Tlabel>
labeled_pair_datasource<T, Tdata, Tlabel>::~labeled_pair_datasource() {
}

// fprop pair
template <typename T, typename Tdata, typename Tlabel>
void labeled_pair_datasource<T, Tdata, Tlabel>::
fprop(state<T> &in1, state<T> &in2, state<Tlabel> &label) {
  eblerror("fixme");
  // in1.resize(this->sample_dims());
  // in2.resize(this->sample_dims());
  // intg id1 = pairsIter.get(0), id2 = pairsIter.get(1);
  // Tlabel lab = this->labels.get(id1);
  // label.x.set(lab);
  // idx<Tdata> im1 = this->data[id1], im2 = this->data[id2];
  // idx_copy(im1, in1.x);
  // idx_copy(im2, in2.x);
  // idx_addc(in1.x, this->bias, in1.x);
  // idx_dotc(in1.x, this->coeff, in1.x);
  // idx_addc(in2.x, this->bias, in2.x);
  // idx_dotc(in2.x, this->coeff, in2.x);
}

// next pair
template <typename T, typename Tdata, typename Tlabel>
bool labeled_pair_datasource<T, Tdata, Tlabel>::next() {
  eblerror("not implemented");
  // ++pairsIter;
  // if(!pairsIter.notdone()) {
  //   pairsIter = pairs.dim_begin(0);
  //   return false;
  // }
  return true;
}

// begin pair
template <typename T, typename Tdata, typename Tlabel>
void labeled_pair_datasource<T, Tdata, Tlabel>::seek_begin() {
  eblerror("not implemented");
  // pairsIter = pairs.dim_begin(0);
}

template <typename T, typename Tdata, typename Tlabel>
unsigned int labeled_pair_datasource<T, Tdata, Tlabel>::size() {
  return pairs.dim(0);
}

// mnist_datasource ////////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
mnist_datasource<T, Tdata, Tlabel>::
mnist_datasource(const char *root, bool train_data, uint size) {
  try {
    // load dataset
    std::string datafile, labelfile, name, setname = "MNIST";
    if (train_data) // training set
      setname = "train";
    else // testing set
      setname = "t10k";
    datafile << root << "/" << setname << "-images-idx3-ubyte";
    labelfile << root << "/" << setname << "-labels-idx1-ubyte";
    name << "MNIST " << setname;
    idx<Tdata> dat = load_matrix<Tdata>(datafile);
    idx<Tlabel> labs = load_matrix<Tlabel>(labelfile);
    dat = dat.narrow(0, MIN(dat.dim(0), (intg) size), 0);
    labs = labs.narrow(0, MIN(labs.dim(0), (intg) size), 0);
    mnist_datasource<T,Tdata,Tlabel>::init(dat, labs, name.c_str());
    if (!train_data)
      this->set_test(); // remember that this is the testing set
    this->init_epoch();
    this->pretty(); // print info about dataset
  } eblcatcherror_msg("failed to load mnist dataset: ");
}

template <typename T, typename Tdata, typename Tlabel>
mnist_datasource<T, Tdata, Tlabel>::
mnist_datasource(const char *root, const char *name, uint size) {
  try {
    // load dataset
    std::string datafile, labelfile;
    datafile << root << "/" << name << "_" << DATA_NAME << MATRIX_EXTENSION;
    labelfile << root << "/" << name
              << "_" << LABELS_NAME << MATRIX_EXTENSION;
    idx<Tdata> dat = load_matrix<Tdata>(datafile);
    idx<Tlabel> labs = load_matrix<Tlabel>(labelfile);
    dat = dat.narrow(0, MIN((uint) dat.dim(0), size), 0);
    labs = labs.narrow(0, MIN((uint) labs.dim(0), size), 0);
    mnist_datasource<T,Tdata,Tlabel>::init(dat, labs, name);
    this->init_epoch();
    this->pretty(); // print info about dataset
  } eblcatcherror_msg("failed to load mnist dataset");
}

template <typename T, typename Tdata, typename Tlabel>
mnist_datasource<T, Tdata, Tlabel>::~mnist_datasource() {
}

template <typename T, typename Tdata, typename Tlabel>
void mnist_datasource<T, Tdata, Tlabel>::fprop1_data(idx<T> &out) {
  // get sample
  idx<Tdata> dat;
  if (multimat) dat = datas.mget(it);
  else dat = data[it];
  // resize if necessary
  if (out.order() != sampledims.order()) out = idx<T>(sampledims);
  else out.resize(this->sample_dims());
  // pad
  uint ni = data.dim(1);
  uint nj = data.dim(2);
  uint di = (uint) (0.5 * (height - ni));
  uint dj = (uint) (0.5 * (width - nj));
  idx_clear(out);
  idx<T> tgt = out.select(0, 0);
  tgt = tgt.narrow(0, ni, di);
  tgt = tgt.narrow(1, nj, dj);
  idx_copy(dat, tgt);
  // bias and coeff
  if (bias != 0) idx_addc(out, bias, out);
  if (coeff != 1) idx_dotc(out, coeff, out);
}

template <typename T, typename Tdata, typename Tlabel>
void mnist_datasource<T, Tdata, Tlabel>::
init(idx<Tdata> &data_, idx<Tlabel> &labels_, const char *name_) {
  class_datasource<T, Tdata, Tlabel>::init(data_, labels_, NULL, name_);
  this->set_data_coeff(.01); // scale input data from [0,255] to [0,2.55]
  // mnist is actually 28x28, but we add some padding
  sampledims = idxdim(1, 32, 32);
  height = sampledims.dim(1);
  width = sampledims.dim(2);
}

////////////////////////////////////////////////////////////////

template <typename Tdata>
idx<Tdata> create_target_matrix(intg nclasses, Tdata target) {
  // fill matrix with 1-of-n code
  idx<Tdata> targets(nclasses, nclasses);
  idx_fill(targets, -target);
  for (int i = 0; i < nclasses; ++i) targets.set(target, i, i);
  return targets; // return by copy
}

} // end namespace ebl

#endif /*DATASOURCE_HPP_*/
