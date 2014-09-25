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

#include "detector_gui.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // bbox parts

  void draw_bbox(bbox &bb, vector<string> &labels, uint h0, uint w0,
		 double dzoom, float transparency, bool scaled,
		 uint color_offset, bool show_class, bool show_conf) {
    ostringstream label;
    int classid, colorid;
    bool show_text = show_class || show_conf;
    classid = bb.class_id;
    colorid = (classid + color_offset) % (sizeof (color_list) / 3);
    int h = (int) (dzoom * bb.h0);
    int w = (int) (dzoom * bb.w0);
    int height = (int) (dzoom * bb.height);
    int width = (int) (dzoom * bb.width);
    if (!scaled) { // draw unscaled box
      h = (int) (dzoom * bb.i.h0);
      w = (int) (dzoom * bb.i.w0);
      height = (int) (dzoom * bb.i.height);
      width = (int) (dzoom * bb.i.width);
    }
    float conf = bb.confidence;
    label.str("");
    label.precision(2);
    if (show_class)
      label << ((uint) classid <labels.size()?labels[classid].c_str() : "***");
    if (show_class && show_conf) label << " ";
    if (show_conf) label << conf;
    ubyte transp = 127;//255;
    if (transparency > 0)
      transp = (ubyte) std::max((float) 50,
    				std::min((float) 255,
    					 (255 * (exp((conf - transparency + (float) .5) *12))
    						 / 60000)));
    draw_box(h0 + h, w0 + w, height, width,
	     color_list[colorid][0],
	     color_list[colorid][1], 
	     color_list[colorid][2], transp,
	     show_text ? new string((const char *)label.str().c_str()): NULL);
  }

  ////////////////////////////////////////////////////////////////
  // bbox parts

  void draw_bbox_parts(bbox_parts &bb, vector<string> &labels, uint h0, uint w0,
		       double dzoom) {
    std::vector<bbox_parts> &parts = bb.get_parts();
    for(uint i = 0; i < parts.size(); ++i) {
      bbox_parts &p = parts[i];
      draw_bbox(p, labels, h0, w0, dzoom, 0.0); // draw part
      draw_bbox_parts(p, labels, h0, w0, dzoom); // explore sub parts
    }
  }

} // end namespace ebl
