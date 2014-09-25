/***************************************************************************
 *   Copyright (C) 2010 by Soumith Chintala and Pierre Sermanet *
 *   chin@nyu.edu, sermanet@cs.nyu.edu  *
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

#ifndef CAMERA_KINECT_HPP_
#define CAMERA_KINECT_HPP_

#ifdef __LINUX__

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#endif

#ifdef __KINECT__
extern "C"{
#include <libfreenect/libfreenect_sync.h>
#include <libfreenect/libfreenect.h>
}
//#include <asm/types.h>          /* for videodev2.h */
//#include <linux/videodev2.h>
#endif

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera_kinect<Tdata>::camera_kinect(int height_, int width_,
				      std::ostream &out,
				      std::ostream &err)
    : camera<Tdata>(height_, width_) {
    std::cout << "Initializing Kinect ..." << std::endl;
    frame = idx<Tdata>(480, 640, 3);
    mresize = false; // use regular resize instead of mean
    resize_mode = 1; // allow ratio loss
  }

  template <typename Tdata>
  camera_kinect<Tdata>::~camera_kinect() {
  }

  template <typename Tdata>
  idx<Tdata> camera_kinect<Tdata>::grab() {
#ifdef __KINECT__
    void *v_depth;
    unsigned int timestamp;
    int b = freenect_sync_get_depth
      (&v_depth, &timestamp, 0,
       (freenect_depth_format) FREENECT_DEPTH_11BIT_SIZE);
    if (b!=0) {
      out << "Kinect capture failed!" << endl;
      return frame;
    }
    uint16_t *depth = (uint16_t *)v_depth;
    //copy depth into an idx
    for(int j=0;j<480;j++) {
      for(int i=0;i<640;i++) {
	for(int k=0;k<3;k++) {
	  frame.set((Tdata)(depth[((640*j)+i)] ),j,i,k);
	  //			  depth_idx.set((depth[((640*j)+i)]*0.124511719),j,i,k);
	}
      }
    }
#endif
    return this->postprocess();
  }

} // end namespace ebl

#endif /* CAMERA_KINECT_HPP_ */
