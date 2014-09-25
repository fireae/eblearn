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

#include <sstream>
#include <iomanip>
#include "stl.h"
#include "ebl_utils.h"
#include "eblapp.h"
#include "tools_utils.h"

uint insize = 0; // number of inputs
intg outsize = 0; // number of outputs
std::vector<intg> fanin; // fanin
float density = 0; // density
uint end0 = 0;
uint end1 = 0;
uint end2 = 0;
uint end3 = 0;
uint end4 = 0;
uint end5 = 0;
bool fanin_set = false; // fanin
bool density_set = false; // density
bool full = false;
bool brandom = false; // random table
bool one2one = false; // 1 to 1 table
bool yuv = false; // yuv layer 0 table
bool uv = false; // uv layer 0 table
bool stereo = false;
bool temporal3 = false;
bool btext = false; // create table from text file
string text_filename; // filename for text file option
// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc <= 1) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  // loop over arguments
  for (int i = 1; i < argc; ++i) {
    try {
      if (strcmp(argv[i], "-full") == 0) {
	++i; if (i >= argc) throw 1;
	insize = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	outsize = atoi(argv[i]);
	full = true;
      } else if (strcmp(argv[i], "-random") == 0) {
	++i; if (i >= argc) throw 1;
	insize = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	outsize = atoi(argv[i]);
	brandom = true;
      } else if (strcmp(argv[i], "-yuv0") == 0 || strcmp(argv[i], "-yuv") == 0){
	++i; if (i >= argc) throw 1;
	end0 = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	end1 = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	end2 = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	end3 = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	end4 = atoi(argv[i]);
	yuv = true;
      } else if (strcmp(argv[i], "-uv0") == 0 || strcmp(argv[i], "-uv") == 0) {
	++i; if (i >= argc) throw 1;
	end1 = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	end2 = atoi(argv[i]);
	uv = true;
      } else if (strcmp(argv[i], "-fanin") == 0) {
	++i; if (i >= argc) throw 1;
	fanin = string_to_intgvector(argv[i]);
	fanin_set = true;
      } else if (strcmp(argv[i], "-density") == 0) {
	++i; if (i >= argc) throw 1;
	density = atof(argv[i]);
	density_set = true;
      } else if (strcmp(argv[i], "-one2one") == 0) {
	one2one = true;
	++i; if (i >= argc) throw 1;
	insize = atoi(argv[i]);
      } else if (strcmp(argv[i], "-stereo") == 0) {
	++i; if (i >= argc) throw 1;
	end0 = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	end1 = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	end2 = atoi(argv[i]);
	stereo = true;
      } else if(strcmp(argv[i], "-text") == 0) {
        ++i; if (i >= argc) throw 1;
        text_filename = argv[i];
        btext = true;
      }
      else if (strcmp(argv[i], "-temporal3") == 0) {
	++i; if (i >= argc) throw 1; end0 = atoi(argv[i]);
	++i; if (i >= argc) throw 1; end1 = atoi(argv[i]);
	++i; if (i >= argc) throw 1; end2 = atoi(argv[i]);
	++i; if (i >= argc) throw 1; end3 = atoi(argv[i]);
	++i; if (i >= argc) throw 1; end4 = atoi(argv[i]);
	++i; if (i >= argc) throw 1; end5 = atoi(argv[i]);
	temporal3 = true;
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
  cout << "Usage: ./maketable [OPTIONS]"
       << endl << "Options are:" << endl;
  cout << "  -full <in size> <out size>" << endl
       << "   Connect fully inputs to outputs." << endl;
  cout << "  -random <in size> <out size>" << endl
       << "   Connect randomly inputs to outputs." << endl;
  cout << "  -fanin <n,m,...>" 
       << "   Number of inputs connected to each output." << endl
       << "   If multiple fanins are provided, each fanin is used in equal amount." << endl;
  cout << "  -density <float [0, 1]>" 
       << "   Connection density (0: empty, 1: full), replaces fanin parameter." << endl;
  cout << "  -one2one <size>" << endl
       << "   Connect [0 .. size] to [0 .. size] in a 1 to 1 mapping." << endl;
  cout << "  -yuv <yend> <uend> <vend> <yuvend> <uvend>" << endl
       << "   Connect 3-channel input (e.g. YUV) independently for layer 0, "
       << "   Y to [0 .. yend - 1], U to [yend .. uend - 1] and V to "
       << "   [uend .. vend - 1] and jointly connecting Y,U,V to [vend .. yuvend - 1]"
       << "   and U,V to [yuvend - 1 .. uvend]." << endl;
  cout << "  -uv <uend> <vend>" << endl
       << "   Connect 2-channel input (e.g. UV) independently for layer 0, "
       << "   U to [0 .. uend - 1] and V to "
       << "   [uend .. vend - 1]." << endl;
  cout << "  -stereo <lend> <rend> <lrend>" << endl
       << "    Create a table for stereo pair, with a full table from the first"
       << "    channel to [0 .. lend-1], from the 2nd channel to " << endl
       << "    [lend .. rend-1]"
       << "    and finally a fully connected table from both channels to"
       << "    [rend .. lrend]." << endl;
  cout << "  -temporal3 <end1> <end2> <end3> <end12> <end23> <end123>" << endl
       << "    Create a table for a temporal triplet, with a full table " <<endl
       << "    from the first channel to [0 .. end1-1], from the 2nd " << endl
       << "    channel to [end1 .. end2-1], the 3rd to [end2 .. end3-1], "<<endl
       << "    from channels 1 & 2 to [end3 .. end12-1], from chans " << endl
       << "    2 & 3 to [end12 .. end23-1], from chans 1, 2 & 3 to " << endl
       << "    [end23 .. end123-1]." << endl;
  cout << "  -text <filename> " << endl
       << "    Create a table from a text file with the following example format:" << endl
       << "    0,0; 0,1; 1,2; 3,1, 4,2; 1,5" << endl
       << "    The file can have newline characters and spaces which "
       << "    will be ignored " << endl;
}

int main(int argc, char **argv) {
  cout << "___________________________________________________________________";
  cout << endl << endl;
  cout << "             maketable for eblearn" << endl;
  cout << "___________________________________________________________________";
  cout << endl;

  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  // make table
  idx<intg> table;
  ostringstream name;
  string type = "";
  if (full) {
    cout << "Making a full table from " << insize << " to "
	 << outsize << endl;
    table = full_table(insize, outsize);
    type = "full";
    fanin.push_back(insize); // full fanin
  } else if (brandom) {
    if (density_set) {
      fanin.clear();
      fanin.push_back((intg) (density * insize));
    }
    if (fanin.size() == 0)
      eblerror("you must set a fanin > 0");
    cout << "Making a random table from " << insize << " to "
	 << outsize << " with fanin(s) " << fanin << endl;
    dynamic_init_drand(); // init random seed
    table = random_table(insize, outsize, fanin);
    type = "random";
  } else if (yuv) {
    if (end0 == 0 && end1 == 0 && end2 == 0 && end3 == 0 && end4 == 0)
      eblerror("you must set at least yend, uend or vend > 0");
    cout << "Making a 3-channel table from 3 to "
	 << end0 << " (chan 0), " << end1 << " (chan 1), " << end2
	 << " (chan 2), from chans 0,1,2 to " << end3
	 << " and chans 1,2 to " << end4 << endl;
    table = yuv_table0(end0, end1, end2, end3, end4, outsize);
    type << "yuv_y" << end0 << "_u" << end1 << "_v" << end2 << "_yuv" << end3
	 << "_uv" << end4;
    insize = 3;
  } else if (uv) {
    if (end1 == 0 && end2 == 0)
      eblerror("you must set at least yend, uend or vend > 0");
    cout << "Making a 2-channel table from 2 to "
	 << end1 << " (chan 0), " << end2 << " (chan 1)." << endl;
    table = uv_table0(end1, end2);
    type << "uv_u" << end1 << "_v" << end2;
    insize = 2;
    outsize = end2;
  } else if (one2one) {
    cout << "Making a one to one table from " << insize << " to "
	 << insize << endl;
    table = one2one_table(insize);
    type = "one2one";
    fanin.push_back(1);
    outsize = insize;
  } else if (stereo) {
    if (end0 == 0 && end1 == 0 && end2 == 0)
      eblerror("you must set at least lend, rend or lrend > 0");
    cout << "Making a 2-channel table from 2 to "
	 << end0 << " (chan 0), " << end1 << " (chan 1), " << end2
	 << " (chan 0 & 1)." << endl;
    table = stereo_table(end0, end1, end2);
    type << "stereo_l" << end0 << "_r" << end1 << "_lr" << end2;
    insize = 2;
    outsize = end2;
  } else if (temporal3) {
    table = temporal3_table(end0, end1, end2, end3, end4, end5, outsize);
    type << "temporal3_1-" << end0 << "_2-" << end1 << "_3-" << end2
	 << "_4-" << end3 << "_5-" << end4 << "_6-" << end5;
    insize = 3;
  } else if (btext) {
    table = text_table(text_filename);
    type << text_filename;
  } else
    eblerror("unknown type");
  table.printElems();
  float density = table.dim(0) / (float) (insize * outsize);
  cout << "Table dimensions: " << table << endl;
  string sfanin = "_fanin";
  for (uint i = 0; i < fanin.size(); ++i) sfanin << "_" << fanin[i];
  name << "table_" << insize << "_" << outsize << "_connect_" << table.dim(0)
       << sfanin << "_density_" << setw(1) << setfill('0') << setprecision(2)
       << density << "_" << type << ".mat";
  save_matrix<int>(table, name.str());
  cout << "Saved table in " << name.str() << endl;
  return 0;
}

