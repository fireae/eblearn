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

#ifndef GRID_DATASET_HPP_
#define GRID_DATASET_HPP_

#include <algorithm>

namespace ebl {

////////////////////////////////////////////////////////////////
// constructors & initializations

template <class Tdata>
grid_dataset<Tdata>::grid_dataset(const char *name_,
                                  const char *inroot_, uint cellh, uint cellw)
    : dataset<Tdata>(name_, inroot_), cell_height(cellh), cell_width(cellw) {
  std::cout << "Grid dataset: using each " << cellh << "x" << cellw
       << " patch as new sample." << std::endl;
}

template <class Tdata>
grid_dataset<Tdata>::~grid_dataset() {
}

////////////////////////////////////////////////////////////////
// data extraction

template <class Tdata>
intg grid_dataset<Tdata>::count_samples() {
  // TODO: implement finding biggest sample size and divide by number of cells
  // and return the maximum possible number of samples
  dataset<Tdata>::count_samples();
  this->total_samples *= 100;
  return this->total_samples;
}

template <class Tdata>
bool grid_dataset<Tdata>::
add_data(idx<Tdata> &d, const std::string &class_name,
         const char *filename, const rect<int> *r,
         std::pair<uint,uint> *center) {
  bool ret;

  for (uint i = 0; i <= d.dim(0) - cell_height; i += cell_height) {
    for (uint j = 0; j <= d.dim(1) - cell_width; j += cell_width) {
      rect<int> roi(i, j, cell_height, cell_width);
      std::cout << "roi: " << roi << std::endl;
      t_label label = this->get_label_from_class(class_name);
      midx<Tdata> dd(1);
      dd.mset(d, 0);
      ret = dataset<Tdata>::add_mdata(dd, label, &class_name, filename, &roi);
      dd.clear();
    }
  }
  return true;
}

} // end namespace ebl

#endif /* GRID_DATASET_HPP_ */
