/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
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

#ifndef CAMERA_OPENCV_HPP_
#define CAMERA_OPENCV_HPP_

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera_opencv<Tdata>::camera_opencv(int id, int height_, int width_)
    : camera<Tdata>(height_, width_) {
#ifndef __OPENCV__
  eblerror("opencv not found, install and recompile");
#else
    cout << "Initializing OpenCV camera..." << endl;
    capture = cvCaptureFromCAM(id);
    if (!capture) {
      fprintf( stderr, "ERROR: capture is NULL \n" );
      getchar();
      eblerror("failed to initialize camera_opencv");
    }
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width_);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height_);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 30);
#endif /* __OPENCV__ */
  }
  
  template <typename Tdata>
  camera_opencv<Tdata>::~camera_opencv() {
#ifdef __OPENCV__
    // release camera_opencv
    cvReleaseCapture(&capture);
#endif /* __OPENCV__ */
  }
  
  ////////////////////////////////////////////////////////////////
  // frame grabbing

  template <typename Tdata>
  idx<Tdata> camera_opencv<Tdata>::grab() {
#ifdef __OPENCV__
    ipl_frame = cvQueryFrame(capture);
    if (!ipl_frame)
      eblerror("failed to grab frame");
    // convert ipl to idx image
    if (grabbed)
      ipl_to_idx(ipl_frame, frame);
    else // first time, allocate frame
      frame = ipl_to_idx<Tdata>(ipl_frame);
#endif /* __OPENCV__ */
    frame_id_++;
    return this->postprocess();
  }
    
} // end namespace ebl

#endif /* CAMERA_OPENCV_HPP_ */
