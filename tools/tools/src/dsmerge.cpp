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
bool fullnames = false;

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
  for (int i = 5; i < argc; ++i) {
    if ((strcmp(argv[i], "-full") == 0))
    fullnames = true;
  }
  return true;
}

// print command line usage
void print_usage() {
  cout << "Usage: ./dsmerge <root> <ds0 name> <ds1 name> <ds2 name> [OPTIONS]" 
       << endl
       << "Options:" << endl
       << "-full (expect full paths for each name)" << endl;
}

template <class Tdata>
void merge() {
  cout << "input parameters:" << endl;
  cout << "  input directory: " << inroot << endl;
  cout << "  output dataset name: " << ds0_name << endl;
  cout << "  input dataset 1 name: " << ds1_name << endl;
  cout << "  input dataset 2 name: " << ds2_name << endl;
  cout << "___________________________________________________________________";
  cout << endl;
  dataset<Tdata> ds0(ds0_name.c_str());
  ds0.merge_and_save(ds1_name.c_str(), ds2_name.c_str(), inroot);
}

int main(int argc, char **argv) {
  // std::system("hostname");
  // return 0;
  cout << "___________________________________________________________________";
  cout << endl << endl;
  cout << "             Dataset merger for libeblearn library " << endl;
  cout << "___________________________________________________________________";
  cout << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  // setup full paths
  inroot << "/";
  string inroot2 = inroot;
  if (fullnames) inroot2 = "";
  string tmp = ds0_name; ds0_name = inroot2;
  ds0_name << tmp;
  tmp = ds1_name; ds1_name = inroot2;
  ds1_name << tmp;
  tmp = ds2_name; ds2_name = inroot2;
  ds2_name << tmp;

  // compile with specificed precision
  string precision;
  string ds_fname;
  ds_fname << ds1_name; //<< inroot2
  string data_fname;
  build_fname(ds_fname, DATA_NAME, data_fname);
  try { // get matrix type
    get_matrix_type(data_fname.c_str(), precision);
    cout << "Data matrix type: " << precision << endl;
    int t = get_matrix_type(data_fname.c_str());
    switch (t) {
    case MAGIC_FLOAT_MATRIX:
    case MAGIC_FLOAT_VINCENT:
      merge<float>();
      break ;
    case MAGIC_DOUBLE_MATRIX:
    case MAGIC_DOUBLE_VINCENT:
      merge<double>();
      break ;
    case MAGIC_INTEGER_MATRIX:
    case MAGIC_INT_VINCENT:
      merge<int>();
      break ;
    case MAGIC_BYTE_VINCENT:
      merge<char>();
      break ;
    case MAGIC_BYTE_MATRIX:
    case MAGIC_UBYTE_VINCENT:
      merge<ubyte>();
      break ;
    case MAGIC_SHORT_MATRIX:
    case MAGIC_SHORT_VINCENT:
      merge<short>();
      break ;
    case MAGIC_LONG_MATRIX:
      merge<long int>();
      break ;
    default:
      eblerror("unsupported precision for dataset loading");
    }
  } eblcatcherror();
  return 0;
}
