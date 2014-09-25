/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifdef __WINDOWS__
//#include <Winsock2.h>
//#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
//#include <unistd.h>
#endif

#include "ebl_utils.h"

uint dump_count = 0;
std::string dump_prefix;

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // table functions

  idx<intg> full_table(intg a, intg b, intg a0, intg b0) {
    if (a <= a0 || b <= b0)
      eblerror("a=" << a << " and b=" << b << " have to be greater than a0="
	       << a0 << " and b0=" << b0);
    idx<intg> m((a - a0) * (b - b0), 2);
    intg p = 0;
    for (intg j = b0; j < b; ++j) {
      for (intg i = a0; i < a; ++i) {
	m.set(i, p, 0);
	m.set(j, p, 1);
	p++;
      }
    }
    return m;
  }

  idx<intg> one2one_table(intg n) {
    idx<intg> m(n, 2);
    for (intg i = 0; i < n; ++i) {
      m.set(i, i, 0);
      m.set(i, i, 1);
    }
    return m;
  }

  idx<intg> concat_tables(idx<intg> &t0, idx<intg> &t1) {
    if (t0.order() == 0 && t1.order() == 0)
      eblerror("expecting tables order to be at least 1, but found: "
	       << t0 << " and " << t1);
    if (t0.order() == 0) return t1;
    if (t1.order() == 0) return t0;
    idx<intg> t(t0.dim(0) + t1.dim(0), 2);
    idx<intg> tmp = t.narrow(0, t0.dim(0), 0);
    idx_copy(t0, tmp);
    tmp = t.narrow(0, t1.dim(0), t0.dim(0));
    idx_copy(t1, tmp);
    return t;
  }

  idx<intg> random_table(intg a, intg b, std::vector<intg> &fanin,
			 intg a0, intg b0) {
    if (a <= a0 || b <= b0)
      eblerror("a and b have to be greater than a0 and b0");
    // initialize fanins
    idx<intg> fanins(b - b0);
    for (intg i = 0; i < fanins.dim(0); ++i)
      fanins.set(fanin[(uint) ((i / (float) fanins.dim(0)) * fanin.size())], i);
    idx_shuffle(fanins, 0);
    intg total_fanins = idx_sum(fanins);
    // init table
    idx<intg> tbl(total_fanins, 2);
    idx<intg> fi(total_fanins);

    intg itbl = 0;
    std::vector<intg> ins, outs;
    for (intg i = 0; i < fanins.dim(0); ++i) {
      intg f = fanins.get(i);
      outs.clear();
      for (intg j = 0; j < f; ++j) {
	// refill input indices if empty
	if (ins.size() == 0) {
	  for (intg j = a0; j < a0 + a; ++j) ins.push_back(j);
	  random_shuffle(ins.begin(), ins.end());
	}
	// pick first input index not present in current fanin set
	intg k = 0;
	for ( ; (uint) k < ins.size(); ++k) {
	  bool found = false;
	  for (intg l = 0; (uint) l < outs.size(); ++l)
	    if (ins[k] == outs[l]) found = true;
	  if (!found) break ;
	}
	if ((uint) k == ins.size()) // all ins were present in outs
	  k = 0; // add first element anyway
	outs.push_back(ins[k]);
	ins.erase(ins.begin() + k);
      }
      // push outs to final outputs
      for (uint j = 0; j < outs.size(); ++j) {
	tbl.set(outs[j], itbl, 0);
	tbl.set(i, itbl, 1);
	itbl++;
      }
    }
    return tbl;
  }

  idx<intg> yuv_table0(intg yend, intg uend, intg vend, intg yuvend,
		       intg uvend, intg &maxend) {
    idx<intg> all;
    maxend = 0;
    if (yend > 0) {
      idx<intg> t = full_table(1, yend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, yend);
    }
    if (uend > 0) {
      idx<intg> t = full_table(2, uend, 1, maxend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, uend);
    }
    if (vend > 0) {
      idx<intg> t = full_table(3, vend, 2, maxend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, vend);
    }
    if (yuvend > 0) {
      idx<intg> t = full_table(3, yuvend, 0, maxend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, yuvend);
    }
    if (uvend > 0) {
      idx<intg> t = full_table(3, uvend, 1, maxend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, uvend);
    }
    return all;
  }

  idx<intg> uv_table0(intg uend, intg vend) {
    idx<intg> t1 = full_table(1, uend); // U channel
    idx<intg> t2 = full_table(2, vend, 1, uend);
    return concat_tables(t1, t2);
  }

  idx<intg> yuv_table1(intg yend, intg uend, intg vend,
		       intg p0, intg p1, intg p2, intg fanin_y,
		       intg fanin_yuv, intg fanin_uv) {
    std::vector<intg> fy, fyuv, fuv;
    fy.push_back(fanin_y);
    fyuv.push_back(fanin_yuv);
    fuv.push_back(fanin_uv);
    dynamic_init_drand();
    if (fanin_y == 0 && fanin_yuv == 0 && fanin_uv == 0)
      eblerror("at least 1 fanin variable must be non zero");
    // table 1
    idx<intg> t3, t4, t5, table1;
    bool t1_defined = false;
    if (fanin_y != 0)   t3 = random_table(yend, p0, fy);
    if (fanin_yuv != 0) t4 = random_table(vend, p1, fyuv, 0, p0);
    if (fanin_uv != 0)  t5 = random_table(vend, p2, fuv, yend, p1);
    if (fanin_y != 0) {
      table1 = t3;
      t1_defined = true;
    }
    if (fanin_yuv != 0) {
      if (t1_defined)
	table1 = concat_tables(table1, t4);
      else {
	table1 = t4;
	t1_defined = true;
      }
    }
    if (fanin_uv != 0) {
      if (t1_defined)
	table1 = concat_tables(table1, t5);
      else {
	table1 = t5;
      }
    }
    return table1;
  }

  idx<intg> stereo_table(intg lend, intg rend, intg lrend) {
    idx<intg> t1, t2, t3, all;
    if (lend > 0) {
      t1 = full_table(1, lend);
      all = concat_tables(all, t1);
    }
    if (rend > 0) {
      t2 = full_table(2, rend, 1, lend);
      all = concat_tables(all, t2);
    }
    if (lrend > 0) {
      t3 = full_table(2, lrend, 0, std::max(lend, rend));
      all = concat_tables(all, t3);
    }
    return all;
  }

  idx<intg> temporal3_table(intg end1, intg end2, intg end3,
			    intg end12, intg end23, intg end123, intg &maxend) {
    idx<intg> all;
    maxend = 0;
    if (end1 > 0) {
      idx<intg> t = full_table(1, end1);
      all = concat_tables(all, t);
      maxend = std::max(maxend, end1);
    }
    if (end2 > 0) {
      idx<intg> t = full_table(2, end2, 1, maxend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, end2);
    }
    if (end2 > 0) {
      idx<intg> t = full_table(3, end3, 2, maxend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, end3);
    }
    if (end12 > 0) {
      idx<intg> t = full_table(2, end12, 0, maxend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, end12);
    }
    if (end23 > 0) {
      idx<intg> t = full_table(3, end23, 1, maxend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, end23);
    }
    if (end123 > 0) {
      idx<intg> t = full_table(3, end123, 0, maxend);
      all = concat_tables(all, t);
      maxend = std::max(maxend, end123);
    }
    return all;
  }

  idx<intg> text_table(std::string filename) {
    vector<intg> inputs, outputs;
    int scanint;
    char scanchar;
    FILE *fp = fopen(filename.c_str(), "rb");
    while (!feof(fp)) {
      // scanint = fscan_int(fp);
      int ret = fscanf (fp, "%d", &scanint);
      if (ret == EOF)
        break;
      if (scanint < 0)
        eblwarn(" Wrong file format, expected number");
      inputs.push_back(scanint);
      // cout << scanint << " "<<endl;
      ret = fscanf (fp, "%c", &scanchar);
      if (scanchar != ',')
        eblwarn(" Wrong file format, expected a comma character ");
      // scanchar = fscan_char(fp);
      // cout << scanchar << " "<<endl;
      // scanint = fscan_int(fp);
      ret = fscanf (fp, "%d", &scanint);
      if (scanint < 0)
        eblwarn(" Wrong file format, expected number");
      outputs.push_back(scanint);
      // cout << scanint << " "<<endl;
      ret = fscanf (fp, "%c", &scanchar);
      if (scanchar != ';')
        eblwarn(" Wrong file format, expected a semicolon character ");
      // scanchar = fscan_char(fp);
      // cout << scanchar << " " <<endl;
    }
    int n = inputs.size();
    idx<intg> m(n, 2);
    for (intg i = 0; i < n; ++i) {
      m.set(inputs[i], i, 0);
      m.set(outputs[i], i, 1);
    }
    return m;

  }

  bool check_table_duplicates(idx<intg> &table) {
    bool found = false;
    for (intg i = 0; i < table.dim(0); ++i)
      for (intg j = 0; j < table.dim(0); ++j)
	if (i != j &&
	    table.get(i, 0) == table.get(j, 0) &&
	    table.get(i, 1) == table.get(j, 1)) {
	  found = true;
	  eblwarn("duplicate input " << table.get(j, 0) << " found for output "
		  << table.get(j, 1) << " at indices " << i << " and " << j);
	}
    return found;
  }

} // end namespace ebl
