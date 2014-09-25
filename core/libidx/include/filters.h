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

#ifndef FILTERS_H_
#define FILTERS_H_

#include "idx.h"
#include "numerics.h"

namespace ebl {
  
  //////////////////////////////////////////////////////////////////////////////
  // Filters

  //! Returns a mexican-hat filter.
  template <typename T> idx<T> create_mexican_hat(double s, int n);
  //! Returns a mexican-hat filter.
  template <typename T> idx<T> 
    create_mexican_hat2(int h, int w, double sigma = 1, double sigma_scale = 3);

  //! Returns a 1D gaussian kernel of size n, with variance 'sigma'.
  template <typename T> idx<T>
    create_gaussian_kernel2(uint n, double sigma = 1);
  //! Return a 2D gaussian kernel of size hxw, with variance 'sigma'.
  template <typename T>
    idx<T> create_gaussian_kernel2(uint h, uint w, double sigma = 1);
  
  //! Returns a gaussian kernel with dimensions d.
  template <typename T> idx<T>
    create_gaussian_kernel(idxdim &d, double mode = 2.0);
  //! Returns a 5x5 gaussian kernel as described by Burt and Adelson for
  //! gaussian/laplacian pyramids.
  template <typename T> idx<T> create_burt_adelson_kernel(double a = .375);
  
  //! Returns a gaussian kernel of size nxn.
  template <typename T> idx<T>
    create_gaussian_kernel(uint n, double mode = 2.0);
  //! Return a gaussian kernel of size hxw
  template <typename T> idx<T>
    create_gaussian_kernel(uint h, uint w, double mode = 2.0);
  
} // end namespace ebl

#include "filters.hpp"

#endif /* FILTERS_H_ */
