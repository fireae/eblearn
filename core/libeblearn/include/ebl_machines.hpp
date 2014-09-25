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

namespace ebl {

// net_cscscf //////////////////////////////////////////////////////////////////

template <typename T>
net_cscscf<T>::net_cscscf() : layers<T>(true) {
}

template <typename T>
net_cscscf<T>::net_cscscf(parameter<T> &prm, intg ini, intg inj,
                          intg ki0, intg kj0, idx<intg> &tbl0,
                          intg si0, intg sj0,
                          intg ki1, intg kj1, idx<intg> &tbl1,
                          intg si1, intg sj1,
                          intg ki2, intg kj2, idx<intg> &tbl2,
                          intg outthick, bool norm, bool mirror,
                          bool tanh, bool shrink, bool diag)
    : layers<T>(true) {
  // owns modules, responsible for deleting it
  init(prm, ini, inj, ki0, kj0, tbl0, si0, sj0, ki1, kj1, tbl1,
       si1, sj1, ki2, kj2, tbl2, outthick, norm, mirror, tanh, shrink, diag);
}

template <typename T>
net_cscscf<T>::~net_cscscf() {}

template <typename T>
void net_cscscf<T>::init(parameter<T> &prm, intg ini, intg inj,
                         intg ki0, intg kj0, idx<intg> &tbl0,
                         intg si0, intg sj0, intg ki1, intg kj1,
                         idx<intg> &tbl1, intg si1, intg sj1,
                         intg ki2, intg kj2, idx<intg> &tbl2,
                         intg outthick, bool norm, bool mirror,
                         bool tanh, bool shrink, bool diag) {
  // here we compute the thickness of the feature maps based on the
  // convolution tables.
  idx<intg> tblmax = tbl0.select(1, 1);
  intg thick0 = 1 + idx_max(tblmax);
  tblmax = tbl1.select(1, 1);
  intg thick1 = 1 + idx_max(tblmax);
  tblmax = tbl2.select(1, 1);
  intg thick2 = 1 + idx_max(tblmax);
  // convolution sizes
  idxdim ker0(ki0, kj0);
  idxdim ker1(ki1, kj1);
  idxdim ker2(ki2, kj2);
  idxdim stride(1, 1);
  // subsampling sizes
  idxdim sker0(si0, sj0);
  idxdim sker1(si1, sj1);

  // layers was initialized with true so it owns the modules we give it,
  // we can add modules with "new".
  // we add convolutions (c), subsamplings (s), and full layers (f)
  // to form a c-s-c-s-c-f network. and we add state_idx in between
  // which serve as temporary buffer to hold the output of a module
  // and feed the input of the following module.

  // convolution
  this->add_module(new convolution_module<T>(&prm, ker0, stride, tbl0, "c0"));
  // bias
  this->add_module(new addc_module<T>(&prm, thick0, "c0bias"));
  // non linearity
  if (shrink) this->add_module(new smooth_shrink_module<T>(&prm, thick0));
  else if (tanh) this->add_module(new tanh_module<T>());
  else this->add_module(new stdsigmoid_module<T>());
  // feature coefficents
  if (diag) this->add_module(new diag_module<T>(&prm, thick0));
  // absolute rectification + contrast normalization
  if (norm) {
    this->add_module(new abs_module<T>());
    this->add_module(new contrast_norm_module<T>
                     (ker0, thick0, mirror, true, false, NULL, "w0"));
  }
  // subsampling
  this->add_module(new subsampling_layer<T>(&prm, thick0,sker0,sker0, tanh));
  // convolution
  this->add_module(new convolution_module<T>
                   (&prm, ker1, stride, tbl1));
  // bias
  this->add_module(new addc_module<T>(&prm, thick1));
  // non linearity
  if (shrink) this->add_module(new smooth_shrink_module<T>(&prm, thick1));
  else if (tanh) this->add_module(new tanh_module<T>());
  else this->add_module(new stdsigmoid_module<T>());
  // feature coefficents
  if (diag)
    this->add_module(new diag_module<T>(&prm, thick1));
  // absolute rectification + contrast normalization
  if (norm) {
    this->add_module(new abs_module<T>());
    this->add_module(new contrast_norm_module<T>
                     (ker1, thick1, mirror, true, false, NULL, "w1"));
  }
  // subsampling
  this->add_module(new subsampling_layer<T>(&prm, thick1,sker1,sker1, tanh));
  // convolution + bias + sigmoid
  this->add_module(new convolution_layer<T>
                   (&prm, ker2, stride, tbl2, tanh));
  // full
  this->add_module(new full_layer<T>(&prm, thick2, outthick, tanh));
}

// net_cscf ////////////////////////////////////////////////////////////////////

template <typename T>
net_cscf<T>::net_cscf() : layers<T>(true) {
}

template <typename T>
net_cscf<T>::net_cscf(parameter<T> &prm, intg ini, intg inj,
                      intg ki0, intg kj0, idx<intg> &tbl0,
                      intg si0, intg sj0,
                      intg ki1, intg kj1, idx<intg> &tbl1,
                      intg outthick, bool norm, bool mirror,
                      bool tanh, bool shrink, bool diag, bool lut_features,
                      idx<T> *lut)
    : layers<T>(true) {
  init(prm, ini, inj, ki0, kj0, tbl0, si0, sj0, ki1, kj1, tbl1,
       outthick, norm, mirror, tanh, shrink, diag, lut_features, lut);
}

template <typename T>
net_cscf<T>::~net_cscf() {}

template <typename T>
void net_cscf<T>::init(parameter<T> &prm, intg ini, intg inj,
                       intg ki0, intg kj0, idx<intg> &tbl0,
                       intg si0, intg sj0, intg ki1, intg kj1,
                       idx<intg> &tbl1, intg outthick, bool norm,
                       bool mirror, bool tanh, bool shrink, bool diag,
                       bool lut_features, idx<T> *lut) {
  // here we compute the thickness of the feature maps based on the
  // convolution tables.
  idx<intg> tblmax = tbl0.select(1, 1);
  intg thick0 = 1 + idx_max(tblmax);
  tblmax = tbl1.select(1, 1);
  intg thick1 = 1 + idx_max(tblmax);
  // convolution sizes
  idxdim ker0(ki0, kj0);
  idxdim ker1(ki1, kj1);
  idxdim stride(1, 1);
  // subsampling sizes
  idxdim sker0(si0, sj0);

  // layers was initialized with true so it owns the modules we give it,
  // we can add modules with "new".
  // we add convolutions (c), subsamplings (s), and full layers (f)
  // to form a c-s-c-s-c-f network. and we add state_idx in between
  // which serve as temporary buffer to hold the output of a module
  // and feed the input of the following module.

  // convolution
  this->add_module(new convolution_module<T>(&prm, ker0, stride,
                                             tbl0, "c0"));
  // bias
  this->add_module(new addc_module<T>(&prm, thick0, "c0bias"));
  // non linearity
  if (shrink)
    this->add_module(new smooth_shrink_module<T>(&prm, thick0));
  else if (tanh)
    this->add_module(new tanh_module<T>());
  else
    this->add_module(new stdsigmoid_module<T>());
  // feature coefficents
  if (diag)
    this->add_module(new diag_module<T>(&prm, thick0));
  // absolute rectification + contrast normalization
  if (norm) {
    this->add_module(new abs_module<T>());
    this->add_module(new contrast_norm_module<T>(ker0, thick0, mirror, NULL,
                                                 "w0"));
  }
  // subsampling
  this->add_module(new subsampling_layer<T>(&prm,thick0,sker0,sker0,tanh,"s0"));
  // convolution
  this->add_module(new convolution_module<T>(&prm,ker1,stride,
                                             tbl1,"c1"));
  // bias
  this->add_module(new addc_module<T>(&prm, thick1, "c1bias"));
  // non linearity
  if (shrink)
    this->add_module(new smooth_shrink_module<T>(&prm, thick1));
  else if (tanh)
    this->add_module(new tanh_module<T>());
  else
    this->add_module(new stdsigmoid_module<T>());
  // feature coefficents
  if (diag)
    this->add_module(new diag_module<T>(&prm, thick1));
  // absolute rectification + contrast normalization
  if (norm) {
    this->add_module(new abs_module<T>());
    this->add_module(new contrast_norm_module<T>(ker1, thick1, mirror, NULL,
                                                 "w1"));
  }
  if (lut_features)
    this->add_module(new range_lut_module<T>(lut));
  // full
  this->add_module(new full_layer<T>(&prm, thick1, outthick, tanh, "f2"));

  // // convolution
  // if (norm) // absolute rectification + contrast normalization
  //   this->add_module(new convabsnorm_layer<T>(&prm, ker0,stride,tbl0,mirror,tanh));
  // else // old fashioned way
  //   this->add_module(new convolution_layer<T>(&prm, ker0, stride, tbl0, tanh));
  // // subsampling
  // this->add_module(new subsampling_layer<T>(&prm,thick0,sker0, sker0, tanh));,
  // // convolution
  // if (norm) // absolute rectification + contrast normalization
  //   this->add_module(new convabsnorm_layer<T>(&prm, ker1,stride,tbl1,mirror,tanh));
  // else // old fashioned way
  //   this->add_module(new convolution_layer<T>(&prm, ker1, stride, tbl1, tanh));
  // // full
  // add_last_module(new full_layer<T>(&prm, thick1, outthick, tanh));
}

// net_cscc ////////////////////////////////////////////////////////////////////

template <typename T>
net_cscc<T>::net_cscc() : layers<T>(true) {
}

template <typename T>
net_cscc<T>::net_cscc(parameter<T> &prm, intg ini, intg inj,
                      intg ki0, intg kj0, idx<intg> &tbl0,
                      intg si0, intg sj0,
                      intg ki1, intg kj1, idx<intg> &tbl1,
                      idx<intg> &tbl2,
                      intg outthick, bool norm, bool mirror,
                      bool tanh, bool shrink, bool diag)
    : layers<T>(true) {
  init(prm, ini, inj, ki0, kj0, tbl0, si0, sj0, ki1, kj1, tbl1,
       tbl2, outthick, norm, mirror, tanh, shrink, diag);
}

template <typename T>
net_cscc<T>::~net_cscc() {}

template <typename T>
void net_cscc<T>::init(parameter<T> &prm, intg ini, intg inj,
                       intg ki0, intg kj0, idx<intg> &tbl0,
                       intg si0, intg sj0, intg ki1, intg kj1,
                       idx<intg> &tbl1, idx<intg> &tbl2,
                       intg outthick, bool norm,
                       bool mirror, bool tanh, bool shrink, bool diag) {
  // here we compute the thickness of the feature maps based on the
  // convolution tables.
  idx<intg> tblmax = tbl0.select(1, 1);
  intg thick0 = 1 + idx_max(tblmax);
  tblmax = tbl1.select(1, 1);
  intg thick1 = 1 + idx_max(tblmax);
  // WARNING: those two numbers must be changed
  // when image-height/image-width change
  // TODO: add assertion test here?
  intg ki2 = (((ini - ki0 + 1) / si0) - ki1 + 1);
  intg kj2 = (((inj  - kj0 + 1) / sj0) - kj1 + 1);

  // convolution sizes
  idxdim ker0(ki0, kj0);
  idxdim ker1(ki1, kj1);
  idxdim ker2(ki2, kj2);
  idxdim stride(1, 1);
  // subsampling sizes
  idxdim sker0(si0, sj0);

  // convolution
  this->add_module(new convolution_module<T>(&prm,ker0,stride,
                                             tbl0,"c0"));
  // bias
  this->add_module(new addc_module<T>(&prm, thick0, "c0bias"));
  // non linearity
  if (shrink)
    this->add_module(new smooth_shrink_module<T>(&prm, thick0));
  else if (tanh)
    this->add_module(new tanh_module<T>());
  else
    this->add_module(new stdsigmoid_module<T>());
  // feature coefficents
  if (diag)
    this->add_module(new diag_module<T>(&prm, thick0));
  // absolute rectification + contrast normalization
  if (norm) {
    this->add_module(new abs_module<T>());
    this->add_module(new contrast_norm_module<T>(ker0, thick0, mirror,
                                                 NULL, "w0"));
  }
  // subsampling
  this->add_module(new subsampling_layer<T>(&prm, thick0, sker0, sker0, tanh, "s0"));
  // convolution
  this->add_module(new convolution_module<T>(&prm,ker1,stride,
                                             tbl1,"c1"));
  // bias
  this->add_module(new addc_module<T>(&prm, thick1, "c1bias"));
  // non linearity
  if (shrink)
    this->add_module(new smooth_shrink_module<T>(&prm, thick1));
  else if (tanh)
    this->add_module(new tanh_module<T>());
  else
    this->add_module(new stdsigmoid_module<T>());
  // feature coefficents
  if (diag)
    this->add_module(new diag_module<T>(&prm, thick1));
  // absolute rectification + contrast normalization
  if (norm) {
    this->add_module(new abs_module<T>());
    this->add_module(new contrast_norm_module<T>(ker1, thick1, mirror,
                                                 NULL, "w1"));
  }
  // convolution + bias + sigmoid
  this->add_module(new convolution_layer<T>(&prm, ker2, stride, tbl2, tanh,"c2"));
}

// net_cscsc ///////////////////////////////////////////////////////////////////

template <typename T>
net_cscsc<T>::net_cscsc() : layers<T>(true) {
}

template <typename T>
net_cscsc<T>::net_cscsc(parameter<T> &prm, intg ini, intg inj,
                        intg ki0, intg kj0, idx<intg> &tbl0,
                        intg si0, intg sj0,
                        intg ki1, intg kj1, idx<intg> &tbl1,
                        intg si1, intg sj1,
                        intg ki2, intg kj2, idx<intg> &tbl2,
                        bool norm, bool mirror, bool tanh,
                        bool shrink, bool diag, bool norm_pos)
    : layers<T>(true) { // owns modules, responsible for deleting it
  init(prm, ini, inj, ki0, kj0, tbl0, si0, sj0, ki1, kj1, tbl1,
       si1, sj1, ki2, kj2, tbl2, norm, mirror, tanh, shrink, diag, norm_pos);
}

template <typename T>
net_cscsc<T>::~net_cscsc() {}

template <typename T>
void net_cscsc<T>::init(parameter<T> &prm, intg ini, intg inj,
                        intg ki0, intg kj0, idx<intg> &tbl0,
                        intg si0, intg sj0, intg ki1, intg kj1,
                        idx<intg> &tbl1, intg si1, intg sj1,
                        intg ki2, intg kj2, idx<intg> &tbl2,
                        bool norm, bool mirror, bool tanh,
                        bool shrink, bool diag, bool norm_pos) {
  // here we compute the thickness of the feature maps based on the
  // convolution tables.
  idx<intg> tblmax = tbl0.select(1, 1);
  intg thick0 = 1 + idx_max(tblmax);
  tblmax = tbl1.select(1, 1);
  intg thick1 = 1 + idx_max(tblmax);

  // convolution sizes
  idxdim ker0(ki0, kj0);
  idxdim ker1(ki1, kj1);
  idxdim ker2(ki2, kj2);
  idxdim stride(1, 1);
  // subsampling sizes
  idxdim sker0(si0, sj0);
  idxdim sker1(si1, sj1);

  // layers was initialized with true so it owns the modules we give it,
  // we can add modules with "new".
  // we add convolutions (c), subsamplings (s), and full layers (f)
  // to form a c-s-c-s-c-f network. and we add state_idx in between
  // which serve as temporary buffer to hold the output of a module
  // and feed the input of the following module.

  if (norm)
    eblprint( "Using contrast normalization " << (norm_pos?"after":"before")
              << " subsampling. " << std::endl);
  // convolution
  this->add_module(new convolution_module<T>
                   (&prm, ker0, stride, tbl0, "c0"));
  // bias
  this->add_module(new addc_module<T>(&prm, thick0, "c0bias"));
  // non linearity
  if (shrink)
    this->add_module(new smooth_shrink_module<T>(&prm, thick0));
  else if (tanh)
    this->add_module(new tanh_module<T>());
  else
    this->add_module(new stdsigmoid_module<T>());
  // feature coefficents
  if (diag)
    this->add_module(new diag_module<T>(&prm, thick0));
  // absolute rectification
  if (norm)
    this->add_module(new abs_module<T>());
  // contrast normalization (positioned before sub)
  if (norm && !norm_pos)
    this->add_module(new contrast_norm_module<T>
                     (ker0, thick0, mirror, true, false, NULL, "w0"));
  // subsampling
  this->add_module(new subsampling_layer<T>(&prm, thick0, sker0, sker0, tanh, "s0"));
  // contrast normalization (positioned after sub)
  if (norm && norm_pos)
    this->add_module(new contrast_norm_module<T>
                     (ker1, thick0, mirror, true, false, NULL, "w0"));
  // convolution
  this->add_module(new convolution_module<T>
                   (&prm, ker1, stride,tbl1, "c1"));
  // bias
  this->add_module(new addc_module<T>(&prm, thick1, "c1bias"));
  // non linearity
  if (shrink)
    this->add_module(new smooth_shrink_module<T>(&prm, thick1));
  else if (tanh)
    this->add_module(new tanh_module<T>());
  else
    this->add_module(new stdsigmoid_module<T>());
  // feature coefficents
  if (diag)
    this->add_module(new diag_module<T>(&prm, thick1));
  // absolute rectification
  if (norm)
    this->add_module(new abs_module<T>());
  // contrast normalization (position before sub)
  if (norm && !norm_pos)
    this->add_module(new contrast_norm_module<T>
                     (ker1, thick1, mirror, true, false, NULL, "w1"));
  // subsampling
  this->add_module(new subsampling_layer<T>(&prm, thick1, sker1, sker1, tanh, "s1"));
  // contrast normalization (position after sub)
  if (norm && norm_pos)
    this->add_module(new contrast_norm_module<T>
                     (ker2, thick1, mirror, true, false, NULL, "w1"));
  // convolution + bias + sigmoid
  this->add_module(new convolution_layer<T>
                   (&prm, ker2, stride, tbl2, tanh,"c2"));
}

// lenet_cscsc /////////////////////////////////////////////////////////////////

template <typename T>
lenet_cscsc<T>::
lenet_cscsc(parameter<T> &prm, intg image_height, intg image_width,
            intg ki0, intg kj0, intg si0, intg sj0, intg ki1,
            intg kj1, intg si1, intg sj1,
            intg output_size, bool norm, bool color, bool mirror, bool tanh,
            bool shrink, bool diag, bool norm_pos, idx<intg> *table0_,
            idx<intg> *table1_, idx<intg> *table2_)
    : net_cscsc<T>() {
  idx<intg> table0, table1, table2;
  if (!color) { // use smaller tables
    table0 = full_table(1, 6);
    table1 = idx<intg>(60, 2);
    memcpy(table1.idx_ptr(), connection_table_6_16,
           table1.nelements() * sizeof (intg));
  } else { // for color (assuming 3-layer input), use bigger tables
    table0 = idx<intg>(14, 2);
    intg tbl0[14][2] = {{0, 0},  {0, 1},  {0, 2}, {0, 3},  {1, 4},  {2, 4},
                        {1, 5},  {2, 5},  {0, 6}, {1, 6},  {2, 6},  {0, 7},
                        {1, 7}, {2, 7}};
    memcpy(table0.idx_ptr(), tbl0, table0.nelements() * sizeof (intg));
    table1 = idx<intg>(96, 2);
    memcpy(table1.idx_ptr(), connection_table_8_24,
           table1.nelements() * sizeof (intg));
  }
  // overide default tables if defined
  if (table0_)
    table0 = *table0_;
  if (table1_)
    table1 = *table1_;
  if (table2_)
    table2 = *table2_;
  else {
    // determine max of previous table
    idx<intg> outtable = table1.select(1, 1);
    table2 = full_table(idx_max(outtable) + 1, output_size);
    eblprint( "Using a full table for table 2: " << idx_max(outtable) + 1
              <<  " -> " << output_size << std::endl);
  }

  // WARNING: those two numbers must be changed
  // when image-height/image-width change
  // TODO: add assertion test here?
  intg ki2 = (((image_height - ki0 + 1) / si0) - ki1 + 1) / si1;
  intg kj2 = (((image_width  - kj0 + 1) / sj0) - kj1 + 1) / sj1;

  this->init(prm, image_height, image_width, ki0, kj0, table0, si0, sj0,
             ki1, kj1, table1, si1, sj1, ki2, kj2, table2, norm, mirror,
             tanh, shrink, diag, norm_pos);
}

// lenet ///////////////////////////////////////////////////////////////////////

template <typename T>
lenet<T>::lenet(parameter<T> &prm, intg image_height,
                intg image_width, intg ki0, intg kj0, intg si0,
                intg sj0, intg ki1, intg kj1, intg si1, intg sj1,
                intg hid, intg output_size, bool norm, bool color,
                bool mirror, bool tanh, bool shrink, bool diag,
                idx<intg> *table0_, idx<intg> *table1_,
                idx<intg> *table2_)
    : net_cscscf<T>() {
  idx<intg> table0, table1, table2;
  if (!color) { // use smaller tables
    table0 = full_table(1, 6);
    table1 = idx<intg>(60, 2);
    memcpy(table1.idx_ptr(), connection_table_6_16,
           table1.nelements() * sizeof (intg));
  } else { // for color (assuming 3-layer input), use bigger tables
    table0 = idx<intg>(14, 2);
    intg tbl0[14][2] = {{0, 0},  {0, 1},  {0, 2}, {0, 3},  {1, 4},  {2, 4},
                        {1, 5},  {2, 5},  {0, 6}, {1, 6},  {2, 6},  {0, 7},
                        {1, 7}, {2, 7}};
    memcpy(table0.idx_ptr(), tbl0, table0.nelements() * sizeof (intg));
    table1 = idx<intg>(96, 2);
    memcpy(table1.idx_ptr(), connection_table_8_24,
           table1.nelements() * sizeof (intg));
  }
  // overide default tables if defined
  if (table0_)
    table0 = *table0_;
  if (table1_)
    table1 = *table1_;
  // allocate a full table for table2 by default
  idx<intg> tb1 = table1.select(1, 1);
  intg max1 = idx_max(tb1) + 1;
  table2 = full_table(max1, hid);
  // override table2 if defined
  if (table2_)
    table2 = *table2_;

  // WARNING: those two numbers must be changed
  // when image-height/image-width change
  // TODO: add assertion test here?
  intg ki2 = (((image_height - ki0 + 1) / si0) - ki1 + 1) / si1;
  intg kj2 = (((image_width  - kj0 + 1) / sj0) - kj1 + 1) / sj1;

  this->init(prm, image_height, image_width, ki0, kj0, table0, si0, sj0,
             ki1, kj1, table1, si1, sj1, ki2, kj2, table2, output_size,
             norm, mirror, tanh, shrink, diag);
}

// lenet ///////////////////////////////////////////////////////////////////////

template <typename T>
lenet_cscf<T>::lenet_cscf(parameter<T> &prm,
                          intg image_height, intg image_width,
                          intg ki0, intg kj0, intg si0, intg sj0,
                          intg ki1, intg kj1, intg output_size,
                          bool norm, bool color, bool mirror,
                          bool tanh, bool shrink, bool diag, idx<intg> *table0_,
                          idx<intg> *table1_)
    : net_cscf<T>() {
  idx<intg> table0, table1;
  if (!color) { // use smaller tables
    table0 = full_table(1, 6);
    table1 = idx<intg>(60, 2);
    memcpy(table1.idx_ptr(), connection_table_6_16,
           table1.nelements() * sizeof (intg));
  } else { // for color (assuming 3-layer input), use bigger tables
    table0 = idx<intg>(14, 2);
    intg tbl0[14][2] = {{0, 0},  {0, 1},  {0, 2}, {0, 3},  {1, 4},  {2, 4},
                        {1, 5},  {2, 5},  {0, 6}, {1, 6},  {2, 6},  {0, 7},
                        {1, 7}, {2, 7}};
    memcpy(table0.idx_ptr(), tbl0, table0.nelements() * sizeof (intg));
    table1 = idx<intg>(96, 2);
    memcpy(table1.idx_ptr(), connection_table_8_24,
           table1.nelements() * sizeof (intg));
  }
  if (table0_)
    table0 = *table0_;
  if (table1_)
    table1 = *table1_;

  this->init(prm, image_height, image_width, ki0, kj0, table0, si0, sj0,
             ki1, kj1, table1, output_size, norm, mirror, tanh, shrink, diag);
}

// lenet5 //////////////////////////////////////////////////////////////////////

template <typename T>
lenet5<T>::lenet5(parameter<T> &prm, intg image_height,
                  intg image_width,
                  intg ki0, intg kj0, intg si0, intg sj0,
                  intg ki1, intg kj1, intg si1, intg sj1,
                  intg hid, intg output_size, bool norm, bool mirror,
                  bool tanh, bool shrink, bool diag) {
  idx<intg> table0 = full_table(1, 6);
  // TODO: add idx constructor taking pointer to data with dimensions
  // and copies it.
  // or if possible operator= taking the array in brackets?
  idx<intg> table1(60, 2);
  memcpy(table1.idx_ptr(), connection_table_6_16,
         table1.nelements() * sizeof (intg));
  idx<intg> table2 = full_table(16, hid);

  // WARNING: those two numbers must be changed
  // when image-height/image-width change
  // TODO: add assertion test here?
  intg ki2 = (((image_height - ki0 + 1) / si0) - ki1 + 1) / si1;
  intg kj2 = (((image_width  - kj0 + 1) / sj0) - kj1 + 1) / sj1;

  this->init(prm, image_height, image_width, ki0, kj0, table0, si0, sj0,
             ki1, kj1, table1, si1, sj1, ki2, kj2, table2, output_size,
             norm, mirror, tanh, shrink, diag);
}

// lenet7 //////////////////////////////////////////////////////////////////////

template <typename T>
lenet7<T>::lenet7(parameter<T> &prm, intg image_height,
                  intg image_width, intg output_size, bool norm,
                  bool mirror, bool tanh, bool shrink, bool diag) {
  intg ki0 = 5, kj0 = 5;
  intg si0 = 4, sj0 = 4;
  intg ki1 = 6, kj1 = 6;
  intg si1 = 3, sj1 = 3;
  intg hid = 100;

  idx<intg> table0 = full_table(1, 8);
  idx<intg> table1(96, 2);
  memcpy(table1.idx_ptr(), connection_table_8_24,
         table1.nelements() * sizeof (intg));
  idx<intg> table2 = full_table(24, hid);

  // WARNING: those two numbers must be changed
  // when image-height/image-width change
  // TODO: add assertion test here?
  intg ki2 = (((image_height - ki0 + 1) / si0) - ki1 + 1) / si1;
  intg kj2 = (((image_width  - kj0 + 1) / sj0) - kj1 + 1) / sj1;

  this->init(prm, image_height, image_width, ki0, kj0, table0, si0, sj0,
             ki1, kj1, table1, si1, sj1, ki2, kj2, table2, output_size,
             norm, mirror, tanh, shrink, diag);
}

// lenet7_binocular ////////////////////////////////////////////////////////////

template <typename T>
lenet7_binocular<T>::
lenet7_binocular(parameter<T> &prm, intg image_height,
                 intg image_width, intg output_size, bool norm, bool mirror,
                 bool tanh, bool shrink, bool diag) {
  intg ki0 = 5, kj0 = 5;
  intg si0 = 4, sj0 = 4;
  intg ki1 = 6, kj1 = 6;
  intg si1 = 3, sj1 = 3;
  intg hid = 100;

  idx<intg> table0(12, 2);
  intg tbl0[12][2] =
      {{0, 0},  {0, 1},  {1, 2},
       {1, 3},  {0, 4},  {1, 4},
       {0, 5},  {1, 5},  {0, 6},
       {1, 6},  {0, 7},  {1, 7}};
  memcpy(table0.idx_ptr(), tbl0, table0.nelements() * sizeof (intg));
  idx<intg> table1(96, 2);
  memcpy(table1.idx_ptr(), connection_table_8_24,
         table1.nelements() * sizeof (intg));
  idx<intg> table2 = full_table(24, hid);

  // WARNING: those two numbers must be changed
  // when image-height/image-width change
  // TODO: add assertion test here?
  intg ki2 = (((image_height - ki0 + 1) / si0) - ki1 + 1) / si1;
  intg kj2 = (((image_width  - kj0 + 1) / sj0) - kj1 + 1) / sj1;

  this->init(prm, image_height, image_width, ki0, kj0, table0, si0, sj0,
             ki1, kj1, table1, si1, sj1, ki2, kj2, table2, output_size,
             norm, mirror, tanh, shrink, diag);
}

// supervised euclidean machine ////////////////////////////////////////////////

template <typename Tdata, class Tlabel>
supervised_euclidean_machine<Tdata,Tlabel>::
supervised_euclidean_machine(module_1_1<Tdata> &m, idx<Tdata> &t, idxdim &dims)
    : fc_ebm2<Tdata>(m, fout, fcost), fcost(t), fout(dims) {
}

template <typename Tdata, class Tlabel>
supervised_euclidean_machine<Tdata,Tlabel>::
~supervised_euclidean_machine() {
}

////////////////////////////////////////////////////////////////////////

} // end namespace ebl
