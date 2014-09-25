/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
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

#ifndef __GUI3D__
#include "win3d.h"
#else
#include "moc_win3d.cxx"

using namespace std;

namespace ebl {

  // text3d //////////////////////////////////////////////////////////////////

  text3d::text3d(const char *s_, float x_, float y_, float z_,
		 int r, int g, int b, int a)
    : s(s_), x(x_), y(y_), z(z_), col(r, g, b, a) {
    //EDEBUG("win3d added: " << describe());
  }

  text3d::~text3d() {
  }

  std::string text3d::describe() {
    std::string sd;
    sd << "text3d, string: " << s.toStdString()
       << ", center: (" << x << "," << y << "," << z
       << "), color: (" << col.red() << "," << col.green() << "," << col.blue()
       << "," << col.alpha() << ")";
    return sd;
  }

  // sphere3d //////////////////////////////////////////////////////////////////

  sphere3d::sphere3d(float x_, float y_, float z_, float rad,
		     int r, int g, int b, int a)
    : x(x_), y(y_), z(z_), radius(rad), node(NULL), col(r, g, b, a) {
    QGLBuilder builder;
    builder << QGLSphere(radius * 2);
    node = builder.finalizedSceneNode();
    //EDEBUG("win3d added: " << describe());
  }

  sphere3d::~sphere3d() {
    if (node) delete node;
  }

  std::string sphere3d::describe() {
    std::string s;
    s << "sphere3d, center: (" << x << "," << y << "," << z
      << "), radius: " << radius
      << "), color: (" << col.red() << "," << col.green() << "," << col.blue()
      << "," << col.alpha() << ")";
    return s;
  }

  // cylinder3d ////////////////////////////////////////////////////////////////

  cylinder3d::cylinder3d(float x_, float y_, float z_, float length,
			 float top_radius_, float base_radius_, float a1_,
			 float a2_, int r, int g, int b, int a,
			 bool tops_)
    : x(x_), y(y_), z(z_), a1(a1_), a2(a2_), top_radius(top_radius_),
      base_radius(base_radius_), tops(tops_), node(NULL), col(r, g, b, a) {
    QGLBuilder builder;
    builder << QGLCylinder(top_radius * 2, base_radius * 2, length,
			   10, 3, tops, tops);
    node = builder.finalizedSceneNode();
    //EDEBUG("win3d added: " << describe());
  }

  cylinder3d::~cylinder3d() {
    if (node) delete node;
  }

  std::string cylinder3d::describe() {
    std::string s;
    s << "cylinder3d, base origin: (" << x << "," << y << "," << z
      << "), degree angles: (" << a1 << "," << a2 << "), top/base radiuses: "
      << top_radius << ", " << base_radius << ", drawing top: " << tops;
    return s;
  }

  // line3d ////////////////////////////////////////////////////////////////

  line3d::line3d(float x_, float y_, float z_, float x1_, float y1_, float z1_,
		 int r, int g, int b, int a)
    : x(x_), y(y_), z(z_), x1(x1_), y1(y1_), z1(z1_), node(NULL),
      col(r, g, b, a) {
    QGLBuilder builder;
    builder.addPane(1);
    node = builder.finalizedSceneNode();
    node->setDrawingMode(QGL::Lines);
    //EDEBUG("win3d added: " << describe());
    eblerror("not implemented");
  }

  line3d::~line3d() {
  }

  std::string line3d::describe() {
    std::string s;
    s << "line3d, from: (" << x << "," << y << "," << z
      << "), to (" << x1 << "," << y1 << "," << z1 << ")";
    return s;
  }

  // void line3d::drawGeometry(QGLPainter *painter) {
  //   cout << "draw geometry $$$$$$$$$$$$$$$$$$" << endl;
  //   painter->begin();
  //   glLineWidth(1);
  //   glBegin(GL_LINES);
  //   glVertex3f(x, y, z);
  //   glVertex3f(x1, y1, z1);
  //   glEnd();
  //   // call parent implementation to do actual draw
  //   QGLSceneNode::drawGeometry(painter);
  // }

  //////////////////////////////////////////////////////////////////////////////
  // win3d

  win3d::win3d(uint wid, const char *wname, uint height, uint width)
    : QGLWidget(), win((QWidget*) this, wid, wname, height, width) {
    if ((height != 0) && (width != 0))
      resize_window(height, width);
    // setAutoFillBackground(false);
    // setAutoBufferSwap(false);
    pixmapScale = 1.0;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    rotationX = 0;//-38.0;
    rotationY = 0;//-58.0;
    rotationZ = 0.0;
    scaling = 1.0;
    update_window();
  }

  win3d::~win3d() {
    clear();
  }

  // window matters ////////////////////////////////////////////////////////////

  void win3d::show() {
    QGLWidget::show();
  }

  void win3d::set_wupdate(bool ud) {
    // if (wupdate != ud) {
    //   wupdate = ud;
    //   if (wupdate) {
    // 	cout << "updating **************************" << endl;
    // 	setUpdatesEnabled(true);
    // 	//	draw_images(false);
    // 	update_window();
    //   }
    //   else {
    // 	cout << "disabling updating **************************" << endl;
    // 	//	setUpdatesEnabled(false);
    //   }
    // }
  }

  void win3d::update_window(bool activate) {
    if (wupdate) {
      QWidget::update();
      // saving pixmap if silent or show it otherwise
      if (silent)
	save(savefname);
      else {
	if (!isVisible())
	  QWidget::show();
	if (activate) {
	  QWidget::activateWindow();
	}
      }
    }
  }

  void win3d::resize_window(uint h, uint w, bool force) {
  }

  // objects adding ////////////////////////////////////////////////////////////

  void win3d::add_sphere(float x, float y, float z, float radius,
			 const char *label, int r, int g, int b, int a) {
    spheres.push_back(new sphere3d(x, y, z, radius, r, g, b, a));
    texts.push_back(new text3d(label, x, y, z, r, g, b, a));
  }

  void win3d::add_cylinder(float x_, float y_, float z_, float length,
			   float top_radius, float base_radius, float a1_,
			   float a2_, const char *label,
			   int r, int g, int b, int a,
			   bool tops) {
    cylinders.push_back(new cylinder3d(x_, y_, z_, length, top_radius,
				       base_radius, a1_, a2_,
				       r, g, b, a, tops));
    // put text on top
    float x = length * std::cos(a1_);
    float y = length * std::cos(a2_) * std::sin(a1_);
    float z = length * std::sin(a2_) * std::sin(a1_);
    // float x = sqrt((x - x1) * (x - x1)) / 2;
    // float y = sqrt((y - y1) * (y - y1)) / 2;
    // float z = sqrt((z - z1) * (z - z1)) / 2;
    texts.push_back(new text3d(label, x, y, z, r, g, b, a));
  }

  void win3d::add_text(float x, float y, float z, const char *s,
		       int r, int g, int b, int a) {
    texts.push_back(new text3d(s, x, y, z, r, g, b, a));
  }

  void win3d::add_line(float x, float y, float z, float x1, float y1, float z1,
		       const char *s, int r, int g, int b, int a) {
    lines.push_back(new line3d(x, y, z, x1, y1, z1));
    if (s)
      texts.push_back(new text3d(s, x1, y1, z1, r, g, b, a));
  }

  // clear methods /////////////////////////////////////////////////////////////

  void win3d::clear() {
    clear_spheres();
    clear_cylinders();
    clear_texts();
    clear_lines();
    win::clear();
  }

  void win3d::clear_spheres() {
    for (vector<sphere3d*>::iterator i = spheres.begin(); i != spheres.end();
	 ++i)
      if (*i) delete *i;
    spheres.clear();
  }

  void win3d::clear_cylinders() {
    for (vector<cylinder3d*>::iterator i = cylinders.begin();
	 i != cylinders.end(); ++i)
      if (*i) delete *i;
    cylinders.clear();
  }

  void win3d::clear_texts() {
    for (vector<text3d*>::iterator i = texts.begin(); i != texts.end(); ++i)
      if (*i) delete *i;
    texts.clear();
  }

  void win3d::clear_lines() {
    for (vector<line3d*>::iterator i = lines.begin(); i != lines.end(); ++i)
      if (*i) delete *i;
    lines.clear();
  }

  ////////////////////////////////////////////////////////////////
  // painting/drawing methods

  void win3d::initializeGL() {
    QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
    QGLPainter painter(this);
    painter.setStandardEffect(QGL::LitMaterial);

    glEnable(GL_BLEND);
    // setFormat(QGLFormat(QGL::SampleBuffers)); // enable anti-aliasing support

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // glShadeModel( GL_SMOOTH );
    // glDepthFunc( GL_LESS );
    // glEnable( GL_COLOR_SUM_EXT );

    // GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    // GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // GLfloat mat_shininess[] = { 30.0 };

    // glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    // glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    // glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    // glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Used to display semi-transparent relection rectangle
    //  glBlendFunc(GL_ONE, GL_ONE);
    // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // glEnable( GL_NORMALIZE );
    // glEnable( GL_LIGHTING );

    // GLfloat ambientLight[] = { 0.4, 0.4, 0.4, 1.0 };
    // GLfloat diffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };
    // GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };
    // GLfloat position[] = { 0.8, 0.7, 1.0, 0.0 };

    // glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL_EXT,
    //     GL_SEPARATE_SPECULAR_COLOR_EXT );
    // glLightfv( GL_LIGHT0, GL_AMBIENT, ambientLight );
    // glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight );
    // glLightfv( GL_LIGHT0, GL_SPECULAR, specularLight );
    // glLightfv( GL_LIGHT0, GL_POSITION, position );
    // glEnable( GL_LIGHT0 );

    //  glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    // glEnableClientState( GL_VERTEX_ARRAY );
    // glEnableClientState( GL_NORMAL_ARRAY );
  }

  void win3d::resizeGL(int w, int h) {
    height = h;
    width = w;
    glViewport(0, 0, width, height);
  }

  void win3d::wheelEvent(QWheelEvent *event) {
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

 void win3d::mousePressEvent(QMouseEvent *event) {
     lastPos = event->pos();
 }

 void win3d::mouseMoveEvent(QMouseEvent *event)
 {
     int dx = event->x() - lastPos.x();
     int dy = event->y() - lastPos.y();

     if (event->buttons() & Qt::LeftButton) {
         setXRotation(xRot + 8 * dy);
         setYRotation(yRot + 8 * dx);
     } else if (event->buttons() & Qt::RightButton) {
         setXRotation(xRot + 8 * dy);
         setZRotation(zRot + 8 * dx);
     }
     lastPos = event->pos();
     repaint();
 }

  void win3d::keyPressEvent(QKeyEvent *event) {
    // push key on the list
    keyspressed.push_back(event->key());
    // // handle key events
    // if (event->key() == Qt::Key_Escape)
    //   close();
    // else if (event->key() == Qt::Key_Left) {
    //   if (scrollbox) {
    // 	cout << "display_previous" << endl;
    // 	scrollbox->display_previous();
    //   }
    // } else if (event->key() == Qt::Key_Right) {
    //   if (scrollbox) {
    // 	cout << "display_next" << endl;
    // 	scrollbox->display_next();
    //   }
    // } else if (event->key() == Qt::Key_Control) {
    //   ctrl_on = true;
    // } else if (event->key() == Qt::Key_T) {
    //   if (ctrl_on) {
    // 	if (text_on) { // text is currently displayed
    // 	  text_on = false;
    // 	  // cout << "Disabling Text in window (ctrl + t)" << endl;
    // 	  update();
    // 	} else { // text not currently displayed
    // 	  text_on = true;
    // 	  // cout << "Enabling Text in window (ctrl + t)" << endl;
    // 	  update();
    // 	}
    //   }
    // } else if (event->key() == Qt::Key_S) {
    //   string fname;
    //   fname << "win" << id;
    //   save(fname, true);
    //   save_mat(fname, true);
    // }
  }

  void win3d::keyReleaseEvent(QKeyEvent *event) {
    // handle key events
    if (event->key() == Qt::Key_Control) {
      ctrl_on = false;
    }
  }

 static void qNormalizeAngle(int &angle)
 {
     while (angle < 0)
         angle += 360 * 16;
     while (angle > 360 * 16)
         angle -= 360 * 16;
 }

 void win3d::setXRotation(int angle)
 {
     qNormalizeAngle(angle);
     if (angle != xRot)
         xRot = angle;
 }


 void win3d::setYRotation(int angle)
 {
     qNormalizeAngle(angle);
     if (angle != yRot)
         yRot = angle;
 }

 void win3d::setZRotation(int angle)
 {
     qNormalizeAngle(angle);
     if (angle != zRot)
         zRot = angle;
 }

// void win3d::drawLegend(QPainter &painter)
// {
//   //    QPainter painter(this);
//     const int Margin = 11;
//     const int Padding = 6;

//     QTextDocument textDocument;
//     textDocument.setDefaultStyleSheet("* { color: #FFEFEF }");
//     textDocument.setHtml("<h4 align=\"center\">Vowel Categories</h4>"
//                          "<p align=\"center\"><table width=\"100%\">"
//                          "<tr><td>Open:<td>a<td>e<td>o<td>&ouml"
//                          "</table>");
//     textDocument.setTextWidth(textDocument.size().width());

//     QRect rect(QPoint(0, 0), textDocument.size().toSize()
//                              + QSize(2 * Padding, 2 * Padding));
//     painter.translate(width() - rect.width() - Margin,
//                        height() - rect.height() - Margin);
//     painter.setPen(QColor(255, 239, 239));
//     painter.setBrush(QColor(255, 0, 0, 31));
//     painter.drawRect(rect);

//     painter.translate(Padding, Padding);
//     textDocument.drawContents(&painter);
// }

  void win3d::paintGL() {
    QPainter painter(this);
    QGLPainter p(this);
    // clear
    qglClearColor(QColor(0,0,0));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 3D painting
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    paint_spheres(&p);
    paint_cylinders(&p);
    paint_lines(&p);

    // finishing 3d painting
    p.disableEffect(); // text painting crashes without this
    p.end();

    // 3D text painting
    glEnable(GL_DEPTH_TEST);
    paint_text();

    // 2D painting
    glDisable(GL_CULL_FACE);
    //    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    draw_text(painter);
  }

  void win3d::paint_text() {
    for (vector<text3d*>::iterator i = texts.begin(); i != texts.end();
  	 ++i) {
      float xfact = width / (float) 2;
      float yfact = height / (float) 2;
      float xfact1 = xfact / pixmapScale;
      float yfact1 = yfact / pixmapScale;
      text3d *t = *i;
      if (t) {
  	// remember current matrix
	glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
	// scale
	glScalef(pixmapScale, pixmapScale, pixmapScale);
        // translate
	glTranslatef(xfact1, yfact1, 1);
	//glScalef(pixmapScale, pixmapScale, 0);
	glRotatef(xRot  /16, 1.0, 0.0, 0.0);
	glRotatef(-yRot /16, 0.0, 1.0, 0.0);
	glRotatef(zRot  /16, 0.0, 0.0, 1.0);
  	// set color
  	qglColor(t->col);
  	// draw
  	renderText(t->x * xfact, -t->y * yfact, -t->z * xfact, t->s);
  	// put matrix back
        glPopMatrix();
      }
    }
  }

  void win3d::paint_spheres(QGLPainter *painter) {
    for (vector<sphere3d*>::iterator i = spheres.begin(); i != spheres.end();
  	 ++i) {
      sphere3d *s = *i;
      if (s) {
  	// remember current matrix
  	painter->modelViewMatrix().push();
	// scale
	painter->modelViewMatrix().scale(pixmapScale, pixmapScale, pixmapScale);
	// rotate
	painter->modelViewMatrix().rotate(xRot / 16.0, 1.0, 0.0, 0.0);
	painter->modelViewMatrix().rotate(yRot / 16.0, 0.0, 1.0, 0.0);
	painter->modelViewMatrix().rotate(zRot / 16.0, 0.0, 0.0, 1.0);
        // translate
  	painter->modelViewMatrix().translate(s->x, s->y, s->z);
  	// set color
  	painter->setFaceColor(QGL::AllFaces, s->col);
  	// draw
  	s->node->draw(painter);
  	// put matrix back
  	painter->modelViewMatrix().pop();
      }
    }
  }

  void win3d::paint_cylinders(QGLPainter *painter) {
    for (vector<cylinder3d*>::iterator i = cylinders.begin();
  	 i != cylinders.end(); ++i) {
      cylinder3d *s = *i;
      if (s) {
  	// remember current matrix
  	painter->modelViewMatrix().push();
	// scale
	painter->modelViewMatrix().scale(pixmapScale, pixmapScale, pixmapScale);
	// rotate
	painter->modelViewMatrix().rotate(xRot / 16.0, 1.0, 0.0, 0.0);
	painter->modelViewMatrix().rotate(yRot / 16.0, 0.0, 1.0, 0.0);
	painter->modelViewMatrix().rotate(zRot / 16.0, 0.0, 0.0, 1.0);
  	// now translate
  	painter->modelViewMatrix().translate(s->x, s->y, s->z);
	// first rotate around each axis
  	painter->modelViewMatrix().rotate(s->a2 + 90, 1, 0, 0);
  	painter->modelViewMatrix().rotate(s->a1 + 90, 0, 1, 0);
  	// set color
  	painter->setFaceColor(QGL::AllFaces, s->col);
  	// draw
  	s->node->draw(painter);
  	// put matrix back
  	painter->modelViewMatrix().pop();
      }
    }
  }

  void win3d::paint_lines(QGLPainter *painter) {
    for (vector<line3d*>::iterator i = lines.begin();
  	 i != lines.end(); ++i) {
      line3d *s = *i;
      if (s) {
  	// remember current matrix
  	painter->modelViewMatrix().push();
	// scale
	painter->modelViewMatrix().scale(pixmapScale, pixmapScale, pixmapScale);
	// rotate
	painter->modelViewMatrix().rotate(xRot / 16.0, 1.0, 0.0, 0.0);
	painter->modelViewMatrix().rotate(yRot / 16.0, 0.0, 1.0, 0.0);
	painter->modelViewMatrix().rotate(zRot / 16.0, 0.0, 0.0, 1.0);
  	// // now translate
  	// painter->modelViewMatrix().translate(s->x, s->y, s->z);
  	// set color
  	painter->setFaceColor(QGL::AllFaces, s->col);
  	// draw
  	s->node->draw(painter);
	//  	s->draw(painter);
	cout << "painting line !!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
  	// put matrix back
  	painter->modelViewMatrix().pop();
      }
    }
  }

} // namespace ebl {

#endif /* __GUI3D__ */
