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

#include "moc_win2d.cxx"
#include <qstyle.h>
#include <math.h>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // win2d

  win2d::win2d(uint wid, const char *wname, uint height, uint width)
    : win((QWidget*) this, wid, wname, height, width), pixmapScale(1.0) {
    pixmap = new QPixmap(1, 1);
    buffer = NULL;
    channel_buffer = NULL;
    drawing_mode = 0;
    qimage = NULL;
    clear();
    buffer_maxh = height;
    buffer_maxw = width;
    if ((height != 0) && (width != 0))
      resize_window(height, width);
    update_window();
  }

  win2d::~win2d() {
    delete pixmap;
    if (buffer) delete buffer;
    if (channel_buffer) delete channel_buffer;
    if (qimage) delete qimage;
  }

  ////////////////////////////////////////////////////////////////
  // clear methods

  void win2d::clear() {
    buffer_fill(buffer);
    // if (pixmap)
    //   pixmap->fill(bg_color);
    // clear regular lists if we can update, temporary otherwise.
    win::clear_all(!wupdate);
    update_window();
  }

  void win2d::clear_resize() {
    buffer_resize(1, 1, true);
    buffer_fill(buffer);
    // if (pixmap)
    //   pixmap->fill(bg_color);
    // clear regular lists if we can update, temporary otherwise.
    win::clear_all(!wupdate);
    update_window();
  }

  void win2d::resize_window(uint h, uint w, bool force) {
    if ((h != 0) && (w != 0)) {
      buffer_resize(h, w, force);
    }
  }

  ////////////////////////////////////////////////////////////////
  // update methods

  void win2d::buffer_resize(uint h, uint w, bool force) {
    // forbid buffer resizing if frozen
    if (frozen_size)
      return ;
    // arbitrary bounding of h and w to prevent gigantic erroneous values.
    uint bound = WIN_MAX_SIZE;
    if (std::max(h, w) > bound) {
      eblerror("trying to resize display buffer to " << h << "x" << w
	       << ", one of those dimensions is greater than " << bound
	       << " and probably erroneous");
    }
    if ((!buffer || (((uint)buffer->dim(0)) < h) ||
	 (((uint)buffer->dim(1)) < w) || force) && ((h != 0) && (w != 0))) {
      if (!buffer) {
	buffer = new idx<ubyte>(h, w, 3);
	buffer_fill(buffer);
      }
      else {
	if (!wupdate)
	  buffer->resize(h, w, 3);
	else {
	  // TODO: we should be able to avoid using this block at all
	  // with use of enable/disable udpates, but does not seem to work,
	  // investigate without this block.
	  idx<ubyte> *inew = new idx<ubyte>(h, w, 3);
	  buffer_fill(inew);
	  idx<ubyte> tmpnew = inew->narrow(0, MIN(h, (uint)buffer->dim(0)), 0);
	  tmpnew = tmpnew.narrow(1, MIN(w, (uint)buffer->dim(1)), 0);
	  idx<ubyte> tmpbuf = buffer->narrow(0, MIN(h, (uint)buffer->dim(0)),0);
	  tmpbuf = tmpbuf.narrow(1, MIN(w, (uint)buffer->dim(1)), 0);
	  idx_copy(tmpbuf, tmpnew);
	  delete buffer;
	  buffer = inew;
	}
      }
    }
  }

  void win2d::update_qimage(uint mode) {
    if (buffer) {
      if (qimage) delete qimage;
      switch (mode) {
      case 0: // RGB
	qimage = new QImage((unsigned char*) buffer->idx_ptr(),
			    buffer->dim(1), buffer->dim(0),
			    buffer->dim(1) * buffer->dim(2) *
			    sizeof (unsigned char),
			    QImage::Format_RGB888);
	break ;
      default: // R, G or B
	if (mode > 3) eblerror("unknown mode " << mode);
	// resize/allocate buffer
	if (channel_buffer &&
	    (channel_buffer->dim(0) != buffer->dim(0) ||
	     channel_buffer->dim(1) != buffer->dim(1)))
	  channel_buffer->resize(buffer->dim(0), buffer->dim(1), 1);
	if (!channel_buffer)
	  channel_buffer = new idx<ubyte>(buffer->dim(0), buffer->dim(1), 1);
	// copy channel into contiguous buffer
	idx<ubyte> slice = buffer->select(2, mode - 1);
	idx_copy(slice, *channel_buffer);
	// create qimage
	qimage = new QImage((unsigned char*) channel_buffer->idx_ptr(),
			    channel_buffer->dim(1), channel_buffer->dim(0),
			    channel_buffer->dim(1) * channel_buffer->dim(2) *
			    sizeof (unsigned char),
			    QImage::Format_Indexed8);
      }
      qimage->setColorTable(colorTable);
    }
  }

  void win2d::buffer_fill(idx<ubyte> *buf) {
    if (buf) {
      idx<ubyte> tmp = buf->select(2, 0);
      idx_fill(tmp, (ubyte) bg_color.red());
      tmp = buf->select(2, 1);
      idx_fill(tmp, (ubyte) bg_color.green());
      tmp = buf->select(2, 2);
      idx_fill(tmp, (ubyte) bg_color.blue());
    }
  }

  void win2d::update_pixmap(idx<ubyte> *img, unsigned int h0,
			     unsigned int w0, bool updatepix) {
    if (img) {
      update_pixmap(*img, h0, w0, updatepix);
      delete img;
    }
  }

  void win2d::update_pixmap(idx<ubyte> &img, uint h0, uint w0,
			     bool updatepix) {
    unsigned int h = std::max(buffer?(uint)buffer->dim(0):0,
			      (uint) (h0 + img.dim(0)));
    unsigned int w = std::max(buffer?(uint)buffer->dim(1):0,
			      (uint) (w0 + img.dim(1)));
    if (wupdate) {
      if (!buffer)
	buffer_resize(h, w);
      else if ((h > (unsigned int) buffer->dim(0)) ||
	       (w > (unsigned int) buffer->dim(1)))
	buffer_resize(h, w);
      if (h0 < buffer->dim(0) && w0 < buffer->dim(1)) {
	uint height = std::min((uint) buffer->dim(0) - h0, (uint) img.dim(0));
	uint width = std::min((uint) buffer->dim(1) - w0, (uint) img.dim(1));
	idx<ubyte> tmpbuf = buffer->narrow(0, height, h0);
	tmpbuf = tmpbuf.narrow(1, width, w0);
	idx<ubyte> tmpimg = img.narrow(0, height, 0);
	tmpimg = tmpimg.narrow(1, width, 0);
	// RGB input, simple copy
	if ((img.order() == 3) && (img.dim(2) == 3))
	  idx_copy(tmpimg, tmpbuf);
	// Grayscale input, replicate over RGB channels
	else if ((img.order() == 2) ||
		 ((img.order() == 3) && (img.dim(2) == 1))) {
	  idx<ubyte> tmpbufl = tmpbuf.select(2, 0);
	  idx_copy(tmpimg, tmpbufl);
	  tmpbufl = tmpbuf.select(2, 1);
	  idx_copy(tmpimg, tmpbufl);
	  tmpbufl = tmpbuf.select(2, 2);
	  idx_copy(tmpimg, tmpbufl);
	}
	else {
	  cerr << "unknown image dimensions: " << img << endl;
	  eblerror("expected a grayscale or rgb image");
	}
      }
      // and ready to be displayed
      // copy buffer to pixmap
      if (updatepix) {
	update_qimage(drawing_mode);
	*pixmap = QPixmap::fromImage(*qimage);
	update_window();
      }
    }
    else { // don't add the image if wupdate is false
      // instead keep it in a list of images to be displayed later
      add_image(img, h0, w0);
      // and remember the maximum size of the display buffer
      buffer_maxh = std::max(buffer_maxh, h);
      buffer_maxw = std::max(buffer_maxw, w);
    }
  }

//   void win2d::set_wupdate(bool ud) {
//     if (ud) {
//       if (wupdate_ndisable > 0) { // update only when necessary
// 	wupdate_ndisable = std::max(0, wupdate_ndisable - 1); // decrement counter
// 	if (wupdate_ndisable == 0) {
// 	  setUpdatesEnabled(true);
// 	  draw_images();
// 	  repaint();
// 	  update_window();
// 	}
//       }
//     }
//     else {
//       setUpdatesEnabled(false);
//       wupdate_ndisable++; // increment disables
//     }
//     wupdate = ud;
//   }

  void win2d::set_wupdate(bool ud) {
    if (wupdate != ud) {
      wupdate = ud;
      if (wupdate) {
	// swap temporary objects to objects to be displayed.
	swap();
	setUpdatesEnabled(true);
	draw_images(false);
	update_window();
      }
      else {
	//	setUpdatesEnabled(false);
      }
    }
  }

  void win2d::swap() {
    // swap text
    clear_text(false);
    for (vector<text*>::iterator i = texts_tmp.begin();
	 i != texts_tmp.end(); ++i)
      texts.push_back(*i);
    texts_tmp.clear();
    // swap arrows
    clear_arrows(false);
    for (vector<arrow*>::iterator i = arrows_tmp.begin();
	 i != arrows_tmp.end(); ++i)
      arrows.push_back(*i);
    arrows_tmp.clear();
    // swap boxes
    clear_boxes(false);
    for (vector<box*>::iterator i = boxes_tmp.begin();
	 i != boxes_tmp.end(); ++i)
      boxes.push_back(*i);
    boxes_tmp.clear();
    // swap crosses
    clear_crosses(false);
    for (vector<cross*>::iterator i = crosses_tmp.begin();
	 i != crosses_tmp.end(); ++i)
      crosses.push_back(*i);
    crosses_tmp.clear();
    // swap ellipses
    clear_ellipses(false);
    for (vector<box*>::iterator i = ellipses_tmp.begin();
	 i != ellipses_tmp.end(); ++i)
      ellipses.push_back(*i);
    ellipses_tmp.clear();
    // swap images
    clear_images(false);
    for (vector<image*>::iterator i = images_tmp.begin();
	 i != images_tmp.end(); ++i)
      images.push_back(*i);
    images_tmp.clear();
    // swap masks
    clear_masks(false);
    for (vector<imask*>::iterator i = masks_tmp.begin();
	 i != masks_tmp.end(); ++i)
      masks.push_back(*i);
    masks_tmp.clear();
  }

  void win2d::update_window(bool activate) {
    if (wupdate) {
      if (buffer && (width() != buffer->dim(1) ||
		     height() != buffer->dim(0)))
	resize(buffer->dim(1), buffer->dim(0));
      else QWidget::update(); //repaint();
      // saving pixmap if silent or show it otherwise
      if (silent) save(savefname);
      else {
	if (!isVisible()) QWidget::show();
	if (activate) QWidget::activateWindow();
      }
    }
  }

  void win2d::repaint_pixmap() {
    update_qimage(drawing_mode);
    if (qimage) *pixmap = QPixmap::fromImage(*qimage);
    update_window();
  }

  void win2d::set_bg_colors(ubyte r, ubyte g, ubyte b) {
    if (frozen_style)
      return ;
    bg_color.setRed(r);
    bg_color.setGreen(g);
    bg_color.setBlue(b);
    buffer_fill(buffer);
  }

  ////////////////////////////////////////////////////////////////
  // painting/drawing methods

  void win2d::paintEvent(QPaintEvent * /* event */) {
    QStylePainter painter(this);

    if (font_size > 0) {
      const QFont &font0 = painter.font();
      QFont font(font0.defaultFamily(), font_size);
      painter.setFont(font);
    }
    painter.fillRect(rect(), bg_color);
    double scaleFactor = pixmapScale / curScale;
    painter.save();
    //QTransform wt = painter.worldTransform();
    //    wt.translate(-pixmapOffset.x(), -pixmapOffset.y());
    //    wt.translate(pixmapOffset.x() - 500, pixmapOffset.y() - 500);
    // wt.translate(- 500, - 500);
    // painter.setWorldTransform(wt);
    // QPoint p = pixmapOffset;
    // p.setX(p.x() - 500);
    // p.setY(p.y() - 500);
    painter.translate(pixmapOffset);
    // painter.translate(p);
    painter.scale(scaleFactor, scaleFactor);
    QRectF exposed =
      painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
    // painter.matrix().inverted().mapRect(rect());
    // cout << "exposed: "<< exposed.x() << "," << exposed.y()
    // 	 << " " << exposed.height() << "x"  << exposed.width() << endl;
    // cout << "rect: "<< rect().x() << "," << rect().y()
    // 	 << " " << rect().height() << "x"  << rect().width() << endl;
    // cout << "hasclipping: " << painter.hasClipping() << endl;
    // exposed.setX(-500);
    // exposed.setY(-500);
    // QRectF exposed2 =
    //   painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
    // exposed2.setX(500);
    // exposed2.setY(500);
    // exposed.setHeight(2500);
    // exposed.setWidth(2500);
    // painter.setMatrix(exposed.inverted());
    // painter.setClipRect(exposed2);

    // cout << "rect: "<< rect().x() << "," << rect().y()
    // 	 << " " << rect().height() << "x"  << rect().width() << endl;

    painter.drawPixmap(exposed, *pixmap, exposed);
    if (!images_only) {
      draw_masks(painter);
      draw_boxes(painter);
      draw_arrows(painter);
      draw_crosses(painter);
      draw_ellipses(painter);
    }

    win::paint(painter, scaleFactor);



    // wt.translate(pixmapOffset.x() , pixmapOffset.y() );
    // painter.setWorldTransform(wt);

    painter.restore();
//     if (!silent) {
//       QString txt = tr("Use mouse wheel to zoom, left click to drag.");
//       QFontMetrics metrics = painter.fontMetrics();
//       int textWidth = metrics.width(txt);
//       painter.setPen(Qt::NoPen);
//       painter.setBrush(QColor(0, 0, 0, 127));
//       painter.drawRect((width() - textWidth) / 2 - 5, height() - 15,
// 		       textWidth + 10,
// 		       metrics.lineSpacing() + 5);
//       painter.setPen(Qt::white);
//       painter.drawText((width() - textWidth) / 2,
// 		       height() - 15 + metrics.leading() + metrics.ascent(),
// 		       txt);
//     }
  }

  void win2d::draw_arrows(QPainter &painter) {
    int len_factor = 1;
    int ax1, ay1, ax2, ay2;
    painter.setBrush(QColor(255, 255, 255, 127));
    painter.setPen(Qt::red);
    for (vector<arrow*>::iterator i = arrows.begin(); i != arrows.end(); ++i) {
      if (*i) {

//     QStyleOption opt;
//     opt.rect.setWidth(100);
//     opt.rect.setHeight(150);
//     QPixmap qp = painter.standardPixmap(QStyle::SP_ArrowUp, &opt);
//     painter.drawPixmap(exposed, qp, exposed);
//     painter.drawPrimitive(QStyle::PE_IndicatorSpinUp, opt);

	ax1 = (*i)->h1;
	ay1 = (*i)->w1;
	ax2 = (*i)->h2;
	ay2 = (*i)->w2;
	double angle = atan2( (double) ay1 - ay2, (double) ax1 - ax2);
	double hypotenuse = sqrt( pow((float)ay1 - ay2, 2)
				  + pow((float)ax1 - ax2, 2));
	// draw body
	ax2 = (int) (ax1 - len_factor * hypotenuse * cos(angle));
	ay2 = (int) (ay1 - len_factor * hypotenuse * sin(angle));
	painter.drawLine(ax1, ay1, ax2, ay2);
	// draw head 1
	if ((*i)->head1) {
	  ax1 = (int) (ax2 + 9 * cos(angle + M_PI / 4));
	  ay1 = (int) (ay2 + 9 * sin(angle + M_PI / 4));
	  painter.drawLine(ax1, ay1, ax2, ay2);
	  ax1 = (int) (ax2 + 9 * cos(angle - M_PI / 4));
	  ay1 = (int) (ay2 + 9 * sin(angle - M_PI / 4));
	  painter.drawLine(ax1, ay1, ax2, ay2);
	}
      }
    }
    painter.setPen(Qt::black);
  }

  // void win2d::draw_boxes(QPainter &painter) {
  //   painter.setBrush(Qt::NoBrush);
  //   for (vector<box*>::iterator i = boxes.begin(); i != boxes.end(); ++i) {
  //     if (*i) {
  // 	box &b = **i;
  // 	// set color
  // 	QPen qp(QColor(b.r, b.g, b.b, b.a));
  // 	qp.setWidth(3);
  // 	painter.setPen(qp);
  // 	// draw box
  // 	uint sz = (uint) (b.h * .1);
  // 	painter.drawLine(b.w0, b.h0, b.w0 + b.w, b.h0);
  // 	painter.drawLine(b.w0, b.h0 + b.h, b.w0 + b.w, b.h0 + b.h);
  // 	painter.drawLine(b.w0, b.h0, b.w0, b.h0 + sz);
  // 	painter.drawLine(b.w0 + b.w, b.h0, b.w0 + b.w, b.h0 + sz);
  // 	painter.drawLine(b.w0, b.h0 + b.h - sz, b.w0, b.h0 + b.h);
  // 	painter.drawLine(b.w0 + b.w, b.h0 + b.h - sz, b.w0 + b.w, b.h0 + b.h);
  //     }
  //   }
  //   painter.setPen(Qt::black);
  // }

  void win2d::draw_boxes(QPainter &painter) {
    painter.setBrush(Qt::NoBrush);
    for (vector<box*>::iterator i = boxes.begin(); i != boxes.end(); ++i) {
      if (*i) {
  	box &b = **i;
  	// set color
  	QPen qp(QColor(b.r, b.g, b.b, b.a));
  	qp.setWidth(1);
        //  	qp.setWidth(4);
  	painter.setPen(qp);
  	// draw box
  	QRectF r(b.w0, b.h0, b.w, b.h);
  	painter.drawRect(r);
  	//painter.drawRoundedRect(r, 5, 5);
      }
    }
    painter.setPen(Qt::black);
  }

  void win2d::draw_crosses(QPainter &painter) {
    painter.setBrush(QColor(255, 255, 255, 127));
    for (vector<cross*>::iterator i = crosses.begin(); i != crosses.end(); ++i){
      if (*i) {
	cross &c = **i;
	painter.setPen(QColor(c.r, c.g, c.b, c.a));
	float half = c.length / 2;
	QLineF f1(c.h0 - half, c.w0 - half, c.h0 + half, c.w0 + half);
	QLineF f2(c.h0 - half, c.w0 + half, c.h0 + half, c.w0 - half);
	painter.drawLine(f1);
	painter.drawLine(f2);
      }
    }
    painter.setPen(Qt::black);
  }

  void win2d::draw_ellipses(QPainter &painter) {
    painter.setBrush(Qt::NoBrush);
    for (vector<box*>::iterator i = ellipses.begin(); i != ellipses.end(); ++i){
      if (*i) {
	box &b = **i;
	// set color
	painter.setPen(QColor(b.r, b.g, b.b, b.a));
	QPointF p(b.h0, b.w0);
	// draw ellipse
	painter.drawEllipse(p, b.h, b.w);
      }
    }
    painter.setPen(Qt::black);
  }

  void win2d::draw_images(bool update) {
    buffer_resize(buffer_maxh, buffer_maxw); // resize to maximum size first
    // then display all images not displayed
    for (vector<image*>::iterator i = images.begin(); i != images.end(); ++i)
      if (*i) {
	update_pixmap((*i)->img, (*i)->h0, (*i)->w0, false);
	delete (*i);
      }
    // now update pixmap
    update_qimage(drawing_mode);
    if (qimage) {
      *pixmap = QPixmap::fromImage(*qimage);
      if (update)
	update_window();
      // clear images
      images.clear();
    }
  }

  void win2d::draw_masks(QPainter &painter) {
    if (masks.size() > 0)
      buffer_resize(buffer_maxh, buffer_maxw); // resize to maximum size first
    // then display all images not displayed
    for (vector<imask*>::iterator i = masks.begin(); i != masks.end(); ++i)
      if (*i) {
	painter.drawPixmap((*i)->w0, (*i)->h0, (*i)->map);
      }
  }

  ////////////////////////////////////////////////////////////////
  // events methods

  void win2d::wheelEvent(QWheelEvent *event) {
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
    update();
  }

  void win2d::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
      lastDragPos = event->pos();
      QCursor qc(Qt::SizeAllCursor);
      setCursor(qc);
    }
  }

  void win2d::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
      pixmapOffset += event->pos() - lastDragPos;
      lastDragPos = event->pos();
      update();
    }
  }

  void win2d::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
      pixmapOffset += event->pos() - lastDragPos;
      lastDragPos = QPoint();

      int deltaX = (width() - pixmap->width()) / 2 - pixmapOffset.x();
      int deltaY = (height() - pixmap->height()) / 2 - pixmapOffset.y();
      scroll(deltaX, deltaY);
      QCursor qc(Qt::ArrowCursor);
      setCursor(qc);
      update();
    }
  }

  void win2d::keyPressEvent(QKeyEvent *event) {
    // push key on the list
    keyspressed.push_back(event->key());
    // handle key events
    if (event->key() == Qt::Key_Escape)
      close();
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
	  // cout << "Disabling Text in window (ctrl + t)" << endl;
	  update();
	} else { // text not currently displayed
	  text_on = true;
	  // cout << "Enabling Text in window (ctrl + t)" << endl;
	  update();
	}
      }
    } else if (event->key() == Qt::Key_I) {
      if (ctrl_on) {
	if (images_only) {
	  images_only = false;
	  qw->update();
	} else {
	  images_only = true;
	  qw->update();
	}
      }
    } else if (event->key() == Qt::Key_S) {
      string fname;
      fname << "win" << id;
      save(fname, true);
      save_mat(fname, true);
    } else if (event->key() == Qt::Key_0) {
      if (ctrl_on) {
	drawing_mode = 0;
	cout << "Gui: Showing all RGB channels" << endl;
	repaint_pixmap();
      }
    } else if (event->key() == Qt::Key_1) {
      if (ctrl_on) {
	drawing_mode = 1;
	cout << "Gui: Showing only channel 0" << endl;
	repaint_pixmap();
      }
    } else if (event->key() == Qt::Key_2) {
      if (ctrl_on) {
	drawing_mode = 2;
	cout << "Gui: Showing only channel 1" << endl;
	repaint_pixmap();
      }
    } else if (event->key() == Qt::Key_3) {
      if (ctrl_on) {
	drawing_mode = 3;
	cout << "Gui: Showing only channel 2" << endl;
	repaint_pixmap();
      }
    }
  }

  void win2d::keyReleaseEvent(QKeyEvent *event) {
    // handle key events
    if (event->key() == Qt::Key_Control) {
      ctrl_on = false;
    }
  }

  void win2d::scroll_previous() {
    if (scrollbox)
      scrollbox->display_previous();
    else {
    //    if (scrollbox- != sb) {
      cerr << "sender: " << sender() << endl;
      cerr << "warning: trying to call operation on a scroll_box object ";
      cerr << "that no longer exists.";
    }
  }

  void win2d::scroll_next() {
    if (scrollbox)
      scrollbox->display_next();
    else {
      //    if (scrollbox != sb) {
      cerr << "sender: " << sender() << endl;
      cerr << "warning: trying to call operation on a scroll_box object ";
      cerr << "that no longer exists.";
    }
  }

} // end namespace ebl
