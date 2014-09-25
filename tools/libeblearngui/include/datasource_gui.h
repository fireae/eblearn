/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef DATASOURCE_GUI_H_
#define DATASOURCE_GUI_H_

#include "libidxgui.h"
#include "datasource.h"

using namespace std;

namespace ebl {

////////////////////////////////////////////////////////////////
//! labeled_datasource_gui
template<typename T, typename Tdata, typename Tlabel>
class labeled_datasource_gui : public scroll_box {
 protected:
  bool		scroll;
  bool		scroll_added;
  unsigned int        pos;
  int			display_wid;
  unsigned int	_nh;
  unsigned int	_nw;
  double		_zoom;
  T                _rmin;
  T                _rmax;
  bool                _replaced;

 public:
  labeled_datasource<T, Tdata, Tlabel>	*_last_ds;
  labeled_datasource<T, Tdata, Tlabel>	*_ds;

  //! if scroll is true, then controls to display more pages appear.
  labeled_datasource_gui(bool scroll = false);
  virtual ~labeled_datasource_gui();

  //! display the first nh * nw samples of dataset ds at coordinates (h0, w0),
  //! with zoom <zoom>. If a window id <wid> is specified, use that window,
  //! otherwise create a new window and reuse it.
  //! <wname> is an optional window title.
  virtual void display(labeled_datasource<T, Tdata, Tlabel> &ds,
                       unsigned int nh, unsigned int nw,
                       unsigned int h0 = 0, unsigned int w0 = 0,
                       double zoom = 1.0, int wid = -1,
                       const char *wname = NULL, bool scrolling = false,
                       T rangemin = 0, T rangemax = 0);

  virtual void display_pickings(labeled_datasource<T, Tdata, Tlabel> &ds,
                                unsigned int nh, unsigned int nw,
                                unsigned int h0 = 0, unsigned int w0 = 0,
                                double zoom = 1.0, int wid = -1,
                                const char *wname = NULL,
                                bool scrolling = false,
                                T rangemin = 0, T rangemax = 0);

  ////////////////////////////////////////////////////////////////
  // inherited methods to implement for scrolling capabilities

  //! scrolling method.
  virtual void display_next ();
  //! scrolling method.
  virtual void display_previous ();
  //! scrolling method.
  virtual unsigned int max_pages ();
  //    virtual scroll_box0* copy();
  virtual labeled_datasource_gui* copy();
};

////////////////////////////////////////////////////////////////
//! labeled_datasource_gui
template<typename T, typename Tdata, typename Tlabel>
class labeled_pair_datasource_gui
    : public labeled_datasource_gui<T, Tdata, Tlabel> {
 protected:
  labeled_pair_datasource<T, Tdata, Tlabel>	*_last_ds;
 public:
  labeled_pair_datasource<T, Tdata, Tlabel>	*_ds;

  //! if scroll is true, then controls to display more pages appear.
  labeled_pair_datasource_gui(bool scroll = false);
  virtual ~labeled_pair_datasource_gui();

  //! display the first nh * nw samples of dataset ds at coordinates (h0, w0),
  //! with zoom <zoom>. If a window id <wid> is specified, use that window,
  //! otherwise create a new window and reuse it.
  //! <wname> is an optional window title.
  virtual void display(labeled_pair_datasource<T, Tdata, Tlabel> &ds,
                       unsigned int nh, unsigned int nw,
                       unsigned int h0 = 0, unsigned int w0 = 0,
                       double zoom = 1.0, int wid = -1,
                       const char *wname = NULL, bool scrolling = false,
                       T rangemin = 0, T rangemax = 0);

  ////////////////////////////////////////////////////////////////
  // inherited methods to implement for scrolling capabilities

  //! scrolling method.
  virtual void display_next ();
  //! scrolling method.
  virtual void display_previous ();
  //! scrolling method.
  virtual unsigned int max_pages ();
  //! copy
  virtual labeled_pair_datasource_gui* copy();
};

} // end namespace ebl

#include "datasource_gui.hpp"

#endif /* DATASOURCE_GUI_H_ */
