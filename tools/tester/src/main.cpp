
/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
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

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResultCollector.h>

#include "libidx.h"
#include "libeblearn.h"
#include "libeblearntools.h"
#include "MyTextOutputter.h"

#include "idxIO_test.h"
#include "idx_test.h"
#include "idxops_test.h"
#include "idxops_test2.h"
#include "ebl_basic_test.h"
#include "ebl_preprocessing_test.h"
#include "datasource_test.h"
#include "idxiter_test.h"
#include "detector_test.h"
#include "ClusterTest.h"
#include "image_test.h"
#include "ebl_machines_test.h"
#include "gui3d_test.h"
#ifdef __TH__
#include "thops_test.h"
#endif

#include "utils.h"
#include "tools_utils.h"
#include "eblapp.h"

#ifdef __IPP__
#include "ippops_test.h"
#endif

#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace std;
using namespace CppUnit;

//! global variables
string *gl_mnist_dir = NULL;
string *gl_data_dir = NULL;
string *gl_mnist_errmsg = NULL;
string *gl_data_errmsg = NULL;
#ifdef __WINDOWS__
bool    color_print = false;
#else
bool    color_print = true;
#endif

#define MNIST_URL "http://yann.lecun.com/exdb/mnist/"
#define MNIST1 "train-labels-idx1-ubyte"
#define MNIST2 "t10k-labels-idx1-ubyte"
#define MNIST3 "t10k-images-idx3-ubyte"
#define MNIST4 "train-images-idx3-ubyte"

// Read arguments from run.init file in working directory
// (easiest way to configure the run within eclipse and without it).
void parse_args(int argc, char **argv) {
  string dir = ebl::dirname(argv[0]);
  string ini = dir;
  ini += "/run.init";
  gl_mnist_dir = NULL;
  gl_data_dir = NULL;
  if (strlen(MNIST_PATH) > 0) gl_mnist_dir = new string(MNIST_PATH);
  if (strlen(DATA_PATH) > 0) gl_data_dir = new string(DATA_PATH);
  gl_mnist_errmsg =
      new string("MNIST directory is unknown, some tests will be ignored (MNIST can be downloaded at http://yann.lecun.com/exdb/mnist/)");
  gl_data_errmsg =
      new string("Data directory is unknown, some tests will be ignored");
  string s;
  ifstream in(ini.c_str());
  if (!in) in.open(ini.c_str());
  if (in) {
    while (!in.eof()) {
      in >> s;
      if (s == "-mnist") {
	in >> s;
	gl_mnist_dir = new string(s.c_str());
      }
      if (s == "-data") {
	in >> s;
	if (gl_data_dir)
          delete gl_data_dir;
	gl_data_dir = new string(s.c_str());
      }
    }
  }
  in.close();
  if (gl_mnist_dir == NULL) {
    // ask user to enter its mnist path
    int res = 0;
    char buf[512];
    memset(buf, 0, 512);
#ifdef __WINDOWS__
    cout << "Please enter your MNIST absolute path or ENTER to ignore:" << endl;
#else
    cout << "Please enter your MNIST absolute path, ENTER to ignore"
	 << " or 'D' and ENTER to download:" << endl;
#endif
    if (gets(buf) && strlen(buf) > 0) {
#ifndef __WINDOWS__
      if (!strcmp(buf, "D")) { // download
	cout << "Please enter the absolute path where you want to download"
             << " MNIST:" << endl;
	memset(buf, 0, 512);
	if (!gets(buf) || !ebl::mkdir_full(buf)) {
          cout << "Cancelling MNIST download." << endl;
          cout << "Warning: " << *gl_mnist_errmsg << endl;
	} else {
          // download
          gl_mnist_dir = new string(buf);
          cout << "Downloading MNIST to " << *gl_mnist_dir << " ..." << endl;
          ostringstream cmd;
          res = 0;
          cmd.str(""); cmd << "cd " << *gl_mnist_dir << " && wget "
                           << MNIST_URL << MNIST1 << ".gz";
          res += std::system(cmd.str().c_str());
          cmd.str(""); cmd << "cd " << *gl_mnist_dir << " && wget "
                           << MNIST_URL << MNIST2 << ".gz";
          res += std::system(cmd.str().c_str());
          cmd.str(""); cmd << "cd " << *gl_mnist_dir << " && wget "
                           << MNIST_URL << MNIST3 << ".gz";
          res += std::system(cmd.str().c_str());
          cmd.str(""); cmd << "cd " << *gl_mnist_dir << " && wget "
                           << MNIST_URL << MNIST4 << ".gz";
          res += std::system(cmd.str().c_str());
          cmd.str(""); cmd << "cd " << *gl_mnist_dir << " && gunzip *";
          res += std::system(cmd.str().c_str());
          if (res < 0) {
            cerr << "error: Failed to retrieve MNIST.";
            delete gl_mnist_dir;
            gl_mnist_dir = NULL;
          } else {
            cout << "Succesfully uncompressed MNIST into " << *gl_mnist_dir
		 << endl;
          }
	}
      } else
#endif
      {
        // check that directory exists and contains the files
        gl_mnist_dir = new string(buf);
        ostringstream f;
        try {
          f.str(""); f << buf;
          if (!ebl::dir_exists(buf)) throw f.str();
          f.str(""); f << buf << "/" << MNIST1;
          if (!ebl::file_exists(f.str().c_str())) throw f.str();
          f.str(""); f << buf << "/" << MNIST2;
          if (!ebl::file_exists(f.str().c_str())) throw f.str();
          f.str(""); f << buf << "/" << MNIST3;
          if (!ebl::file_exists(f.str().c_str())) throw f.str();
          f.str(""); f << buf << "/" << MNIST4;
          if (!ebl::file_exists(f.str().c_str())) throw f.str();
        } catch (string &err) {
          delete gl_mnist_dir;
          gl_mnist_dir = NULL;
          cerr << "Could not find " << err << endl;
          cout << "Warning: " << *gl_mnist_errmsg << endl;
        }
      }
    }
    if (gl_mnist_dir) { // the path was successfully set, remember it
      ofstream out(ini.c_str());
      if (!out)
	out.open(ini.c_str());
      if (out) {
	out << "-mnist " << *gl_mnist_dir << endl;
	out.close();
	cout << "Remembering mnist path in " << ini << endl;
      }
    }
  }
  if (gl_mnist_dir == NULL) {
    cout << "Warning: " << *gl_mnist_errmsg << endl;
  } else
    cout << "Using MNIST directory: " << *gl_mnist_dir << endl;
  if (gl_data_dir == NULL) {
    cout << "Warning: " << *gl_data_errmsg << endl;
  } else
    cout << "Using data directory: " << *gl_data_dir << endl;

  // Read arguments from shell input
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-nocolor") == 0) {
      color_print = false;
      cout << "Not using colors in shell." << endl;
    }
  }
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) {
#else
  int main(int argc, char **argv) {
#endif
#ifdef __LINUX__
    // enable float exceptions to halt instead of propagating errors
    feenableexcept(FE_DIVBYZERO | FE_INVALID);
#endif
    ebl::ipp_init(1); // limit IPP (if available) to 1 core
    //  gui.set_silent();
    cout << "***** Unit tester for libeblearn and libidx libraries *****" << endl;
    // parse arguments
    parse_args(argc, argv);
    cout << "***********************************************************" << endl;

    // cppunit tests
    CppUnit::BriefTestProgressListener listener;
    CppUnit::TestResultCollector collector;
    CppUnit::TextUi::TestRunner runner;
    runner.eventManager().addListener(&listener); // show each test
    runner.eventManager().addListener(&collector);
    // note: the custom outputter will be freed by the runner
    MyTextOutputter *outputter = new MyTextOutputter(&collector, cout);
    runner.setOutputter(outputter);

    // fixed randomization
    fixed_init_drand();

    // adding test suites
    runner.addTest(idxIO_test::suite());
    runner.addTest(idx_test::suite());
    runner.addTest(idxiter_test::suite());
    runner.addTest(idxops_test::suite());
    runner.addTest(idxops_test2::suite());
#ifdef __IPP__
    runner.addTest(ippops_test::suite());
#endif
#ifdef __TH__
    runner.addTest(thops_test::suite());
#endif
    runner.addTest(ClusterTest::suite());
    runner.addTest(datasource_test::suite());
    runner.addTest(ebl_basic_test::suite());
    runner.addTest(ebl_preprocessing_test::suite());
    runner.addTest(image_test::suite());
    runner.addTest(detector_test::suite());
    runner.addTest(ebl_machines_test::suite());
#ifdef __GUI3D__
    //   runner.addTest(gui3d_test::suite());
#endif

    // run all tests
    runner.run();

    // print summary
    if (!collector.wasSuccessful()) {
      outputter->printHeader();
      cout << endl;
    }

    if (gl_mnist_dir) delete gl_mnist_dir;
    if (gl_data_dir) delete gl_data_dir;
    if (gl_mnist_errmsg) delete gl_mnist_errmsg;
    if (gl_data_errmsg) delete gl_data_errmsg;

    return 0;
  }
