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

#ifndef EBL_NORMALIZATION_GUI_H_
#define EBL_NORMALIZATION_GUI_H_

#include "libidxgui.h"
#include "ebl_normalization.h"

namespace ebl {


class contrast_norm_module_gui {
 public:
  contrast_norm_module_gui();
  virtual ~contrast_norm_module_gui();

  template <typename T>
  static void display_fprop(contrast_norm_module<T> &m, state<T> &in,
                            state<T> &out, uint &h0, uint &w0, double zoom,
                            T vmin = 0, T vmax = 0,
                            bool show_out = false, bool run = true);

  template <typename T>
  static void display_bprop(contrast_norm_module<T> &m,
                            state<T> &in, state<T> &out,
                            uint &h0, uint &w0, double zoom,
                            T vmin = 0, T vmax = 0,
                            bool show_out = false, bool run = true);

  template <typename T>
  static void display_bbprop(contrast_norm_module<T> &m,
                             state<T> &in, state<T> &out,
                             uint &h0, uint &w0, double zoom,
                             T vmin = 0, T vmax = 0,
                             bool show_out = false, bool run = true);
};

} // namespace ebl {

#include "ebl_normalization_gui.hpp"

#endif /* EBL_NORMALIZATION_GUI_H_ */
