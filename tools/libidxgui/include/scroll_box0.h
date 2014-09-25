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

#ifndef SCROLL_BOX0_H_
#define SCROLL_BOX0_H_

#include <QtGui/QPixmap>
#include <QtGui/QWidget>
#include <QtGui/QtGui>
#include <QtGui/QResizeEvent>
#include <QtGui/QPushButton>
#include <math.h>
#include <iostream>
#include "defines.h"

using namespace std;

namespace ebl {

  class EXPORT scroll_box0 {
  public:
    scroll_box0();
    virtual ~scroll_box0();
    virtual void set_parent(void *parent) = 0;
    virtual void display_next() = 0;
    virtual void display_previous() = 0;
    virtual unsigned int max_pages () = 0;
    virtual void display_controls() = 0;
    virtual scroll_box0* copy() = 0;
    virtual bool next_page();
    virtual bool previous_page();

  protected:
    unsigned int page_number;
    QPushButton *button_next;
    QPushButton *button_previous;
    unsigned int _h0;
    unsigned int _w0;
    unsigned int _h1;
    unsigned int _w1;
  };

} // namespace ebl {

#endif /* SCROLL_BOX0_H_ */
