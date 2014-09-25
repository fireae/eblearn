/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#include "ebl_basic.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////

#ifdef USE_IPP

  // TODO: enable IPP for float convolutions
  // TODO: ipp 64 for doubles?

//   void c_layer_ipp::fprop (state_idx *in, state_idx *out) {
//     intg ki = kernel->x.dim(1);
//     intg kj = kernel->x.dim(2);
//     intg sini = in->x.dim(1);
//     intg sinj = in->x.dim(2);

//     if (((sini - (ki - stridei) % stridei) != 0) ||
// 	((sinj - (kj - stridej) % stridej) != 0))
//       eblerror("inconsistent input size, kernel size, and subsampling ratio.");
//     if ((stridei != 1) || (stridej != 1))
//       eblerror("stride > 1 not implemented yet.");
//     idx<double> uuin = in->x.unfold(1, ki, stridei);
//     uuin = uuin.spec.unfold_inplace(2, kj, stridej);
//     idx<double> lki = idx<double>(kernel->x.dim(1), kernel->x.dim(2));
//     // resize output if necessary
//     sum->resize(thickness, uuin.dim(1), uuin.dim(2));
//     out->resize(thickness, uuin.dim(1), uuin.dim(2));
//     idx_clear(sum->x);
//     // generic convolution
//     idx<double> tout = idx<double>(sum->x.dim(1), sum->x.dim(2));
//     { idx_bloop2(lk, kernel->x, double, lt, *table, intg) {
// 	rev_idx2_tr(*lk, lki);
// 	//      ipp_convolution_float(in->x.select(0, lt.get(0)), lki, tout);
// 	//      ipp_add_float(tout, sum->x.select(0, lt.get(1)));
//       }
//     }
//     // add bias
//     { idx_bloop3(sumx, sum->x, double, biasx, bias->x, double,
// 		 outx, out->x, double) {
// 	//      ipp_addc_nip_float(sumx, biasx.get(), outx);
//       }
//     }
//     // call squashing function
//     squash->fprop(*sum, *out);
//   }

//   void c_layer_ipp::bprop (state_idx *in, state_idx *out) {
//     // backprop gradient through squasher
//     squash->bprop(*sum, *out);
//     // compute gradient of bias
//     { idx_bloop2(lha, sum->dx, double, lb, bias->dx, double) {
// 	idx_sumacc(lha, lb);
//       }
//     }
//     // backprop through convolution
//     idx_clear(in->dx);
//     /*
//       (let* ((ki (idx-dim :kernel:dx 1))	 (kj (idx-dim :kernel:dx 2))
//       (ini (idx-dim :in:dx 1))	 (inj (idx-dim :in:dx 2))
//       (outi (idx-dim :out:dx 1))	 (outj (idx-dim :out:dx 2))
//       (sumi (idx-dim :sum:dx 1))	 (sumj (idx-dim :sum:dx 2))
//       (souti (gbtype-matrix sumi sumj))
//       (tout (gbtype-matrix ki kj)))
//       (idx-bloop ((lk :kernel:dx) (lkf :kernel:x) (lt table))
//       (let* ((islice (lt 0))
//       (sout  (select :sum:dx 0 (lt 1)))
//       )
//       ;; backward convolution
//       (ipp-convolution-full-float sout lkf (select :in:dx 0 islice))
//       ;; compute gradient for kernel
//       (rev-idx2-tr-float sout souti)
//       (ipp-convolution-float (select :in:x 0 islice) souti tout)
//       (ipp-add-float tout lk)
//       )))
//     */
//   }

#endif

} // end namespace ebl
