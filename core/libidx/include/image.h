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

#ifndef IMAGE_H_
#define IMAGE_H_

#include "idx.h"
#include "idxops.h"
#include "geometry.h"
#include "pyramids.h"
#include "numerics.h"
#include "filters.h"
#include "padder.h"

namespace ebl {
  
#define BILINEAR_RESIZE 0
#define GAUSSIAN_RESIZE 1
#define MEAN_RESIZE 2
  
  ////////////////////////////////////////////////////////////////
  //! crop rectangle (<x>,<y>,<w>,<h>) from image <in>
  //! and return the result (a copy).
  template<typename T>
    idx<T> image_crop(idx<T> &in, int x, int y, int w, int h);

  //! resize a greyscale image to any size using bilinear interpolation
  //! Appropriate local averaging (smoothing) is performed for scaling
  //! factors smaller than 0.5. If one of the desired dimensions is
  //! 0, an aspect-ratio-preserving scaling is performed on
  //! the basis of the other dimension. When both <width> and <height>
  //! are non zero, the last parameter, <mode> determines how they are
  //! interpreted.
  //! If iregion is provided, resize the image so that this region is resized
  //! to hxw. If oregion is provided, it is filled and describes that same
  //! region in the return resized image.
  //!
  //! if either <width> or <height> is zero, <mode> is ignored.
  //! mode=0: return an image that fits into a <width> by <height> frame
  //!         while preserving the aspect ratio.
  //! mode=1: scale the image to <width> by <height> possibly changing the 
  //!         aspect ratio.
  //! mode=2: <width> and <height> are interpreted as scaling ratios
  //! mode=3: return an image of size <width> by <height> but while preserving
  //!         the aspect ratio. the image is placed in the center of the
  //!         target dimensions and background is filled with zeros.
  //! The sizes of the output image are rounded to nearest integers
  //! smaller than the computed sizes, or to 1, whichever is largest.
  template<typename T> 
    idx<T> image_resize(idx<T> &im, double h, double w, int mode = 1,
			rect<int> *iregion = NULL, rect<int> *oregion = NULL);

  //! resizes an image (a region iregion of im if specified) into an image of
  //! size oheightxowidth using gaussian pyramids. Bilinear resizing is first
  //! used to resize to the closest gaussian-compatible size, then subsample by
  //! with gaussians to reach target size.
  //! The output will still be an image of size oheightxowidth centered on the
  //! iregion (the entire image if not specified).
  //! oregion is filled by the function if given and represents the resized
  //! region of iregion.
  template<typename T>
    idx<T> image_gaussian_resize(idx<T> &im_, double oheight, double owidth,
				 uint mode = 0, rect<int> *iregion = NULL,
				 rect<int> *oregion = NULL);

  //! resizes an image (a region iregion of im if specified) into an image of
  //! size oheightxowidth using mean. Bilinear resizing is first used to
  //! resize to the closest power of 2 size, then subsample by taking mean of
  //! power of 2 regions to reach target size.
  //! The output will still be an image of size oheightxowidth centered on the
  //! iregion (the entire image if not specified).
  //! oregion is filled by the function if given and represents the resized
  //! region of iregion.
  template<typename T>
    idx<T> image_mean_resize(idx<T> &im_, double oheight, double owidth,
			     uint mode = 0, rect<int> *iregion = NULL,
			     rect<int> *oregion = NULL);

  //! returns the biggest square image including image region r.
  template<typename T> 
    idx<T> image_region_to_square(idx<T> &im, const rect<uint> &r);

  //! This function takes 2D or 3D images (greyscale or RGB)
  //! as input of type T and converts
  //! it to a ubyte image, by mapping the range [minv, maxv] to [0,255]
  //! and applying zoom factors zoomx and zoomy along each axis.
  template<typename T> 
    idx<ubyte> image_to_ubyte(idx<T> &im, double zoomh, double zoomw,
			      T minv, T maxv);

  //! subsamples image <in> with integer ratios <nlin> (vertical) <ncol>
  //! (horizontal). the subsampled image is returned.
  //! The horizontal (resp. vertical) size of the returned image is
  //! equal to the integer part of the horizontal (resp vertical)
  //! size of <in> divided by <ncol> (resp <nlin>).
  //!
  //! returns (copy-matrix <in>) when subsample rate is 1
  template<typename T> idx<T> image_subsample(idx<T> &in, int nlin, int ncol);

  // image warpings ////////////////////////////////////////////////////////////
  
  //! RETURNS: Null
  //! SIDE EFFECTS: <out>
  //! AUTHOR: Y. LeCun
  //! COMPILABLE: Yes
  //! DESCRIPTION:
  //! transforms ubimage <in> (idx2 of ubyte) mapping quadrilateral <x1> <y1> 
  //! <x2> <y2> <x3> <y3> <x4> (points are numbered clockwise starting from 
  //! upper left) to rectangle whose upper left and lower right points are 
  //! <p1><q1>, <p3><q3>. result is put in <out> (idx2 of ubyte). 
  //! Clipping is automatically performed.
  //! pixels outside of the bounds of <in> are assumed to have the value
  //! <background>.  <mode>=0: no antialiasing, <mode>=1: antialiasing with
  //! bilinear interpolation (2 times slower).  
  //! execution time on sparc 10 is about
  //! 5 ms in mode 0 and 10 ms in mode 1 for a 32x32 target image.
  template<typename T> 
    void image_warp_quad(idx<T> &in, idx<T> &out, idx<T> &background, int mode,
			 float x1, float y1, float x2, float y2, float x3, 
			 float y3, float x4, float y4, float p1, float q1, 
			 float p3, float q3);

  //! RETURNS: Null
  //! SIDE EFFECTS: <out>
  //! AUTHOR: Y. LeCun
  //! COMPILABLE: Yes
  //! DESCRIPTION:
  //! Warps an image using <pi> and <pj> as tabulated coordinate transforms.
  //! <in> and <out> are idx2 of ubytes. <background> is the value assumed 
  //! outside of the input image. <pi> and <pj> are tabulated coordinates which
  //! can be filled up using compute-bilin-transform or similar functions.
  //! Pixel values are antialiased using bilinear interpolation.
  template<typename T> 
    void image_warp(idx<T> &in, idx<T> &out, idx<T> &background, 
		    idx<int> &pi, idx<int> &pj);

  //! fast version, not interpolation.
  //! RETURNS: Null
  //! SIDE EFFECTS: <out>
  //! AUTHOR: Y. LeCun
  //! COMPILABLE: Yes
  //! DESCRIPTION:
  //! Warps an image using <pi> and <pj> as tabulated coordinate transforms.
  //! <in> and <out> are idx2 of ubytes. <background> is the value assumed 
  //! outside of the input image. <pi> and <pj> are tabulated coordinates which
  //! can be filled up using compute-bilin-transform or similar functions.
  //! This is essentially identical to warp-ubimage, except no antialiasing
  //! is performed (it goes a lot faster, but is not nearly as nice).
  template<typename T> 
    void image_warp_fast(idx<T> &in, idx<T> &out, T *background,
			 idx<int> &pi, idx<int> &pj);
 
  //! Warps an image, according to an (x,y) flow field. The flow
  //! field is in the space of the destination image, each vector
  //! ponts to a source pixel in the original image.
  //! \param bilinear If true, use bilinear interpolation, otherwise nearest
  //!   neighbor.
  //! \param use_background If true, use 'background' value as default value
  //!   when going beyond original image, otherwise just propagate image's
  //!   borders.
  template <typename T>
    void image_warp_flow(idx<T> &src, idx<T> &dst, idx<float> &flow,
			 bool bilinear = true, bool use_background = true,
			 T background = 0);
 
  //////////////////////////////////////////////////////////////////////////////
  // bilinear interpolation
  
  //! Ubyte specialization of image_interpolate_bilin template.
  EXPORT void image_interpolate_bilin(ubyte* background, ubyte *pin, int indimi,
				      int indimj, int inmodi, int inmodj, 
				      int ppi, int ppj, ubyte* out, 
				      int outsize);

  //! Float specialization of image_interpolate_bilin template.
  EXPORT void image_interpolate_bilin(float* background, float *pin, int indimi,
				      int indimj, int inmodi, int inmodj, 
				      int ppi, int ppj, float* out, 
				      int outsize);

  //! RETURNS: (-ubyte-)
  //! SIDE EFFECTS: None
  //! AUTHOR: Y. LeCun
  //! DESCRIPTION:
  //! returns a bilinearly interpolated pixel value for coordinate
  //! <ppi> <ppj>. The image data is pointed to by <pin>, with
  //! <indimi> <indimj> <inmodi> <inmodj> being the dimensions and modulos.
  //! This function clips automatically if <ppi> <ppj> are outside of the
  //! bounds by assuming the outside values are equal to <background>.
  //! pixel values are ubytes, while coordinates are 32 bit fixed point
  //! with 16 bit integer part and 16 bit fractional part.
  //! The function does not use floating point arithmetics.
  template<typename T> 
    void image_interpolate_bilin(T* background, T *pin, int indimi, int indimj,
				 int inmodi, int inmodj, int ppi, int ppj,
				 T *out, int outsize);

  //! RETURNS: Null
  //! SIDE EFFECTS: <dispi> <dispj>
  //! AUTHOR: Y. LeCun
  //! COMPILABLE: Yes
  //! DESCRIPTION:
  //! Tabulates a bilinear transformation that maps the quadrilateral defined
  //! by the <xi> <yi> to a rectangle whose upper left point is <p1> <q1> and
  //! lower right point is <p3> <q3>.
  //! <x1> <y1> is the upper left point on the quadrilateral, and the points
  //! are numbered clockwise.
  //! <dispi> and <dispj> must be idx2 of int. On output, element (i,j)
  //! of <dispi> and <dispj> will be filled respectively with the abscissa
  //! and ordinate of the point in the quadrilateral that maps to point (i,j).
  //! Values in these idx2 are interpreted as 32bit fixed point numbers
  //! with 16 bit integer part and 16 bit fractional part.
  //! <dispi> and <dispj> can subsequently be used as input to
  //! warp-shimage, or warp-shimage-fast.
  //! This function makes minimal use of floating point arithmetics.
  template<typename T> 
    void compute_bilin_transform(idx<int> &dispi, idx<int> &dispj, 
				 float x1, float y1, float x2, float y2, 
				 float x3, float y3, float x4, float y4, 
				 float p1, float q1, float p3, float q3);

  //////////////////////////////////////////////////////////////////////////////
  // rotation functions
  
  //! Rotate, scale, and translate image 'src' and returns the result.
  //! Point (h0,w0) in 'src' will be mapped to point (h1,w1) in resulting image.
  //! Image will be rotated clockwise by <angle> degrees
  //! and scaled by <coeff>. Pixels that fall off the boundary
  //! are clipped and pixels in the destination that are not
  //! determined by a source pixel are set to color <bg>
  //! (which must be an idx1 of ubytes of size 4).
  //! It is generally preferable to call rgbaim-rotscale-rect
  //! before hand to get appropriate values for <dx>,<dy> and
  //! for the size of <dst> so that no pixel is clipped.
  //! \param angle The rotation angle in degrees.
  //! \param bg The value of the cut out areas (0 by default).
  template<typename T> 
    idx<T> image_rotscale(idx<T> &src, double h0, double w0, double h1,
			  double w1, double angle, double coeff = 1, T bg = 0);

  //! Rotate image 'src' by 'angle' (in degrees) around center (h,w)
  //! and return it. By default,
  //! use image's center (when equal to -1) and fill cut out areas with zeros.
  //! \param angle The rotation angle in degrees.
  //! \param bg The value of the cut out areas (0 by default).
  template<typename T> 
    idx<T> image_rotate(idx<T> &src, double angle, float h = -1, float w = -1,
			T bg = 0);

  //! Given an input image of width <w>, height <h>, with a "hot" point
  //! at coordinate <cx> and <cy>, this function computes the width,
  //! height, and hot point coordinates of the same image rotated
  //! by <angle> and scaled by <coeff> so as to ensure that no pixel
  //! in the rotated/scaled image will have negative coordinates
  //! (so the image will not be clipped).
  //! <wh> and <cxcy> which must be idx1 of floats with two elements.
  //! This function should be called before rgbaim-rotscale.
  void image_rotscale_rect(int w, int h, double cx, double cy, double angle, 
			   double coeff, idx<intg> &wh, idx<double> &cxcy);

  //! Draw a box in img.
  template<typename T> 
    void image_draw_box(idx<T> &img, T val, unsigned int x, unsigned int y, 
			unsigned int dx, unsigned int dy);

  bool collide_rect(int x1, int y1, int w1, int h1,
		    int x2, int y2, int w2, int h2);

  //! returns how the 2nd rect overlaps the 1rst, in percentage of the area of 
  //! the 1rst rect.
  double common_area(int x1, int y1, int w1, int h1,
		     int x2, int y2, int w2, int h2);

  //! applies a mexican filter <filter> with paramters (s, n)
  //! on <in> and puts the results in <out>.
  //! if tmp is not NULL, use that buffer as temporary copy of the input
  //! taking into account the margins of the filter, this buffer can be kept
  //! around to decrease time wasted on memory allocation if this function
  //! is called a lot.
  //! similarly, if filter is not NULL, reuse that filter instead of creating
  //! a new one.
  template<typename T>
    void image_mexican_filter(idx<T> &in, idx<T> &out, double s, int n,
			      idx<T> *filter = NULL, idx<T> *tmp = NULL);
  
  //! in-place normalization: zero-mean and divided by standard deviation.
  template<typename T>
    void image_global_normalization(idx<T> &in);
  
  //! In-place local normalization: zero-mean and divided by standard deviation
  //! in n by n neighborhoods.
  //! This function only accepts 2D inputs.
  template<typename T>
    void image_local_normalization(idx<T> &in, idx<T> &out, int n);
  
  //! applies the filter <filter> on <in> and puts the results in <out>.
  //! if tmp is not NULL, use that buffer as temporary copy of the input
  //! taking into account the margins of the filter, this buffer can be kept
  //! around to decrease time wasted on memory allocation if this function
  //! is called a lot.
  template<typename T>
    void image_apply_filter(idx<T> &in, idx<T> &out, idx<T> &filter,
			    idx<T> *tmp = NULL);
  template<typename T>
    idx<T> image_filter(idx<T> &in, idx<T> &filter);

  // deformations //////////////////////////////////////////////////////////////
  
  template<typename T>
  void image_deformation_ranperspective(idx<T> &in, idx<T> &out,
					int hrange, int wrange, T background);

  //! Returns the flow of pixels from source 'in' to destination image
  //! after applying scaling shxsw, translation thxtw.
  //! The returned flow is of dimensions 2xHxW.
  //! \param th Height translation offset.
  //! \param tw Width translation offset.
  //! \param sh Height scaling ratio.
  //! \param sw Width scaling ratio.
  //! \param shh Shear factor for shear parallel to height axis.
  //! \param shw Shear factor for shear parallel to width axis.
  template<typename T>
    idx<float> image_deformation_flow(idx<T> &in, float th, float tw,
				      float sh, float sw, float deg,
				      float shh, float shw,
				      uint elsize, float elcoeff,
				      T background = 0);
  //! 
  template<typename T>
    void image_deformation(idx<T> &in, idx<T> &out, float th, float tw,
			   float sx, float sy, float deg,
			   float shh, float shw, uint elsize, float elcoeff,
			   T background = 0);

  // vector flows //////////////////////////////////////////////////////////////

  //! Creates a vector field corresponding to each input pixel location
  //! relative to the input's center. The returned grid's first dimension
  //! is of size 2 (x and y), and remaining dimensions are input's.
  EXPORT idx<float> create_grid(idxdim &inputd);
  //! Accumulates the translation flow given translations offsets h and w
  //! into 'flow'.
  //! \param flow A 2xHxW matrix where dimension 0 contains h and w flows.
  EXPORT void translation_flow(idx<float> &grid, idx<float> &flow, float h, float w);
  //! Accumulates the shear flow given shear factors h and w //! into 'flow'.
  //! \param flow A 2xHxW matrix where dimension 0 contains h and w flows.
  EXPORT void shear_flow(idx<float> &grid, idx<float> &flow, float h, float w);
  //! Accumulates the 'flow' of 'grid' when scaled by ratios x and y.
  //! \param flow A 2xHxW matrix where dimension 0 contains h and w flows.
  //! \param h Height scaling ratio.
  //! \param w Width scaling ratio.
  EXPORT void scale_flow(idx<float> &grid, idx<float> &flow, float h, float w);
  //! Accumulates the 'flow' of 'grid' when rotated by 'deg' degrees.
  //! \param flow A 2xHxW matrix where dimension 0 contains h and w flows.
  //! \param deg Rotation in degrees.
  EXPORT void rotation_flow(idx<float> &grid, idx<float> &flow, float deg);
  EXPORT void affine_flow(idx<float> &grid, idx<float> &flow,
		   float th, float tw, float sh, float sw,
		   float shh, float shw, float deg);
  EXPORT void elastic_flow(idx<float> &flow, uint elsize, float elcoeff);

  //////////////////////////////////////////////////////////////////////////////

  //! This returns 0, 1 or 2 if type contains "bilinear", "gaussian" or "mean".
  EXPORT uint get_resize_type(const char *type);
  
} // end namespace ebl

#include "image.hpp"

#endif /* IMAGE_H_ */
