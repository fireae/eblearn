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

#ifndef DETECTOR_GUI_H_
#define DETECTOR_GUI_H_

#include "libidxgui.h"
#include "libeblearn.h"
#include "detector.h"
#include "ebl_arch_gui.h"
#include "utils.h"

#include <deque>

using namespace std;

namespace ebl {

//! A pre-defined color list
//EXPORT extern ubyte color_list[12][3];

EXPORT void draw_bbox_parts(bbox_parts &bb, vector<string> &labels, uint h0,
                            uint w0, double dzoom);
//! \param scaled If false, scale box with respect to its original scale,
//!           otherwise draw its unscaled size.
EXPORT void draw_bbox(bbox &bb, vector<string> &labels, uint h0, uint w0,
                      double dzoom, float transparency, bool scaled = true,
                      uint color_offset = 0, bool show_class = true,
                      bool show_conf = true);

// detector_gui ////////////////////////////////////////////////////////////////

//! The display class of class detector.
template <typename T> class EXPORT detector_gui {
public:
  //! Constructor.
  //! \param show_detqueue If true, show the queue of the latest detected
  //!        windows with step 'step'.
  //! \param step Step for show_detqueue.
  //! \param qheight Number of rows to show for show_detqueue.
  //! \param qwidth Number of cols to show for show_detqueue.
  //! \param show_detqueue2 If true, show another queue of the latest detected
  //!        windows with step 'step2'.
  //! \param step2 Step for show_detqueue2.
  //! \param qheight2 Number of rows to show for show_detqueue2.
  //! \param qwidth2 Number of cols to show for show_detqueue2.
  //! \param draw_extracted 0: do not draw, 1: draw preprocessed,
  //!   2: draw original image extractions
  detector_gui(uint draw_extracted = 0,
               bool show_detqueue = false, uint step = 1, uint qheight = 5,
               uint qwidth = 5, bool show_detqueue2 = false, uint step2 = 1,
               uint qheight2 = 5, uint qwidth2 = 5, bool show_class = true,
               bool show_conf = true);
  //! Destructor.
  virtual ~detector_gui();

  //! displays only the output of the classifier after a a call to
  //! detector::fprop(img, zoom, threshold, objsize) at coordinates
  //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified,
  //! use that window, otherwise create a new window and reuse it.
  //! <wname> is an optional window title.
  template <typename Tin>
      bboxes& display(detector<T> &cl, idx<Tin> &img,
		      const char *frame_name = NULL, int frame_id = -1,
		      uint h0 = 0, uint w0 = 0, double dzoom = 1.0, T vmin = 0,
		      T vmax = 0, int wid = -1, const char *wname = NULL,
		      float transparency = 1.0);

  template <typename Tin>
      void display_groundtruth(detector<T> &cl, idx<Tin> &img,
			       bboxes &groundtruth, bboxes &filtered,
			       bboxes &nonfiltered, bboxes &pos, bboxes &neg,
			       svector<midx<T> > &pp_pos,
			       svector<midx<T> > &pp_neg,
			       uint h0 = 0, uint w0 = 0, double dzoom = 1.0,
			       T vmin = 0, T vmax = 0, int wid = -1);

  //! displays only the output of the classifier after a a call to
  //! detector::fprop(img, zoom, threshold, objsize) at coordinates
  //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified,
  //! use that window, otherwise create a new window and reuse it.
  //! <wname> is an optional window title.
  template <typename Tin>
      static void display_minimal(idx<Tin> &img, bboxes& vb,
				  vector<string> &labels,
				  uint &h0, uint &w0,
				  double dzoom = 1.0, T vmin = 0, T vmax = 0,
				  int wid = -1, bool show_parts = false,
				  float transparency = 1.0,
				  bool show_class = true,
				  bool show_conf = true,
				  bboxes *bb2 = NULL);

  //! displays only the output of the classifier after a a call to
  //! detector::fprop(img, zoom, threshold, objsize) at coordinates
  //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified,
  //! use that window, otherwise create a new window and reuse it.
  //! <wname> is an optional window title.
  template <typename Tin>
      bboxes& display_input(detector<T> &cl, idx<Tin> &img,
			    const char *frame_name = NULL, int frame_id = -1,
			    uint h0 = 0, uint w0 = 0,
			    double dzoom = 1.0, T vmin = 0,
			    T vmax = 0,
			    int wid = -1, const char *wname = NULL,
			    float transparency = 1.0);

  void display_preprocessed(svector<midx<T> > &pp, bboxes &bbs,
                            vector<string> &labels, uint &h0, uint &w0,
                            double dzoom = 1.0, T vmin = 0, T vmax = 0,
                            uint wmax = 3000);

  //! display the regular input/output display but also the inputs and outputs
  //! corresponding to each scale, corresponding to a call to
  //! detector::fprop(img, zoom, threshold, objsize) at coordinates
  //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified,
  //! use that window, otherwise create a new window and reuse it.
  //! <wname> is an optional window title.
  template <typename Tin>
      bboxes& display_inputs_outputs(detector<T> &cl, idx<Tin> &img,
				     const char *frame_name = NULL,
				     int frame_id = -1,
				     uint h0 = 0, uint w0 = 0,
				     double dzoom = 1.0, T vmin = 0,
				     T vmax = 0, int wid = -1,
				     const char *wname = NULL,
				     T in_vmin = 0, T in_vmax = 255,
				     float transparency = 1.0,
				     uint wmax = 3000);

  void display_inputs(detector<T> &cl, uint &h0, uint &w0,
                      bboxes &bb, double dzoom = 1.0, T vmin = 0, T vmax = 0,
                      float transparency = 1.0);
  void display_outputs(detector<T> &cl, uint &h0, uint &w0,
                       bboxes &bb, double dzoom = 1.0, T vmin = 0, T vmax = 0,
                       float transparency = 1.0);

  //! display all, display_inputs_outputs and the internal states of the fprop
  //! on the first scale, corresponding to a call to
  //! detector::fprop(img, zoom, threshold, objsize) at coordinates
  //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified,
  //! use that window, otherwise create a new window and reuse it.
  //! <wname> is an optional window title.
  template <typename Tin>
      bboxes& display_all(detector<T> &cl, idx<Tin> &img,
			  const char *frame_name = NULL, int frame_id = -1,
			  uint h0 = 0, uint w0 = 0, double dzoom = 1.0,
			  T vmin = 0, T vmax = 0,
			  int wid = -1,const char *wname = NULL);

  //! displays all the current state of the classifier.
  //! If a window id <wid> is specified,
  //! use that window, otherwise create a new window and reuse it.
  //! <wname> is an optional window title.
  template <typename Tin>
      void display_current(detector<T> &cl, idx<Tin> &sample,
			   int wid = -1, const char *wname = NULL,
                           double dzoom = 1.0);

  void set_mask_class(const char *name, T threshold);


  // private methods ///////////////////////////////////////////////////////////
private:
  void update_and_display_queue(deque<idx<T> > &queue, uint step,
                                uint qheight, uint qwidth,
                                vector<idx<T> > &new_detections,
                                uint detcnt, uint &h0, uint &w0,
                                double dzoom);

  // members ///////////////////////////////////////////////////////////////////
private:
  int	 display_wid;                   //!< window id
  int	 display_wid_fprop;             //!< window id for fprop
  int	 display_wid_gt;                //!< window id for groundtruth
  uint   draw_extracted;
  bool   show_detqueue;                 //!< show queue of last detection or not
  bool   show_detqueue2;                //!< show queue of last detection or not
  uint   step;                          //!< step for detqueue
  uint   step2;                         //!< step for detqueue
  uint   qheight;                       //!< height of queue display
  uint   qwidth;                        //!< width of queue display
  uint   qheight2;                      //!< height of queue2 display
  uint   qwidth2;                       //!< width of queue2 display
  deque<idx<T> > detqueue;              //!< queue of last detections
  deque<idx<T> > detqueue2;             //!< queue of last detections
  uint   detcnt;                        //!< counter of all detections
  string mask_class;
  T      mask_threshold;
  bool   show_class;                    //!< Print class name in bbox.
  bool   show_conf;                     //!< Print confidence in bbox.
};

} // end namespace ebl

#include "detector_gui.hpp"

#endif /* DETECTOR_GUI_H_ */
