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

#ifndef COLOR_SPACES_HPP_
#define COLOR_SPACES_HPP_

#include <math.h>

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // RGB -> YUV

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than yuv matrix.
  template<class T> void rgb_to_yuv_1D(idx<T> &rgb, idx<T> &yuv) {
    if (rgb.idx_ptr() == yuv.idx_ptr()) {
      eblerror("rgb_to_yuv: dst must be different than src");
      return ;
    }
    double r, g, b;
    r = rgb.get(0);
    g = rgb.get(1);
    b = rgb.get(2);
    yuv.set((T) (  0.299 * r + 0.587 * g + 0.114 * b), 0);
    yuv.set((T) ((-0.147 * r - 0.289 * g + 0.437 * b + 111) * 1.14678), 1);
    yuv.set((T) (( 0.615 * r - 0.515 * g - 0.100 * b + 157) * 0.81300), 2);
  }

  template<class T> void rgb_to_yuv(idx<T> &rgb, idx<T> &yuv) {
    idx_checknelems2_all(rgb, yuv);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_yuv_1D(rgb, yuv);
      //      idx_m2dotm1(rgb_yuv, rgb, yuv);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, yuv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      rgb_to_yuv_1D(r, y);
	      //	      idx_m2dotm1(rgb_yuv, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_yuv dimension not implemented");
    }
  }

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than y matrix.
  template<class T> void rgb_to_y_1D(idx<T> &rgb, idx<T> &y) {
    if (rgb.idx_ptr() == y.idx_ptr()) {
      eblerror("rgb_to_y: dst must be different than src");
      return ;
    }
    double r, g, b;
    r = rgb.get(0);
    g = rgb.get(1);
    b = rgb.get(2);
    y.set(  (T) 0.299 * (T) r + (T) 0.587 * (T) g + (T) 0.114 * (T) b, 0);
  }

  template<class T> void rgb_to_y(idx<T> &rgb, idx<T> &y) {
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_y_1D(rgb, y);
      //      idx_m2dotm1(rgb_yuv, rgb, yuv);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yy, y, T) {
	  { idx_bloop2(r, rg, T, yyy, yy, T) {
	      rgb_to_y_1D(r, yyy);
	      //	      idx_m2dotm1(rgb_yuv, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_y dimension not implemented");
    }
  }
  
  template<class T> idx<T> rgb_to_yuv(idx<T> &rgb) {
    idxdim d(rgb);
    idx<T> yuv(d);
    rgb_to_yuv(rgb, yuv);
    return yuv;
  }

  ////////////////////////////////////////////////////////////////
  // BGR -> YUV

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than yuv matrix.
  template<class T> void bgr_to_yuv_1D(idx<T> &bgr, idx<T> &yuv) {
    if (bgr.idx_ptr() == yuv.idx_ptr()) {
      eblerror("bgr_to_yuv: dst must be different than src");
      return ;
    }
    double r, g, b;
    b = bgr.get(0);
    g = bgr.get(1);
    r = bgr.get(2);
    yuv.set(  0.299 * r + 0.587 * g + 0.114 * b, 0);
    yuv.set((-0.147 * r - 0.289 * g + 0.437 * b + 111) * 1.14678, 1);
    yuv.set(( 0.615 * r - 0.515 * g - 0.100 * b + 157) * 0.81300, 2);
  }
  
  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than y matrix.
  template<class T> void bgr_to_y_1D(idx<T> &bgr, idx<T> &y) {
    if (bgr.idx_ptr() == y.idx_ptr()) {
      eblerror("bgr_to_y: dst must be different than src");
      return ;
    }
    double r, g, b;
    b = bgr.get(0);
    g = bgr.get(1);
    r = bgr.get(2);
    y.set(  0.299 * r + 0.587 * g + 0.114 * b, 0);
  }

  template<class T> void bgr_to_y(idx<T> &bgr, idx<T> &y) {
    switch (bgr.order()) {
    case 1: // process 1 pixel
      bgr_to_y_1D(bgr, y);
      return ;
    case 3: // process 2D image
      { idx_bloop2(bg, bgr, T, yy, y, T) {
	  { idx_bloop2(b, bg, T, yyy, yy, T) {
	      rgb_to_y_1D(b, yyy);
	    }}
	}}
      return ;
    default:
      eblerror("bgr_to_y dimension not implemented");
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // YUV -> RGB
  
  template<class T> void yuv_to_rgb_1D(idx<T> &yuv, idx<T> &rgb) {
    if (rgb.idx_ptr() == yuv.idx_ptr()) {
      eblerror("yuv_to_rgb: dst must be different than src");
      return ;
    }
    float y, u, v;
    y = std::min((float) 255, std::max((float) 0, 
                                       (float) ((float) 1.164383562 
                                                * (yuv.get(0) - 16))));
    u = std::min((float) 128, std::max((float) -127, 
                             (float) ((float) 1.133928571 
                                      * (yuv.get(1) - 128))));
    v = std::min((float) 128, std::max((float) -127, 
                             (float) ((float) 1.133928571 
                                      * (yuv.get(2) - 128))));
    rgb.set(std::min((T) 255, std::max((T) 0, 
                             (T) (1.0 * y + 0.00000 * u + 1.402 * v))), 0);
    rgb.set(std::min((T) 255, std::max((T) 0, 
                             (T) (1.0 * y - 0.344 * u - 0.714 * v))), 1);
    rgb.set(std::min((T) 255, std::max((T) 0, 
                             (T) (1.0 * y + 1.772 * u + 0.00000 * v))), 2);
  }

  template<class T> void yuv_to_rgb(idx<T> &yuv, idx<T> &rgb) {
    idx_checknelems2_all(rgb, yuv);
    switch (yuv.order()) {
    case 1: // process 1 pixel
      yuv_to_rgb_1D(yuv, rgb);
      //idx_m2dotm1(yuv_rgb, yuv, rgb);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, yuv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      yuv_to_rgb_1D(y, r);
	      //idx_m2dotm1(yuv_rgb, y, r);
	    }}
	}}
      return ;
    default:
      eblerror("yuv_to_rgb dimension not implemented");
    }
  }

  template<class T> idx<T> yuv_to_rgb(idx<T> &yuv) {
    idxdim d(yuv);
    idx<T> rgb(d);
    yuv_to_rgb(yuv, rgb);
    return rgb;
  }

  ////////////////////////////////////////////////////////////////
  // HSV

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than hsv matrix.
  template<class T> void rgb_to_hsv_1D(idx<T> &rgb, idx<T> &hsv) {
    if (rgb.idx_ptr() == hsv.idx_ptr()) {
      eblerror("rgb_to_hsv: dst must be different than src");
      return ;
    }
    double r, g, b;
    r = rgb.get(0);
    g = rgb.get(1);
    b = rgb.get(2);
    double h, s, v;
    PIX_RGB_TO_HSV_COMMON(r, g, b, h, s, v, false);
    hsv.set((T) h, 0);
    hsv.set((T) s, 1);
    hsv.set((T) v, 2);
  }

  template<class T> void rgb_to_h_1D(idx<T> &rgb, idx<T> &h) {
    if (rgb.idx_ptr() == h.idx_ptr()) {
      eblerror("rgb_to_h: dst must be different than src");
      return ;
    }
    double r, g, b;
    r = rgb.get(0);
    g = rgb.get(1);
    b = rgb.get(2);
    double h_, s_, v_;
    PIX_RGB_TO_HSV_COMMON(r, g, b, h_, s_, v_, false);
    h.set((T) h_, 0);
  }

  template<class T> void rgb_to_hsv(idx<T> &rgb, idx<T> &hsv) {
    idx_checknelems2_all(rgb, hsv);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_hsv_1D(rgb, hsv);
      //      idx_m2dotm1(rgb_hsv, rgb, hsv);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, hsv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      rgb_to_hsv_1D(r, y);
	      //	      idx_m2dotm1(rgb_hsv, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_hsv dimension not implemented");
    }
  }

  template<class T> idx<T> rgb_to_hsv(idx<T> &rgb) {
    idxdim d(rgb);
    idx<T> hsv(d);
    rgb_to_hsv(rgb, hsv);
    return hsv;
  }
  
  template<class T> void hsv_to_rgb_1D(idx<T> &hsv, idx<T> &rgb) {
    if (rgb.idx_ptr() == hsv.idx_ptr()) {
      eblerror("hsv_to_rgb: dst must be different than src");
      return ;
    }
    double h, s, v;
    h = hsv.get(0);
    s = hsv.get(1);
    v = hsv.get(2);
    double r, g, b;
    PIX_HSV_TO_RGB_COMMON(h, s, v, r, g, b);
    rgb.set((T)r, 0);
    rgb.set((T)g, 1);
    rgb.set((T)b, 2);
  }

  template<class T> void hsv_to_rgb(idx<T> &hsv, idx<T> &rgb) {
    idx_checknelems2_all(rgb, hsv);
    switch (hsv.order()) {
    case 1: // process 1 pixel
      hsv_to_rgb_1D(hsv, rgb);
      //idx_m2dotm1(hsv_rgb, hsv, rgb);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, hsv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      hsv_to_rgb_1D(y, r);
	      //idx_m2dotm1(hsv_rgb, y, r);
	    }}
	}}
      return ;
    default:
      eblerror("hsv_to_rgb dimension not implemented");
    }
  }

  template<class T> idx<T> hsv_to_rgb(idx<T> &hsv) {
    idxdim d(hsv);
    idx<T> rgb(d);
    hsv_to_rgb(hsv, rgb);
    return rgb;
  }

  ////////////////////////////////////////////////////////////////
  // HSV3

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than hsv3 matrix.
  template<class T> void rgb_to_hsv3_1D(idx<T> &rgb, idx<T> &hsv3,
					double threshold1, double threshold2) {
    if (rgb.idx_ptr() == hsv3.idx_ptr()) {
      eblerror("rgb_to_hsv3: dst must be different than src");
      return ;
    }
    double r, g, b;
    r = rgb.get(0);
    g = rgb.get(1);
    b = rgb.get(2);
    double h, s, v;
    PIX_RGB_TO_HSV_COMMON(r, g, b, h, s, v, false);
    if ((s < threshold1) || (v < threshold2)) {
      h = 360 + 60 * v;
    }
    hsv3.set((T) h, 0);
    hsv3.set((T) s, 1);
    hsv3.set((T) v, 2);
  }

  template<class T> void rgb_to_hsv3(idx<T> &rgb, idx<T> &hsv3,
				     double threshold1, double threshold2) {
    idx_checknelems2_all(rgb, hsv3);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_hsv3_1D(rgb, hsv3, threshold1, threshold2);
      //      idx_m2dotm1(rgb_hsv3, rgb, hsv3);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, hsv3, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      rgb_to_hsv3_1D(r, y, threshold1, threshold2);
	      //	      idx_m2dotm1(rgb_hsv3, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_hsv3 dimension not implemented");
    }
  }

  template<class T> idx<T> rgb_to_hsv3(idx<T> &rgb, double threshold1,
				       double threshold2) {
    idxdim d(rgb);
    idx<T> hsv3(d);
    rgb_to_hsv3(rgb, hsv3, threshold1, threshold2);
    return hsv3;
  }
  
  template<class T> void hsv3_to_rgb_1D(idx<T> &hsv3, idx<T> &rgb) {
    if (rgb.idx_ptr() == hsv3.idx_ptr()) {
      eblerror("hsv3_to_rgb: dst must be different than src");
      return ;
    }
    double h, s, v;
    h = hsv3.get(0);
    s = hsv3.get(1);
    v = hsv3.get(2);
    double r, g, b;
    PIX_HSV3_TO_RGB_COMMON(h, s, v, r, g, b);
    rgb.set((T)r, 0);
    rgb.set((T)g, 1);
    rgb.set((T)b, 2);
  }

  template<class T> void hsv3_to_rgb(idx<T> &hsv3, idx<T> &rgb) {
    idx_checknelems2_all(rgb, hsv3);
    switch (hsv3.order()) {
    case 1: // process 1 pixel
      hsv3_to_rgb_1D(hsv3, rgb);
      //idx_m2dotm1(hsv3_rgb, hsv3, rgb);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, hsv3, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      hsv3_to_rgb_1D(y, r);
	      //idx_m2dotm1(hsv3_rgb, y, r);
	    }}
	}}
      return ;
    default:
      eblerror("hsv3_to_rgb dimension not implemented");
    }
  }

  template<class T> idx<T> hsv3_to_rgb(idx<T> &hsv3) {
    idxdim d(hsv3);
    idx<T> rgb(d);
    hsv3_to_rgb(hsv3, rgb);
    return rgb;
  }
  
  ////////////////////////////////////////////////////////////////
  // YH3

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than yh3 matrix.
  template<class T> void rgb_to_yh3_1D(idx<T> &rgb, idx<T> &yh3,
					double threshold1, double threshold2) {
    if (rgb.idx_ptr() == yh3.idx_ptr()) {
      eblerror("rgb_to_yh3: dst must be different than src");
      return ;
    }
    idx<T> hsv3(3);
    rgb_to_hsv3_1D(rgb, hsv3, threshold1, threshold2);
    yh3.set((0.299 * rgb.get(0) + 0.587 * rgb.get(1) + 0.114 * rgb.get(2))
	    /255.0, 0);
    yh3.set(hsv3.get(0) / 210.0 - 1, 1);
  }

  template<class T> void rgb_to_yh3(idx<T> &rgb, idx<T> &yh3,
				     double threshold1, double threshold2) {
    //    idx_checknelems2_all(rgb, yh3);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_yh3_1D(rgb, yh3, threshold1, threshold2);
      //      idx_m2dotm1(rgb_yh3, rgb, yh3);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, yh3, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      rgb_to_yh3_1D(r, y, threshold1, threshold2);
	      //	      idx_m2dotm1(rgb_yh3, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_yh3 dimension not implemented");
    }
  }

  template<class T> idx<T> rgb_to_yh3(idx<T> &rgb, double threshold1,
				       double threshold2) {
    idxdim d(rgb);
    idx<T> yh3(d);
    rgb_to_yh3(rgb, yh3, threshold1, threshold2);
    return yh3;
  }

  template<class T> void h3_to_rgb_1D(idx<T> &h3, idx<T> &rgb) {
    if (rgb.idx_ptr() == h3.idx_ptr()) {
      eblerror("h3_to_rgb: dst must be different than src");
      return ;
    }
    double r, g, b;
    PIX_HSV3_TO_RGB_COMMON(h3.get(), .5, .5, r, g, b);
    rgb.set((T)r, 0);
    rgb.set((T)g, 1);
    rgb.set((T)b, 2);
  }

  template<class T> void h3_to_rgb(idx<T> &h3, idx<T> &rgb) {
    //    idx_checknelems2_all(rgb, h3);
    switch (h3.order()) {
    case 2: // process 1 pixel
//       h3_to_rgb_1D(h3, rgb);
//       //idx_m2dotm1(h3_rgb, h3, rgb);
//       return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, h3, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      h3_to_rgb_1D(y, r);
	      //idx_m2dotm1(h3_rgb, y, r);
	    }}
	}}
      return ;
    default:
      eblerror("h3_to_rgb dimension not implemented");
    }
  }

  template<class T> idx<T> h3_to_rgb(idx<T> &h3) {
    idxdim d(h3);
    idx<T> rgb(d);
    h3_to_rgb(h3, rgb);
    return rgb;
  }  
  
  ////////////////////////////////////////////////////////////////
  // H2sv

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than h2sv matrix.
  template<class T> void rgb_to_h2sv_1D(idx<T> &rgb, idx<T> &h2sv) {
    if (rgb.idx_ptr() == h2sv.idx_ptr()) {
      eblerror("rgb_to_h2sv: dst must be different than src");
      return ;
    }
    double r, g, b;
    r = rgb.get(0);
    g = rgb.get(1);
    b = rgb.get(2);
    double h, s, v, h1, h2;
    PIX_RGB_TO_HSV_COMMON(r, g, b, h, s, v, false);
    PIX_HSV_TO_H2SV1_COMMON(h, h1, h2);
    h2sv.set((T)h1, 0);
    h2sv.set((T)h2, 1);
    h2sv.set((T)s, 2);
    h2sv.set((T)v, 3);
  }

  template<class T> void rgb_to_h2sv(idx<T> &rgb, idx<T> &h2sv) {
    //    idx_checknelems2_all(rgb, h2sv);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_h2sv_1D(rgb, h2sv);
      //      idx_m2dotm1(rgb_h2sv, rgb, h2sv);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, h2sv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      rgb_to_h2sv_1D(r, y);
	      //	      idx_m2dotm1(rgb_h2sv, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_h2sv dimension not implemented");
    }
  }

  ////////////////////////////////////////////////////////////////
  // VpH2SV

  template<class T> void rgb_to_vph2sv(idx<T> &rgb, idx<T> &vph2sv,
				       double s, int n) {
    // TODO: check dimensions, dimension where channels are
    idx<T> h2sv = vph2sv.narrow(2, vph2sv.dim(2) - 1, 1);
    idx<T> vp = vph2sv.select(2, 0);
    idx<T> v = vph2sv.select(2, 4);
    idxdim d(vp);
    idx<T> tmp(d);
    rgb_to_h2sv(rgb, h2sv);
    idx_copy(v, tmp);
    //    image_mexican_filter(tmp, vp, s, n);
    image_global_normalization(tmp);
    image_local_normalization(tmp, vp, n);
    //    vph2sv = vph2sv.narrow(0, vph2sv.dim(0) - n + 1, floor(n / 2));
    //    vph2sv = vph2sv.narrow(1, vph2sv.dim(1) - n + 1, floor(n / 2));
  }

} // end namespace ebl

#endif /* COLOR_SPACES_HPP_ */
