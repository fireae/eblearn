/***************************************************************************
 *   Copyright (C) 2012 by Soumith Chintala *
 *   soumith@gmail.com *
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

#ifdef __MATLAB__

#include <sstream>
#include <iomanip>
#include "stl.h"
#include <string.h>
#include "libidx.h"

#define APP_DOESNT_USE_GUI // needed to not include gui defs
#include "eblapp.h"

intg dim = 0, size = 0, off = 0;
char* in_file;
char* out_file;
char* out_var;

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc <= 3) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  in_file = argv[1];
  out_file = argv[2];
  out_var = argv[3];
  return true;
}

// print command line usage
void print_usage() {
  cout << "Usage: ./ebl2matlab input_file output_file " 
       << "output_variable_name" << endl ;
  cout << "Example: ./ebl2matlab a.mat b.mat var_a" << endl;
}

template <typename T>
void convert(char* infile, char* outfile, char* varname) {
  idx<T> in_idx = load_matrix<T>(infile);
  ebl::matlab ebm(outfile);
  ebm.save_matrix<T>(in_idx, varname);
}

int main(int argc, char **argv) {
  cout << "___________________________________________________________________";
  cout << endl << endl;
  cout << "             ebl2matlab for eblearn" << endl;
  cout << "  A tool to convert eblearn format .mat files to matlab "
       <<" format .mat files " << endl;
  cout << "___________________________________________________________________";
  cout << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  try {
    // load file
    string filename = in_file;
    if (!file_exists(filename.c_str()))
      eblerror("file not found: " << filename);
    int type = get_matrix_type(filename.c_str());
    // load based on type
    switch (type) {
    case MAGIC_BYTE_MATRIX:
    case MAGIC_UBYTE_VINCENT:
      convert<ubyte>(in_file, out_file, out_var);
      break ;
    case MAGIC_INTEGER_MATRIX:
    case MAGIC_INT_VINCENT:
      convert<int>(in_file, out_file, out_var);
      break ;
    case MAGIC_FLOAT_MATRIX:
    case MAGIC_FLOAT_VINCENT:
      convert<float>(in_file, out_file, out_var);
      break ;
    case MAGIC_DOUBLE_MATRIX:
    case MAGIC_DOUBLE_VINCENT:
      convert<double>(in_file, out_file, out_var);
      break ;
    case MAGIC_LONG_MATRIX:
      convert<long>(in_file, out_file, out_var);
      break ;
    case MAGIC_UINT_MATRIX:
      convert<uint>(in_file, out_file, out_var);
      break ;
    default: eblerror("unsupported type " << type);
    }
  } eblcatch();
  return 0;
}


#endif // __MATLAB__
