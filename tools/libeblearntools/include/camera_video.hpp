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

#ifndef CAMERA_VIDEO_HPP_
#define CAMERA_VIDEO_HPP_

#include <ostream>
#include <stdlib.h>
#include "configuration.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera_video<Tdata>::camera_video(const char *filename,
				    int height_, int width_,
				    uint sstep, uint endpos)
    : camera_directory<Tdata>(height_, width_), fps_video(1) {
    std::cout << "Initializing camera from video file: "
              << filename << std::endl;
    std::string dir = filename;
    dir += "_images";
    // create temporary directory for images
    mkdir_full(dir);
    // get video fps
    std::cout << "Getting video information..." << std::endl;
    std::ostringstream cmd;
    std::ostringstream videoconf;
    int ret;
    videoconf << filename << ".conf";
    cmd << "mplayer -identify " << filename;
    cmd << " -ao null -vo null -frames 0 2>/dev/null | grep ID_ ";
    cmd << " > " << videoconf.str();
    ret = std::system(cmd.str().c_str());
    configuration conf(videoconf.str());
    if (conf.exists("ID_VIDEO_FPS"))
      fps_video = conf.get_float("ID_VIDEO_FPS");
    std::cout << "Video FPS is: " << fps_video << std::endl;
    // extract all images from video
    cmd.str("");
    cmd << "mplayer " << filename << " -ao null -vo pnm:outdir=" << dir;
    if (endpos > 0)
      cmd << " -endpos " << endpos;
    if (sstep > 0)
      cmd << " -sstep " << sstep;
    std::cout << "Extracting video frames with command:" << std::endl
	 << cmd.str() << std::endl;
    ret = std::system(cmd.str().c_str());
    if (ret < 0)
      eblerror("video images extraction failed");
    // extract audio
    this->audio_filename = dir;
    this->audio_filename += "/audio.wav";
    std::cout << "Extracting audio into " << this->audio_filename << std::endl;
    cmd.str("");
    cmd << "mplayer -vo null -hardframedrop -ao pcm:file=";
    cmd << this->audio_filename << " " << filename;
    if (endpos > 0)
      cmd << " -endpos " << endpos;
    if (sstep > 0)
      cmd << " -sstep " << sstep;
    ret = std::system(cmd.str().c_str());
    // find all images
    this->read_directory(dir.c_str());
  }

  template <typename Tdata>
  camera_video<Tdata>::~camera_video() {
  }

  ////////////////////////////////////////////////////////////////
  // info

  template <typename Tdata>
  float camera_video<Tdata>::fps() {
    return fps_video;
  }

} // end namespace ebl

#endif /* CAMERA_VIDEO_HPP_ */
