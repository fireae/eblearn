/***************************************************************************
 *   Copyright (C) 2009 Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef EBL_ARCH_GUI_H_
#define EBL_ARCH_GUI_H_

#include "libidxgui.h"
#include "ebl_arch.h"
#include "ebl_march.h"
#include "ebl_answer.h"

#define MAXWIDTH 1000
#define MAXHEIGHT 1000
#define TEXT_MIN_HEIGHT 50
#define TEXT_MARGIN 170
#define MODULES_HSPACE 2

namespace ebl {

// module_1_1_gui ////////////////////////////////////////////////////////////

class EXPORT module_1_1_gui {
public:
  module_1_1_gui(int fwid = -1)
      : display_wid_fprop(fwid), display_wid_bprop(-1), display_wid_bbprop(-1) {
  };
  virtual ~module_1_1_gui() {};

#define DISPLAY_PROTO_1_1(name)                                         \
  template <typename T>                                                 \
      void name(module_1_1<T> &m, state<T> &in, state<T> &out, uint h0 = 0, \
		uint w0 = 0, double dzoom = 1.0, T vmin = 0, T vmax = 0, \
		bool show_out = true, int wid = -1, const char *wname = NULL);

#define DISPLAY2_PROTO_1_1(name)                                        \
  template <typename T>                                                 \
      void name(module_1_1<T> &m, state<T> &in, state<T> &out, uint &h0, \
		uint &w0, double dzoom = 1.0, T vmin = 0, T vmax = 0,	\
		bool show_out = true, int wid = -1, const char *wname = NULL);

  DISPLAY_PROTO_1_1(display_fprop)
  DISPLAY_PROTO_1_1(display_bprop)
  DISPLAY_PROTO_1_1(display_bbprop)
  DISPLAY2_PROTO_1_1(display_fprop2)
  DISPLAY2_PROTO_1_1(display_bprop2)
  DISPLAY2_PROTO_1_1(display_bbprop2)

  //! Display internal buffers of module 'm', as declared in
  //! module_1_1's member 'internals'.
  //! \param maxwidth Max width after which to create a new line
  template <typename T>
    void display_internals(module_1_1<T> &m, uint &h0, uint &w0,
                           double zoom = 1.0, T vmin = 0, T vmax = 0,
                           int wid = -1, const char *wname = NULL,
			   uint maxwidth = MAXWIDTH);

public:
  int	 display_wid_fprop;
  int	 display_wid_bprop;
  int	 display_wid_bbprop;
  static uint cnt;
};

// module_2_1_gui //////////////////////////////////////////////////////////////

class module_2_1_gui {
 public:
  module_2_1_gui() {};
  virtual ~module_2_1_gui() {};

#define DISPLAY_PROTO_2_1(name)                                         \
  template <typename T>                                                 \
  void name(module_2_1<T> &m,                                           \
            state<T> &in1, state<T> &in2, state<T> &out,                \
            uint &h0, uint &w0, double dzoom = 1.0, T vmin = 0, T vmax = 0, \
            bool show_out = true, int wid = -1, const char *wname = NULL);

  DISPLAY_PROTO_2_1(display_fprop)
  DISPLAY_PROTO_2_1(display_bprop)
  DISPLAY_PROTO_2_1(display_bbprop)

 private:
  uint	 display_wid_fprop;
};

// layers_gui //////////////////////////////////////////////////////////////////

class layers_gui {
 public:
  layers_gui(int wid = -1) : m11g(wid) {};
  virtual ~layers_gui() {};

#define DISPLAY_PROTO_LAYERSN(name)                                     \
  template <typename T>                                                 \
  static void name(module_1_1_gui &g, layers<T> &ln, state<T> &in,      \
                   state<T> &out, uint &h0, uint &w0, double dzoom = 1.0, \
                   T vmin = 0, T vmax = 0, bool show_out = false);

  DISPLAY_PROTO_LAYERSN(display_fprop)
  DISPLAY_PROTO_LAYERSN(display_bprop)
  DISPLAY_PROTO_LAYERSN(display_bbprop)

  //! Display internal buffers of modules contained in layer 'ln'.
  template <typename T>
  void display_internals(layers<T> &ln, uint &h0, uint &w0, double dzoom = 1.0,
                         T vmin = 0, T vmax = 0, uint maxwidth = MAXWIDTH);

  //! Tries to cast 'm' as a layers object and display internal buffers
  //! of all sub modules.
  template <typename T>
  void display_internals(module_1_1<T> &m, uint &h0, uint &w0,
                         double dzoom = 1.0,
                         T vmin = 0, T vmax = 0, uint maxwidth = MAXWIDTH);

 protected:
  module_1_1_gui	m11g;
};

// ms_module_gui /////////////////////////////////////////////////////////////

class ms_module_gui {
 public:
  ms_module_gui(int wid = -1) : m11g(wid) {};
  virtual ~ms_module_gui() {};

#define DISPLAY_PROTO_MSMODULE(name, T)                                 \
  template <typename T>                                                  \
  static void name(module_1_1_gui &g, ms_module<T> &ln, state<T> &in,   \
                   state<T> &out, uint &h0, uint &w0, double dzoom = 1.0, \
                   T vmin = 0, T vmax = 0, bool show_out = false);

  // in: state<T> out: state<T>
  DISPLAY_PROTO_MSMODULE(display_fprop, T)
  DISPLAY_PROTO_MSMODULE(display_bprop, T)
  DISPLAY_PROTO_MSMODULE(display_bbprop, T)

  protected:
  module_1_1_gui	m11g;
};

// layers_2_gui ////////////////////////////////////////////////////////////////

/*   class layers_2_gui { */
/*   public: */
/*     layers_2_gui() {}; */
/*     virtual ~layers_2_gui() {}; */

/* #define DISPLAY_PROTO_LAYERS2(name)				\ */
/*     template <typename T, class state<T>>							\ */
/*       static void name(module_1_1_gui &g,				\ */
/* 		       layers_2<T> &ln,					\ */
/* 		       state<T> &in, state<T> &out,			\ */
/* 		       uint &h0, uint &w0,		\ */
/* 		       double dzoom = 1.0,				\ */
/* 		       T vmin = 0, T vmax = 0,				\ */
/* 		       bool show_out = false); */

/*     DISPLAY_PROTO_LAYERS2(display_fprop) */
/*     DISPLAY_PROTO_LAYERS2(display_bprop) */
/*     DISPLAY_PROTO_LAYERS2(display_bbprop) */
/*   }; */

// trainable_module_gui ////////////////////////////////////////////////////////

class trainable_module_gui {
 public:
  trainable_module_gui() {};
  virtual ~trainable_module_gui() {};

#define DISPLAY_PROTO_TRAINABLE(name)                                   \
  template <typename T, typename Tds1, typename Tds2>                   \
  static void name(trainable_module<T,Tds1,Tds2> &dse,                  \
                   state<T> &i1, state<T> &i2, state<T> &energy,        \
                   uint &h0, uint &w0,                                  \
                   double zoom, T vmin = 0, T vmax = 0,                 \
                   bool show_out = true,				\
                   int wid = -1, const char *wname = NULL);

  DISPLAY_PROTO_TRAINABLE(display_fprop)
  DISPLAY_PROTO_TRAINABLE(display_bprop)
  DISPLAY_PROTO_TRAINABLE(display_bbprop)
};

} // namespace ebl {

#include "ebl_arch_gui.hpp"

#endif /* EBL_ARCH_GUI_H_ */
