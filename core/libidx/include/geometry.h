/***************************************************************************
 *   Copyright (C) 2008 by Pierre Sermanet *
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

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "defines.h"
#include "stl.h"

namespace ebl {

//! A templated rectange class with default type 'uint'.
template <typename T = uint> class EXPORT rect {
public:
  ////////////////////////////////////////////////////////////////////////////
  // constructors/destructors

  //! A rectangle with offset (h0, w0) and size (height, width).
  rect(T h0, T w0, T height, T width);
  //! Empty constructor. All values are initialized 0.
  rect();
  //! Values are initialized with 'r' values.
  rect(const rect<T> &r);
  //! Destructor.
  virtual ~rect();

  ////////////////////////////////////////////////////////////////////////////
  //! Return bottom coordinate, i.e. h0 + height.
  T h1() const;
  //! Return right coordinate, i.e. w0 + width.
  T w1() const;
  //! Return center's height, i.e. h0 + height / 2.
  float hcenter() const;
  //! Return center's width, i.e. h0 + height / 2.
  float wcenter() const;
  //! Return area of rectangle (i.e. height * width).
  T area() const;
  //! Return area of intersection between this rectangle and r.
  T intersection_area(const rect<T> &r) const;
  //! Return area of union between this rectangle and r,
  //! i.e. this.area + r.area - intersection
  T union_area(const rect<T> &r) const;
  //! Return how much this rect matches with r, between 0.0 and 1.0,
  //! i.e. intersection / union, 0.0 means the 2 rectangles do not overlap
  //! at all whereas 1.0 means they are identical.
  float match(const rect<T> &r) const;
  //! Return how much this rect matches with r, between 0.0 and 1.0,
  //! i.e. intersection / min(area, r.area), 0.0 means the 2 rectangles
  //! do not overlap at all whereas 1.0 means they are identical.
  float min_match(const rect<T> &r);
  //! Return the euclidean distance between this and r's centers.
  float center_distance(const rect<T> &r) const;
  //! Return the height distance between this and r's centers height,
  //! over the height of this rect.
  float center_hdistance(const rect<T> &r) const;
  //! Return the width distance between this and r's centers width,
  //! over the width of this rect.
  float center_wdistance(const rect<T> &r) const;
  //! return true if current rectangle overlaps with r.
  bool overlap(const rect<T> &r) const;
  //! Returns the ratio of the intersection of this rect and r over this
  //! rect's area.
  float overlap_ratio(const rect<T> &r) const;
  //! Returns true if current rectangle overlaps with r more than authorized
  //! ratios hmin and wmin. The ratio is computed with overlap / smallest
  //! rectangle.
  //! \param hmin Minimum height overlap ratio required to declare overlap.
  //! \param wmin Minimum width overlap ratio required to declare overlap.
  bool min_overlap(const rect<T> &r, float hmin, float wmin) const;
  //! Returns the overlap ratio with the smallest of the 2 rectangles,
  //! i.e. intersection / min(this.area, r.area).
  float min_overlap(const rect<T> &r) const;
  //! Returns true if the ratio of the overlapping area over the smallest
  //! rectangle is more than the minarea ratio.
  //! \param minarea Ratio of minimum area overlap required to declare
  //!    overlap.
  bool min_overlap(const rect<T> &r, float minarea) const;
  //! return true current rectangle is entirely contained in r.
  bool is_within(const rect<T> &r);
  //! Shift rectangle's origin by h and w.
  void shift(T h, T w);
  //! Scale height and width while keeping the same center, as opposed
  //! to operator* that scales height and width but also h0 and w0, thus
  //! changing the original center.
  void scale_centered(float sh, float sw);
  //! Scale width to height * 'woverh_ratio', and shift w0 accordingly
  //! to keep the same center.
  void scale_width(float woverh_ratio);
  //! Scale height to width * 'woverh_ratio', and shift h0 accordingly
  //! to keep the same center.
  void scale_height(float woverh_ratio);
  //! Returns the radius of the circle passing by each corner of this rect.
  float radius();
  //! Rotates the rectangle clockwise by 'alpha', with the center of the
  //! rect as center of rotation.
  //! \param degree_angle The rotation angle in degrees.
  void rotate(float degree_angle);

  ////////////////////////////////////////////////////////////////////////////
  // operators

  //! Divide all components (h0, w0, height, width) of this rect by 'd'.
  rect<T> operator/(double d);
  //! Multiply all components (h0, w0, height, width) of this rect by 'd'.
  rect<T> operator*(double d);
  //! Assign 'r2' to this rect.
  template <typename T2>
      rect<T> operator=(rect<T2> &r2);

  // internal methods ////////////////////////////////////////////////////////

  //! Rotate a point (h,w) by alpha radians.
  void rotate_point(float alpha, float &h, float &w);

  // members /////////////////////////////////////////////////////////////////
public:
  T h0, w0, height, width;
};

template <typename T>
EXPORT std::string& operator<<(std::string& out, rect<T>& r);
template <typename T>
EXPORT std::string& operator<<(std::string& out, const rect<T>& r);
template <typename T>
EXPORT std::ostream& operator<<(std::ostream& out, rect<T>& r);
template <typename T>
EXPORT std::ostream& operator<<(std::ostream& out, const rect<T>& r);

} // end namespace ebl

#include "geometry.hpp"

#endif /* GEOMETRY_H_ */
