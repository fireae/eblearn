/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#include "ebl_module.h"

using namespace std;

namespace ebl {

////////////////////////////////////////////////////////////////
// module

module::module(const char *n)
    : _name(""), mout(&std::cout), merr(&std::cerr),
      silent(false), _enabled(true), gpu_support(false) {
  if (n) _name = n;
}

module::~module() {
}

const char *module::name() {
  return this->_name.c_str();
}

void module::set_name(const char *n) {
  _name = n;
}

void module::default_name(const char *n) {
  if (_name.empty())
    _name = n;
}

void module::set_output_streams(std::ostream &out, std::ostream &err) {
  mout = &out;
  merr = &err;
}

std::string module::describe() {
  std::string desc = _name; // default, just return the module's name
  return desc;
}

std::string module::describe_indent(uint indent) {
  std::string desc;
  for (uint j = 0; j < indent; ++j) desc << "\t";
  desc << this->describe();
  return desc;
}

void module::enable() {
  _enabled = true;
  eblprint( "Module " << this->name() << " is enabled." << std::endl);
}

void module::disable() {
  _enabled = false;
  eblprint( "Module " << this->name() << " is disabled." << std::endl);
}

} // end namespace ebl
