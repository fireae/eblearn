/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef ELB_LAYERS_GUI_HPP_
#define ELB_LAYERS_GUI_HPP_

using namespace std;

#define MAXWIDTH 1000

namespace ebl {

// full_layer //////////////////////////////////////////////////////////////////

#define FULL_LAYER_GUI(dname, TYPE, op)                                 \
  template <typename T>                                                 \
  void full_layer_gui::dname(full_layer<T> &nn,                         \
                             state<T> &in, state<T> &out,               \
                             uint &h0, uint &w0,                        \
                             double zoom, T vmin, T vmax,		\
                             bool show_out) {				\
    /* run it */							\
    nn.op(in, out);							\
    uint h = h0, w = w0;                                                \
    /* display text */							\
    gui << gui_only() << at(h, w) << nn.name() << " in:" << in.T;	\
    w += 150;								\
    zoom *= 3;								\
    /* display inputs */						\
    idx_bloop1(m, in.TYPE[0], T) {                                      \
      if (w - w0 < MAXWIDTH) {						\
	draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	w += (uint) (m.dim(1) * zoom + 1);				\
      }									\
    }									\
    h0 += (uint) (std::max((uint) 10, (uint) (m.dim(0) * zoom + 1)));	\
  }

FULL_LAYER_GUI(display_fprop, f, fprop)
FULL_LAYER_GUI(display_bprop, b, bprop)
FULL_LAYER_GUI(display_bbprop, bb, bbprop)

// convolution_layer ///////////////////////////////////////////////////////////

#define CONVOLUTION_LAYER_GUI(dname, TYPE, op)                          \
  template <typename T>                                                 \
  void convolution_layer_gui::dname(convolution_layer<T> &nn,           \
                                    state<T> &in, state<T> &out,        \
                                    uint &h0, uint &w0,			\
                                    double zoom, T vmin, T vmax,        \
                                    bool show_out, bool run) {		\
    /* run it */							\
    if (run)								\
      nn.op(in, out);							\
    uint h = h0, w = w0;                                                \
    /* display text */							\
    gui << gui_only() << at(h, w) << nn.name() << " in:" << in.TYPE[0]	\
	<< at(h + 15, w) << "min:" << idx_min(in.TYPE[0])               \
	<< at(h + 30, w) << "max:" << idx_max(in.TYPE[0]);              \
    w += 150;								\
    /* display inputs */						\
    idx_bloop1(m, in.TYPE[0], T) {                                      \
      if (w - w0 < MAXWIDTH) {						\
	draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	w += (uint) (m.dim(1) * zoom + 1);				\
      }									\
    }									\
    h0 += (uint) (m.dim(0) * zoom + 1);					\
    w = w0;								\
    h = h0;								\
    /* display kernels text */						\
    gui << gui_only()<< at(h, w) << "kernels:" << nn.convol.kernel.TYPE[0]; \
    w += 150;								\
    /* display kernels */						\
    /* zoom *= 4; */	 						\
    idx_bloop1(mk, nn.convol.kernel.TYPE[0], T) {                       \
      if (w - w0 < MAXWIDTH) {						\
	draw_matrix(mk, h, w, zoom, zoom, vmin, vmax);			\
	w += (uint) (mk.dim(1) * zoom + 1);				\
      }									\
    }									\
    h0 += (uint) (std::max((uint) 10, (uint) (mk.dim(0) * zoom + 1)));	\
  }

CONVOLUTION_LAYER_GUI(display_fprop, f, fprop)
CONVOLUTION_LAYER_GUI(display_bprop, b, bprop)
CONVOLUTION_LAYER_GUI(display_bbprop, bb, bbprop)

// convabsnorm_layer ///////////////////////////////////////////////////////////

#define CONVABSNORM_LAYER_GUI(dname, TYPE, op)                          \
  template <typename T>                                                 \
  void convabsnorm_layer_gui::dname(convabsnorm_layer<T> &layer,	\
                                    state<T> &in, state<T> &out,        \
                                    uint &h0, uint &w0,                 \
                                    double zoom, T vmin, T vmax,        \
                                    bool show_out) {			\
    convolution_layer_gui::dname(layer.lconv, in, out, h0, w0,		\
				 zoom, vmin, vmax, show_out, false);	\
    /* run it */							\
    layer.op(in, out);							\
    uint h = h0, w = w0;                                                \
    /* display text */							\
    gui << gui_only() << at(h, w) << layer.name()                       \
        << " out:" << layer.tmp->TYPE[0]                                \
	<< at(h + 15, w) << "min:" << idx_min(layer.tmp->TYPE[0])       \
	<< at(h + 30, w) << "max:" << idx_max(layer.tmp->TYPE[0]);      \
    w += 150;								\
    /* display output of convolution */					\
    idx_bloop1(m, layer.tmp->TYPE[0], T) {                              \
      if (w - w0 < MAXWIDTH) {						\
	draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	w += (uint) (m.dim(1) * zoom + 1);				\
      }									\
    }									\
    h0 += (uint) (m.dim(0) * zoom + 1);					\
    w = w0;								\
    h = h0;								\
    /* display text */							\
    gui << gui_only() << at(h, w) << "abs out:" << layer.tmp2->TYPE[0]	\
	<< at(h + 15, w) << "min:" << idx_min(layer.tmp2->TYPE[0])      \
	<< at(h + 30, w) << "max:" << idx_max(layer.tmp2->TYPE[0]);     \
    w += 150;								\
    /* display output of abs */						\
    idx_bloop1(m2, layer.tmp2->TYPE[0], T) {                            \
      if (w - w0 < MAXWIDTH) {						\
	draw_matrix(m2, h, w, zoom, zoom, vmin, vmax);			\
	w += (uint) (m2.dim(1) * zoom + 1);				\
      }									\
    }									\
    h0 += (uint) (m2.dim(0) * zoom + 1);				\
    w = w0;								\
    h = h0;								\
    /* display normalization internals */				\
    contrast_norm_module_gui::dname(layer.norm, in, out, h0, w0,	\
				    zoom, vmin, vmax, show_out, false);	\
  }

CONVABSNORM_LAYER_GUI(display_fprop, f, fprop)
CONVABSNORM_LAYER_GUI(display_bprop, b, bprop)
CONVABSNORM_LAYER_GUI(display_bbprop, bb, bbprop)

// subsampling_layer ///////////////////////////////////////////////////////////

#define SUBSAMPLING_LAYER_GUI(dname, TYPE, op)                          \
  template <typename T>                                                 \
  void subsampling_layer_gui::dname(subsampling_layer<T> &nn,           \
                                    state<T> &in, state<T> &out,        \
                                    uint &h0, uint &w0,			\
                                    double zoom, T vmin, T vmax,        \
                                    bool show_out) {			\
    /* run it */							\
    nn.op(in, out);							\
    uint h = h0, w = w0;                                                \
    /* display input text	*/					\
    gui << gui_only() << at(h, w) << nn.name() << " in:" << in.TYPE[0];	\
    w += 150;								\
    /* display inputs */						\
    idx_bloop1(m, in.TYPE[0], T) {                                      \
      if (w - w0 < MAXWIDTH) {						\
	draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	w += (uint) (m.dim(1) * zoom + 1);				\
      }									\
    }									\
    h0 += (uint) (m.dim(0) * zoom + 1);					\
    w = w0;								\
    h = h0;								\
    /* display kernels text */						\
    gui << gui_only()<< at(h, w) << "kernels:"                          \
        << nn.subsampler.sub.TYPE[0].dim(0);                            \
    gui << "x" << in.TYPE[0].dim(1) / nn.subsampler.sub.TYPE[0].dim(1); \
    gui << "x" << in.TYPE[0].dim(2) / nn.subsampler.sub.TYPE[0].dim(2); \
    w += 150;								\
    h0 += 10;								\
  }

SUBSAMPLING_LAYER_GUI(display_fprop, f, fprop)
SUBSAMPLING_LAYER_GUI(display_bprop, b, bprop)
SUBSAMPLING_LAYER_GUI(display_bbprop, bb, bbprop)

}

#endif /* ELB_LAYERS_GUI_HPP_ */
