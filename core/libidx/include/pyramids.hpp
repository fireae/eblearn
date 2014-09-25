/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet   *
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

#ifndef PYRAMIDS_HPP_
#define PYRAMIDS_HPP_

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors/allocation

  template <class T>
  gaussian_pyramid<T>::gaussian_pyramid(double c) : a(c) {
    filter = create_burt_adelson_kernel<T>(a);
  }

  template <class T>
  gaussian_pyramid<T>::~gaussian_pyramid() {
  }

  ////////////////////////////////////////////////////////////////
  // scaling operations

  template <class T>
  idx<T> gaussian_pyramid<T>::reduce(idx<T> &in, uint n) {
    if (n == 0) // no reductions anymore
      return in;
    // only accept 2D images or 3D with channel dim to 0.
    if ((in.order() != 2) && (in.order() != 3))
      eblerror("unexpected image format");
    int d0 =  1;
    int d1 = d0 + 1;
    intg ii = 1 + 2 * ((int) ((in.dim(d0) - 1) / 2));
    intg ij = 1 + 2 * ((int) ((in.dim(d1) - 1) / 2));
    intg oi = 1 + (ii - 5) / 2;
    intg oj = 1 + (ij - 5) / 2;
    // copy in into tin without borders
    idxdim di(in);
    di.setdim(d0, ii);
    di.setdim(d1, ij);
    red_tin = idx<T>(di);
    image_paste_center(in, red_tin);
    idx<T> tin = red_tin;
    // convolve filter over tin into out
    idxdim dout(in);
    dout.setdim(d0, oi);
    dout.setdim(d1, oj);
    idx<T> out(dout);
    idx_clear(out);
    if (tin.order() == 3) { // loop over each channel
      idx_bloop2(ttin, tin, T, outt, out, T) {
	idx<T> uin = ttin.unfold(0, 5, 2);
	uin = uin.unfold(1, 5, 2);
	idx_m4dotm2acc(uin, filter, outt);
      }
    }
    else {
      idx<T> uin = tin.unfold(0, 5, 2);
      uin = uin.unfold(1, 5, 2);
      idx_m4dotm2acc(uin, filter, out); // just one channel
    }
    return reduce(out, n - 1);
  }

  template <class T>
  rect<uint> gaussian_pyramid<T>::reduce_rect(const rect<uint> &r, uint n) {
    if (n == 0)
      return r;
    uint h = 1+ ((1 + 2 * ((int) (r.height - 1) / 2)) - 5) / 2;
    uint w = 1+((1 + 2 * ((int) (r.width - 1) / 2)) - 5) / 2;
    uint h0 = r.h0 / 2;
    uint w0 = r.w0 / 2;
    rect<uint> rr(h0, w0, h, w);
    return reduce_rect(rr, n - 1);
  }

  template <class T>
  uint gaussian_pyramid<T>::count_reductions(uint insz, uint outsz,
						 uint &dist) {
    if (insz <= outsz) {
      // set distance below outsz
      dist = outsz - insz;
      return 0;
    }
    uint newsz = ((1 + 2 * ((int) (insz - 1) / 2)) - 5) / 2;
    return 1 + count_reductions(newsz, outsz, dist);
  }

  template <class T>
  uint gaussian_pyramid<T>::
  count_reductions_exact(rect<uint> &inr, rect<uint> &outr,
			 rect<uint> &inr_exact) {
    // upsample from exact target size beyond current input size
    uint distup;
    uint expansions = count_expansions(outr.height, inr.height, distup);
    rect<uint> uprect = expand_rect(outr, expansions);
    rect<uint> downrect = expand_rect(outr, (std::max)((uint) 0,
						       expansions - 1));
    if (uprect.height - inr.height < inr.height - downrect.height) {
      inr_exact = uprect;
      return expansions;
    } else {
      inr_exact = downrect;
      return expansions - 1;
    }
  }

  template <class T>
  idx<T> gaussian_pyramid<T>::expand(idx<T> &in, uint n) {
    if (n == 0) // no more expansions
      return in;
    // only accept 2D images or 3D with channel dim to 0.
    if ((in.order() != 2) && (in.order() != 3)) {
      eblwarn( "error: gaussian_pyramid only accepts 2D images or 3D. "
               << "input image is " << in << std::endl);
      eblerror("unexpected image format");
    }
    int d0 = 1;
    int d1 = 2;
     intg ii = in.dim(d0);
    intg ij = in.dim(d1);
    intg oi = (ii - 1) * 2 + 5;
    intg oj = (ij - 1) * 2 + 5;

    idxdim di(in);
    di.setdim(d0, oi);
    di.setdim(d1, oj);
    idx<T> tin(di);
    idx_clear(tin);

    // prepare filter x4
    idxdim df(filter);
    idx<T> filt(df);
    idx_copy(filter, filt);
    idx_dotc(filt, (T) 4, filt);

    if (tin.order() == 3) { // loop over each channel
      idx_bloop2(inn, in, T, ttin, tin, T) {
	idx<T> uin = ttin.unfold(0, 5, 2);
	uin = uin.unfold(1, 5, 2);
	idx_m2extm2acc(inn, filt, uin);
      }
    }
    else {
      idx<T> uin = tin.unfold(0, 5, 2);
      uin = uin.unfold(1, 5, 2);
      idx_m2extm2acc(in, filt, uin); // just one channel
    }
    tin = cut_pad(tin, 2);
    return expand(tin, n - 1);
  }

  template <class T>
  rect<uint> gaussian_pyramid<T>::expand_rect(const rect<uint> &r, uint n) {
    if (n == 0)
      return r;
    uint h = (r.height - 1) * 2 + 5;
    uint w = (r.width - 1) * 2 + 5;
    uint h0 = r.h0 * 2;
    uint w0 = r.w0 * 2;
    rect<uint> rr(h0, w0, h, w);
    return expand_rect(rr, n - 1);
  }

  template <class T>
  uint gaussian_pyramid<T>::count_expansions(uint insz, uint outsz,
						 uint &dist) {
    if (insz > outsz) {
      return 0;
    }
    dist = (uint) std::abs((float)outsz - insz);
    uint newsz = (insz - 1) * 2 + 5;
    return 1 + count_expansions(newsz, outsz, dist);
  }

  ////////////////////////////////////////////////////////////////
  // util methods

  template <class T>
  idx<T> gaussian_pyramid<T>::cut_pad(idx<T> &in, int nz) {
    idxdim dout(in);
    int d0 = 1;
    int d1 = d0 + 1;
    dout.setdim(d0, in.dim(d0) - 2 * nz);
    dout.setdim(d1, in.dim(d1) - 2 * nz);
    idx<T> o = idx<T>(dout);
    idx<T> i = in.narrow(d0, o.dim(d0), nz);
    i = i.narrow(d1, o.dim(d1), nz);
    idx_copy(i, o);
    return o;
  }

} // end namespace ebl

#endif /* PYRAMIDS_HPP_ */
