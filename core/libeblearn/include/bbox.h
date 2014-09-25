/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#ifndef BBOX_H_
#define BBOX_H_

#include "libidx.h"

namespace ebl {

//! Different ways of saving bboxes to file(s).
//! bbox_eblearn: output 1 bbox per line, 1 file for all images.
//!   one line contains: filename class_id confidence w0 h0 w1 h1
//! bbox_caltech: output 1 bbox per line, 1 file per image.
//!   one line contains: w0,h0,width,height,confidence (positive only?)
//! bbox_class: single output file with 1 line per bbox which values are
//!   comma-separated, no confidence nor bbox, only class id:
//!   filename; classid
enum t_bbox_saving { bbox_none = 0, bbox_all = 1,
                     bbox_eblearn = 2, bbox_caltech = 3,
                     bbox_class = 4};

// bbox //////////////////////////////////////////////////////////////////////

//! A bounding box class, based on the rect class.
class EXPORT bbox : public rect<float>, public smart_pointer {
public:
  //! Empty constructor (assign a new unique instance_id).
  bbox();
  //! Construct a bbox.
  bbox(float h0, float w0, float height, float width);
  //! Copy constructor.
  bbox(const bbox &other);
  //! Destructor.
  virtual ~bbox();

  //! Set instance_id to zero.
  static void init_instance_id();
  //! Assign a new unique instance_id to this bbox.
  //! This can be useful if this bbox was instantiated with the copy
  //! constructor which retains original instance_id.
  void new_instance_id();
  //! Accumulate all spatial values and confidence of b into this box.
  //! This can be useful to compute an average of boxes.
  void accumulate(bbox &b);
  //! Multiply all spatial values of this box by d.
  //! This can be useful to compute an average of boxes.
  void mul(float d);
  //! Narrow this bbox so that it doesn't overlap outside of image with
  //! dimensions imh x imw.
  void narrow(uint imh, uint imw);

  // printing ////////////////////////////////////////////////////////////////

  //! Returns a string describing this bbox in details.
  //! \param labels An optional vector of strings of class labels.
  std::string pretty(std::vector<std::string> *labels = NULL);

  // member variables ////////////////////////////////////////////////////////
public:
  int		class_id;               //!< object class
  float         confidence;             //!< detection confidence, 1 is the best.
  uint          nacc;                   //!< Number of accumulated boxes.
  // original map //////////////////////////////////////////////
  using rect<float>::h0;                //!< height of top left pixel
  using rect<float>::w0;                //!< width of top left pixel
  using rect<float>::height;            //!< height of bounding box in original image
  using rect<float>::width;             //!< width of bounding box in original image
  // scale /////////////////////////////////////////////////
  double	scaleh;                 //!< scale factor at which object was detected
  double	scalew;                 //!< scale factor at which object was detected
  int		iscale_index;           //!< Input scale index of bbox.
  int		oscale_index;           //!< Output scale index of bbox.
  int		output_index;           //!< Output index of bbox.
  // input map /////////////////////////////////////////////////
  rect<float>   i;                      //!< Box in pyramid input space.
  std::vector<rect<float> > mi;         //!< All input boxes in inputs.
  rect<float>   i0;                     //!< Non-transformed box in pyramid input space.
  int           iheight;                //!< scaled input image height
  int           iwidth;                 //!< scaled input image width
  // output map ////////////////////////////////////////////////
  rect<int>     o;                      //!< Box in pyramid output space.
  int           oheight;                //!< height of network's output map
  int           owidth;                 //!< width of network's output map
  svector<bbox> children;               //!< Boxes that compose this box.
  // temporary
  double        tmp_score;

protected:
  uint instance_id;    //<! A unique id for this bbox.
  static uint iid_cnt;    //<! A unique id for this bbox.
};

//! A bounding box that can be composed from multiple bounding boxes.
class EXPORT bbox_parts : public bbox {
public:
  //! Empty constructor (assign a new unique instance_id).
  bbox_parts();
  //! Copy constructor from another bbox.
  bbox_parts(const bbox &other);
  //! Copy constructor.
  bbox_parts(const bbox_parts &other);
  //! Destructor.
  virtual ~bbox_parts();
  //! Declare another bbox to be part of this bbox.
  void add_part(const bbox &p);
  //! Declare another bbox to be part of this bbox.
  void add_part(const bbox_parts &p);
  //! Returns true if this and bbp share at least 1 part,
  //! using instance_id to identify parts uniquely.
  bool share_parts(bbox_parts &bbp);
  //! Return a reference to the parts vector.
  std::vector<bbox_parts>& get_parts();
protected:
  std::vector<bbox_parts> parts;
};

EXPORT bbox mean_bbox(std::vector<bbox*> &boxes, float bonus_per_bbox,
                      int classid);

//! bbox stream operator.
EXPORT std::ostream& operator<<(std::ostream& out, const bbox& b);
//! bbox stream operator.
EXPORT std::string& operator<<(std::string& s, const bbox& b);

// bboxes ////////////////////////////////////////////////////////////////////

//! A collection of bounding boxes, that can be saved to multiple formats,
//! and grouped by image.
class EXPORT bboxes : public svector<bbox> {
public:
  //! Initialize parameters.
  //! \param saving_type Determines the formats save() will save data.
  bboxes(t_bbox_saving saving_type = bbox_all, std::string *outdir_ = NULL,
         std::ostream &out = std::cout, std::ostream &err = std::cerr);
  //! Destructor. Delete bbox if own_boxes is true.
  virtual ~bboxes();

  //! Create a new group of bounding boxes. Add() will then put new bboxes
  //! into that group (unless index is less than boxes.size()).
  //! \param dims The image size associated with this group.
  //! \param index If negative (default), the order is the addition order,
  //!   otherwise use the index to order the bboxes.
  //! \param name An optional name for the new group.
  void new_group(idxdim &dims, std::string *name = NULL,
                 int index = -1);

  //! Return an entire vector of pointers to bbox for group with name
  //! 'name'. This throws an exception if the group is not found.
  bboxes* get_group(const std::string &name);

  //! Return the image dimensions associated with a group of bbox with
  //! name 'name'. This throws an exception if the group is not found.
  idxdim get_group_dims(const std::string &name);

  //! Add a new bbox to the collection (in last group or the group with index
  //! 'index').
  //! \param dims The image size associated with this bbox.
  //! \param index If negative (default), the order is the addition order,
  //!   otherwise use the index to order the bboxes.
  //! \param name An optional name for the new group.
  void add(bbox *b, idxdim &dims, std::string *name = NULL, int index = -1);
  //! Add a new bbox to the collection (in last group or the group with index
  //! 'index').
  //! \param dims The image size associated with this bbox.
  //! \param index If negative (default), the order is the addition order,
  //!   otherwise use the index to order the bboxes.
  //! \param name An optional name for the new group.
  void add(bbox &b, idxdim &dims, std::string *name = NULL, int index = -1);
  //! Create a new group and add a set of bbox to this group.
  //! \param dims The image size associated with this group.
  //! \param index If negative (default), the order is the addition order,
  //!   otherwise use the index as the group's index in the vector of groups.
  //! \param name An optional name for the new group.
  void add(bboxes &bbs, idxdim &dims, std::string *name = NULL, int index = -1);
  //! Create a new group and add a set of bbox to this group.
  //! \param dims The image size associated with this group.
  //! \param index If negative (default), the order is the addition order,
  //!   otherwise use the index as the group's index in the vector of groups.
  //! \param name An optional name for the new group.
  void add(std::vector<bbox> &bbs, idxdim &dims, std::string *name = NULL,
           int index = -1);
  //! Returns the subset of current bboxes that exclude bboxes in 'other'.
  bboxes exclude(bboxes &other);

  // loading /////////////////////////////////////////////////////////////////

  //! Load the bboxes found the file 'filename' in the eblearn bbox format.
  void load_eblearn(const std::string &filename);

  // saving //////////////////////////////////////////////////////////////////

  //! Save boxes using the internal parameters initialized by the constructor.
  //! \param dir Output directory. If null (default), use internal directory
  //!   initialized by the constructor.
  void save(std::string *dir = NULL);
  //! Save boxes eblearn-style in dir.
  //! \param dir Output directory. If null (default), use internal directory
  //!   initialized by the constructor.
  void save_eblearn(std::string *dir = NULL);
  //! Save boxes caltech-style in dir.
  //! \param dir Output directory. If null (default), use internal directory
  //!   initialized by the constructor.
  void save_caltech(std::string *dir = NULL);
  //! Save boxes class-style in dir.
  //! \param dir Output directory. If null (default), use internal directory
  //!   initialized by the constructor.
  void save_class(std::string *dir = NULL);

  // sorting /////////////////////////////////////////////////////////////////

  //! Sort all boxes in decreasing order of their confidence
  //! (highest confidence first).
  void sort_by_confidence();
  //! Sort all 'inputs' and internal boxes based on input l2 distances
  //! (most different pairs of input first) in a stable way (original
  //! ordering is kept).
  template <typename T>
      void sort_by_difference(svector<midx<T> >& inputs);
  //! Sort all boxes given the score of a * confidence - b * distance
  //! where distance is the euclidean distance of each box and center rect.
  void sort_by_confidence_and_distance(rect<float> &center,
                                       double a, double b);
  //! Returns the single most confident box or NULL if empty.
  bbox *get_most_confident();
  //! Returns the most confident boxes, potentially multiple ones
  //! if several rank 1st with the same score.
  bboxes get_most_confidents();

  // scaling /////////////////////////////////////////////////////////////////

  //! Normalize widths of boxes w.r.t. to their height with ratio 'woverh'.
  void normalize_widths(float woverh);
  //! Scale all boxes while keeping the same center, if 'hfact' or 'wfact'
  //! is different than 1.0.
  void scale_centered(float hfact, float wfact);

  // thresholding ////////////////////////////////////////////////////////////

  //! Remove all elements with confidence strictly below threshold.
  void threshold(float threshold);

  // printing ////////////////////////////////////////////////////////////////

  //! Returns a string describing the number of bboxes and number of groups.
  std::string describe();
  //! Print each bbox with a brief description to 'mout'.
  void print();
  //! Print the saving type of boxes.
  void print_saving_type();
  //! Returns a string describing each bbox in details.
  //! \param labels An optional vector of strings of class labels.
  std::string pretty(std::vector<std::string> *labels = NULL);
  //! Returns a string with a short description of each bbox.
  //! \param labels A vector of strings of class labels.
  std::string pretty_short(std::vector<std::string> &labels);
  //! Returns a string with a short description of each bbox (without label
  //! strings).
  std::string str();

  // Making hidden operator visible.
  using svector<bbox>::operator=;

  // member variables ////////////////////////////////////////////////////////
private:
  std::ostream        	*mout;	//! output stream.
  std::ostream        	*merr;	//! error output stream.
  std::vector<svector<bbox>*>	 grouped_boxes;
  std::vector<std::string>	 group_names;
  std::vector<idxdim>		 group_dims;
  std::string		 	 outdir;
  t_bbox_saving        	 saving_type;
};

} // end namespace ebl

#include "bbox.hpp"

#endif /* BBOX_H_ */
