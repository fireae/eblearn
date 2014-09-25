/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
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

#include <iomanip>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include "libidx.h"
#include "libeblearn.h"
#include "libeblearntools.h"
#include "eblapp.h"

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#ifdef __GUI__
#include "libeblearngui.h"
#endif

typedef float T; // precision at which network is fprop (float is ok)
typedef float Tdata; // data's original type
typedef int Tlabel; // label's original type
#define bbsds T,Tdata,int

// global variables

bool dump = false; // Dump weights/buffers of first sample.

// dataset fproping ////////////////////////////////////////////////////////////

void fprop_and_save(configuration &conf, module_1_1<T> &net,
		    datasource<T,Tdata> &ds,
		    string outdir, string data_fname, string arch_name,
		    uint &counter, uint total_size) {
  cout << "Fprop network on " << ds.size() << " samples from " << ds.name()
       << endl;
  ostringstream name, fname;
  state<T> in, out;

  // determine output size
  ds.init_epoch();
  ds.fprop_data(in);
  net.fprop(in, out);
  cout << "Input size: " << in << " Output size: " << out << endl;

  // input/output names
  string output = outdir;
  output << "/" << ebl::basename(data_fname.c_str()) << "_" << arch_name;
  string outtmp;
  outtmp << output << "_tmp/";
  if (!mkdir_full(outtmp.c_str()))
    eblerror("failed to create " << outtmp);
  cout << "Input: " << data_fname << endl;
  cout << "Output: " << output << endl;
  cout << "Temporary outputs: " << outtmp << endl;

  // extract
  timer textraction;
  textraction.start(); // start extraction timer
  uint info = 100;
  std::list<string> files;
  if (conf.exists("epoch_show_modulo"))
    info = conf.get_uint("epoch_show_modulo");
  ds.init_epoch();
  ds.set_test(); // make sure we iterate straight through dataset
  for (uint i = 0; i < ds.size(); ++i) {
    // fprop
    TIMING2("between fprop and sample retrieval");
    ds.fprop_data(in); // get input
    TIMING2("sample retrieval");
    if (dump) {
      cout << "Dumping weights/internal buffers of " << in << endl;
      net.fprop_dump(in, out);
      return ;
    } else
      net.fprop(in, out); // fprop input
    TIMING2("entire fprop");
    // save output to individual files
    string file;
    file << outtmp << i << ".mat";
    save_matrix(out, file);
    files.push_back(file);
    // move to next input
    ds.next();
    counter++;
    if (counter % info == 0)
      cout << "Extracted " << counter << " / " << total_size << ", elapsed: "
	   << textraction.elapsed() << ", ETA: "
	   << textraction.eta(counter, total_size) << endl;
  }
  cout << "Extraction time: " << textraction.elapsed() << endl;

  // saving outputs
  string out_fname;
  out_fname << output << "_data.mat";
  if (conf.exists_true("save_static")) {
    cout << "Saving (static) matrix file to " << out_fname << "..." << endl;
    save_matrix<T>(files, out_fname);
    idx<T> allout = load_matrix<T>(out_fname);
    cout << "Saved output to " << out_fname << " (" << allout
	 << ")" << endl;
  } else {
    cout << "Saving (dynamic) matrix file to " << out_fname << "..." << endl;
    save_matrices<T>(files, out_fname);
    midx<T> allout = load_matrices<T>(out_fname, true);
    cout << "Saved output to " << out_fname << " (" << allout
	 << ")" << endl;
  }
  // delete temporary files
  list<string>::iterator fi = files.begin();
  for ( ; fi != files.end(); ++fi)
    rm_file(fi->c_str());
  rm_file(outtmp.c_str());
}

// args ////////////////////////////////////////////////////////////////////////

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc <= 1) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  // loop over arguments
  for (int i = 2; i < argc; ++i) {
    try {
      if (strcmp(argv[i], "-dump") == 0) {
	dump = true;
      } else throw 2;
    } catch (int err) {
      cerr << "input error: ";
      switch (err) {
        case 0: cerr << "expecting string after " << argv[i-1]; break;
        case 1: cerr << "expecting integer after " << argv[i-1]; break;
        case 2: cerr << "unknown parameter " << argv[i-1]; break;
        default: cerr << "undefined error";
      }
      cerr << endl << endl;
      return false;
    }
  }
  return true;
}

// print command line usage
void print_usage() {
  cout << "Usage: ./dsfprop <conf> [OPTIONS]"
       << endl << "Options are:" << endl;
  cout << "  -dump" << endl
       << "   Dump individual weights and internal buffers of 1st sample."
       << endl;
}

// main ////////////////////////////////////////////////////////////////////////

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
  int main(int argc, char **argv) { // regular main without gui
#endif
    cout << "* Dataset fprop" << endl;
    // parse arguments
    if (!parse_args(argc, argv)) {
      print_usage();
      return -1;
    }
#ifdef __LINUX__
    feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
    try {
      cout << "Using random seed " << dynamic_init_drand(argc, argv) << endl;
      timer gtimer;
      gtimer.start(); // total running time
      configuration conf(argv[1]); // configuration file
      conf.set("run_type", "fprop"); // tell conf that we are in fprop mode
      conf.resolve();
      string input_root = conf.get_string("root");
      string outdir = input_root;
      if (conf.exists("root_out")) outdir = conf.get_string("root_out");
      uint              ipp_cores     = 1;
      if (conf.exists("ipp_cores")) ipp_cores = conf.get_uint("ipp_cores");
      ipp_init(ipp_cores); // limit IPP (if available) to 1 core

      //! load datasets
      uint noutputs = 0;
      labeled_datasource<T,Tdata,Tlabel> *train_ds = NULL;
      labeled_datasource<T,Tdata,Tlabel> *test_ds = NULL;
      string valdata, traindata;
      test_ds = create_validation_set<T,Tdata,Tlabel>(conf, noutputs, valdata);
      if (!conf.exists_true("test_only"))
        train_ds =
            create_training_set<T,Tdata,Tlabel>(conf, noutputs, traindata);

      // create network ////////////////////////////////////////////////////////
      answer_module<bbsds> *answer = create_answer<bbsds>(conf, noutputs);
      if (answer) {
        cout << "Answering module: " << answer->describe() << endl;
        // update number of outputs given the answer module
        noutputs = answer->get_nfeatures();
      }
      //! create the network weights, network and trainer
      parameter<T> theparam;// create trainable parameter
      intg inthick = conf.exists("input_thickness") ?
          conf.get_int("input_thickness") : -1;
      module_1_1<T> *net =
          create_network<T>(theparam, conf, inthick, noutputs, "arch");
      //! initialize the network weights
      bool fixed_random = conf.try_get_bool("fixed_randomization", false);
      forget_param_linear fgp(1, 0.5, !fixed_random);
      if (!fixed_random) fgp.seed(conf.str());
      if (conf.exists_true("retrain")) {
        theparam.load_x(conf.get_cstring("retrain_weights"));
      } else {
        cout << "Initializing weights from random." << endl;
        net->forget(fgp);
      }
      if (!conf.exists_true("retrain") && conf.exists_true("manual_load"))
        manually_load_network(*((layers<T>*)net), conf);

      // save weights //////////////////////////////////////////////////////////
      outdir << "/features/";
      mkdir_full(outdir);
      cout << "Saving outputs to " << outdir << endl;
      string arch_name = conf.get_string("arch_name");
      if (conf.exists("meta_conf_shortname"))
        arch_name = conf.get_string("meta_conf_variables");
      string weights_name = outdir;
      weights_name << arch_name << "_weights.mat";
      theparam.save_x(weights_name.c_str());
      cout << "Saved weights to " << weights_name << endl;

      // fprop network /////////////////////////////////////////////////////////
      uint total_size = 0;
      if (test_ds) total_size += test_ds->size();
      if (train_ds) total_size += train_ds->size();
      uint counter = 0;
      if (test_ds)
        fprop_and_save(conf, *net, *test_ds, outdir, valdata, arch_name,
                       counter, total_size);
      if (!dump && train_ds)
        fprop_and_save(conf, *net, *train_ds, outdir, traindata, arch_name,
                       counter, total_size);

      //free variables
      if (net) delete net;
      if (test_ds) delete test_ds;
      if (train_ds) delete train_ds;
#ifdef __GUI__
      quit_gui(); // close all windows
#endif
      cout << "dsfprop done. Running time: " << gtimer.elapsed() << endl;
    } eblcatcherror();
    return 0;
  }
