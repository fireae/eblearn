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

#ifndef IDXGUI_HPP_
#define IDXGUI_HPP_

using namespace std;

namespace ebl {
  
  template<class T>
  void idxgui::draw_matrix(idx<T> &im, unsigned int h0, unsigned int w0, 
			   double zoomh, double zoomw, T minv, T maxv) {
    idx<ubyte> *uim = new idx<ubyte>(image_to_ubyte<T>(im, zoomh, zoomw,
						       minv, maxv));
    // send image to main gui thread
    emit gui_drawImage(uim, h0, w0);
  }

  template<class T>
  void idxgui::draw_matrix(idx<T> &im, unsigned int h0, unsigned int w0) {
    idx<ubyte> *uim = new idx<ubyte>(im.get_idxdim());
    // cast data
    idx_copy(im, *uim);
    // send image to main gui thread
    emit gui_drawImage(uim, h0, w0);
  }

  template<class T>
  void idxgui::draw_matrix(idx<T> &im, const char *str,
			   unsigned int h0, unsigned int w0, 
			   double zoomh, double zoomw, T minv, T maxv) {
    draw_matrix(im, h0, w0, zoomh, zoomw, minv, maxv);
    if (str)
      (*this) << white_on_transparent() << gui_only() << at(h0, w0) << str;
  }

  template<class T>
  void idxgui::draw_matrix_frame(idx<T> &im, ubyte r, ubyte g, ubyte b,
				 unsigned int h0, unsigned int w0, 
				 double zoomh, double zoomw,
				 T minv, T maxv) {
    idx<ubyte> uim = image_to_ubyte<T>(im, zoomh, zoomw, minv, maxv);
    idx<ubyte> tmp(uim.dim(0) + 2, uim.dim(1) + 2);
    idx<ubyte> *fim = new idx<ubyte>(tmp);
    idx<ubyte> tmp2 = tmp.narrow(0, uim.dim(0), 1);
    tmp2 = tmp2.narrow(1, uim.dim(1), 1);
    idx_copy(uim, tmp2);
    tmp2 = tmp.narrow(0, 1, 0); idx_fill(tmp2, r);
    tmp2 = tmp.narrow(0, 1, tmp.dim(0) - 1); idx_fill(tmp2, r);
    tmp2 = tmp.narrow(1, 1, 0); idx_fill(tmp2, r);
    tmp2 = tmp.narrow(1, 1, tmp.dim(1) - 1); idx_fill(tmp2, r);
    // send image to main gui thread
    emit gui_drawImage(fim, h0, w0);
  }

  template<class T>
  void idxgui::draw_mask(idx<T> &im, uint h0, uint w0, 
			 double zoomh, double zoomw,
			 ubyte r, ubyte g, ubyte b, ubyte a, T threshold) {
    idx<float> binary(im.get_idxdim());
    idx_threshold(im, (T) threshold, (float) 1.0, (float) 0.0, binary);
    idx<ubyte> *uim = new idx<ubyte>
      (image_to_ubyte<float>(binary, zoomh, zoomw, (float) 0.5, (float) .6));
    // QT mask works with rgb input, TODO: find how to use 2D also
    if (uim->order() == 2) { // replicate 2D into 3D rgb
      idx<ubyte> *rgb = new idx<ubyte>(uim->dim(0), uim->dim(1), 3);
      idx<ubyte> tmp = rgb->select(2, 0);
      idx_copy(*uim, tmp);
      tmp = rgb->select(2, 1);
      idx_copy(*uim, tmp);
      tmp = rgb->select(2, 2);
      idx_copy(*uim, tmp);
      delete uim;
      uim = rgb;
    }
    // send image to main gui thread
    emit gui_draw_mask(uim, h0, w0, r, g, b, a);
  }
  
  template<class T1, class T2> 
  idxgui& operator<<(idxgui& r, const ManipInfra<T1, T2> &manip) {
    manip(r);
    return r;
  }

  template<class T> 
  idxgui& operator<<(idxgui& r, const T val) {
    ostringstream o;
    o << val;
    r.draw_text(new std::string(o.str()));
    if (r.cout_output)
      cout << o.str() << flush;
    return r;
  }

} // end namespace ebl

#endif /* IDXGUI_HPP_ */
