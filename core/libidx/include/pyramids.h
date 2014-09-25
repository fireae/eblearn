/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
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

#ifndef PYRAMIDS_H_
#define PYRAMIDS_H_

#include "defines.h"
#include "idx.h"
#include "numerics.h"
#include "geometry.h"
#include "padder.h"
#include "filters.h"

namespace ebl {

  //! A gaussian pyramid implementation.
  //! Uses a 5x5 kernel as sepcified in Burt and Adelson paper with a=0.375.
  //! This is the same setup used in Matlab too.
  //! In 1D, f is given as follows:
  //!
  //! f = [1/4-a/2 1/4 a 1/4 1/4-a/2]
  //!
  //! The scaling in this implementation fixed to 2, so each downsampled
  //! image will be almost half the size in each dimension.
  //!
  //! One can create a Laplacian Image Pyramid by successively
  //! reducing and expanding an image and taking the difference between two.
  template <class T> class gaussian_pyramid {
  public:
    //! The default value of <a> is 0.375, however one can
    //! enter diffferent values to change the shape of the 
    //! gaussian.
    gaussian_pyramid(double c = 0.375);
    //! Destructor.
    virtual ~gaussian_pyramid();

    // scaling operations //////////////////////////////////////////////////////

    //! The reduce method for obtaining a scaled down version of the given
    //! image. Optionally specify with n how many reductions to perform.
    //! Returns the result.
    //! 
    //! <in> is a -idx2- of -double-.  <out> is a -idx2- of -double-.
    //! <out> can be any size, it will be resized approriately.
    //! Size of <in> might not be suitable for convolving with a 5x5
    //! filter, followed by a 2x2 downsampling. Borders might be removed
    //! to satisfy convolution requirements. The original imaage will not
    //! be modified, a copy will be generated.
    idx<T> reduce(idx<T> &in, uint n = 1);
    //! returns a reduced rectangle by n reductions.
    rect<uint> reduce_rect(const rect<uint> &r, uint n = 1);
    //! return number of reductions necessary for size insz to be <= outsz
    uint count_reductions(uint insz, uint outsz, uint &dist);    
    //! return number of reductions and set size of the input (closest to
    //! current one) to exactly downsample to outr (aspect ratio unchanged).
    uint count_reductions_exact(rect<uint> &inr, rect<uint> &outr,
				rect<uint> &inr_exact);    
    //! The expand method for obtaining an upsampled version of a given
    //! image from gaussian pyramid.
    //! Optionally specify with n how many expansions to perform.
    //! Returns the result.
    //! <in> is a -idx2- of -double-.  <out> is a -idx2- of -double-.
    //! <out> can be any size, it will be resized approriately.
    idx<T> expand(idx<T> &in, uint n = 1);    
    //! returns a expanded rectangle by n expansions.
    rect<uint> expand_rect(const rect<uint> &r, uint n = 1);
    //! return number of expansions necessary for size insz to be >= outsz
    uint count_expansions(uint insz, uint outsz, uint &dist);

    // util methods ////////////////////////////////////////////////////////////

    //! copy in into out, cutting the extra padding of size nz on each border,
    //! and return out.
    idx<T> cut_pad(idx<T> &in, int nz);

    // members /////////////////////////////////////////////////////////////////
    idx<T> red_tin;
  protected:
    double a;
    idx<T> filter;
  };

} // end namespace ebl

#include "pyramids.hpp"

#endif /* PYRAMIDS_H_ */
