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
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <stdio.h>
#include <limits>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "string_utils.h"
#include "tools_utils.h"
#include "configuration.h"

using namespace std;

#define SQ '\''
#define DQ '\"'
#define BQ '`'

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // textlist class

  textlist::textlist() {
  }

  textlist::textlist(const textlist &txt) {
    // deep copy
    for (list<pair<std::string,std::string> >::const_iterator i = txt.begin();
         i != txt.end(); ++i) {
      push_back(pair<std::string,std::string>(i->first, i->second));
    }
  }

  textlist::~textlist() {
  }

  void textlist::update(const std::string &varname, const std::string &value) {
    bool found = false;
    std::string s;
    s << varname << "=" << value;
    for (list< pair<std::string,std::string> >::iterator i = this->begin();
         i != this->end(); ++i) {
      if (i->second == varname) {
        i->first = s;
        found = true;
      }
    }
    if (!found) {
      s << " # variable added by metarun";
      push_back(pair<std::string,std::string>(s, varname));
    }
  }

  void textlist::print(ostream &out) {
    for (list< pair<std::string,std::string> >::iterator i = this->begin();
         i != this->end(); ++i) {
      out << i->first << endl;
    }
  }

  ////////////////////////////////////////////////////////////////
  // utility functions

  void remove_trailing_whitespaces(std::string &s) {
    std::string::size_type pos;
    char whites[] = " \t";
    pos = s.find_first_not_of(whites);
    if (pos == std::string::npos)
      s = "";
    else {
      s = s.substr(pos);
      pos = s.find_last_not_of(whites);
      s = s.erase(pos + 1);
    }
  }

  //! Remove quotes, except if they are preceded by a \.
  std::string replace_quotes(const std::string &s) {
    std::string res(s), tmp;
    size_t qpos = res.find(DQ);
    while (qpos != std::string::npos) {
      if (qpos > 0 && res[qpos - 1] == '\\') {
        qpos--;
        tmp = res.substr(qpos + 2);
        res = res.substr(0, qpos);
        res += DQ;
        res += tmp;
      } else {
        tmp = res.substr(qpos + 1);
        res = res.substr(0, qpos);
        res += tmp;
      }
      qpos = res.find(DQ);
    }
    return res;
  }

  std::string configuration::resolve0(string_map_t &m, const std::string &variable,
                                      const std::string &v,
                                      bool firstonly) {
    std::string res(v);
    if (v.size() == 0)
      return res;
    // 1. if we find quotes, resolve each unquoted string and concatenate res
    size_t qpos = v.find(DQ);
    // skip all quotes preceded by slash
    while (qpos != std::string::npos && qpos > 0 && v[qpos - 1] == '\\') {
      qpos = res.find(DQ, qpos + 1);
    }
    if (qpos != std::string::npos) { // quote found
      // find matching quote
      size_t qpos2 = res.find(DQ, qpos + 1);
      // skip all quotes preceded by slash
      while (qpos2 != std::string::npos && qpos2 > 0 && res[qpos2 - 1] == '\\') {
        qpos2 = res.find(DQ, qpos2 + 1);
      }
      if (qpos2 == std::string::npos)
        eblerror("unmatched quote in: " << res);
      // resolve both sides of quoted section
      std::string s0 = res.substr(0, (std::max)((size_t) 0, qpos -1));
      std::string s1 = res.substr(qpos + 1, qpos2 - 1 - qpos);
      std::string s2 = res.substr(qpos2 + 1);
      res = "";
      if (qpos != 0) {
        s0 = resolve0(m, variable, s0);
        res += s0;
      }
      res += DQ;
      res += resolve0(m, variable, s1);
      res += DQ;
      s2 = resolve0(m, variable, s2);
      res += s2;
      // concatenate resolved and quoted sections
      return res;
    }
    // 2. no quotes are present, now resolve single quotes
    res = resolve_backquotes(m, variable, res, firstonly);
    // 3. no more backquotes, resolve regular string
    res = resolve_string(m, variable, res, firstonly);
    return res;
  }

  //! Resolve single quotes blocs, or entire string if not present.
  std::string configuration::
  resolve_backquotes(string_map_t &m, const std::string &variable,
                     const std::string &v, bool firstonly) {
    std::string res(v);
    if (v.size() == 0)
      return res;
    // if we find quotes, resolve each unquoted string and concatenate res
    size_t qpos = res.find(BQ);
    // skip all quotes preceded by slash
    while (qpos != std::string::npos && qpos > 0 && v[qpos - 1] == '\\') {
      qpos = res.find(BQ, qpos + 1);
    }
    if (qpos != std::string::npos) { // quote found
      // find matching quote
      size_t qpos2 = res.find(BQ, qpos + 1);
      // skip all quotes preceded by slash
      while (qpos2 != std::string::npos && qpos2 > 0 && res[qpos2 - 1] == '\\') {
        qpos2 = res.find(BQ, qpos2 + 1);
      }
      if (qpos2 == std::string::npos)
        eblerror("unmatched single quote in: " << res);
      // resolve both sides of quoted section
      std::string s0 = res.substr(0, (std::max)((size_t) 0, qpos -1));
      std::string s1 = res.substr(qpos + 1, qpos2 - 1 - qpos);
      std::string s2 = res.substr(qpos2 + 1);
      res = "";
      if (qpos != 0) {
        res += s0;
      }
      s1 = resolve0(m, variable, s1);
      // execute resolved quoted command in s1
      res += system_to_string(s1);
      res += s2;
    }
    return res;
  }

  //! Resolve an unquoted string (just variables).
  std::string configuration::resolve_string(string_map_t &m, const std::string &variable,
                                            const std::string &v, bool firstonly) {
    std::string res(v);
    if (v.size() == 0)
      return res;
    size_t pos = res.find("${");
    size_t pos2, pos3;
    uint cnt = 0;
    // loop until it's all resolved
    while (pos != std::string::npos) {
      if (cnt == 1 && firstonly)
        break ; // only process first found variable
      pos2 = res.find("}", pos);
      pos3 = res.find("${", pos + 2);
      std::string rest0 = "";
      while (pos3 != std::string::npos && pos3 < pos2) {
        // there is another variable and it's before the closing of current
        // recursively call on rest of the string
        std::string rest = res.substr(pos + 2);
        //			if (!strcmp(rest0.c_str(), rest.c_str()))
        // 			break ; // we are looping because we could not resolve a var
        rest0 = rest;
        std::string var = resolve0(m, variable, rest, true);
        res = res.substr(0, pos + 2);
        res += var;
        pos2 = res.find("}", pos);
        pos3 = res.find("${", pos + 2);
      }
      if (pos2 == std::string::npos) {
        eblwarn("unmatched closing bracket in: " << v << std::endl);
        eblerror("error resolving variables in configuration");
      }
      // variable to replace
      std::string var = res.substr(pos + 2, pos2 - (pos + 2));
      if (m.find(var) != m.end() && (var != variable)) {
        std::string val = resolve0(m, var, m[var]);
        res = res.replace(pos, pos2 - pos + 1, val);
        pos2 = pos;
      } else { // not found locally, check environment
        char *val = getenv(var.c_str());
        if (val) {
          // 			cout << "using environment variable \"" << var << "\": "
          // 		 << val << endl;
          res = res.replace(pos, pos2 - pos + 1, val);
          pos2 = pos;
        } else { // found nowhere, replace with an empty string
          res = res.replace(pos, pos2 - pos + 1, "");
          pos2 = pos;
        }
      }
      // check if we have more variables to resolve
      pos = res.find("${", pos2);
      cnt++;
    }
    return res;
  }

  // open file fname and put variables assignments in smap.
  // e.g. " i = 42 # comment " will yield a entry in smap
  // with "i" as first value and "42" as second value.
  bool configuration::extract_variables(const std::string &content,
                                        string_map_t &smap, textlist &txt,
                                        string_map_t *meta_smap, bool bresolve,
                                        bool replquotes, bool silent) {
    std::string s0, s;
    char separator = '=';
    char comment1 = '#';
    std::string comment2 = ";;";
    std::string::size_type pos;
    std::string name, value, line;
    std::string cont = content;
    bool done = false;
    while (cont.size() && !done) {
      name = "";
      value = "";
      int line_end = std::min(cont.size(), cont.find_first_of('\n'));
      line = cont.substr(0, line_end);
      // check if there's still anything left
      // this check is needed when confs end without a new line
      if (line_end + 1 < (int) cont.size())
        cont = cont.substr(line_end + 1);
      else
        done = true;
      // remove comments first
      s = line;
      pos = s.find(comment1);
      if (pos != std::string::npos) // comment found, remove it
        s = s.erase(pos);
      pos = s.find(comment2);
      if (pos != std::string::npos) // comment found, remove it
        s = s.erase(pos);
      if (s.size() > 0) {
        // look for separator
        pos = s.find(separator);
        if (pos != std::string::npos) {
          name = s.substr(0, pos);
          if (pos >= s.size() - 1) value = ""; // empty variable
          else value = s.substr(pos + 1);
          // removing trailing whitespaces
          remove_trailing_whitespaces(name);
          remove_trailing_whitespaces(value);
          // forbid spaces in names
          pos = name.find(' ');
          if (pos != std::string::npos && !silent) {
            eblwarn("warning: variable name cannot contain an empty space,"
                    << " ignoring this line: " << line << std::endl);
            continue ;
          }
          // 		// forbid empty values
          // 		if (value.size() == 0) {
          // 			cerr << "warning: variable value cannot be empty,";
          // 			cerr << " ignoring this line: " << line << endl;
          // 			continue ;
          // 		}
          // forbid duplicates
          // 		if (smap.find(name) != smap.end() && !silent) {
          // 			cerr << "warning: duplicate variable name \"" << name;
          // 			cerr << "\", using latest assignment: " << line << endl;
          // 		}
          // if variable name starts with "meta_" put it in the meta conf list
          if (meta_smap && (name.compare(0, 5, "meta_") == 0))
            (*meta_smap)[name] = value;
          else // add variable to map
            smap[name] = value;
        }
      }
      // add original line and variable name (if any) to txt
      txt.push_back(pair<std::string,std::string>(line, name));
    }
    // resolve variables
    if (bresolve) {
      resolve_variables(smap, replquotes);
      if (meta_smap)
        resolve_variables(*meta_smap, replquotes);
    }
    return true;
  }

  // transform each value containing whitespaces into a list of values
  void variables_to_variables_list(string_map_t &smap,
                                   string_list_map_t &lmap) {
    std::string::size_type pos, qpos, qpos2;
    std::string s, stmp;
    string_map_t::iterator smi = smap.begin();
    for ( ; smi != smap.end(); ++smi) {
      s = smi->second;
      std::vector<std::string> &vs = lmap[smi->first];
      // loop over list of elements
      pos = s.find_first_of(' ');
      while ((pos != std::string::npos) && (pos < s.size())) {
        // check for double quotes
        qpos = s.find(DQ);
        if ((qpos != std::string::npos) && (qpos < pos)) { // quote before space
          // look for matching quote
          qpos2 = s.find(DQ, qpos + 1);
          while (qpos2 != std::string::npos && qpos2 > 0 && s[qpos2 - 1] == '\\') {
            qpos2 = s.find(DQ, qpos2 + 1);
          }
          if (qpos2 == std::string::npos) {
            eblwarn("unmatched quote in: " << s << std::endl);
            eblerror("unmatched quote");
          }
          pos = qpos2 + 1; // update pos to skip quoted section
          //		s = s.substr(pos);
          //		pos = s.find(' ');
          continue ; // try again with updated pos
        }
        // check for single quotes
        qpos = s.find(BQ);
        if ((qpos != std::string::npos) && (qpos < pos)) { // quote before space
          // look for matching quote
          qpos2 = s.find(BQ, qpos + 1);
          while (qpos2 != std::string::npos && qpos2 > 0 && s[qpos2 - 1] == '\\') {
            qpos2 = s.find(BQ, qpos2 + 1);
          }
          if (qpos2 == std::string::npos) {
            eblwarn("unmatched single quote in: " << s << std::endl);
            eblerror("unmatched single quote");
          }
          pos = qpos2 + 1; // update pos to skip quoted section
          //		s = s.substr(pos);
          //		pos = s.find(' ');
          continue ; // try again with updated pos
        }
        stmp = s.substr(0, pos);
        vs.push_back(stmp);
        s = s.substr(pos);
        remove_trailing_whitespaces(s);
        pos = s.find(' ');
      }
      vs.push_back(s);
    }
  }

  int config_combinations(string_list_map_t &lmap) {
    int combinations = 1;
    string_list_map_t::iterator lmi = lmap.begin();
    for ( ; lmi != lmap.end(); ++lmi) {
      combinations *= lmi->second.size();
    }
    return combinations;
  }

  // increment the conf_indices by 1 when there is room (based the size of the
  // lmap lists).
  bool config_indices_incr(std::vector<size_t> &conf_indices,
                           string_list_map_t &lmap) {
    // find first index that can be increased (less that list size)
    string_list_map_t::iterator lmi = lmap.begin();
    std::vector<size_t>::iterator ci_incr = conf_indices.begin();
    for ( ; lmi != lmap.end(); ++lmi, ++ci_incr) {
      if (*ci_incr < lmi->second.size() - 1) { // did not reach end of list yet
        // we found the first index that can be increased.
        // increase it and clear all preceding indices
        (*ci_incr)++; // increment index
        std::vector<size_t>::iterator ci_prev = conf_indices.begin();
        for ( ; ci_prev != ci_incr; ++ci_prev)
          *ci_prev = 0;
        return true; // we incremented the configuration indices by 1, return
      }
    }
    return false;
  }

  void set_conf_name(std::vector<size_t> &conf_indices, string_list_map_t &lmap,
                     int combination_id, bool no_conf_id,
                     uint conf_combinations,
                     std::string &fullname, std::string &shortname,
                     std::string &variables) {
    string_list_map_t::iterator lmi = lmap.begin();
    std::vector<size_t>::iterator ci = conf_indices.begin();
    ostringstream name;

    // short name
    name << "conf" << setfill('0')
         << setw(1 + (int) floor(log10((float)conf_combinations))) << combination_id;
    shortname = name.str();
    // full name
    fullname = "";
    if (!no_conf_id)
      fullname << shortname;
    variables = "";
    bool first = true;
    for ( ; lmi != lmap.end(); ++lmi, ++ci)
      if (lmi->second.size() > 1) {
        if (first) first = false;
        else variables << "_";
        variables << lmi->first << "_" << lmi->second[*ci];
      }
    fullname << "_" << variables;
  }

  void print_conf(std::vector<size_t> &conf_indices, string_list_map_t &lmap) {
    string_list_map_t::iterator lmi = lmap.begin();
    std::vector<size_t>::iterator ci = conf_indices.begin();

    for ( ; lmi != lmap.end(); ++lmi, ++ci)
      eblprint(lmi->first << " = " << lmi->second[*ci] << std::endl);
  }

  void print_string_map(string_map_t &smap) {
    string_map_t::iterator smi = smap.begin();
    for ( ; smi != smap.end(); ++smi)
      eblprint(smi->first << " : " << smi->second << std::endl);
  }

  void print_string_list_map(string_list_map_t &lmap) {
    eblprint("___________________________________________________________"
             << std::endl);
    string_list_map_t::iterator lmi = lmap.begin();
    for ( ; lmi != lmap.end(); ++lmi) {
      eblprint(lmi->first << " : ");
      std::vector<std::string>::iterator vi = lmi->second.begin();
      for ( ; vi != lmi->second.end(); ++vi) {
        eblprint(*vi << ", ");
      }
      eblprint(std::endl);
    }
  }

  void assign_current_smap(string_map_t &new_smap, std::vector<size_t> &conf_indices,
                           string_list_map_t &lmap) {
    string_list_map_t::iterator lmi = lmap.begin();
    std::vector<size_t>::iterator ci = conf_indices.begin();

    for ( ; lmi != lmap.end(); ++lmi, ++ci)
      new_smap[lmi->first] = lmi->second[*ci];
  }

  // configuration ///////////////////////////////////////////////////////////////

  configuration::configuration() : silent(false) {
  }

  configuration::configuration(const char *filename, bool replquotes,
                               bool silent_, bool bresolve,
															 const std::string *extra)
		: silent(silent_) {
    if (!read(filename, bresolve, replquotes, silent_, NULL, extra))
      eblerror("failed to open configuration file");
  }

  configuration::configuration(const std::string &filename, bool replquotes,
                               bool silent_, bool bresolve,
															 const std::string *extra)
		: silent(silent_) {
    if (!read(filename.c_str(), bresolve, replquotes, silent_, NULL, extra))
      eblerror("failed to open configuration file");
  }

  configuration::configuration(const configuration &other)
    : smap(other.smap), name(other.name),
      output_dir(other.output_dir), otxt(other.otxt),
      silent(false) {
  }

  configuration::configuration(string_map_t &smap_, textlist &txt,
                               std::string &name_, std::string &output_dir_)
    : smap(smap_), name(name_), output_dir(output_dir_), otxt(txt),
      silent(false) {
  }

  configuration::~configuration() {
  }

  bool configuration::read(const char *fname, bool bresolve,
                           bool replacequotes, bool silent_, const char *outdir,
													 const string *extra) {
    silent = silent_;
    // extract conf name from from filename
    name = basename(fname, ".conf");
    // read file and extract all variables and values
    if (!silent) eblprint("loading configuration file: " << fname << std::endl);
    std::string content = file_to_string(fname);
	  if (extra) content += *extra;
    return read(content, bresolve, replacequotes, silent_, outdir, fname);
  }

  bool configuration::read(const std::string &content, bool bresolve,
                           bool replacequotes, bool silent_,
                           const char *outdir, const char *name_) {
    silent = silent_;
    if (name_) name = name_;
    else name = "configuration";
    // read file and extract all variables and values
    if (!extract_variables(content, smap, otxt, NULL, bresolve, replacequotes,
                           silent_))
      return false;
    // set output directory
    if (outdir) output_dir = outdir;
    else if (exists("meta_output_dir")) output_dir = get_string("meta_output_dir");
    else output_dir = dirname(name.c_str()); // set output dir to conf's dir
    if (!silent) pretty();
    return true;
  }

  bool configuration::write(const char *fname) {
    ofstream of(fname);
    if (!of) {
      eblwarn("error: failed to open " << fname << std::endl);
      return false;
    }
    // update all values in original text
    string_map_t::iterator smi = smap.begin();
    for ( ; smi != smap.end(); ++smi)
      otxt.update(smi->first, smi->second);
    // write updated text
    otxt.print(of);
    of.close();
    return true;
  }

  void configuration::resolve_variables(string_map_t &m, bool replquotes) {
    string_map_t::iterator mi = m.begin();
    for ( ; mi != m.end(); ++mi) {
      std::string val = mi->second;
      mi->second = resolve0(m, mi->first, val);
      if (replquotes)
        mi->second = replace_quotes(mi->second);
    }
  }

  void configuration::resolve(bool replquotes) {
    resolve_variables(smap, replquotes);
  }

  void configuration::resolve_bq() {
    string_map_t::iterator mi = smap.begin();
    for ( ; mi != smap.end(); ++mi) {
      std::string val = mi->second;
      mi->second = resolve_backquotes(smap, mi->first, val);
    }
  }

  const std::string &configuration::get_name() {
    return name;
  }

  void configuration::set_name(const std::string &n) {
    name = n;
  }

  const std::string &configuration::get_output_dir() {
    return output_dir;
  }

  void configuration::set_output_dir(const std::string &d) {
    output_dir = d;
  }

  void configuration::get(intg &v, const char *varname) {
    exists_throw(varname);
    v = (intg) string_to_int(get_cstr(varname));
  }

  void configuration::get(int &v, const char *varname) {
    exists_throw(varname);
    v = string_to_int(get_cstr(varname));
  }

  void configuration::get(uint &v, const char *varname) {
    exists_throw(varname);
    v = string_to_uint(get_cstr(varname));
  }

  void configuration::get(double &v, const char *varname) {
    exists_throw(varname);
    v = string_to_double(get_cstr(varname));
  }

  void configuration::get(float &v, const char *varname) {
    exists_throw(varname);
    v = string_to_float(get_cstr(varname));
  }

  void configuration::get(std::string &v, const char *varname) {
    exists_throw(varname);
    v = get_string(varname);
  }

  void configuration::get(bool &v, const char *varname) {
    exists_throw(varname);
    v = string_to_bool(get_cstr(varname));
  }

  const std::string &configuration::get_string(const char *varname) {
    exists_throw(varname);
    // remove quotes if present
    std::string s = get_cstr(varname);
    if ((s[0] == DQ) && (s[s.size() - 1] == DQ))
      s = s.substr(1, s.size() - 2);
    // remove slash preceding quotes
    size_t pos;
    while ((pos = s.rfind("\\\"")) != std::string::npos) {
      s.replace(pos, 2, "\"");
    }
    tmp_smap[varname] = s;
    return tmp_smap[varname];
  }

  const std::string &configuration::get_string(const std::string &varname) {
    return get_string(varname.c_str());
  }

  const char *configuration::get_cstring(const char *varname) {
    return get_string(varname).c_str();
  }

  double configuration::get_double(const char *varname) {
    exists_throw(varname);
    return string_to_double(get_cstr(varname));
  }

  float configuration::get_float(const char *varname) {
    exists_throw(varname);
    return string_to_float(get_cstr(varname));
  }

  bool configuration::try_get_bool(const char *varname, bool default_val) {
    if (!exists(varname)) return default_val;
    return get_uint(varname) != 0;
  }

  float configuration::try_get_float(const char *varname, float default_val) {
    if (!exists(varname)) return default_val;
    return string_to_float(get_cstr(varname));
  }

  double configuration::try_get_double(const char *varname, double default_val) {
    if (!exists(varname)) return default_val;
    return string_to_double(get_cstr(varname));
  }

  uint configuration::try_get_uint(const char *varname, uint default_val) {
    if (!exists(varname)) return default_val;
    return string_to_uint(get_cstr(varname));
  }

  int configuration::try_get_int(const char *varname, int default_val) {
    if (!exists(varname)) return default_val;
    return string_to_int(get_cstr(varname));
  }

  intg configuration::try_get_intg(const char *varname, intg default_val) {
    if (!exists(varname)) return default_val;
    return string_to_intg(get_cstr(varname));
  }

  std::string configuration::try_get_string(const char *varname,
                                            const char *default_val) {
    std::string s = default_val;
    if (!exists(varname)) return s;
    return s = get_cstr(varname);
  }

  uint configuration::get_uint(const char *varname) {
    exists_throw(varname);
    return string_to_uint(get_cstr(varname));
  }

  int configuration::get_int(const char *varname) {
    exists_throw(varname);
    return string_to_int(get_cstr(varname));
  }

  intg configuration::get_intg(const char *varname) {
    exists_throw(varname);
    return string_to_intg(get_cstr(varname));
  }

  bool configuration::get_bool(const char *varname) {
    exists_throw(varname);
    return get_uint(varname) != 0;
  }

  char configuration::get_char(const char *varname) {
    exists_throw(varname);
    std::string s = get_cstr(varname);
    return s[0];
  }

  bool configuration::exists_bool(const char *varname) {
    if (!exists(varname))	return false;
    return get_uint(varname) != 0;
  }

  bool configuration::exists_true(const char *varname) {
    if (!exists(varname)) return false;
    return get_uint(varname) != 0;
  }

  bool configuration::exists_true(const std::string &varname) {
    return exists_true(varname.c_str());
  }

  bool configuration::exists_false(const char *varname) {
    if (!exists(varname))
      return false;
    if (get_uint(varname) == 0)
      return true;
    return false;
  }

  void configuration::set(const char *varname, const char *value) {
    smap[varname] = value;
  }

  const char* configuration::get_cstr(const char *varname, bool silent_) {
    if (smap.find(varname) != smap.end())
      return smap[varname].c_str();
    char *val = getenv(varname);
    if (val) {
      if (!silent_)
        // 	cout << "using environment variable \"" << varname << "\": "
        // 			 << val << std::endl;
        return val;
    }
    return NULL;
  }

  std::vector<std::string> configuration::get_all_strings(const std::string &s) {
    string_map_t::iterator smi = smap.begin();
    std::vector<std::string> all;
    for ( ; smi != smap.end(); ++smi)
      if (smi->first.find(s) != std::string::npos)
        all.push_back(smi->first);
    return all;
  }

  bool configuration::exists(const char *varname) {
    const char *s = get_cstr(varname, true);
    // does not exist if not defined
    if (s == NULL)
      return false;
    // does not exist if empty string
    if (!strcmp(s, ""))
      return false;
    return true;
  }

  bool configuration::exists(const std::string &varname) {
    return exists(varname.c_str());
  }

  void configuration::exists_throw(const char *varname) {
    if ((smap.find(varname) == smap.end()) && (!getenv(varname)))
      eblthrow("unknown requested variable \"" << varname
               << "\" in configuration file");
  }

  void configuration::pretty() {
    eblprint("_____________________ Configuration _____________________" << std::endl);
    print_string_map(smap);
    eblprint("_________________________________________________________" << std::endl);
  }

  std::string& configuration::str() {
    desc_ = "";
    string_map_t::iterator smi = smap.begin();
    for ( ; smi != smap.end(); ++smi)
      desc_ << smi->first << " : " << smi->second << "\n";
    return desc_;
  }

  void configuration::pretty_match(const std::string &s) {
    string_map_t::iterator smi = smap.begin();
    for ( ; smi != smap.end(); ++smi)
      if (s.find(smi->first) != std::string::npos)
        eblprint(smi->first << "=" << smi->second << std::endl);
  }

  ////////////////////////////////////////////////////////////////
  // meta_configuration

  meta_configuration::meta_configuration() {
  }

  meta_configuration::~meta_configuration() {
  }

  bool meta_configuration::read(const char *fname, bool bresolve,
                                const std::string *stamp, bool replacequotes,
                                const char *resume_name, bool silent,
																const string *extra) {
    if (!silent)
      eblprint("Reading meta configuration file: " << fname << std::endl);
    // read file and extract all variables and values
    std::string content = file_to_string(fname);
		// enqueue extra content
	  if (extra) content += *extra;
		// extract variables
    if (!extract_variables(content, smap, otxt, NULL, bresolve, replacequotes,
                           silent))
      return false;
    if (!silent) {
      eblprint("loaded: " << std::endl);
      pretty();
    }
    // transpose values into list of values (a variable can be assigned a list
    // of values
    variables_to_variables_list(smap, lmap);
    // count number of possible configurations
    conf_combinations = config_combinations(lmap);
    // resolve
    if (bresolve)
      resolve(false);

    // name of entire experiment
    if (resume_name) // we are resuming an existing job
      name = resume_name;
    else {
      if (stamp && strcmp(stamp->c_str(), ""))
        name = *stamp; // override timestamp manually
      else // use current timestamp
        name = tstamp();
      name += ".";
      if (smap.find("meta_name") != smap.end())
        name += smap["meta_name"];
    }
    // name of output directory
    output_dir = "output"; // default name (other name optional)
    if (smap.find("meta_output_dir") != smap.end())
      output_dir = smap["meta_output_dir"];
    output_dir += "/";
    output_dir += name;
    return true;
  }

  std::vector<configuration>& meta_configuration::configurations() {
    eblprint("Creating all " << conf_combinations
             << " possible configurations..." << std::endl);
    // create all possible configurations
    confs.clear();
    conf_indices.assign(lmap.size(), 0); // reset conf
    for (int i = 0; i < conf_combinations; ++i) {
      std::string conf_name = name;
      std::string shortname, fullname, variables;
      set_conf_name(conf_indices, lmap, i, exists_true("meta_no_conf_id"),
                    conf_combinations, fullname, shortname, variables);
      conf_name << "_" << fullname;
      string_map_t new_smap;
      assign_current_smap(new_smap, conf_indices, lmap);
      configuration conf(new_smap, otxt, conf_name, output_dir);
      conf.set("meta_conf_shortname", shortname.c_str());
      conf.set("meta_conf_fullname", fullname.c_str());
      conf.set("meta_conf_variables", variables.c_str());
      confs.push_back(conf);
      config_indices_incr(conf_indices, lmap); // incr conf
    }
    return confs;
  }

  void meta_configuration::pretty() {
    eblprint("__________________ Meta configuration ___________________"
             << std::endl);
    print_string_map(smap);
    eblprint("_________________________________________________________"
             << std::endl);
  }

  void meta_configuration::pretty_combinations() {
    eblprint("Configuration has " << conf_combinations
             << " combinations:" << std::endl);
    string_list_map_t::iterator lmi = lmap.begin();
    std::vector<std::string>::iterator lmj;
    for ( ; lmi != lmap.end(); ++lmi) {
      if (lmi->second.size() > 1) {
        eblprint(lmi->second.size() << " " << lmi->first << ":");
        for (lmj = lmi->second.begin(); lmj != lmi->second.end(); ++lmj)
          eblprint(" " << *lmj);
        eblprint(std::endl);
      }
    }
  }

} // namespace ebl
