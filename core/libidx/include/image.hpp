/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <math.h>
#include <stdlib.h>

#define BLK_AVRG(nlin, ncol) {						\
    int k,l; int norm = ncol * nlin;					\
    int acc0=0, acc1=0, acc2=0;						\
    for (k=0; k<nlin; k++) {						\
      register T *pinptr = pin+k*in_mod0;				\
      for (l=0; l<ncol; l++) {						\
	acc0 += (int) pinptr[0];					\
	acc1 += (int) pinptr[1];					\
	acc2 += (int) pinptr[2];					\
	pinptr += in_mod1; }}						\
    pout[0] = acc0 / norm;pout[1] = acc1 / norm;pout[2] = acc2 / norm;}

namespace ebl {

  template <typename T>
  idx<T> image_crop(idx<T> &in, int x, int y, int w, int h) {
    idx<T> bla = in.narrow(0, h, y).narrow(1, w, x);
    idx<T> bla3(bla.dim(0), bla.dim(1), bla.order() < 3 ? -1 : bla.dim(2));
    idx_copy(bla, bla3);
    return bla3;
  }

  //////////////////////////////////////////////////////////////////////////////
  // resize

  template<class T> idx<T> image_resize(idx<T> &image, double h, double w,
					int mode, rect<int> *iregion_,
					rect<int> *oregion_) {
    if (image.order() < 2) eblerror("image must have at least an order of 2.");
    // iregion is optional, set it to entire image if not given
    rect<int> iregion = rect<int>(0, 0, image.dim(0), image.dim(1));
    if (iregion_)
      iregion = *iregion_;
    double ratioh = h / iregion.height;
    double ratiow = w / iregion.width;
    double ratiomin = std::min(ratiow, ratioh);
    // if data is not contiguous, copy it to a contiguous buffer
    idx<T> contim(image);
    if (!image.contiguousp()) {
      idxdim d(image.spec);
      idx<T> tmp(d);
      idx_copy(image, tmp);
      contim = tmp;
    }
    int imw = (int) contim.dim(1);
    int imh = (int) contim.dim(0);
    //    int rw = 0, rh = 0;
    double ow = 0, oh = 0;
    if ((imw == 0) || (imh == 0))
      eblerror("cannot have dimensions of size 0"
	       << " while trying to resize image " << image
	       << " to " << h << "x" << w << " with mode " << mode);
    // determine actual size of output image
    if ((0 == w) || (0 == h)) {
      if (0 == w) {
	if (0 == h)
	  eblerror("desired width and height cannot be both zero"
		   << " while trying to resize image " << image
		   << " to " << h << "x" << w << " with mode " << mode);
	  else	w = std::max(1, (int) (imw * (w / imh)));
      } else	h = std::max(1, (int) (imh * (h / imw)));
    }
    if ((mode == 0) || (mode == 3)) { // preserve aspect ratio
      ratiow = ratiomin;
      ratioh = ratiomin;
    }
    else if (mode == 1) { // possibly modify aspect ratio
      //      ratiow = w; //ratiow;
      //      ratioh = h; //ratioh;
    }
    else if (mode == 2) { // use w and h as scaling ratios
      ratiow = w;
      ratioh = h;
    }
    else
      eblerror("illegal mode or desired dimensions"
	       << " while trying to resize image " << image
	       << " to " << h << "x" << w << " with mode " << mode);
    // output sizes of entire image
    if (iregion_ || (mode == 2) || (mode == 0) || (mode == 3)) {
      ow = rint(std::max(1.0, imw * ratiow));
      oh = rint(std::max(1.0, imh * ratioh));
    } else {
      ow = w;//std::max(1.0, imw * ratiow);
      oh = h;//std::max(1.0, imh * ratioh);
    }
    // compute closest integer subsampling ratio
    //rw = std::std::max(1, (int) (1 / ratiow));
    //rh = std::std::max(1, (int) (1 / ratioh));
    // compute output region
    rect<int> oregion((uint)(iregion.h0 * ratioh), (uint)(iregion.w0 * ratiow),
		 (uint)(iregion.height * ratioh),
		 (uint)(iregion.width * ratiow));
    if (oregion_)
      *oregion_ = oregion;
    // TODO: why is this useful? for ubyte images?
    // subsample by integer ratio if necessary
    //     if ((rh > 1) || (rw > 1)) {
    //       contim = image_subsample(contim, rh, rw);
    //       imw = contim.dim(1);
    //       imh = contim.dim(0);
    //     }
    // resample from subsampled image with bilinear interpolation
    idx<T> rez((intg) oh, (intg) ow, (contim.order() == 3) ? contim.dim(2) : 1);
    idx<T> bg(4);
    idx_clear(bg);
    // the 0.5 thingies are necessary because warp-bilin interprets
    // integer coordinates as being at the center of each pixel.
    float x1 = (float) -0.5, y1 = (float) -0.5;
    float x3 = (float) imw - (float) 0.5, y3 = (float) imh - (float) 0.5;
    float p1 = (float) -0.5, q1 = (float) -0.5;
    float p3 = (float) ow - (float) 0.5, q3 = (float) oh - (float) 0.5;
    image_warp_quad(contim, rez, bg, 1, x1, y1, x3, y1, x3, y3, x1, y3,
		    p1, q1, p3, q3);
    if (contim.order() == 2)
      return rez.select(2, 0);
    // copy preserved ratio output in the middle of the wanted out size
    if ((mode == 3) && ((rez.dim(0) != h) || (rez.dim(1) != w))) {
      idx<T> out((intg) h, (intg) w, rez.dim(2));
      idx_clear(out);
      idx<T> tmp = out.narrow(0, rez.dim(0), (intg) ((h - rez.dim(0)) / 2));
      tmp = tmp.narrow(1, rez.dim(1), (intg) ((w - rez.dim(1)) / 2));
      idx_copy(rez, tmp);
      rez = out;
    }
    return rez;
  }

  template <typename T>
  idx<T> image_gaussian_resize(idx<T> &im, double oheight, double owidth,
			       uint mode, rect<int> *iregion_,
			       rect<int> *oregion) {
    // only accept 2D or 3D images
    if ((im.order() != 2) && (im.order() != 3)) {
      eblwarn("illegal order: " << im << std::endl);
      eblerror("unexpected image format");
    }
    // iregion is optional, set it to entire image if not given
    rect<uint> iregion(0, 0, im.dim(0), im.dim(1));
    if (iregion_)
      iregion = *iregion_;
    // if region's height and width already have the correct size, return
    if ((iregion.height == oheight) && (iregion.width <= owidth)) {
      if (oregion)
	*oregion = iregion;
      return im;
    }
    // gaussian resize
    gaussian_pyramid<T> gp;
    rect<uint> outr;
    idx<T> rim;
    ////////////////////////////////////////////////////////////////
    if ((iregion.width > owidth) || (iregion.height > oheight)) { // reduce
      rect<uint> exact_inr;
      uint reductions;

      switch (mode) {
      case 0: // keep aspect ratio
	// compute biggest down/up-sizing factor. do not use a double variable
	// to compute ratio, compute it directly. somehow the results are
	// more precise without the variable.
	if (oheight / (double) iregion.height <
	    owidth  / (double) iregion.width)
	  outr = rect<uint>
	    ((uint)(iregion.h0 * oheight / (double)iregion.height),
	     (uint)(iregion.w0 * oheight / (double)iregion.height),
	     (uint)(iregion.height * oheight / (double)iregion.height),
	     (uint)(iregion.width * oheight / (double)iregion.height));
	else
	  outr = rect<uint>
	    ((uint) (iregion.h0     * owidth / (double)iregion.width),
	     (uint) (iregion.w0     * owidth / (double)iregion.width),
	     (uint) (iregion.height * owidth / (double)iregion.width),
	     (uint) (iregion.width  * owidth / (double)iregion.width));
	reductions = gp.count_reductions_exact(iregion, outr, exact_inr);
	break ;
      default:
	eblerror("unsupported mode " << mode);
      }
      // bilinear resize at closest resolution to current resolution
      double exact_imh = (exact_inr.height / (double) iregion.height)
	* (double) im.dim(0);
      double exact_imw = (exact_inr.width / (double) iregion.width)
	* (double) im.dim(1);
      rim = image_resize(im, exact_imh, exact_imw, 1);
      // now gaussian resize to exact target size
      rim = rim.shift_dim(2, 0);
      rim = gp.reduce(rim, reductions);
      rim = rim.shift_dim(0, 2);
    ////////////////////////////////////////////////////////////////
    } else { // expand
      uint expansions;
      uint imax, omax, dist;
      rect<uint> outr2;
      // select biggest side
      if (iregion.height > iregion.width) {
	imax = iregion.height;
	omax = (uint) oheight;
      } else {
	imax = iregion.width;
	omax = (uint) owidth;
      }
      // count how many expansions necessary
      switch (mode) {
      case 0: // keep aspect ratio
	expansions = gp.count_expansions(imax, omax, dist);
	break ;
      default:
	eblerror("unsupported mode");
      }
      // now gaussian resize to exact target size
      rim = im.shift_dim(2, 0);
      rim = gp.expand(rim, expansions);
      outr2 = gp.expand_rect(iregion, expansions);
      // bilinear resize to target resolution
      rim = rim.shift_dim(0, 2);
      // TODO: casting to int correct?
      rect<int> oor2, oor;
      oor2 = outr2; oor = outr;
      rim = image_resize(rim, oheight, owidth, mode, &oor2, &oor);
      outr2 = oor2; outr = oor;
    }
    ////////////////////////////////////////////////////////////////
    if (oregion)
      *oregion = outr;
    return rim;
  }

  template <typename T>
  idx<T> image_mean_resize(idx<T> &im, double oheight, double owidth,
			   uint mode, rect<int> *iregion_, rect<int> *oregion) {
    // only accept 2D or 3D images
    if ((im.order() != 2) && (im.order() != 3)) {
      eblwarn( "illegal order: " << im << std::endl);
      eblerror("unexpected image format");
    }
    if (oheight == 0 || owidth == 0) {
      eblwarn( "oheight: " << oheight << " owidth " << owidth << std::endl);
      eblerror("illegal resize image to zero");
    }
    // iregion is optional, set it to entire image if not given
    rect<int> iregion(0, 0, im.dim(0), im.dim(1));
    if (iregion_)
      iregion = *iregion_;
    // if region's height and width already have the correct size, return
    if ((iregion.height == oheight) && (iregion.width <= owidth)) {
      if (oregion)
	*oregion = iregion;
      return im;
    }
    // mean resize
    rect<int> outr, inr;
    idx<T> rim, out;
    ////////////////////////////////////////////////////////////////
    if ((iregion.width > owidth) || (iregion.height > oheight)) { // reduce
      switch (mode) {
      case 0: // keep aspect ratio
	// compute biggest down/up-sizing factor. do not use a double variable
	// to compute ratio, compute it directly. somehow the results are
	// more precise without the variable.
	if (oheight / (double) iregion.height <
	    owidth  / (double) iregion.width) {
	  outr =
	    rect<int>((uint)(iregion.h0    * oheight / (double)iregion.height),
		      (uint)(iregion.w0    * oheight / (double)iregion.height),
		      (uint)(iregion.height* oheight / (double)iregion.height),
		      (uint)(iregion.width * oheight / (double)iregion.height));
	  inr =
	    rect<int>(0, 0,
		      (uint) (im.dim(0) * oheight / (double)iregion.height),
		      (uint) (im.dim(1) * oheight / (double)iregion.height));
	} else { // use width factor (smaller factor)
	  outr =
	    rect<int>((uint) (iregion.h0     * owidth / (double)iregion.width),
		      (uint) (iregion.w0     * owidth / (double)iregion.width),
		      (uint) (iregion.height * owidth / (double)iregion.width),
		      (uint) (iregion.width  * owidth / (double)iregion.width));
	  inr =
	    rect<int>(0, 0,
		      (uint) (im.dim(0) * owidth / (double)iregion.width),
		      (uint) (im.dim(1) * owidth / (double)iregion.width));
	}
	break ;
      default:
	eblerror("unsupported mode " << mode);
      }
      // find closest multiple of target area that is smaller than input area
      // (smaller to avoid upsampling)
      uint fact = (std::min)((uint)floor(iregion.height / (float) outr.height),
			     (uint)floor(iregion.width / (float) outr.width));
      if (fact == 0) // no multiple smaller than input, go straight for bilinear
	return image_resize(im, oheight, owidth, mode, iregion_, oregion);
      // bilinear resize at closest resolution to current resolution
      rim = image_resize(im, inr.height * fact, inr.width * fact, 1);
      //  add extra padding around original image if it's not a multiple of fact
      // rect rrim(0, 0, rim.dim(0), rim.dim(1));
      // rect cropped;
      // uint hmod = rim.dim(0) % fact;
      // if (hmod) hmod = fact - hmod;
      // uint wmod = rim.dim(1) % fact;
      // if (wmod) wmod = fact - wmod;
      // idx<T> rimf = image_region_to_rect(rim, rrim, rim.dim(0) + hmod,
      // 					 rim.dim(1) + wmod, cropped);
      // allocate output
      out = idx<T>(rim.dim(0) / fact, rim.dim(1) / fact, rim.dim(2));
      idx<T> tmp, tmpout;
      uint i = 0, j = 0;
      // now mean resize to exact target size
      idx_bloop1(ou, out, T) { // loop on output rows
	idx_bloop1(o, ou, T) { // loop on output cols
	  tmp = rim.narrow(0, fact, i * fact);
	  tmp = tmp.narrow(1, fact, j * fact);
	  idx_eloop2(layer, tmp, T, olayer, o, T) { // loop on each layer
	    olayer.set(idx_mean(layer));
	  }
	  j++;
	}
	i++;
	j = 0;
      }
    ////////////////////////////////////////////////////////////////
    } else { // expansion: return bilinear resizing
      return image_resize(im, oheight, owidth, mode, iregion_, oregion);
    }
    ////////////////////////////////////////////////////////////////
    if (oregion)
      *oregion = outr;
    return out;
  }

  template <typename T>
  idx<T> image_region_to_square(idx<T> &im, const rect<uint> &r) {
    // TODO: check expecting 2D or 3D
    // TODO: check that rectangle is within image
    uint sz = std::max(r.height, r.width);
    idxdim d(im);
    uint dh = 0;
    uint dw = 1;
    d.setdim(dh, sz);
    d.setdim(dw, sz);
    idx<T> res(d);
    uint tmp_h = MIN(sz, r.height);
    uint tmp_w = MIN(sz, r.width);
    idx<T> tmpres = res.narrow(dh, tmp_h, res.dim(dh) / 2 - tmp_h / 2);
    tmpres = tmpres.narrow(dw, tmp_w, res.dim(dw) / 2 - tmp_w / 2);
    idx<T> tmpim = im.narrow(dh, tmp_h, r.h0);
    tmpim = tmpim.narrow(dw, tmp_w, r.w0);
    idx_clear(res);
    idx_copy(tmpim, tmpres);
    return res;
  }

  template <typename T>
  idx<ubyte> image_to_ubyte(idx<T> &im, double zoomh, double zoomw,
			    T minv, T maxv) {
    // check the order and dimensions
    if ((im.order() < 2) || (im.order() > 3) ||
	((im.order() == 3) && (im.dim(2) != 1) && (im.dim(2) != 3))) {
      eblerror("expected a 2D idx or a 3D idx with 1 or 3 channels only "
	       << "but got: " << im);
    }
    // create a copy
    idxdim d(im);
    idx<T> im1(d);
    idx_copy(im, im1);
    // check zoom factor
    if ((zoomw <= 0.0) || (zoomh <= 0.0))
      eblerror("cannot zoom by a factor <= 0.0");
    // check minv maxv
    if (minv > maxv) {
      T tmp = minv;
      minv = maxv;
      maxv = tmp;
    }
    // if minv and maxv are defaults, take actual min and max of the image
    if (minv == maxv) {
      minv = idx_min(im1);
      maxv = idx_max(im1);
    }
    if (minv == maxv) { // if minv still == maxv, use 0 and 1
      minv = 0;
      maxv = 1;
    }
    // create target image
    int newh = (int) (im1.dim(0) * zoomh);
    int neww = (int) (im1.dim(1) * zoomw);
    idx<T> im2 = ((newh == im1.dim(0)) && (neww == im1.dim(1))) ?
      im1 : image_resize(im1, newh, neww);
    d.setdim(0, newh);
    d.setdim(1, neww);
    idx<ubyte> image(d);
    // map values between minv and maxv to 0 .. 255
    idx_subc_bounded(im2, minv, im2);
    idx_dotc_bounded(im2, (T) (255.0 / (double) (maxv - minv)), im2);
    idx_copy_clip(im2, image);
    return image;
  }

  template <typename T>
  idx<T> image_subsample_grayscale(idx<T> &in, int nlin, int ncol) {
    intg h = in.dim(0);
    intg w = in.dim(1);
    intg nh = h / nlin;
    intg nw = w / ncol;
    idx<T> out(nh, nw);
    if ((nlin == 1) && (ncol == 1)) {
      idx_copy(in, out);
      return out;
    }
    idx<T> inp = in.narrow(0, nlin * nh, 0);
    inp = inp.narrow(1, ncol * nw, 0);
    T *_idx2loopc1, *pin;
    T *_idx2loopc2, *pout;
    int i, _imax = out.dim(0);
    int j, _jmax = out.dim(1);
    int _imat1_m0 = inp.mod(0);
    int _imat1_m1 = inp.mod(1);
    int _imat2_m0 = out.mod(0);
    int _imat2_m1 = out.mod(1);
    int pin_incr = ncol * _imat1_m1;
    int norm = ncol * nlin;
    register int acc0;
    register int k,l;
    register T *pinptr;
    register int pinptr_incr = _imat1_m0 - ncol* _imat1_m1;
    _idx2loopc1 = inp.idx_ptr();
    _idx2loopc2 = out.idx_ptr();
    for (i = 0; i < _imax; i++) {
      pin = _idx2loopc1;
      pout = _idx2loopc2;
      for (j = 0; j < _jmax; j++) {
	acc0 =0;
	pinptr = pin;
	for (k=0; k<nlin; k++) {
	  for (l=0; l<ncol; l++) {
	    acc0 += (int) pinptr[0];
	    pinptr += _imat1_m1;
	  }
	  pinptr += pinptr_incr;
	}
	pout[0] = acc0/norm;
	pin += pin_incr;
	pout += _imat2_m1;
      }
      _idx2loopc1 += _imat1_m0 * nlin;
      _idx2loopc2 += _imat2_m0;
    }
    return out;
  }

  template <typename T> idx<T> image_subsample_rgb(idx<T> &in, int nlin, int ncol) {
    intg h = in.dim(0);
    intg w = in.dim(1);
    intg nh = h / nlin;
    intg nw = w / ncol;
    idx<T> out(nh, nw, in.dim(2));
    if ((nlin == 1) && (ncol == 1)) {
      idx_copy(in, out);
      return out;
    }
    idx<T> inp = in.narrow(0, nlin * nh, 0).narrow(1, ncol * nw, 0);
    T *in_line, *pin;
    T *out_line, *pout;
    int i, _imax = out.dim(0);
    int j, _jmax = out.dim(1);
    int in_mod0 = inp.mod(0);
    int in_mod1 = inp.mod(1);
    int out_mod0 = out.mod(0);
    int out_mod1 = out.mod(1);
    int pin_incr = ncol * in_mod1;
    in_line = inp.idx_ptr();
    out_line = out.idx_ptr();
    for (i = 0; i < _imax; i++) {
      pin = in_line;
      pout = out_line;
      for (j = 0; j < _jmax; j++) {
	BLK_AVRG(nlin, ncol);
	pin += pin_incr;
	pout += out_mod1;
      }
      in_line += in_mod0 * nlin;
      out_line += out_mod0;
    }
    return out;
  }

  template <typename T> idx<T> image_subsample(idx<T> &in, int nlin, int ncol) {
    switch (in.order()) {
    case 2:
      return image_subsample_grayscale(in, nlin, ncol);
    case 3:
      return image_subsample_rgb(in, nlin, ncol);
    default:
      eblerror("image must have at least an order of 2.");
      return in;
    }
  }

  template <typename T>
  void image_warp_quad(idx<T> &in, idx<T> &out,
		       idx<T> &background, int mode,
		       float x1, float y1, float x2, float y2,
		       float x3, float y3, float x4, float y4,
		       float p1, float q1, float p3, float q3) {
    intg outi = out.dim(0);
    intg outj = out.dim(1);
    idx<int> dispi(outi, outj);
    idx<int> dispj(outi, outj);
    compute_bilin_transform<float>(dispi, dispj, x1, y1, x2, y2, x3, y3,
				   x4, y4, p1, q1, p3, q3);
    if (0 == mode)
      image_warp_fast(in, out, background.idx_ptr(), dispi, dispj);
    else
      image_warp(in, out, background, dispi, dispj);
  }


  template <typename T>
  void image_warp(idx<T> &in, idx<T> &out, idx<T> &background,
		  idx<int> &pi, idx<int> &pj) {
    T* pin = in.idx_ptr();
    int indimi = in.dim(0);
    int indimj = in.dim(1);
    int inmodi = in.mod(0);
    int inmodj = in.mod(1);
    int ppi, ppj;
    { idx_bloop3(lout, out, T, lpi, pi, int, lpj, pj, int) {
  	{ idx_bloop3(llout, lout, T, llpi, lpi, int, llpj, lpj, int) {
	    ppi = llpi.get();
	    ppj = llpj.get();
	    image_interpolate_bilin(background.idx_ptr(), pin, indimi, indimj,
				    inmodi, inmodj, ppi, ppj, llout.idx_ptr(),
				    (int) out.dim(2));
	  }}
      }}
  }

  template <typename T>
  void image_warp_fast(idx<T> &in, idx<T> &out, T *background,
		       idx<int> &pi, idx<int> &pj) {
    T* pin = in.idx_ptr();
    int indimi = in.dim(0);
    int indimj = in.dim(1);
    int inmodi = in.mod(0);
    int inmodj = in.mod(1);
    int ppi, ppj;
    register int li, lj;
    register T *inn, *outt;
    int outsize = out.dim(2);
    { idx_bloop3(lout, out, T, lpi, pi, int, lpj, pj, int) {
	{ idx_bloop3(llout, lout, T, llpi, lpi, int, llpj, lpj, int) {
	    outt = llout.idx_ptr();
	    ppi = llpi.get();
	    ppj = llpj.get();
	    li = (ppi+0x7f) >> 16;
	    lj = (ppj+0x7f) >> 16;
	    if ((li>=0) && (li<indimi) && (lj>=0) && (lj<indimj)) {
	      inn = (T*)(pin) + inmodi * li + inmodj * lj;
	      outt[0] = inn[0];
	      if (outsize >= 3) {
		outt[1] = inn[1];
		outt[2] = inn[2];
	      }
	      if (outsize >= 4)
		outt[3] = inn[3];
	    } else {
	      outt[0] = *(background);
	      if (outsize >= 3) {
		outt[1] = *(background+1);
		outt[2] = *(background+2);
	      }
	      if (outsize >= 4)
		outt[3] = *(background+3);
	    }
	  }}
      }}
  }

  template <typename T>
  void image_warp_flow(idx<T> &src, idx<T> &dst, idx<float> &flow,
  		       bool bilinear, bool use_bg, T background) {
    // dims
    int width = src.dim(1);
    int height = src.dim(0);
    int channels = src.dim(2);
    intg *is = src.mod_ptr();
    intg *os = dst.mod_ptr();
    intg *fs = flow.mod_ptr();

    // get raw pointers
    T *dst_data = dst.idx_ptr();
    T *src_data = src.idx_ptr();
    float *flow_data = flow.idx_ptr();

    // resample
    intg k,x,y;
    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
  	// subpixel position:
  	float flow_y = flow_data[ 0*fs[0] + y*fs[1] + x*fs[2] ];
  	float flow_x = flow_data[ 1*fs[0] + y*fs[1] + x*fs[2] ];
  	float iy = y + flow_y;
  	float ix = x + flow_x;

  	// when going beyond image's boundaries, use background value
  	if (use_bg && (ix < 0 || ix >= width || iy < 0 || iy >= height)) {
  	  for (k = 0; k < channels; k++)
  	    dst_data[ y*os[0] + x*os[1] + k*os[2] ] = background;
  	  continue ;
  	}

  	// borders
  	ix = std::max(ix,(float)0);
  	ix = std::min(ix,(float)(width-1));
  	iy = std::max(iy,(float)0);
  	iy = std::min(iy,(float)(height-1));

  	// bilinear?
  	if (bilinear) {
  	  // 4 nearest neighbors:
  	  int ix_nw = (int) floor(ix);
  	  int iy_nw = (int) floor(iy);
  	  int ix_ne = ix_nw + 1;
  	  int iy_ne = iy_nw;
  	  int ix_sw = ix_nw;
  	  int iy_sw = iy_nw + 1;
  	  int ix_se = ix_nw + 1;
  	  int iy_se = iy_nw + 1;

  	  // get surfaces to each neighbor:
  	  float nw = ((float)(ix_se-ix))*(iy_se-iy);
  	  float ne = ((float)(ix-ix_sw))*(iy_sw-iy);
  	  float sw = ((float)(ix_ne-ix))*(iy-iy_ne);
  	  float se = ((float)(ix-ix_nw))*(iy-iy_nw);

  	  // weighted sum of neighbors:
  	  for (k=0; k<channels; k++) {
  	    dst_data[ k*os[2] + y*os[0] + x*os[1] ] =
              src_data[ k*is[2] + iy_nw*is[0] + ix_nw*is[1] ] * nw
  	      + src_data[ k*is[2] + iy_ne*is[0]
  			  + std::min(ix_ne,width-1)*is[1]] * ne
  	      + src_data[ k*is[2] + std::min(iy_sw,height-1)*is[0]
  			  + ix_sw*is[1] ] * sw
  	      + src_data[ k*is[2] + std::min(iy_se,height-1)*is[0]
  			  + std::min(ix_se,width-1)*is[1] ] * se;
  	  }
  	} else {
  	  // 1 nearest neighbor:
  	  int ix_n = (int) floor(ix+0.5);
  	  int iy_n = (int) floor(iy+0.5);

  	  // weighted sum of neighbors:
  	  for (k = 0; k < channels; k++)
  	    dst_data[ y*os[0] + x*os[1] + k*os[2] ] =
  	      src_data[ iy_n*is[0] + ix_n*is[1] + k*is[2] ];
  	}
      }
    }
  }

  template <typename T>
  void image_interpolate_bilin(T* background, T *pin, int indimi, int indimj,
			       int inmodi, int inmodj, int ppi, int ppj,
			       T* out, int outsize) {
    int li0, lj0;
    register int li1, lj1;
    int deltai, ndeltai;
    int deltaj, ndeltaj;
    register T *pin00;
    register T *v00, *v01, *v10, *v11;
    li0 = ppi >> 16;
    li1 = li0+1;
    deltai = ppi & 0x0000ffff;
    ndeltai = 0x00010000 - deltai;
    lj0 = ppj  >> 16;
    lj1 = lj0+1;
    deltaj = ppj & 0x0000ffff;
    ndeltaj = 0x00010000 - deltaj;
    pin00 = (T*)(pin) + inmodi * li0 + inmodj * lj0;
    if ((li1>0)&&(li1<indimi)) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==0) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==indimi) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = background;
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else {
      v00 = background;
      v01 = background;
      v11 = background;
      v10 = background;
    }
    // TODO: this does not work for ubyte (cf ubyte implementation in image.cpp)
    // make it generic to avoid code redondancy.
    double dd = 1.0 / 65536.0;
    T d = (T) dd;
    if (sizeof (T) <= 2)
      d = 0;
    if (outsize >= 1)
      *out = (ndeltaj * (( *v10*deltai + *v00*ndeltai )*d) +
	      deltaj  * (( *v11*deltai + *v01*ndeltai )*d))*d;
    if (outsize >= 2)
      *(out + 1) = (ndeltaj * ((v10[1]*deltai + v00[1]*ndeltai) * d) +
		    deltaj  * ((v11[1]*deltai + v01[1]*ndeltai) * d)) * d;
    if (outsize >= 3)
      *(out + 2) = (ndeltaj * ((v10[2]*deltai + v00[2]*ndeltai) * d) +
		    deltaj  * ((v11[2]*deltai + v01[2]*ndeltai) * d)) * d;
    if (outsize >= 4)
      *(out + 3) = (ndeltaj * ((v10[3]*deltai + v00[3]*ndeltai) * d) +
		    deltaj  * ((v11[3]*deltai + v01[3]*ndeltai) * d)) * d;
    if (outsize >= 5) {
      eblerror("not implemented for more than 4 channels ("
	       << outsize << " channels in " << indimi << "x" << indimj
	       << " image)");
    }
  }

  template <typename T> void compute_bilin_transform(idx<int> &dispi,
						 idx<int> &dispj,
						 float x1, float y1, float x2,
						 float y2, float x3, float y3,
						 float x4, float y4, float p1,
						 float q1, float p3, float q3) {
    // compute transformation matrix from coordinates
    // in target (rectangular) space to coordinates
    // in original (irregular quadrilateral) image
    // transformation matrix is in 16.16 fixed point format.
    float k = 65536 / ((p3 - p1) * (q3 - q1));
    float x41 = x4 - x1;
    float x21 = x2 - x1;
    float x43 = x4 - x3;
    float x23 = x2 - x3;
    int mx0 = (int) (k * ((q3 * x21) + (q1 * x43)));
    int mx1 = (int) (k * ((p3 * x41) + (p1 * x23)));
    int mx2 = (int) ((-k) * (x41 + x23));
    int mx3 = (int) (k * (((p3 * q3 * x1) + (p1 * q1 * x3)) -
			  ((p1 * q3 * x2) + (p3 * q1 * x4))));
    float y41 = y4 - y1;
    float y21 = y2 - y1;
    float y43 = y4 - y3;
    float y23 = y2 - y3;
    int my0 = (int) (k * ((q3 * y21) + (q1 * y43)));
    int my1 = (int) (k * ((p3 * y41) + (p1 * y23)));
    int my2 = (int) ((-k) * (y41 + y23));
    int my3 = (int) (k * (((p3 * q3 * y1) + (p1 * q1 * y3)) -
			  ((p1 * q3 * y2) + (p3 * q1 * y4))));
    int q = 0, p = 0;
    { idx_bloop2(ispi, dispi, int, ispj, dispj, int) {
	p = 0;
	{ idx_bloop2(di, ispi, int, dj, ispj, int) {
	    di.set(my0 * p + my1 * q + my2 * p * q + my3);
	    dj.set(mx0 * p + mx1 * q + mx2 * p * q + mx3);
	    p++;
	  }}
	q++;
      }}
  }


//   template <typename T> void compute_bilin_transform2(idx<int> &dispi,
// 						  idx<int> &dispj,
// 						  float x1, float y1,
// 						  float x2, float y2,
// 						  float x3, float y3,
// 						  float x4, float y4,
// 						  float p1, float q1,
// 						  float p2, float q2,
// 						  float p3, float q3,
// 						  float p4, float q4) {
//     // compute transformation matrix from coordinates
//     // in target (rectangular) space to coordinates
//     // in original (irregular quadrilateral) image
//     // transformation matrix is in 16.16 fixed point format.
//     float k = 65536 / ((p3 - p1) * (q3 - q1));
//     float x41 = x4 - x1;
//     float x21 = x2 - x1;
//     float x43 = x4 - x3;
//     float x23 = x2 - x3;
//     int mx0 = (int) (k * ((q3 * x21) + (q1 * x43)));
//     int mx1 = (int) (k * ((p3 * x41) + (p1 * x23)));
//     int mx2 = (int) ((-k) * (x41 + x23));
//     int mx3 = (int) (k * (((p3 * q3 * x1) + (p1 * q1 * x3)) -
// 			  ((p1 * q3 * x2) + (p3 * q1 * x4))));
//     float y41 = y4 - y1;
//     float y21 = y2 - y1;
//     float y43 = y4 - y3;
//     float y23 = y2 - y3;
//     int my0 = (int) (k * ((q3 * y21) + (q1 * y43)));
//     int my1 = (int) (k * ((p3 * y41) + (p1 * y23)));
//     int my2 = (int) ((-k) * (y41 + y23));
//     int my3 = (int) (k * (((p3 * q3 * y1) + (p1 * q1 * y3)) -
// 			  ((p1 * q3 * y2) + (p3 * q1 * y4))));
//     int q = 0, p = 0;
//     { idx_bloop2(ispi, dispi, int, ispj, dispj, int) {
// 	p = 0;
// 	{ idx_bloop2(di, ispi, int, dj, ispj, int) {
// 	    di.set(my0 * p + my1 * q + my2 * p * q + my3);
// 	    dj.set(mx0 * p + mx1 * q + mx2 * p * q + mx3);
// 	    p++;
// 	  }}
// 	q++;
//       }}
//   }

  template <typename T>
  idx<T> image_rotscale(idx<T> &src, double sx, double sy, double dx,
			double dy, double angle, double coeff, T bg_val) {
    if (!src.contiguousp())
      eblerror("image must be contiguous");
    double q = 1000;
    double coeff_inv = 1/coeff;
    double sa = q*sin(angle * 0.017453292);
    double ca = q*cos(angle * 0.017453292);
    double ca_plus_sa = coeff_inv * (sa + ca);
    double ca_minus_sa = coeff_inv * (ca - sa);
    float x1 = sy - ca_plus_sa;
    float y1 = sx - ca_minus_sa;
    float x2 = sy + ca_minus_sa;
    float y2 = sx - ca_plus_sa;
    float x3 = sy + ca_plus_sa;
    float y3 = sx + ca_minus_sa;
    float x4 = sy - ca_minus_sa;
    float y4 = sx + ca_plus_sa;
    float p1 = dy - q;
    float q1 = dx - q;
    float p3 = dy + q;
    float q3 = dx + q;
    idx<T> out(src.get_idxdim());
    idx<T> bg(src.dim(0));
    idx_fill(bg, bg_val);
    image_warp_quad(src, out, bg, 1, x1, y1, x2, y2, x3, y3, x4, y4,
		    p1, q1, p3, q3);
    return out;
  }

  template <typename T>
  idx<T> image_rotate(idx<T> &src, double angle, float ch, float cw, T bg_val) {
    if (!src.contiguousp())
      eblerror("image must be contiguous");
    if (ch == -1) ch = src.dim(0) / (float) 2;
    if (cw == -1) cw = src.dim(1) / (float) 2;
    idx<T> rotated(src.get_idxdim());
    double q = 1000;
    double sa = q*sin(angle * 0.017453292);
    double ca = q*cos(angle * 0.017453292);
    double ca_plus_sa = sa + ca;
    double ca_minus_sa = ca - sa;
    float x1 = cw - ca_plus_sa;
    float y1 = ch - ca_minus_sa;
    float x2 = cw + ca_minus_sa;
    float y2 = ch - ca_plus_sa;
    float x3 = cw + ca_plus_sa;
    float y3 = ch + ca_minus_sa;
    float x4 = cw - ca_minus_sa;
    float y4 = ch + ca_plus_sa;
    float p1 = cw - q;
    float q1 = ch - q;
    float p3 = cw + q;
    float q3 = ch + q;
    idx<T> bg(src.dim(0));
    idx_fill(bg, bg_val);
    image_warp_quad(src, rotated, bg, 1, x1, y1, x2, y2, x3, y3, x4, y4,
		    p1, q1, p3, q3);
    return rotated;
  }

  ////////////////////////////////////////////////////////////////
  // Drawing

  template <typename T> void image_draw_box(idx<T> &img, T val,
					uint x, uint y, uint dx, uint dy) {
    idx_checkorder1(img, 2);
    for (unsigned int i = x; i < x + dx; ++i) {
      img.set(val, i, y);
      img.set(val, i, y + dy);
    }
    for (unsigned int j = y; j < y + dy; ++j) {
      img.set(val, x, j);
      img.set(val, x + dx, j);
    }
  }

  ////////////////////////////////////////////////////////////////
  // Filters

  template <typename T>
  void image_mexican_filter(idx<T> &in, idx<T> &out, double s, int n,
			    idx<T> *filter_, idx<T> *tmp_) {
    idx<T> filter = filter_ ? *filter_ : create_mexican_hat<T>(s, n);
    idxdim d(in);
    idx<T> tmp = tmp_ ? *tmp_ : idx<T>(d);
    idx_checkorder3(in, 2, filter, 2, out, 2);
    image_apply_filter(in, out, filter, &tmp);
  }

  // TODO: handle empty sides
  // TODO: check for tmp size incompatibilities
  template <typename T>
  void image_global_normalization(idx<T> &in) {
    idx_std_normalize(in, in, (T *)NULL);
    // problem with code below: it separates by channels which are assumed to
    // be in last dimension. this can lead to nasty bugs.
    // switch (in.order()) {
    // case 2:
    //   idx_std_normalize(in, in); // zero-mean and divide by standard deviation
    //   break ;
    // case 3:
    //   // normalize layer by layer
    //   { idx_eloop1(i, in, T) {
    // 	idx_std_normalize(i, i); // zero-mean and divide by standard deviation
    // 	}}
    //   break ;
    // default:
    //   eblerror("image_global_normalization: dimension not implemented");
    // }
  }

  // TODO: handle empty sides
  // TODO: check for tmp size incompatibilities
  // TODO: cleanup
  template <typename T>
  void image_local_normalization(idx<T> &in, idx<T> &out, int n) {
    if (in.order() != 2 || out.order() != 2)
      eblerror("this function only accepts 2D inputs, but got " << in
	       << " and " << out);
    // 1. create normalized gaussian kernel (kernel / sum(kernel))
    idx<T> kernel = create_gaussian_kernel<T>(n);
    idx<T> tmp(in.dim(0) + n - 1, in.dim(1) + n - 1);
    idx<T> tmp2 = tmp.narrow(0, in.dim(0), (intg) floor((float) n / 2));
    tmp2 = tmp2.narrow(1, in.dim(1), (intg) floor((float) n / 2));
    idx<T> tmp3(n, n);
    idxdim d(in);
    idx<T> tmp4(d);
    idx<T> tmp5(d);

    // sum_j (w_j * in_j)
    idx<T> out1 = image_filter(in, kernel);
    idx<T> in2 = in.narrow(0, out1.dim(0), (intg)floor((float)kernel.dim(0)/2));
    in2 = in2.narrow(1, out1.dim(1), (intg) floor((float)kernel.dim(1)/2));
    idxdim outd(out1);
    idx<T> out2(outd);
    // in - mean
    idx_sub(in2, out1, out1);
    // (in - mean)^2
    idx_mul(out1, out1, out2);
    // sum_j (w_j * (in - mean)^2)
    idx<T> out3 = image_filter(out2, kernel);
    // sqrt(sum_j (w_j (in - mean)^2))
    idx_sqrt(out3, out3);
    // std(std < 1) = 1
    idx_threshold(out3, (T)1.0);
    // 1/std
    idx_inv(out3, out3);
    // out = (in - mean) / std
    idx<T> out4 = out1.narrow(0, out3.dim(0),
			      (intg) floor((float)kernel.dim(0)/2));
    out4 = out4.narrow(1, out3.dim(1), (intg) floor((float)kernel.dim(1)/2));
    idx_mul(out4, out3, out3);
    // finally copy result centered on an image the same size as in
    idx<T> out5 = out.narrow(0, out3.dim(0), (out.dim(0) - out3.dim(0)) / 2);
    out5 = out5.narrow(1, out3.dim(1), (out.dim(1) - out3.dim(1)) / 2);
    idx_clear(out);
    idx_copy(out3, out5);

//     // in - mean
//     idx_sub(in, tmp5, tmp5);
//     // (in - mean)^2
//     idx_mul(tmp5, tmp5, tmp4);
//     // sum_j (w_j * (in - mean)^2)
//     image_apply_filter(tmp4, out, kernel, &tmp);
//     // sqrt(sum_j (w_j (in - mean)^2))
//     idx_sqrt(out, out);
//     // std(std < 1) = 1
//     idx_threshold(out, (T)1.0, out);
//     // 1/std
//     idx_inv(out, out);
//     // out = (in - mean) / std
//     idx_mul(tmp5, out, out);
  }

  // TODO: get rid of this function
  template <typename T>
  void image_apply_filter(idx<T> &in, idx<T> &out, idx<T> &filter,
			  idx<T> *tmp_) {
    idxdim d(in);
    if ((out.dim(0) != d.dim(0)) || (out.dim(1) != d.dim(1)))
      out.resize(d);
    idx_clear(out);
    idx<T> tmp = out.narrow(0, in.dim(0) - filter.dim(0) + 1,
			    (intg) floor((float)filter.dim(0)/2));
    tmp = tmp.narrow(1, in.dim(1) - filter.dim(1) + 1,
		     (intg) floor((float)filter.dim(1)/2));
    idx_2dconvol(in, filter, tmp);

//     // compute sizes of the temporary buffer
//     d.setdim(0, in.dim(0) + filter.dim(0) - 1);
//     d.setdim(1, in.dim(1) + filter.dim(1) - 1);
//     idx<T> tmp;
//     if (tmp_) {
//       tmp = *tmp_;
//       if ((tmp.dim(0) != d.dim(0)) || (tmp.dim(1) != d.dim(1))) {
// 	tmp.resize(d);
// 	*tmp_ = tmp;
//       }
//     } else
//       tmp = idx<T>(d);
//     // copy input into temporary buffer
//     idx_clear(tmp);
//     idx<T> tmp2 = tmp.narrow(0, in.dim(0), floor(filter.dim(0)/2));
//     tmp2 = tmp2.narrow(1, in.dim(1), floor(filter.dim(1)/2));
//     idx_copy(in, tmp2);
//     idx_2dconvol(tmp, filter, out);
  }

  template <typename T>
  idx<T> image_filter(idx<T> &in, idx<T> &filter) {
    // check that image is bigger than filter
    if ((in.dim(0) < filter.dim(0)) ||
	(in.dim(1) < filter.dim(1))) {
      eblwarn( "error: image " << in << " is too small to be convolved with "
               << filter << " filter." << std::endl);
      eblerror("too small image for convolution");
    }
    idxdim d(in);
    d.setdim(0, in.dim(0) - filter.dim(0) + 1);
    d.setdim(1, in.dim(1) - filter.dim(1) + 1);
    idx<T> out(d);
    idx_clear(out);
    idx_2dconvol(in, filter, out);
    return out;
  }

  // deformations //////////////////////////////////////////////////////////////

  template <typename T>
  void image_deformation_ranperspective(idx<T> &in, idx<T> &out,
					int hrange, int wrange, T background) {
    idx<float> bg(1);
    bg.set(background, 0);
    float x1diff = (float) drand( wrange/2, -wrange/2);
    float y1diff = (float) drand( hrange/2, -hrange/2);
    float x2diff = (float) drand(-wrange/2,  wrange/2);
    float y2diff = (float) drand( hrange/2, -hrange/2);
    float x3diff = (float) drand(-wrange/2,  wrange/2);
    float y3diff = (float) drand(-hrange/2,  hrange/2);
    float x4diff = (float) drand( wrange/2, -wrange/2);
    float y4diff = (float) drand(-hrange/2,  hrange/2);
    image_warp_quad(in, out, bg, 1,
		    -.5 + x1diff, -.5 + y1diff, // x1 y1
		    in.dim(1) -.5 + x2diff, -.5 + y2diff, // x2 y2
		    in.dim(1) -.5 + x3diff, in.dim(0) -.5 + y3diff, // x3 y3
		    -.5 + x4diff, in.dim(0) - .5 + y4diff, // x4 y4
		    -.5, -.5, in.dim(1) - .5, in.dim(0) - .5);
  }

  template <typename T>
  idx<float> image_deformation_flow(idx<T> &in, float th, float tw,
				    float sh, float sw, float deg,
				    float shh, float shw,
				    uint elsize, float elcoeff,
				    T background) {
    idxdim d(in);
    d.remove_dim(2);
    idx<float> grid = create_grid(d);
    idx<float> flow(grid.get_idxdim());
    idx<T> bg(in.dim(2));
    idx_fill(bg, background);
    idx_clear(flow);


    if (th != 0 || tw != 0) translation_flow(grid, flow, th, tw);
    if (deg != 0) rotation_flow(grid, flow, deg);
    if (th != 1 || tw != 1) scale_flow(grid, flow, sh, sw);
    if (shh != 0 || shw != 0) shear_flow(grid, flow, shh, shw);
    if (elsize > 0) elastic_flow(flow, elsize, elcoeff);

    // affine_flow(grid, flow, 0, 0, sh, sw, 0, 0, deg);
    return flow;
  }

  template <typename T>
  void image_deformation(idx<T> &in, idx<T> &out, float th, float tw,
			 float sh, float sw, float deg,
			 float shh, float shw, uint elsize, float elcoeff,
			 T background) {
    idx<float> flow =
      image_deformation_flow(in, th, tw, sh, sw, deg, shh, shw, elsize, elcoeff,
			     background);
    // apply flow
    image_warp_flow(in, out, flow);
  }

} // end namespace ebl

#endif /* IMAGE_HPP_ */
