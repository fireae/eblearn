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

#ifndef BBOX_HPP_
#define BBOX_HPP_

namespace ebl {

  // sorting ///////////////////////////////////////////////////////////////////

  template <typename T>
  void bboxes::sort_by_difference(svector<midx<T> >& inputs) {
    if (inputs.size() != this->size())
      eblerror("expected same number as input as bboxes but got "
	       << inputs.size() << " and " << this->size());
    // note: inputs and boxes are assumned to be sorted by confidence already
    double dmax, smax;
    for (int i = 0; i < (int) inputs.size() - 1; ++i) {
      int id = i;
      dmax = 0, smax = 0;
      // find most different sample below
      for (int j = i + 1; j < (int) inputs.size(); j++) {
	// compute total distance of input j with all previous samples
	midx<T> e1 = inputs[j];
	double d = 0, s = 0;
	for (int k = 0; k < i + 1; k++) {
	  midx<T> e2 = inputs[k];
	  if (!e1.same_dim(e2)) d += idx_l2common(e1, e2);
	  else d += idx_l2(e1, e2);
	  // accumulate dimensions differences
	  for (uint l = 0; l < e1.dim(0); ++l) {
	    idx<T> e11 = e1.mget(l), e22 = e2.mget(l);
	    for (int m = 0; m < e11.order(); ++m)
	      s += std::abs(e11.dim(m) - e22.dim(m));
	  }
	}
	if (d > dmax || s > smax) {
	  id = j;
	  dmax = d;
	  smax = s;
	}
      }
      // swap
      if (id != i + 1) {
	// swap inputs
	midx<T> etmp = inputs[id];
	inputs[id] = inputs[i + 1];
	inputs[i + 1] = etmp;
	// swap boxes
	this->swap(id, i + 1);
      }
    }
  }

} // end namespace ebl

#endif /* BBOX_HPP_ */
