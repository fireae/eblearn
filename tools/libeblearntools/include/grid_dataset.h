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

#ifndef GRID_DATASET_H_
#define GRID_DATASET_H_

#include "dataset.h"

namespace ebl {

//! The grid_dataset class divides each input image into a grid, making
//! each resulting patch a new training sample.
template <class Tdata> class grid_dataset : public dataset<Tdata> {
 public:

  ////////////////////////////////////////////////////////////////
  // constructors

  //! Initialize the dataset's name and other internal variables, but does
  //! not allocate data matrices, user must call alloc for that effect.
  //! outdims are the target output dimensions of each sample.
  //! inroot is the root directory from which we extract data.
  //! ignore_diff ignores difficult objects if true.
  grid_dataset(const char *name, const char *inroot = NULL,
               uint cellh = 32, uint cellw = 32);

  //! Destructor.
  virtual ~grid_dataset();

  ////////////////////////////////////////////////////////////////
  // data

  //! count how many samples are present in dataset files to be compiled.
  virtual intg count_samples();

  //! add sample d to the data with label class_name
  //! (and converting from Toriginal to Tdata type).
  //! r is an optional region of interest rectangle in the image d.
  virtual bool add_data(idx<Tdata> &d, const std::string &class_name,
                        const char *filename = NULL,
                        const rect<int> *r = NULL,
                        std::pair<uint,uint> *center = NULL);

 protected:
  // base class members to be used ///////////////////////////////
  uint cell_height;
  uint cell_width;
};

} // end namespace ebl

#include "grid_dataset.hpp"

#endif /* GRID_DATASET_H_ */
