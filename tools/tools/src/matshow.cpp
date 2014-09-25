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

#include "libidx.h"
#include "libeblearn.h"
#include "tools_utils.h"
#include "configuration.h"
#include "netconf.h"
#include "eblapp.h"

//#define NOCONSOLE

#define MBOX_ERROR_TITLE "MatShow error"
#ifdef __WINDOWS__
#ifdef NOCONSOLE
#define  ERROR_MSG(err) MessageBox(NULL, err, MBOX_ERROR_TITLE, MB_OK)
#else
#define  ERROR_MSG(err) cerr << err << endl
#endif /* NOCONSOLE */
#else
#define ERROR_MSG(err) cerr << err << endl
#endif

#ifdef __GUI__
#include "libidxgui.h"
#include "libeblearngui.h"
// #include "defines_windows.h"
#endif

////////////////////////////////////////////////////////////////
// global parameters

#ifdef __GUI__
layers_gui lg;
#endif

bool show_info = false;
bool show_help = false;
bool autorange = false;
bool fixed_range = false;
bool explore = false; // explore working dir for other images
bool video = false; // show and save all frames
bool print = false; // print matrix values
bool show_filename = false;
uint nh = 1;
uint nw = 1;
configuration *conf = NULL;
float zoom = 1.0;
vector<double> range;
int chans = -1; // -1: show all chans, 0: chan 0 only, etc
uint maxwidth = 1000;
bool interleaved = false;
bool save_individually = false;

////////////////////////////////////////////////////////////////
// interface

// print command line usage
void print_usage() {
  cout << "Usage: ./matshow <mat or image file(s)> [OPTIONS]" << endl;
  cout << " e.g.: ./matshow *.mat" << endl;
  cout << " e.g.: ./matshow *.jpg" << endl;
  cout << " e.g.: ./matshow *.mat -conf best.conf -zoom 2.0" << endl;
  cout << "Options are:" << endl;
  cout << "  -conf <filename> (show a trained network's weights)" << endl;
  cout << "  -zoom <factor> (zoom on weights by this factor)" << endl;
  cout << "  -range <range, e.g.: -1,1>: range to map to 0..255 for display."
       << endl;
  cout << "  -video (display and save all input images in a video_<timestamp>"
       << " directory, than can later be compile into a video)" << endl;
  cout << "  -filename (display filename within drawing area)" << endl;
  cout << "  -print (print all values of current matrix)" << endl;
  cout << "  -interleaved (channels are in dimension 0)" << endl;
  cout << "  -save_individually (save each sub-matrix individually if midx)"
       << endl;
  cout << "  -diff <other.mat> (prints sum of differences between 2 matrices"
       << " with same dimensions)" << endl;
}

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 2) {
    ERROR_MSG("input error: expecting arguments.");
    print_usage();
    return false;
  }
  // if requesting help, print usage
  if ((strcmp(argv[1], "-help") == 0) ||
      (strcmp(argv[1], "-h") == 0)) {
    print_usage();
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////////////
// gui

template <typename T>
int display_net(list<string>::iterator &ifname,
		bool signd, bool load, list<string> *mats) {
  // create network
  idx<ubyte> classes(1,1);
  load_matrix<ubyte>(classes, conf->get_cstring("classes"));
  answer_module<T,T,T> *answer = create_answer<T,T,T>(*conf, classes.dim(0));
  if (!answer) eblerror("no answer module found");
  uint noutputs = answer->get_nfeatures();
  parameter<T> theparam;
  intg thick;
  module_1_1<T> *net = create_network<T>(theparam, *conf, thick, noutputs);
  cout << "Network parameters: " << theparam << endl;
  vector<string> weights =
      string_to_stringvector(conf->get_string("weights_file"));
  // loading weights
  theparam.load_x(weights);
  // displaying internals
  uint h0 = 0, w0 = 0;
#ifdef __GUI__
  disable_window_updates();
  clear_window();
  if (show_filename) {
    gui << at(h0, w0) << black_on_white() << ifname->c_str();
    h0 += 16;
  }
  if (autorange) // automatic range
    lg.display_internals(*net, h0, w0, zoom, (T) 0, (T) 0, maxwidth);
  else // fixed range
    lg.display_internals(*net, h0, w0, zoom, (T) range[0], (T) range[1],
			 maxwidth);
  // update title
  string title;
  title << "matshow: " << ebl::basename(ifname->c_str());
  set_window_title(title.c_str());
  enable_window_updates();
#endif
  return 1;
}

template <typename T>
int display(list<string>::iterator &ifname,
	    bool signd, bool load, list<string> *mats) {
  //cout << "displaying " << ifname->c_str() << endl;
  // conf mode
  if (conf)
    return display_net<T>(ifname, signd, load, mats);
  // mat mode
  if (is_matrix(ifname->c_str())) {
    idxdim d = get_matrix_dims(ifname->c_str());
    if (interleaved)
      d.shift_dim(0, 2);
    if (save_individually || print
        || !(d.order() == 2  || (d.order() == 3 &&
                                 (d.dim(2) == 1 || d.dim(2) == 3)))) {
      // this is probably not an image, just display info and print matrix
      string type;
      get_matrix_type(ifname->c_str(), type);
      idx<T> m = load_matrix<T>(ifname->c_str());
      cout << "Matrix " << ifname->c_str() << " is of type " << type
	   << " with dimensions " << d << " (min " << idx_min(m)
	   << ", max " << idx_max(m) << ", mean " << idx_mean(m)
	   << "):" << endl;
      m.print();
      if (has_multiple_matrices(ifname->c_str())) {
	midx<T> ms = load_matrices<T>(ifname->c_str(), true);
	// saving sub-matrices
	if (save_individually) {
	  cout << "Saving each sub-matrix of " << *ifname << " individually..."
	       << endl;
	  save_matrices_individually(ms, *ifname, true);
	}
	// printing sub-matrices
	cout << "This file contains " << m << " matrices: ";
	if (ms.order() == 1) {
	  for (intg i = 0; i < ms.dim(0); ++i) {
	    idx<T> tmp = ms.mget(i);
	    cout << tmp.info() << " ";
	  }
	} else if (ms.order() == 2) {
	  for (intg i = 0; i < ms.dim(0); ++i) {
	    for (intg j = 0; j < ms.dim(1); ++j) {
	      idx<T> tmp = ms.mget(i, j);
	      cout << tmp.info() << " ";
	    }
	    cout << endl;
	  }
	}
	cout << endl;
      } else
	cout << "This is a single-matrix file." << endl;
      return 0;
    }
  }
  // image mode
  int loaded = 0;
  static idx<T> mat;
  uint h = 0, w = 0, rowh = 0, maxh = 0;
  list<string>::iterator fname = ifname;
#ifdef __GUI__
  disable_window_updates();
  clear_window();
  if (show_filename) {
    gui << at(h, w) << black_on_white() << ifname->c_str();
    h += 16;
  }
#endif
  maxh = h;
  for (uint i = 0; i < nh; ++i) {
    rowh = maxh;
    for (uint j = 0; j < nw; ++j) {
      if (fname == mats->end())
	fname = mats->begin();
      try {
	//      if (load)
	mat = load_image<T>(*fname);
	if (print)
	  cout << *fname << ": " << mat << endl << mat.str() << endl;
	// show only some channels
	if (chans >= 0)
	  mat = mat.select(2, chans);
	loaded++;
	maxh = (std::max)(maxh, (uint) (rowh + mat.dim(0)));
	T min = 0, max = 0;
#ifdef __GUI__
	if (autorange || signd) {
	  if (autorange) {
	    min = idx_min(mat);
	    max = idx_max(mat);
	  } else if (signd) {
	    T matmin = idx_min(mat);
	    if ((double)matmin < 0) {
	      min = -1;
	      max = -1;
	    }
	  }
	  draw_matrix(mat, rowh, w, zoom, zoom, min, max);
	} else
	  draw_matrix(mat, rowh, w, zoom, zoom, (T) range[0], (T) range[1]);
#endif
	w += mat.dim(1) + 1;
      } catch(string &err) {
	ERROR_MSG(err.c_str());
      }
      fname++;
      if (fname == ifname)
	break ;
    }
    if (fname == ifname)
      break ;
    maxh++;
    w = 0;
  }
#ifdef __GUI__
  // info
  if (show_info) {
    set_text_colors(0, 0, 0, 255, 255, 255, 255, 200);
    gui << mat;
    gui << at(15, 0) << *fname;
    gui << at(29, 0) << "min: " << idx_min(mat) << " max: " << idx_max(mat);
  }
  // help
  if (show_help) {
    h = 0;
    w = 0;
    uint hstep = 14;
    set_text_colors(0, 0, 255, 255, 255, 255, 255, 200);
    gui << at(h, w) << "Controls:"; h += hstep;
    set_text_colors(0, 0, 0, 255, 255, 255, 255, 200);
    gui << at(h, w) << "Right/Space: next image"; h += hstep;
    gui << at(h, w) << "Left/Backspace: previous image"; h += hstep;
    gui << at(h, w) << "i: image info"; h += hstep;
    gui << at(h, w) << "a: auto-range (use min and max as range)"; h += hstep;
    gui << at(h, w) << "x/z: show more/less images on width axis"; h += hstep;
    gui << at(h, w) << "y/t: show more/less images on height axis"; h += hstep;
    gui << at(h, w) << "0,1,2: show channel 0, 1 or 2 only"; h += hstep;
    gui << at(h, w) << "9: show alls channels"; h += hstep;
    gui << at(h, w) << "h: help";
  }
  // update title
  string title;
  title << "matshow: " << ebl::basename(ifname->c_str());
  set_window_title(title.c_str());
  enable_window_updates();
#endif
  return loaded;
}

//! Retrieve type so that we know if we can look
//! for negative values when estimating range.
int load_display(list<string>::iterator &ifname,
		 bool load, list<string> *mats) {
  try {
    switch (get_matrix_type((*ifname).c_str())) {
      case MAGIC_BYTE_MATRIX:
      case MAGIC_UBYTE_VINCENT:
        return display<ubyte>(ifname, false, load, mats);
        break ;
      case MAGIC_INTEGER_MATRIX:
      case MAGIC_INT_VINCENT:
        return display<int>(ifname, true, load, mats);
        break ;
      case MAGIC_FLOAT_MATRIX:
      case MAGIC_FLOAT_VINCENT:
        return display<float>(ifname, true, load, mats);
        break ;
      case MAGIC_DOUBLE_MATRIX:
      case MAGIC_DOUBLE_VINCENT:
        return display<double>(ifname, true, load, mats);
        break ;
      case MAGIC_LONG_MATRIX:
        return display<long>(ifname, true, load, mats);
        break ;
      case MAGIC_UINT_MATRIX:
#ifndef __WINDOWS__
        return display<uint>(ifname, false, load, mats);
#else
	eblerror("matshow for UINT disabled in Windows");
#endif
	break ;
      default: // not a matrix, try as regular float image
        return display<float>(ifname, true, load, mats);
    }
  } eblcatcherror();
  return 0;
}

////////////////////////////////////////////////////////////////
// main
typedef float Tnet;

#ifdef __GUI__
#ifdef NOCONSOLE
NOCONSOLE_MAIN_QTHREAD(int, argc, char**, argv) {
#else
  MAIN_QTHREAD(int, argc, char**, argv) {
#endif /* NOCONSOLE */
#else
    int main(int argc, char **argv) {
      ERROR_MSG("QT not found, install and recompile.");
#endif /* __GUI__ */
      try {
        if (!parse_args(argc, argv))
          return -1;
        // variables
        range.push_back(-1.0); // default range min
        range.push_back(1.0); // default range max
        string conf_fname, diff_fname;
        // show mat images
        list<string> *argmats = new list<string>();
        list<string>::iterator i;
        for (int i = 1; i < argc; ++i) {
          // check for options
          try {
            if (!strcmp(argv[i], "-conf")) {
              ++i; if (i >= argc) throw 0;
              conf_fname = argv[i];
            } else if (!strcmp(argv[i], "-diff")) {
              ++i; if (i >= argc) throw 0;
              diff_fname = argv[i];
            } else if (!strcmp(argv[i], "-zoom")) {
              ++i; if (i >= argc) throw 0;
              zoom = (float) atof(argv[i]);
            } else if (!strcmp(argv[i], "-maxwidth")) {
              ++i; if (i >= argc) throw 0;
              maxwidth = (int) atoi(argv[i]);
            } else if (!strcmp(argv[i], "-video")) {
              video = true;
            } else if (!strcmp(argv[i], "-print")) {
              print = true;
            } else if (!strcmp(argv[i], "-interleaved")) {
              interleaved = true;
            } else if (!strcmp(argv[i], "-save_individually")) {
              save_individually = true;
            } else if (!strcmp(argv[i], "-filename")) {
              show_filename = true;
            } else if (!strcmp(argv[i], "-range")) {
              range.clear();
              ++i; if (i >= argc) throw 0;
              string s = argv[i];
              int k = 0;
              while (s.size()) {
                uint j;
                for (j = 0; j < s.size(); ++j)
                  if (s[j] == ',')
                    break ;
                string s0 = s.substr(0, j);
                if (j >= s.size())
                  s = "";
                else
                  s = s.substr(j + 1, s.size());
                range.push_back(atof(s0.c_str()));
                k++;
              }
              cout << "Fixing input range to " << range[0] << " .. "
                   << range[1] << endl;
              fixed_range = true;
            }
            // enqueue file names
            else {
              cout << argv[i] << endl;
              argmats->push_back(argv[i]);
            }
          } catch (int err) {
            cerr << "input error: ";
            switch (err) {
              case 0: cerr << "expecting string after " << argv[i-1]; break;
              case 1: cerr << "expecting integer after " << argv[i-1]; break;
              case 2: cerr << "unknown parameter " << argv[i-1]; break;
              case 3: cerr << "unknown channel mode " << argv[i-1]; break;
              default: cerr << "undefined error";
            }
            cerr << endl << endl;
            return false;
          }
        }

        // load configuration in conf mode
        if (conf_fname.size() > 0) {
          conf = new configuration(conf_fname);
          if (!conf->exists("root2")) {
            string dir = dirname(conf_fname.c_str());
            cout << "Looking for trained files in: " << dir << endl;
            conf->set("root2", dir.c_str());
            conf->resolve();
          }
          // enable auto range by default in conf mode
          if (!fixed_range)
            autorange = true;
        }
        // diff mode, just print matrices differences
        if (!diff_fname.empty()) {
          i = argmats->begin();
          idx<double> m1 = load_matrix<double>((*i).c_str());
          idx<double> m2 = load_matrix<double>(diff_fname);
          if (!m1.same_dim(m2.get_idxdim())) {
            eblerror("cannot compare matrices of different dimensions " << m1
                     << " and " << m2);
            return -1;
          }
          idx_sub(m1, m2);
          double sum = idx_sum(m1);
          cout << "sum(m1 - m2) = " << sum << endl;
          return 0;
        }

        // display first matrix/image
        i = argmats->begin();
        if (!video) {
          if (!load_display(i, true, argmats)) {
            ERROR_MSG("failed to load image(s)");
            return -1;
          }
        }
        // explore working directory for more images only if a
        // single file was passed
        if (argmats->size() == 1)
          explore = true;

#ifdef __GUI__
#ifdef __BOOST__
        // list all other mat files in image directory
        string dir = argv[1];
        string imgname, tmpname;
        size_t pos = dir.find_last_of('/');
        if (pos == string::npos) {
          imgname = dir;
          dir = "./";
        } else { // it contains a directory
          imgname = dir.substr(pos + 1, dir.size() - pos + 1);
          dir = dir.substr(0, pos);
        }
        list<string> *mats = argmats;
        if (explore)
          mats = find_fullfiles(dir, IMAGE_PATTERN_MAT,
                                NULL, true, false);
        // video mode
        if (video) {
          string out;
          out << "video_" << tstamp();
          mkdir_full(out);
          uint j = 0;
          for (i = mats->begin(); i != mats->end(); ++i, ++j) {
            cout << "video mode: displaying " << *i << endl;
            if (!load_display(i, true, mats)) {
              ERROR_MSG("failed to load image(s)");
              return -1;
            }
            millisleep(3000);
            ostringstream fname;
            fname << out << "/" << setfill('0') << setw(4) << j << "_"
                  << ebl::basename(i->c_str()) << ".png";
            save_window(fname.str().c_str());
            cout << "video mode: saved " << fname.str() << endl;
          }
        } else { // interactive mode
          if ((mats) && (mats->size() >= 1)) {
            // find current position in this list
            for (i = mats->begin(); i != mats->end(); ++i) {
              tmpname = ebl::basename(i->c_str());
              if (!imgname.compare(tmpname))
                break ;
            }
            if (i == mats->end())
              i = mats->begin();
            // loop and wait for key pressed
            while (1) {
              millisleep(50);
              int key = gui.pop_key_pressed();
              // next/previous images only if not everuything is already displayed
              if (mats->size() > nh * nw) {
                if ((key == Qt::Key_Space) || (key == Qt::Key_Right)) {
                  // show next image
                  for (uint k = 0; k < nw * nh; ++k) {
                    i++;
                    if (i == mats->end()) {
                      i = mats->begin();
                    }
                  }
                  load_display(i, true, mats);
                } else if ((key == Qt::Key_Backspace) || (key == Qt::Key_Left)) {
                  // show previous image
                  for (uint k = 0; k < nw * nh; ++k) {
                    if (i == mats->begin())
                      i = mats->end();
                    i--;
                  }
                  load_display(i, true, mats);
                }
              }
              if (key == Qt::Key_I) {
                // show info
                show_info = !show_info;
                if (show_info)
                  show_help = false;
                load_display(i, false, mats);
              } else if (key == Qt::Key_A) {
                // enable autorange
                autorange = !autorange;
                if (autorange)
                  cout << "Enabling automatic input range." << endl;
                else
                  cout << "Disabling automatic input range." << endl;
                load_display(i, false, mats);
              } else if (key == Qt::Key_H) {
                // show help
                show_help = !show_help;
                if (show_help)
                  show_info = false;
                load_display(i, false, mats);
              } else if (key == Qt::Key_Y) {
                // increase number of images shown on height axis
                if (nh * nw < mats->size())
                  nh++;
                load_display(i, false, mats);
              } else if (key == Qt::Key_T) {
                // decrease number of images shown on height axis
                nh = (std::max)((uint) 1, nh - 1);
                load_display(i, false, mats);
              } else if (key == Qt::Key_X) {
                // increase number of images shown on width axis
                if (nh * nw < mats->size())
                  nw++;
                load_display(i, false, mats);
              } else if (key == Qt::Key_Z) {
                // decrease number of images shown on width axis
                nw = (std::max)((uint) 1, nw - 1);
                load_display(i, false, mats);
              } else if (key == Qt::Key_0) {
                // show only channel 0
                chans = 0;
                load_display(i, false, mats);
                cout << "Showing channel 0 only." << endl;
              } else if (key == Qt::Key_1) {
                // show only channel 1
                chans = 1;
                load_display(i, false, mats);
                cout << "Showing channel 1 only." << endl;
              } else if (key == Qt::Key_2) {
                // show only channel 2
                chans = 2;
                load_display(i, false, mats);
                cout << "Showing channel 2 only." << endl;
              } else if (key == Qt::Key_9) {
                // show all channels
                chans = -1;
                load_display(i, false, mats);
                cout << "Showing alls channel." << endl;
              }
            }
          }
        }
        // free objects
        if (mats) delete mats;
#endif /* __BOOST__ */
#endif /* __GUI__ */
        if (argmats) delete argmats;
        if (conf) delete conf;
      } catch(string &err) {
        ERROR_MSG(err.c_str());
      }
      millisleep(500); // TODO: this lets time for window to open, fix this issue
      return 0;
    }
