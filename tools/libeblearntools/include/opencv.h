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

#ifndef OPENCV_H_
#define OPENCV_H_

#ifdef __OPENCV__

#include <cv.h>
#include <highgui.h>

#include "libidx.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // interface with opencv

  //! Allocates a new IplImage header that points to the data of idx im.
  //! Warning: you have to make sure that the data pointed to by the idx stays
  //! alive (i.e. is still referenced by some idx) otherwise the IplImage
  //! will point to deallocated data.
  IplImage* idx_to_iplptr(idx<ubyte> &im);

  //! Allocate a new Ipl image with same dimensions as im, and copy im data
  //! into ipl, return ipl.
  //! Warning: idx data will be cast to ubyte.
  template <typename T>
    IplImage* idx_to_ipl(idx<T> &im);

  //! Allocate an idx with dimensions of the opencv image, copy data into
  //! idx and return it.
  template <typename T>
    idx<T> ipl_to_idx(IplImage *im);

  //! Copy data from an opencv image into the passed idx.
  template <typename T>
    void ipl_to_idx(IplImage *im, idx<T> &out);

} // end namespace ebl

#include "opencv.hpp"

#endif /* __OPENCV__ */

#endif /* OPENCV_H_ */
