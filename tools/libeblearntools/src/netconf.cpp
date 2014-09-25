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

#include "netconf.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////

  bool load_table(configuration &conf, const std::string &module_name,
		  idx<intg>& table, intg thickness, intg noutputs) {
    std::string name = module_name; name << "_table"; // table name
    std::string name_in = name; name_in << "_in"; // table max input
    std::string name_out = name; name_out << "_out"; // table max output

    if (conf.exists(name)) { // request to load table from file
      std::string filename = conf.get_string(name.c_str());
      if (!file_exists(filename))
	eblerror("cannot find table file declared in variable " << name
		 << ": " << filename);
      table = load_matrix<intg>(filename);
      eblprint("Loaded " << name << " (" << table << ") from " << filename
               << endl);
      return true;
    } else if (conf.exists(name_in) && conf.exists(name_out)) {
      intg in, out;
      // special case, if in == "thickness", use current thickness
      std::string val_in = conf.get_string(name_in);
      if (!val_in.compare("thickness"))
	in = thickness;
      else // regular case, use string as int
	in = conf.get_int(name_in.c_str());
      // special case, if out == "noutputs", use current thickness
      std::string val_out = conf.get_string(name_out);
      if (!val_out.compare("noutputs"))
	out = noutputs;
      else // regular case, use string as int
	out = conf.get_int(name_out.c_str());
      // create table
      table = full_table(in, out);
      eblprint("Using a full table for " << name << ": "
               << in << " -> " << out << " (" << table << ")" << std::endl);
      return true;
    }
    eblwarn("Failed to load table " << name << std::endl);
    return false;
  }

  ////////////////////////////////////////////////////////////////

  void load_gd_param(configuration &conf, gd_param &gdp) {
    // mandatory learning parameters
    gdp.eta = conf.get_double("eta");
    // optional learning parameters
    if (conf.exists("reg_time"))
      gdp.decay_time = conf.get_intg("reg_time");
    if (conf.exists("reg_l1"))
      gdp.decay_l1 = conf.get_double("reg_l1");
    if (conf.exists("reg_l2"))
      gdp.decay_l2 = conf.get_double("reg_l2");
    if (conf.exists("inertia"))
      gdp.inertia = conf.get_double("inertia");
    if (conf.exists("anneal_value"))
      gdp.anneal_value = conf.get_double("anneal_value");
    if (conf.exists("anneal_period"))
      gdp.anneal_period = conf.get_intg("anneal_period");
    if (conf.exists("gradient_threshold"))
      gdp.gradient_threshold = conf.get_double("gradient_threshold");
    // printing parameters
    eblprint(gdp << std::endl);
  }

} /* namespace ebl */
