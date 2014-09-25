/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet   *
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

#ifndef CAMERA_DATASOURCE_HPP_
#define CAMERA_DATASOURCE_HPP_

#include <sstream>

namespace ebl {

template <typename Tdata, typename Tlabel>
camera_datasource<Tdata,Tlabel>::
camera_datasource(configuration &conf, bool randomize_, uint npasses_,
                  std::ostream &o, std::ostream &e)
    : camera<Tdata>(-1, -1, o, e), randomize(randomize_), npasses(npasses_) {
  if (npasses == 0) eblerror("number of passes must be >= 1");
  // load datasource from conf
  std::string dataset_root = conf.get_string("camera_datasource_root");
  out << "Initializing datasource camera from " << dataset_root << std::endl;
  ds.init_root(dataset_root.c_str());
  ds.pretty();
  ds.init_epoch();
  log.init(ds.get_nclasses());
  save_incorrect = conf.try_get_uint("camera_datasource_save_incorrect", 0);
  save_correct = conf.try_get_uint("camera_datasource_save_correct", 0);
  save_root = conf.try_get_string("camera_datasource_save_root", "");
}

template <typename Tdata, typename Tlabel>
camera_datasource<Tdata,Tlabel>::~camera_datasource() {
}

// frame grabbing //////////////////////////////////////////////////////////////

template <typename Tdata, typename Tlabel>
void camera_datasource<Tdata,Tlabel>::next() {
  ds.next();
  frame_id_++;
}

template <typename Tdata, typename Tlabel>
void camera_datasource<Tdata,Tlabel>::previous() {
  eblerror("not implemented");
}

template <typename Tdata, typename Tlabel>
idx<Tdata> camera_datasource<Tdata,Tlabel>::grab() {
  next();
  ds.fprop1_data(frame_raw);
  frame = frame_raw.shift_dim(0, 2);
  if (!silent)
    out << frame_id_ << "/" << size() << ": grabbed " << frame.info()
        << std::endl;
  return this->postprocess();
}

template <typename Tdata, typename Tlabel>
void camera_datasource<Tdata,Tlabel>::skip(uint n) {
  if (n == 0) return ;
  uint i;
  for (i = 0; i < n; ++i) {
    if (empty()) {
      i--;
      break ;
    }
    next();
  }
  if (!silent)
    std::cout << "Skipped " << i << " frames." << std::endl;
}

template <typename Tdata, typename Tlabel>
bool camera_datasource<Tdata,Tlabel>::empty() {
  if (ds.epoch_done()) {
    if (npasses > 0) {
      npasses--;
      if (npasses > 0) ds.seek_begin(); // reset to begining.
    }
    if (npasses == 0) return true; // we did all passes, stop.
  }
  return false;
}

template <typename Tdata, typename Tlabel>
int camera_datasource<Tdata,Tlabel>::remaining() {
  return (int) (size() - frame_id_);
}

template <typename Tdata, typename Tlabel>
int camera_datasource<Tdata,Tlabel>::size() {
  return (int) ds.size();
}

template <typename Tdata, typename Tlabel>
void camera_datasource<Tdata,Tlabel>::log_answers(bboxes &bb) {
  // get true label
  ds.fprop_label(label);
  // compare with answers
  bool correct = false;
  int answer = 0;
  if (bb.size() > 0) {
    EDEBUG("before sort by dist: " << bb.str());

    rect<float> c(0, 0, 32, 32);
    for (bboxes::iterator i = bb.begin(); i != bb.end(); ++i)
      i->confidence *= (1 - .1 * (i->center_distance(c)))
          * (i->area() / c.area());

    bb.sort_by_confidence();
    // bb.sort_by_confidence_and_distance(c, 1, 1/(double)8);
    answer = bb[0].class_id;
    EDEBUG("after sort by dist: " << bb.str());

    // if (bb.size() >= 2 && bb[2].class_id == (int) label.gget())
    //   answer = label.gget();
    // if (bb.size() >= 3 && bb[3].class_id == (int) label.gget())
    //   answer = label.gget();

      //correct = true;
  }
  correct = label.gget() == answer;
  if (!silent && !correct)
    std::cout << "incorrect answer, expected " << label.gget()
              << " but answered " << answer << std::endl;
  // remember in dataset
  ds.set_sample_correctness(correct);
  // log answer
  //  log.update(frame_id_, correct, 0);
  log.update(frame_id_, (uint) label.gget(), answer, 0);
  if (frame_id_ % 100 == 0) {
    log.display(frame_id_, ds.name());
  }
  // save correct samples
  if (save_correct && frame_id_ % save_correct == 0) {
    std::string fname;
    fname << save_root << "/correct" << frame_id_;
    ds.save_correct(fname.c_str());
  }
  // save incorrect samples
  if (save_incorrect && frame_id_ % save_incorrect == 0) {
    std::string fname;
    fname << save_root << "/incorrect" << frame_id_;
    ds.save_incorrect(fname.c_str());
  }
}

} // end namespace ebl

#endif /* CAMERA_DATASOURCE_HPP_ */
