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

#ifndef WIN_H_
#define WIN_H_

#include <QPixmap>
#include <QWidget>
#include <QtGui>
#include <QResizeEvent>
#include <math.h>
#include <iostream>

#include "defines_idxgui.h"
#include "libidx.h"
#include "scroll_box0.h"
#include "defines.h"

using namespace std;

namespace ebl {

  // TODO: derive all objects from a common drawable_object and hold
  // all of them in a same vector, to retain the drawing order.

  ////////////////////////////////////////////////////////////////
  // string

  class text : public string {
  public:
    float	h0, w0;
    bool                pos_reset;
    unsigned char       fg_r, fg_g, fg_b, fg_a, bg_r, bg_g, bg_b, bg_a;
    text(float h0, float w0, bool pos_reset = false,
	 unsigned char fg_r = 255, unsigned char fg_g = 255,
	 unsigned char fg_b = 255, unsigned char fg_a = 255,
	 unsigned char bg_r = 0, unsigned char bg_g = 0,
	 unsigned char bg_b = 0, unsigned char bg_a = 127);
    ~text() {};
  };

  ////////////////////////////////////////////////////////////////
  // arrow

  class arrow {
  public:
    int	                 h1, w1, h2, w2;
    bool head1, head2;
    arrow(int h1, int w1, int h2, int w2,
	  bool head1 = true, bool head2 = false);
    ~arrow() {};
  };

  ////////////////////////////////////////////////////////////////
  // box

  class box {
  public:
    box(float h0, float w0, float h, float w, ubyte r, ubyte g, ubyte b,
	ubyte a);
    ~box() {};
    float h0, w0, h, w;
    ubyte r, g, b, a;
  };

  ////////////////////////////////////////////////////////////////
  // cross

  class cross {
  public:
    float	 h0, w0, length;
    ubyte	 r, g, b, a;
    cross(float h0, float w0, float length, ubyte r,
	  ubyte g, ubyte b, ubyte a);
    ~cross() {};
  };

  ////////////////////////////////////////////////////////////////
  // image

  class image {
  public:
    uint         h0, w0;
    idx<ubyte>           img;
    image(idx<ubyte> &img, uint h0, uint w0);
    ~image() {};
  };

  ////////////////////////////////////////////////////////////////
  // mask

  class imask {
  public:
    //! Constructor
    imask(idx<ubyte> *img, uint h0, uint w0,
	 ubyte r, ubyte g, ubyte b, ubyte a);
    ~imask() {};

    // members
    uint        h0, w0;
    QPixmap     map;
  };

  ////////////////////////////////////////////////////////////////
  // window

  class IDXGUIEXPORT win {
  public:
    //! \param qwidj The instance QWidget of child class. This is necessary
    //!   because virtual inheritance is not possible with QWidget.
    win(QWidget *qwidj, uint wid, const char *wname = NULL,
	uint height = 1, uint width = 1);
    //! Empty constructor, do not use.
    win();
    virtual ~win();

    virtual void show();
    virtual QWidget* get_widget();

    //! Save the Qt window to 'filename'.png.
    virtual void save(const string &filename, bool confirm = false);
    //! Save raw buffer matrix to 'filename'.mat (this will just contain
    //! images, not text or any Qt drawing.
    virtual void save_mat(const string &filename, bool confirm = false);
    //! Do not display, instead save to image file 'filename'.
    virtual void set_silent(const std::string *filename);
    //! Freeze or unfreeze style. No modification of colors are allowed
    //! in frozen mode.
    virtual void freeze_style(bool freeze);
    //! Force window size to hxw and forbid any resizing.
    virtual void freeze_window_size(uint h, uint w);
    //! Change window title.
    virtual void set_title(const char *title);
    //! used to disable or enable updating of window, for batch displaying.
    //! this is useful to avoid flickering and speed up display.
    virtual void set_wupdate(bool ud);
    //! Resize this window to hxw.
    //! \param force If true, will resize to any size, otherwise, will not
    //!   accept to resize smaller than current size.
    virtual void resize_window(uint h, uint w, bool force = false);
    //! Refresh display of the window.
    //! \param activate Raises the focus to this window if true.
    virtual void update_window(bool activate = false);
    //! Returns true if busy drawing.
    virtual bool busy_drawing();

    ////////////////////////////////////////////////////////////////
    // Objects adding methods

    //! Add text a current text location (See 'set_text_origin()').
    virtual void add_text(const std::string *s);
    virtual void add_arrow(int h1, int w1, int h2, int w2);
    virtual void add_flow(idx<float> *flow, int h, int w);
    virtual void add_box(float h0, float w0, float h, float w, ubyte r, ubyte g,
		 ubyte b, ubyte a, string *s);
    virtual void add_cross(float h0, float w0, float length, ubyte r, ubyte g,
			   ubyte b, ubyte a, string *s);
    virtual void add_ellipse(float h0, float w0, float h, float w, ubyte r,
			     ubyte g, ubyte b, ubyte a, string *s);
    virtual void add_image(idx<ubyte> &img, uint h0, uint w0);
    virtual void add_mask(idx<ubyte> *img, uint h0, uint w0,
			  ubyte r, ubyte g, ubyte b, ubyte a);

    //! Change current location of text to be drawn to (h0,w0).
    virtual void set_text_origin(float h0, float w0);

    ////////////////////////////////////////////////////////////////
    // clear methods

    //! Clears the window, but does not resize it, leaves it to the current
    //! size.
    virtual void clear();
    //! Clears the window and resizes it to 1x1.
    virtual void clear_resize();
    //! Clears the window and resizes it to 1x1.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    virtual void clear_all(bool clear_tmp = false);
    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_text(bool clear_tmp = false);
    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_arrows(bool clear_tmp = false);
    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_boxes(bool clear_tmp = false);
    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_crosses(bool clear_tmp = false);
    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_ellipses(bool clear_tmp = false);
    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_images(bool clear_tmp = false);
    //! Clear this list of objects.
    //! \param clear_tmp If true, clears the temporary list of objects instead.
    void clear_masks(bool clear_tmp = false);

    ////////////////////////////////////////////////////////////////
    // Style methods

    //! \param ignore_frozen Ignore style freeze and apply color changes anyway.
    virtual void set_text_colors(ubyte fg_r, ubyte fg_g, ubyte fg_b, ubyte fg_a,
				 ubyte bg_r, ubyte bg_g, ubyte bg_b, ubyte bg_a,
				 bool ignore_frozen = false);
    //! Set color of background.
    virtual void set_bg_colors(ubyte r, ubyte g, ubyte b);
    //! Set font to this size.
    virtual void set_font_size(int sz);

    ////////////////////////////////////////////////////////////////
    // Events methods

    //! Return first key pressed of the key-pressed even list for
    //! current window and pop it out of the list, or return -1 if no key.
    virtual int pop_key_pressed();

    ////////////////////////////////////////////////////////////////
    // Scrolling methods

    virtual void add_scroll_box(scroll_box0 *sb);
    virtual void remove_scroll_box(scroll_box0 *sb);
    virtual void replace_scroll_box_with_copy(scroll_box0 *sb);

    ////////////////////////////////////////////////////////////////
    // Painting methods

    //! Draw all elements with scale 'scale'.
    virtual void paint(QPainter &painter, double scale = 1.0);
    //! Draw all text into painter with scale 'scale'.
    virtual void draw_text(QPainter &painter);

  protected:
    ////////////////////////////////////////////////////////////////
    // event methods
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

  private slots:
    void scroll_previous();
    void scroll_next();

  protected:
    QWidget             *qw; //!< The widget object of children classes.
    QPixmap		*pixmap;
    QPoint		 pixmapOffset;
    QPoint		 lastDragPos;
    double		 pixmapScale;
    double		 curScale;
    float		 scaleIncr;
    idx<ubyte>		*buffer;
    uint		 buffer_maxh;
    uint		 buffer_maxw;
    QVector<QRgb>	 colorTable;
    QImage		*qimage;
    vector<text*>        texts;
    vector<text*>        texts_tmp;
    text*		 txt;
    vector<arrow*>       arrows;
    vector<arrow*>       arrows_tmp;
    vector<box*>         boxes;
    vector<box*>         boxes_tmp;
    vector<cross*>       crosses;
    vector<cross*>       crosses_tmp;
    vector<box*>         ellipses;
    vector<box*>         ellipses_tmp;
    vector<image*>       images;
    vector<image*>       images_tmp;
    vector<imask*>       masks;
    vector<imask*>       masks_tmp;
    bool		 silent;
    uint		 id;
    string		 savefname;
    float		 text_h0;
    float		 text_w0;
    bool                 pos_reset;	// text position was reset or not
    bool                 wupdate;
    QColor               text_fg_color;
    QColor               text_bg_color;
    QColor               bg_color;
    unsigned char        fg_r, fg_g, fg_b, fg_a, bg_r, bg_g, bg_b, bg_a;
    int                  wupdate_ndisable;// count how many disables called
    bool                 frozen_style; //!< style is frozen or not
    bool                 frozen_size; //!< window size is frozen
    idxdim               frozen_dims; //!< Dimensions of frozen window.
    list<int>            keyspressed; //!< a list of key pressed events
    int                  font_size; //!< The size of the font.
    bool                 ctrl_on; //!< ctrl key is on or not.
    bool                 text_on; //!< Text is displayed or not.
    bool                 images_only; //!< Only display images if true.
    scroll_box0         *scrollbox;
    bool                 busy; //!< Busy drawing.
    uint                 drawing_mode;
  };

} // namespace ebl {

#endif /* WIN_H_ */
