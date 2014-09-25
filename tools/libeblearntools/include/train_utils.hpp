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

#ifndef TRAIN_UTILS_HPP_
#define TRAIN_UTILS_HPP_

#include "netconf.h"

namespace ebl {

template <typename T, typename Tdata, typename Tlabel>
supervised_trainer<T,Tdata,Tlabel>*
create_trainable_network(ddparameter<T> &theparam, configuration &conf,
                         uint noutputs, module_1_1<T> **network, uint &iter) {
  answer_module<T,Tdata,Tlabel> *answer =
      create_answer<T,Tdata,Tlabel>(conf, noutputs);
  if (!answer) eblerror("no answer module found");
  std::cout << "Answering module: " << answer->describe() << std::endl;
  // update number of outputs given the answer module
  noutputs = answer->get_nfeatures();
  intg inthick = conf.try_get_int("input_thickness", -1);
  //! create the network weights, network and trainer
  module_1_1<T> *net = create_network<T>(theparam, conf, inthick, noutputs,
					 "arch");
  *network = net;
  if (!net) eblerror("failed to create network");
  if (((layers<T>*)net)->size() == 0) eblerror("0 modules in network");
  trainable_module<T,Tdata,Tlabel> *train =
      create_trainer<T,Tdata,Tlabel>(conf, *net, *answer);
  supervised_trainer<T,Tdata,Tlabel> *thetrainer =
      new supervised_trainer<T,Tdata,Tlabel>(*train, theparam);
  thetrainer->set_progress_file(job::get_progress_filename());
  // initialize the network weights
  bool fixed_random = conf.try_get_bool("fixed_randomization", false);
  forget_param_linear fgp(1, 0.5, !fixed_random);
  if (!fixed_random) fgp.seed(conf.str());
  iter = 0;
  if (conf.exists_true("retrain")) {
    if (!conf.exists("retrain_weights"))
      eblerror("retrain_weights variable not defined");
    // concatenate weights if multiple ones
    std::vector<std::string> w =
	string_to_stringvector(conf.get_string("retrain_weights"));
    theparam.load_x(w);
    if (conf.exists("retrain_iteration")) {
      iter = std::max(0, conf.get_int("retrain_iteration") - 1);
      thetrainer->set_iteration(iter - 1);
    }
  } else {
    std::cout << "Initializing weights from random." << std::endl;
    train->forget(fgp);
  }
  if (!conf.exists_true("retrain") && conf.exists_true("manual_load"))
    manually_load_network(*((layers<T>*)net), conf);
  return thetrainer;
}

// testing and saving //////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
void test_and_save(uint iter, configuration &conf, std::string &conffname,
                   parameter<T> &theparam,
                   supervised_trainer<T,Tdata,Tlabel> &thetrainer,
                   labeled_datasource<T,Tdata,Tlabel> &train_ds,
                   labeled_datasource<T,Tdata,Tlabel> &test_ds,
                   classifier_meter &trainmeter,
                   classifier_meter &testmeter,
                   infer_param &infp, gd_param &gdp, std::string &shortname,
                   long iteration_seconds) {
  // determine base name of output files
  std::ostringstream wname, wfname;
  wname.str("");
  if (conf.exists("job_name")) wname << conf.get_string("job_name");
  else if (conf.exists("name")) wname << conf.get_string("name");
  if (wname.str().length() > 0) wname << "_";
  wname << "net" << std::setfill('0') << std::setw(5) << iter;

  // save samples picking statistics
  if (conf.exists_true("save_pickings")) {
    std::string fname; fname << "pickings_" << iter;
    train_ds.save_pickings(fname.c_str());
  }
  // test
  test(iter, conf, conffname, theparam, thetrainer, train_ds, test_ds,
       trainmeter, testmeter, infp, gdp, shortname, wname.str());

  // save weights and confusion matrix for test set
  // (save after test so that the right saved matrix is associated
  // with latest test in logs).
  wfname.str(""); wfname << wname.str() << ".mat";
  if (conf.exists_false("save_weights"))
    std::cout << "Not saving weights (save_weights set to 0)." << std::endl;
  else {
    std::cout << "saving net to " << wfname.str() << std::endl;
    theparam.save_x(wfname.str().c_str()); // save trained network
    std::cout << "saved=" << wfname.str() << std::endl;
  }
	std::cout << "save_pattern=" << wname.str() << std::endl;

  // set retrain to next iteration with current saved weights
  std::ostringstream progress;
  progress << "retrain_iteration = " << iter + 1 << std::endl
           << "retrain_weights = " << wfname.str() << std::endl;
  if (iteration_seconds > 0)
    progress << "meta_timeout = " << iteration_seconds * 1.2 << std::endl;
  // save progress
  job::write_progress(iter + 1, conf.get_uint("iterations"),
                      progress.str().c_str());
  // save confusion
  if (conf.exists_true("save_confusion")) {
    std::string fname; fname << wname.str() << "_confusion_test.mat";
    std::cout << "saving confusion to " << fname << std::endl;
    save_matrix(testmeter.get_confusion(), fname.c_str());
  }
}

template <typename T, typename Tdata, typename Tlabel>
void test(uint iter, configuration &conf, std::string &conffname,
          parameter<T> &theparam,
          supervised_trainer<T,Tdata,Tlabel> &thetrainer,
          labeled_datasource<T,Tdata,Tlabel> &train_ds,
          labeled_datasource<T,Tdata,Tlabel> &test_ds,
          classifier_meter &trainmeter, classifier_meter &testmeter, 
          infer_param &infp, gd_param &gdp, std::string &shortname,
	  const std::string &basename) {
  timer ttest;
  std::ostringstream wname, wfname;

  //   // some code to average several random solutions
  //     std::cout << "Testing...";
  //     if (original_tests > 1) std::cout << " (" << original_tests << " times)";
  //     std::cout << std::endl;
  //     ttest.restart();
  //     for (uint i = 0; i < original_tests; ++i) {
  //       if (test_only && original_tests > 1) {
  // 	// we obviously wanna test several random solutions
  // 	std::cout << "Initializing weights from random." << std::endl;
  // 	thenet.forget(fgp);
  //       }
  //       if (!no_training_test)
  // 	thetrainer.test(train_ds, trainmeter, infp);
  //       thetrainer.test(test_ds, testmeter, infp);
  //       std::cout << "testing_time="; ttest.pretty_elapsed(); std::cout << std::endl;
  //     }
  //     if (test_only && original_tests > 1) {
  //       // display averages over all tests
  //       testmeter.display_average(test_ds.name(), test_ds.lblstr,
  // 				test_ds.is_test());
  //       trainmeter.display_average(train_ds.name(), train_ds.lblstr,
  // 				 train_ds.is_test());
  //     }
  eblprint("Testing on " << test_ds.size() << " samples...");
  uint maxtest = conf.exists("max_testing") ? conf.get_uint("max_testing") :0;
  ttest.start();
  if (!conf.exists_true("no_training_test")) {
    // test
    thetrainer.test(train_ds, trainmeter, infp, maxtest);
    // save answers
    std::string fname;
    fname << basename << "_" << train_ds.name() << "_answers.csv";
    if (conf.exists_true("save_answers")) train_ds.save_answers(fname);
  }
  if (!conf.exists_true("no_testing_test")) {
    // test
    thetrainer.test(test_ds, testmeter, infp, maxtest);
    // save answers
    std::string fname;
    fname << basename << "_" << test_ds.name() << "_answers.csv";
    if (conf.exists_true("save_answers")) test_ds.save_answers(fname);    
  }
  std::cout << "testing_time="; ttest.pretty_elapsed(); std::cout << std::endl;
  // detection test
  if (conf.exists_true("detection_test")) {
    uint dt_nthreads = 1;
    if (conf.exists("detection_test_nthreads"))
      dt_nthreads = conf.get_uint("detection_test_nthreads");
    timer dtest;
    dtest.start();
    // copy config file and augment it and detect it
    std::string cmd, params;
    if (conf.exists("detection_params")) {
      params = conf.get_string("detection_params");
      params = string_replaceall(params, "\\n", "\n");
    }
    cmd << "cp " << conffname << " tmp.conf && echo \"silent=1\n"
	<< "nthreads=" << dt_nthreads << "\nevaluate=1\nweights_file="
	<< wfname.str() << "\n" << params
	<< "\" >> tmp.conf && detect tmp.conf";
    if (std::system(cmd.c_str()))
      std::cerr << "warning: failed to execute: " << cmd << std::endl;
    std::cout << "detection_test_time="; dtest.pretty_elapsed();
    std::cout << std::endl;
  }
#ifdef __GUI__ // display
  static supervised_trainer_gui<T,Tdata,Tlabel> stgui(shortname.c_str());
  static supervised_trainer_gui<T,Tdata,Tlabel> stgui2(shortname.c_str());
  if (conf.exists("show_font_size"))
    set_global_font_size(conf.get_uint("show_font_size"));
  bool display = conf.exists_true("show_train"); // enable/disable display
  uint ninternals = conf.try_get_uint("show_train_ninternals", 1);
  bool show_train_errors = conf.exists_true("show_train_errors");
  bool show_train_correct = conf.exists_true("show_train_correct");
  bool show_val_errors = conf.exists_true("show_val_errors");
  bool show_val_correct = conf.exists_true("show_val_correct");
  bool show_only_images = conf.exists_true("show_only_images");
  bool show_raw_outputs = conf.exists_true("show_raw_outputs");
  bool show_energies = conf.exists_true("show_energies");
  bool show_all_jitter = conf.exists_true("show_all_jitter");
  uint hsample = conf.try_get_uint("show_hsample", 5);
  uint wsample = conf.try_get_uint("show_wsample", 5);
  if (display) {
    std::cout << "Displaying training..." << std::endl;
    if (show_train_errors) {
      stgui2.display_correctness(true, true, thetrainer, train_ds, infp,
				 hsample, wsample, show_raw_outputs,
				 show_energies, show_all_jitter,
				 show_only_images);
      stgui2.display_correctness(true, false, thetrainer, train_ds, infp,
				 hsample, wsample, show_raw_outputs,
				 show_energies, show_all_jitter,
				 show_only_images);
    }
    if (show_train_correct) {
      stgui2.display_correctness(false, true, thetrainer, train_ds, infp,
				 hsample, wsample, show_raw_outputs,
				 show_energies, show_all_jitter,
				 show_only_images);
      stgui2.display_correctness(false, false, thetrainer, train_ds, infp,
				 hsample, wsample, show_raw_outputs,
				 show_energies, show_all_jitter,
				 show_only_images);
    }
    if (show_val_errors) {
      stgui.display_correctness(true, true, thetrainer, test_ds, infp,
				hsample, wsample, show_raw_outputs,
				show_energies, show_all_jitter,
				show_only_images);
      stgui.display_correctness(true, false, thetrainer, test_ds, infp,
				hsample, wsample, show_raw_outputs,
				show_energies, show_all_jitter,
				show_only_images);
    }
    if (show_val_correct) {
      stgui.display_correctness(false, true, thetrainer, test_ds, infp,
				hsample, wsample, show_raw_outputs,
				show_energies, show_all_jitter,
				show_only_images);
      stgui.display_correctness(false, false, thetrainer, test_ds, infp,
				hsample, wsample, show_raw_outputs,
				show_energies, show_all_jitter,
				show_only_images);
    }
    stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
  }
#endif
}

template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T,Tdata,Tlabel>*
create_validation_set(configuration &conf, uint &noutputs,
                      std::string &valdata) {
  bool classification = conf.exists_true("classification");
  valdata = conf.get_string("val");
  std::string vallabels, valclasses, valjitters, valscales;
  vallabels = conf.try_get_string("val_labels");
  valclasses = conf.try_get_string("val_classes");
  valjitters = conf.try_get_string("val_jitters");
  valscales = conf.try_get_string("val_scales");
  uint maxval = 0;
  if (conf.exists("val_size")) maxval = conf.get_uint("val_size");
  labeled_datasource<T,Tdata,Tlabel> *val_ds = NULL;
  if (classification) { // classification task
    class_datasource<T,Tdata,Tlabel> *ds =
	new class_datasource<T,Tdata,Tlabel>;
    ds->init(valdata.c_str(), vallabels.c_str(), valjitters.c_str(),
             valscales.c_str(), valclasses.c_str(), "val", maxval);
    if (conf.exists("limit_classes"))
      ds->limit_classes(conf.get_int("limit_classes"), 0,
			conf.exists_true("limit_classes_random"));
    noutputs = ds->get_nclasses();
    val_ds = ds;
  } else { // regression task
    val_ds = new labeled_datasource<T,Tdata,Tlabel>;
    val_ds->init(valdata.c_str(), vallabels.c_str(), valjitters.c_str(),
		 valscales.c_str(), "val", maxval);
    idxdim d = val_ds->label_dims();
    noutputs = d.nelements();
  }
  if (conf.exists("classification_outputs"))
    noutputs = conf.get_uint("classification_outputs");
  val_ds->set_test(); // test is the test set, used for reporting
  val_ds->pretty();
  if (conf.exists("data_bias"))
    val_ds->set_data_bias((T)conf.get_double("data_bias"));
  if (conf.exists("data_coeff"))
    val_ds->set_data_coeff((T)conf.get_double("data_coeff"));
  if (conf.exists("label_bias"))
    val_ds->set_label_bias((T)conf.get_double("label_bias"));
  if (conf.exists("label_coeff"))
    val_ds->set_label_coeff((T)conf.get_double("label_coeff"));
  if (conf.exists("epoch_show_modulo"))
    val_ds->set_epoch_show(conf.get_uint("epoch_show_modulo"));
  val_ds->keep_outputs(conf.exists_true("keep_outputs"));
  if (conf.exists_true("save_answers")) {
    eblprint("Forcing datasource to keep outputs in order to save answers.");
    val_ds->keep_outputs(true);    
  }
  return val_ds;
}

template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T,Tdata,Tlabel>*
create_training_set(configuration &conf, uint &noutputs, std::string &traindata) {
  bool classification = conf.exists_true("classification");
  traindata = conf.get_string("train");
  std::string trainlabels, trainclasses, trainjitters, trainscales;
  trainlabels = conf.try_get_string("train_labels");
  trainclasses = conf.try_get_string("train_classes");
  trainjitters = conf.try_get_string("train_jitters");
  trainscales = conf.try_get_string("train_scales");
  uint maxtrain = 0;
  if (conf.exists("train_size")) maxtrain = conf.get_uint("train_size");
  labeled_datasource<T,Tdata,Tlabel> *train_ds = NULL;
  if (classification) { // classification task
    class_datasource<T,Tdata,Tlabel> *ds =
	new class_datasource<T,Tdata,Tlabel>;
    ds->init(traindata.c_str(), trainlabels.c_str(),
             trainjitters.c_str(), trainscales.c_str(),
             trainclasses.c_str(), "train", maxtrain);
    if (conf.exists("balanced_training"))
      ds->set_balanced(conf.get_bool("balanced_training"));
    if (conf.exists("random_class_order"))
      ds->set_random_class_order(conf.get_bool("random_class_order"));
    if (conf.exists("limit_classes"))
      ds->limit_classes(conf.get_int("limit_classes"), 0,
			conf.exists_true("limit_classes_random"));
    if (conf.exists("class_probabilities")) {
      std::string s = conf.get_string("class_probabilities");
      std::vector<float> v = string_to_floatvector(s.c_str());
      ds->set_class_probabilities(v);
    }
    noutputs = ds->get_nclasses();
    train_ds = ds;
  } else { // regression task
    train_ds = new labeled_datasource<T,Tdata,Tlabel>;
    train_ds->init(traindata.c_str(), trainlabels.c_str(),
                   trainjitters.c_str(), trainscales.c_str(),
                   "train", maxtrain);
    idxdim d = train_ds->label_dims();
    noutputs = d.nelements();
  }
  train_ds->ignore_correct(conf.exists_true("ignore_correct"));
  train_ds->set_weigh_samples(conf.exists_true("sample_probabilities"),
                              conf.exists_true("hardest_focus"),
                              conf.exists_true("per_class_norm"),
                              conf.exists("min_sample_weight") ?
                              conf.get_double("min_sample_weight") : 0.0);
  train_ds->set_shuffle_passes(conf.exists_bool("shuffle_passes"));
  if (conf.exists("epoch_size"))
    train_ds->set_epoch_size(conf.get_int("epoch_size"));
  if (conf.exists("epoch_mode"))
    train_ds->set_epoch_mode(conf.get_uint("epoch_mode"));
  if (conf.exists("epoch_show_modulo"))
    train_ds->set_epoch_show(conf.get_uint("epoch_show_modulo"));
  train_ds->pretty();
  if (conf.exists("data_bias"))
    train_ds->set_data_bias((T)conf.get_double("data_bias"));
  if (conf.exists("data_coeff"))
    train_ds->set_data_coeff((T)conf.get_double("data_coeff"));
  if (conf.exists("label_bias"))
    train_ds->set_label_bias((T)conf.get_double("label_bias"));
  if (conf.exists("label_coeff"))
    train_ds->set_label_coeff((T)conf.get_double("label_coeff"));
  train_ds->keep_outputs(conf.exists_true("keep_outputs"));
  if (conf.exists_true("save_answers")) {
    eblprint("Forcing datasource to keep outputs in order to save answers.");
    train_ds->keep_outputs(true);    
  }
  return train_ds;
}

} // end namespace ebl

#endif /* TRAIN_UTILS_HPP_ */
