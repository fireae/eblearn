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

#ifndef PASCALFULL_DATASET_H_
#define PASCALFULL_DATASET_H_

#include "dataset.h"
#include "xml_utils.h"

namespace ebl {

  //! The pascalfull_dataset class extracts full images instead of bounding
  //! boxes and saves each image into the outdir. It does not compile the images
  //! since they have no labels.
  //! This is useful if one wants to use the full original images of pascal
  //! but exclude some classes, adding those images as a background class
  //! into another dataset and then calling the regular dataset compiler.
  template <class Tdata> class pascalfull_dataset
    : public pascal_dataset<Tdata> {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors

    //! Initialize the dataset's name and other internal variables, but does
    //! not allocate data matrices, user must call alloc for that effect.
    //! outdims are the target output dimensions of each sample.
    //! inroot is the root directory from which we extract data.
    pascalfull_dataset(const char *name, const char *inroot,
		       const char *outdir, const char *annotations);

    //! Destructor.
    virtual ~pascalfull_dataset();

  protected:
    
#ifdef __XML__ // disable some derived methods if XML not available
    
    ////////////////////////////////////////////////////////////////
    // internal methods

    //! process an xml file.
    virtual bool process_xml(const std::string &xmlfile);

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
    using dataset<Tdata>::print_stats;
    using dataset<Tdata>::data_cnt;
    using dataset<Tdata>::extension;
    using dataset<Tdata>::exclude;
  };

} // end namespace ebl

#include "pascalfull_dataset.hpp"

#endif /* PASCALFULL_DATASET_H_ */
