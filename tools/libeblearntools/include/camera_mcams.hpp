/***************************************************************************
 *   Copyright (C) 2011 by Soumith Chintala   *
 *   soumith@gmail.com   *
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

#ifndef CAMERA_MCAMS_HPP_
#define CAMERA_MCAMS_HPP_

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
#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>

#endif

#include "camera_v4l2.h"
#include "camera_opencv.h"

namespace ebl {

template <typename Tdata>
camera_mcams<Tdata>::camera_mcams(configuration &conf,
                                  std::vector<std::string> &devs, int height_,
                                  int width_, bool grayscale_,
                                  bool mode_rgb)
    : camera<Tdata>(height_ * ((devs.size() % 2) ? ((devs.size() / 2) + 1) :
			       (devs.size() / 2)), width_ * 2 ),
      started(false), nbuffers(grayscale_ ? 1 : 3) {
  // reading mcam_type to initialize the appropriate constructors
  std::string cam_type = conf.get_string("mcam_type");
  // get devices paths
  std::vector<std::string> devices;
  for (uint i = 0; i < devs.size(); ++i)
    devices.push_back(conf.get_string(devs[i]));
  std::cout << "Initializing " << cam_type << " cameras with devices: " << devices
            << std::endl;

  uint ncams = devs.size();
  // this multiplier to split the display into 2x(n/2) matrix of cameras
  int height_multiplier = ncams % 2 ? ncams / 2 + 1 : ncams / 2;
  // if (ncams < 2) eblerror("mcams needs at least 2 cameras as arguments");
  // allocate buffers
  frame = idx<Tdata>(height_ * height_multiplier , width_ * 2, nbuffers);
  for (uint i = 0; i < devices.size(); ++i) {
    cam_frames.push_back(frame.narrow(0, height_ , height_ * (i/2)));
    cam_frames[i] = cam_frames[i].narrow(1, width_, width_ * (i%2));
  }
  // instantiate cameras
  for (uint i = 0; i < devices.size(); ++i) {
    if (cam_type.compare("v4l2") == 0)
      cams.push_back(new ebl::camera_v4l2<Tdata>
                     (devices[i].c_str(), height_, width_,
                      grayscale_, mode_rgb));
    else if (cam_type.compare("opencv") == 0)
      cams.push_back(new ebl::camera_opencv<Tdata>(i, height_, width_));
    else eblerror("Camera type not supported in camera_mcams");
  }
}

template <typename Tdata>
camera_mcams<Tdata>::~camera_mcams() {
  for (uint i = 0; i < cams.size(); ++i)
    delete cams[i];
}

#ifdef __LINUX__

////////////////////////////////////////////////////////////////
// frame grabbing

template <typename Tdata>
void camera_mcams<Tdata>::start() {
  std::cout<<" Starting camera_mcams "<< std::endl;
  started = true;
}

#endif

template <typename Tdata>
idx<Tdata> camera_mcams<Tdata>::grab() {
#ifdef __LINUX__
  if (!started)
    this->start();
  // for each camera, grab the new frame
  for( uint i=0; i < cams.size(); ++i) {
    idx_copy(cams[i]->grab(),cam_frames[i]);
  }
  std::cout<<"fps_grab:"<<this->fps_grab<< std::endl;
#endif
  frame_id_++;
  return this->postprocess();
  //return frame_;
}

} // end namespace ebl

#endif /* CAMERA_MCAMS_HPP_ */
