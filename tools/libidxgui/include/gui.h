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

#ifndef GUI_H_
#define GUI_H_

#include <ostream>
#include "libidx.h"
#include "gui_thread.h"

using namespace std;

namespace ebl {

//! creates a new window.
EXPORT int new_window(const char *wname = NULL, uint h = 0, uint w = 0);
//! creates a new window.
EXPORT int new_window(const string &wname, uint h = 0, uint w = 0);
//! creates a new window with 3D rendering.
EXPORT int new_window3d(const char *wname = NULL, uint h = 0, uint w = 0);
//! creates a new window with 3D rendering.
EXPORT int new_window3d(const string &wname, uint h = 0, uint w = 0);
//! selects window wid.
EXPORT void select_window(int wid);
//! do not update display until enable_updates is called again.
//! this should make the display faster.
EXPORT void disable_window_updates();
//! allow display updates and display if it was previously off after a
//! call to disable_updates().
EXPORT void enable_window_updates();

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
void draw_matrix(idx<T> &im, unsigned int h0, unsigned int w0,
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
                       unsigned int h0 = 0, unsigned int w0 = 0,
                       double zoomh = 1.0, double zoomw = 1.0,
                       T minv = 0, T maxv = 0);
//! same a draw_matrix but overlays the string <str> in the top left corner.
template<class T>
void draw_matrix(idx<T> &im, const char *str,
                 unsigned int h0 = 0, unsigned int w0 = 0,
                 double zoomh = 1.0, double zoomw = 1.0,
                 T minv = 0, T maxv = 0);
//! Draws a mask from image.
template<class T>
void draw_mask(idx<T> &im, uint h0 = 0, uint w0 = 0,
               double zoomh = 1.0, double zoomw = 1.0,
               ubyte r = 255, ubyte g = 0, ubyte b = 0, ubyte a = 127,
               T threshold = 0.0);

//! closes all windows.
EXPORT void quit_gui();
//! Clears the window, but does not resize it to 1x1.
EXPORT void clear_window();
//! Clears the window and resizes it to 1x1.
EXPORT void clear_resize_window();
//! Save current window as a PNG into filename image.
//! filename should contain an extension, '.png' will be added.
//! wid is optional, if given save window with id wid.
EXPORT void save_window(const char *filename, int wid = -1);

//! draws an arrow from (h1, w1) to (h2, w2).
EXPORT void draw_arrow(int h1, int w1, int h2, int w2);
//! draws a 'flow' at (h,w), where flow is 2xHxW flow field.
EXPORT void draw_flow(idx<float> &flow, int h, int w);
//! draws a bounding box with top left corner (h0, w0) and size (h, w).
//! the (r,g,b,a) color of the box can optionally be specified as well as
//! a caption string.
EXPORT void draw_box(rect<int> &box, ubyte r = 255, ubyte g = 255,
                     ubyte b = 255, ubyte a = 255, string *s = NULL);
//! draws a bounding box with top left corner (h0, w0) and size (h, w).
//! the (r,g,b,a) color of the box can optionally be specified as well as
//! a caption string.
EXPORT void draw_box(rect<float> &box, ubyte r = 255, ubyte g = 255,
                     ubyte b = 255, ubyte a = 255, string *s = NULL);
//! draws a bounding box with top left corner (h0, w0) and size (h, w).
//! the (r,g,b,a) color of the box can optionally be specified as well as
//! a caption string.
EXPORT void draw_box(int h0, int w0, int h, int w, ubyte r = 255,
                     ubyte g = 255, ubyte b = 255, ubyte a = 255,
                     string *s = NULL);
//! draws a bounding box with top left corner (h0, w0) and size (h, w).
//! the (r,g,b,a) color of the box can optionally be specified as well as
//! a caption string.
EXPORT void draw_box(float h0, float w0, float h, float w, ubyte r = 255,
                     ubyte g = 255, ubyte b = 255, ubyte a = 255,
                     string *s = NULL);
//! draws a cross at location (h0, w0) and length 'length'.
//! the (r,g,b,a) color of the box can optionally be specified as well as
//! a caption string.
EXPORT void draw_cross(float h0, float w0, float length, ubyte r = 255,
                       ubyte g = 255, ubyte b = 255, ubyte a = 255,
                       string *s = NULL);
//! draws an ellipse with center (h0,w0) and radii (hxw).
//! the (r,g,b,a) color of the box can optionally be specified as well as
//! a caption string.
EXPORT void draw_ellipse(float h0, float w0, float h, float w, ubyte r = 255,
                         ubyte g = 255, ubyte b = 255, ubyte a = 255,
                         string *s = NULL);

//! do not show windows, instead save them in png files in current dir.
EXPORT void set_gui_silent();
//! do not show windows, instead save them in png files in current dir.
EXPORT void set_gui_silent(const std::string *filename);
//! do not show windows, instead save them in png files in current dir.
EXPORT void set_gui_silent(const char *filename);

//! draws text on the current window.
//! you can also use the << operator instead of this function to add text
//! to the gui. for example: gui << "text" << endl;
EXPORT void draw_text(string *s);
//! draws text on the current window at origin (h0, w0).
//! you can also use the << operator instead of this function to add text
//! to the gui. for example: gui << at(h0, w0) << "text" << endl;
EXPORT void draw_text(string *s, unsigned int h0, unsigned int w0);
//! sets the origin of further calls to draw_text or gui << "text".
//! you can also use the at() function instead of this one.
//! for example: gui << at(42, 0) << "text";
EXPORT void set_text_origin(unsigned int h0, unsigned int w0);
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
EXPORT void set_text_colors(int fg_r, int fg_g, int fg_b, int fg_a,
                            int bg_r, int bg_g, int bg_b, int bg_a);
//! Set the color of the background.
EXPORT void set_bg_colors(int r, int g, int b);
//! Set size of font for current window.
EXPORT void set_font_size(int sz);
//! Set size of font for all windows.
EXPORT void set_global_font_size(int sz);

//! set the << operator to output text on both std::cout and the current
//! window.
//! you can also use the cout_and_gui() function.
//! for example: gui << cout_and_gui() << "text";
EXPORT void set_window_cout_and_gui();
//! set the << operator to output text only to the current window.
//! you can also use the gui_only() function and not to std::cout.
//! for example: gui << gui_only() << "text";
EXPORT void set_window_gui_only();
//! Override all colors into night mode: blue on black background.
EXPORT void night_mode();
//! Set the title of the currently selected window.
EXPORT void set_window_title(const char *s);
//! Force window size to hxw and forbid any resizing.
EXPORT void freeze_window_size(uint h, uint w);

/// 3d calls ///////////////////////////////////////////////////////////////

//! Add a sphere at position (x,y,z) with 'radius' and color (r,g,b,a).
EXPORT void draw_sphere(float x, float y, float z, float radius,
                        const char *s = NULL,
                        int r = 255, int g = 255, int b = 255, int a = 255);
//! Add a cylinder which base is centered at (x,y,z), with length 'length',
//! with radiuses 'top_radius' and 'base_radius', with degree angles
//! 'a1', 'a2' and color (r,g,b,a).
//! \param tops If true, draw closing caps on each end of cylinder.
EXPORT void draw_cylinder(float x, float y, float z, float length,
                          float top_radius, float base_radius,
                          float a1, float a2, const char *s = NULL,
                          int r = 255, int g = 255,
                          int b = 255, int a = 255, bool tops = false);
//! Draw 3d text 's' at (x,y,z) with color (r,g,b,a).
EXPORT void draw_text(float x, float y, float z, const char *s,
                      int r = 255, int g = 255, int b = 255, int a = 255);
//! Draw 3d line from (x,y,z) to (x1,y1,z1) with color (r,g,b,a)
//! and with text 's' at (x1,y1,z1).
EXPORT void draw_line(float x, float y, float z,
                      float x1, float y1, float z1,
                      const char *s = NULL, int r = 255, int g = 255,
                      int b = 255, int a = 255);


} // namespace ebl

#include "gui.hpp"

#endif /* GUI_H_ */
