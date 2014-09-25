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

#include "opencv.h"

namespace ebl {

#ifdef __OPENCV__

  ////////////////////////////////////////////////////////////////
  // interface with opencv
  
  IplImage* idx_to_iplptr(idx<ubyte> &im) {
    idx_check_contiguous1(im); // im must be contiguous
    idx_checkorder1(im, 3); // im must have order 3
    if (im.dim(2) != 1 && im.dim(2) != 3) {
      cerr << "idx dimensions are: " << im << endl;
      eblerror("expecting an image with 1 or 3 channels");
    }
    IplImage *ipl = new IplImage();
    ipl->nSize = sizeof(IplImage);
    ipl->ID = 0;
    ipl->nChannels = im.dim(2);
    ipl->alphaChannel = 0;
    ipl->depth = IPL_DEPTH_8U;
    // ipl->colorModel = "GRAY";
    // ipl->channelSeq = "GRAY";
    ipl->dataOrder = 0; // 0: interleaved color
    ipl->origin = 0; // 0: top-left origin
    ipl->align = 4;
    ipl->width = im.dim(1);
    ipl->height = im.dim(0);
    ipl->roi = NULL;
    ipl->maskROI = NULL;
    ipl->imageId = NULL;
    ipl->tileInfo = NULL;
    ipl->imageSize = im.nelements();
    ipl->imageData = (char *) im.idx_ptr();
    ipl->widthStep = im.dim(1) * im.dim(2);
    ipl->imageDataOrigin = NULL;
    return ipl;
  }
  
#endif /* __OPENCV__ */

} // end namespace ebl

