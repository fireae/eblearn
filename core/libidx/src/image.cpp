/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

// tell header that we are in the libidx scope
#define LIBIDX

#ifndef __WINDOWS__
#include <inttypes.h>
#endif

#include "image.h"
#include "idxops.h"
#include "idx.h"
#include "stl.h"

namespace ebl {

  bool collide_rect(int x1, int y1, int w1, int h1,
		    int x2, int y2, int w2, int h2) {
    int x,y,w,h;
    x = std::max(x1,x2);
    y = std::max(y1,y2);
    w = std::min(x1+w1,x2+w2)-x;
    h = std::min(y1+h1,y2+h2)-y;
    if (w>0 && h>0)
      return true;
    else
      return false;
  }

  double common_area(int x1, int y1, int w1, int h1,
		     int x2, int y2, int w2, int h2) {
    int x,y,w,h;
    x = std::max(x1,x2);
    y = std::max(y1,y2);
    w = std::min(x1+w1,x2+w2)-x;
    h = std::min(y1+h1,y2+h2)-y;
    if((w <= 0)||(h <= 0)) return 0;
    else return (double)((w*h)/(w1*h1));

  }

  ////////////////////////////////////////////////////////////////
  // interpolation

  void image_interpolate_bilin(ubyte* background, ubyte *pin,
			       int indimi, int indimj, int inmodi, int inmodj,
			       int ppi, int ppj,
			       ubyte* out, int outsize) {
    int li0, lj0;
    register int li1, lj1;
    int deltai, ndeltai;
    int deltaj, ndeltaj;
    register ubyte *pin00;
    register ubyte *v00, *v01, *v10, *v11;
    li0 = ppi >> 16;
    li1 = li0+1;
    deltai = ppi & 0x0000ffff;
    ndeltai = 0x00010000 - deltai;
    lj0 = ppj  >> 16;
    lj1 = lj0+1;
    deltaj = ppj & 0x0000ffff;
    ndeltaj = 0x00010000 - deltaj;
    pin00 = (ubyte*)(pin) + inmodi * li0 + inmodj * lj0;
    if ((li1>0)&&(li1<indimi)) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==0) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==indimi) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = background;
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else {
      v00 = background;
      v01 = background;
      v11 = background;
      v10 = background;
    }
    if (outsize >= 1)
      *out = (ndeltaj * (( *v10*deltai + *v00*ndeltai )>>16) +
	      deltaj  * (( *v11*deltai + *v01*ndeltai )>>16))>>16;
    if (outsize >= 2)
      *(out + 1) = (ndeltaj * (( v10[1]*deltai + v00[1]*ndeltai )>>16) +
		    deltaj  * (( v11[1]*deltai + v01[1]*ndeltai )>>16))>>16;
    if (outsize >= 3)
      *(out + 2) = (ndeltaj * (( v10[2]*deltai + v00[2]*ndeltai )>>16) +
		    deltaj  * (( v11[2]*deltai + v01[2]*ndeltai )>>16))>>16;
    if (outsize >= 4)
      *(out + 3) = (ndeltaj * (( v10[3]*deltai + v00[3]*ndeltai )>>16) +
		    deltaj  * (( v11[3]*deltai + v01[3]*ndeltai )>>16))>>16;
    if (outsize >= 5) {
      eblerror("not implemented for more than 4 channels ("
	       << outsize << " channels in " << indimi << "x" << indimj
	       << " image)");
    }
  }

  void image_interpolate_bilin(float* background, float *pin,
			       int indimi, int indimj,
			       int inmodi, int inmodj,
			       int ppi, int ppj,
			       float* out, int outsize) {
    int li0, lj0;
    register int li1, lj1;
    float deltai, ndeltai;
    float deltaj, ndeltaj;
    register float *pin00;
    register float *v00, *v01, *v10, *v11;
    li0 = ppi >> 16;
    li1 = li0+1;
    deltai = 0.0000152587890625 * (float)(ppi & 0x0000ffff);
    ndeltai = 1.0 - deltai;
    lj0 = ppj  >> 16;
    lj1 = lj0+1;
    deltaj = 0.0000152587890625 * (float)(ppj & 0x0000ffff);
    ndeltaj = 1.0 - deltaj;
    pin00 = (float*)(pin) + inmodi * li0 + inmodj * lj0;
    if ((li1>0)&&(li1<indimi)) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==0) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==indimi) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = background;
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else {
      v00 = background;
      v01 = background;
      v11 = background;
      v10 = background;
    }
    if (outsize >= 1)
      *out = ndeltaj * (*v10*deltai + *v00*ndeltai) +
	     deltaj  * (*v11*deltai + *v01*ndeltai);
    if (outsize >= 2)
      *(out + 1) = ndeltaj * (v10[1]*deltai + v00[1]*ndeltai) +
	           deltaj  * (v11[1]*deltai + v01[1]*ndeltai);
    if (outsize >= 3)
      *(out + 2) = ndeltaj * (v10[2]*deltai + v00[2]*ndeltai) +
		   deltaj  * (v11[2]*deltai + v01[2]*ndeltai);
    if (outsize >= 4)
      *(out + 3) = ndeltaj * (v10[3]*deltai + v00[3]*ndeltai) +
		   deltaj  * (v11[3]*deltai + v01[3]*ndeltai);
    if (outsize >= 5) {
      eblerror("not implemented for more than 4 channels ("
	       << outsize << " channels in " << indimi << "x" << indimj
	       << " image)");
    }
  }

  /////////////////////////////////////////////////////////////////

  void image_rotscale_rect(int w, int h, double cx, double cy,
			   double angle, double coeff, idx<intg> &wh,
			   idx<double> &cxcy){
    double sa = sin(0.017453292*angle);
    double ca = cos(0.017453292*angle);
    double x1 = 0;
    double y1 = 0;
    double cw = coeff * w;
    double ch = coeff * h;
    double x2 = cw * ca;
    double y2 = cw * sa;
    double x4 = -(ch * sa);
    double y4 = ch * ca;
    double x3 = x2 + x4;
    double y3 = y2 + y4;
    double dcx = coeff * ( cx*ca - cy*sa);
    double dcy = coeff * ( cx*sa + cy*ca);
    double lx = std::min(std::min(x1, x2), std::min(x3, x4));
    double ly = std::min(std::min(y1, y2), std::min(y3, y4));
    double rx = std::max(std::max(x1, x2), std::max(x3, x4));
    double ry = std::max(std::max(y1, y2), std::max(y3, y4));
    wh.set((intg)(1 + rx - lx), 0);
    wh.set((intg)(1 + ry - ly), 1);
    cxcy.set(dcx - lx, 0);
    cxcy.set(dcy - ly, 1);
  }

  // vector flows //////////////////////////////////////////////////////////////

  idx<float> create_grid(idxdim &inputd) {
    idxdim d(inputd);
    d.insert_dim(0, 2);
    idx<float> grid(d);
    idx<float> fx = grid.select(0, 0), fy = grid.select(0, 1);
    float xs = (float) (((inputd.dim(0) % 2 == 0) ?
			 inputd.dim(0) : inputd.dim(0) - 1) * .5);
    float ys = (float) (((inputd.dim(1) % 2 == 0) ?
			 inputd.dim(1) : inputd.dim(1) - 1) * .5);
    // fill x
    for (intg i = 0; i < inputd.dim(0); ++i) {
      idx<float> fxi = fx.select(0, i);
      idx_fill(fxi, (float) ((float) i - xs));
    }
    // fill y
    for (intg i = 0; i < inputd.dim(1); ++i) {
      idx<float> fyi = fy.select(1, i);
      idx_fill(fyi, (float) ((float) i - ys));
    }
    return grid;
  }

  void translation_flow(idx<float> &grid, idx<float> &flow, float h, float w) {
    if (flow.dim(0) != 2 && flow.order() <= 1)
      eblerror("expected first dimension to be 2 and at least order 2");
    idx<float> gh = grid.select(0, 0);
    idx<float> gw = grid.select(0, 1);
    idx<float> grid0(grid.get_idxdim());
    idx_copy(grid, grid0);
    idx_addc(gh, -h);
    idx_addc(gw, -w);
    // remove grid from flow
    idx_subacc(grid, grid0, flow);
  }

  void shear_flow(idx<float> &grid, idx<float> &flow, float h, float w) {
    if (flow.dim(0) != 2 && flow.order() <= 1)
      eblerror("expected first dimension to be 2 and at least order 2");
    idx<float> gh = grid.select(0, 0);
    idx<float> gw = grid.select(0, 1);
    idx<float> grid0(grid.get_idxdim());
    idx_copy(grid, grid0);
    idx_dotcacc(gw, -h, gh);
    idx_dotcacc(gh, -w, gw);
    // remove grid from flow
    idx_subacc(grid, grid0, flow);
  }

  void scale_flow(idx<float> &grid, idx<float> &flow, float h, float w) {
    if (h == 0 || w == 0) eblerror("expected non-zero scalings");
    if (grid.dim(0) != 2 && grid.order() <= 1)
      eblerror("expected first dimension to be 2 and at least order 2");
    idx<float> gh = grid.select(0, 0), gw = grid.select(0, 1);
    idx<float> fh = flow.select(0, 0), fw = flow.select(0, 1);
    idx<float> grid0(grid.get_idxdim());
    idx_copy(grid, grid0);
    // multiply grid by scalings and accumulate to flow
    idx_dotc(gh, 1/h, gh);
    idx_dotc(gw, 1/w, gw);
    // remove grid from flow
    idx_subacc(grid, grid0, flow);
  }

  void rotation_flow(idx<float> &grid, idx<float> &flow, float deg) {
    idx<float> rot(2, 2);
    // make contiguous version of grid
    idx<float> tmp = grid.shift_dim(0, 2);
    idxdim d(tmp);
    idx<float> tmp2(d), tmp3(d);
    idx_copy(tmp, tmp2);
    idx<float> grid0(grid.get_idxdim());
    idx_copy(grid, grid0);

    float rad = deg / 180 * PI;
    rot.set(cos(rad), 0, 0);
    rot.set(-sin(rad), 0, 1);
    rot.set(sin(rad), 1, 0);
    rot.set(cos(rad), 1, 1);
    idx_m2dotm3(rot, tmp2, tmp3);
    tmp3 = tmp3.shift_dim(2, 0);
    idx_copy(tmp3, grid);
    // remove grid values
    idx_subacc(grid, grid0, flow);
  }

  void affine_flow(idx<float> &grid, idx<float> &flow, float th, float tw,
		   float sh, float sw, float shh, float shw, float deg) {
    if (sh == 0 || sw == 0) eblerror("expected non-zero scalings");
    idx<float> t(2, 3);
    // make contiguous version of grid
    idx<float> tmp = grid.shift_dim(0, 2);
    idxdim d(tmp), d2(tmp);
    d2.setdim(2, 3);
    idx<float> tmp2(d2), tmp3(d);
    idx<float> l01 = tmp2.narrow(2, 2, 0), l2 = tmp2.select(2, 2);
    idx_copy(tmp, l01);
    idx_fill(l2, (float) 1);
    idx<float> grid0(grid.get_idxdim());
    idx_copy(grid, grid0);

    float rad = deg / 180 * PI;
    t.set(cos(rad) / sh, 0, 0);
    t.set((-sin(rad) + shh) / sh, 0, 1);
    t.set(0, 0, 2);
    // t.set(-th / sh, 0, 2);

    t.set((sin(rad) + shw) / sw, 1, 0);
    t.set(cos(rad) / sw, 1, 1);
    t.set(0, 1, 2);
    // t.set(-tw / sw, 1, 2);

    idx_m2dotm3(t, tmp2, tmp3);
    tmp3 = tmp3.shift_dim(2, 0);
    idx_copy(tmp3, grid);
    // remove grid values
    idx_subacc(grid, grid0, flow);
  }

  void elastic_flow(idx<float> &flow, uint sz, float coeff) {
    // make contiguous version of grid
    idxdim d(flow);
    d.setdim(1, d.dim(1) + sz - 1);
    d.setdim(2, d.dim(2) + sz - 1);
    idx<float> f(d), f2(flow.get_idxdim());
    idx_random(f, -.5, .5);
    idx<float> g = create_gaussian_kernel<float>(sz);
    idx_3dconvol(f, g, f2);
    // remove grid values
    idx_dotcacc(f2, coeff, flow);
  }

  uint get_resize_type(const char *resize_method) {
    if (!strcmp(resize_method, "bilinear")) return BILINEAR_RESIZE;
    else if (!strcmp(resize_method, "gaussian")) return GAUSSIAN_RESIZE;
    else if (!strcmp(resize_method, "mean")) return MEAN_RESIZE;
    eblerror("undefined resizing method" << resize_method);
    return -1;
  }

} // end namespace ebl
