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

#include "scroll_box.h"
#include <ostream>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // scroll_box

  scroll_box::scroll_box() : w(NULL) {
  }
  
  scroll_box::~scroll_box() {
    //    cout << "removing scroll box " << this << " of win " << win << endl;
  }

  void scroll_box::set_parent(void *win_) {
    if (dynamic_cast<win*>((win*)win_)) {
      w = (win*) win_;
      // page controls
      if (!button_previous) {
	button_previous = new QPushButton("<", w->get_widget());
	button_previous->setGeometry(_w1 - 80, _h0 - 1, 40, 20);
	w->get_widget()->connect(button_previous, SIGNAL(clicked()), 
				 (QWidget*) w->get_widget(),
				 SLOT(scroll_previous()));
	button_previous->show();
      }
      if (!button_next) {
	button_next = new QPushButton(">", w->get_widget());
	button_next->setGeometry(_w1 - 40, _h0 - 1, 40, 20);
	w->get_widget()->connect(button_next, SIGNAL(clicked()), 
				 (QWidget*) w->get_widget(),
				 SLOT(scroll_next()));
	button_next->show();
      }
    }
    else
      eblerror("expected a win* object");
  }

  void scroll_box::display_controls() {
    if (w) {
      // page count
      //      gui << black_on_white() << at(h0, w0);
      //gui << "page " << page_number << "/" << max_pages();
      w->set_text_colors(0,0,0,255,255,255,255,255);
      w->set_text_origin(_h0, _w0);
      ostringstream o("");
      o << "page " << page_number << "/" << max_pages();
      w->add_text(new string(o.str()));
    }
  }

} // end namespace ebl
