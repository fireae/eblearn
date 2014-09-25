/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet and Marc Howard *
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

// ************************************************************************
//	class Camera_windows
//	Camera functionality provided by separate CVFWCapture class.
//  Author:	Marc Howard
//	Email:	marc.e.howard AT gmail DOT com
//	Last Modified: Juuly 2010
//	Borrowed some code from Audrey J. W. Mbogho  walegwa AT yahoo DOT com
//	Last Modified: January 2005
//	Environment: Visual Studio 2008, Windows 7 64 Bit,
//        Logitech Quickcam 4000 Pro.
// ************************************************************************

#ifndef CAMERA_WINDOWS_HPP_
#define CAMERA_WINDOWS_HPP_

#include <fstream>

namespace ebl {

  template <typename Tdata>
  camera_windows<Tdata>::camera_windows(int height_, int width_) {
    : camera<Tdata>(height_, width_) {
#ifdef __WINDOWS__
    bmpData = '\0';
    pbmi = NULL;
    BitmapSize = 0;
    cap.Initialize();
#else
    eblerror("cannot use camera_windows when not under Windows");
#endif
  }

  template <typename Tdata>
  camera_windows<Tdata>::~camera_windows() {
#ifdef __WINDOWS__
    cap.Destroy();	// Done using VFW object
#endif
  }

  template <typename Tdata>
  idx<Tdata> camera_windows<Tdata>::grab() {
    BITMAPINFOHEADER bmih;	// Contained in BITMAPINFO structure
    pbmi = NULL;  // CaptureDIB will automatically allocate this if set to NULL
    // Capture an image from the capture device.
    if (cap.CaptureDIB(&pbmi, 0, &BitmapSize)) {
	// Obtain args for SetDIBitsToDevice
	bmih = pbmi->bmiHeader;
	int height = -1, width = -1;
	width = bmih.biWidth;
	height = bmih.biHeight;
	cout << "camera is returning " << height << "x" << width << " frames."
	     << endl;
	height = (bmih.biHeight>0) ? bmih.biHeight : -bmih.biHeight;
	bmpData = (char *)pbmi;
	bmpData += cap.CalcBitmapInfoSize(bmih);
      }
    frame_id_++;
    return bmpData;
  }

} /* namespace ebl */

#endif CAMERA_WINDOWS_HPP_
