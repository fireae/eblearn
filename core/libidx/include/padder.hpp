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

#ifndef PADDER_HPP_
#define PADDER_HPP_

namespace ebl {

  //////////////////////////////////////////////////////////////////////////////
  // padder

  template <typename T>
  padder<T>::padder(const idxdim &kerdims, bool mirror_)
    : nrow((int) (floor(kerdims.dim(0) / (float) 2.0))),
      ncol((int) (floor(kerdims.dim(1) / (float) 2.0))),
      nrow2(nrow), ncol2(ncol), bmirror(mirror_) {
    // remove 1 pixel on right and bottom borders if even.
    if (kerdims.dim(0) % 2 == 0)
      nrow2 -= 1;
    if (kerdims.dim(1) % 2 == 0)
      ncol2 -= 1;
  }

  template <typename T>
  padder<T>::~padder() {
  }

  template <typename T>
  idx<T> padder<T>::pad(idx<T> &in) {
    // allocate padded buffer
    idxdim d = in;
    d.setdim(1, d.dim(1) + nrow + nrow2);
    d.setdim(2, d.dim(2) + ncol + ncol2);
    idx<T> out(d);
    idx_clear(out);
    // copy in to padded buffer
    idx<T> tmp = out.narrow(1, in.dim(1), nrow);
    tmp = tmp.narrow(2, in.dim(2), ncol);
    idx_copy(in, tmp);
    if (bmirror)
      mirror(in, out);
    // return result
    return out;
  }
  
  template <typename T>
  void padder<T>::pad(idx<T> &in, idx<T> &out) {
    // allocate padded buffer
    idxdim d = in;
    d.setdim(1, d.dim(1) + nrow + nrow2);
    d.setdim(2, d.dim(2) + ncol + ncol2);
    if (out.get_idxdim() != d)
      out.resize(d);
    idx_clear(out);
    // copy in to padded buffer
    idx<T> tmp = out.narrow(1, in.dim(1), nrow);
    tmp = tmp.narrow(2, in.dim(2), ncol);
    idx_copy(in, tmp);
    if (bmirror)
      mirror(in, out);
  }
  
  template <typename T>
  void padder<T>::mirror(idx<T> &in, idx<T> &padded) {
    idx<T> tmp, tmp2;
    int i;
    // mirror border left
    for (i = std::max(0, (int) (ncol - in.dim(1) / 2)); i < ncol; ++i) {
      tmp2 = in.narrow(1, 1, ncol - i - 1);
      tmp = padded.narrow(1, 1, i);
      tmp = tmp.narrow(2, in.dim(2), ncol);
      idx_copy(tmp2, tmp);
    }
    // mirror border right
    for (i = std::max(0, (int) (ncol - in.dim(1) / 2)); i < ncol; ++i) {
      tmp2 = in.narrow(1, 1, in.dim(1) - ncol - 1 + i);
      tmp = padded.narrow(1, 1, padded.dim(1) - 1 - i);
      tmp = tmp.narrow(2, in.dim(2), ncol);
      idx_copy(tmp2, tmp);
    }
    // mirror border top using out as input
    for (i = std::max(0, (int) (nrow - in.dim(2) / 2)); i < nrow; ++i) {
      tmp2 = padded.narrow(2, 1, nrow + nrow - i - 1);
      tmp = padded.narrow(2, 1, i);
      idx_copy(tmp2, tmp);
    }
    // mirror border bottom using out as input
    for (i = std::max(0, (int) (nrow - in.dim(2) / 2)); i < nrow; ++i) {
      tmp2 = padded.narrow(2, 1, padded.dim(2) - nrow * 2 - 1 + i);
      tmp = padded.narrow(2, 1, padded.dim(2) - 1 - i);
      idx_copy(tmp2, tmp);
    }    
  }

  //////////////////////////////////////////////////////////////////////////////
  // padding utilities
  
  template <typename T> 
  idx<T> image_region_to_rect(idx<T> &im, const rect<int> &r, uint oheight,
			      uint owidth, rect<int> *cropped,
			      uint dh, uint dw) {
    // TODO: check that rectangle is within image
    if (im.order() != 2 && im.order() != 3)
      eblerror("expected a 2d or 3d input but got " << im);
    idxdim d(im);
    d.setdim(dh, oheight);
    d.setdim(dw, owidth);
    idx<T> res(d);

    float hcenter = r.h0 + (float) r.height / 2; // input height center
    float wcenter = r.w0 + (float) r.width / 2; // input width center
    // // limit centers to half the width/height away from borders
    // // to handle incorrect regions
    // hcenter = MIN((float)im.dim(dh) - (float)r.height/2,
    // 		  std::max((float)r.height/2, hcenter));
    // wcenter = MIN((float)im.dim(dw) - (float)r.width/2,
    // 		  std::max((float)r.width/2, wcenter));
    float h0 = hcenter - (float) oheight / 2; // out height offset in input
    float w0 = wcenter - (float) owidth / 2; // out width offset in input
    float h1 = hcenter + (float) oheight / 2;
    float w1 = wcenter + (float) owidth / 2;
    int gh0 = (int) std::max(0, (int) MIN(im.dim(dh), h0)); // input h offset
    int gw0 = (int) std::max(0, (int) MIN(im.dim(dw), w0)); // input w offset
    int gh1 = (int) std::max(0, (int) MIN(im.dim(dh), h1));
    int gw1 = (int) std::max(0, (int) MIN(im.dim(dw), w1));
    int h = gh1 - gh0 + std::max(0, -r.h0); // out height narrow
    int w = gw1 - gw0 + std::max(0, -r.w0); // out width narrow
    int fh0 = (int) std::max(0, (int) (gh0 - h0)); // out height offset narrow
    int fw0 = (int) std::max(0, (int) (gw0 - w0)); // out width offset narrow

    // narrow original image
    int hmin = std::max(0, std::min((int)res.dim(dh) - fh0,
				    std::min((int)im.dim(dh) - gh0, h)));
    int wmin = std::max(0, std::min((int)res.dim(dw) - fw0,
				    std::min((int)im.dim(dw) - gw0, w)));
    // clear result image
    idx_clear(res);
    if (hmin != 0 && wmin != 0) {// only copy if overlap with image      
      idx<T> tmpim = im.narrow(dh, hmin, gh0);
      tmpim = tmpim.narrow(dw, wmin, gw0);
      // narrow target image
      idx<T> tmpres = res.narrow(dh, hmin, fh0);
      tmpres = tmpres.narrow(dw, wmin, fw0);
      // copy original to target
      idx_copy(tmpim, tmpres);
    }
    // set cropped rectangle to region in the output image containing input
    if (cropped) {
      cropped->h0 = fh0;
      cropped->w0 = fw0;
      cropped->height = hmin;
      cropped->width = wmin;
    }
    return res;
  }
  
  template <typename T>
  void image_paste_center(idx<T> &in, idx<T> &out) {
    if (in.order() != 3 || out.order() != 3)
      eblerror("expected 3d idx but got " << in << " and " << out);
    int d0 = 1;
    int d1 = d0 + 1;
    intg ci = in.dim(d0) - out.dim(d0);
    intg cj = in.dim(d1) - out.dim(d1);
    intg xci = (int) (ci / 2);
    intg xcj = (int) (cj / 2);
    idx<T> i = in.narrow(d0, out.dim(d0), xci);
    i = i.narrow(d1, out.dim(d1), xcj);
    idx_copy(i, out);
  }
  
} // end namespace ebl

#endif /* PADDER_HPP_ */
