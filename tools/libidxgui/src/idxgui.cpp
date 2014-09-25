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

#include "moc_idxgui.cxx"

using namespace std;

namespace ebl {

////////////////////////////////////////////////////////////////
// idxgui

idxgui::idxgui() {
  thread_init = false;
}

void idxgui::init(int argc_, char **argv_, const unsigned int *nwindows_,
                  gui_thread *gt_) {
  gt = gt_;
  argc = argc_;
  argv = argv_;
  nwindows = nwindows_;
  nwid = 0;
  str("");
  set_gui_only();
  main_done = false;
  thread_init = true;
}

idxgui::~idxgui() {
  wait();
}

void idxgui::draw_matrix_unsafe(idx<ubyte> &im, unsigned int h0,
                                unsigned int w0) {
  idx<ubyte> *uim = new idx<ubyte>(im);
  // send image to main gui thread
  emit gui_drawImage(uim, h0, w0);
}

void idxgui::check_init() {
  if (!thread_init) {
    cerr << "warning: trying to use gui function but gui wasn't initialized.";
    cerr << endl;
    cerr << "         replace your \"int main(int argc, char **argv) {\" by:";
    cerr << endl;
    cerr << "                      \"MAIN_QTHREAD(int, argc, char**, argv)";
    cerr << " {\"" << endl;
  }
}

void idxgui::quit() {
  check_init();
  emit appquit();
  // set main thread's internal bquit to true, so that it can skip
  // for queue of messages, in case the queue is too busy.
  gt->quit();
}

void idxgui::clear() {
  check_init();
  emit gui_clear();
}

void idxgui::clear_resize() {
  check_init();
  emit gui_clear_resize();
}

void idxgui::save_window(const char *filename, int wid) {
  check_init();
  emit gui_save_window(new string(filename), wid);
}

int idxgui::new_window(const char *wname, uint h, uint w) {
  check_init();
  // lock this block to make sure different threads don't use the same window
  mutex1.lock();
  int wid = nwid;
  nwid++; // increment number of windows
  emit gui_new_window(wname, h, w);
  mutex1.unlock();
  return wid;
}

int idxgui::new_window3d(const char *wname, uint h, uint w) {
  check_init();
  // lock this block to make sure different threads don't use the same window
  mutex1.lock();
  int wid = nwid;
  nwid++; // increment number of windows
  emit gui_new_window3d(wname, h, w);
  mutex1.unlock();
  return wid;
}

void idxgui::select_window(int wid) {
  check_init();
  if (wid < 0) {
    cerr << "cannot select a window with a negative id: " << wid << endl;
    eblerror("trying to select_window() with negative id");
  }
  emit gui_select_window(wid);
}

void idxgui::set_silent() {
  check_init();
  emit gui_set_silent(NULL);
}

void idxgui::set_silent(const std::string *filename) {
  check_init();
  emit gui_set_silent(filename);
}

void idxgui::set_silent(const char *filename) {
  check_init();
  if (filename)
    emit gui_set_silent(new string(filename));
  else
    emit gui_set_silent(NULL);
}

void idxgui::run() {
  run_main(argc, argv);
  main_done = true;
  if (*nwindows == 0) {
    quit();
  }
}

void idxgui::draw_text(std::string *s) {
  check_init();
  emit gui_add_text(s);
}

void idxgui::draw_text(std::string *s, unsigned int h0, unsigned int w0) {
  set_text_origin(h0, w0);
  emit gui_add_text(s);
}

void idxgui::draw_arrow(int h1, int w1, int h2, int w2) {
  check_init();
  emit gui_add_arrow(h1, w1, h2, w2);
}

void idxgui::draw_flow(idx<float> &flow, int h, int w) {
  check_init();
  idx<float> *f = new idx<float>(flow);
  emit gui_add_flow(f, h, w);
}

void idxgui::draw_box(float h0, float w0, float h, float w, ubyte r, ubyte g,
                      ubyte b, ubyte a, string *s) {
  check_init();
  emit gui_add_box(h0, w0, h, w, r, g, b, a, (s ? new string(*s) : s));
}

void idxgui::draw_cross(float h0, float w0, float length, ubyte r, ubyte g,
                        ubyte b, ubyte a, string *s) {
  check_init();
  emit gui_add_cross(h0, w0, length, r, g, b, a, (s ? new string(*s) : s));
}

void idxgui::draw_ellipse(float h0, float w0, float h, float w,
                          ubyte r, ubyte g, ubyte b, ubyte a, string *s) {
  check_init();
  emit gui_add_ellipse(h0, w0, h, w, r, g, b, a, (s ? new string(*s) : s));
}

void idxgui::set_text_origin(unsigned int h0, unsigned int w0) {
  check_init();
  emit gui_set_text_origin(h0, w0);
}

void idxgui::set_text_colors(int fg_r, int fg_g, int fg_b, int fg_a,
                             int bg_r, int bg_g, int bg_b, int bg_a) {
  check_init();
  set_text_colors((unsigned char) fg_r, (unsigned char) fg_g,
                  (unsigned char) fg_b, (unsigned char) fg_a,
                  (unsigned char) bg_r, (unsigned char) bg_g,
                  (unsigned char) bg_b, (unsigned char) bg_a);
}

void idxgui::set_text_colors(unsigned char fg_r, unsigned char fg_g,
                             unsigned char fg_b, unsigned char fg_a,
                             unsigned char bg_r, unsigned char bg_g,
                             unsigned char bg_b, unsigned char bg_a) {
  check_init();
  emit gui_set_text_colors(fg_r, fg_g, fg_b, fg_a,
                           bg_r, bg_g, bg_b, bg_a);
}

void idxgui::set_bg_colors(unsigned char r, unsigned char g,
                           unsigned char b) {
  check_init();
  emit gui_set_bg_colors(r, g, b);
}

void idxgui::set_font_size(int sz) {
  check_init();
  emit gui_set_font_size(sz);
}

void idxgui::set_global_font_size(int sz) {
  check_init();
  emit gui_set_global_font_size(sz);
}

void idxgui::enable_updates() {
  check_init();
  emit gui_set_wupdate(true);
}

void idxgui::disable_updates() {
  check_init();
  emit gui_set_wupdate(false);
}

void idxgui::freeze_style(bool freeze) {
  check_init();
  emit gui_freeze_style(freeze);
}

void idxgui::freeze_window_size(uint h, uint w) {
  check_init();
  emit gui_freeze_window_size(h, w);
}

idxgui& att(idxgui& r, unsigned int h0, unsigned int w0) {
  r.set_text_origin(h0, w0);
  return r;
}

ManipInfra<unsigned int, unsigned int> at(unsigned int h0, unsigned int w0) {
  return (ManipInfra<unsigned int, unsigned int>(att, h0, w0));
}

idxgui& fcout_and_gui(idxgui& r) {
  r.set_cout_and_gui();
  return r;
}

ManipInfra<int, int> cout_and_gui() {
  return (ManipInfra<int, int>(fcout_and_gui));
}

idxgui& fgui_only(idxgui& r) {
  r.set_gui_only();
  return r;
}

ManipInfra<int, int> gui_only() {
  return (ManipInfra<int, int>(fgui_only));
}

idxgui& fblack_on_white(idxgui& r, unsigned char fg_a, unsigned char bg_a) {
  r.set_text_colors(0, 0, 0, (int) fg_a, 255, 255, 255, (int) bg_a);
  return r;
}

ManipInfra<unsigned char, unsigned char> black_on_white(unsigned char fg_a,
                                                        unsigned char bg_a) {
  return (ManipInfra<unsigned char, unsigned char>(fblack_on_white,
                                                   fg_a, bg_a));
}

idxgui& fwhite_on_transparent(idxgui& r) {
  r.set_text_colors(255, 255, 255, 255, 0, 0, 0, 127);
  return r;
}

ManipInfra<int, int> white_on_transparent() {
  return (ManipInfra<int, int>(fwhite_on_transparent));
}

idxgui& fset_colors(idxgui& r, unsigned char fg_r, unsigned char fg_g,
                    unsigned char fg_b, unsigned char fg_a,
                    unsigned char bg_r, unsigned char bg_g,
                    unsigned char bg_b, unsigned char bg_a) {
  r.set_text_colors(fg_r, fg_g, fg_b, fg_a,
                    bg_r, bg_g, bg_b, bg_a);
  return r;
}

ManipInfra<unsigned char, unsigned char>
set_colors(unsigned char fg_r, unsigned char fg_g,
           unsigned char fg_b, unsigned char fg_a,
           unsigned char bg_r, unsigned char bg_g,
           unsigned char bg_b, unsigned char bg_a) {
  return (ManipInfra<unsigned char, unsigned char>(fset_colors,
                                                   fg_r, fg_g, fg_b, fg_a,
                                                   bg_r, bg_g, bg_b, bg_a));
}

void idxgui::set_cout_and_gui() {
  check_init();
  cout_output = true;
}

void idxgui::set_gui_only() {
  check_init();
  cout_output = false;
}

void idxgui::add_scroll_box(scroll_box0 *sb) {
  emit gui_add_scroll_box(sb);
}

int idxgui::pop_key_pressed() {
  return gt->pop_key_pressed();
}

bool idxgui::busy_drawing() {
  return gt->busy_drawing();
}

void idxgui::set_window_title(const char *s) {
  check_init();
  emit gui_set_title(new string(s));
}

// 3d signals ////////////////////////////////////////////////////////////////

void idxgui::draw_sphere(float x, float y, float z, float radius,
                         const char *s, int r, int g, int b, int a) {
  check_init();
  emit gui_add_sphere(x, y, z, radius, s ? new string(s) : NULL, r, g, b, a);
}

void idxgui::draw_cylinder(float x, float y, float z, float length,
                           float top_radius, float base_radius, float a1,
                           float a2, const char *s,
                           int r, int g, int b, int a, bool tops) {
  check_init();
  emit gui_add_cylinder(x, y, z, length, top_radius, base_radius,
                        a1, a2, s ? new string(s) : NULL, r, g, b, a, tops);
}

void idxgui::draw_text_3d(float x, float y, float z, const char *s,
                          int r, int g, int b, int a) {
  check_init();
  if (!s) eblerror("expected a non-null string");
  emit gui_draw_text_3d(x, y, z, new string(s), r, g, b, a);
}

void idxgui::draw_line_3d(float x, float y, float z,
                          float x1, float y1, float z1, const char *s,
                          int r, int g, int b, int a) {
  check_init();
  emit gui_draw_line_3d(x, y, z, x1, y1, z1,
                        s ? new string(s) : NULL, r, g, b, a);
}

idxgui& operator<<(idxgui& r, const ManipInfra<int,int> &manip) {
  manip(r);
  return r;
}

idxgui& operator<<(idxgui& r, const ManipInfra<uint,uint> &manip) {
  manip(r);
  return r;
}

} // end namespace ebl
