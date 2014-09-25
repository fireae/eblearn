/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet *
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

#ifndef EBL_LUA_H_
#define EBL_LUA_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_utils.h"
#include "ebl_preprocessing.h"

namespace ebl {

// lua_module //////////////////////////////////////////////////////////////////

//! An interface to lua modules.
template <typename T> class lua_module: public module_1_1<T> {
 public:
  //! Constructor.
  lua_module(const char *script, const char *name = "lua");
  //! destructor
  virtual ~lua_module();

  //! Forward propagation of all tensors from 'in' tensors to 'out' tensors.
  virtual void fprop(state<T> &in, state<T> &out);
  //! 1st order backward propagation of all tensors from out to in.
  virtual void bprop(state<T> &in, state<T> &out);
  //! 2nd order backward propagation of all tensors from out to in.
  virtual void bbprop(state<T> &in, state<T> &out);

  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // members
 protected:
  std::string   script_fname;
  long          ninputs;
  float       **inputs;
  long        **inputs_size;
  long          noutputs;
  float       **outputs;
  long        **outputs_size;
};

} // namespace ebl {

#include "ebl_lua.hpp"

#endif /* EBL_LUA_H_ */
