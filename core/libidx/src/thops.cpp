/***************************************************************************
 *   Copyright (C) 2012 by Soumith Chintala   *
 *   soumith@gmail.com   *
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

#include "thops.h"
#include "idxops.h"

namespace ebl {

} // end namespace ebl

#ifdef __TH__


#ifdef __TH__
extern "C" {
#include <TH/TH.h>
}
#endif


namespace ebl {
  

  //----------------------------------------------------------------
  //------------TH Bindings-----------------------------------------
  //----------Long bindings-------------
  THLongStorage* eblsrg2thsrg(ebl::srg<long> *input) {
    THLongStorage *output = (THLongStorage *)malloc(sizeof(THLongStorage));
    output->data = input->get_data();
    output->size = input->size();

    output->refcount  = input->get_count();
    //input->refcount;
    //TODO: not sure what isMapped is for
    //output->isMapped = false;//input->changesize()
    return output;
  }

  THLongTensor* idx2th(ebl::idx<long> *input) {
    THLongTensor *output;
    THLongStorage* output_storage = eblsrg2thsrg(input->getstorage());
    if(input->order() == 1) {
      output = THLongTensor_newWithStorage1d(output_storage,     \
                                              input->offset(),    \
                                              input->dim(0),      \
                                              input->mod(0));     \
    }
    else if(input->order() == 2) {
      output = THLongTensor_newWithStorage2d(output_storage,     \
                                              input->offset(),    \
                                              input->dim(0),      \
                                              input->mod(0),      \
                                              input->dim(1),      \
                                              input->mod(1));
    }
    else if(input->order() == 3)
      output = THLongTensor_newWithStorage3d(output_storage,     \
                                              input->offset(),    \
                                              input->dim(0),      \
                                              input->mod(0),      \
                                              input->dim(1),      \
                                              input->mod(1),      \
                                              input->dim(2),      \
                                              input->mod(2));
    else if(input->order() == 4)
      output = THLongTensor_newWithStorage4d(output_storage,     \
                                              input->offset(),    \
                                              input->dim(0),      \
                                              input->mod(0),      \
                                              input->dim(1),      \
                                              input->mod(1),      \
                                              input->dim(2),      \
                                              input->mod(2),      \
                                              input->dim(3),      \
                                              input->mod(3));     \
    else
      std::cerr<<"ERROR: Unsupported dimensions in idx2TH";
    return output;
  }

  //-----------Float bindings----------------------------------------
    
  THFloatStorage* eblsrg2thsrg(ebl::srg<float> *input) {
    THFloatStorage *output = (THFloatStorage *)malloc(sizeof(THFloatStorage));
    output->data = input->get_data();
    output->size = input->size();

    output->refcount  = input->get_count();
    //input->refcount;
    //TODO: not sure what isMapped is for
    //output->isMapped = false;//input->changesize()
    return output;
  }


  ebl::srg<float> * thsrg2eblsrg(THFloatStorage* input) {
    ebl::srg<float> *output = new ebl::srg<float>();
    output->set_data(input->data);
    output->changesize(input->size);
    //output->set_count(input->refcount);
    //TODO: not sure what isMapped is for
    //output->isMapped = false;//input->changesize()
    return output;
  }

  ebl::idx<float>* th2idx(THFloatTensor* input) {
    ebl::srg<float>* output_storage = thsrg2eblsrg(input->storage);
    ebl::idx<float> *output = new ebl::idx<float>(output_storage,         \
                                                  input->storageOffset,   \
                                                  input->nDimension,      \
                                                  input->size,            \
                                                  input->stride);         
    return output;
  }

  THFloatTensor* idx2th(ebl::idx<float> *input) {
    THFloatTensor *output;
    THFloatStorage* output_storage = eblsrg2thsrg(input->getstorage());
    if(input->order() == 1) {
      output = THFloatTensor_newWithStorage1d(output_storage,     \
                                              input->offset(),    \
                                              input->dim(0),      \
                                              input->mod(0));     \
    }
    else if(input->order() == 2) {
      output = THFloatTensor_newWithStorage2d(output_storage,     \
                                              input->offset(),    \
                                              input->dim(0),      \
                                              input->mod(0),      \
                                              input->dim(1),      \
                                              input->mod(1));
    }
    else if(input->order() == 3)
      output = THFloatTensor_newWithStorage3d(output_storage,     \
                                              input->offset(),    \
                                              input->dim(0),      \
                                              input->mod(0),      \
                                              input->dim(1),      \
                                              input->mod(1),      \
                                              input->dim(2),      \
                                              input->mod(2));
    else if(input->order() == 4)
      output = THFloatTensor_newWithStorage4d(output_storage,     \
                                              input->offset(),    \
                                              input->dim(0),      \
                                              input->mod(0),      \
                                              input->dim(1),      \
                                              input->mod(1),      \
                                              input->dim(2),      \
                                              input->mod(2),      \
                                              input->dim(3),      \
                                              input->mod(3));     \
    else
      std::cerr<<"ERROR: Unsupported dimensions in idx2TH";
    return output;
  }

  //----------------------------Double bindings--------------------------------
  THDoubleStorage* eblsrg2thsrg(ebl::srg<double> *input) {
    THDoubleStorage *output = (THDoubleStorage *)malloc(sizeof(THDoubleStorage));
    output->data = input->get_data();
    output->size = input->size();

    output->refcount  = 1;
    //input->refcount;
    //TODO: not sure what isMapped is for
    //output->isMapped = false;//input->changesize()
    return output;
  }


  ebl::srg<double> * thsrg2eblsrg(THDoubleStorage* input) {
    ebl::srg<double> *output = new ebl::srg<double>();
    output->set_data(input->data);
    output->changesize(input->size);
    //output->refcount  = 0;//input->refcount;
    //TODO: not sure what isMapped is for
    //output->isMapped = false;//input->changesize()
    return output;
  }

  ebl::idx<double>* th2idx(THDoubleTensor* input) {
    ebl::srg<double>* output_storage = thsrg2eblsrg(input->storage);
    ebl::idx<double> *output = new ebl::idx<double>(output_storage,       \
                                                    input->storageOffset, \
                                                    input->nDimension,    \
                                                    input->size,          \
                                                    input->stride);         
    return output;
  }

  THDoubleTensor* idx2th(ebl::idx<double>* input) {
    THDoubleTensor *output;
    THDoubleStorage* output_storage = eblsrg2thsrg(input->getstorage());
    if(input->order() == 1) {
      output = THDoubleTensor_newWithStorage1d(output_storage,    \
                                               input->offset(),    \
                                               input->dim(0),      \
                                               input->mod(0));     \
    }
    else if(input->order() == 2) {
      output = THDoubleTensor_newWithStorage2d(output_storage,    \
                                               input->offset(),    \
                                               input->dim(0),      \
                                               input->mod(0),      \
                                               input->dim(1),      \
                                               input->mod(1));
    }
    else if(input->order() == 3)
      output = THDoubleTensor_newWithStorage3d(output_storage,    \
                                               input->offset(),    \
                                               input->dim(0),      \
                                               input->mod(0),      \
                                               input->dim(1),      \
                                               input->mod(1),      \
                                               input->dim(2),      \
                                               input->mod(2));
    else if(input->order() == 4)
      output = THDoubleTensor_newWithStorage4d(output_storage,    \
                                               input->offset(),    \
                                               input->dim(0),      \
                                               input->mod(0),      \
                                               input->dim(1),      \
                                               input->mod(1),      \
                                               input->dim(2),      \
                                               input->mod(2),      \
                                               input->dim(3),      \
                                               input->mod(3));     \
    else
      std::cerr<<"ERROR: Unsupported dimensions in idx2TH";
    return output;
  }

  void freeTH(THLongTensor *tf_in, bool freestorage = true) {
    if(freestorage)
      free(tf_in->storage);
    free(tf_in->size);
    free(tf_in->stride);
    free(tf_in);
  }
  void freeTH(THFloatTensor *tf_in, bool freestorage = true) {
    if(freestorage)
      free(tf_in->storage);
    free(tf_in->size);
    free(tf_in->stride);
    free(tf_in);
  }
  
  void freeTH(THDoubleTensor *tf_in, bool freestorage = true) {
    if(freestorage)
      free(tf_in->storage);
    free(tf_in->size);
    free(tf_in->stride);
    free(tf_in);
  }

  // class thidx {
  // public:
  //   thidx(idx<double> &m) {
  //     idx2th();
  //   }
  //   THDoubleTensor *get_tensor() { return t; };
    
  // private:
  //   THDoubleTensor *t;
  // };

  ////////////////////////////////////////////////////////////////////////
  // th implementations

  // templates for in-place th_add

  template <>
  void th_add(idx<float32> &in1, idx<float32> &in2) {
    THFloatTensor* tf1=idx2th(&in1);
    THFloatTensor* tf2=idx2th(&in2);
    THFloatTensor_cadd(tf2,1,tf1);
    freeTH(tf1);
    freeTH(tf2);
  }

  template <>
  void th_add(idx<float64> &in1, idx<float64> &in2) {
    // thidx t(in1);
    // THDoubleTensor_cadd(t.tensor(), 1, t1.tensor());

    THDoubleTensor* tf1=idx2th(&in1);
    THDoubleTensor* tf2=idx2th(&in2);
    THDoubleTensor_cadd(tf2,1,tf1);
    freeTH(tf1);
    freeTH(tf2);
  }

  // templates for not in place th_add

  template <>
  void th_add(idx<float32> &in1, idx<float32> &in2,
	      idx<float32> &out) {
    idx<float32> outtmp(in1.get_idxdim());
    th_copy(in1,outtmp);
    THFloatTensor* tf2=idx2th(&in2);
    THFloatTensor* tfout=idx2th(&outtmp);
    THFloatTensor_cadd(tfout,1,tf2);
    th_copy(outtmp,out);
    freeTH(tf2);
    freeTH(tfout);
  }

  template <>
  void th_add(idx<float64> &in1, idx<float64> &in2,
	      idx<float64> &out) {
    idx<float64> outtmp(in1.get_idxdim());
    th_copy(in1,outtmp);
    THDoubleTensor* tf2=idx2th(&in2);
    THDoubleTensor* tfout=idx2th(&outtmp);
    THDoubleTensor_cadd(tfout,1,tf2);
    th_copy(outtmp,out);
    freeTH(tf2);
    freeTH(tfout);
  }
//-----------------------------------------------------
  // templates for th_copy


  template <>
  void th_copy(idx<float32> &in, idx<float32> &out) {
    THFloatTensor* tf_in=idx2th(&in);
    THFloatTensor* tf_out=idx2th(&out);
    THFloatTensor_copy(tf_out,tf_in);
    freeTH(tf_in);
    freeTH(tf_out);
  }

  template <>
  void th_copy(idx<float64> &in, idx<float64> &out) {
    THDoubleTensor* tf_in=idx2th(&in);
    THDoubleTensor* tf_out=idx2th(&out);
    THDoubleTensor_copy(tf_out,tf_in);
    freeTH(tf_in);
    freeTH(tf_out);
  }

//-----------------------------------------------------
  //templates for th_convolution

  template <>
  void th_convolution(idx<float32> &in, idx<float32> &ker,
		      idx<float32> &out, intg stride_x, intg stride_y) {
    THFloatTensor* tf_in=idx2th(&in);
    THFloatTensor* tf_out=idx2th(&out);
    THFloatTensor* tf_ker=idx2th(&ker);
    char type[2];
    type[0]='v';
    type[1]='x';
    THFloatLab_conv2Dmul(tf_out, 0.0, 1.0, tf_in, 
                         tf_ker, stride_x,stride_y,type);
    freeTH(tf_in);
    freeTH(tf_out);
    freeTH(tf_ker);
  }
template <>
  void th_convolution(idx<float64> &in, idx<float64> &ker,
		      idx<float64> &out, intg stride_x, intg stride_y) {  
  // THDoubleLab_validXCorr2Dptr(out.idx_ptr(), 1.0, in.idx_ptr(), 
  //                             in.dim(0),in.dim(1),
  //                             ker.idx_ptr(), ker.dim(0), ker.dim(1),
  //                             stride_x, stride_y);
    THDoubleTensor* tf_in=idx2th(&in);
    THDoubleTensor* tf_out=idx2th(&out);
    THDoubleTensor* tf_ker=idx2th(&ker);
    char type[2];
    type[0]='v';
    type[1]='x';
    THDoubleLab_conv2Dmul(tf_out, 0.0, 1.0, tf_in, 
                         tf_ker, stride_x,stride_y,type);
    freeTH(tf_in);
    freeTH(tf_out);
    freeTH(tf_ker);
  
  }
//-----------------------------------------------------

  template <>
  void th_convolution_3d(idx<float32> &in, idx<float32> &ker,
		      idx<float32> &out, 
                        intg stride_x, intg stride_y) {
    THFloatTensor* tf_in=idx2th(&in);
    THFloatTensor* tf_out=idx2th(&out);
    THFloatTensor* tf_ker=idx2th(&ker);
    char type[2];
    type[0]='v';
    type[1]='x';
    THFloatLab_conv2Dmv(tf_out, 0.0, 1.0, tf_in, 
                         tf_ker, stride_x,stride_y,type);
    freeTH(tf_in);
    freeTH(tf_out);
    freeTH(tf_ker);
  }

template <>
  void th_convolution_3d(idx<float64> &in, idx<float64> &ker,
                         idx<float64> &out, 
                        intg stride_x, intg stride_y) {  
    THDoubleTensor* tf_in=idx2th(&in);
    THDoubleTensor* tf_out=idx2th(&out);
    THDoubleTensor* tf_ker=idx2th(&ker);
    char type[2];
    type[0]='v';
    type[1]='x';
    THDoubleLab_conv2Dmv(tf_out, 0.0, 1.0, tf_in, 
                         tf_ker, stride_x,stride_y,type);
    freeTH(tf_in);
    freeTH(tf_out);
    freeTH(tf_ker);
  }
//-----------------------------------------------------

template <>
  void th_convolution_3dmap(idx<float32> &in, idx<float32> &ker,
		      idx<float32> &out, idx<intg> &table,
                            intg stride_x, intg stride_y) {  
  THFloatTensor* tf_in = idx2th(&in);
  THFloatTensor* tf_out = idx2th(&out);
  THFloatTensor* tf_ker = idx2th(&ker);
  THLongTensor* tf_table = idx2th(&table);
  THFloatTensor *input = tf_in;
  int kW = ker.dim(1);
  int kH = ker.dim(2);
  int dW = stride_x;
  int dH = stride_y;
  int nInputPlane = in.dim(0);
  int nOutputPlane = out.dim(0);
  THLongTensor *connTable = tf_table;
  THFloatTensor *weight = tf_ker;
  THFloatTensor *output = tf_out;
  THFloatTensor_resize3d(output, nOutputPlane,
                      (input->size[1] - kH) / dH + 1, 
                      (input->size[2] - kW) / dW + 1);
  // contiguous
  input = THFloatTensor_newContiguous(input);
  output = THFloatTensor_newContiguous(output);
  // get raw pointers
  float *input_data = THFloatTensor_data(input);
  float *output_data = THFloatTensor_data(output);
  float *weight_data = THFloatTensor_data(weight);
  // and dims
  long input_n = input->size[0];
  long input_h = input->size[1];
  long input_w = input->size[2];
  long output_n = output->size[0];
  long output_h = output->size[1];
  long output_w = output->size[2];
  long weight_n = weight->size[0];
  long weight_h = weight->size[1];
  long weight_w = weight->size[2];
  // convolve all maps
  int i,o,k;
  int nweight = connTable->size[0];
  for (k = 0; k < nweight; k++) {
    // get offsets for input/output
    o = (int)THLongTensor_get2d(connTable,k,1);
    i = (int)THLongTensor_get2d(connTable,k,0);
    // convolve each map
    THFloatLab_validXCorr2Dptr(output_data + o*output_w*output_h,
                            1.0,
                            input_data + i*input_w*input_h, input_h, input_w,
                            weight_data + k*weight_w*weight_h, weight_h, weight_w,
                            dH, dW);
  }
  // clean up
  THFloatTensor_free(input);
  THFloatTensor_free(output);
  freeTH(tf_in);
  freeTH(tf_out);
  freeTH(tf_ker);
  freeTH(tf_table);
}
template <>
  void th_convolution_3dmap(idx<float64> &in, idx<float64> &ker,
		      idx<float64> &out, idx<intg> &table,
                            intg stride_x, intg stride_y) {
  THDoubleTensor* tf_in = idx2th(&in);
  THDoubleTensor* tf_out = idx2th(&out);
  THDoubleTensor* tf_ker = idx2th(&ker);
  THLongTensor* tf_table = idx2th(&table);
  THDoubleTensor *input = tf_in;
  int kW = ker.dim(1);
  int kH = ker.dim(2);
  int dW = stride_x;
  int dH = stride_y;
  int nInputPlane = in.dim(0);
  int nOutputPlane = out.dim(0);
  THLongTensor *connTable = tf_table;
  THDoubleTensor *weight = tf_ker;
  THDoubleTensor *output = tf_out;
  THDoubleTensor_resize3d(output, nOutputPlane,
                      (input->size[1] - kH) / dH + 1, 
                      (input->size[2] - kW) / dW + 1);
  // contiguous
  input = THDoubleTensor_newContiguous(input);
  output = THDoubleTensor_newContiguous(output);
  // get raw pointers
  double *input_data = THDoubleTensor_data(input);
  double *output_data = THDoubleTensor_data(output);
  double *weight_data = THDoubleTensor_data(weight);
  // and dims
  long input_n = input->size[0];
  long input_h = input->size[1];
  long input_w = input->size[2];
  long output_n = output->size[0];
  long output_h = output->size[1];
  long output_w = output->size[2];
  long weight_n = weight->size[0];
  long weight_h = weight->size[1];
  long weight_w = weight->size[2];
  // convolve all maps
  int i,o,k;
  int nweight = connTable->size[0];
  for (k = 0; k < nweight; k++) {
    // get offsets for input/output
    o = (int)THLongTensor_get2d(connTable,k,1);
    i = (int)THLongTensor_get2d(connTable,k,0);
    // convolve each map
    THDoubleLab_validXCorr2Dptr(output_data + o*output_w*output_h,
                            1.0,
                            input_data + i*input_w*input_h, input_h, input_w,
                            weight_data + k*weight_w*weight_h, weight_h, weight_w,
                            dH, dW);
  }
  // clean up
  THDoubleTensor_free(input);
  THDoubleTensor_free(output);
  freeTH(tf_in);
  freeTH(tf_out);
  freeTH(tf_ker);
  freeTH(tf_table);
}
  ///////////////////////////////////////////////////////////////////////////

  template <>
  void th_convolution_3dmap_bprop(idx<float32> &inx, idx<float32> &kerx,
                                  idx<float32> &outdx, idx<float32> &indx, 
                                  idx<float32> &kerdx,idx<intg> &table,
                            intg stride_w, intg stride_h) {  
    // Update grad input
    THFloatTensor* tf_inx = idx2th(&inx);
    THFloatTensor* tf_indx = idx2th(&indx);
    THFloatTensor* tf_outdx = idx2th(&outdx);
    THFloatTensor* tf_kerx = idx2th(&kerx);
    THFloatTensor* tf_kerdx = idx2th(&kerdx);
    THLongTensor* tf_table = idx2th(&table);
    THFloatTensor *input = tf_inx;  
    THFloatTensor *gradOutput = tf_outdx;  
    int kW = kerx.dim(1);
    int kH = kerx.dim(2);
    int dW = stride_w;
    int dH = stride_h;
    int nInputPlane = indx.dim(0);
    int nOutputPlane = outdx.dim(0);
    THLongTensor *connTable = tf_table;
    THFloatTensor *weight = tf_kerx;
    THFloatTensor *gradWeight = tf_kerdx;
    THFloatTensor *gradInput = tf_indx;
    // contiguous
    input = THFloatTensor_newContiguous(input);
    gradInput = THFloatTensor_newContiguous(gradInput);
    gradOutput = THFloatTensor_newContiguous(gradOutput);
    // Resize/Zero
    THFloatTensor_resizeAs(gradInput, input);
    THFloatTensor_zero(gradInput);
    // get raw pointers
    float *input_data = THFloatTensor_data(input);
    float *gradInput_data = THFloatTensor_data(gradInput);
    float *gradOutput_data = THFloatTensor_data(gradOutput);
    float *weight_data = THFloatTensor_data(weight);
    float *gradWeight_data = THFloatTensor_data(gradWeight);
    // and dims
    long input_n = input->size[0];
    long input_h = input->size[1];
    long input_w = input->size[2];
    long output_n = gradOutput->size[0];
    long output_h = gradOutput->size[1];
    long output_w = gradOutput->size[2];
    long weight_n = weight->size[0];
    long weight_h = weight->size[1];
    long weight_w = weight->size[2];
    // updateGradInput all
    int k;
    int nkernel = connTable->size[0];
    for(k = 0; k < nkernel; k++)
      {
        int o = (int)THLongTensor_get2d(connTable,k,1);
        int i = (int)THLongTensor_get2d(connTable,k,0);
        // gradient to input
        THFloatLab_fullConv2Dptr(gradInput_data + i*input_w*input_h,
                                  1.0,
                                  gradOutput_data + o*output_w*output_h,  output_h,  output_w,
                                  weight_data + k*weight_w*weight_h, weight_h, weight_w,
                                  dH, dW);
        THFloatLab_validXCorr2DRevptr(gradWeight_data + k*weight_w*weight_h,
                                       1,
                                       input_data + i*input_w*input_h, input_h, input_w,
                                       gradOutput_data + o*output_w*output_h, 
                                       output_h, output_w,
                                       dH, dW);
      }
    // clean up
    THFloatTensor_free(input);
    THFloatTensor_free(gradInput);
    THFloatTensor_free(gradOutput);
    // final cleanup
    freeTH(tf_inx);
    freeTH(tf_indx);
    freeTH(tf_outdx);
    freeTH(tf_kerx);
    freeTH(tf_kerdx);
    freeTH(tf_table);
}

  template <>
  void th_convolution_3dmap_bprop(idx<float64> &inx, idx<float64> &kerx,
                                  idx<float64> &outdx, idx<float64> &indx, 
                                  idx<float64> &kerdx,
                                  idx<intg> &table,
                            intg stride_w, intg stride_h) {  
    // Update grad input
    THDoubleTensor* tf_inx = idx2th(&inx);
    THDoubleTensor* tf_indx = idx2th(&indx);
    THDoubleTensor* tf_outdx = idx2th(&outdx);
    THDoubleTensor* tf_kerx = idx2th(&kerx);
    THDoubleTensor* tf_kerdx = idx2th(&kerdx);
    THLongTensor* tf_table = idx2th(&table);
    THDoubleTensor *input = tf_inx;  
    THDoubleTensor *gradOutput = tf_outdx;  
    int kW = kerx.dim(1);
    int kH = kerx.dim(2);
    int dW = stride_w;
    int dH = stride_h;
    int nInputPlane = indx.dim(0);
    int nOutputPlane = outdx.dim(0);
    THLongTensor *connTable = tf_table;
    THDoubleTensor *weight = tf_kerx;
    THDoubleTensor *gradWeight = tf_kerdx;
    THDoubleTensor *gradInput = tf_indx;
    // contiguous
    input = THDoubleTensor_newContiguous(input);
    gradInput = THDoubleTensor_newContiguous(gradInput);
    gradOutput = THDoubleTensor_newContiguous(gradOutput);
    // Resize/Zero
    THDoubleTensor_resizeAs(gradInput, input);
    THDoubleTensor_zero(gradInput);
    // get raw pointers
    double *input_data = THDoubleTensor_data(input);
    double *gradInput_data = THDoubleTensor_data(gradInput);
    double *gradOutput_data = THDoubleTensor_data(gradOutput);
    double *weight_data = THDoubleTensor_data(weight);
    double *gradWeight_data = THDoubleTensor_data(gradWeight);
    // and dims
    long input_n = input->size[0];
    long input_h = input->size[1];
    long input_w = input->size[2];
    long output_n = gradOutput->size[0];
    long output_h = gradOutput->size[1];
    long output_w = gradOutput->size[2];
    long weight_n = weight->size[0];
    long weight_h = weight->size[1];
    long weight_w = weight->size[2];
    // updateGradInput all
    int k;
    int nkernel = connTable->size[0];
    for(k = 0; k < nkernel; k++)
      {
        int o = (int)THLongTensor_get2d(connTable,k,1);
        int i = (int)THLongTensor_get2d(connTable,k,0);
        // gradient to input
        THDoubleLab_fullConv2Dptr(gradInput_data + i*input_w*input_h,
                                  1.0,
                                  gradOutput_data + o*output_w*output_h,  output_h,  output_w,
                                  weight_data + k*weight_w*weight_h, weight_h, weight_w,
                                  dH, dW);
        THDoubleLab_validXCorr2DRevptr(gradWeight_data + k*weight_w*weight_h,
                                       1,
                                       input_data + i*input_w*input_h, input_h, input_w,
                                       gradOutput_data + o*output_w*output_h, 
                                       output_h, output_w,
                                       dH, dW);
      }
    // clean up
    THDoubleTensor_free(input);
    THDoubleTensor_free(gradInput);
    THDoubleTensor_free(gradOutput);
    // final cleanup
    freeTH(tf_inx);
    freeTH(tf_indx);
    freeTH(tf_outdx);
    freeTH(tf_kerx);
    freeTH(tf_kerdx);
    freeTH(tf_table);
}

  /////////////////////////////////////////////////////////////////////
  // th_tanh and th_dtanh

  template <>
  void th_tanh(idx<float32> &in, idx<float32> &out) {
    THFloatTensor *tf_in = idx2th(&in);
    THFloatTensor *tf_out = idx2th(&out);
    THFloatTensor_copy(tf_out,tf_in);
    THFloatTensor_tanh(tf_out);
    freeTH(tf_in);
    freeTH(tf_out);
  }
  template <>
  void th_tanh(idx<float64> &in, idx<float64> &out) {
    THDoubleTensor *tf_in = idx2th(&in);
    THDoubleTensor *tf_out = idx2th(&out);
    THDoubleTensor_copy(tf_out,tf_in);
    THDoubleTensor_tanh(tf_out);
    freeTH(tf_in);
    freeTH(tf_out);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  // th_pow

  template <>
  void th_pow(idx<float32> &in, idx<float32> &out, float32 p) {
    THFloatTensor *tf_in = idx2th(&in);
    THFloatTensor *tf_out = idx2th(&out);
    THFloatTensor_copy(tf_out,tf_in);
    THFloatTensor_pow(tf_out, p);
    freeTH(tf_in);
    freeTH(tf_out);
  }
  template <>
  void th_pow(idx<float64> &in, idx<float64> &out, float64 p) {
    THDoubleTensor *tf_in = idx2th(&in);
    THDoubleTensor *tf_out = idx2th(&out);
    THDoubleTensor_copy(tf_out,tf_in);
    THDoubleTensor_pow(tf_out, p);
    freeTH(tf_in);
    freeTH(tf_out);
  }
  //////////////////////////////////////////////////////////////////////////////
  // th_maxpool_3d

template <>
  void th_maxpool_3d(idx<float32> &in, intg kernel_w, intg kernel_h,
		      idx<float32> &out, 
                            intg stride_x, intg stride_y, idx<float32> &indices_e) {
  THFloatTensor* tf_in = idx2th(&in);
  THFloatTensor* tf_out = idx2th(&out);
  THFloatTensor* tf_indices = idx2th(&indices_e);
  THFloatTensor *input = tf_in;
  int kW = kernel_w;
  int kH = kernel_h;
  int dW = stride_x;
  int dH = stride_y;
  int nInputPlane = in.dim(0);
  int nOutputPlane = out.dim(0);
  THFloatTensor *indices = tf_indices;
  THFloatTensor *output = tf_out;

  // sizes
  long nslices = input->size[0];
  long iheight = input->size[1];
  long iwidth = input->size[2];
  long oheight = (iheight - kH) / dH + 1;
  long owidth = (iwidth - kW) / dW + 1;

  // get contiguous input
  input = THFloatTensor_newContiguous(input);

  // resize output
  THFloatTensor_resize3d(output, nslices, oheight, owidth);

  // indices will contain i,j locatyions for each output point
  THFloatTensor_resize4d(indices, 2, nslices, oheight, owidth);

  // get raw pointers
  float *input_data = THFloatTensor_data(input);
  float *output_data = THFloatTensor_data(output);
  float *indices_data = THFloatTensor_data(indices);

  // compute max pooling for each input slice
  long k;
  for (k = 0; k < nslices; k++) {
    // pointers to slices
    float *input_p = input_data + k*iwidth*iheight;
    float *output_p = output_data + k*owidth*oheight;
    float *indy_p = indices_data + k*owidth*oheight;
    float *indx_p = indices_data + (k+nslices)*owidth*oheight;

    // loop over output
    int i,j;
    for(i = 0; i < oheight; i++) {
      for(j = 0; j < owidth; j++) {
        // local pointers
        float *ip = input_p + i*iwidth*dH + j*dW;
        float *op = output_p + i*owidth + j;
        float *indyp = indy_p + i*owidth + j;
        float *indxp = indx_p + i*owidth + j;

        // compute local max:
        long maxindex = -1;
        float maxval = -THInf;
        long tcntr = 0;
        int x,y;
        for(y = 0; y < kH; y++) {
          for(x = 0; x < kW; x++) {
            float val = *(ip + y*iwidth + x);
            if (val > maxval) {
              maxval = val;
              maxindex = tcntr;
            }
            tcntr++;
          }
        }

        // set output to local max
        *op = maxval;

        // store location of max (x,y)
        *indyp = (int)(maxindex / kW)+1;
        *indxp = (maxindex % kW) +1;
      }
    }
  }
  
  bool freestorage = true;
  if(input->storage == tf_in->storage)
    freestorage = false;
  // cleanup
  THFloatTensor_free(input);
  freeTH(tf_in);//, freestorage);
  freeTH(tf_out, false);
  freeTH(tf_indices);
}

template <>
  void th_maxpool_3d(idx<float64> &in, intg kernel_w, intg kernel_h,
		      idx<float64> &out, 
                     intg stride_x, intg stride_y, idx<float64> &indices_e) {
  THDoubleTensor* tf_in = idx2th(&in);
  THDoubleTensor* tf_out = idx2th(&out);
  THDoubleTensor* tf_indices = idx2th(&indices_e);
  THDoubleTensor *input = tf_in;
  int kW = kernel_w;
  int kH = kernel_h;
  int dW = stride_x;
  int dH = stride_y;
  int nInputPlane = in.dim(0);
  int nOutputPlane = out.dim(0);
  THDoubleTensor *indices = tf_indices;
  THDoubleTensor *output = tf_out;

  // sizes
  long nslices = input->size[0];
  long iheight = input->size[1];
  long iwidth = input->size[2];
  long oheight = (iheight - kH) / dH + 1;
  long owidth = (iwidth - kW) / dW + 1;

  // get contiguous input
  input = THDoubleTensor_newContiguous(input);

  // resize output
  THDoubleTensor_resize3d(output, nslices, oheight, owidth);

  // indices will contain i,j locatyions for each output point
  THDoubleTensor_resize4d(indices, 2, nslices, oheight, owidth);

  // get raw pointers
  double *input_data = THDoubleTensor_data(input);
  double *output_data = THDoubleTensor_data(output);
  double *indices_data = THDoubleTensor_data(indices);

  // compute max pooling for each input slice
  long k;
  for (k = 0; k < nslices; k++) {
    // pointers to slices
    double *input_p = input_data + k*iwidth*iheight;
    double *output_p = output_data + k*owidth*oheight;
    double *indy_p = indices_data + k*owidth*oheight;
    double *indx_p = indices_data + (k+nslices)*owidth*oheight;

    // loop over output
    int i,j;
    for(i = 0; i < oheight; i++) {
      for(j = 0; j < owidth; j++) {
        // local pointers
        double *ip = input_p + i*iwidth*dH + j*dW;
        double *op = output_p + i*owidth + j;
        double *indyp = indy_p + i*owidth + j;
        double *indxp = indx_p + i*owidth + j;

        // compute local max:
        long maxindex = -1;
        double maxval = -THInf;
        long tcntr = 0;
        int x,y;
        for(y = 0; y < kH; y++) {
          for(x = 0; x < kW; x++) {
            double val = *(ip + y*iwidth + x);
            if (val > maxval) {
              maxval = val;
              maxindex = tcntr;
            }
            tcntr++;
          }
        }

        // set output to local max
        *op = maxval;

        // store location of max (x,y)
        *indyp = (int)(maxindex / kW)+1;
        *indxp = (maxindex % kW) +1;
      }
    }
  }

  bool freestorage = true;
  if(input->storage == tf_in->storage)
    freestorage = false;
  // cleanup
  THDoubleTensor_free(input);
  freeTH(tf_in);//, freestorage);
  freeTH(tf_out);
  freeTH(tf_indices);
}
  ///////////////////////////////////////////////////////////////////////////

  template <>
  void th_maxpool_3d_bprop(idx<float32> &inx, intg kernel_w, intg kernel_h,
                                  idx<float32> &outdx, idx<float32> &indx, 
                           intg stride_w, intg stride_h, idx<float32> &indices_e) {  
    // Update grad input
    THFloatTensor* tf_inx = idx2th(&inx);
    THFloatTensor* tf_indx = idx2th(&indx);
    THFloatTensor* tf_outdx = idx2th(&outdx);
    THFloatTensor* tf_indices = idx2th(&indices_e);
    THFloatTensor *input = tf_inx;  
    THFloatTensor *gradOutput = tf_outdx;  
    int kW = kernel_w;
    int kH = kernel_h;
    int dW = stride_w;
    int dH = stride_h;
    int nInputPlane = indx.dim(0);
    int nOutputPlane = outdx.dim(0);
    THFloatTensor *gradInput = tf_indx;
    THFloatTensor *indices = tf_indices;

    // get contiguous gradOutput
    gradOutput = THFloatTensor_newContiguous(gradOutput);

    // resize
    THFloatTensor_resizeAs(gradInput, input);
    THFloatTensor_zero(gradInput);

    // sizes
    int ichannels = input->size[0];
    int iheight = input->size[1];
    int iwidth = input->size[2];
    int ochannels = ichannels;
    int oheight = gradOutput->size[1];
    int owidth = gradOutput->size[2];

    // get raw pointers
    float *gradInput_data = THFloatTensor_data(gradInput);
    float *gradOutput_data = THFloatTensor_data(gradOutput);
    float *indices_data = THFloatTensor_data(indices);

    // backprop
    long k;
    for (k = 0; k < input->size[0]; k++) {
      // pointers to slices
      float *gradOutput_p = gradOutput_data + k*owidth*oheight;
      float *gradInput_p = gradInput_data + k*iwidth*iheight;
      float *indy_p = indices_data + k*owidth*oheight;
      float *indx_p = indices_data + (k+ochannels)*owidth*oheight;

      // calculate max points
      int i,j;
      for(i = 0; i < oheight; i++) {
        for(j = 0; j < owidth; j++) {
          // retrieve position of max
          long maxi = *(indy_p + i*owidth + j) - 1 + i*dH;
          long maxj = *(indx_p + i*owidth + j) - 1 + j*dW;

          // update gradient
          *(gradInput_p + maxi*iwidth + maxj) += *(gradOutput_p + i*owidth + j);
        }
      }
    }
    bool freestorage = true;
    if(gradOutput->storage == tf_outdx->storage)
      freestorage = false;
    // cleanup
    THFloatTensor_free(gradOutput);
  
    // final cleanup
    freeTH(tf_outdx);//, freestorage);
  
    // final cleanup
    freeTH(tf_inx);
    freeTH(tf_indx);  
    freeTH(tf_indices);
  }

  template <>
  void th_maxpool_3d_bprop(idx<float64> &inx, intg kernel_w, intg kernel_h,
                                  idx<float64> &outdx, idx<float64> &indx, 
                           intg stride_w, intg stride_h, idx<float64> &indices_e) {  
    // Update grad input
    THDoubleTensor* tf_inx = idx2th(&inx);
    THDoubleTensor* tf_indx = idx2th(&indx);
    THDoubleTensor* tf_outdx = idx2th(&outdx);
    THDoubleTensor* tf_indices = idx2th(&indices_e);
    THDoubleTensor *input = tf_inx;  
    THDoubleTensor *gradOutput = tf_outdx;  
    int kW = kernel_w;
    int kH = kernel_h;
    int dW = stride_w;
    int dH = stride_h;
    int nInputPlane = indx.dim(0);
    int nOutputPlane = outdx.dim(0);
    THDoubleTensor *gradInput = tf_indx;
    THDoubleTensor *indices = tf_indices;

    // get contiguous gradOutput
    gradOutput = THDoubleTensor_newContiguous(gradOutput);

    // resize
    THDoubleTensor_resizeAs(gradInput, input);
    THDoubleTensor_zero(gradInput);

    // sizes
    int ichannels = input->size[0];
    int iheight = input->size[1];
    int iwidth = input->size[2];
    int ochannels = ichannels;
    int oheight = gradOutput->size[1];
    int owidth = gradOutput->size[2];

    // get raw pointers
    double *gradInput_data = THDoubleTensor_data(gradInput);
    double *gradOutput_data = THDoubleTensor_data(gradOutput);
    double *indices_data = THDoubleTensor_data(indices);

    // backprop
    long k;
    for (k = 0; k < input->size[0]; k++) {
      // pointers to slices
      double *gradOutput_p = gradOutput_data + k*owidth*oheight;
      double *gradInput_p = gradInput_data + k*iwidth*iheight;
      double *indy_p = indices_data + k*owidth*oheight;
      double *indx_p = indices_data + (k+ochannels)*owidth*oheight;

      // calculate max points
      int i,j;
      for(i = 0; i < oheight; i++) {
        for(j = 0; j < owidth; j++) {
          // retrieve position of max
          long maxi = *(indy_p + i*owidth + j) - 1 + i*dH;
          long maxj = *(indx_p + i*owidth + j) - 1 + j*dW;

          // update gradient
          *(gradInput_p + maxi*iwidth + maxj) += *(gradOutput_p + i*owidth + j);
        }
      }
    }

  bool freestorage = true;
  if(gradOutput->storage == tf_outdx->storage)
    freestorage = false;
  // cleanup
  THDoubleTensor_free(gradOutput);
  
  // final cleanup
  freeTH(tf_outdx);//, freestorage);
  freeTH(tf_inx);
  freeTH(tf_indx);
  freeTH(tf_indices);
  }

} /* ebl namespace */
#endif /* __TH__ */
