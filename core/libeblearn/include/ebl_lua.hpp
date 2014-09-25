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

#include <stdlib.h>

#ifdef __LUA__

#include "TH/TH.h"
#undef inline
#include <lua.h>
#include <luaT.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef __cplusplus
extern "C" {
#endif

  extern lua_State *sl_stack;

  void sl_init(void);
  void sl_cleanup(void);
  int sl_dofile(const char *file);
  int sl_dostring(const char *std::string);
  lua_State * sl_getstack(void);
  void sl_callfunc(const char *funcname, int nargs, int nouts);

#ifdef __cplusplus
}
#endif

static bool lua_init_done = false;
#endif

namespace ebl {

// lua_module //////////////////////////////////////////////////////////////////

template <typename T>
lua_module<T>::lua_module(const char *script, const char *name_)
    : module_1_1<T>(name_), script_fname(script), ninputs(0), noutputs(0) {
#ifdef __LUA__
  // map neuflow
  bool neuflow = false;

  // init
  if (!lua_init_done) {
    sl_init();
    lua_init_done = true;
  }

  // load script
  printf("parsing Lua file: %s \n", script);
  int err = sl_dofile(script);
  if (err) {
    printf("could not load lua file: verify script \n");
    exit(1);
  }

  // initialize neuFlow: init()
  lua_getfield(sl_stack, LUA_GLOBALSINDEX, script);
  lua_getfield(sl_stack, -1, "init");
  lua_pushboolean(sl_stack, neuflow);
  lua_call(sl_stack, 1, 0);
  lua_pop(sl_stack, 1);

  // get pointers to inputs
  lua_getfield(sl_stack, LUA_GLOBALSINDEX, script);
  lua_getfield(sl_stack, -1, "inputs");
  int nbstates = lua_objlen(sl_stack, -1);
  this->inputs = (float **)malloc(sizeof(float *) * nbstates);
  this->inputs_size = (long **)malloc(sizeof(long *) * nbstates);
  printf("--> network has %d inputs: \n", nbstates);
  for (int i=0; i<nbstates; i++) {
    // grab input i
    lua_rawgeti(sl_stack, -1, i+1);
    THFloatTensor *tensor = (THFloatTensor *)
        luaT_toudata(sl_stack, -1,
                     luaT_checktypename2id(sl_stack, "torch.FloatTensor"));
    lua_pop(sl_stack, 1);
    // get raw pointer
    this->inputs[i] = THFloatTensor_data(tensor);
    // print dims
    this->inputs_size[i] = (long *)malloc(sizeof(long)*3);
    this->inputs_size[i][2] = 1;
    for (int s=0; s<THFloatTensor_nDimension(tensor); s++) {
      this->inputs_size[i][s] = tensor->size[s];
    }
    this->ninputs = nbstates;
    printf("    @state %d : %ldx%ldx%ld \n", i, tensor->size[0],
           tensor->size[1], tensor->size[2]);
  }
  printf("\n");
  lua_pop(sl_stack, 2);

  // get pointers to outputs
  lua_getfield(sl_stack, LUA_GLOBALSINDEX, script);
  lua_getfield(sl_stack, -1, "outputs");
  this->outputs = (float **)malloc(sizeof(float *) * nbstates);
  this->outputs_size = (long **)malloc(sizeof(long *) * nbstates);
  printf("--> network has %d outputs: \n", nbstates);
  for (int i=0; i<nbstates; i++) {
    // grab output i
    lua_rawgeti(sl_stack, -1, i+1);
    THFloatTensor *tensor = (THFloatTensor *)
        luaT_toudata(sl_stack, -1,
                     luaT_checktypename2id(sl_stack, "torch.FloatTensor"));
    lua_pop(sl_stack, 1);
    // get raw pointer
    this->outputs[i] = THFloatTensor_data(tensor);
    // print dims
    this->outputs_size[i] = (long *)malloc(sizeof(long)*3);
    this->outputs_size[i][2] = 1;
    for (int s=0; s<THFloatTensor_nDimension(tensor); s++) {
      this->outputs_size[i][s] = tensor->size[s];
    }
    this->noutputs = nbstates;
    printf("    @state %d : %ldx%ldx%ld \n", i, tensor->size[0], tensor->size[1], tensor->size[2]);
  }
  printf("\n");
  lua_pop(sl_stack, 2);
#endif
}

template <typename T>
lua_module<T>::~lua_module() {
  free(this->inputs);
  free(this->outputs);
  free(this->inputs_size);
  free(this->outputs_size);
}

template <typename T>
void lua_module<T>::fprop(state<T> &in, state<T> &out) {
  EDEBUG(this->name() << ": in " << in << " out " << out);
#ifdef __LUA__
  // get inputs
  long p = 0;
  cout << "ebl inputs -> lua" << endl;
  for (typename state<T>::iterator i = in.begin(); i != in.end(); ++i) {
    T *src = i->x.idx_ptr();
    cout << i->x << endl;
    long n = inputs_size[p][0] * inputs_size[p][1] * inputs_size[p][2];
    for (long k=0; k < n; k++)
      this->inputs[p][k] = src[k];
    p++;
  }

  // do fprop
  lua_getfield(sl_stack, LUA_GLOBALSINDEX, this->script_fname.c_str());
  lua_getfield(sl_stack, -1, "fprop");
  lua_pushboolean(sl_stack, true);
  lua_call(sl_stack, 1, 0);
  lua_pop(sl_stack, 1);

  // copy output
  p = 0;
  out.resize(in);
  cout << "lua outputs -> ebl" << endl;
  for (typename state<T>::iterator o = out.begin();
       o != out.end(); ++o) {
    // resize output
    Tstate &oo = *o;

    idxdim d(this->outputs_size[p][0], this->outputs_size[p][1], this->outputs_size[p][2]);
    if (oo.x.get_idxdim() != d) oo.resize(d);
    T *dst = oo.x.idx_ptr();
    cout << o->x << endl;

    long n = outputs_size[p][0] * outputs_size[p][1] * outputs_size[p][2];
    for (long k = 0; k < n; k++)
      dst[k] = this->outputs[p][k];
    p++;
  }
#else
  eblerror("trying to use lua_module but this was not compiled with lua"
           << ", recompile with it");
#endif
}

template <typename T>
void lua_module<T>::bprop(state<T> &in, state<T> &out) {
  not_implemented();
}

template <typename T>
void lua_module<T>::bbprop(state<T> &in, state<T> &out) {
  not_implemented();
}

template <typename T>
std::string lua_module<T>::describe() {
  std::string desc;
  desc << "lua module " << this->name();
  return desc;
}

} // end namespace ebl
