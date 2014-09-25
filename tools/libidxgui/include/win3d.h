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

#ifndef _WIN3D_H_
#define _WIN3D_H_

#include <QPixmap>
#include <QColor>
#include <QWidget>
#include <QtGui>
#include <QResizeEvent>
#include <math.h>
#include <iostream>

#include "defines_idxgui.h"
#include "libidx.h"
#include "scroll_box0.h"
#include "defines.h"
#include "win.h"

#ifdef __GUI3D__
#include "qgl.h"
#include "qglpainter.h"
#include "qglbuilder.h"
#include "qglcamera.h"
#include "qgltexture2d.h"
#include "qglview.h"
#include "qglsphere.h"
#include "qglcylinder.h"
#include "qglcolormaterial.h"
#include "qglbuilder.h"
#endif

using namespace std;

namespace ebl {

#ifdef __GUI3D__
  
  class text3d {
  public:
    //! Construct a text 's' at position (x,y,z).
    text3d(const char *s, float x, float y, float z,
	   int r = 255, int g = 255, int b = 255, int a = 255);
    virtual ~text3d();
    //! Returns a description of this cylinder.
    std::string describe();
    // members
    QString s;
    float x, y, z;
    QColor col;
  };
  
  class sphere3d {
  public:
    //! Construct a sphere at position (x,y,z) with 'radius' and
    //! color (r,g,b,a).
    sphere3d(float x, float y, float z, float radius,
	     int r = 255, int g = 255, int b = 255, int a = 255);
    virtual ~sphere3d();
    //! Returns a description of this cylinder.
    std::string describe();
    // members
    float x, y, z, radius;
    QGLSceneNode *node;
    QColor col;
  };
  
  class cylinder3d {
  public:
    //! Construct a cylinder which base is centered at (x,y,z), with length,
    //! with radiuses 'top_radius' and 'base_radius', with degree angles
    //! 'ax', 'ay', 'az' and color (r,g,b,a).
    //! \param tops If true, draw closing caps on each end of cylinder.
    cylinder3d(float x, float y, float z, float length, float top_radius,
	       float base_radius, float a1, float a2, int r = 255,
	       int g = 255, int b = 255, int a = 255, bool tops = false);
    virtual ~cylinder3d();
    //! Returns a description of this cylinder.
    std::string describe();
    // members
    float x, y, z; //!< Center 1.
    float a1, a2; //!< Angles in degrees.
    float top_radius, base_radius;
    bool tops;
    QGLSceneNode *node;
    QColor col;
  };
  
  class line3d { //: public QGLSceneNode {
  public:
    //! Construct a cylinder which base is centered at (x,y,z), with length,
    //! with radiuses 'top_radius' and 'base_radius', with degree angles
    //! 'ax', 'ay', 'az' and color (r,g,b,a).
    //! \param tops If true, draw closing caps on each end of cylinder.
    line3d(float x, float y, float z, float x1, float y1, float z1,
	   int r = 255, int g = 255, int b = 255, int a = 255);
    virtual ~line3d();
    //! Returns a description of this line.
    std::string describe();
  /* protected: */
  /*   virtual void drawGeometry(QGLPainter *painter); */
    // members
  public:
    float x, y, z; //!< Center 1.
    float x1, y1, z1; //!< Center 1.
    QGLSceneNode *node;
    QColor col;
  };
  
  ////////////////////////////////////////////////////////////////
  // win3d
  
  class IDXGUIEXPORT win3d : public QGLWidget, public win {
    Q_OBJECT
      
  public:
    // constructors ////////////////////////////////////////////////////////////
    
    win3d(uint wid, const char *wname = NULL, uint height = 0, uint width = 0);
    virtual ~win3d();

    // window matters //////////////////////////////////////////////////////////
    
    virtual void show();
    //! Used to disable or enable updating of window, for batch displaying.
    //! This is useful to avoid flickering and speed up display.
    virtual void set_wupdate(bool ud);
    //! Resize this window to hxw.
    //! \param force If true, will resize to any size, otherwise, will not
    //!   accept to resize smaller than current size.
    virtual void resize_window(uint h, uint w, bool force = false);
    //! Refresh display of the window.
    //! \param activate Raises the focus to this window if true.
    virtual void update_window(bool activate = false);

    // objects adding //////////////////////////////////////////////////////////
    
    //! Add a sphere at position (x,y,z) with 'radius' and color (r,g,b,a).
    void add_sphere(float x, float y, float z, float radius,
		    const char *label = NULL,
		    int r = 255, int g = 255, int b = 255, int a = 255);
    //! Add a cylinder which base is centered at (x,y,z), with length 'length',
    //! with radiuses 'top_radius' and 'base_radius', with degree angles
    //! 'ax', 'ay' and color (r,g,b,a).
    //! \param tops If true, draw closing caps on each end of cylinder.
    void add_cylinder(float x, float y, float z, float length, float top_radius,
		      float base_radius, float a1, float a2,
		      const char *label = NULL,
		      int r = 255, int g = 255, int b = 255, int a = 255,
		      bool tops = false); 
    //! Draw 3d text 's' at (x,y,z) with color (r,g,b,a).    
    void add_text(float x, float y, float z, const char *s,
		  int r = 255, int g = 255, int b = 255, int a = 255);
    //! Draw 3d text 's' at (x,y,z) with color (r,g,b,a).    
    void add_line(float x, float y, float z, float x1, float y1, float z1,
		   const char *s, int r = 255, int g = 255, int b = 255,
		   int a = 255);

    // clear methods ///////////////////////////////////////////////////////////

    //! Clears the window, but does not resize it, leaves it to the current
    //! size.
    virtual void clear();
    void clear_spheres();
    void clear_cylinders();
    void clear_texts();
    void clear_lines();

    ////////////////////////////////////////////////////////////////
    // event methods
  protected:
    //! 3D initalizations.
    virtual void initializeGL();
    //! Resize window.
    void resizeGL(int width, int height);
    //! The event triggered when painting is needed.
    virtual void paintGL();
    //! Paint text.
    void paint_text();
    //! Paint spheres.
    void paint_spheres(QGLPainter *painter);
    //! Paint cylinders.
    void paint_cylinders(QGLPainter *painter);
    //! Paint cylinders.
    void paint_lines(QGLPainter *painter);

  protected:
    ////////////////////////////////////////////////////////////////
    // event methods

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    //    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    /* void setupViewport(int width, int height); */
    //       void paintEvent(QPaintEvent *);

    GLfloat rotationX;
    GLfloat rotationY;
    GLfloat rotationZ;
    GLfloat scaling;
    QPoint lastPos;

    //  QSize sizeHint() const;
     int xRotation() const { return xRot; }
     int yRotation() const { return yRot; }
     int zRotation() const { return zRot; }

 int xRot;
     int yRot;
     int zRot;
    double		 pixmapScale;

    int height; //!< Window height.
    int width; //!< Window width.
     
     
 public slots:
     void setXRotation(int angle);
     void setYRotation(int angle);
     void setZRotation(int angle);

    
  private:
    vector<text3d*> texts;
    vector<sphere3d*> spheres;
    vector<cylinder3d*> cylinders;
    vector<line3d*> lines;
  };

#else // No 3D available, just define a dummy class

  class IDXGUIEXPORT win3d : public win {
  public:
    win3d(uint wid, const char *wname = NULL, uint height = 0, uint width = 0) {
      eblerror("install Qt3d required"); }
  };
  
#endif

} // namespace ebl {

#endif /* _WIN3D_H_ */
