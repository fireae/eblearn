/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#include <typeinfo>
#include "utils.h"

namespace ebl {

// supervised_trainer //////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
supervised_trainer<T, Tdata, Tlabel>::
supervised_trainer(trainable_module<T,Tdata,Tlabel> &m, ddparameter<T> &p)
    : machine(m), param(p), energy(), answers(NULL), label(NULL), age(0),
      iteration(-1), iteration_ptr(NULL), prettied(false), progress_cnt(0),
      test_running(false), test_display_modulo(0) {
  energy.resize_dx();
  energy.resize_ddx();
  energy.dx[0].set(1.0); // d(E)/dE is always 1
  energy.ddx[0].set(0.0); // dd(E)/dE is always 0
  eblprint( "Training with: " << m.describe() << std::endl);
}

template <typename T, typename Tdata, typename Tlabel>
supervised_trainer<T, Tdata, Tlabel>::~supervised_trainer() {
}

// per-sample methods ////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
bool supervised_trainer<T, Tdata, Tlabel>::
test_sample(labeled_datasource<T,Tdata,Tlabel> &ds,
            state<T> &label, state<T> &answers, infer_param &infp) {
  machine.compute_answers(answers);
  return machine.correct(answers, label);
}

template <typename T, typename Tdata, typename Tlabel>
T supervised_trainer<T, Tdata, Tlabel>::
train_sample(labeled_datasource<T,Tdata,Tlabel> &ds, gd_param &args) {
  TIMING2("until train_sample");
  machine.fprop(ds, energy);
  param.zero_dx();
  machine.bprop(ds, energy);
  param.update(args);
  TIMING2("entire train_sample");
  return energy.get();
}

template <typename T, typename Tdata, typename Tlabel>
T supervised_trainer<T, Tdata, Tlabel>::
train_sample(state<T> &sample, const Tlabel lab, gd_param &args) {
  idx<T> target = machine.get_target(lab);
  if (!label || label->get_idxdim() != target.get_idxdim()) {
    delete label;
    label = new state<T>(target.get_idxdim());
  }
  EDEBUG("machine: " << machine.describe());
  EDEBUG_MAT("training sample ", sample);
  EDEBUG("training with label " << lab << " target: " << target.str());

  machine.fprop(sample, *label, energy);
  param.zero_ddx();
  machine.bprop(sample, *label, energy);
  param.update(args);
  return energy.get();
}

// epoch methods /////////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
void supervised_trainer<T, Tdata, Tlabel>::
test(labeled_datasource<T, Tdata, Tlabel> &ds, classifier_meter &log,
     infer_param &infp, uint max_test) {
  init(ds, &log, true);
  idx<T> target;
  uint ntest = ds.size();
  if (max_test > 0) { // limit the number of tests
    ntest = std::min(ntest, max_test);
    eblprint( "Limiting the number of tested samples to " << ntest
              << std::endl);
  }
  // TODO: simplify this
  std::vector<std::string*> lblstr;
  class_datasource<T,Tdata,Tlabel> *cds =
      dynamic_cast<class_datasource<T,Tdata,Tlabel>*>(&ds);
  if (cds) lblstr = cds->get_label_strings();

  // loop
  uint i = 0;
  do {
    TIMING2("until beginning of sample test");
    ds.fprop_label_net(*label);
    machine.fprop(ds, energy);
    bool correct = test_sample(ds, *label, *answers, infp);
    target = machine.compute_targets(ds);
    machine.update_log(log, age, energy, *answers, *label, target,
                       machine.out1);
    // use energy as distance for samples probabilities to be used
    ds.set_sample_energy((double) energy.get(), correct, machine.out1,
                         *answers, target);
    ds.pretty_progress();
    if (test_display_modulo > 0 && i % test_display_modulo == 0)
      log.display(iteration, ds.name(), &lblstr, ds.is_test());
    update_progress(); // tell the outside world we're still running
    TIMING2("sample test (" << machine.msin1 << ")");
  } while (ds.next() && i++ < ntest);
  ds.normalize_all_probas();
  log.display(iteration, ds.name(), &lblstr, ds.is_test());
  eblprint( std::endl);
}

template <typename T, typename Tdata, typename Tlabel>
void supervised_trainer<T, Tdata, Tlabel>::
train(labeled_datasource<T, Tdata, Tlabel> &ds, classifier_meter &log,
      gd_param &gdp, int niter, infer_param &infp,
      intg hessian_period, intg nhessian, double mu) {
  eblprint( "training on " << niter * ds.get_epoch_size());
  if (nhessian == 0) {
    eblprint( " samples and disabling 2nd order derivative calculation" << std::endl);
    param.set_epsilon(1.0);
  } else {
    eblprint( " samples and recomputing 2nd order "
              << "derivatives on " << nhessian << " samples after every "
              << hessian_period << " trained samples..." << std::endl);
  }
  timer t;
  init(ds, &log);
  bool selected = true, correct;
  idx<T> target;
  for (int i = 0; i < niter; ++i) { // niter iterations
    t.start();
    ds.init_epoch();
    // training on lowest size common to all classes (times # classes)
    while (!ds.epoch_done()) {
      // recompute 2nd order derivatives
      if (hessian_period > 0 && nhessian > 0 &&
          ds.get_epoch_count() % hessian_period == 0)
        compute_diaghessian(ds, nhessian, mu);
      // get label
      ds.fprop_label_net(*label);
      if (selected) // selected for training
        train_sample(ds, gdp);
      // test if answer is correct
      correct = test_sample(ds, *label, *answers, infp);
      machine.update_log(log, age, energy, *answers, *label, target,
                         machine.out1);
      // use energy and answer as distance for samples probabilities
      target = machine.compute_targets(ds);
      ds.set_sample_energy((double) energy.get(), correct, machine.out1,
                           *answers, target);
      //      log.update(age, output, label.get(), energy);
      age++;
      // select next sample
      selected = ds.next_train();
      ds.pretty_progress();
      // decrease learning rate if specified
      if (gdp.anneal_period > 0 && ((age - 1) % gdp.anneal_period) == 0) {
        gdp.eta = gdp.eta /
	    (1 + ((age / gdp.anneal_period) * gdp.anneal_value));
        eblprint( "age: " << age << " updated eta=" << gdp.eta << std::endl);
      }
      update_progress(); // tell the outside world we're still running
    }
    ds.normalize_all_probas();
    eblprint( "epoch_count=" << ds.get_epoch_count() << std::endl);
    eblprint( "training_time="; t.pretty_elapsed());
    eblprint( std::endl);
    // report accuracy on trained sample
    if (test_running) {
      eblprint( "Training running test:" << std::endl);
      // TODO: simplify this
      class_datasource<T,Tdata,Tlabel> *cds =
	  dynamic_cast<class_datasource<T,Tdata,Tlabel>*>(&ds);
      log.display(iteration, ds.name(), cds ? cds->lblstr : NULL,
                  ds.is_test());
      eblprint( std::endl);
    }
  }
}

template <typename T, typename Tdata, typename Tlabel>
void supervised_trainer<T,Tdata,Tlabel>::
compute_diaghessian(labeled_datasource<T,Tdata,Tlabel> &ds, intg niter,
                    double mu) {
  eblprint( "computing 2nd order derivatives on " << niter << " samples..."
            << std::endl << "parameters: " << param.info() << std::endl);
  timer t;
  t.start();
  //     init(ds, NULL);
  //     ds.init_epoch();
  ds.save_state(); // save current ds state
  ds.set_count_pickings(false); // do not counts those samples in training
  param.clear_ddeltax();
  // loop
  for (int i = 0; i < niter; ++i) {
    machine.fprop(ds, energy);
    param.zero_dx();
    machine.bprop(ds, energy);
    param.zero_ddx();
    machine.bbprop(ds, energy);
    EDEBUG("param: " << param.info());
    param.update_ddeltax((1 / (double) niter), 1.0);
    EDEBUG("param: " << param.info());
    EDEBUG_MAT("param.ddeltax:", param.ddeltax);
    while (!ds.next_train()) ; // skipping all non selected samples
    ds.pretty_progress();
    update_progress(); // tell the outside world we're still running
  }
  ds.restore_state(); // set ds state back
  param.compute_epsilons(mu);
  eblprint( "diaghessian inf: " << idx_min(param.epsilons));
  eblprint( " sup: " << idx_max(param.epsilons));
  eblprint( " diaghessian_minutes=" << t.elapsed_minutes() << std::endl);
}

// accessors /////////////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
void supervised_trainer<T, Tdata, Tlabel>::set_iteration(int i) {
  eblprint( "Setting iteration id to " << i << std::endl);
  iteration = i;
}

template <typename T, typename Tdata, typename Tlabel>
void supervised_trainer<T, Tdata, Tlabel>::set_test_display_modulo(intg mod) {
  test_display_modulo = mod;
}

template <typename T, typename Tdata, typename Tlabel>
void supervised_trainer<T, Tdata, Tlabel>::
pretty(labeled_datasource<T, Tdata, Tlabel> &ds) {
  if (!prettied) {
    // pretty sizes of input/output for each module the first time
    mfidxdim d(ds.sample_mfdims());
    eblprint( "machine sizes: " << d << machine.mod1.pretty(d) << std::endl
              << "trainable parameters: " << param << std::endl);
    prettied = true;
  }
}

template <typename T, typename Tdata, typename Tlabel>
void supervised_trainer<T, Tdata, Tlabel>::
set_progress_file(const std::string &f) {
  progress_file = f;
  eblprint( "Setting progress file to \"" << f << "\"" << std::endl);
}

template <typename T, typename Tdata, typename Tlabel>
void supervised_trainer<T, Tdata, Tlabel>::update_progress() {
  progress_cnt++;
#if __WINDOWS__ !=1
  // tell the outside world we are still running every 20 samples
  if (!progress_file.empty() && progress_cnt % 20 == 0)
    touch_file(progress_file);
#endif
}

// internal methods //////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
void supervised_trainer<T, Tdata, Tlabel>::
init(labeled_datasource<T, Tdata, Tlabel> &ds,
     classifier_meter *log, bool new_iteration) {
  pretty(ds); // pretty info
  // if not allocated, allocate answers. answers are allocated dynamically
  // based on ds dimensions because fstate_idx cannot change orders.
  idxdim d = ds.sample_dims();
  d.setdims(1);
  if (answers)
    delete answers;
  answers = new state<T>(d);
  //
  idxdim dl = ds.label_dims();
  if (label)
    delete label;
  label = new state<T>(dl);
  // reinit ds
  ds.seek_begin();
  if (log) { // reinit logger
    class_datasource<T,Tdata,Tlabel> *cds =
	dynamic_cast<class_datasource<T,Tdata,Tlabel>* >(&ds);
    log->init(cds ? cds->get_nclasses() : 0);
  }
  // new iteration
  if (new_iteration) {
    if (!iteration_ptr)
      iteration_ptr = (void *) &ds;
    if (iteration_ptr == (void *) &ds)
      ++iteration;
  }
}

} // end namespace ebl
