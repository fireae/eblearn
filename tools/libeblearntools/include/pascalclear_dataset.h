/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
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

#ifndef PASCALCLEAR_DATASET_H_
#define PASCALCLEAR_DATASET_H_

#include "dataset.h"
#include "xml_utils.h"

namespace ebl {

  //! The pascalclear_dataset class extract background patches from
  //! a dataset of type PASCAL.
  template <class Tdata> class pascalclear_dataset
    : public pascalbg_dataset<Tdata> {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors

    //! Initialize the dataset's name and other internal variables, but does
    //! not allocate data matrices, user must call alloc for that effect.
    //! outdims are the target output dimensions of each sample.
    //! inroot is the root directory from which we extract data.
    pascalclear_dataset(const char *name, const char *inroot,
			const char *outdir, const char *annotations);

    //! Destructor.
    virtual ~pascalclear_dataset();

    ////////////////////////////////////////////////////////////////
    // data

    //! Extract data from files into dataset.
    virtual bool extract();

  protected:

#ifdef __BOOST__ // disable some derived methods if BOOST not available
#ifdef __XML__ // disable some derived methods if XML not available

    ////////////////////////////////////////////////////////////////
    // internal methods

    //! process image given all bounding boxes.
    virtual void process_image(idx<ubyte> &img, std::vector<rect<int> >& bboxes,
			       const std::string &image_filename);

#endif /* __BOOST__ */
#endif /* __XML__ */

  protected:
    // base class members to be used ///////////////////////////////
    using pascal_dataset<Tdata>::annroot;
    using pascal_dataset<Tdata>::imgroot;
    using dataset<Tdata>::inroot;
    using dataset<Tdata>::display_extraction;
    using dataset<Tdata>::display_result;
    using dataset<Tdata>::outdims;
    using dataset<Tdata>::outdir;
    using dataset<Tdata>::sleep_display;
    using dataset<Tdata>::sleep_delay;
    using dataset<Tdata>::data_cnt;
    using dataset<Tdata>::extension;
  };

} // end namespace ebl

#include "pascalclear_dataset.hpp"

#endif /* PASCALCLEAR_DATASET_H_ */
