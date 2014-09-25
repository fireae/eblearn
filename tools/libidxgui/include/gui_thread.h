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

#ifndef GUI_THREAD_H_
#define GUI_THREAD_H_

#include <QPixmap>
#include <QWidget>
#include <QtGui>
#include <QResizeEvent>
#include <math.h>
#include <vector>

#include "libidx.h"
#include "win.h"
#include "win2d.h"
#include "win3d.h"
#include "idxgui.h"
#include "scroll_box0.h"
#include "defines.h"

namespace ebl {

class idxgui;
//! Global pointer to gui, allows to call for example
//! gui.draw_matrix() from anywhere in the code.
extern IDXGUIEXPORT ebl::idxgui gui;

//! window is a simple "whiteboard" on which you can display
//! idxs with for example draw_matrix.
//! Warning: do not use electric fence with QT as it is unstable.

class IDXGUIEXPORT gui_thread : public QWidget {
  Q_OBJECT
      public:
  gui_thread(int argc, char **argv, QApplication &qa);
  virtual ~gui_thread();

  //! Return first key pressed of the key pressed even list for
  //! current window and pop it out of the list, or return -1 if no key.
  int pop_key_pressed();

  //! Returns true if busy drawing, false otherwise.
  bool busy_drawing();

  //! Set the internal flag 'bquit' to true in order to skip the queue
  //! of graphic messages.
  void quit();

  ////////////////////////////////////////////////////////////////
private slots:
  void window_destroyed(QObject *obj);

  //! used to disable or enable updating of window, for batch displaying.
  //! this is useful to avoid flickering and speed up display.
  void set_wupdate(bool update);

  void add_text(const string *s);
  void add_arrow(int x1, int y1, int x2, int y2);
  void add_flow(idx<float> *flow, int h, int w);
  void add_box(float h0, float w0, float h, float w, ubyte r, ubyte g,
               ubyte b, ubyte a, string *s);
  void add_ellipse(float h0, float w0, float h, float w,
                   ubyte r, ubyte g, ubyte b, ubyte a, string *s);
  void add_cross(float h0, float w0, float length, ubyte r, ubyte g,
                 ubyte b, ubyte a, string *s);
  void set_text_origin(unsigned int h0, unsigned int w0);
  void set_text_colors(unsigned char fg_r, unsigned char fg_g,
                       unsigned char fg_b, unsigned char fg_a,
                       unsigned char bg_r, unsigned char bg_g,
                       unsigned char bg_b, unsigned char bg_a);
  void set_bg_colors(unsigned char r, unsigned char g,
                     unsigned char b);
  //! Set size of font.
  void set_font_size(int sz);
  void set_global_font_size(int sz);
  void updatePixmap(idx<ubyte> *img, uint h0, uint w0);
  void add_mask(idx<ubyte> *img, uint h0, uint w0,
                ubyte r, ubyte g, ubyte b, ubyte a);
  void appquit();

  //! Clears the current window, but does not resize it.
  void clear();

  //! Clears the current windows and resizes it to 1x1.
  void clear_resize();

  //! save window with id wid into filename image.
  //! if wid == -1, save current window.
  void save_window(const string *filename, int wid);
  //! Create a new window with name 'wname' and size 'h'x'w'.
  void new_window(const char *wname = NULL, uint h = 0, uint w = 0);
  //! Create a new 3D window with name 'wname' and size 'h'x'w'.
  void new_window3d(const char *wname = NULL, uint h = 0, uint w = 0);
  void select_window(int wid);
  void set_silent(const std::string *filename = NULL);
  void add_scroll_box(scroll_box0 *sb);
  //! Modify current window's title.
  void set_title(const string *s);

  //! Freeze or unfreeze style, no modification of colors are allowed when
  //! frozen.
  void freeze_style(bool freeze);

  //! Force window size to hxw and forbid any resizing.
  void freeze_window_size(uint h, uint w);

  /// 3d calls ///////////////////////////////////////////////////////////////

  //! Add a sphere at position (x,y,z) with 'radius' and color (r,g,b,a).
  void add_sphere(float x, float y, float z, float radius, string *s,
                  int r = 255, int g = 255, int b = 255, int a = 255);
  //! Add a cylinder which base is centered at (x,y,z), with length 'length',
  //! with radiuses 'top_radius' and 'base_radius', with degree angles
  //! 'a1', 'a2' and color (r,g,b,a).
  //! \param tops If true, draw closing caps on each end of cylinder.
  void add_cylinder(float x, float y, float z, float length, float top_radius,
                    float base_radius, float a1, float a2, string *s,
                    int r = 255, int g = 255, int b = 255, int a = 255,
                    bool tops = false);
  //! Draw 3d text 's' at (x,y,z) with color (r,g,b,a).
  void draw_text_3d(float x, float y, float z, string *s,
                    int r, int g, int b, int a);
  //! Draw 3d line from (x,y,z) to (x1,y1,z1) with color (r,g,b,a)
  //! and with text 's' at (x1,y1,z1).
  void draw_line_3d(float x, float y, float z, float x1, float y1, float z1,
                    string *s, int r, int g, int b, int a);

  // class members /////////////////////////////////////////////////////////////
public:
  idxgui	    &thread;
private:
  int		     wcur;
  unsigned int	     nwindows;
  std::vector<win*>  windows;
  bool		     silent;
  std::string	     savefname;
  bool               busy;              // flag when busy drawing
  QMutex             mutex1;
  bool               bquit;             // indicate if we want to quit
  QApplication      &qapp;              // reference to main QT thread
  int                global_font_size;         // global font size
};

//! This macro is intended to replace your int main(int argc, char **argv)
//! declaration and hides the declaration of the application and thread.
//! What happens is QT takes over the main thread and runs your code
//! in a thread.
#define MAIN_QTHREAD(targc, argc, targv, argv)	\
  int run_main(targc argc, targv argv);		\
  using namespace ebl;				\
  int main(targc argc, targv argv) {		\
    gui.thread_init = true;			\
    gui.run_main = &run_main;			\
    gui.thread_init = true;			\
    QApplication a(argc, argv);			\
    a.setQuitOnLastWindowClosed(false);		\
    ebl::gui_thread gt(argc, argv, a);		\
    gt.thread.start();				\
    a.exec();					\
    return 0;					\
  }						\
  int run_main(targc argc, targv argv)


#define WINMAIN_QTHREAD(targc, argc, targv, argv)			\
  int run_main(targc argc, targv argv);					\
  using namespace ebl;							\
  int WINAPI WinMain(HINSTANCE d1, HINSTANCE d2, LPSTR d3, int d4) {	\
    LPWSTR *argvw;							\
    LPTSTR *argv;							\
    targc argc = 0;							\
    argvw = CommandLineToArgvW(GetCommandLineW(), &argc);		\
    USES_CONVERSION;							\
    argv = new LPTSTR[argc];						\
    for (targc i = 0; i < argc; ++i) {					\
      argv[i] = W2A(argvw[i]);						\
    }									\
    gui.thread_init = true;						\
    gui.run_main = &run_main;						\
    gui.thread_init = true;						\
    QApplication a(argc, argv);						\
    a.setQuitOnLastWindowClosed(false);					\
    ebl::gui_thread gt(argc, argv), a;					\
    gt.thread.start();							\
    a.exec();								\
    LocalFree(argv);							\
    for (targc i = 0; i < argc; ++i) {					\
      if (argv[i]) delete argv[i];					\
    }									\
    return 0;								\
  }									\
  int run_main(targc argc, targv argv)

#ifdef __WINDOWS__
#define NOCONSOLE_MAIN_QTHREAD WINMAIN_QTHREAD
#else
#define NOCONSOLE_MAIN_QTHREAD MAIN_QTHREAD
#endif

} // namespace ebl {

#endif /* GUI_THREAD_H_ */
