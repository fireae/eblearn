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

#ifndef IDXGUI_H_
#define IDXGUI_H_

#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtGui/QtGui>

#include <ostream>
#include "libidx.h"
#include "scroll_box0.h"
#include "gui_thread.h"
#include "defines.h"

using namespace std;

namespace ebl {

class EXPORT gui_thread;
template <class T1, class T2> class ManipInfra;

class idxgui : public QThread, public ostringstream {
  Q_OBJECT
  private:
  int			  argc;
  char		**argv;
  int	                  nwid;
  const uint   *nwindows; // owned by gui_thread
  gui_thread           *gt;
  QMutex                mutex1;

 public:
  bool		  thread_init;
  bool		  cout_output;
  int                 (*run_main)(int, char**);
  bool		  main_done;

 public:
  idxgui();
  EXPORT void init(int argc_, char **argv_, const uint *nwindows,
                   gui_thread *gt_);
  virtual ~idxgui();

  //! creates a new window.
  EXPORT int new_window(const char *wname = NULL, uint h = 0, uint w = 0);

  //! creates a new window with 3D display.
  EXPORT int new_window3d(const char *wname = NULL, uint h = 0, uint w = 0);

  //! selects window wid.
  EXPORT void select_window(int wid);

  //! operator<< for text drawing on the gui.
  template<class T> friend
  idxgui& operator<<(idxgui& r, const T val);

  //! draws an arrow from (h1, w1) to (h2, w2).
  EXPORT void draw_arrow(int h1, int w1, int h2, int w2);
  //! draws a 'flow' at (h,w), where flow is 2xHxW flow field.
  EXPORT void draw_flow(idx<float> &flow, int h, int w);

  //! draws a bounding box with top left corner (h0, w0) and size (h, w).
  //! the (r,g,b) color of the box can optionally be specified as well as
  //! a caption string.
  EXPORT void draw_box(float h0, float w0, float h, float w, ubyte r = 255,
                       ubyte g = 255, ubyte b = 255, ubyte a = 255,
                       string *s = NULL);
  //! draws a cross at location (h0, w0) with length 'length'.
  //! the (r,g,b) color of the box can optionally be specified as well as
  //! a caption string.
  EXPORT void draw_cross(float h0, float w0, float length, ubyte r = 255,
                         ubyte g = 255, ubyte b = 255, ubyte a = 255,
                         string *s = NULL);
  //! draws an ellipse with center (h0,w0) and radii (h,w).
  //! the (r,g,b) color of the box can optionally be specified as well as
  //! a caption string.
  EXPORT void draw_ellipse(float h0, float w0, float h, float w,
                           ubyte r = 255, ubyte g = 255, ubyte b = 255,
                           ubyte a = 255, string *s = NULL);

  //! do not show windows, instead save them in png files in current dir.
  EXPORT void set_silent();
  //! do not show windows, instead save them in png files in current dir.
  EXPORT void set_silent(const std::string *filename);
  //! do not show windows, instead save them in png files in current dir.
  EXPORT void set_silent(const char *filename);

  //! do not update display until enable_updates is called again.
  //! this should make the display faster.
  EXPORT void disable_updates();

  //! allow display updates and display if it was previously off after a
  //! call to disable_updates().
  EXPORT void enable_updates();

  //! closes all windows.
  EXPORT void quit();

  //! Clears the window but does not resizes it to 1x1.
  EXPORT void clear();

  //! Clears the window and resizes it to 1x1.
  EXPORT void clear_resize();

  //! save current window into filename image.
  //! wid is optional, if given save window with id wid.
  EXPORT void save_window(const char *filename, int wid = -1);

  //! draw_matrix displays your idx2 or the first layer of your idx3 in
  //! grayscale on the whiteboard. This function does a copy of your idx and
  //! won't change in in any way !
  //! @param idx and @param type are, like before, used to templatize the
  //! function
  //! @param x and @param y are the coordinates of the top-left corner of
  //! your picture on the whiteboard
  //! @param minv and @param maxv are the min and max values to set colors.
  //! If left to zero, the min of your idx will be set to 0 and the max will
  //! be 255
  //! @param zoomw and @param zoomh are the zoom factors in width and height
  template<class T>
  void draw_matrix(idx<T> &im, uint h0, uint w0,
                   double zoomh, double zoomw, T minv = 0, T maxv = 0);

  //! Like draw_matrix template but without zooming and range options
  //! (faster).
  template<class T>
  void draw_matrix(idx<T> &im, uint h0 = 0, uint w0 = 0);

  //! ubyte specialised draw_matrix (faster, does not involve type
  //! conversion) without zooming and range options (also faster).
  //! Warning: use with caution. This is not thread safe (but faster),
  //! i.e. there might be a race condition on the deletion of the idx data.
  //! To avoid race condition, only manipulate/delete the idx when
  //! the gui's data mutex is unlocked (see (un)lock_data_mutex()).
  EXPORT void draw_matrix_unsafe(idx<ubyte> &im, uint h0 = 0, uint w0 = 0);

  //! same as draw_matrix but draws a frame of color (r,g,b) around it.
  template<class T>
  void draw_matrix_frame(idx<T> &im, ubyte r, ubyte g, ubyte b,
                         uint h0 = 0, uint w0 = 0,
                         double zoomh = 1.0, double zoomw = 1.0,
                         T minv = 0, T maxv = 0);

  //! same a draw_matrix but overlays the string <str> in the top left corner.
  template<class T>
  void draw_matrix(idx<T> &im, const char *str,
                   uint h0 = 0, uint w0 = 0,
                   double zoomh = 1.0, double zoomw = 1.0,
                   T minv = 0, T maxv = 0);

  //! Draws a mask from image.
  template<class T>
  void draw_mask(idx<T> &im, uint h0 = 0, uint w0 = 0,
                 double zoomh = 1.0, double zoomw = 1.0,
                 ubyte r = 255, ubyte g = 0, ubyte b = 0, ubyte a = 127,
                 T threshold = 0.0);

  //! draws text on the current window.
  //! you can also use the << operator instead of this function to add text
  //! to the gui. for example: gui << "text" << endl;
  EXPORT void draw_text(std::string *s);

  //! draws text on the current window at origin (h0, w0).
  //! you can also use the << operator instead of this function to add text
  //! to the gui. for example: gui << at(h0, w0) << "text" << endl;
  EXPORT void draw_text(std::string *s, uint h0, uint w0);

  //! sets the origin of further calls to draw_text or gui << "text".
  //! you can also use the at() function instead of this one.
  //! for example: gui << at(42, 0) << "text";
  EXPORT void set_text_origin(uint h0, uint w0);

  //! sets the text color for further calls to draw_text or gui << "text".
  //! you can also use the set_colors() function to set
  //! text and background colors and transparency.
  //! for example: gui << set_colors(255, 255, 255, 255, 0, 0, 0, 127);
  //! this sets the text color to fully opaque white on a semi-transparent
  //! black background.
  EXPORT void set_text_colors(unsigned char fg_r, unsigned char fg_g,
                              unsigned char fg_b, unsigned char fg_a,
                              unsigned char bg_r, unsigned char bg_g,
                              unsigned char bg_b, unsigned char bg_a);
  //! see unsigned char version.
  EXPORT void set_text_colors(int fg_r, int fg_g,
                              int fg_b, int fg_a,
                              int bg_r, int bg_g,
                              int bg_b, int bg_a);

  //! Set color of background.
  EXPORT void set_bg_colors(unsigned char r, unsigned char g,
                            unsigned char b);

  //! Set size of font for current window.
  EXPORT void set_font_size(int sz);
  //! Set size of font for all windows.
  EXPORT void set_global_font_size(int sz);

  //! set the << operator to output text on both std::cout and the current
  //! window.
  //! you can also use the cout_and_gui() function.
  //! for example: gui << cout_and_gui() << "text";
  EXPORT void set_cout_and_gui();

  //! set the << operator to output text only to the current window.
  //! you can also use the gui_only() function and not to std::cout.
  //! for example: gui << gui_only() << "text";
  EXPORT void set_gui_only();

  EXPORT void add_scroll_box(scroll_box0* sb);

  //! Set the title of the currently selected window.
  EXPORT void set_window_title(const char *s);

  //! Freeze or unfreeze style, no modification of colors are allowed when
  //! frozen.
  EXPORT void freeze_style(bool freeze);

  //! Force window size to hxw and forbid any resizing.
  EXPORT void freeze_window_size(uint h, uint w);

  //! Return the first key pressed in the queue of key events and remove it
  //! from the queue.
  EXPORT int pop_key_pressed();

  //! Return true if the gui is busy drawing, false otherwise.
  EXPORT bool busy_drawing();

  // 3d calls ////////////////////////////////////////////////////////////////

  //! Add a sphere at position (x,y,z) with 'radius' and color (r,g,b,a).
  EXPORT void draw_sphere(float x, float y, float z, float radius,
                          const char *s = NULL,
                          int r = 255, int g = 255, int b = 255, int a = 255);
  //! Add a cylinder which base is centered at (x,y,z), with length 'length',
  //! with radiuses 'top_radius' and 'base_radius', with degree angles
  //! 'a1', 'a2' and color (r,g,b,a).
  //! \param tops If true, draw closing caps on each end of cylinder.
  EXPORT void draw_cylinder(float x, float y, float z, float length,
                            float top_radius, float base_radius, float a1,
                            float a2, const char *s = NULL,
                            int r = 255, int g = 255,
                            int b = 255, int a = 255, bool tops = false);
  //! Draw 3d text 's' at (x,y,z) with color (r,g,b,a).
  EXPORT void draw_text_3d(float x, float y, float z, const char *s,
                           int r = 255, int g = 255, int b = 255, int a =255);
  //! Draw 3d line from (x,y,z) to (x1,y1,z1) with color (r,g,b,a)
  //! and with text 's' at (x1,y1,z1).
  EXPORT void draw_line_3d(float x, float y, float z,
                           float x1, float y1, float z1,
                           const char *s = NULL, int r = 255, int g = 255,
                           int b = 255, int a = 255);

 private:
  // check that user used MAIN_QTHREAD instead of regular main
  void check_init();

 signals:
  EXPORT void gui_drawImage(idx<ubyte> *img, uint h0, uint w0);
  EXPORT void gui_draw_mask(idx<ubyte> *img, uint h0, uint w0,
                            ubyte r, ubyte g, ubyte b, ubyte a);
  EXPORT void appquit();
  EXPORT void gui_clear();
  EXPORT void gui_clear_resize();
  EXPORT void gui_save_window(const string *filename, int wid);
  EXPORT void gui_new_window(const char *wname, uint h, uint w);
  EXPORT void gui_new_window3d(const char *wname, uint h, uint w);
  EXPORT void gui_select_window(int wid);
  EXPORT void gui_add_text(const string *s);
  EXPORT void gui_add_arrow(int h1, int w1, int h2, int w2);
  EXPORT void gui_add_flow(idx<float> *flow, int h, int w);
  EXPORT void gui_add_box(float h0, float w0, float h, float w, ubyte r,
                          ubyte g, ubyte b, ubyte a, string *s);
  EXPORT void gui_add_cross(float h0, float w0, float length, ubyte r,
			    ubyte g, ubyte b, ubyte a, string *s);
  EXPORT void gui_add_ellipse(float h0, float w0, float h, float w,
                              ubyte r, ubyte g, ubyte b, ubyte a, string *s);
  EXPORT void gui_set_text_origin(uint h0, uint w0);
  EXPORT void gui_set_text_colors(unsigned char fg_r, unsigned char fg_g,
                                  unsigned char fg_b, unsigned char fg_a,
                                  unsigned char bg_r, unsigned char bg_g,
                                  unsigned char bg_b, unsigned char bg_a);
  EXPORT void gui_set_bg_colors(unsigned char r, unsigned char g,
                                unsigned char b);
  EXPORT void gui_set_font_size(int);
  EXPORT void gui_set_global_font_size(int);
  EXPORT void gui_set_silent(const std::string *filename);
  EXPORT void gui_set_wupdate(bool update);
  EXPORT void gui_freeze_style(bool freeze);
  EXPORT void gui_freeze_window_size(uint h, uint w);
  EXPORT void gui_add_scroll_box(scroll_box0 *sb);
  EXPORT void gui_set_title(const string *s);
  // 3d signals //////////////////////////////////////////////////////////////
  EXPORT void gui_add_sphere(float,float,float,float, string *s,
                             int,int,int,int);
  EXPORT void gui_add_cylinder(float x, float y, float z, float length,
                               float top_radius, float base_radius,
                               float a1, float a2, string *s,
                               int r, int g, int b, int a, bool tops);
  EXPORT void gui_draw_text_3d(float x, float y, float z, string* s,
                               int r, int g, int b, int a);
  EXPORT void gui_draw_line_3d(float x, float y, float z,
                               float x1, float y1, float z1, string* s,
                               int r, int g, int b, int a);

 protected:
  virtual void run();
};


template<class T1, class T2>
class ManipInfra {
 public:
  ManipInfra(idxgui& (*pFun) (idxgui&))
      : manipFun0(pFun), val1(0), val2(0), nval(0) {}
  ManipInfra(idxgui& (*pFun) (idxgui&, T1), T1 val1_)
      : manipFun1(pFun), val1(val1_), val2(0), nval(1) {}
  ManipInfra(idxgui& (*pFun) (idxgui&, T1, T2),
             T1 val1_, T2 val2_)
      : manipFun2(pFun), val1(val1_), val2(val2_), nval(2) {}
  ManipInfra(idxgui& (*pFun) (idxgui&, T1, T2, T2, T2, T2, T2, T2, T2),
             T1 val1_, T2 val2_, T2 val3_, T2 val4_,
             T2 val5_, T2 val6_, T2 val7_, T2 val8_)
      : manipFun8(pFun), val1(val1_), val2(val2_), val3(val3_), val4(val4_),
	val5(val5_), val6(val6_), val7(val7_), val8(val8_), nval(8) {}

  void operator() (idxgui& r) const {
    switch (nval) {
      case 0: manipFun0(r); break ;
      case 1: manipFun1(r, val1); break ;
      case 2: manipFun2(r, val1, val2); break ;
      case 8: manipFun8(r, val1, val2, val3, val4, val5, val6, val7, val8);
	break ;
      default: eblerror("unknown mode");
    }
  }
 private:
  idxgui& (*manipFun0) (idxgui&);
  idxgui& (*manipFun1) (idxgui&, T1);
  idxgui& (*manipFun2) (idxgui&, T1, T2);
  idxgui& (*manipFun8) (idxgui&, T1, T2, T2, T2, T2, T2, T2, T2);
  T1 val1;
  T2 val2, val3, val4, val5, val6, val7, val8;
  int nval;
};

//template class ManipInfra<int,int>;
//template class ManipInfra<uint,uint>;
EXPORT idxgui& operator<<(idxgui& r, const ManipInfra<uint,uint> &manip);
EXPORT idxgui& operator<<(idxgui& r, const ManipInfra<int,int> &manip);

//! specifies the origin of the text to draw.
//! calling 'gui << at(42, 0) << "text";' will draw "text" at height 42
//! and with 0.
EXPORT ManipInfra<uint,uint> at(uint h0, uint w0);
EXPORT idxgui& att(idxgui& r, uint h0, uint w0);

//! specifies to output text to both the gui and std::cout.
//! usage: gui << cout_and_gui();
EXPORT ManipInfra<int,int> cout_and_gui();
EXPORT idxgui& fcout_and_gui(idxgui& r);

//! specifies to output text to both the gui and std::cout.
//! usage: gui << gui_only();
EXPORT ManipInfra<int, int> gui_only();
EXPORT idxgui& fgui_only(idxgui& r);

//! set the text color to black on white background with optional
//! transparency factors fg_a and bg_a respectively for foreground and
//! background colors, ranging from 0 to 255.
//! usage: gui << black_on_white();
//! usage: gui << black_on_white(127);
//! usage: gui << black_on_white(50, 255);
EXPORT ManipInfra<unsigned char, unsigned char>
black_on_white(unsigned char fg_a = 255, unsigned char bg_a = 255);
EXPORT idxgui& fblack_on_white(idxgui& r, unsigned char fg_a,
                               unsigned char bg_a);

//! set the text color to white on a transparent background
//! (transparency = 127 with black background).
EXPORT idxgui& fwhite_on_transparent(idxgui& r);
EXPORT ManipInfra<int, int> white_on_transparent();

//! set the text color to rgba on a rgba background where each value ranges
//! from 0 to 255.
//! usage: gui << set_colors(255, 255, 255, 255, 0, 0, 0, 127);
//! this sets the text color to fully opaque white on a semi-transparent black
//! background.
EXPORT ManipInfra<unsigned char, unsigned char>
set_colors(unsigned char fg_r, unsigned char fg_g,
           unsigned char fg_b, unsigned char fg_a,
           unsigned char bg_r, unsigned char bg_g,
           unsigned char bg_b, unsigned char bg_a);
EXPORT idxgui& fset_colors(idxgui& r,
                           unsigned char fg_r, unsigned char fg_g,
                           unsigned char fg_b, unsigned char fg_a,
                           unsigned char bg_r, unsigned char bg_g,
                           unsigned char bg_b, unsigned char bg_a);

} // namespace ebl {

#include "idxgui.hpp"

#endif /* IDXGUI_H_ */
