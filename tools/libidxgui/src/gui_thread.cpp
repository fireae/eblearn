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

#include "moc_gui_thread.cxx"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // global variables

  // global variable
  idxgui gui;

  ////////////////////////////////////////////////////////////////
  // cons/destructors

  gui_thread::gui_thread(int argc, char** argv, QApplication &qa)
    : thread(gui), wcur(-1), nwindows(0), silent(false), busy(false),
      bquit(false), qapp(qa), global_font_size(-1) {
    thread.init(argc, argv, &nwindows, this);

    // register exotic types
    qRegisterMetaType<ubyte>("ubyte");
    // connect methods to incoming signals
    connect(&thread, SIGNAL(gui_drawImage(idx<ubyte> *, uint, uint)),
	    this, SLOT(updatePixmap(idx<ubyte> *, uint, uint)));
    connect(&thread, SIGNAL(gui_draw_mask(idx<ubyte>*, uint, uint, ubyte,
					  ubyte, ubyte, ubyte)), this,
	    SLOT(add_mask(idx<ubyte>*, uint,uint, ubyte, ubyte, ubyte, ubyte)));
    connect(&thread, SIGNAL(appquit()), this, SLOT(appquit()));
    connect(&thread, SIGNAL(gui_clear()), this, SLOT(clear()));
    connect(&thread, SIGNAL(gui_clear_resize()), this, SLOT(clear_resize()));
    connect(&thread, SIGNAL(gui_save_window(const string*, int)),
			    this, SLOT(save_window(const string*, int)));
    connect(&thread, SIGNAL(gui_new_window(const char*, uint, uint)),
	    this, SLOT(new_window(const char*, uint, uint)));
    connect(&thread, SIGNAL(gui_new_window3d(const char*, uint, uint)),
	    this, SLOT(new_window3d(const char*, uint, uint)));
    connect(&thread, SIGNAL(gui_select_window(int)),
	    this, SLOT(select_window(int)));
    connect(&thread, SIGNAL(gui_add_text(const string*)),
	    this, SLOT(add_text(const string*)));
    connect(&thread, SIGNAL(gui_add_arrow(int, int, int, int)),
	    this, SLOT(add_arrow(int, int, int, int)));
    connect(&thread, SIGNAL(gui_add_flow(idx<float>*, int, int)),
	    this, SLOT(add_flow(idx<float>*, int, int)));
    connect(&thread, SIGNAL(gui_add_box(float, float, float, float, ubyte,
					ubyte, ubyte, ubyte, string *)),
	    this, SLOT(add_box(float, float, float, float, ubyte, ubyte, ubyte,
			       ubyte, string *)));
    connect(&thread, SIGNAL(gui_add_cross(float, float, float, ubyte, ubyte,
					  ubyte, ubyte, string *)),
	    this, SLOT(add_cross(float, float, float, ubyte, ubyte, ubyte,
				 ubyte, string *)));
    connect(&thread, SIGNAL(gui_add_ellipse(float, float, float, float, ubyte,
					    ubyte, ubyte, ubyte, string *)),
	    this, SLOT(add_ellipse(float, float, float, float, ubyte, ubyte,
				   ubyte, ubyte, string *)));
    connect(&thread, SIGNAL(gui_set_text_origin(unsigned int, unsigned int)),
	    this, SLOT(set_text_origin(unsigned int, unsigned int)));
    connect(&thread, SIGNAL(gui_set_text_colors(unsigned char, unsigned char,
						unsigned char, unsigned char,
						unsigned char, unsigned char,
						unsigned char, unsigned char)),
	    this, SLOT(set_text_colors(unsigned char, unsigned char,
				       unsigned char, unsigned char,
				       unsigned char, unsigned char,
				       unsigned char, unsigned char)));
    connect(&thread, SIGNAL(gui_set_bg_colors(unsigned char,
					      unsigned char, unsigned char)),
	    this, SLOT(set_bg_colors(unsigned char,
				     unsigned char, unsigned char)));
    connect(&thread, SIGNAL(gui_set_font_size(int)),
            this, SLOT(set_font_size(int)));
    connect(&thread, SIGNAL(gui_set_global_font_size(int)),
            this, SLOT(set_global_font_size(int)));
    connect(&thread, SIGNAL(gui_set_silent(const std::string *)),
	    this, SLOT(set_silent(const std::string *)));
    connect(&thread, SIGNAL(gui_set_wupdate(bool)),
	    this, SLOT(set_wupdate(bool)));
    connect(&thread, SIGNAL(gui_freeze_style(bool)),
	    this, SLOT(freeze_style(bool)));
    connect(&thread, SIGNAL(gui_freeze_window_size(uint, uint)),
	    this, SLOT(freeze_window_size(uint, uint)));
    connect(&thread, SIGNAL(gui_add_scroll_box(scroll_box0*)),
	    this, SLOT(add_scroll_box(scroll_box0*)));
    connect(&thread, SIGNAL(gui_set_title(const string*)),
	    this, SLOT(set_title(const string*)));
    // 3d calls ////////////////////////////////////////////////////////////////
    connect(&thread,
	    SIGNAL(gui_add_sphere(float,float,float,float,
				  string*,int,int,int,int)),
	    this, SLOT(add_sphere(float,float,float,float,
				  string*,int,int,int,int)));
    connect(&thread,
	    SIGNAL(gui_add_cylinder(float,float,float,float,float,float,
				    float,float,string*,int,int,int,int,bool)),
	    this, SLOT(add_cylinder(float,float,float,float,float,float,
				    float,float,string*,int,int,int,int,bool)));
    connect(&thread,
	    SIGNAL(gui_draw_text_3d(float,float,float,string*,
				    int,int,int,int)),
	    this, SLOT(draw_text_3d(float,float,float,string*,
				    int,int,int,int)));
    connect(&thread,
	    SIGNAL(gui_draw_line_3d(float,float,float,float,float,float,string*,
				    int,int,int,int)),
	    this, SLOT(draw_line_3d(float,float,float,float,float,float,string*,
				    int,int,int,int)));
  }

  gui_thread::~gui_thread() {
    for (vector<win*>::iterator i = windows.begin(); i != windows.end(); ++i)
      if (*i)
	delete *i;
  }

  void gui_thread::window_destroyed(QObject *obj) {
    for (uint i = 0; i < windows.size(); ++i) {
      // decrement windows counter
      if (windows[i] && windows[i]->get_widget() == obj) {
	windows[i] = NULL;
	nwindows--;
      }
    }
    if (gui.main_done && (nwindows == 0))
      appquit();
  }

  ////////////////////////////////////////////////////////////////
  // add methods

  void gui_thread::add_text(const string *s) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->add_text(s);
  }

  void gui_thread::add_arrow(int x1, int y1, int x2, int y2) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->add_arrow(x1, y1, x2, y2);
  }

  void gui_thread::add_flow(idx<float> *flow, int h, int w) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->add_flow(flow, h, w);
  }

  void gui_thread::add_box(float h0, float w0, float h, float w,
			   ubyte r, ubyte g, ubyte b, ubyte a, string *s) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->add_box(h0, w0, h, w, r, g, b, a, s);
  }

  void gui_thread::add_cross(float h0, float w0, float length,
			     ubyte r, ubyte g, ubyte b, ubyte a, string *s) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->add_cross(h0, w0, length, r, g, b, a, s);
  }

  void gui_thread::add_ellipse(float h0, float w0, float h, float w,
			       ubyte r, ubyte g, ubyte b, ubyte a, string *s) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->add_ellipse(h0, w0, h, w, r, g, b, a, s);
  }

  void gui_thread::set_text_origin(unsigned int h0, unsigned int w0) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->set_text_origin(h0, w0);
  }

  void gui_thread::set_text_colors(unsigned char fg_r, unsigned char fg_g,
				   unsigned char fg_b, unsigned char fg_a,
				   unsigned char bg_r, unsigned char bg_g,
				   unsigned char bg_b, unsigned char bg_a) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->set_text_colors(fg_r, fg_g, fg_b, fg_a,
				     bg_r, bg_g, bg_b, bg_a);
  }

  void gui_thread::set_bg_colors(unsigned char r, unsigned char g,
				 unsigned char b) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->set_bg_colors(r, g, b);
  }

  void gui_thread::set_font_size(int sz) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->set_font_size(sz);
  }

  void gui_thread::set_global_font_size(int sz) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if (sz <= 0) return ;
    global_font_size = sz;
    for (vector<win*>::iterator i = windows.begin(); i != windows.end(); ++i) {
      if (*i) (*i)->set_font_size(sz);
    }
  }

  void gui_thread::set_wupdate(bool update) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur])) {
      // when drawing, turn busy flag on
      windows[wcur]->set_wupdate(update);
      if (update)
	busy = false;
      else
	busy = true;
    }
  }

  void gui_thread::freeze_style(bool freeze) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->freeze_style(freeze);
  }

  void gui_thread::freeze_window_size(uint h, uint w) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->freeze_window_size(h, w);
  }

  void gui_thread::set_silent(const std::string *filename) {
    if (bquit) return ; // do not do any work if we are trying to quit
    silent = true;
    for (vector<win*>::iterator i = windows.begin(); i != windows.end(); ++i) {
      if (*i) (*i)->set_silent(filename);
    }
    if (filename) {
      savefname = *filename;
      delete filename;
    }
  }

  void gui_thread::appquit() {
    qapp.quit();
    exit(0);
  }

  // add image
  void gui_thread::updatePixmap(idx<ubyte> *img, uint h0, uint w0) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if (nwindows == 0)
      new_window();
    if ((wcur >= 0) && (windows[wcur])) {
      win2d *w = dynamic_cast<win2d*>(windows[wcur]);
      if (!w) eblerror("drawing requires a 2d window");
      w->update_pixmap(img, h0, w0);
    }
  }

  // add mask
  void gui_thread::add_mask(idx<ubyte> *img, uint h0, uint w0, ubyte r,
			    ubyte g, ubyte b, ubyte a) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if (nwindows == 0)
      new_window();
    if ((wcur >= 0) && (windows[wcur])) {
      win2d *w = dynamic_cast<win2d*>(windows[wcur]);
      if (!w) eblerror("drawing requires a 2d window");
      w->add_mask(img, h0, w0, r, g, b, a);
    }
  }

  void gui_thread::clear() {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->clear();
  }

  void gui_thread::clear_resize() {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->clear_resize();
  }

  void gui_thread::save_window(const string *filename, int wid) {
    if (bquit) return ; // do not do any work if we are trying to quit
    int id = wid < 0 ? wcur : wid;
    if ((id >= 0) && (windows[id])) {
      if (filename) {
	windows[id]->save(*filename);
	delete filename;
      }
      else
	cerr << "warning: trying to save window with NULL filename" << endl;
    }
  }

  void gui_thread::new_window(const char *wname, uint h, uint w) {
    if (bquit) return ; // do not do any work if we are trying to quit
    mutex1.lock();
    windows.push_back(new win2d(windows.size(), wname, h, w));
    wcur = windows.size() - 1;
    nwindows++;
    if (silent) windows[wcur]->set_silent(&savefname);
    connect(windows[wcur]->get_widget(), SIGNAL(destroyed(QObject*)),
    	    this, SLOT(window_destroyed(QObject*)));
    if (global_font_size > 0)
      windows[wcur]->set_font_size(global_font_size);
    mutex1.unlock();
  }

  void gui_thread::new_window3d(const char *wname, uint h, uint w) {
    if (bquit) return ; // do not do any work if we are trying to quit
    mutex1.lock();
    //    QGL::setPreferredPaintEngine ( QPaintEngine::OpenGL );

    windows.push_back(new win3d(windows.size(), wname, h, w));
    wcur = windows.size() - 1;
    nwindows++;
    if (silent)
      windows[wcur]->set_silent(&savefname);
    connect(windows[wcur]->get_widget(), SIGNAL(destroyed(QObject*)),
    	    this, SLOT(window_destroyed(QObject*)));
    mutex1.unlock();
  }

  void gui_thread::select_window(int wid) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if (wid >= (int) windows.size()) {
      cerr << "gui Warning: trying to select an unknown window (id = ";
      cerr << wid << ")." << endl;
    }
    else if (windows[wid] == NULL) {
      wcur = -1;
      //cerr <<
      //"idxGui Warning: trying to select an window that was destroyed (id = ";
      //cerr << wid << ")." << endl;
    }
    else if (wid < 0) {
      cerr << "gui warning: trying to select a window with negative id: ";
      cerr << wid << endl;
    }
    else {
      wcur = wid;
      if ((windows[wcur]) && (!silent)) {
	windows[wcur]->show();
      }
    }
  }

  void gui_thread::add_scroll_box(scroll_box0 *sb) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur])) {
      win2d *w = dynamic_cast<win2d*>(windows[wcur]);
      if (!w) eblerror("drawing requires a 2d window");
      w->add_scroll_box(sb);
    }
  }

  void gui_thread::set_title(const string *s) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->set_title(s->c_str());
    delete s; // caller expects us to delete this string
  }

  ////////////////////////////////////////////////////////////////
  // public methods

  int gui_thread::pop_key_pressed() {
    if ((wcur >= 0) && (windows[wcur]))
      return windows[wcur]->pop_key_pressed();
    return -1;
  }

  bool gui_thread::busy_drawing() {
    bool b = busy;
    if ((wcur >= 0) && (windows[wcur]))
      b = b || windows[wcur]->busy_drawing();
    return b;
  }

  void gui_thread::quit() {
    mutex1.lock();
    bquit = true;
    mutex1.unlock();
  }

  //////////////////////////////////////////////////////////////////////////////
  // 3d calls

  void gui_thread::add_sphere(float x, float y, float z, float radius,
			      string *s, int r, int g, int b, int a) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if (nwindows == 0)
      new_window();
    if ((wcur >= 0) && (windows[wcur])) {
      win3d *w = dynamic_cast<win3d*>(windows[wcur]);
      if (!w) eblerror("drawing a sphere3d requires a 3d window");
#ifdef __GUI3D__
      w->add_sphere(x, y, z, radius, s?s->c_str():NULL, r, g, b, a);
#endif
    }
    // if we don't delete this string, no one will
    if (s) delete s;
  }

  void gui_thread::add_cylinder(float x, float y, float z, float length,
				float top_radius, float base_radius,
				float a1, float a2, string *s,
				int r, int g, int b, int a, bool tops) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if (nwindows == 0)
      new_window();
    if ((wcur >= 0) && (windows[wcur])) {
      win3d *w = dynamic_cast<win3d*>(windows[wcur]);
      if (!w) eblerror("drawing a cylinder3d requires a 3d window");
#ifdef __GUI3D__
      w->add_cylinder(x, y, z, length, top_radius, base_radius,
		      a1, a2, s?s->c_str():NULL, r, g, b, a, tops);
#endif
    }
    // if we don't delete this string, no one will
    if (s) delete s;
  }

  void gui_thread::draw_text_3d(float x, float y, float z, string *s,
				int r, int g, int b, int a) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if (nwindows == 0)
      new_window();
    if ((wcur >= 0) && (windows[wcur])) {
      win3d *w = dynamic_cast<win3d*>(windows[wcur]);
      if (!w) eblerror("drawing a text3d requires a 3d window");
#ifdef __GUI3D__
      w->add_text(x, y, z, s->c_str(), r, g, b, a);
#endif
    }
    // if we don't delete this string, no one will
    if (s) delete s;
  }

  void gui_thread::draw_line_3d(float x, float y, float z,
				float x1, float y1, float z1, string *s,
				int r, int g, int b, int a) {
    if (bquit) return ; // do not do any work if we are trying to quit
    if (nwindows == 0)
      new_window();
    if ((wcur >= 0) && (windows[wcur])) {
      win3d *w = dynamic_cast<win3d*>(windows[wcur]);
      if (!w) eblerror("drawing a line3d requires a 3d window");
#ifdef __GUI3D__
      w->add_line(x, y, z, x1, y1, z1, s ? s->c_str() : NULL, r, g, b, a);
#endif
    }
    // if we don't delete this string, no one will
    if (s) delete s;
  }

} // end namespace ebl
