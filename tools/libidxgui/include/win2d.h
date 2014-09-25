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

#ifndef WIN2D_H_
#define WIN2D_H_

#include <QPixmap>
#include <QWidget>
#include <QtGui>
#include <QResizeEvent>
#include <math.h>
#include <iostream>

#include "defines_idxgui.h"
#include "libidx.h"
#include "defines.h"
#include "win.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // win2d

  class IDXGUIEXPORT win2d : public QWidget, public win { 
    Q_OBJECT
      
  public:
    win2d(uint wid, const char *wname = NULL, uint height = 1, uint width = 1);
    virtual ~win2d();

    ////////////////////////////////////////////////////////////////
    // overloaded methods from win
    
    //! used to disable or enable updating of window, for batch displaying.
    //! this is useful to avoid flickering and speed up display.
    virtual void set_wupdate(bool ud);
    //! Clears the window, but does not resize it, leaves it to the current
    //! size.
    virtual void clear();
    //! Clears the window and resizes it to 1x1.
    virtual void clear_resize();
    //! Resize this window to hxw.
    //! \param force If true, will resize to any size, otherwise, will not
    //!   accept to resize smaller than current size.
    virtual void resize_window(uint h, uint w, bool force = false);
    //! Refresh display of the window.
    //! \param activate Raises the focus to this window if true.
    virtual void update_window(bool activate = false);
    //! Repaints the pixmap and updates window. This will take changes in
    //! drawing_mode into account.
    virtual void repaint_pixmap();
    //! Set color of background.
    virtual void set_bg_colors(ubyte r, ubyte g, ubyte b);

    ////////////////////////////////////////////////////////////////
    // additionnal methods
    
    //! add img to the window and delete img.
    void update_pixmap(idx<ubyte> *img, uint h0, uint w0, bool updatepix =true);
    //! add img to the window.
    void update_pixmap(idx<ubyte> &img, uint h0, uint w0, bool updatepix =true);

  protected:
    ////////////////////////////////////////////////////////////////
    // event methods

    //! The event triggered when painting is needed.
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    ////////////////////////////////////////////////////////////////
    // update methods
    
    //! Resize current buffer to given height and width. 
    //! \param force If true, will resize to any size, otherwise, will not
    //!   accept to resize smaller than current size.
    void buffer_resize(uint h, uint w, bool force = false);
    //! Fill given buffer with background color.
    void buffer_fill(idx<ubyte> *buf);
    //! \param mode The channel(s) to display, 0: RGB, 1: R, 2: G, 3: B.
    void update_qimage(uint mode = 0);
    void swap();

    ////////////////////////////////////////////////////////////////
    // painting/drawing methods
    void draw_arrows(QPainter &painter);
    void draw_boxes(QPainter &painter);
    void draw_crosses(QPainter &painter);
    void draw_ellipses(QPainter &painter);
    void draw_masks(QPainter &painter);
    void draw_images(bool update = false);

    // member variables ////////////////////////////////////////////////////////
  protected:
    using win::buffer;
    using win::drawing_mode;
    
  private slots:
    void scroll_previous();
    void scroll_next();
    
  private:
    QPixmap		*pixmap;
    QPoint		 pixmapOffset;
    QPoint		 lastDragPos;
    double		 pixmapScale;
    uint		 buffer_maxh;
    uint		 buffer_maxw;
    QImage		*qimage;
    idx<ubyte>          *channel_buffer;
  };

} // namespace ebl {

#endif /* WIN2D_H_ */
