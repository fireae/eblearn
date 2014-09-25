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

#ifndef PADDER_H_
#define PADDER_H_

#include "idx.h"
#include "geometry.h"

namespace ebl {

  //////////////////////////////////////////////////////////////////////////////
  // padder

  //! A class that adds padding around an input to be convolved by a filter.
  template <typename T> class padder {
  public:
    //! Create a padder given kernel size 'kerdims'.
    //! \param mirror If true, mirror input into padding.
    padder(const idxdim &kerdims, bool mirror = true);
    virtual ~padder();
    //! This method allocates an idx with zero pads accomodating the kernel,
    //! copies 'in' to the center and returns the idx.
    virtual idx<T> pad(idx<T> &in);
    //! This method resizes idx 'out' if necessary with extra zero pads
    //! accomodating the kernel, copies 'in' to the center and returns the idx.
    virtual void pad(idx<T> &in, idx<T> &out);

  protected:
    //! Fills padded areas in 'padded' with mirrors of 'in'.
    virtual void mirror(idx<T> &in, idx<T> &padded);

  protected: // members ////////////////////////////////////////////////////////
    int nrow, ncol, nrow2, ncol2;
    bool bmirror;
  };

  //////////////////////////////////////////////////////////////////////////////
  // padding utilities

  //! Returns the rectangular image of size hxw centered on region 'r'.
  //! Padding is filled with zeros.
  //! \param cropped If not null, cropped is set to the region in the output
  //!   image that comes from the input image.
  //! \param dh The height dimension (0 by default).
  //! \param dw The width dimension (1 by default).
  template<typename T>
    idx<T> image_region_to_rect(idx<T> &im, const rect<int> &r, uint h,
				uint w, rect<int> *cropped = NULL,
				uint dh = 0, uint dw = 1);
  //! Copies 'in' to the center of 'out' (expected to be 3d idx).
  template<typename T>
    void image_paste_center(idx<T> &in, idx<T> &out);

} // end namespace ebl

#include "padder.hpp"

#endif /* PADDER_H_ */
