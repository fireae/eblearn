/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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
#include <vector>
#include <sstream>
#include <iomanip>

#include "libidx.h"
#include "libeblearn.h"
#include "eblapp.h"

#include "camera_opencv.h"

#ifdef __GUI__
#include "libidxgui.h"
#include "libeblearngui.h"
#endif

// TODO: temporary, need cleaner solution
typedef float t_data;

// global variables: parameters
idxdim		dims = idxdim(4, 8);	//!< number of samples to display (hxw)
bool		info = false;	//!< only print information and quit
bool		size = false;	//!< only print size info and quit
vector<double>	range;		//!< display range of values

////////////////////////////////////////////////////////////////
// interface

// print command line usage
void print_usage() {
  cout << "Usage: ./dataset_display <root/name> [OPTIONS]" << endl;
  cout << "  example: ./dataset_display /datasets/pascal" << endl;
  cout << "Options are:" << endl;
  cout << "  -info, do not display dataset, just statistics on stdout."<< endl;
  cout << "  -size, do not display dataset, just print the size and exit."
       << endl;
  cout << "  -dims <dimensions, default: 4x8>, the number of columns and ";
  cout << " rows to display." << endl;
  cout << "  -range <range, e.g.: -1,1>: range to map to 0..255 for display.";
  cout << endl;
}

// parse command line input
bool parse_args(int argc, char **argv) {
  // // Read arguments from shell input
  // if (argc < 2) {
  //   cerr << "input error: expecting arguments." << endl;
  //   return false;
  // }
  // ds_name = argv[1];
  // // if requesting help, print usage
  // if ((strcmp(argv[1], "-help") == 0) ||
  //     (strcmp(argv[1], "-h") == 0))
  //   return false;
  // // default arguments
  // range.push_back(-1.0);
  // range.push_back(1.0);
  // // loop over arguments
  // for (int i = 2; i < argc; ++i) {
  //   try {
  //     if (strcmp(argv[i], "-info") == 0) {
  // 	info = true;
  //     } else if (strcmp(argv[i], "-size") == 0) {
  // 	size = true;
  //     } else if (strcmp(argv[i], "-dims") == 0) {
  // 	++i; if (i >= argc) throw 0;
  // 	idxdim d;
  // 	string s = argv[i];
  // 	int k = 0;
  // 	while (s.size()) {
  // 	  uint j;
  // 	  for (j = 0; j < s.size(); ++j)
  // 	    if (s[j] == 'x')
  // 	      break ;
  // 	  string s0 = s.substr(0, j);
  // 	  if (j >= s.size())
  // 	    s = "";
  // 	  else
  // 	    s = s.substr(j + 1, s.size());
  // 	  d.insert_dim(k++, atoi(s0.c_str()));
  // 	}
  // 	dims = d;
  //     } else if (strcmp(argv[i], "-range") == 0) {
  // 	range.clear();
  // 	++i; if (i >= argc) throw 0;
  // 	string s = argv[i];
  // 	int k = 0;
  // 	while (s.size()) {
  // 	  uint j;
  // 	  for (j = 0; j < s.size(); ++j)
  // 	    if (s[j] == ',')
  // 	      break ;
  // 	  string s0 = s.substr(0, j);
  // 	  if (j >= s.size())
  // 	    s = "";
  // 	  else
  // 	    s = s.substr(j + 1, s.size());
  // 	  range.push_back(atof(s0.c_str()));
  // 	  k++;
  // 	}
  //     }
  //   } catch (int err) {
  //     cerr << "input error: ";
  //     switch (err) {
  //     case 0: cerr << "expecting string after " << argv[i-1]; break;
  //     case 1: cerr << "expecting integer after " << argv[i-1]; break;
  //     case 2: cerr << "unknown parameter " << argv[i-1]; break;
  //     case 3: cerr << "unknown channel mode " << argv[i-1]; break;
  //     default: cerr << "undefined error";
  //     }
  //     cerr << endl << endl;
  //     return false;
  //   }
  // }
  return true;
}

unsigned int draw_layer(idx<float> &layer, const char *s,
			unsigned int h, unsigned int w) {
#ifdef __GUI__
  draw_matrix(layer, s, h, w, 1.0, 1.0, (float)-1.0, (float)1.0);
#endif
  return layer.dim(1) + 3;
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) { 
#else
int main(int argc, char **argv) {
#endif
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  
#ifndef __GUI__ // return error if gui not enabled
  cerr << "warning: QT gui libraries not available, install them and recompile"
       << endl;
#else
  idx<t_data> frame;
  ostringstream dir, fname;
  int capcnt = 0;
  dir << "captured_" << tstamp();
  //camera_opencv<t_data> cam(-1);
  camera_opencv<t_data> cam(-1, 192, 256);
  while (!cam.empty()) {
    frame = cam.grab();
    // crop frame
    frame = frame.narrow(0, 128, 0);
    frame = frame.narrow(1, 196, 0);
    // display
    disable_window_updates();
    clear_window();
    draw_matrix(frame);
    // draw lines grid on frame
    uint sz = 64;
    uint szline = sz / 4;
    for (uint h0 = 0; h0 <= frame.dim(0) - szline; h0 += szline)
      draw_box(h0, 0, sz, frame.dim(1), 0, 255, 0);
    // draw grid on frame
    for (int h0 = 0; h0 <= frame.dim(0) - sz; h0 += sz)
      for (int w0 = 0; w0 <= frame.dim(1) - sz; w0 += sz)
	draw_box(h0, w0, sz, sz, 0, 0, 255);
    // saving
    if (gui.pop_key_pressed() == Qt::Key_Space) {
      mkdir_full(dir.str().c_str());
      fname.str("");
      fname << dir.str() << "/" << "frame_" << setw(5) << setfill('0')
	    << capcnt++ << ".mat";
      save_matrix(frame, fname.str());
      cout << "saved " << fname.str() << endl;
    }
    enable_window_updates();
  }
#endif 
  return 0;
}
