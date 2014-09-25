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

#include "win.h"
#include <qstyle.h>
#include <math.h>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // text

  text::text(float h0_, float w0_, bool pos_reset_,
	     unsigned char fg_r_, unsigned char fg_g_,
	     unsigned char fg_b_, unsigned char fg_a_,
	     unsigned char bg_r_, unsigned char bg_g_,
	     unsigned char bg_b_, unsigned char bg_a_)
    : string(""), h0(h0_), w0(w0_), pos_reset(pos_reset_),
      fg_r(fg_r_), fg_g(fg_g_), fg_b(fg_b_), fg_a(fg_a_),
      bg_r(bg_r_), bg_g(bg_g_), bg_b(bg_b_), bg_a(bg_a_) {
  }

  ////////////////////////////////////////////////////////////////
  // arrow

  arrow::arrow(int h1_, int w1_, int h2_, int w2_, bool head1_, bool head2_)
    : h1(h1_), w1(w1_), h2(h2_), w2(w2_), head1(head1_), head2(head2_) {
  }

  ////////////////////////////////////////////////////////////////
  // box

  box::box(float h0_, float w0_, float h_, float w_, ubyte r_, ubyte g_,
	   ubyte b_, ubyte a_)
    : h0(h0_), w0(w0_), h(h_), w(w_), r(r_), g(g_), b(b_), a(a_) {
  }

  ////////////////////////////////////////////////////////////////
  // cross

  cross::cross(float h0_, float w0_, float length_,
	       ubyte r_, ubyte g_, ubyte b_, ubyte a_)
    : h0(h0_), w0(w0_), length(length_), r(r_), g(g_), b(b_), a(a_) {
  }

  ////////////////////////////////////////////////////////////////
  // image

  image::image(idx<ubyte> &img_, unsigned int h0_, unsigned int w0_)
    : h0(h0_), w0(w0_) {
    img = img_;
  }

  ////////////////////////////////////////////////////////////////
  // imask

  imask::imask(idx<ubyte> *img, unsigned int h0_, unsigned int w0_,
	       ubyte r, ubyte g, ubyte b, ubyte a)
    : h0(h0_), w0(w0_), map(img->dim(1), img->dim(0)) {
    // fill the pixmap with desired color
    map.fill(QColor(r, g, b, a));
    // set mask
    QImage qim((unsigned char*) img->idx_ptr(), img->dim(1), img->dim(0),
	       img->dim(1) * img->dim(2) * sizeof (unsigned char),
	       QImage::Format_RGB888);
    map.setMask(QBitmap::fromImage(qim));
  }

  ////////////////////////////////////////////////////////////////
  // win

  win::win(QWidget *qw_, uint wid, const char *wname,
	   uint height, uint width)
    : qw(qw_),
      pixmapScale(1.0), curScale(1.0), scaleIncr(1), colorTable(256),
      texts(), silent(false), id(wid), savefname(""), wupdate_ndisable(0),
      frozen_style(false), frozen_size(false), font_size(-1), ctrl_on(false),
      text_on(true), images_only(false), scrollbox(NULL), drawing_mode(0) {
    if (!qw) eblerror("expected a non-null QWidget pointer in qw");
    qw->setAttribute(Qt::WA_DeleteOnClose);
    if (wname) {
      QString q(wname);
      qw->setWindowTitle(q);
    } else {
      QString q("EBLearn Graphics");
      qw->setWindowTitle(q);
    }
    pixmap = new QPixmap(1, 1);
    buffer = NULL;
    qimage = NULL;
    for (int i = 0; i < 256; i++){
      colorTable[i] = qRgb(i, i, i);
    }
    clear();
    wupdate = true; // always update display
    set_text_colors(255, 255, 255, 255, 0, 0, 0, 127); // default text color
    set_bg_colors(255, 255, 255); // default background color is white
    buffer_maxh = height;
    buffer_maxw = width;
    pos_reset = true;
    scrollbox = NULL;
    qw->setFocusPolicy(Qt::NoFocus);
    busy = false;
  }

  win::win() {
  }

  win::~win() {
    clear();
  }

  void win::show() {
    qw->show();
  }

  QWidget* win::get_widget() {
    return qw;
  }

  void win::set_wupdate(bool ud) {
    cout << "warning: set_wupdate not implemented" << endl;
  }

  void win::update_window(bool activate) {
    cout << "warning: update_window not implemented" << endl;
  }

  void win::resize_window(uint h, uint w, bool force) {
    cout << "warning: resize_window not implemented" << endl;
  }

  bool win::busy_drawing() {
    return busy;
  }

  ////////////////////////////////////////////////////////////////
  // add methods

  void win::add_text(const string *s) {
    if (!txt) {
      txt = new text(text_h0, text_w0, pos_reset, fg_r, fg_g, fg_b, fg_a,
		     bg_r, bg_g, bg_b, bg_a);
      if (!wupdate)
	texts_tmp.push_back(txt);
      else
	texts.push_back(txt);
    }
    *txt += *s;
    delete s;
    update_window();
    pos_reset = false;
  }

  void win::add_arrow(int h1, int w1, int h2, int w2) {
    arrow *a = new arrow(h1, w1, h2, w2);
    if (!wupdate)
      arrows_tmp.push_back(a);
    else
      arrows.push_back(a);
    update_window();
  }

  void win::add_flow(idx<float> *flow, int h0, int w0) {
    if (flow) {
      for (uint h = 0; h < flow->dim(1); ++h) {
	for (uint w = 0; w < flow->dim(2); ++w) {
	  float fh = flow->get(0, h, w);
	  float fw = flow->get(1, h, w);
	  float hh = h0 + flow->dim(1) - h;
	  float ww = w0 + flow->dim(2) - w;
	  if (fh != 0 && fw != 0) {
	    arrow *a = new arrow((int) ww, (int) hh, (int) (ww + fw),
				 (int) (hh + fh), false);
	    if (!wupdate) arrows_tmp.push_back(a);
	    else arrows.push_back(a);
	  }
	}
      }
      update_window();
      delete flow;
    }
  }

  void win::add_box(float h0, float w0, float h, float w, ubyte r, ubyte g,
		    ubyte b, ubyte a, string *s) {
    box *bb = new box(h0, w0, h, w, r, g, b, a);
    // add box
    if (!wupdate)
      boxes_tmp.push_back(bb);
    else
      boxes.push_back(bb);
    // add caption
    //    set_text_origin(h0 + 1, w0 + 1);
    set_text_origin(h0 + h - 16, w0 + 1);
    // modulate caption transparency with bbox's transparency
    ubyte save_fga = fg_a, save_bga = bg_a;
    set_text_colors(fg_r, fg_g, fg_b, a, bg_r, bg_g, bg_b, a, true);
    if (s)
      add_text(s);
    // restore previous transparency
    set_text_colors(fg_r, fg_g, fg_b, save_fga, bg_r, bg_g, bg_b, save_bga,
		    true);
    update_window();
  }

  void win::add_cross(float h0, float w0, float length, ubyte r, ubyte g,
		      ubyte b, ubyte a, string *s) {
    cross *c = new cross(w0, h0, length, r, g, b, a);
    // add cross
    if (!wupdate)
      crosses_tmp.push_back(c);
    else
      crosses.push_back(c);
    // add caption
    set_text_origin(h0 + 1, w0 + 1);
    if (s)
      add_text(s);
    update_window();
  }

  void win::add_ellipse(float h0, float w0, float h, float w,
			ubyte r, ubyte g, ubyte b, ubyte a, string *s) {
    box *c = new box(w0, h0, h, w, r, g, b, a);
    // add ellipse
    if (!wupdate)
      ellipses_tmp.push_back(c);
    else
      ellipses.push_back(c);
    // add caption
    set_text_origin(h0 + 1, w0 + 1);
    if (s)
      add_text(s);
    update_window();
  }

  void win::add_image(idx<ubyte> &img, unsigned int h0, unsigned int w0) {
    image *i = new image(img, h0, w0);
    if (!wupdate)
      images_tmp.push_back(i);
    else
      images.push_back(i);
    update_window();
  }

  void win::add_mask(idx<ubyte> *img, unsigned int h0, unsigned int w0,
		     ubyte r, ubyte g, ubyte b, ubyte a) {
    imask *m = new imask(img, h0, w0, r, g, b, a);
    if (!wupdate)
      masks_tmp.push_back(m);
    else
      masks.push_back(m);
    // update maximum buffer size
    buffer_maxh = std::max(buffer_maxh, std::max(buffer?(uint)buffer->dim(0):0,
						 (uint) (h0 + img->dim(0))));
    buffer_maxw = std::max(buffer_maxw, std::max(buffer?(uint)buffer->dim(1):0,
						 (uint) (w0 + img->dim(1))));
    // we are responsible for deleting img
    delete img;
    update_window();
  }

  void win::set_text_origin(float h0, float w0) {
    text_h0 = h0;
    text_w0 = w0;
    txt = NULL;
    pos_reset = true;
  }

  ////////////////////////////////////////////////////////////////
  // clear methods

  void win::clear() {
    clear_all();
  }

  void win::clear_resize() {
    resize_window(1, 1);
    clear_all();
    update_window();
  }

  void win::clear_all(bool clear_tmp) {
    clear_text(clear_tmp);
    clear_arrows(clear_tmp);
    clear_boxes(clear_tmp);
    clear_crosses(clear_tmp);
    clear_ellipses(clear_tmp);
    clear_images(clear_tmp);
    clear_masks(clear_tmp);
  }

  void win::clear_text(bool clear_tmp) {
    vector<text*> *t = &texts;
    if (clear_tmp)
      t = &texts_tmp;
    for (vector<text*>::iterator i = t->begin(); i != t->end(); ++i)
      if (*i)
	delete (*i);
    t->clear();
    txt = NULL;
    text_h0 = 0;
    text_w0 = 0;
  }

  void win::clear_arrows(bool clear_tmp) {
    vector<arrow*> *a = &arrows;
    if (clear_tmp)
      a = &arrows_tmp;
    for (vector<arrow*>::iterator i = a->begin(); i != a->end(); ++i)
      if (*i)
	delete (*i);
    a->clear();
  }

  void win::clear_boxes(bool clear_tmp) {
    vector<box*> *b = &boxes;
    if (clear_tmp)
      b = &boxes_tmp;
    for (vector<box*>::iterator i = b->begin(); i != b->end(); ++i)
      if (*i)
	delete (*i);
    b->clear();
  }

  void win::clear_crosses(bool clear_tmp) {
    vector<cross*> *b = &crosses;
    if (clear_tmp)
      b = &crosses_tmp;
    for (vector<cross*>::iterator i = b->begin(); i != b->end(); ++i)
      if (*i)
	delete (*i);
    b->clear();
  }

  void win::clear_ellipses(bool clear_tmp) {
    vector<box*> *b = &ellipses;
    if (clear_tmp)
      b = &ellipses_tmp;
    for (vector<box*>::iterator i = b->begin(); i != b->end(); ++i)
      if (*i)
	delete (*i);
    b->clear();
  }

  void win::clear_images(bool clear_tmp) {
    vector<image*> *ims = &images;
    if (clear_tmp)
      ims = &images_tmp;
    for (vector<image*>::iterator i = ims->begin(); i != ims->end(); ++i)
      if (*i)
	delete (*i);
    ims->clear();
    buffer_maxh = 0;
    buffer_maxw = 0;
  }

  void win::clear_masks(bool clear_tmp) {
    vector<imask*> *m = &masks;
    if (clear_tmp)
      m = &masks_tmp;
    for (vector<imask*>::iterator i = m->begin(); i != m->end(); ++i)
      if (*i)
	delete (*i);
    m->clear();
    buffer_maxh = 0;
    buffer_maxw = 0;
  }

  ////////////////////////////////////////////////////////////////

  void win::save(const string &filename, bool confirm) {
    QPixmap p = QPixmap::grabWidget(qw, qw->rect());
    string fname;
    fname << filename << ".png";
    // make sure directory exists
    string dir = dirname(fname.c_str());
    mkdir_full(dir);
    if (!p.save(fname.c_str(), "PNG", 90))
      cerr << "Warning: failed to save window to " << filename << "." << endl;
    else if (confirm)
      cout << "Saved " << fname << endl;
  }

  void win::save_mat(const string &filename, bool confirm) {
    if (!this->buffer) {
      cerr << "Warning: cannot save buffer matrix, not allocated." << endl;
      return ;
    }
    string fname = filename;
    fname += ".mat";
    try {
      save_matrix(*this->buffer, fname);
      if (confirm)
	cout << "Saved " << fname << endl;
    } eblcatch();
  }

  void win::set_silent(const std::string *filename) {
    silent = true;
    ostringstream o;
    if ((filename) && (strcmp(filename->c_str(), "") != 0))
      o << *filename << ".";
    o << id << ".png";
    savefname = o.str();
    qw->hide();
    qw->update();
  }

  void win::freeze_style(bool freeze) {
    frozen_style = freeze;
  }

  void win::freeze_window_size(uint h, uint w) {
    resize_window(h, w, true);
    frozen_dims = idxdim(h, w);
    cout << "Freezing window size to " << frozen_dims << endl;
    frozen_size = true;
  }

  void win::set_title(const char *title) {
    QString t(title);
    qw->setWindowTitle(t);
  }

  ////////////////////////////////////////////////////////////////
  // style methods

  void win::set_text_colors(ubyte fg_r_, ubyte fg_g_, ubyte fg_b_, ubyte fg_a_,
			    ubyte bg_r_, ubyte bg_g_, ubyte bg_b_, ubyte bg_a_,
			    bool ignore_frozen) {
    txt = NULL;
    if (frozen_style && !ignore_frozen)
      return ;
    fg_r = fg_r_;
    fg_g = fg_g_;
    fg_b = fg_b_;
    fg_a = fg_a_;
    bg_r = bg_r_;
    bg_g = bg_g_;
    bg_b = bg_b_;
    bg_a = bg_a_;
  }

  void win::set_bg_colors(ubyte r, ubyte g, ubyte b) {
    if (frozen_style)
      return ;
    bg_color.setRed(r);
    bg_color.setGreen(g);
    bg_color.setBlue(b);
  }

  void win::set_font_size(int sz) {
    font_size = sz;
  }

  ////////////////////////////////////////////////////////////////
  // events methods

  void win::wheelEvent(QWheelEvent *event) {
    float precision = .25;
    float sign = (event->delta() > 0)? precision : -precision;
    if ((scaleIncr >= -1) && (scaleIncr <= 1)) {
      if (sign > 0)
	scaleIncr = 1.0;
      else
	scaleIncr = -1;
    }
    scaleIncr += sign;
    if ((scaleIncr >= -1) && (scaleIncr <= 1)) {
      if (sign > 0)
	scaleIncr = 1.0;
      else
	scaleIncr = -1;
    }
    if (scaleIncr >= 1)
      pixmapScale = scaleIncr;
    else
      pixmapScale = std::fabs(1 / scaleIncr);
    qw->update();
  }

  void win::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
      lastDragPos = event->pos();
      QCursor qc(Qt::SizeAllCursor);
      qw->setCursor(qc);
    }
  }

  void win::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
      pixmapOffset += event->pos() - lastDragPos;
      lastDragPos = event->pos();
      qw->update();
    }
  }

  void win::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
      pixmapOffset += event->pos() - lastDragPos;
      lastDragPos = QPoint();

      int deltaX = (qw->width() - pixmap->width()) / 2 - pixmapOffset.x();
      int deltaY = (qw->height() - pixmap->height()) / 2 - pixmapOffset.y();
      qw->scroll(deltaX, deltaY);
      QCursor qc(Qt::ArrowCursor);
      qw->setCursor(qc);
      qw->update();
    }
  }

  void win::keyPressEvent(QKeyEvent *event) {
    // push key on the list
    keyspressed.push_back(event->key());
    // handle key events
    if (event->key() == Qt::Key_Escape)
      qw->close();
    else if (event->key() == Qt::Key_Left) {
      if (scrollbox) {
	cout << "display_previous" << endl;
	scrollbox->display_previous();
      }
    } else if (event->key() == Qt::Key_Right) {
      if (scrollbox) {
	cout << "display_next" << endl;
	scrollbox->display_next();
      }
    } else if (event->key() == Qt::Key_Control) {
      ctrl_on = true;
    } else if (event->key() == Qt::Key_T) {
      if (ctrl_on) {
	if (text_on) { // text is currently displayed
	  text_on = false;
	  cout << "Disabling Text in window (ctrl + t)" << endl;
	  qw->update();
	} else { // text not currently displayed
	  text_on = true;
	  cout << "Enabling Text in window (ctrl + t)" << endl;
	  qw->update();
	}
      }
    } else if (event->key() == Qt::Key_S) {
      string fname;
      fname << "win" << id;
      save(fname, true);
      save_mat(fname, true);
    }
  }

  void win::keyReleaseEvent(QKeyEvent *event) {
    // handle key events
    if (event->key() == Qt::Key_Control) {
      ctrl_on = false;
    }
  }

  int win::pop_key_pressed() {
    if (keyspressed.size() == 0)
      return -1;
    int key = keyspressed.front();
    keyspressed.pop_front();
    return key;
  }

  void win::add_scroll_box(scroll_box0 *sb) {
    scrollbox = sb;
    scrollbox->set_parent(this);
    //cout << "adding scrollbox " << sb << endl;
  }

  void win::remove_scroll_box(scroll_box0 *sb) {
    scrollbox = NULL;
    //cout << "removing scroll box " << sb << endl;
  }

  void win::replace_scroll_box_with_copy(scroll_box0 *sb) {
    //cout << "replacing scroll box " << scrollbox << " with a copy: " << endl;
    scrollbox = sb->copy();
    //cout << scrollbox << endl;
  }

  //////////////////////////////////////////////////////////////////////////////
  // drawing methods

  void win::paint(QPainter &painter, double scale) {
    draw_text(painter);
  }

  void win::draw_text(QPainter &painter) {
    if (!text_on || images_only) // text can be disabled
      return ;
    unsigned int th = 0, tw = 0;
    for (vector<text*>::iterator i = texts.begin(); i != texts.end(); ++i) {
      if (*i) {
	text &t = **i;
	text_fg_color.setRgb(t.fg_r, t.fg_g, t.fg_b, t.fg_a);
	text_bg_color.setRgb(t.bg_r, t.bg_g, t.bg_b, t.bg_a);
	QString txt(t.c_str());
	QRectF bg;
	//	QFontMetrics metrics = painter.fontMetrics();
	QRectF qr = qw->rect();
	qr.setLeft(t.pos_reset ? t.w0 : tw);
	qr.setTop( t.pos_reset ? t.h0 - 1 : th);
	// 	// resize buffer if text is out?
	// 	QRect br = painter.boundingRect(qr, Qt::AlignLeft & Qt::TextWordWrap
	// 					& Qt::AlignTop, txt);
	// 	buffer_maxh = std::max(buffer?(unsigned int)buffer->dim(0):0,
	// 			  t.h0 + br.height());
	// 	buffer_maxw = std::max(buffer?(unsigned int)buffer->dim(1):0,
	// 			  t.w0 + br.width());
	// 	buffer_resize(buffer_maxh, buffer_maxw);

	//	painter.setPen(text_fg_color);
	//	painter.setPen(Qt::NoPen);
	// painter.setBrush(text_fg_color);
	// painter.setPen(text_fg_color);
	painter.drawText(qr, Qt::AlignLeft & Qt::TextWordWrap & Qt::AlignTop,
			 txt, &bg);
	th = (uint) bg.top();
	tw = (uint) bg.right();
	bg.setTop(bg.top() + 1);
	bg.setHeight(bg.height() - 3);
	painter.setBrush(text_bg_color);
	// QPen qp(Qt::SolidLine);
	// qp.setWidth(5);
	// painter.setPen(qp);
	// painter.drawRoundedRect((int) bg.left(), (int) bg.top(),
	// 			(int) bg.width(), (int) bg.height(), 20, 20);
	painter.setPen(Qt::NoPen);
	painter.drawRect((int) bg.left(), (int) bg.top(),
			 (int) bg.width(), (int) bg.height());
	painter.setPen(text_fg_color);
	painter.drawText(qr, Qt::AlignLeft & Qt::TextWordWrap & Qt::AlignTop,
			 txt, &bg);
	// QtQStaticText st(txt);
	// painter.drawText(qr.x(), qr.y(), st);
      }
    }
  }

} // end namespace ebl
