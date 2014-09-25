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

#ifndef CAMERA_WINDOWS_H_
#define CAMERA_WINDOWS_H_

#ifdef __WINDOWS__
#include <windows.h>
#endif

#include "vfw.h"
#include "camera.h"

namespace ebl {

  //! The camera_windows class interfaces with the camera and images under 
  //! Microsoft Windows.
  //! It allows to grab images from a webcam in the idx format, and also
  //! to save gui outputs into video files.
  template <typename Tdata>
    class camera_windows : public camera<Tdata> {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors/allocation

    //! Initialize a camera under Windows.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    camera_windows(int height = -1, int width = -1);

    //! Destructor.
    virtual ~camera_windows();

    ////////////////////////////////////////////////////////////////
    // frame grabbing

    //! Return a new frame.
    virtual idx<Tdata> grab();

  private:
#ifdef __WINDOWS__
    CVFWCapture cap;		// VFWCapture Object
    char * bmpData;			// Bits of the Image.
    BITMAPINFO * pbmi;		// BITMAPINFO structure
    ULONG BitmapSize;
#endif
    // members ////////////////////////////////////////////////////////
  protected:
    using camera<Tdata>::frame;		//!< frame buffer 
    using camera<Tdata>::grabbed;	//!< frame buffer grabbed yet or not
    using camera<Tdata>::frame_id;	//!< frame counter
    using camera<Tdata>::height;	//!< height
    using camera<Tdata>::width;	        //!< width
    using camera<Tdata>::bresize;       //!< resize or not during postproc.
    bool started;
    int nbuffers;
    void* *buffers;
    int *sizes;
    int fd;
  };

} /* namespace ebl */

#include "camera_windows.hpp"

#endif /* CAMERA_WINDOWS_H_ */
