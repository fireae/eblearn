/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include "dataset.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // Helper functions

  void build_fname(std::string &ds_name, const char *fname,
                   std::string &fullname) {
    fullname = ds_name;
    fullname += "_";
    fullname += fname;
    fullname += MATRIX_EXTENSION;
  }

  //////////////////////////////////////////////////////////////////////////////
  // object methods

  object::object(uint id_)
    : rect<int>(), id(id_), visible(NULL), centroid(NULL), name(""),
      difficult(false), truncated(false), occluded(false), pose("") {
  }

  object::~object() {
    if (visible)
      delete visible;
    if (centroid)
      delete centroid;
  }

  void object::set_rect(int xmin, int ymin, int xmax, int ymax) {
    h0 = ymin;
    w0 = xmin;
    height = ymax - ymin;
    width = xmax - xmin;
  }

  void object::set_visible(int xmin, int ymin, int xmax, int ymax) {
    visible = new rect<int>(ymin, xmin, ymax - ymin, xmax - xmin);
  }

  void object::set_centroid(int x, int y) {
    centroid = new pair<int,int>(x, y);
  }

  //////////////////////////////////////////////////////////////////////////////
  // jitter

  jitter::jitter(float h_, float w_, float s_, float r_, int spatial_norm)
    : h(h_), w(w_), s(s_), r(r_), jitts(JITTERS) {
    jitts.set(s, 0);
    jitts.set(h / (float) spatial_norm, 1);
    jitts.set(w / (float) spatial_norm, 2);
    jitts.set(r, 3);
  }

  jitter::jitter(rect<float> &context, rect<float> &jit, int spatial_norm)
    : jitts(JITTERS) {
    h = jit.hcenter() - context.hcenter();
    w = jit.wcenter() - context.wcenter();
    r = 0;
    s = context.height / (float) jit.height;
    jitts.set(s, 0);
    jitts.set(h / (float) spatial_norm, 1);
    jitts.set(w / (float) spatial_norm, 2);
    jitts.set(r, 3);
  }

  jitter::jitter()
    : h(0), w(0), s(0), r(0), jitts(JITTERS) {
    idx_clear(jitts);
  }

  jitter::~jitter() {
  }

  const idx<t_jitter>& jitter::get_jitter_vector() const {
    return jitts;
  }

  void jitter::set(const idx<t_jitter> &j) {
    s = j.get(0);
    h = (int) j.get(1);
    w = (int) j.get(2);
    r = j.get(3);
    idx_copy(j, jitts);
  }

} // end namespace ebl
