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

#ifndef PASCAL_XML_H_
#define PASCAL_XML_H_

#include "dataset.h"
#include "xml_utils.h"
#include "bbox.h"

#define XML_PATTERN ".*[.]xml"

namespace ebl {

  //! A class capable of extracting pascal format bounding boxes from xml files.
  class EXPORT pascal_xml {
  public:
    pascal_xml();
    virtual ~pascal_xml();

    // bboxes extraction ///////////////////////////////////////////////////////

    //! Returns bboxes found in file 'xml_fname'.
    static bboxes get_bboxes(const std::string &xml_fname,
                             std::vector<std::string> &labels);
    //! Returns bboxes found in file 'xml_fname' and filtered with
    //! these parameters.
    //! \param filtered This will be filled with the rejected boxes.
    static bboxes get_filtered_bboxes
      (const std::string &xml_fname, float minvisibility, float min_aspect_ratio,
       float max_aspect_ratio, idxdim &mindims, idxdim &minborders,
       std::vector<std::string> &included, bboxes &filtered,
       std::vector<std::string> *labels = NULL);

    // xml extraction //////////////////////////////////////////////////////////

    //! Gets all properties from an xml file.
    //! \param cropr If image is cropped to cropr, adjust coordinates
    //!    of objects in the image, do nothing if NULL.
    //! \param ignore If true, object is not used as sample.
    static bool get_properties(const std::string &imgroot,
			       const std::string &xmlfile, std::string &image_filename,
			       std::string &image_fullname, std::string &folder,
			       int &height, int &width, int &depth,
			       std::vector<object*> &objs, rect<int> **cropr,
			       bool ignored = false);
#ifdef __XML__
    //! Gets all objects found in node list l.
    //! \param cropr If image is cropped to cropr, adjust coordinates
    //!    of objects in the image, do nothing if NULL.
    //! \param ignore If true, object is not used as sample.
    static void get_objects(Node::NodeList &l, std::vector<object*> &objs,
			    rect<int> *cropr = NULL, bool ignore = false);
#endif /* __XML__ */
  };

} // end namespace ebl

#endif /* PASCAL_XML_H_ */
