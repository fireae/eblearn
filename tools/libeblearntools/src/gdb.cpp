/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include "gdb.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // instantiations of blas functions, useful for debugging under gdb

  double idx_max_double(idx<double> &m) {
    return idx_max<double>(m);
  }
  
  double idx_min_double(idx<double> &m) {
    return idx_min<double>(m);
  }
  
  float idx_max_float(idx<float> &m) {
    return idx_max<float>(m);
  }
  
  float idx_min_float(idx<float> &m) {
    return idx_min<float>(m);
  }
  
#ifdef __GUI__
  ////////////////////////////////////////////////////////////////
  // instantiations of draw_matrix, useful for debugging under gdb

  void draw_matrix_double(idx<double> &im, unsigned int h0, unsigned int w0, 
			  double zoomh, double zoomw, double minv, double maxv){
    draw_matrix<double>(im, h0, w0, zoomh, zoomw, minv, maxv);
  }
  
  void draw_matrix_float(idx<float> &im, unsigned int h0, unsigned int w0, 
			 double zoomh, double zoomw, float minv, float maxv) {
    draw_matrix<float>(im, h0, w0, zoomh, zoomw, minv, maxv);
  }
    
  void draw_matrix_ubyte(idx<ubyte> &im, unsigned int h0, unsigned int w0, 
			 double zoomh, double zoomw, ubyte minv, ubyte maxv) {
    draw_matrix<ubyte>(im, h0, w0, zoomh, zoomw, minv, maxv);
  }

#endif
  
  ////////////////////////////////////////////////////////////////
  // global idx variables, useful for debugging under gdb
  
  idx<double> gdb_idx_double1;
  idx<double> gdb_idx_double2;
  idx<float> gdb_idx_float1;
  idx<float> gdb_idx_float2;
  idx<ubyte> gdb_idx_ubyte1;
  idx<ubyte> gdb_idx_ubyte2;  

} // namespace ebl
