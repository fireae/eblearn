
/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
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

#include <cstdlib>
#include <iostream>

#ifdef __GUI__
#include "libidxgui.h"
#include "libeblearngui.h"
#endif

#include "dataset.h"
#include "pascal_dataset.h"
#include "pascalbg_dataset.h"
#include "eblapp.h"

string inroot = ".";
string ds0_name = "";
string ds1_name = "";
string ds2_name = "";
intg ds1_max_per_class = 0;
int draws = 1;
bool draws_set = false;
vector<string>  exclude;

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 5) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  inroot = argv[1];
  ds0_name = argv[2];
  ds1_name = argv[3];
  ds2_name = argv[4];

  // if requesting help, print usage
  if ((strcmp(argv[1], "-help") == 0) ||
      (strcmp(argv[1], "-h") == 0))
    return false;
  // loop over arguments
  for (int i = 2; i < argc; ++i) {
    if (strcmp(argv[i], "-maxperclass") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -max_per_class." << endl;
	return false;
      }
      ds1_max_per_class = atoi(argv[i]);
    } else if (strcmp(argv[i], "-draws") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -draws." << endl;
	return false;
      }
      draws = atoi(argv[i]);
      draws_set = true;
    } else if (strcmp(argv[i], "-exclude") == 0) {
      ++i; if (i >= argc) throw 0;
      exclude.push_back(argv[i]);
    }
  }
  return true;
}

// print command line usage
void print_usage() {
  cout << "Usage: ./dataset_split <root> <ds0 name> <ds1 name> <ds2 name>";
  cout << " [OPTIONS]" << endl;
  cout << "Options are:" << endl;
  cout << "  -maxperclass <integer n>" << endl;
  cout << "    This limits ds1 to n samples per class,";
  cout << "    assigning the rest to ds2." << endl;
  cout << "  -draws <integer n>" << endl;
  cout << "    Draws n dataset splits, shuffling assignments between";
  cout << " each draw." << endl;
  cout << "  -exclude <class name> (include all but excluded classes," << endl;
  cout << "                         exclude can be called multiple times)";
  cout << endl;
}

template <class Tdata>
void split() {
  dataset<Tdata> ds0(ds0_name.c_str());
  if (exclude.size() > 0)
    ds0.set_exclude(exclude);
  ds0.load(inroot);
  for (int i = 0; i < draws; ++i) {
    string ds1 = ds1_name;
    string ds2 = ds2_name;
    if (draws_set) {
      int id = i + 1;
      ostringstream oss;
      oss << id;
      cout << "__________________________ drawing dataset pair #" << id;
      cout << " __________________________" << endl;
      ds1 += oss.str();
      ds2 += oss.str();
    }
    ds0.split_max_and_save(ds1.c_str(), ds2.c_str(),
			   ds1_max_per_class, inroot);
  }
}

int main(int argc, char **argv) {
  cout << "___________________________________________________________________";
  cout << endl << endl;
  cout << "             Dataset splitter for libeblearn library " << endl;
  cout << "___________________________________________________________________";
  cout << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  cout << "input parameters:" << endl;
  cout << "  input directory: " << inroot << endl;
  cout << "  input dataset name: " << ds0_name << endl;
  cout << "  output dataset 1 name: " << ds1_name << endl;
  cout << "  output dataset 2 name: " << ds2_name << endl;
  cout << "  max per class for dataset 1: " << ds1_max_per_class;
  cout << " (the rest goes to dataset 2)" << endl;
  cout << "  number of dataset pairs to randomly draw: " << draws << endl;
  cout << "___________________________________________________________________";
  cout << endl;

  // compile with specificed precision
  string precision;
  string ds_fname = inroot;
  ds_fname += "/";
  ds_fname += ds0_name;
  string data_fname;
  build_fname(ds_fname, DATA_NAME, data_fname);
  try { // get matrix type
    get_matrix_type(data_fname.c_str(), precision);
    cout << "Data matrix type: " << precision << endl;
    int t = get_matrix_type(data_fname.c_str());
    switch (t) {
    case MAGIC_FLOAT_MATRIX:
    case MAGIC_FLOAT_VINCENT:
      split<float>();
      break ;
    case MAGIC_DOUBLE_MATRIX:
    case MAGIC_DOUBLE_VINCENT:
      split<double>();
      break ;
    case MAGIC_INTEGER_MATRIX:
    case MAGIC_INT_VINCENT:
      split<int>();
      break ;
    case MAGIC_BYTE_VINCENT:
      split<char>();
      break ;
    case MAGIC_BYTE_MATRIX:
    case MAGIC_UBYTE_VINCENT:
      split<ubyte>();
      break ;
    case MAGIC_SHORT_MATRIX:
    case MAGIC_SHORT_VINCENT:
      split<short>();
      break ;
    case MAGIC_LONG_MATRIX:
      split<long int>();
      break ;
    default:
      eblerror("unsupported precision for dataset loading");
    }
  } eblcatcherror();
  return 0;
}
