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

#ifndef PATCH_DATASET_H_
#define PATCH_DATASET_H_

#include "dataset.h"

namespace ebl {

//! The patch_dataset class extracts random patches at differents scales
//! from images. This can be used to generate background patches from
//! a set of images known to exclude the non-background classes.
//! All possible patches at each specified scale and matching the
//! output size are found, then randomized and only the first n
//! patches of the random list are saved per image, n being specified
//! in the constructor as 'max_folders' variable.
template <class Tdata> class patch_dataset : public dataset<Tdata> {
 public:

  ////////////////////////////////////////////////////////////////
  // constructors

  //! Initialize the dataset's name and other internal variables, but does
  //! not allocate data matrices, user must call alloc for that effect.
  //! outdims are the target output dimensions of each sample.
  //! inroot is the root directory from which we extract data.
  //! @param max_folders The first 'max_folders' patches of each image's
  //!         random list of patches are saved in a different folder each.
  patch_dataset(const char *name, const char *inroot, const char *outdir,
                uint max_folders = 1);

  //! Destructor.
  virtual ~patch_dataset();

  ////////////////////////////////////////////////////////////////
  // data

  //! Extract patches from image.
  virtual bool add_mdata(midx<Tdata> &d, const t_label label,
                         const std::string *class_name,
                        const char *filename = NULL,
                        const rect<int> *r = NULL,
                         std::pair<int,int> *center = NULL,
                        const rect<int> *visr = NULL,
                        const rect<int> *cropr = NULL,
                         const std::vector<object*> *objs = NULL,
                        const jitter *jittforce = NULL);

 protected:

  //! save patches into directory outdir, creating 1 subdirectory per patch
  //! until reaching max_folders, then filling last folder with remaining
  //! patches, using filename as base filename for patches filenames.
  void save_patches(std::vector<idx<Tdata> > &patches, const std::string &outdir,
                    uint max_folders, const std::string &filename,
                    double scale, const t_label label);

 protected:
  uint max_folders; // maximum number of patch directories
  // base class members to be used ///////////////////////////////
  using dataset<Tdata>::inroot;
  using dataset<Tdata>::allocated;
  using dataset<Tdata>::display_extraction;
  using dataset<Tdata>::display_result;
  using dataset<Tdata>::outdims;
  using dataset<Tdata>::outdir;
  using dataset<Tdata>::sleep_display;
  using dataset<Tdata>::sleep_delay;
  using dataset<Tdata>::print_stats;
  using dataset<Tdata>::data_cnt;
  using dataset<Tdata>::extension;
  using dataset<Tdata>::scales;
  using dataset<Tdata>::max_data;
  using dataset<Tdata>::max_data_set;
  using dataset<Tdata>::save_mode;
  using dataset<Tdata>::original_bbox;
  using dataset<Tdata>::nopadded;
};

} // end namespace ebl

#include "patch_dataset.hpp"

#endif /* PATCH_DATASET_H_ */
