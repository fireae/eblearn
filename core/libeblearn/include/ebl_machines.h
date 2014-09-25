/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
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

#ifndef EBL_MACHINES_H_
#define EBL_MACHINES_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_state.h"
#include "ebl_normalization.h"
#include "ebl_basic.h"
#include "ebl_arch.h"
#include "ebl_nonlinearity.h"
#include "ebl_layers.h"
#include "ebl_energy.h"

namespace ebl {

////////////////////////////////////////////////////////////////////////////////

//! Standard LeNet5-type architecture without the final e-dist RBF layer.
template <typename T> class net_cscscf : public layers<T> {
 public:
  //! Empty constructor, awaiting for initialization by the user via the
  //! init() function.
  net_cscscf();
  //! Complete constructor, calls the init() function.
  //! See the init() description for complete arguments description.
  net_cscscf(parameter<T> &prm, intg ini, intg inj, intg ki0, intg kj0,
             idx<intg> &tbl0, intg si0, intg sj0, intg ki1, intg kj1,
             idx<intg> &tbl1, intg si1, intg sj1, intg ki2, intg kj2,
             idx<intg> &tbl2, intg outthick, bool norm = false,
             bool mirror = false, bool tanh = false,
             bool shrink = false, bool diag = false);
  virtual ~net_cscscf();

  //! The init function creates the machine by stacking the modules in this
  //! order (c-s-c-s-c-f): convolution_layer, subsampling_layer,
  //! convolution_layer, subsampling_layer, convolution_layer,
  //! full_layer.
  //! <ini> <inj>: expected max size of input for preallocation of internal
  //! states
  //! <ki0> <kj0>: kernel size for first convolutional layer
  //! <tbl0>: table of connections between input anf feature maps for first
  //! layer
  //! <si0> <sj0>: subsampling for first layer
  //! <ki1> <kj1> <tbl1> <si1> <sj1>: same for next 2 layers
  //! <ki2> <kj2> <tbl2>: same for last convolution layer
  //! <outthick>: number of outputs.
  //! <prm> an idx1-ddparam in which the parameters will be allocated.
  void init(parameter<T> &prm, intg ini, intg inj, intg ki0, intg kj0,
            idx<intg> &tbl0, intg si0, intg sj0, intg ki1, intg kj1,
            idx<intg> &tbl1, intg si1, intg sj1, intg ki2, intg kj2,
            idx<intg> &tbl2, intg outthick, bool norm = false,
            bool mirror = false, bool tanh = false, bool shrink = false,
            bool diag = false);
};

////////////////////////////////////////////////////////////////////////////////

//! Standard LeNet5-type architecture without the final e-dist RBF layer.
template <typename T>
class net_cscf : public layers<T> {
 public:
  //! Empty constructor, awaiting for initialization by the user via the
  //! init() function.
  net_cscf();
  //! Complete constructor, calls the init() function.
  //! See the init() description for complete arguments description.
  net_cscf(parameter<T> &prm, intg ini, intg inj, intg ki0, intg kj0,
           idx<intg> &tbl0, intg si0, intg sj0, intg ki1, intg kj1,
           idx<intg> &tbl1, intg outthick, bool norm = false,
           bool mirror = false, bool tanh = false,
           bool shrink = false, bool diag = false, bool lut_features = false,
           idx<T> *lut = NULL);
  virtual ~net_cscf();

  //! The init function creates the machine by stacking the modules in this
  //! order (c-s-c-s-c-f): convolution_layer, subsampling_layer,
  //! convolution_layer, subsampling_layer, convolution_layer,
  //! full_layer.
  //! <ini> <inj>: expected max size of input for preallocation of internal
  //! states
  //! <ki0> <kj0>: kernel size for first convolutional layer
  //! <tbl0>: table of connections between input anf feature maps for first
  //! layer
  //! <si0> <sj0>: subsampling for first layer
  //! <ki1> <kj1> <tbl1> <si1> <sj1>: same for next 2 layers
  //! <ki2> <kj2> <tbl2>: same for last convolution layer
  //! <outthick>: number of outputs.
  //! <prm> an idx1-ddparam in which the parameters will be allocated.
  void init(parameter<T> &prm, intg ini, intg inj, intg ki0, intg kj0,
            idx<intg> &tbl0, intg si0, intg sj0, intg ki1, intg kj1,
            idx<intg> &tbl1, intg outthick, bool norm = false,
            bool mirror = false, bool tanh = false, bool shrink = false,
            bool diag = false, bool lut_features = false, idx<T> *lut = NULL);
};

////////////////////////////////////////////////////////////////////////////////

//! Standard LeNet5-type architecture without the final e-dist RBF layer.
template <typename T> class net_cscc : public layers<T> {
 public:
  //! Empty constructor, awaiting for initialization by the user via the
  //! init() function.
  net_cscc();
  //! Complete constructor, calls the init() function.
  //! See the init() description for complete arguments description.
  net_cscc(parameter<T> &prm, intg ini, intg inj, intg ki0, intg kj0,
           idx<intg> &tbl0, intg si0, intg sj0, intg ki1, intg kj1,
           idx<intg> &tbl1, idx<intg> &tbl2, intg outthick, bool norm = false,
           bool mirror = false, bool tanh = false,
           bool shrink = false, bool diag = false);
  virtual ~net_cscc();

  //! The init function creates the machine by stacking the modules in this
  //! order (c-s-c-s-c-f): convolution_layer, subsampling_layer,
  //! convolution_layer, subsampling_layer, convolution_layer,
  //! full_layer.
  //! <ini> <inj>: expected max size of input for preallocation of internal
  //! states
  //! <ki0> <kj0>: kernel size for first convolutional layer
  //! <tbl0>: table of connections between input anf feature maps for first
  //! layer
  //! <si0> <sj0>: subsampling for first layer
  //! <ki1> <kj1> <tbl1> <si1> <sj1>: same for next 2 layers
  //! <ki2> <kj2> <tbl2>: same for last convolution layer
  //! <outthick>: number of outputs.
  //! <prm> an idx1-ddparam in which the parameters will be allocated.
  void init(parameter<T> &prm, intg ini, intg inj, intg ki0, intg kj0,
            idx<intg> &tbl0, intg si0, intg sj0, intg ki1, intg kj1,
            idx<intg> &tbl1, idx<intg> &tbl2, intg outthick,
            bool norm = false,
            bool mirror = false, bool tanh = false, bool shrink = false,
            bool diag = false);
};

////////////////////////////////////////////////////////////////////////////////

//! Standard LeNet5-type architecture without the final e-dist RBF layer.
template <typename T> class net_cscsc : public layers<T> {
 public:
  //! Empty constructor, awaiting for initialization by the user via the
  //! init() function.
  net_cscsc();
  //! Complete constructor, calls the init() function.
  //! See the init() description for complete arguments description.
  //! \param norm_pos Position of normalization: before subsampling (false)
  //!   or after (true).
  net_cscsc(parameter<T> &prm, intg ini, intg inj, intg ki0, intg kj0,
            idx<intg> &tbl0, intg si0, intg sj0, intg ki1, intg kj1,
            idx<intg> &tbl1, intg si1, intg sj1, intg ki2, intg kj2,
            idx<intg> &tbl2, bool norm = false,
            bool mirror = false, bool tanh = false,
            bool shrink = false, bool diag = false, bool norm_pos = false);
  virtual ~net_cscsc();

  //! The init function creates the machine by stacking the modules in this
  //! order (c-s-c-s-c-f): convolution_layer, subsampling_layer,
  //! convolution_layer, subsampling_layer, convolution_layer,
  //! full_layer.
  //! <ini> <inj>: expected max size of input for preallocation of internal
  //! states
  //! <ki0> <kj0>: kernel size for first convolutional layer
  //! <tbl0>: table of connections between input anf feature maps for first
  //! layer
  //! <si0> <sj0>: subsampling for first layer
  //! <ki1> <kj1> <tbl1> <si1> <sj1>: same for next 2 layers
  //! <ki2> <kj2> <tbl2>: same for last convolution layer
  //! <outthick>: number of outputs.
  //! <prm> an idx1-ddparam in which the parameters will be allocated.
  //! \param norm_pos Position of normalization: before subsampling (false)
  //!   or after (true).
  void init(parameter<T> &prm, intg ini, intg inj, intg ki0, intg kj0,
            idx<intg> &tbl0, intg si0, intg sj0, intg ki1, intg kj1,
            idx<intg> &tbl1, intg si1, intg sj1, intg ki2, intg kj2,
            idx<intg> &tbl2, bool norm = false, bool mirror = false,
            bool tanh = false, bool shrink = false, bool diag = false,
            bool norm_pos = false);
};

////////////////////////////////////////////////////////////////////////////////

//! lenet type of architecture without last full layer.
//! absolution rectification + contrast normalization can be turned on
//! with the norm boolean.
//! color can be turned on with the color boolean, in which case
//! a 3-layer input is assumed and bigger tables are used.
template <typename T> class lenet_cscsc : public net_cscsc<T> {
 public:
  lenet_cscsc(parameter<T> &prm, intg image_height, intg image_width,
              intg ki0, intg kj0, intg si0, intg sj0, intg ki1, intg kj1,
              intg si1, intg sj1, intg output_size,
              bool norm = false, bool color = false, bool mirror = false,
              bool tanh = false, bool shrink = false, bool diag = false,
              bool norm_pos = false,
              idx<intg> *table0_ = NULL, idx<intg> *table1_ = NULL,
              idx<intg> *table2_ = NULL);
  virtual ~lenet_cscsc() {}
};

////////////////////////////////////////////////////////////////////////////////

//! lenet type of architecture.
//! absolution rectification + contrast normalization can be turned on
//! with the norm boolean.
//! color can be turned on with the color boolean, in which case
//! a 3-layer input is assumed and bigger tables are used.
template <typename T> class lenet : public net_cscscf<T> {
 public:
  lenet(parameter<T> &prm, intg image_height, intg image_width,
        intg ki0, intg kj0, intg si0, intg sj0, intg ki1, intg kj1,
        intg si1, intg sj1, intg hid, intg output_size,
        bool norm = false, bool color = false, bool mirror = false,
        bool tanh = false, bool shrink = false, bool diag = false,
        idx<intg> *table0_ = NULL, idx<intg> *table1_ = NULL,
        idx<intg> *table2_ = NULL);
  virtual ~lenet() {}
};

////////////////////////////////////////////////////////////////////////////////

//! lenet type of architecture.
//! absolution rectification + contrast normalization can be turned on
//! with the norm boolean.
//! color can be turned on with the color boolean, in which case
//! a 3-layer input is assumed and bigger tables are used.
template <typename T> class lenet_cscf : public net_cscf<T> {
 public:
  lenet_cscf(parameter<T> &prm, intg image_height, intg image_width,
             intg ki0, intg kj0, intg si0, intg sj0, intg ki1, intg kj1,
             intg output_size, bool norm = false, bool color = false,
             bool mirror = false, bool tanh = false, bool shrink = false,
             bool diag = false,
             idx<intg> *table0_ = NULL, idx<intg> *table1_ = NULL);
  virtual ~lenet_cscf() {}
};

////////////////////////////////////////////////////////////////////////////////

//! create a new instance of net-cscscf implementing a LeNet-5 type
//! convolutional neural net. This network has regular sigmoid
//! units on the output, not an extra RBF layer as described
//! in the Proc. IEEE paper. The network has 6 feature
//! maps at the first layer and 16 feature maps at the second layer
//! with a connection matrix between feature maps as described in
//! the paper.
//! Arguments:
//! {<pre>
//!  <image-height> <image-width>: height and width of input image
//!  <ki0> <kj0>: height and with of convolutional kernel, first layer.
//!  <si0> <sj0>: subsampling ratio of subsampling layer, second layer.
//!  <ki1> <kj1>: height and with of convolutional kernel, third layer.
//!  <si1> <sj1>: subsampling ratio of subsampling layer, fourth layer.
//!  <hid>: number of hidden units, fifth layer
//!  <output-size>: number of output units
//!  <net-param>: idx1-ddparam that will hold the trainable parameters
//!               of the network
//! </pre>}
//! example
//! {<code>
//!  (setq p (new idx1-ddparam 0 0.1 0.02 0.02 80000))
//!  (setq z (new-lenet5 32 32 5 5 2 2 5 5 2 2 120 10 p))
//! </code>}
template <typename T> class lenet5 : public net_cscscf<T> {
 public:
  lenet5(parameter<T> &prm, intg image_height, intg image_width,
         intg ki0, intg kj0, intg si0, intg sj0,
         intg ki1, intg kj1, intg si1, intg sj1,
         intg hid, intg output_size, bool norm = false, bool mirror = false,
         bool tanh = false, bool shrink = false, bool diag = false);
  virtual ~lenet5() {}
};

////////////////////////////////////////////////////////////////////////////////

//! Lenet7, similar to lenet5 with different neural connections.
//! This network takes a 1-layer image as input.
template <typename T> class lenet7 : public net_cscscf<T> {
 public:
  //! @param output_size the number of ouputs. For a 5 class classifier
  //!        like NORB, this would be 5.
  lenet7(parameter<T> &prm, intg image_height, intg image_width,
         intg output_size, bool norm = false, bool mirror = false,
         bool tanh = false, bool shrink = false, bool diag = false);
  virtual ~lenet7() {}
};

////////////////////////////////////////////////////////////////////////////////

//! Lenet7_binocular, similar to lenet5 with different neural connections.
//! This network expects a 2-layer image containing each stereoscopic left
//! and right images.
template <typename T> class lenet7_binocular : public net_cscscf<T> {
 public:
  //! @param output_size the number of ouputs. For a 5 class classifier
  //!        like NORB, this would be 5.
  lenet7_binocular(parameter<T> &prm, intg image_height,
                   intg image_width,
                   intg output_size, bool norm = false, bool mirror = false,
                   bool tanh = false, bool shrink = false, bool diag = false);
  virtual ~lenet7_binocular() {}
};

// supevised euclidean machine /////////////////////////////////////////////////

template <typename Tdata, class Tlabel>
class supervised_euclidean_machine : public fc_ebm2<Tdata> {
 public:
  supervised_euclidean_machine(module_1_1<Tdata> &net_,
                               idx<Tdata> &targets, idxdim &dims);
  virtual ~supervised_euclidean_machine();

 protected:
  l2_energy<Tdata> fcost;               // euclidean cost function
  state<Tdata>	   fout;                // hidden state in between
};

// some connection tables //////////////////////////////////////////////////////

//! connection table used in lenet5 (6 inputs, 16 outputs)
static intg connection_table_6_16[60][2] =
{{0, 0},  {1, 0},  {2, 0},
 {1, 1},  {2, 1},  {3, 1},
 {2, 2},  {3, 2},  {4, 2},
 {3, 3},  {4, 3},  {5, 3},
 {4, 4},  {5, 4},  {0, 4},
 {5, 5},  {0, 5},  {1, 5},

 {0, 6},  {1, 6},  {2, 6},  {3, 6},
 {1, 7},  {2, 7},  {3, 7},  {4, 7},
 {2, 8},  {3, 8},  {4, 8},  {5, 8},
 {3, 9},  {4, 9},  {5, 9},  {0, 9},
 {4, 10}, {5, 10}, {0, 10}, {1, 10},
 {5, 11}, {0, 11}, {1, 11}, {2, 11},

 {0, 12}, {1, 12}, {3, 12}, {4, 12},
 {1, 13}, {2, 13}, {4, 13}, {5, 13},
 {2, 14}, {3, 14}, {5, 14}, {0, 14},

 {0, 15}, {1, 15}, {2, 15}, {3, 15}, {4, 15}, {5, 15}};

//! connection table used in lenet7 (8 inputs, 24 outputs)
static intg connection_table_8_24[96][2] =
{{0,  0}, {2,  0}, {4,  0}, {5,  0},
 {0,  1}, {2,  1}, {4,  1}, {6,  1},
 {0,  2}, {2,  2}, {4,  2}, {7,  2},
 {0,  3}, {2,  3}, {5,  3}, {6,  3},
 {0,  4}, {2,  4}, {5,  4}, {7,  4},
 {0,  5}, {2,  5}, {6,  5}, {7,  5},
 {1,  6}, {3,  6}, {4,  6}, {5,  6},
 {1,  7}, {3,  7}, {4,  7}, {6,  7},
 {1,  8}, {3,  8}, {4,  8}, {7,  8},
 {1,  9}, {3,  9}, {5,  9}, {6,  9},
 {1, 10}, {3, 10}, {5, 10}, {7, 10},
 {1, 11}, {3, 11}, {6, 11}, {7, 11},
 {1, 12}, {2, 12}, {4, 12}, {5, 12},
 {1, 13}, {2, 13}, {4, 13}, {6, 13},
 {1, 14}, {2, 14}, {4, 14}, {7, 14},
 {1, 15}, {2, 15}, {5, 15}, {6, 15},
 {1, 16}, {2, 16}, {5, 16}, {7, 16},
 {1, 17}, {2, 17}, {6, 17}, {7, 17},
 {0, 18}, {3, 18}, {4, 18}, {5, 18},
 {0, 19}, {3, 19}, {4, 19}, {6, 19},
 {0, 20}, {3, 20}, {4, 20}, {7, 20},
 {0, 21}, {3, 21}, {5, 21}, {6, 21},
 {0, 22}, {3, 22}, {5, 22}, {7, 22},
 {0, 23}, {3, 23}, {6, 23}, {7, 23}};

} // namespace ebl {

#include "ebl_machines.hpp"

#endif /* EBL_MACHINES_H_ */
