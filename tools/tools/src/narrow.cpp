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

#include <sstream>
#include <iomanip>
#include "stl.h"
#include <string.h>
#include "libidx.h"

#define APP_DOESNT_USE_GUI // needed to not include gui defs
#include "eblapp.h"

intg dim = 0, size = 0, off = 0;

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc <= 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  // loop over arguments
  for (int i = 2; i < argc; ++i) {
    try {
      if (strcmp(argv[i], "-dim") == 0) {
	++i; if (i >= argc) throw 1;
	dim = atoi(argv[i]);
      } else if (strcmp(argv[i], "-size") == 0) {
	++i; if (i >= argc) throw 1;
	size = atoi(argv[i]);
      } else if (strcmp(argv[i], "-off") == 0) {
	++i; if (i >= argc) throw 1;
	off = atoi(argv[i]);
      } else if ((strcmp(argv[i], "-help") == 0) ||
		 (strcmp(argv[i], "-h") == 0)) {
	return false;
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
  cout << "Usage: ./narrow <filename> [OPTIONS]"
       << endl << "Options are:" << endl;
  cout << "  -dim <dimension>" << endl
       << "   Dimensions to narrow." << endl;
  cout << "  -size <size>" << endl
       << "   Target size of dimension." << endl;
  cout << "  -off <offset>" 
       << "   Offset at which to narrow dimension." << endl;
}

template <typename T>
void narrow(string &filename, intg dim, intg size, intg off) {  
  string f2;
  f2 << filename << "_narrowed_" << dim << "_" << size << "_" << off << ".mat";
  if (has_multiple_matrices(filename.c_str())) {
    midx<T> m = load_matrices<T>(filename);
    midx<T> n = m.narrow(dim, size, off);
    cout << "narrowed matrix " << m << " into " << n << endl;
    cout << "saving matrix to " << f2 << endl;
    save_matrices(n, f2);
  } else {
    idx<T> m = load_matrix<T>(filename);
    idx<T> n = m.narrow(dim, size, off);
    cout << "narrowed matrix " << m << " into " << n << endl;
    cout << "saving matrix to " << f2 << endl;
    save_matrix(n, f2);
  }
}

int main(int argc, char **argv) {
  cout << "___________________________________________________________________";
  cout << endl << endl;
  cout << "             narrow for eblearn" << endl;
  cout << "___________________________________________________________________";
  cout << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  try {
    // load file
    string filename = argv[1];
    if (!file_exists(filename.c_str()))
      eblerror("file not found: " << filename);
    int type = get_matrix_type(filename.c_str());
    // load based on type
    switch (type) {
    case MAGIC_BYTE_MATRIX:
    case MAGIC_UBYTE_VINCENT:
      narrow<ubyte>(filename, dim, size, off);
      break ;
    case MAGIC_INTEGER_MATRIX:
    case MAGIC_INT_VINCENT:
      narrow<int>(filename, dim, size, off);
      break ;
    case MAGIC_FLOAT_MATRIX:
    case MAGIC_FLOAT_VINCENT:
      narrow<float>(filename, dim, size, off);
      break ;
    case MAGIC_DOUBLE_MATRIX:
    case MAGIC_DOUBLE_VINCENT:
      narrow<double>(filename, dim, size, off);
      break ;
    case MAGIC_LONG_MATRIX:
      narrow<long>(filename, dim, size, off);
      break ;
    case MAGIC_UINT_MATRIX:
      narrow<uint>(filename, dim, size, off);
      break ;
    default: eblerror("unsupported type " << type);
    }
  } eblcatch();
  return 0;
}

