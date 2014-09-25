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

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <iomanip>

namespace ebl {

////////////////////////////////////////////////////////////////
// constructors & initializations

template <typename Tdata>
camera<Tdata>::camera(int height_, int width_, std::ostream &o,
                      std::ostream &e)
    : height(height_), width(width_), bresize(false), mresize(true),
      resize_mode(0), frame_id_(0),
      grabbed(false), wid(0), recording_name("video"), record_cnt(0),
      fps_grab(0.0), audio_filename(""), out(o), err(e), cntfps(0),
      narrow_dim(-1), narrow_size(-1), narrow_off(-1), grayscale(false),
      silent(false) {
  // decide if we resize input or not
  if ((height != -1) && (width != -1))
    bresize = true;
  tfps.start(); // timer for computing fps
}

template <typename Tdata>
camera<Tdata>::~camera() {
}

////////////////////////////////////////////////////////////////
// grabbin

template <typename Tdata>
std::string camera<Tdata>::grab_filename() {
  std::string err;
  eblerror("not implemented");
  return err;
}

template <typename Tdata>
void camera<Tdata>::next() {
  eblerror("not implemented");
}

template <typename Tdata>
void camera<Tdata>::previous() {
  eblerror("not implemented");
}

template <typename Tdata>
bool camera<Tdata>::empty() {
  return false; // never empty by default
}

template <typename Tdata>
void camera<Tdata>::skip(uint n) {
  eblerror("not implemented");
}

template <typename Tdata>
void camera<Tdata>::set_input_narrow(int dim, int size, int off) {
  narrow_dim = dim;
  narrow_size = size;
  narrow_off = off;
}

template <typename Tdata>
void camera<Tdata>::set_grayscale() {
  grayscale = true;
}

////////////////////////////////////////////////////////////////
// video recording

template <typename Tdata>
bool camera<Tdata>::start_recording(uint window_id, const char *name) {
  wid = window_id;
  record_cnt = 0;
  if (name)
    recording_name = name;
  // add timestamp to name
  recording_name += "_";
  recording_name += tstamp();
  // create directory
  mkdir_full(recording_name);
  return true;
}

template <typename Tdata>
bool camera<Tdata>::record_frame() {
#ifdef __GUI__
  std::ostringstream oss;
  oss << recording_name << "/frame_";
  oss << setfill('0') << setw(6) << record_cnt << ".png";
  save_window(oss.str().c_str());
  out << "saved " << oss.str() << std::endl;
  record_cnt++;
#endif
  return true;
}

template <typename Tdata>
bool camera<Tdata>::stop_recording(float fps, const char *root) {
  if (record_cnt == 0)
    return false;
  std::string rname;
  if (root) {
    rname += root;
    rname += "/";
  }
  rname += recording_name;
  std::ostringstream oss;
  uint optimal_bitrate = 50 * 25 * frame.dim(0) * frame.dim(1) / 256;
  std::ostringstream options;
  std::string codec = "msmpeg4v2";
  options << "vbitrate=" << optimal_bitrate << ":mbd=2:keyint=132:";
  options << "vqblur=1.0:cmp=2:subcmp=2:dia=2:mv0:last_pred=3";
  // pass 1
  oss << "mencoder \"mf://" << rname;
  oss << "/*.png\" -mf type=png:fps=" << fps;
  oss << " -ovc lavc";
  oss << " -lavcopts vcodec=" << codec << ":vpass=1:" << options.str();
  oss << " -o /dev/null ";
  int ret = std::system(oss.str().c_str());
  if (ret < 0)
    return false;
  // pass 2
  oss.str("");
  oss << "mencoder \"mf://" << rname;
  oss << "/*.png\" -mf type=png:fps=" << fps;
  oss << " -ovc lavc";
  oss << " -lavcopts vcodec=" << codec << ":vpass=2:" << options.str();
  if (audio_filename != "") {
    oss << " -audiofile " << audio_filename;
    oss << " -oac mp3lame -lameopts cbr:br=32 ";
  }
  oss << " -o " << rname << ".avi ";
  ret = std::system(oss.str().c_str());
  if (ret < 0)
    return false;
  out << "Saved " << rname << ".avi";
  out << " at " << fps << " fps." << std::endl;
  return true;
}

////////////////////////////////////////////////////////////////
// info

template <typename Tdata>
float camera<Tdata>::fps() {
  return fps_grab;
}

template <typename Tdata>
uint camera<Tdata>::frame_id() {
  return frame_id_;
}

template <typename Tdata>
std::string camera<Tdata>::frame_name() {
  std::ostringstream name;
  name << "frame_" << frame_id_;
  return name.str();
}

template <typename Tdata>
std::string camera<Tdata>::frame_fullname() {
  return frame_name();
}

template <typename Tdata>
int camera<Tdata>::remaining() {
  return -1;
}

template <typename Tdata>
int camera<Tdata>::size() {
  return -1;
}

template <typename Tdata>
void camera<Tdata>::set_silent() {
  silent = true;
}

////////////////////////////////////////////////////////////////
// internal methods

template <typename Tdata>
inline idx<Tdata> camera<Tdata>::postprocess() {
  fps_ms_elapsed = tfps.elapsed_milliseconds();
  cntfps++;
  if (narrow_dim >= 0)
    frame = frame.narrow(narrow_dim, narrow_size, narrow_off);
  if (fps_ms_elapsed > 1000) {
    fps_grab = cntfps * 1000 / (float) fps_ms_elapsed;
    tfps.restart(); // restart timer
    cntfps = 0; // reset counter
  }
  if (!bresize)
    return frame; // return original frame
  else { // or return a resized frame
    if (mresize)
      return image_mean_resize(frame, height, width, resize_mode);
    else
      return image_resize(frame, height, width, resize_mode);
  }
}

} // end namespace ebl

#endif /* CAMERA_HPP_ */
