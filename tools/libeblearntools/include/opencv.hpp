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

namespace ebl {

#ifdef __OPENCV__

  ////////////////////////////////////////////////////////////////
  // interface with opencv

  template <typename T>
  IplImage* idx_to_ipl(idx<T> &im) {
    // TODO: check dimensions match
    IplImage* ipl = cvCreateImage(cvSize(im.dim(1), im.dim(0)),
				  IPL_DEPTH_8U, im.dim(2));
    uint sz = ipl->width * ipl->height * ipl->nChannels;
    ubyte *out = (ubyte*) ipl->imageData;
    T *in = im.idx_ptr();
    // cast and copy data
    for (uint i = 0; i < sz; ++i, ++out, ++in)
      *out = (ubyte) *in;
    return ipl;
  }

  template <typename T>
  idx<T> ipl_to_idx(IplImage *im) {
    idx<T> f(im->height, im->width, im->nChannels);
    uint sz = im->width * im->height * im->nChannels;
    ubyte *in = (ubyte*) im->imageData;
    T *out = f.idx_ptr();
    // cast and copy data
    for (uint i = 0; i < sz; ++i, ++out, ++in)
      *out = (T) *in;
    return f;
  }
    
  template <typename T>
  void ipl_to_idx(IplImage *im, idx<T> &out) {
    // TODO: check out dimensions match
    uint sz = im->width * im->height * im->nChannels;
    ubyte *in = (ubyte*) im->imageData;
    T *pout = out.idx_ptr();
    // cast and copy data
    for (uint i = 0; i < sz; ++i, ++pout, ++in)
      *pout = (T) *in;
  }
  
#endif /* __OPENCV__ */

} // end namespace ebl

