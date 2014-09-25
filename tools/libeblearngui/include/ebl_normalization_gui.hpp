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

#define MAXWIDTH 1000

using namespace std;

namespace ebl {

////////////////////////////////////////////////////////////////////////////////

#define CONTRAST_NORM_MODULE_GUI(name, TYPE, op)                        \
  template <typename T>                                                 \
  void contrast_norm_module_gui::name(contrast_norm_module<T> &wsm,     \
                                      state<T> &in, state<T> &out,      \
                                      unsigned int &h0,			\
                                      unsigned int &w0,			\
                                      double zoom, T vmin, T vmax,	\
                                      bool show_out, bool run) {        \
    /* run it */							\
    if (run)								\
      wsm.op(in, out);							\
    uint h = h0, w = w0;						\
    /* display text */							\
    gui << gui_only() << at(h, w) << "inmean out:"                      \
        << wsm.subnorm.inmean.TYPE[0]                                   \
	<< at(h + 15, w) << "min:" << idx_min(wsm.subnorm.inmean.TYPE[0]) \
	<< at(h + 30, w) << "max:" << idx_max(wsm.subnorm.inmean.TYPE[0]); \
    w += 150;								\
    /* display image */							\
    {idx_bloop1(m, wsm.subnorm.inmean.TYPE[0], T) {                     \
	if (w - w0 < MAXWIDTH) {					\
	  draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	  w += (uint) (m.dim(1) * zoom + 1);				\
	}								\
      }									\
      h0 += (uint) (m.dim(0) * zoom + 1);				\
    }									\
    w = w0;								\
    h = h0;								\
    /* display text */							\
    gui << gui_only() << at(h, w) << "insq out:" << wsm.divnorm.insq.TYPE[0] \
	<< at(h + 15, w) << "min:" << idx_min(wsm.divnorm.insq.TYPE[0])	\
	<< at(h + 30, w) << "max:" << idx_max(wsm.divnorm.insq.TYPE[0]); \
    w += 150;								\
    /* display image */							\
    {idx_bloop1(m, wsm.divnorm.insq.TYPE[0], T) {                       \
	if (w - w0 < MAXWIDTH) {					\
	  draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	  w += (uint) (m.dim(1) * zoom + 1);				\
	}								\
      }									\
      h0 += (uint) (m.dim(0) * zoom + 1);				\
    }									\
    w = w0;								\
    h = h0;								\
    /* display text */							\
    gui << gui_only() << at(h, w) << "invar out:" << wsm.divnorm.invar.TYPE[0] \
	<< at(h + 15, w) << "min:" << idx_min(wsm.divnorm.invar.TYPE[0]) \
	<< at(h + 30, w) << "max:" << idx_max(wsm.divnorm.invar.TYPE[0]); \
    w += 150;								\
    /* display image */							\
    {idx_bloop1(m, wsm.divnorm.invar.TYPE[0], T) {                      \
	if (w - w0 < MAXWIDTH) {					\
	  draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	  w += (uint) (m.dim(1) * zoom + 1);				\
	}								\
      }									\
      h0 += (uint) (m.dim(0) * zoom + 1);				\
    }									\
    w = w0;								\
    h = h0;								\
    /* display text */							\
    gui << gui_only() << at(h, w) << "instd out:" << wsm.divnorm.instd.TYPE[0] \
	<< at(h + 15, w) << "min:" << idx_min(wsm.divnorm.instd.TYPE[0]) \
	<< at(h + 30, w) << "max:" << idx_max(wsm.divnorm.instd.TYPE[0]); \
    w += 150;								\
    /* display image */							\
    {idx_bloop1(m, wsm.divnorm.instd.TYPE[0], T) {                      \
	if (w - w0 < MAXWIDTH) {					\
	  draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	  w += (uint) (m.dim(1) * zoom + 1);				\
	}								\
      }									\
      h0 += (uint) (m.dim(0) * zoom + 1);				\
    }									\
    w = w0;								\
    h = h0;								\
    /* display text */							\
    gui << gui_only() << at(h, w) << "thstd out:" << wsm.divnorm.thstd.TYPE[0] \
	<< at(h + 15, w) << "min:" << idx_min(wsm.divnorm.thstd.TYPE[0]) \
	<< at(h + 30, w) << "max:" << idx_max(wsm.divnorm.thstd.TYPE[0]); \
    w += 150;								\
    /* display image */							\
    {idx_bloop1(m, wsm.divnorm.thstd.TYPE[0], T) {                      \
	if (w - w0 < MAXWIDTH) {					\
	  draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	  w += (uint) (m.dim(1) * zoom + 1);				\
	}								\
      }									\
      h0 += (uint) (m.dim(0) * zoom + 1);				\
    }									\
    w = w0;								\
    h = h0;								\
    /* display text */							\
    gui << gui_only() << at(h, w) << "invstd out:" << wsm.divnorm.invstd.TYPE[0] \
	<< at(h + 15, w) << "min:" << idx_min(wsm.divnorm.invstd.TYPE[0]) \
	<< at(h + 30, w) << "max:" << idx_max(wsm.divnorm.invstd.TYPE[0]); \
    w += 150;								\
    /* display image */							\
    {idx_bloop1(m, wsm.divnorm.invstd.TYPE[0], T) {                     \
	if (w - w0 < MAXWIDTH) {					\
	  draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	  w += (uint) (m.dim(1) * zoom + 1);				\
	}								\
      }									\
      h0 += (uint) (m.dim(0) * zoom + 1);				\
    }									\
    w = w0;								\
    h = h0;								\
  }

CONTRAST_NORM_MODULE_GUI(display_fprop, f, fprop)
CONTRAST_NORM_MODULE_GUI(display_bprop, b, bprop)
CONTRAST_NORM_MODULE_GUI(display_bbprop, bb, bbprop)

} // end namespace ebl
