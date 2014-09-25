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

#include <cstdlib>

#ifdef __GUI__
#include "libidxgui.h"
#endif

#include "ebl_preprocessing.h"
#include "dataset.h"
#include "pascal_dataset.h"
#include "pascalbg_dataset.h"
#include "pascalclear_dataset.h"
#include "pascalfull_dataset.h"
#include "grid_dataset.h"
#include "tools_utils.h"
#include "patch_dataset.h"
#include "netconf.h"
#include "ebl_defines.h"
#include "eblapp.h"

using namespace std;

//! A class containing preprocessing parameters.
class pp {
 public:
  pp(midxdim &dims_, const char *ppc, midxdim &kersz,
     const char *rsz, bool keep, int lp, vector<double> *fov,
     midxdim *fov_size, bool gn, bool divnorm_, bool divnorm2_,
     bool colornorm_, bool colornorm_across_, double bbhf, double bbwf)
      : dims(dims_), ppchan(ppc), resize_method(rsz), kernel_sz(kersz),
        keep_aspect_ratio(keep), lpyramid(lp), fovea(fov),
        fovea_scales_size(fov_size), global_norm(gn),
	divnorm(divnorm_), divnorm2(divnorm2_), colornorm(colornorm_),
	colornorm_across(colornorm_across_), bboxhfact(bbhf), bboxwfact(bbwf) {
  };
  virtual ~pp() {}

  midxdim dims;
  string ppchan, resize_method;
  midxdim kernel_sz;
  bool keep_aspect_ratio;
  int lpyramid;
  vector<double> *fovea;
  midxdim *fovea_scales_size;
  string name;
  bool global_norm, divnorm, divnorm2, colornorm, colornorm_across;
  double bboxhfact, bboxwfact;
};

// global variables ////////////////////////////////////////////////////////////

string		images_root	 = ".";
string		image_pattern	 = IMAGE_PATTERN_MAT;
string		channels_mode	 = "RGB";
bool            preprocessing    = false;
bool		display		 = false;
bool		stereo		 = false;
bool		ignore_difficult = false;
bool		ignore_bbox      = false;
bool		ignore_truncated = false;
bool		ignore_occluded  = false;
bool		shuffle		 = false;
bool		scale_mode	 = false;
vector<double>  scales;
bool		fovea_mode = false;
vector<double>  fovea_scales;
midxdim         fovea_scales_size;
uint            lpyramid         = 0; //!< # scales in laplacian pyramid
bool            keep_aspect_ratio= true;
string		stereo_lpattern	 = "_L";
string		stereo_rpattern	 = "_R";
string		outdir		 = ".";
string		tmpout   	 = "";
string		dataset_name	 = "ds";
intg		maxperclass	 = 0;	// 0 means no limitation
intg            maxdata          = 0;	// 0 means no limitation
unsigned int	mexican_hat_size = 0;
midxdim 	kernelsz;	 // kernel size for pp
int		deformations	 = -1;	// <= means no deformations
string		stype		 = "regular";
string          resize           = "bilinear";
string		precision	 = "float";
uint		sleep_delay	 = 0;	// sleep between frames displayed in ms
midxdim         outdims;	        // dimensions of output sample
bool		outdims_set	 = false;
idxdim          mindims;	        // minimum dimensions in input
bool		mindims_set	 = false;
idxdim          maxdims;	        // maximum dimensions in input
bool		maxdims_set	 = false;
vector<string>  exclude;
vector<string>  include;
bool            usepose          = false; // use pose if given
bool            useparts         = false; // use parts if given
bool            partsonly        = false; // use parts only if given
string          save             = DYNSET_SAVE;// save into lush dataset format
bool            save_set         = true;
bool            individual_save  = true;
bool            separate_layers_save = false;
string          load             = ""; // dataset to load
bool            load_set         = false;
float           bboxfact         = 1.0;
float           bboxhfact        = 1.0;
float           bboxwfact        = 1.0;
float           bbox_woverh      = 1.0;
bool            bboxfact_set     = false;
bool            bboxhfact_set     = false;
bool            bboxwfact_set     = false;
bool            bbox_woverh_set  = false;
bool            force_label      = false;
bool            nopadded         = false;
string          label            = "";
idxdim          gridsz;
string          annotations;
string		ignore_path;
uint            tjitter_step = 0; // translation step in pixels
uint            tjitter_hmin = 0; // translation min height in pixels
uint            tjitter_hmax = 0; // translation max height in pixels
uint            tjitter_wmin = 0; // translation min height in pixels
uint            tjitter_wmax = 0; // translation max height in pixels
uint            sjitter_steps    = 0; // number of possible scale jitters
float           sjitter_min      = 1; // min scale jitter
float           sjitter_max      = 1; // max scale jitter
uint            rjitter_steps    = 0; // number of possible rotation jitters
float           rjitter          = 0; // rotation jitter range around original
uint            njitter          = 0; // number of jitters (including original)
bool            wmirror          = false; // add symmetry with vertical axis
float           minvisibility    = 0.0; // minimum visible / true area ratio
bool            minvisibility_set= false; // minvisibility has been set or not
idxdim          minborders;	        // minimum distance to borders for bbs
bool		minborders_set	 = false;
bool            save_display     = false;
idxdim          save_display_dims;
string          save_display_dir;
float           max_aspect_ratio = 0.0; // maximum width/height aspect ratio
bool            max_aspect_ratio_set = false; // has been set or not
float           min_aspect_ratio = 0.0; // minimum width/height aspect ratio
bool            min_aspect_ratio_set = false; // has been set or not
bool            nopp = false; // force disabling of preprocessing
float           max_jitt_match = 0.0; // maximum jitter match with others.
bool            stats_mode = false; // extract statistics
bool            planar_loading = false;
vector<pp*>     pp_params;
bool            global_norm = true; // preprocessing uses global norm
bool            nocount = false; // Do not count samples if true.
bool            divnorm = true; // divisive normalization
bool            divnorm2 = false; // 2nd divisive normalization
bool            colornorm = false; // color normalization
bool            colornorm_across = true; // color norm across channels
bool            videobox = false; // read bboxes in video sequences
uint            videobox_stride; // the stride for videobox
uint            videobox_n; // number of images to read-ahead

// command line ////////////////////////////////////////////////////////////////

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  images_root = argv[1];
  outdir = images_root;
  // if requesting help, print usage
  if ((strcmp(argv[1], "-help") == 0) ||
      (strcmp(argv[1], "-h") == 0))
    return false;
  // loop over arguments
  for (int i = 2; i < argc; ++i) {
    try {
      if (strcmp(argv[i], "-channels") == 0) {
	++i; if (i >= argc) throw 0;
	channels_mode = argv[i];
	if ((strcmp(channels_mode.c_str(), "RGB") == 0) ||
	    (strcmp(channels_mode.c_str(), "YnUV") == 0) ||
	    (strcmp(channels_mode.c_str(), "YpUV") == 0) ||
	    (strcmp(channels_mode.c_str(), "YnUVn") == 0) ||
	    (strcmp(channels_mode.c_str(), "YUVn") == 0) ||
	    (strcmp(channels_mode.c_str(), "RGBn") == 0) ||
	    (strcmp(channels_mode.c_str(), "YUV") == 0) ||
	    (strcmp(channels_mode.c_str(), "HSV") == 0) ||
	    (strcmp(channels_mode.c_str(), "Yp") == 0))
	  ;
	else throw 3;
	preprocessing = true;
      } else if (strcmp(argv[i], "-image_pattern") == 0) {
	++i; if (i >= argc) throw 0;
	image_pattern = argv[i];
      } else if (strcmp(argv[i], "-annotations") == 0) {
	++i; if (i >= argc) throw 0;
	annotations = argv[i];
      } else if (strcmp(argv[i], "-tmpout") == 0) {
	++i; if (i >= argc) throw 0;
	tmpout = argv[i];
      } else if (strcmp(argv[i], "-ignore_path") == 0) {
	++i; if (i >= argc) throw 0;
	ignore_path = argv[i];
      } else if (strcmp(argv[i], "-disp") == 0) {
	display = true;
      } else if (strcmp(argv[i], "-nopp") == 0) {
	nopp = true;
      } else if (strcmp(argv[i], "-addpp") == 0) {
	// add current preprocessing configuration
	if (!outdims_set)
	  eblerror("output dimensions should be set before adding preprocessing"
		   << " (using -dims options)");
	pp_params.push_back(new pp(outdims,
				   channels_mode.c_str(), kernelsz,
				   resize.c_str(), keep_aspect_ratio, lpyramid,
                                   &fovea_scales, &fovea_scales_size,
                                   global_norm, divnorm, divnorm2, colornorm,
                                   colornorm_across, bboxhfact, bboxwfact));
      } else if (strcmp(argv[i], "-wmirror") == 0) {
	wmirror = true;
      } else if (strcmp(argv[i], "-ignore_difficult") == 0) {
	ignore_difficult = true;
      } else if (strcmp(argv[i], "-ignore_bbox") == 0) {
	ignore_bbox = true;
      } else if (strcmp(argv[i], "-ignore_truncated") == 0) {
	ignore_truncated = true;
      } else if (strcmp(argv[i], "-ignore_occluded") == 0) {
	ignore_occluded = true;
      } else if (strcmp(argv[i], "-shuffle") == 0) {
	shuffle = true;
      } else if (strcmp(argv[i], "-usepose") == 0) {
	usepose = true;
      } else if (strcmp(argv[i], "-useparts") == 0) {
	useparts = true;
      } else if (strcmp(argv[i], "-partsonly") == 0) {
	partsonly = true;
      } else if (strcmp(argv[i], "-stereo") == 0) {
	stereo = true;
      } else if (strcmp(argv[i], "-stereo_lpattern") == 0) {
	++i; if (i >= argc) throw 0;
	stereo_lpattern = argv[i];
      } else if (strcmp(argv[i], "-stereo_rpattern") == 0) {
	++i; if (i >= argc) throw 0;
	stereo_rpattern = argv[i];
      } else if (strcmp(argv[i], "-outdir") == 0) {
	++i; if (i >= argc) throw 0;
	outdir = argv[i];
	if (!strcmp(outdir.c_str(), images_root.c_str()))
	  eblerror("Using the same directory for inputs and outputs "
		   << "might produce conflicts, use a different output "
		   << "directory.");
      } else if (strcmp(argv[i], "-save") == 0) {
	++i; if (i >= argc) throw 0;
	save = argv[i];
	save_set = true;
      } else if (strcmp(argv[i], "-nosave") == 0) {
	save_set = false;
      } else if (strcmp(argv[i], "-noindividualsave") == 0) {
	individual_save = false;
      } else if (strcmp(argv[i], "-save_layers_separately") == 0) {
	separate_layers_save = true;
      } else if (strcmp(argv[i], "-load") == 0) {
	++i; if (i >= argc) throw 0;
	load = argv[i];
	load_set = true;
      } else if (strcmp(argv[i], "-stats") == 0) {
	stats_mode = true;
      } else if (strcmp(argv[i], "-planar_loading") == 0) {
	planar_loading = true;
      } else if (strcmp(argv[i], "-type") == 0) {
	++i; if (i >= argc) throw 0;
	stype = argv[i];
      } else if (strcmp(argv[i], "-precision") == 0) {
	++i; if (i >= argc) throw 0;
	precision = argv[i];
      } else if (strcmp(argv[i], "-resize") == 0) {
	++i; if (i >= argc) throw 0;
	resize = argv[i];
      } else if (strcmp(argv[i], "-dname") == 0) {
	++i; if (i >= argc) throw 0;
	dataset_name = argv[i];
      } else if (strcmp(argv[i], "-exclude") == 0) {
	++i; if (i >= argc) throw 0;
	exclude.push_back(argv[i]);
      } else if (strcmp(argv[i], "-include") == 0) {
	++i; if (i >= argc) throw 0;
	include.push_back(argv[i]);
      } else if (strcmp(argv[i], "-maxperclass") == 0) {
	++i; if (i >= argc) throw 1;
	maxperclass = atoi(argv[i]);
      } else if (strcmp(argv[i], "-maxdata") == 0) {
	++i; if (i >= argc) throw 1;
	maxdata = atoi(argv[i]);
      } else if (strcmp(argv[i], "-kernelsz") == 0) {
	++i; if (i >= argc) throw 1;
	kernelsz = string_to_idxdimvector(argv[i]);
      } else if (strcmp(argv[i], "-sleep") == 0) {
	++i; if (i >= argc) throw 1;
	sleep_delay = atoi(argv[i]);
      } else if (strcmp(argv[i], "-mexican_hat_size") == 0) {
	++i; if (i >= argc) throw 1;
	mexican_hat_size = atoi(argv[i]);
      } else if (strcmp(argv[i], "-deformations") == 0) {
	++i; if (i >= argc) throw 1;
	deformations = atoi(argv[i]);
      } else if (strcmp(argv[i], "-dims") == 0) {
	++i; if (i >= argc) throw 0;
	outdims = string_to_idxdimvector(argv[i]);
	outdims_set = true;
	preprocessing = true;
      } else if (strcmp(argv[i], "-mindims") == 0) {
	++i; if (i >= argc) throw 0;
	mindims = string_to_idxdim(argv[i]);
	mindims_set = true;
      } else if (strcmp(argv[i], "-maxdims") == 0) {
	++i; if (i >= argc) throw 0;
	maxdims = string_to_idxdim(argv[i]);
	maxdims_set = true;
      } else if (strcmp(argv[i], "-minborders") == 0) {
	++i; if (i >= argc) throw 0;
	minborders = string_to_idxdim(argv[i]);
	minborders_set = true;
      } else if (strcmp(argv[i], "-savedisplay") == 0) {
	++i; if (i >= argc) throw 0;
	save_display_dir = argv[i];
	++i; if (i >= argc) throw 0;
	save_display_dims = string_to_idxdim(argv[i]);
	save_display = true;
      } else if (strcmp(argv[i], "-gridsz") == 0) {
	++i; if (i >= argc) throw 0;
	gridsz = string_to_idxdim(argv[i]);
      } else if (strcmp(argv[i], "-scales") == 0) {
	++i; if (i >= argc) throw 0;
	string s = argv[i];
	scales = string_to_doublevector(s);
	scale_mode = true;
	preprocessing = true;
      } else if (strcmp(argv[i], "-fovea_scales") == 0) {
	++i; if (i >= argc) throw 0;
	string s = argv[i];
	fovea_scales = string_to_doublevector(s);
        ++i; if (i >= argc) throw 0;
        string s1 = argv[i];
        fovea_scales_size = string_to_idxdimvector(s1.c_str(), ',', 'x');
	fovea_mode = true;
	preprocessing = true;
      } else if (strcmp(argv[i], "-fovea") == 0) {
	++i; if (i >= argc) throw 0;
	fovea_mode = (bool) atoi(argv[i]);
	if (!fovea_mode) {
	  fovea_scales.clear();
	  fovea_scales_size.clear();
	}
      } else if (strcmp(argv[i], "-lpyramid") == 0) {
	++i; if (i >= argc) throw 0;
	lpyramid = (uint) atoi(argv[i]);
      } else if (strcmp(argv[i], "-aspect_ratio") == 0) {
	++i; if (i >= argc) throw 0;
	keep_aspect_ratio = (bool) atoi(argv[i]);
      } else if (strcmp(argv[i], "-global_norm") == 0) {
	++i; if (i >= argc) throw 0;
	global_norm = (bool) atoi(argv[i]);
      } else if (strcmp(argv[i], "-divnorm") == 0) {
	++i; if (i >= argc) throw 0;
	divnorm = (bool) atoi(argv[i]);
      } else if (strcmp(argv[i], "-divnorm2") == 0) {
	++i; if (i >= argc) throw 0;
	divnorm2 = (bool) atoi(argv[i]);
      } else if (strcmp(argv[i], "-colornorm") == 0) {
	++i; if (i >= argc) throw 0;
	colornorm = (bool) atoi(argv[i]);
      } else if (strcmp(argv[i], "-colornorm_across") == 0) {
	++i; if (i >= argc) throw 0;
	colornorm_across = (bool) atoi(argv[i]);
      } else if (strcmp(argv[i], "-nocount") == 0) {
	nocount = true;
      } else if (strcmp(argv[i], "-njitter") == 0) {
	++i; if (i >= argc) throw 0;
	njitter = (uint) atoi(argv[i]);
      } else if (strcmp(argv[i], "-jitter_translation") == 0) {
	++i; if (i >= argc) throw 0;
	vector<uint> l = string_to_uintvector(argv[i]);
	if (l.size() != 5)
	  eblerror("expected 5 arguments to -jitter_translation");
	tjitter_step = l[0];
	tjitter_hmin = l[1];
	tjitter_hmax = l[2];
	tjitter_wmin = l[3];
	tjitter_wmax = l[4];
      } else if (strcmp(argv[i], "-jitter_rotation") == 0) {
	++i; if (i >= argc) throw 0;
	vector<double> l = string_to_doublevector(argv[i]);
	rjitter_steps = (uint) l[0];
	rjitter = l[1];
      } else if (strcmp(argv[i], "-jitter_scale") == 0) {
	++i; if (i >= argc) throw 0;
	vector<double> l = string_to_doublevector(argv[i]);
	if (l.size() != 3)
	  eblerror("expected 3 arguments to -jitter_scale");
	sjitter_steps = (uint) l[0];
	sjitter_min = l[1];
	sjitter_max = l[2];
      } else if (strcmp(argv[i], "-forcelabel") == 0) {
	++i; if (i >= argc) throw 0;
	label = argv[i];
	force_label = true;
      } else if (strcmp(argv[i], "-nopadded") == 0) {
	nopadded = true;
      } else if (strcmp(argv[i], "-bboxfact") == 0) {
	++i; if (i >= argc) throw 0;
	bboxfact = (float) atof(argv[i]);
	bboxhfact = bboxfact;
	bboxwfact = bboxfact;
	bboxfact_set = true;
      } else if (strcmp(argv[i], "-bboxhfact") == 0) {
	++i; if (i >= argc) throw 0;
	bboxhfact = (float) atof(argv[i]);
	bboxhfact_set = true;
      } else if (strcmp(argv[i], "-bboxwfact") == 0) {
	++i; if (i >= argc) throw 0;
	bboxwfact = (float) atof(argv[i]);
	bboxwfact_set = true;
      } else if (strcmp(argv[i], "-bbox_woverh") == 0) {
	++i; if (i >= argc) throw 0;
	bbox_woverh = (float) atof(argv[i]);
	bbox_woverh_set = true;
      } else if (strcmp(argv[i], "-minvisibility") == 0) {
	++i; if (i >= argc) throw 0;
	minvisibility = (float) atof(argv[i]);
	minvisibility_set = true;
      } else if (strcmp(argv[i], "-max_aspect_ratio") == 0) {
	++i; if (i >= argc) throw 0;
	max_aspect_ratio = (float) atof(argv[i]);
	max_aspect_ratio_set = true;
      } else if (strcmp(argv[i], "-min_aspect_ratio") == 0) {
	++i; if (i >= argc) throw 0;
	min_aspect_ratio = (float) atof(argv[i]);
	min_aspect_ratio_set = true;
      } else if (strcmp(argv[i], "-max_jitt_match") == 0) {
	++i; if (i >= argc) throw 0;
	max_jitt_match = (float) atof(argv[i]);
      } else if (strcmp(argv[i], "-videobox") == 0) {
	++i; if (i >= argc) throw 0;
	vector<uint> l = string_to_uintvector(argv[i]);
	if (l.size() != 2)
	  eblerror("expected 2 arguments to -videobox");
        videobox = true;
        videobox_n = l[0];
        videobox_stride = l[1];
      } else if ((strcmp(argv[i], "-help") == 0) ||
		 (strcmp(argv[i], "-h") == 0)) {
	return false;
      } else {
	cerr << "input error: unknown parameter: " << argv[i] << endl;
	return false;
      }
    } catch (int err) {
      cerr << "input error: ";
      switch (err) {
        case 0: cerr << "expecting string after " << argv[i-1]; break;
        case 1: cerr << "expecting integer after " << argv[i-1]; break;
        case 2: cerr << "unknown parameter " << argv[i-1]; break;
        case 3: cerr << "unknown channel mode " << channels_mode; break;
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
  cout << "Usage: ./dscompile <images_root> [OPTIONS]" << endl;
  cout << "Options are:" << endl;
  cout << "  -type <regular(default)|patch|pascal|pascalbg|pascalfull|grid>" << endl;
  cout << "     regular: compile images labeled by their top folder name"<<endl;
  cout << "     patch: extract random (position & scale) patches from images"
       << endl;
  cout << "     pascal: compile images labeled by xml files (PASCAL challenge)";
  cout << endl;
  cout << "     pascalbg: compile background images of PASCAL challenge"<< endl;
  cout << "     pascalclear: clear objects from original images of PASCAL "
       << "challenge"<< endl;
  cout << "     pascalfull: copy full original PASCAL images into outdir"<<endl;
  cout << "       (allows to exclude some classes, then call regular compiler)";
  cout << endl;
  cout << "     grid: extract non-overlapping cells from each image."
       << "       cell sizes are determined by -gridsz option"<<endl;
  cout << "  -precision <float(default)|double|ubyte>" << endl;
  cout << "  -tmpout <directory>" << endl
       << "     A directory to put temporary files." << endl;
  cout << "  -annotations <directory>" << endl;
  cout << "  -ignore_path <directory>" << endl
       << "     Path of ignored annotations files." << endl;
  cout << "  -image_pattern <pattern>" << endl;
  cout << "     default: " << IMAGE_PATTERN_MAT << endl;
  cout << "  -channels <channel>" << endl;
  cout << "     channels are: RGB (default), RGBn, YUV, HSV, Yn, YnUV, YnUVn,"
       << " YUVn (where is is a local normalization across channels)" << endl;
  cout << "  -disp (display extraction)" << endl;
  cout << "  -nopp (no preprocessing, i.e. no resizing or conversion)" << endl;
  cout << "  -addpp "
       << "    Add the current preprocessing config as one pp module." << endl;
  cout << "  -sleep <delay in ms> (sleep between frame display)" << endl;
  cout << "  -shuffle" << endl;
  cout << "  -usepose (separate classes with pose if available)" << endl;
  cout << "  -stereo" << endl;
  cout << "  -stereo_lpattern <pattern>" << endl;
  cout << "  -stereo_rpattern <pattern>" << endl;
  cout << "  -outdir <directory (default=images_root)>" << endl;
  cout << "  -load <dataset name> (this loads the dataset instead of compiling";
  cout << "     it from images found in root." << endl;
  cout << "  -stats" << endl
       << "     Extracts dataset statistics into a csv file." << endl;
  cout << "  -save <dynset(default)|dataset|mat|ppm|png|jpg|...>" << endl;
  cout << "     dynset: dynamically loadable dataset (more memory efficient)" << endl;
  cout << "     dataset: regular dataset (needs to entirely fit in memory)" << endl;
  cout << "  -nosave" << endl
       << "     Do not save as a single dataset (individual files still saved)" << endl;
  cout << "  -noindividualsave" << endl
       << "     Do not save individual samples." << endl;
  cout << "  -save_layers_separately" << endl
       << "     Save each layer of each sampe in separate files." << endl;
  cout << "  -dname <name>" << endl;
  cout << "  -maxperclass <integer>" << endl;
  cout << "  -maxdata <integer>" << endl;
  cout << "  -kernelsz <dims list, e.g.: 7x7 or 7x7,5x5,3x3>" << endl;
  cout << "  -mexican_hat_size <integer>" << endl;
  cout << "  -deformations <integer>" << endl;
  cout << "  -dims <dimensions (default: 96x96x3)>" << endl;
  cout << "  -mindims <dimensions (default: 1x1)>" << endl;
  cout << "     (exclude inputs for which one dimension is less than specified";
  cout << "  -maxdims <dimensions (e.g.: 100x100)>" << endl;
  cout << "     (exclude inputs for which one dimension is more than specified";
  cout << endl;
  cout << "  -scales <scales (e.g: 1.5,2,4)>" << endl;
  cout << "  -lpyramid <uint>" << endl
       << "     Input will be a Laplacian pyramid with <uint> scales." << endl;
  cout << "  -divnorm <bool |0,1: add divisive normalization to preprocessing>"
       << endl;
  cout << "  -divnorm2 <bool |0,1: add 2nd divisive normalization to "
       << " preprocessing>" << endl;
  cout << "  -colornorm <bool |0,1: add contrast normalization to color>"
       << endl;
  cout << "  -colornorm_across <bool |0,1: color contrast normalization is "
       << "across channels or independent for each channel>" << endl;
  cout << "  -aspect_ratio <bool |0: not keeping 1: keeping (default)>" << endl;
  cout <<"  -global_norm <bool |0,1: removes mean and divides by stddev>"<<endl;
  cout << "  -fovea_scales <scales (e.g: 1.5,2,4)> <scalesizes (e.g: 10x10,20x20,30x30)>" << endl;
  cout << "  -fovea <bool |0,1: deactivate or activate fovea mode>" << endl;
  cout << "  -bboxfact <float factor> (multiply bounding boxes by a factor)";
  cout << endl;
  cout << "  -bboxhfact <float factor> (multiply bboxes height by a factor)";
  cout << endl;
  cout << "  -bboxwfact <float factor> (multiply bboxes width by a factor)";
  cout << endl;
  cout << "  -bbox_woverh <float factor> (force w to be h * this factor)"<<endl;
  cout << "  -resize <mean(default)|gaussian|bilinear" << endl;
  cout << "  -exclude <class name> (include all but excluded classes," << endl;
  cout << "                         exclude can be called multiple times)";
  cout << endl;
  cout << "  -include <class name> (exclude all but included classes," << endl;
  cout << "                         include can be called multiple times)";
  cout << endl;
  cout << "  -useparts  (also extract object parts, ";
  cout << "e.g. person->(head,hand,foot)" << endl;
  cout << "  -partsonly  (only extract object parts, ";
  cout << "e.g. person->(head,hand,foot)" << endl;
  cout << "  -ignore_difficult (ignore sample if \"difficult\" flag is on)";
  cout << endl;
  cout << "  -ignore_bbox (Use entire image instead of bounding box)" << endl;
  cout << "  -ignore_truncated (ignore sample if \"truncated\" flag is on)";
  cout << endl;
  cout << "  -ignore_occluded (ignore sample if \"occluded\" flag is on)";
  cout << endl;
  cout << "  -nopadded (ignore padded image too small for target size)" << endl;
  cout << "  -njitter <uint>" << endl
       << "   Number of jitters to take among all possible ones." << endl;
  cout << "  -jitter_translation <step,hmin,hmax,wmin,wmax>" << endl
       << "   Range of translations for jitters in pixels." << endl;
  cout << "  -jitter_roration <nsteps,range (in degrees)>" << endl
       << "   Steps and range of rotation jitter, e.g. 10,90" << endl;
  cout << "  -jitter_scale <nsteps,min_scale,max_scale>" << endl
       << "   Steps and range of scale jitter, e.g. 10,.8,1.2" << endl;
  cout << "  -max_jitt_match <float>" << endl
       << "     Maximum match allowed for a jittered rect with other objects."
       << endl;
  cout << "  -wmirror (add mirrored sample using vertical-axis symmetry)"
       << endl;
  cout << "  -forcelabel <label name>" << endl;
  cout << "  -minvisibility <ratio [0.0,1.0]>" << endl;
  cout << "     If visible bounding boxes are defined, reject bbs with " << endl
       << "     visible/full ratios less than ratio." << endl;
  cout << "  -minborders <dimensions (default: 0x0)>" << endl;
  cout << "     Exclude bounding boxes which are closer to image borders"
       << endl << "     than those dimensions." << endl;
  cout << "  -min_aspect_ratio <ratio [0.0,1.0]>" << endl;
  cout << "     Minimum aspect ratio (width/height) allowed for input bb."
       << endl;
  cout << "  -max_aspect_ratio <ratio [0.0,1.0]>" << endl;
  cout << "     Maximum aspect ratio (width/height) allowed for input bb."
       << endl;
  cout << "  -planar_loading" << endl
       << "     Input is considered as planar (channels as 1st dim)." << endl;
  cout << "  -nocount" << endl
       << "     Do not count samples prior extraction." << endl;
  cout << "  -videobox <n> <stride>" << endl
       << "     For video sequences, the bbox regions in the next 'n' frames "
       << endl <<"are taken, for every 'stride' frame and added as a feature."
       << endl;
}

// compilation /////////////////////////////////////////////////////////////////

template <typename Tdata>
void compile() {
  // create preprocessing modules
  vector<resizepp_module<Tdata>*> ppmodules;
  if (!nopp) {
    midxdim dummy_zpad; // dummy var because we dont have zpad option in dscompile
    if (pp_params.size() == 0) // no pp added yet, add only 1
      ppmodules.push_back(create_preprocessing<Tdata>
			  (outdims, channels_mode.c_str(),
			   kernelsz, dummy_zpad, resize.c_str(),
                           keep_aspect_ratio, lpyramid, &fovea_scales,
                           &fovea_scales_size, global_norm, divnorm, divnorm2,
                           colornorm, colornorm_across, bboxhfact, bboxwfact));
    else // add vector of all preprocessing params
      for (uint i = 0; i < pp_params.size(); ++i) {
	pp &p = *(pp_params[i]);
	ppmodules.push_back(create_preprocessing<Tdata>
			    (p.dims, p.ppchan.c_str(), p.kernel_sz,
			     dummy_zpad, p.resize_method.c_str(),
                             p.keep_aspect_ratio,
			     p.lpyramid, p.fovea, p.fovea_scales_size,
			     p.global_norm, p.divnorm, p.divnorm2, p.colornorm,
			     p.colornorm_across, p.bboxhfact, p.bboxwfact));
      }
  }
  // allocate datasets
  dataset<Tdata> *ds = NULL;
  if (!strcmp(stype.c_str(), "grid"))
    ds = new grid_dataset<Tdata>(dataset_name.c_str(), images_root.c_str(),
				 gridsz.dim(0), gridsz.dim(1));
  else if (!strcmp(stype.c_str(), "pascal")) {
    pascal_dataset<Tdata> *d =
        new pascal_dataset<Tdata>(dataset_name.c_str(), images_root.c_str(),
                                  ignore_difficult, ignore_truncated,
                                  ignore_occluded, annotations.c_str(),
                                  ignore_path.c_str(), ignore_bbox);
    ds = d;
    if (min_aspect_ratio_set) d->set_min_aspect_ratio(min_aspect_ratio);
    if (max_aspect_ratio_set) d->set_max_aspect_ratio(max_aspect_ratio);
    if (minborders_set) d->set_minborders(minborders);
    if (max_jitt_match > 0.0) d->set_max_jitter_match(max_jitt_match);
  } else if (!strcmp(stype.c_str(), "pascalbg")) {
    ds = new pascalbg_dataset<Tdata>
        (dataset_name.c_str(), images_root.c_str(), outdir.c_str(), maxperclass,
         ignore_difficult, ignore_truncated, ignore_occluded, annotations.c_str(),
         tmpout.c_str());
    if (force_label) { // force all labels to a unique "bg" background label
      force_label = true;
      label = "bg";
    }
  } else if (!strcmp(stype.c_str(), "pascalclear"))
    ds = new pascalclear_dataset<Tdata>
        (dataset_name.c_str(), images_root.c_str(), outdir.c_str(),
         annotations.c_str());
  else if (!strcmp(stype.c_str(), "pascalfull"))
    ds = new pascalfull_dataset<Tdata>
        (dataset_name.c_str(), images_root.c_str(), outdir.c_str(),
         annotations.c_str());
  else if (!strcmp(stype.c_str(), "regular"))
    ds = new dataset<Tdata>(dataset_name.c_str(), images_root.c_str());
  else if (!strcmp(stype.c_str(), "patch"))
    ds = new patch_dataset<Tdata>(dataset_name.c_str(), images_root.c_str(),
				  outdir.c_str(), maxperclass);
  else eblerror("unknown dataset type " << stype);
  // set preprocessings
  ds->set_preprocessing(ppmodules);
  // set all parameters
  ds->set_outdir(outdir.c_str(), tmpout.c_str());
  if (wmirror) ds->set_wmirror();
  if (njitter > 0)
    ds->set_jitter(tjitter_step, tjitter_hmin, tjitter_hmax,
                   tjitter_wmin, tjitter_wmax,
                   sjitter_steps, sjitter_min, sjitter_max,
                   rjitter_steps, rjitter, njitter);
  if(videobox)
    ds->set_videobox(videobox_n, videobox_stride);
  if (bbox_woverh_set) ds->set_bbox_woverh(bbox_woverh);
  if (usepose) ds->use_pose();
  if (useparts) ds->use_parts();
  if (partsonly) ds->use_parts_only();
  ds->set_exclude(exclude);
  ds->set_include(include);
  if (outdims_set) ds->set_outdims(outdims[0]);
  if (mindims_set) ds->set_mindims(mindims);
  if (maxdims_set) ds->set_maxdims(maxdims);
  ds->set_display(display);
  if (save_display)
    ds->save_display(save_display_dir, save_display_dims.dim(0),
                     save_display_dims.dim(1));
  ds->set_nopadded(nopadded);
  ds->set_sleepdisplay(sleep_delay);
  if (save_set) ds->set_save(save);
  ds->set_individual_save(individual_save);
  ds->set_separate_layers_save(separate_layers_save);
  if (maxperclass > 0) ds->set_max_per_class(maxperclass);
  if (maxdata > 0) ds->set_max_data(maxdata);
  ds->set_image_pattern(image_pattern);
  if (force_label) ds->set_label(label);
  if (scale_mode) ds->set_scales(scales, outdir);
  if (minvisibility_set) ds->set_minvisibility(minvisibility);
  if (planar_loading) ds->set_planar_loading();
  // execute extraction
  // switch between load and normal mode
  if (load_set) { // in load mode, do nothing but loading dataset
    ds->set_name(load); // dataset to load
    ds->load(images_root); // directory in which dataset is
  } else if (stats_mode) { // extract statistics about dataset
    ds->extract_statistics();
  } else { // normal mode, do extraction
    if (!nocount)
      ds->count_total();
    if (scale_mode) ds->set_scales(scales, outdir);
    else ds->alloc();
    ds->extract();
  }
  // shuffle
  if (shuffle) ds->shuffle();
  // save dataset
  if (save_set && !scale_mode) {
    if (force_label) ds->set_unique_label(label);
    ds->set_name(dataset_name);
    //ds->set_save(DYNSET_SAVE);
    ds->save(outdir);
  }
  // delete variables
  for (uint i = 0; i < pp_params.size(); ++i)
    delete pp_params[i];
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) {
#else
  int main(int argc, char **argv) {
#endif
    try {
      cout << "_______________________________________________________________";
      cout << endl << endl;
      cout << "             Dataset compiler for libeblearn library " << endl;
      cout << "_______________________________________________________________";
      cout << endl;
      // parse arguments
      if (!parse_args(argc, argv)) {
	print_usage();
	return -1;
      }
      // print info
      cout << "input parameters:" << endl;
      cout << "  dataset name: " << dataset_name << endl;
      cout << "  dataset type: " << stype << endl;
      cout << "  dataset precision: " << precision << endl;
      cout << "  images root directory: " << images_root << endl;
      cout << "  annotations directory: " << annotations << endl;
      cout << "  ignored annotations directory: " << ignore_path << endl;
      cout << "  output directory: " << outdir << endl;
      cout << "  outputs: " << outdir << "/" << dataset_name << "_*.mat"
	   << endl;
      cout << "  images pattern: " << image_pattern << endl;
      cout << "  channels mode: " << channels_mode.c_str() << endl;
      cout << "  preprocessing: " << (preprocessing ? "yes" : "no") << endl;
      cout << "  display: " << (display ? "yes" : "no") << endl;
      cout << "  display sleep: " << sleep_delay << " ms." << endl;
      cout << "  shuffling: " << (shuffle ? "yes" : "no") << endl;
      cout << "  usepose: " << (usepose ? "yes" : "no") << endl;
      cout << "  useparts: " << (useparts ? "yes" : "no") << endl;
      cout << "  partsonly: " << (partsonly ? "yes" : "no") << endl;
      cout << "  stereo: " << (stereo ? "yes" : "no") << endl;
      if (stereo) {
	cout << "    stereo left pattern: " << stereo_lpattern << endl;
	cout << "    stereo right pattern: " << stereo_rpattern << endl;
      }
      cout << "  max per class limitation: ";
      if (maxperclass > 0) cout << maxperclass; else cout << "none";
      cout << endl;
      cout << "  max data limitation: ";
      if (maxdata > 0) cout << maxdata; else cout << "none"; cout << endl;
      cout << "  mexican_hat_size: " << mexican_hat_size << endl;
      cout << "  preprocessing kernel size: " << kernelsz << endl;
      cout << "  deformations: " << deformations << endl;
      cout << "  resizing method: " << resize << endl;
      cout << "  output dimensions: " << outdims << endl;
      cout << "  minimum input dimensions: " << mindims << endl;
      cout << "  maximum input dimensions: " << maxdims << endl;
      cout << "  no padded: " << (nopadded ? "yes" : "no") << endl;
      cout << "  scales: ";
      if (!scale_mode) cout << "none";
      else for (vector<double>::iterator i = scales.begin(); i != scales.end();
		++i)
	     cout << *i << " ";
      cout << endl;
      cout << "  laplacian pyramid: " << lpyramid << endl;
      cout << "_______________________________________________________________";
      cout << endl;

      // compile with specificed precision
      if (!strcmp(precision.c_str(), "float"))
	compile<float>();
      else if (!strcmp(precision.c_str(), "double"))
	compile<double>();
#ifndef __WINDOWS__ // TODO: check if its a compiler bug, works fine with gcc
      else if (!strcmp(precision.c_str(), "ubyte"))
	compile<ubyte>();
#endif
      else
	eblerror("trying to compile dataset with unsupported precision \""
		 << precision << "\"");
    } eblcatcherror();
#ifdef __GUI__
    cout << "Closing windows..." << endl;
    quit_gui(); // close all windows
#endif
    return 0;
  }
