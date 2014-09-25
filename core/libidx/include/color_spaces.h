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

#ifndef COLOR_SPACES_H_
#define COLOR_SPACES_H_

#include "defines.h"
#include "idx.h"

namespace ebl {

  extern idx<double> rgb_yuv;
  extern idx<double> yuv_rgb;

  ////////////////////////////////////////////////////////////////
  // RGB -> YUV

  //! Convert all pixels of rgb idx to yuv pixels (normalized, i.e. the range
  //! is the same for each component: 0 .. 255).
  //! WARNING: we assume the values are 0..255.
  //! TODO: make this more generic.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  //! The output y is expected to be allocated with the correct size.
  template<class T>
    void rgb_to_yuv(idx<T> &rgb, idx<T> &yuv);

  //! RGB to YUV, on a 1-dimensional idx rgb.
  //! The output y is expected to be allocated with the correct size.
  template<class T>
    void rgb_to_yuv_1D(idx<T> &rgb, idx<T> &yuv);

  //! Same as yuv_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T>
    idx<T> rgb_to_yuv(idx<T> &rgb);

  //! RGB to Y, looping on the 3rd dimension if present, calling rgb_to_y_1D
  //! otherwise.
  //! The output y is expected to be allocated with the correct size.
  template<class T>
    void rgb_to_y(idx<T> &rgb, idx<T> &y);

  //! RGB to Y, on a 1-dimensional idx rgb.
  //! The output y is expected to be allocated with the correct size.
  template<class T>
    void rgb_to_y_1D(idx<T> &rgb, idx<T> &y);

  ////////////////////////////////////////////////////////////////
  // BGR -> YUV

  //! BGR to Y, looping on the 3rd dimension if present, calling bgr_to_y_1D
  //! otherwise.
  //! The output y is expected to be allocated with the correct size.
  template<class T>
    void bgr_to_y(idx<T> &bgr, idx<T> &y);

  //! BGR to YUV, on a 1-dimensional idx bgr.
  //! The output y is expected to be allocated with the correct size.
  template<class T>
    void bgr_to_yuv_1D(idx<T> &bgr, idx<T> &yuv);

  //! BGR to Y, on a 1-dimensional idx bgr.
  //! The output y is expected to be allocated with the correct size.
  template<class T>
    void bgr_to_y_1D(idx<T> &bgr, idx<T> &y);

  ////////////////////////////////////////////////////////////////
  // YUV -> RGB

  //! Convert all pixels of yuv idx to rgb pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T>
    void yuv_to_rgb(idx<T> &yuv, idx<T> &rgb);

  //! Same as yuv_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T>
    idx<T> yuv_to_rgb(idx<T> &yuv);

  //! Normalize a YUV image (wxhx3), centered between on [-2.5 .. 2.5]
  void YUVGlobalNormalization(idx<float> &yuv);

  ////////////////////////////////////////////////////////////////
  // HSV

  EXPORT void PIX_RGB_TO_HSV_COMMON(double H, double S, double V,
																		double &R, double &G, double &B, bool NORM);

  EXPORT void PIX_HSV_TO_RGB_COMMON(double H, double S, double V,
																		double &R, double &G, double &B);


  template<class T>
    void rgb_to_h_1D(idx<T> &rgb, idx<T> &h);

  //! Convert all pixels of rgb idx to hsv pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T>
    void rgb_to_hsv(idx<T> &rgb, idx<T> &hsv);

  //! Same as hsv_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T>
    idx<T> rgb_to_hsv(idx<T> &rgb);

  //! Convert all pixels of hsv idx to rgb pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T>
    void hsv_to_rgb(idx<T> &hsv, idx<T> &rgb);

  //! Same as hsv_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T>
    idx<T> hsv_to_rgb(idx<T> &hsv);

  ////////////////////////////////////////////////////////////////
  // HSV3

  EXPORT void PIX_HSV3_TO_RGB_COMMON(double H, double S, double V,
																		 double &R, double &G, double &B);
  //! Convert all pixels of rgb idx to hsv3 pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T>
    void rgb_to_hsv3(idx<T> &rgb, idx<T> &hsv3,
										 double threshold1, double threshold2);

  //! Same as hsv3_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T>
    idx<T> rgb_to_hsv3(idx<T> &rgb, double threshold1, double threshold2);

  //! Convert all pixels of hsv3 idx to rgb pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T>
    void hsv3_to_rgb(idx<T> &hsv3, idx<T> &rgb);

  //! Same as hsv3_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T>
    idx<T> hsv3_to_rgb(idx<T> &hsv3);

  ////////////////////////////////////////////////////////////////
  // YH3

  //! Convert all pixels of rgb idx to yh3 pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T>
    void rgb_to_yh3(idx<T> &rgb, idx<T> &yh3, double threshold1 = .10,
										double threshold2 = .15);

  //! Same as yh3_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T>
    idx<T> rgb_to_yh3(idx<T> &rgb, double threshold1 = .10,
											double threshold2 = .15);

  //! Convert all pixels of h3 idx to rgb pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T>
    void h3_to_rgb(idx<T> &h3, idx<T> &rgb);

  //! Same as h3_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T>
    idx<T> h3_to_rgb(idx<T> &h3);

  ////////////////////////////////////////////////////////////////
  // H2sv

  void PIX_HSV_TO_H2SV1_COMMON(double H, double &H1, double &H2);

  template<class T>
    void rgb_to_h2sv_1D(idx<T> &rgb, idx<T> &h2sv);

  template<class T>
    void rgb_to_h2sv(idx<T> &rgb, idx<T> &h2sv);

  ////////////////////////////////////////////////////////////////
  // VpH2SV

  template<class T>
    void rgb_to_vph2sv(idx<T> &rgb, idx<T> &vph2sv, double s, int n);

} // end namespace ebl

#include "color_spaces.hpp"

#endif /* COLOR_SPACES_H_ */
