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
#include <sstream>
#include <stdio.h>
#include <map>
#include <algorithm>
#include <iomanip>

#ifdef __BOOST__
#ifndef BOOST_FILESYSTEM_VERSION
#define BOOST_FILESYSTEM_VERSION 3
#endif
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#endif

#include "stl.h"
#include "numerics.h"
#include "metaparser.h"
#include "string_utils.h"
#include "tools_utils.h"

using namespace std;

namespace ebl {

// pairtree ////////////////////////////////////////////////////////////////////

// static members
map<std::string,uint> pairtree::vars_map;
map<std::string,uint> pairtree::vals_map;
vector<std::string> pairtree::vars_vector;
vector<std::string> pairtree::vals_vector;

pairtree::pairtree(std::string &var, std::string &val)
	: variable(get_var_id(var)), value(get_val_id(val)), subvariable(0) {
}

pairtree::pairtree(uint var, uint val)
	: variable(var), value(val), subvariable(0) {
}

pairtree::pairtree()
	: variable(0), value(0), subvariable(0) {
}

pairtree::~pairtree() {
}

uint pairtree::get_var_id(const std::string &var) {
	uint id = 0;
	map<std::string,uint>::iterator i = vars_map.find(var);
	if (i == vars_map.end()) { // new variable not yed added
		id = vars_vector.size();
		vars_vector.push_back(var);
		vars_map[var] = id;
	} else {
		id = vars_map[var];
	}
	//EDEBUG("var id of " << var << " is: " << id);
	return id;
}

uint pairtree::get_var_id(const char *var) {
	std::string s = var;
	return get_var_id(s);
}

uint& pairtree::get_variable() {
	return variable;
}

uint& pairtree::get_value() {
	return value;
}

std::string& pairtree::get_variable_string() {
	return get_var(variable);
}

std::string& pairtree::get_value_string() {
	return get_val(value);
}

std::string& pairtree::get_var(uint id) {
	if (id >= vars_vector.size())
		eblerror("trying to access elements " << id << " of vector of size "
						 << vars_vector.size() << ": " << vars_vector);
	return vars_vector[id];
}

std::string& pairtree::get_val(uint id) {
	if (id >= vals_vector.size())
		eblerror("trying to access elements " << id << " of vector of size "
						 << vals_vector.size() << ": " << vals_vector);
	return vals_vector[id];
}

uint pairtree::get_val_id(const std::string &val) {
	uint id = 0;
	map<std::string,uint>::iterator i = vals_map.find(val);
	if (i == vals_map.end()) { // new variable not yed added
		id = vals_vector.size();
		vals_vector.push_back(val);
		vals_map[val] = id;
	} else {
		id = vals_map[val];
	}
	//EDEBUG("val id of " << val << " is: " << id);
	return id;
}

std::list<uint> pairtree::to_varid_list(std::list<std::string> &l) {
	std::list<uint> ret;
	for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i)
		ret.push_back(get_var_id(*i));
	return ret;
}

list<std::string> pairtree::to_vars(list<uint> &ids) {
	list<std::string> ret;
	for (list<uint>::iterator i = ids.begin(); i != ids.end(); ++i)
		ret.push_back(get_var(*i));
	return ret;
}

map<std::string,std::string> pairtree::to_pairs(map<uint,uint> &p) {
	map<std::string,std::string> ret;
	for (map<uint,uint>::iterator i = p.begin(); i != p.end(); ++i)
		ret[get_var(i->first)] = get_val(i->second);
	return ret;
}

void pairtree::add(const std::string &var, std::string &val) {
	vars[get_var_id(var)] = get_val_id(val);
}

map<uint,uint> pairtree::add(list<uint> &subvar_, map<uint,uint> &ivars) {
	EDEBUG("adding path " << to_vars(subvar_) << " with: " << to_pairs(ivars));
	list<uint> subvar = subvar_;
	uint subval;
	// the path leading to the current node
	map<uint,uint> path;
	// use first string that is in vars as subvariable
	if (subvar.size() > 0) {
		subvariable = subvar.front();
		subvar.pop_front();
		// if (ivars.find(subvariable) == ivars.end())
		// 	eblwarn("variable " << pairtree::get_var(subvariable) << " not found in "
		// 					 << to_pairs(ivars));
		while ((subvar.size() > 0) && (ivars.find(subvariable) == ivars.end())) {
			subvariable = subvar.front();
			subvar.pop_front();
		}
	}
	if (ivars.size() > 0) {
		// check if subvariable is in vars, if yes, this is a node, leaf otherwise
		if (ivars.find(subvariable) == ivars.end()) { // leaf
			// add all variables found in vars
			vars.insert(ivars.begin(), ivars.end());
		} else { // node
			subval = ivars[subvariable];
			// the var list without subvariable
			map<uint,uint> tmp(ivars);
			tmp.erase(subvariable);
			// subvariable list without current subvariable
			// get existing node
			bool found = false;
			//	map<uint,pairtree,natural_less>::iterator p;
			map<uint,pairtree,std::less<uint> >::iterator p;
			for (p = subtree.begin(); p != subtree.end(); ++p) {
				if (p->first == subval) {
					found = true;
					break ;
				}}
			if (!found) {
				// no node, add new one
				pairtree t(subvariable, subval);
				path = t.add(subvar, tmp);
				subtree[subval] = t;
			} else {
				path = p->second.add(subvar, tmp);
			}
			// add current node pair to path
			path[subvariable] = subval;
		}
	}
	return path;
}

natural_varmap pairtree::flatten(const std::string &key, natural_varmap *flat,
																 map<uint,uint> *path) {
	natural_varmap flat2;
	map<uint,uint> path2;
	if (!flat)
		flat = &flat2;
	if (!path)
		path = &path2;

	for (map<uint,pairtree>::iterator i = subtree.begin();
			 i != subtree.end(); ++i) {
		(*path)[subvariable] = i->first;
		i->second.flatten(key, flat, path);
	}
	if (subtree.size() == 0) { // leaf
		// merge path and vars
		map<uint,uint> all = vars;
		all.insert(path->begin(), path->end());
		// look for key
		map<uint,uint>::iterator k = all.find(get_var_id(key));
		if (k == all.end())
			return *flat;
		// key is found, extract key
		uint kval = k->second;
		all.erase(k);
		(*flat)[kval] = all;
	}
	return *flat;
}

varmaplist pairtree::flatten(varmaplist *flat, map<uint,uint> *path) {
	varmaplist flat2;
	map<uint,uint> path2;
	if (!flat)
		flat = &flat2;
	if (!path)
		path = &path2;

	for (map<uint,pairtree>::iterator i = subtree.begin();
			 i != subtree.end(); ++i) {
		(*path)[subvariable] = i->first;
		i->second.flatten(flat, path);
	}
	if (subtree.size() == 0) { // leaf
		// merge path and vars
		map<uint,uint> all = vars;
		all.insert(path->begin(), path->end());
		// push this map of variables onto the list of maps
		flat->push_back(all);
	}
	return *flat;
}

natural_varmap pairtree::best(const std::string &key, uint n, bool display) {
	timer t; t.start();
	natural_varmap flat = flatten(key);
	// keep only first n entries
	natural_varmap::iterator i = flat.begin();
	if (n == 0)
		n = flat.size();
	for (uint j = 0; j < n && i != flat.end(); j++, i++) ;
	flat.erase(i, flat.end());
	// display
	if (display && flat.size() > 0) {
		cout << "Best " << n << " results for \"" << key << "\":" << endl;
		pretty_flat(key, &flat);
	}
	cout << "best() computation time: " << t.elapsed() << endl;
	return flat;
}

varmaplist pairtree::best(std::list<std::string> &keys, uint n, bool display,
													int maxiter) {
	timer t; t.start();
	varmaplist flat = flatten();
	// sort
	std::list<uint> ukeys = pairtree::to_varid_list(keys);
	flat.sort(map_natural_less_uint(ukeys, vals_vector));
	// remove all occurences of i > maxiter
	if (maxiter >= 0) {
		std::string sid = "i";
		if (vars_map.find(sid) != vars_map.end()) {
			uint id = vars_map[sid];
			varmaplist::iterator i = flat.begin();
			for ( ; i != flat.end(); ) {
				std::string ival = vals_vector[(*i)[id]];
				if (atoi(ival.c_str()) > maxiter)
					i = flat.erase(i);
				else
					i++;
			}
		}
	}
	// keep only first n entries
	varmaplist::iterator i = flat.begin();
	if (n == 0)
		n = flat.size();
	for (uint j = 0; j < n && i != flat.end(); j++, i++) ;
	flat.erase(i, flat.end());
	// display
	if (display && flat.size() > 0) {
		cout << "Best " << n << " results for keys";
		for (std::list<std::string>::iterator i = keys.begin(); i != keys.end(); ++i)
			cout << " \"" << *i << "\"";
		cout << ":" << endl;
		pretty_flat(&flat, &keys);
	}
	cout << "best() computation time: " << t.elapsed() << endl;
	return flat;
}

varmaplist pairtree::best(std::list<std::string> &keys, const std::string &key,
													bool display) {
	timer t; t.start();
	varmaplist flat = flatten();
	varmaplist res;
	std::list<uint> seen; // remember which 'key' values have been seen.
	std::list<uint> ukeys = pairtree::to_varid_list(keys);
	// sort based on keys
	flat.sort(map_natural_less_uint(ukeys, vals_vector));
	// loop on sorted answers, only add each first time we see a different
	// value for key 'key'. This will only report the best of each
	// possible value of key
	uint keyval;
	varmaplist::iterator i = flat.begin();
	for ( ; i != flat.end(); i++) {
		keyval = (*i)[get_var_id(key)];
		// check if we have seen this value of 'key' before
		if (find(seen.begin(), seen.end(), keyval) != seen.end())
			continue ; // we got best result for this value already
		// if not, add it to res and seen
		res.push_back(*i);
		seen.push_back(keyval);
	}
	// display
	if (display && flat.size() > 0) {
		cout << "Best results for each value of " << key << ":" << endl;
		pretty_flat(&res, &keys);
	}
	cout << "best() computation time: " << t.elapsed() << endl;
	return res;
}

varmaplist pairtree::best(std::list<std::string> &keys, std::list<std::string> &keycomb,
													bool display) {
	timer t; t.start();
	varmaplist flat = flatten();
	varmaplist res;
	std::list<std::list<uint> > seen; // remember which 'key' values have been seen.
	std::list<uint> val;
	std::list<uint> ukeys = pairtree::to_varid_list(keys);
	std::list<uint> ukeycomb = pairtree::to_varid_list(keycomb);
	// sort based on keys
	flat.sort(map_natural_less_uint(ukeys, vals_vector));
	// loop on sorted answers, only add each first time we see a different
	// value for key 'keycomb'. This will only report the best of each
	// possible value of keycomb
	std::string keyval;
	varmaplist::iterator i = flat.begin();
	for ( ; i != flat.end(); i++) {
		// get each value of each keycomb
		val.clear();
		for (std::list<uint>::iterator j = ukeycomb.begin(); j != ukeycomb.end(); ++j)
			val.push_back((*i)[*j]);
		// check if we have seen this value combination of 'keycomb' before
		bool found = false, breaked = false;
		for (std::list<std::list<uint> >::iterator k = seen.begin(); k != seen.end();++k){
			// check that we can find an element with all values equal
			std::list<uint>::iterator j = k->begin();
			std::list<uint>::iterator v = val.begin();
			breaked = false;
			for (; j != k->end(); ++j, ++v) {
				// stop this loop if 2 strings differ
				if (*j != *v) {
					breaked = true;
					break ;
				}
			}
			if (breaked)
				continue ; // try another element
			found = true;
			break ; // if found a match, stop this loop
		}
		if (found)
			continue ; // we got best result for this value already
		// if not, add it to res and seen
		res.push_back(*i);
		seen.push_back(val);
	}
	// display
	if (display && flat.size() > 0) {
		cout << "Best results for each combination of " << keycomb << ":" << endl;
		pretty_flat(&res, &keys);
	}
	cout << "best() computation time: " << t.elapsed() << endl;
	return res;
}

// printing methods //////////////////////////////////////////////////////////

void pairtree::pretty(std::string offset) {
	std::string off = offset;
	off += "--";
	cout << off << " (" << get_var(variable) << ", "
			 << get_val(value) << ")" << endl
			 << off << " vars: ";
	for (map<uint,uint>::iterator i = vars.begin(); i != vars.end(); ++i)
		cout << "(" << vars_vector[i->first] << ", "
				 << vals_vector[i->second] << ") ";
	cout << endl << off << " subtree:" << endl;
	for (map<uint,pairtree>::iterator i = subtree.begin();
			 i != subtree.end(); ++i) {
		i->second.pretty(off);
	}
}

std::string pairtree::flat_to_string(const std::string key, natural_varmap *flat) {
	std::ostringstream s;
	if (!flat) return s.str();
	s << "________________________________________________________" << endl;
	for (natural_varmap::iterator i = flat->begin(); i != flat->end(); ++i) {
		s << vars_vector[i->first] << ": ";
		s << map_to_string2(i->second);
		s << endl;
		s << "________________________________________________________" << endl;
	}
	return s.str();
}

std::string pairtree::flat_to_string(varmaplist *flat_,
																		 std::list<std::string> *keys) {
	std::ostringstream s;
	if (!flat_) return s.str();
	std::list<uint> ukeys;
	if (keys) ukeys = pairtree::to_varid_list(*keys);
	s << "________________________________________________________" << endl;
	varmaplist flat = *flat_; // make a copy
	uint j = 1;
	for (varmaplist::iterator i = flat.begin(); i != flat.end(); ++i, ++j) {
		s << j << ":";
		// first display keys in order
		if (keys) {
			for (std::list<uint>::iterator k = ukeys.begin(); k != ukeys.end(); ++k) {
				map<uint,uint>::iterator key = i->find(*k);
				if (key != i->end()) {
					// display key
					s << " " << vars_vector[key->first] << ": " << vals_vector[key->second];
					// remove key
					i->erase(key);
				}
			}
		}
		s << endl;
		for (map<uint,uint>::iterator j = i->begin(); j != i->end(); ++j) {
			s << get_var(j->first) << ": " << get_val(j->second) << endl;
		}
		s << "________________________________________________________" << endl;
	}
	return s.str();
}

void pairtree::pretty_flat(const std::string key, natural_varmap *flat) {
	natural_varmap flat2;
	if (!flat) {
		flat2 = flatten(key);
		flat = &flat2;
	}
	cout << flat_to_string(key, flat);
}

void pairtree::pretty_flat(varmaplist *flat, std::list<std::string> *keys) {
	varmaplist flat2;
	if (!flat) {
		flat2 = flatten();
		flat = &flat2;
	}
	cout << flat_to_string(flat, keys);
}

uint pairtree::get_max_uint(const std::string &var) {
	uint max = 0;
	uint varid = get_var_id(var);
	// find max locally
	map<uint,uint>::iterator i = vars.find(varid);
	if (i != vars.end())
		max = std::max(max, string_to_uint(vals_vector[i->second]));
	// find max is subtrees
	for (map<uint,pairtree>::iterator i = subtree.begin();
			 i != subtree.end(); ++i) {
		if (subvariable == varid)
			max = std::max(max, string_to_uint(vals_vector[i->first]));
		max = std::max(max, i->second.get_max_uint(var));
	}
	return max;
}

bool pairtree::exists(const std::string &var) {
	// check locally
	uint varid = get_var_id(var);
	map<uint,uint>::iterator i = vars.find(varid);
	if (i != vars.end())
		return true;
	// check in subtrees
	for (map<uint,pairtree>::iterator i = subtree.begin();
			 i != subtree.end(); ++i) {
		if (subvariable == varid)
			return true;
		if (i->second.exists(var))
			return true;
	}
	return false;
}

bool pairtree::delete_pair(const char *var, const char *value) {
	uint varid = get_var_id(var);
	uint valid = get_val_id(value);
	for (map<uint,pairtree>::iterator i = subtree.begin();
			 i != subtree.end(); ++i) {
		if (subvariable == varid // same var
				&& i->first == valid) { // same value
			subtree.erase(i); // found var/value, delete it
			return true;
		}
		i->second.delete_pair(var, value);
	}
	return false;
}

map<uint,pairtree>& pairtree::get_subtree() {
	return subtree;
}

////////////////////////////////////////////////////////////////
// metaparser

metaparser::metaparser() : separator(VALUE_SEPARATOR) {
	hierarchy.push_back(pairtree::get_var_id("job"));
	hierarchy.push_back(pairtree::get_var_id("i"));
}

metaparser::~metaparser() {
}

bool metaparser::parse_log(const std::string &fname,
													 std::list<std::string> *sticky,
													 std::list<std::string> *watch) {
	ifstream in(fname.c_str());
	std::string s, var, val;
	uint varid, valid;
	char separator = VALUE_SEPARATOR;
	std::string::size_type itok, stok;
	map<uint,uint> vars, stick;
	std::list<uint> usticky;
	std::list<uint> uwatch;
	if (sticky) usticky = pairtree::to_varid_list(*sticky);
	if (watch) uwatch = pairtree::to_varid_list(*watch);

	if (!in) {
		cerr << "warning: failed to open " << fname << endl;
		return false;
	}
	// parse all lines
	while (!in.eof()) {
		// extract all variables for this line
		getline(in, s);
		istringstream iss(s, istringstream::in);
		vars.clear(); // clear previous variables
		// keep sticky variables from previous lines in this new line
		// hierarchy keys are sticky by default, and additional sticky
		// variables are defined by 'sticky' list.
		vars.insert(stick.begin(), stick.end());
		// loop over variable/value pairs
		itok = s.find(separator);
		while (itok != std::string::npos) { // get remaining values
			stok = s.find_last_of(' ', itok - 1);
			var = s.substr(stok + 1, itok - stok - 1);
			stok = s.find_first_of(" \n\t\0", itok + 1);
			if (stok == std::string::npos)
				stok = s.size();
			val = s.substr(itok + 1, stok - itok - 1);
			s = s.substr(stok);
			itok = s.find(separator);
			varid = pairtree::get_var_id(var);
			// if not in watch list, ignore
			if (watch && watch->size()
					&& find(uwatch.begin(), uwatch.end(), varid) == uwatch.end())
				continue ;
			// remember var/val
			valid = pairtree::get_val_id(val);
			vars[varid] = valid;
			// if a key, make it sticky
			if (find(hierarchy.begin(), hierarchy.end(), varid) != hierarchy.end())
				stick[varid] = valid;
			// if sticky, remember value
			if (sticky && find(usticky.begin(), usticky.end(), varid) != usticky.end())
				stick[varid] = valid;
		}
		// add variables to tree, and remember the path to the leaf
		tree.add(hierarchy, vars);
	}
	in.close();
#ifdef __DEBUG__
	tree.pretty();
#endif
	return true;
}

int metaparser::get_max_iter() {
	if (!tree.exists("i"))
		return -1;
	return (int) tree.get_max_uint("i");
}

int metaparser::get_max_common_iter(configuration &conf,
																		const std::string &dir) {
	std::list<std::string> sticky, watch;
	// get list of sticky variables
	if (conf.exists("meta_sticky_vars")) {
		sticky = string_to_stringlist(conf.get_string("meta_sticky_vars"));
		//cout << "Sticky variables: " << stringlist_to_string(sticky) << endl;
	}
	// get list of variables to watch for
	if (conf.exists("meta_watch_vars")) {
		watch = string_to_stringlist(conf.get_string("meta_watch_vars"));
		//cout << "Variables to watch (ignoring others): "
		//<< stringlist_to_string(watch) << endl;
	}
	parse_logs(dir, &sticky, &watch);
	return get_max_common_iter();
}

int metaparser::get_max_common_iter() {
	if (!tree.exists("i")) return -1;
	int minmax = limits<int>::max();
	// assuming that "job" is first level and "i" second one:
	for (map<uint,pairtree>::iterator i = tree.get_subtree().begin();
			 i != tree.get_subtree().end(); ++i) {
		// for job i, find maximum i
		int max = 0;
		for (map<uint,pairtree>::iterator j = i->second.get_subtree().begin();
				 j != i->second.get_subtree().end(); ++j) {
			max = std::max(max, string_to_int(pairtree::get_val(j->first)));
		}
		// find minimum of the maximums
		minmax = MIN(minmax, max);
	}
	return minmax;
}

natural_varmap metaparser::best(const std::string &key, uint n, bool display) {
	return tree.best(key, n, display);
}

varmaplist metaparser::best(std::list<std::string> &keys, uint n, bool display) {
	return tree.best(keys, n, display);
}

void metaparser::process(const std::string &dir) {
	std::string confname, jobs_info;
	configuration conf;
	// find all configurations in non-sorted order, the meta conf
	// should be the first element.
	std::list<std::string> *confs = find_fullfiles(dir, ".*[.]conf", NULL, false);
	if (confs && confs->size() > 0) {
		confname = confs->front();
		delete confs;
		conf.read(confname.c_str(), true, false);
	} else {
		cerr << "warning: could not find a .conf file describing how to analyze "
				 << "this directory" << endl;
	}
	// check if the hierarchy is manually specified
	if (conf.exists("meta_hierarchy")) {
		hierarchy.clear();
		std::list<std::string> l =
			string_to_stringlist(conf.get_string("meta_hierarchy"));
		cout << "Using user-specified hierarchy: " << l << endl;
		for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i)
			hierarchy.push_back(pairtree::get_var_id(i->c_str()));
	}
	// analyze
	int maxiter = 0, maxiter_common = 0;
	varmaplist besteach, best_common;
	varmaplist best = analyze(conf, dir, maxiter, besteach,
														conf.exists_true("meta_display_all"),
														&maxiter_common, &best_common);
	send_report(conf, dir, best, maxiter, confname, jobs_info, 0, 0, 0,
							&besteach, &best_common, &maxiter_common);
}

void metaparser::organize_plot(std::list<std::string> &names, varmaplist &flat,
															 pairtree &p, std::string &job) {
	// subvar represents the hierarchy, with "job" followed by last of 'names'
	std::list<uint> subvar;
	std::list<uint> unames = pairtree::to_varid_list(names);
	uint jobid = pairtree::get_var_id(job.c_str());
	subvar.push_back(jobid);
	subvar.push_back(unames.back());
	// loop over each set of variables in flat
	varmaplist::iterator i = flat.begin();
	for ( ; i != flat.end(); ++i) {
		// extract and merge all variables found in names (except last one)
		std::string name;
		int l = 0;
		bool first = true;
		for (std::list<uint>::iterator j = unames.begin();
				 l < (int) unames.size() - 1 && j != unames.end(); ++j, l++) {
			map<uint,uint>::iterator k = i->find(*j);
			if (k != i->end()) {
				if (!first)
					name << "_";
				else
					first = false;
				name << pairtree::vars_vector[*j] << "_"
						 << pairtree::vals_vector[k->second];
				i->erase(k);
			}
		}
		// rename job with new name
		(*i)[jobid] = pairtree::get_val_id(name);
		// add this to tree
		p.add(subvar, *i);
	}
}

// write plot files, using gpparams as additional gnuplot parameters
void metaparser::write_plots(configuration &conf, const char *dir,
														 pairtree *p, std::string *prefix) {
	pairtree *pt = &tree;
	if (p) pt = p;
	std::string gpparams = "", gpterminal = "pdf", gpfont = "Times=10",
		gpline = "";
	bool usefont = false;
	if (conf.exists("meta_gnuplot_params"))
		gpparams = conf.get_string("meta_gnuplot_params");
	if (conf.exists("meta_gnuplot_terminal"))
		gpterminal = conf.get_string("meta_gnuplot_terminal");
	if (conf.exists("meta_gnuplot_font")) {
		gpfont = conf.get_string("meta_gnuplot_font");
		usefont = true;
	}
	if (conf.exists("meta_gnuplot_line"))
		gpline = conf.get_string("meta_gnuplot_line");

	std::string colsep = "\t";
	std::string gnuplot_config1 = "clear ; set terminal ";
	gnuplot_config1 += gpterminal;
	gnuplot_config1 += "; \n";
	std::string gnuplot_config2 = " set output \"";
	std::string gnuplot_config3 = ".";
	gnuplot_config3 += gpterminal;
	gnuplot_config3 += "\" ;	plot ";
	// a map of file pointers for .plot and .p files
	map<std::string,ofstream*> plotfiles, pfiles;
	std::list<std::string> plist; // list p file names

	if (!pt->exists("job"))
		cerr << "warning: expected a \"job\" variable to differentiate each "
				 << "curve in the plots but not found." << endl;
	bool iexists = true;
	if (!pt->exists("i")) {
		iexists = false;
		cerr << "warning: expected a \"i\" variable for the x-axis "
				 << "in the plots but not found." << endl;
	}
	// we assume that the tree has been extracted using those keys in that
	// order: job, i
	// loop on each job
	uint ijob = 0;
	t_subtree &st = pt->get_subtree();
	for (t_subtree::iterator i = st.begin(); i != st.end(); ++i, ++ijob) {
		uint job = i->first;
		// flatten remaining tree based on key "i"
		std::string ikey = "i";
		// if key doesn't exist, define it
		if (!iexists) {
			std::string ival;
			ival << ijob;
			i->second.add(ikey, ival);
		}
		natural_varmap flat = i->second.flatten(ikey);
		map<std::string,bool> initiated;
		// loop on all i
		for (natural_varmap::iterator j = flat.begin(); j != flat.end(); ++j) {
			// convert i to double
			double ival = string_to_double(pairtree::vals_vector[j->first]);
			// loop on all variables
			for (map<uint,uint>::iterator k = j->second.begin();
					 k != j->second.end(); ++k) {
				std::string kvar = pairtree::vars_vector[k->first];
				std::string kval = pairtree::vals_vector[k->second];
				// try to convert value to double
				istringstream iss(kval);
				double val;
				iss >> val;
				if (iss.fail())
					continue ; // not a number, do not plot
				// check that p file is open
				if (pfiles.find(kvar) == pfiles.end()) {
					// not open, add file
					std::string fname;
					if (dir)
						fname << dir << "/";
					if (prefix)
						fname << *prefix;
					fname << kvar << ".p";
					ofstream *outp = new ofstream(fname.c_str());
					if (!outp) {
						cerr << "warning: failed to open " << fname << endl;
						continue ; // keep going
					}
					pfiles[kvar] = outp;
					fname = "";
					if (prefix)
						fname << *prefix;
					fname << kvar << ".p";
					plist.push_back(fname);
					*outp << gnuplot_config1;
					*outp << gpparams;
					*outp << ";" << gnuplot_config2;
					if (prefix)
						*outp << *prefix;
					*outp << kvar << gnuplot_config3;
				}
				// check that plot file is open
				if (plotfiles.find(kvar) == plotfiles.end()) {
					// not open, add file
					std::string fname;
					if (dir)
						fname << dir << "/";
					if (prefix)
						fname << *prefix;
					fname << kvar << ".plot";
					plotfiles[kvar] = new ofstream(fname.c_str());
					if (!plotfiles[kvar]) {
						cerr << "warning: failed to open " << fname << endl;
						continue ; // keep going
					}
				}
				// if not initiated, add job plot description in p file
				if (initiated.find(kvar) == initiated.end()) {
					initiated[kvar] = true;
					ofstream &outp = *pfiles[kvar];
					if (ijob > 0)
						outp << ", ";
					outp << "\"";
					if (prefix)
						outp << *prefix;
					outp << kvar << ".plot\" using 1:"
							 << ijob + 2 << " title \"";
					if (usefont) outp << "{/" << gpfont;
					std::string title = pairtree::vals_vector[job];
					// if (i->second.exists("meta_conf_variables"))
					// 	title = pairtree::vals_vector[i->second.get_var_id
					// 																("meta_conf_variables")];
					for (uint i = 0; i < title.size(); ++i)
						if (title[i] == '_') title[i] = ' ';
					outp << " " << title;
					if (usefont) outp << "}";
					outp << "\" with linespoints " << gpline;
				}
				// add this value into plot file
				ofstream &outp = *plotfiles[kvar];
				// first add abscissa value i
				outp << ival << colsep;
				// then fill with empty values until reaching job's column
				for (uint c = 0; c < ijob; ++c)
					outp << "?" << colsep;
				// finally add job's value
				outp << val << endl;
			}
		}
	}
	// close and delete all file pointers
	for (map<std::string,ofstream*>::iterator i = pfiles.begin();
			 i != pfiles.end(); ++i)
		if (*i->second) {
			(*i->second).close();
			delete i->second;
		}
	for (map<std::string,ofstream*>::iterator i = plotfiles.begin();
			 i != plotfiles.end(); ++i)
		if (*i->second) {
			(*i->second).close();
			delete i->second;
		}
	// convert all plots to pdf using gnuplot
	std::string cmd;
	for (std::list<std::string>::iterator i = plist.begin(); i != plist.end(); ++i) {
		cout << "Creating plot from " << *i << endl;
		cmd = "";
		cmd << "cd " << dir << " && cat " << *i << " | gnuplot && sleep .1";
		cout << "with cmd: " << cmd << endl;
		int ret = std::system(cmd.c_str());
		if (ret < 0)
			cerr << "warning: command failed" << endl;
	}
	if (plist.size() <= 0)
		cerr << "Warning: no plots created." << endl;
}

void metaparser::parse_logs(const std::string &root, std::list<std::string> *sticky,
														std::list<std::string> *watch) {
	cout << "Parsing all .log files recursively..." << endl;
	std::list<std::string> *fl = find_fullfiles(root, ".*[.]log");
	if (fl) {
		for (std::list<std::string>::iterator i = fl->begin(); i != fl->end(); ++i) {
			cout << "Parsing " << *i << endl;
			parse_log(*i, sticky, watch);
		}
		delete fl;
	}
}

varmaplist metaparser::analyze(configuration &conf, const std::string &dir,
															 int &maxiter, varmaplist &besteach,
															 bool displayall,
															 int *maxiter_common,
															 varmaplist *best_common) {
	std::list<std::string> sticky, watch, keycomb;
	varmaplist best;
	// get list of sticky variables
	if (conf.exists("meta_sticky_vars"))
		sticky = string_to_stringlist(conf.get_string("meta_sticky_vars"));
	// get list of variables to watch for
	if (conf.exists("meta_watch_vars"))
		watch = string_to_stringlist(conf.get_string("meta_watch_vars"));
	// default sticky list
	sticky.push_back("meta_conf_variables");
	// default watch list
	std::string job = "job";
	if (conf.exists("meta_job_var")) job = conf.get_string("meta_job_var");
	for (list<string>::iterator i = sticky.begin(); i != sticky.end(); ++i)
		watch.push_back(*i);
	cout << "Sticky variables: " << stringlist_to_string(sticky) << endl;
	cout << "Variables to watch (ignoring others): "
			 << stringlist_to_string(watch) << endl;
	parse_logs(dir, &sticky, &watch);
	maxiter = get_max_iter();
	if (maxiter_common)
		*maxiter_common = get_max_common_iter();
	if (!conf.exists("meta_minimize"))
		cerr << "Warning: meta_minimize not defined, not attempting to determine"
				 << " variables minimum." << endl;
	else {
		std::list<std::string> keys =
			string_to_stringlist(conf.get_string("meta_minimize"));
		EDEBUG("minimizing keys in that order: " << keys);
		// display all (sorted) results if required
		if (displayall) {
			cout << "All sorted results at iteration " << maxiter << ":" << endl;
			varmaplist b = tree.best(keys, 0, true);
		}
		if (conf.exists_true("meta_ignore_iter0")) { // ignore up until iter0's results
			int i0 = conf.get_int("meta_ignore_iter0");
			int miter = get_max_iter();
			for (int i = 0; i <= i0 && i < miter; ++i) {
				std::string s;
				s << i;
				tree.delete_pair("i", s.c_str());
			}
		}
		// get best value of each job
		if (conf.exists("meta_best_keycomb")) {
			keycomb = string_to_stringlist(conf.get_string("meta_best_keycomb"));
			cout << "Computing best combinations of " << keycomb << " ..." << endl;
			besteach = tree.best(keys, keycomb, true);
		} else {
			cout << "Computing best answers..." << endl;
			besteach = tree.best(keys, job, true);
		}
		// get best values to be minimized
		uint nbest = conf.exists("meta_nbest") ? conf.get_uint("meta_nbest") : 1;
		best = tree.best(keys, std::max((uint) 1, nbest));
		if (best_common && maxiter_common)
			*best_common = tree.best(keys, std::max((uint) 1, nbest), false,
															 *maxiter_common);

		// save best files
		save_best(conf, dir, "best", best, false);
		save_best(conf, dir, "best_each", besteach, true);
	}
	return best;
}

void metaparser::save_best(configuration &conf,
													 const std::string &dir, const char *dir2,
													 varmaplist &best, bool use_conf_name) {
	ostringstream dirbest, tmpdir, cmd;
	std::string job;
	int ret;

	// save best weights
	dirbest << dir << "/" << dir2;
	cmd << "rm -Rf " << dirbest.str(); // remove previous best
	ret = std::system(cmd.str().c_str());
	mkdir_full(dirbest.str().c_str());
	uint j = 1;
	for (varmaplist::iterator i = best.begin(); i != best.end(); ++i, ++j) {
		// find job name
		uint jobid = pairtree::get_var_id("job");
		if (i->find(jobid) == i->end()) { // not found, continue
			eblwarn("not saving files for best job #" << j
							<< " because \"" << job << "\" variable id not found");
			continue ; // can't do anything without job name
		} else job = pairtree::vals_vector[i->find(jobid)->second];
		// determine directory name to save to
		tmpdir.str("");
		if (use_conf_name) // each directory is named after conf
			tmpdir << dirbest.str() << "/" << job << "/";
		else // each directory is the best place number
			tmpdir << dirbest.str() << "/" << setfill('0') << setw(2) << j << "/";
		mkdir_full(tmpdir.str().c_str());
		// look for conf filename to save
		uint configid = pairtree::get_var_id("config");
		if (i->find(configid) != i->end()) { // found config
			cmd.str("");
			cmd << "cp " << pairtree::vals_vector[i->find(configid)->second]
					<< " " << tmpdir.str();
			ret = std::system(cmd.str().c_str());
		} else eblwarn("config variable not defined");
		// look for classes filename to save
		if (conf.exists("classes")) {
			cmd.str("");
			cmd << "cp " << conf.get_string("classes") << " " << tmpdir.str();
			ret = std::system(cmd.str().c_str());
			if (ret < 0)
				cerr << "warning: failed to copy classes with cmd: "
						 << cmd.str() << endl;
		} else eblwarn("classes variable not defined");

		// uint classesid = pairtree::get_var_id("classes");
		// if (i->find(classesid) != i->end()) { // found classes
		//   cmd.str("");
		//   cmd << "cp " //<< dir << "/" << job << "/"
		//       << pairtree::vals_vector[i->find(classesid)->second] << " " << tmpdir.str();
		//   ret = std::system(cmd.str().c_str());
		//   if (ret < 0)
		//     cerr << "warning: failed to copy classes with cmd: "
		// 	 << cmd.str() << endl;
		// } else eblwarn("classes file not found, classes="
		// 				 << conf.get_string("classes"));
		// save out log
		cmd.str("");
		cmd << "cp " << dir << "/" << job << "/"
				<< "out_" << job << ".log " << tmpdir.str();
		ret = std::system(cmd.str().c_str());
		// copy all files that match save patter
		uint pat_id = pairtree::get_var_id("save_pattern");
		if (i->find(pat_id) != i->end()) { // found variable
			cmd.str("");
			cmd << "cp " << dir << "/" << job << "/"
					<< pairtree::vals_vector[i->find(pat_id)->second]
					<< "* " << tmpdir.str();
			ret = std::system(cmd.str().c_str());
		}
		// look for weights filename to save
		uint savedid = pairtree::get_var_id("saved");
		if (i->find(savedid) != i->end()) { // found weights
			cmd.str("");
			cmd << "cp " << dir << "/" << job << "/"
					<< pairtree::vals_vector[i->find(savedid)->second]
					<< " " << tmpdir.str();
			ret = std::system(cmd.str().c_str());
			// add weights filename into configuration
#ifdef __BOOST__
			if ((i->find(configid) != i->end()) &&
					(i->find(savedid) != i->end())) {
				boost::filesystem::path p(pairtree::vals_vector[i->find(configid)->second]);
				cmd.str("");
				cmd << "echo \"weights_file="
						<< pairtree::vals_vector[i->find(savedid)->second]
						<< " # variable added by metarun\n\" >> "
						<< tmpdir.str() << "/" << p.leaf();
				ret = std::system(cmd.str().c_str());
			}
#endif
		}
	}
	// tar all best files
	tar(dirbest.str(), dir);
}

void metaparser::send_report(configuration &conf, const std::string dir,
														 varmaplist &best, int maxiter,
														 std::string conf_fullfname, std::string jobs_info,
														 uint nrunning, double maxminutes,
														 double minminutes, varmaplist *besteach,
														 varmaplist *best_common, int *maxiter_common) {
	ostringstream cmd;
	std::string tmpfile = "report.tmp";
	std::string extension = "pdf"; // plots extension
	std::string job = "job";
	if (conf.exists("meta_job_var")) job = conf.get_string("meta_job_var");
	if (conf.exists("meta_gnuplot_terminal"))
		extension = conf.get_string("meta_gnuplot_terminal");

	if (conf.exists_bool("meta_send_email")) {
		if (!conf.exists("meta_email")) {
			cerr << "undefined email address, not sending report." << endl;
			return ;
		}
		// write body of email
		cmd.str("");
		cmd << "rm -f " << tmpfile; // remove tmp file first
		int ret = std::system(cmd.str().c_str());
		// print summary infos
		cmd.str("");
		cmd << "echo \"Iteration: " << maxiter << endl;
		cmd << "Jobs running: " << nrunning << endl;
		cmd << "Min running time: " << minminutes << " mins ("
				<< minminutes / 60 << " hours) (" << minminutes / (60 * 24)
				<< " days)" << endl;
		cmd << "Max running time: " << maxminutes << " mins ("
				<< maxminutes / 60 << " hours) (" << minminutes / (60 * 24)
				<< " days)" << endl;
		cout << cmd.str();
		cmd << "\" >> " << tmpfile;
		std::system(cmd.str().c_str());
		// print best results at any iteration
		std::list<std::string> keys =
			string_to_stringlist(conf.get_string("meta_minimize"));
		if (best.size() > 0) {
			cmd.str("");
			cmd << "echo \"Best " << best.size() << " results at iteration "
					<< maxiter << ":" << endl;
			cmd << pairtree::flat_to_string(&best, &keys) << "\"";
			std::system(cmd.str().c_str()); // print on screen
			cmd << " >> " << tmpfile;
			std::system(cmd.str().c_str());
		}
		// print best results at maximum common iter
		if (best_common && maxiter_common && best_common->size() > 0) {
			cmd.str("");
			cmd << "echo \"Best " << best_common->size() << " results at iteration "
					<< *maxiter_common << " (maximum common iteration):" << endl;
			cmd << pairtree::flat_to_string(best_common, &keys) << "\"";
			std::system(cmd.str().c_str()); // print on screen
			cmd << " >> " << tmpfile;
			std::system(cmd.str().c_str());
		}
		// print best of each job
		if (besteach) {
			cmd.str("");
			cmd << "echo \"Best result of each job at iteration " << maxiter << ":"
					<< endl << pairtree::flat_to_string(besteach, &keys) << "\"";
			std::system(cmd.str().c_str()); // print on screen
			cmd << " >> " << tmpfile;
			std::system(cmd.str().c_str());
			// write plots of best of each job for given plot axis
			if (conf.exists("meta_plot_keys")) {
				pairtree p;
				std::list<std::string> plot_keys =
					string_to_stringlist(conf.get_string("meta_plot_keys"));
				std::string prefix;
				prefix << plot_keys.back() << "_";
				organize_plot(plot_keys, *besteach, p, job);
				write_plots(conf, dir.c_str(), &p, &prefix);
			}
		}
		// print err logs
		std::list<std::string> *errlogs = find_fullfiles(dir, ".*[.]errlog");
		if (errlogs) {
			cmd.str("");
			cmd << "echo \"Errors / Warnings:\"";
			cmd << " >> " << tmpfile;
			std::system(cmd.str().c_str());
			for (std::list<std::string>::iterator ierr = errlogs->begin();
					 ierr != errlogs->end(); ++ierr) {
				cmd.str("");
				cmd << "echo \"\n" << *ierr << ":\"";
				cmd << " >> " << tmpfile;
				std::system(cmd.str().c_str());
				cmd.str("");
				cmd << "cat " << *ierr << " >> " << tmpfile;
				std::system(cmd.str().c_str());
			}
		}
		// print jobs infos
		cmd.str("");
		cmd << "echo \"\nJobs running: " << nrunning << endl;
		cmd << jobs_info << endl;
		cout << cmd.str();
		cmd << "\" >> " << tmpfile;
		std::system(cmd.str().c_str());
		// create plot files
		write_plots(conf, dir.c_str());
		// print metaconf
		cmd.str("");
		cmd << "echo \"\nMeta Configuration (" << conf_fullfname << "):\""
				<< " >> " << tmpfile;
		std::system(cmd.str().c_str());
		cmd.str("");
		cmd << "cat " << conf_fullfname << " >> " << tmpfile;
		std::system(cmd.str().c_str());
		// create command
		cmd.str("");
		cmd << "cat " << tmpfile << " | mutt " << conf.get_string("meta_email");
		// subject of email
		cmd << " -s \"MetaRun " << conf.get_name() << "\"";
		// attach files
		if (!conf.exists_false("meta_send_best") && best.size() > 0)
			cmd << " -a " << dir << "/best.tgz";
		// attach logs
		if (!conf.exists_false("meta_send_logs")
				&& tar_pattern(dir, dir, "logs.tgz", ".*[.]log"))
			cmd << " -a " << dir << "/logs.tgz";
		// attach plots
		std::string pat;
		pat << ".*[.]" << extension;
		std::list<std::string> *plots = find_fullfiles(dir, pat.c_str());
		if (plots) {
			for (std::list<std::string>::iterator i = plots->begin();
					 i != plots->end(); ++i)
				cmd << " -a " << *i;
			delete plots;
		}
		// send email
		cout << "Sending email report to " << conf.get_string("meta_email")
				 << ":" << endl;
		cout << cmd.str() << endl;
		std::system(cmd.str().c_str());
	}
}

} // end namespace ebl
