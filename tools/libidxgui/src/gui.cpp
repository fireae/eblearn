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

#include "gui.h"

using namespace std;

namespace ebl {

////////////////////////////////////////////////////////////////
// gui

int new_window(const char *wname, uint h, uint w) {
  return gui.new_window(wname, h, w);
}

int new_window(const string &wname, uint h, uint w) {
  return gui.new_window(wname.c_str(), h, w);
}

int new_window3d(const char *wname, uint h, uint w) {
  return gui.new_window3d(wname, h, w);
}

int new_window3d(const string &wname, uint h, uint w) {
  return gui.new_window3d(wname.c_str(), h, w);
}

void select_window(int wid) {
  gui.select_window(wid);
}

void disable_window_updates() {
  gui.disable_updates();
}

void enable_window_updates() {
  gui.enable_updates();
}

void draw_matrix(idx<ubyte> &im, uint h0, uint w0) {
  gui.draw_matrix(im, h0, w0);
}

void quit_gui() {
  gui.quit();
}

void clear_window() {
  gui.clear();
}

void clear_resize_window() {
  gui.clear_resize();
}

void save_window(const char *filename, int wid) {
  gui.save_window(filename, wid);
}

void draw_arrow(int h1, int w1, int h2, int w2) {
  gui.draw_arrow(h1, w1, h2, w2);
}

void draw_flow(idx<float> &flow, int h, int w) {
  gui.draw_flow(flow, h, w);
}

void draw_box(int h0, int w0, int h, int w, ubyte r, ubyte g, ubyte b,
              ubyte a, string *s) {
  gui.draw_box((float) h0, (float) w0, (float) h, (float) w, r, g, b, a, s);
}

void draw_box(rect<int> &box, ubyte r, ubyte g, ubyte b,
              ubyte a, string *s) {
  gui.draw_box((float) box.h0, (float) box.w0, (float) box.height,
               (float) box.width, r, g, b, a, s);
}

void draw_box(rect<float> &box, ubyte r, ubyte g, ubyte b,
              ubyte a, string *s) {
  gui.draw_box(box.h0, box.w0, box.height, box.width, r, g, b, a, s);
}

void draw_box(float h0, float w0, float h, float w, ubyte r, ubyte g, ubyte b,
              ubyte a, string *s) {
  gui.draw_box(h0, w0, h, w, r, g, b, a, s);
}

void draw_cross(float h0, float w0, float length, ubyte r, ubyte g, ubyte b,
                ubyte a, string *s) {
  gui.draw_cross(h0, w0, length, r, g, b, a, s);
}

void draw_ellipse(float h0, float w0, float h, float w,
                  ubyte r, ubyte g, ubyte b, ubyte a, string *s) {
  gui.draw_ellipse(h0, w0, h, w, r, g, b, a, s);
}

void set_gui_silent() {
  gui.set_silent();
}

void set_gui_silent(const std::string *filename) {
  gui.set_silent(filename);
}

void set_gui_silent(const char *filename) {
  gui.set_silent(filename);
}

void draw_text(std::string *s) {
  gui.draw_text(s);
}

void draw_text(std::string *s, unsigned int h0, unsigned int w0) {
  gui.draw_text(s, h0, w0);
}

void set_text_origin(unsigned int h0, unsigned int w0) {
  gui.set_text_origin(h0, w0);
}

void set_text_colors(unsigned char fg_r, unsigned char fg_g,
                     unsigned char fg_b, unsigned char fg_a,
                     unsigned char bg_r, unsigned char bg_g,
                     unsigned char bg_b, unsigned char bg_a) {
  gui.set_text_colors(fg_r, fg_g, fg_b, fg_a, bg_r, bg_g, bg_b, bg_a);
}

void set_text_colors(int fg_r, int fg_g, int fg_b, int fg_a,
                     int bg_r, int bg_g, int bg_b, int bg_a) {
  gui.set_text_colors(fg_r, fg_g, fg_b, fg_a, bg_r, bg_g, bg_b, bg_a);
}

void set_bg_colors(int r, int g, int b) {
  gui.set_bg_colors(r, g, b);
}

void set_font_size(int sz) {
  gui.set_font_size(sz);
}

void set_global_font_size(int sz) {
  gui.set_global_font_size(sz);
}

void set_window_cout_and_gui() {
  gui.set_cout_and_gui();
}

void set_window_gui_only() {
  gui.set_gui_only();
}

void night_mode() {
  gui.set_bg_colors(0, 0, 0);
  gui.set_text_colors(255, 255, 255, 255, 0, 0, 0, 127);
  //gui.freeze_style(true);
}

void set_window_title(const char *s) {
  gui.set_window_title(s);
}

void freeze_window_size(uint h, uint w) {
  gui.freeze_window_size(h, w);
}

// 3d calls //////////////////////////////////////////////////////////////////

void draw_sphere(float x, float y, float z, float radius,
                 const char *s, int r, int g, int b, int a) {
  gui.draw_sphere(x, y, z, radius, s, r, g, b, a);
}

void draw_cylinder(float x, float y, float z, float length,
                   float top_radius, float base_radius, float a1,
                   float a2, const char *s, int r, int g, int b,
                   int a, bool tops) {
  gui.draw_cylinder(x, y, z, length, top_radius, base_radius,
                    a1, a2, s, r, g, b, a, tops);
}

void draw_text(float x, float y, float z, const char *s,
               int r, int g, int b, int a) {
  gui.draw_text_3d(x, y, z, s, r, g, b, a);
}

void draw_line(float x, float y, float z, float x1, float y1, float z1,
               const char *s, int r, int g, int b, int a) {
  gui.draw_line_3d(x, y, z, x1, y1, z1, s, r, g, b, a);
}

} // end namespace ebl
