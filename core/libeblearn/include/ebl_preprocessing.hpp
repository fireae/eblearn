/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet *
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

namespace ebl {

// channels_module /////////////////////////////////////////////////////////////

template <typename T>
channels_module<T>::channels_module(bool globnorm_, const char *name_)
    : module_1_1<T>(name_), globnorm(globnorm_) {
}

template <typename T>
channels_module<T>::~channels_module() {
}

// channorm_module /////////////////////////////////////////////////////////////

template <typename T>
channorm_module<T>::
channorm_module(idxdim &kerdim_, bool mirror_, t_norm norm_mode_,
                const char *name_, int nf, bool globnorm, double epsilon,
                double epsilon2)
    : channels_module<T>(globnorm, name_), normker(kerdim_),
      tmp(1,1,1), norm(NULL), mirror(mirror_), norm_mode(norm_mode_) {
  norm = new_norm(normker, mirror, norm_mode, nf, epsilon, epsilon2);
  EDEBUG(this->describe());
}

template <typename T>
channorm_module<T>::~channorm_module() {
  if (norm) delete norm;
}

template <typename T>
bool channorm_module<T>::resize_output(state<T> &in, state<T> &out, idxdim *d) {
  module_1_1<T>::resize_output(in, out, d);
  return module_1_1<T>::resize_output(in, tmp, d);
}

template <typename T>
bool channorm_module<T>::resize_output(idx<T> &in, idx<T> &out, idxdim *d) {
  module_1_1<T>::resize_output(in, out, d);
  return module_1_1<T>::resize_output(in, tmp, d);
}

template <typename T>
std::string channorm_module<T>::describe() {
  std::string s;
  s << this->name() << "'s normalization module: " << norm->describe();
  return s;
}

template <typename T>
module_1_1<T>* channorm_module<T>::
new_norm(idxdim &normker, bool mirror, t_norm norm_mode, int nf,
         double epsilon, double epsilon2) {
  switch (norm_mode) {
    case WSTD_NORM:
      return new contrast_norm_module<T>
          (normker, nf, mirror,
	   false, /* never do thresholding for inputs */
	   true, /* always do global norm for inputs */
	   NULL, "chan_cnorm",
	   true, /* normalize across features */
	   false,
	   DEFAULT_GAUSSIAN_COEFF, false, 1.0, epsilon, epsilon2);
      break ;
    case LAPLACIAN_NORM:
      return new laplacian_module<T>(nf, mirror, true);
      break ;
    default: eblerror("unknown normalization mode " << norm_mode);
  }
  return NULL;
}

// rgb_to_ynuv_module //////////////////////////////////////////////////////////

template <typename T>
rgb_to_ynuv_module<T>::
rgb_to_ynuv_module(idxdim &normker_, bool mirror, t_norm norm_mode,
                   bool globnorm, double epsilon, double epsilon2)
    : channorm_module<T>(normker_, mirror, norm_mode, "rgb_to_ynuv",
			 1, globnorm, epsilon, epsilon2) {
}

template <typename T>
rgb_to_ynuv_module<T>::~rgb_to_ynuv_module() {
}

template <typename T>
void rgb_to_ynuv_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize (iff necessary)
  if (in.dim(0) != 3) {
    // cerr << "warning: in rgb_to_ynuv, input is not 3-channel, "
    // 	   << "ignoring color." << endl;
  } else {
    idx<T> uv, yuv;
    // RGB to YUV
    idx_eloop2(inx, in, T, outx, out, T) {
      idx_eloop2(inxx, inx, T, outxx, outx, T) {
        rgb_to_yuv_1D(inxx, outxx); }}
  }
  EDEBUG(this->name() << ": yuv " << out << " min " << idx_min(out)
         << " max " << idx_max(out));
  // remove global mean and divide by stddev of UV
  idx<T> uv = out.narrow(0, 2, 1);
  if (this->globnorm) image_global_normalization(uv);
  else {
    idx_addc(uv, (T)-128, uv);
    idx_dotc(uv, (T).01, uv);
  }
  EDEBUG(this->name() << ": y(uv-normalizeed) " << out << " min "
         << idx_min(out) << " max " << idx_max(out));
  // normalize Y
  this->tmp = out.narrow(0, 1, 0);
  // lcn
  this->norm->fprop1(this->tmp, this->tmp);
  EDEBUG(this->name() << ": out " << out << " min " << idx_min(out)
         << " max " << idx_max(out));
}

template <typename T>
module_1_1<T>* rgb_to_ynuv_module<T>::copy(parameter<T> *p) {
  return new rgb_to_ynuv_module<T>(this->normker, this->mirror,
                                   this->norm_mode, this->globnorm);
}

// rgb_to_ynuvn_module /////////////////////////////////////////////////////////

template <typename T>
rgb_to_ynuvn_module<T>::
rgb_to_ynuvn_module(idxdim &normker_, bool mirror, t_norm norm_mode,
                    bool globnorm, double epsilon, double epsilon2)
    : channorm_module<T>(normker_, mirror, norm_mode, "rgb_to_ynuvn",
				1, globnorm, epsilon, epsilon2) {
  norm2 = this->new_norm(normker_, mirror, norm_mode, 2, epsilon, epsilon2);
}

template <typename T>
rgb_to_ynuvn_module<T>::~rgb_to_ynuvn_module() {
  delete norm2;
}

template <typename T>
void rgb_to_ynuvn_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize (iff necessary)
  if (in.dim(0) != 3) {
    // cerr << "warning: in rgb_to_ynuvn, input is not 3-channel, "
    // 	   << "ignoring color." << endl;
  } else {
    idx<T> uv, yuv;
    // RGB to YUV
    idx_eloop2(inx, in, T, outx, out, T) {
      idx_eloop2(inxx, inx, T, outxx, outx, T) {
        rgb_to_yuv_1D(inxx, outxx); }}
  }
  // normalize Y
  this->tmp = out.narrow(0, 1, 0);
  // lcn
  this->norm->fprop1(this->tmp, this->tmp);
  // normalize UV
  this->tmp = out.narrow(0, 2, 1);
  // lcn
  norm2->fprop1(this->tmp, this->tmp);
}

template <typename T>
module_1_1<T>* rgb_to_ynuvn_module<T>::copy(parameter<T> *p) {
  return new rgb_to_ynuvn_module<T>(this->normker, this->mirror,
				    this->norm_mode, this->globnorm);
}

////////////////////////////////////////////////////////////////
// rgb_to_ynunvn_module

template <typename T>
rgb_to_ynunvn_module<T>::
rgb_to_ynunvn_module(idxdim &normker_, bool mirror, t_norm norm_mode,
                     bool globnorm, double epsilon, double epsilon2)
    : channorm_module<T>(normker_, mirror, norm_mode, "rgb_to_ynunvn",
			 1, globnorm, epsilon, epsilon2) {
}

template <typename T>
rgb_to_ynunvn_module<T>::~rgb_to_ynunvn_module() {
}

template <typename T>
void rgb_to_ynunvn_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize (iff necessary)
  if (in.dim(0) != 3) {
    // cerr << "warning: in rgb_to_ynunvn, input is not 3-channel, "
    // 	   << "ignoring color." << endl;
  } else {
    idx<T> uv, yuv;
    // RGB to YUV
    idx_eloop2(inx, in, T, outx, out, T) {
      idx_eloop2(inxx, inx, T, outxx, outx, T) {
        rgb_to_yuv_1D(inxx, outxx); }}
  }
  // normalize Y
  this->tmp = out.narrow(0, 1, 0);
  // lcn
  this->norm->fprop1(this->tmp, this->tmp);
  // normalize U
  this->tmp = out.narrow(0, 1, 1);
  // lcn
  this->norm->fprop1(this->tmp, this->tmp);
  // normalize V
  this->tmp = out.narrow(0, 1, 2);
  // lcn
  this->norm->fprop1(this->tmp, this->tmp);
}

template <typename T>
module_1_1<T>* rgb_to_ynunvn_module<T>::copy(parameter<T> *p) {
  return new rgb_to_ynunvn_module<T>(this->normker, this->mirror,
				     this->norm_mode, this->globnorm);
}

////////////////////////////////////////////////////////////////
// rgb_to_yuv_module

template <typename T>
rgb_to_yuv_module<T>::rgb_to_yuv_module(bool globnorm)
    : channels_module<T>(globnorm, "rgb_to_yuv") {
}

template <typename T>
rgb_to_yuv_module<T>::~rgb_to_yuv_module() {
}

template <typename T>
void rgb_to_yuv_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  channels_module<T>::resize_output(in, out); // resize (iff necessary)
  if (in.dim(0) != 3) {
    eblerror("expected 3 channels in dim 0 but found: " << in);
  } else {
    // RGB to YUV
    idx_eloop2(inx, in, T, outx, out, T) {
      idx_eloop2(inxx, inx, T, outxx, outx, T) {
        rgb_to_yuv_1D(inxx, outxx); }}
    // remove global mean and divide by stddev
    if (this->globnorm) { // normalize Y
      idx<T> y = out.narrow(0, 1, 0);
      image_global_normalization(y);
    }
    // normalize UV
    idx<T> uv = out.narrow(0, 2, 1);
    if (this->globnorm) image_global_normalization(uv);
    else { // fixed normalization around -1,1
      idx<T> uv = out.narrow(0, 2, 1);
      idx_addc(uv, (T)-128, uv);
      idx_dotc(uv, (T).01, uv);
    }
  }
}

template <typename T>
module_1_1<T>* rgb_to_yuv_module<T>::copy(parameter<T> *p) {
  return new rgb_to_yuv_module<T>(this->globnorm);
}

////////////////////////////////////////////////////////////////
// rgb_to_yuvn_module

template <typename T>
rgb_to_yuvn_module<T>::
rgb_to_yuvn_module(idxdim &normker_, bool mirror, t_norm norm_mode,
                   bool globnorm, double epsilon, double epsilon2)
    : channorm_module<T>(normker_, mirror, norm_mode, "rgb_to_yuvn", 3,
			 globnorm, epsilon, epsilon2) {
}

template <typename T>
rgb_to_yuvn_module<T>::~rgb_to_yuvn_module() {
}

template <typename T>
void rgb_to_yuvn_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize (iff necessary)
  if (in.dim(0) != 3) {
    eblerror("expected 3 channels in dim 0 but found: " << in);
  } else {
    // RGB to YUV
    idx_eloop2(inx, in, T, outx, out, T) {
      idx_eloop2(inxx, inx, T, outxx, outx, T) {
        rgb_to_yuv_1D(inxx, outxx); }}
  }
  // first normalize globally Y and UV separately
  idx<T> y = out.narrow(0, 1, 0);
  image_global_normalization(y);
  idx<T> uv = out.narrow(0, 2, 1);
  image_global_normalization(uv);
  // normalize YUV
  this->tmp = out;
  this->norm->fprop1(this->tmp, this->tmp); // local
}

template <typename T>
module_1_1<T>* rgb_to_yuvn_module<T>::copy(parameter<T> *p) {
  return new rgb_to_yuvn_module<T>(this->normker, this->mirror,
                                          this->norm_mode, this->globnorm);
}

// rgb_to_rgb_module ///////////////////////////////////////////////////////////

template <typename T>
rgb_to_rgb_module<T>::
rgb_to_rgb_module(bool globnorm, const char *name_)
    : channels_module<T>(globnorm, name_) {
}

template <typename T>
rgb_to_rgb_module<T>::~rgb_to_rgb_module() {
}

template <typename T>
void rgb_to_rgb_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  channels_module<T>::resize_output(in, out); // resize (iff necessary)
  // plain copy
  idx_copy(in, out);
  // remove global mean and divide by stddev
  if (this->globnorm) image_global_normalization(out);
}

template <typename T>
module_1_1<T>* rgb_to_rgb_module<T>::copy(parameter<T> *p) {
  return new rgb_to_rgb_module<T>(this->globnorm);
}

// y_to_y_module ///////////////////////////////////////////////////////////

template <typename T>
y_to_y_module<T>::y_to_y_module(bool globnorm)
    : channels_module<T>(globnorm, "y_to_y") {
}

template <typename T>
y_to_y_module<T>::~y_to_y_module() {
}

template <typename T>
void y_to_y_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  channels_module<T>::resize_output(in, out); // resize (iff necessary)
  // plain copy
  idx_copy(in, out);
  // remove global mean and divide by stddev
  if (this->globnorm) image_global_normalization(out);
}

template <typename T>
module_1_1<T>* y_to_y_module<T>::copy(parameter<T> *p) {
  return new y_to_y_module<T>(this->globnorm);
}

// rgb_to_rgbn_module //////////////////////////////////////////////////////////

template <typename T>
rgb_to_rgbn_module<T>::
rgb_to_rgbn_module(idxdim &normker_, bool mirror, t_norm norm_mode,
                   bool globnorm, double epsilon, double epsilon2)
    : channorm_module<T>(normker_, mirror, norm_mode, "rgb_to_rgbn", 3,
				globnorm, epsilon, epsilon2) {
}

template <typename T>
rgb_to_rgbn_module<T>::~rgb_to_rgbn_module() {
}

template <typename T>
void rgb_to_rgbn_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize (iff necessary)
  // normalize RGB
  this->norm->fprop1(in, out); // local
}

template <typename T>
module_1_1<T>* rgb_to_rgbn_module<T>::copy(parameter<T> *p) {
  return new rgb_to_rgbn_module<T>(this->normker, this->mirror,
                                          this->norm_mode, this->globnorm);
}

// rgb_to_y_module /////////////////////////////////////////////////////////////

template <typename T>
rgb_to_y_module<T>::rgb_to_y_module(bool globnorm)
    : channels_module<T>(globnorm, "rgb_to_y") {
}

template <typename T>
rgb_to_y_module<T>::~rgb_to_y_module() {
}

template <typename T>
void rgb_to_y_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  idxdim d(in);
  d.setdim(0, 1);
  this->resize_output(in, out, &d); // resize (iff necessary)
  if (in.dim(0) != 3) {
    eblerror("expected 3 channels in dim 0 but found: " << in);
  } else {
    // RGB to YUV
    idx_eloop2(inx, in, T, outx, out, T) {
      idx_eloop2(inxx, inx, T, outxx, outx, T) {
        rgb_to_y_1D(inxx, outxx); }}
    // remove global mean and divide by stddev
    if (this->globnorm) image_global_normalization(out);
  }
}

template <typename T>
module_1_1<T>* rgb_to_y_module<T>::copy(parameter<T> *p) {
  return new rgb_to_y_module<T>(this->globnorm);
}

// rgb_to_yn_module ////////////////////////////////////////////////////////////

template <typename T>
rgb_to_yn_module<T>::rgb_to_yn_module(idxdim &normker, bool mirror,
				      t_norm norm_mode, bool globnorm,
				      double epsilon, double epsilon2)
    : channorm_module<T>(normker, mirror, norm_mode, "rgb_to_yn", 1,
				globnorm, epsilon, epsilon2) {
}

template <typename T>
rgb_to_yn_module<T>::~rgb_to_yn_module() {
}

template <typename T>
void rgb_to_yn_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  idxdim d(in);
  d.setdim(0, 1);
  this->resize_output(in, out, &d); // resize (iff necessary)
  if (in.dim(0) != 3) {
    // cerr << "warning: in rgb_to_yn, input is not 3-channel, "
    // 	   << "ignoring color." << endl;
    // convert Y to Yp
    this->norm->fprop1(in, out); // local
  } else {
    // RGB to Y
    idx_eloop2(inx, in, T, tmpx, this->tmp, T) {
      idx_eloop2(inxx, inx, T, tmpxx, tmpx, T) {
        rgb_to_y_1D(inxx, tmpxx); }}
    // convert Y to Yp
    this->norm->fprop1(this->tmp, out); // local
  }
}

template <typename T>
module_1_1<T>* rgb_to_yn_module<T>::copy(parameter<T> *p) {
  return new rgb_to_yn_module<T>(this->normker, this->mirror,
                                        this->norm_mode, this->globnorm);
}

////////////////////////////////////////////////////////////////
// y_to_yp_module

template <typename T>
y_to_yp_module<T>::y_to_yp_module(idxdim &normker, bool mirror,
                                         t_norm norm_mode, bool globnorm,
                                         double epsilon, double epsilon2)
    : channorm_module<T>(normker, mirror, norm_mode, "y_to_yp", 1,
				globnorm, epsilon, epsilon2) {
}

template <typename T>
y_to_yp_module<T>::~y_to_yp_module() {
}

template <typename T>
void y_to_yp_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  // lcn
  this->norm->fprop1(in, out);
}

template <typename T>
module_1_1<T>* y_to_yp_module<T>::copy(parameter<T> *p) {
  return new y_to_yp_module<T>(this->normker, this->mirror,
                                      this->norm_mode, this->globnorm);
}

////////////////////////////////////////////////////////////////
// bgr_to_ypuv_module

template <typename T>
bgr_to_ypuv_module<T>::
bgr_to_ypuv_module(idxdim &normker, bool mirror, t_norm norm_mode,
                   bool globnorm, double epsilon, double epsilon2)
    : channorm_module<T>(normker, mirror, norm_mode, "bgr_to_ypuv", 1,
				globnorm, epsilon, epsilon2) {
}

template <typename T>
bgr_to_ypuv_module<T>::~bgr_to_ypuv_module() {
}

template <typename T>
void bgr_to_ypuv_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  this->resize_output(in, out); // resize (iff necessary)
  idx<T> uv, yp, yuv;

  // BGR to YUV
  idx_eloop2(inx, in, T, outx, out, T) {
    idx_eloop2(inxx, inx, T, outxx, outx, T) {
      bgr_to_yuv_1D(inxx, outxx);
    }
  }
  // remove global mean and divide by stddev
  uv = out.narrow(0, 2, 1);
  if (this->globnorm) image_global_normalization(uv);
  // convert Y to Yp
  this->tmp = out.narrow(0, 1, 0);
  // lcn
  this->norm->fprop1(this->tmp, this->tmp);
}

template <typename T>
module_1_1<T>* bgr_to_ypuv_module<T>::copy(parameter<T> *p) {
  return new bgr_to_ypuv_module<T>(this->normker, this->mirror,
                                          this->norm_mode, this->globnorm);
}

////////////////////////////////////////////////////////////////
// bgr_to_yp_module

template <typename T>
bgr_to_yp_module<T>::bgr_to_yp_module(idxdim &normker, bool mirror,
                                             t_norm norm_mode, bool globnorm,
                                             double epsilon, double epsilon2)
    : channorm_module<T>(normker, mirror, norm_mode, "bgr_to_yp", 1,
			 globnorm, epsilon, epsilon2) {
}

template <typename T>
bgr_to_yp_module<T>::~bgr_to_yp_module() {
}

template <typename T>
void bgr_to_yp_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  idxdim d(in);
  d.setdim(0, 1);
  this->resize_output(in, out, d); // resize (iff necessary)
  // BGR to YUV
  idx_eloop2(inx, in, T, tmpx, this->tmp, T) {
    idx_eloop2(inxx, inx, T, tmpxx, tmpx, T) {
      bgr_to_y_1D(inxx, tmpxx); }}
  // convert Y to Yp
  this->norm->fprop1(this->tmp, out); // local
}

template <typename T>
module_1_1<T>* bgr_to_yp_module<T>::copy(parameter<T> *p) {
  return new bgr_to_yp_module<T>(this->normker, this->mirror,
				 this->norm_mode, this->globnorm);
}

////////////////////////////////////////////////////////////////
// rgb_to_hp_module

template <typename T>
rgb_to_hp_module<T>::rgb_to_hp_module(idxdim &normker, bool mirror,
                                             t_norm norm_mode, bool globnorm,
                                             double epsilon, double epsilon2)
    : channorm_module<T>(normker, mirror, norm_mode, "rgb_to_hp", 1,
			 globnorm, epsilon, epsilon2) {
}

template <typename T>
rgb_to_hp_module<T>::~rgb_to_hp_module() {
}

template <typename T>
void rgb_to_hp_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  idxdim d(in);
  d.setdim(0, 1);
  this->resize_output(in, out, &d); // resize (iff necessary)
  // RGB to YUV
  idx_eloop2(inx, in, T, tmpx, this->tmp, T) {
    idx_eloop2(inxx, inx, T, tmpxx, tmpx, T) {
      rgb_to_h_1D(inxx, tmpxx); }}
  // convert H to Hp
  this->norm->fprop1(this->tmp, out); // local
}

template <typename T>
module_1_1<T>* rgb_to_hp_module<T>::copy(parameter<T> *p) {
  return new rgb_to_hp_module<T>(this->normker, this->mirror,
                                        this->norm_mode, this->globnorm);
}

// resize_module ///////////////////////////////////////////////////////////////

template <typename T>
resize_module<T>::
resize_module(idxdim &size_, uint mode_, idxdim *dzpad_, bool pratio,
              const char *name_)
    : module_1_1<T>(name_),
      size(size_), tmp3(1,1,1), mode(mode_), input_mode(0), inrect(0, 0, 0, 0),
      inrect_set(false), outrect_set(false), dzpad(NULL), zpad(NULL),
      hjitter(0), wjitter(0), sjitter(1.0), rjitter(0.0),
      scale_hfactor(1.0), scale_wfactor(1.0),
      preserve_ratio(pratio), hratio(0), wratio(0), lastout(NULL),
      out_copy(NULL), display_min(-1), display_max(1), input_bboxes(1),
      original_bboxes(1), mod(NULL) {
  set_dimensions(size_.dim(0), size_.dim(1));
  if (dzpad_ && !dzpad_->empty()) {
    dzpad = new idxdim(*dzpad_);
    set_zpads(dzpad->dim(0), dzpad->dim(1));
  }
  if (!preserve_ratio) input_mode = 1; // do not preserve aspect ratio
}

template <typename T>
resize_module<T>::
resize_module(uint mode_, idxdim *dzpad_, bool pratio,
              const char *name_)
    : module_1_1<T>(name_), size(1,1), height(0), width(0), tmp3(1,1,1),
      mode(mode_), input_mode(0),
      inrect(0, 0, 0, 0), inrect_set(false), outrect_set(false), dzpad(NULL),
      zpad(NULL), hjitter(0), wjitter(0), sjitter(1.0), rjitter(0.0),
      scale_hfactor(1.0), scale_wfactor(1.0), preserve_ratio(pratio), hratio(0),
      wratio(0), lastout(NULL), out_copy(NULL), display_min(-1), display_max(1),
      input_bboxes(1), original_bboxes(1), mod(NULL) {
  if (dzpad_ && !dzpad_->empty()) {
    dzpad = new idxdim(*dzpad_);
    set_zpads(dzpad->dim(0), dzpad->dim(1));
  }
  if (!preserve_ratio) input_mode = 1; // do not preserve aspect ratio
}

template <typename T>
resize_module<T>::
resize_module(double hratio_, double wratio_, uint mode_,
              idxdim *dzpad_, bool pratio, const char *name_)
    : module_1_1<T>(name_), size(1,1), height(0), width(0), tmp3(1,1,1),
      mode(mode_), input_mode(0),
      inrect(0, 0, 0, 0), inrect_set(false), outrect_set(false), dzpad(NULL),
      zpad(NULL), hjitter(0), wjitter(0), sjitter(1.0), rjitter(0.0),
      scale_hfactor(1.0), scale_wfactor(1.0), preserve_ratio(pratio),
      hratio(hratio_), wratio(wratio_),
      lastout(NULL), out_copy(NULL), display_min(-1), display_max(1),
      input_bboxes(1), original_bboxes(1), mod(NULL) {
  if (dzpad_ && !dzpad_->empty()) {
    dzpad = new idxdim(*dzpad_);
    set_zpads(dzpad->dim(0), dzpad->dim(1));
  }
  if (preserve_ratio) input_mode = 2;
  else input_mode = 1;
}

template <typename T>
resize_module<T>::
resize_module(module_1_1<T> *mod_, idxdim &add, uint mode_,
              idxdim *dzpad_, const char *name_)
    : module_1_1<T>(name_), size(1,1), height(0), width(0), tmp3(1,1,1),
      mode(mode_), input_mode(0),
      inrect(0, 0, 0, 0), inrect_set(false), outrect_set(false), dzpad(NULL),
      zpad(NULL), hjitter(0), wjitter(0), sjitter(1.0), rjitter(0.0),
      scale_hfactor(1.0), scale_wfactor(1.0), preserve_ratio(false),
      hratio(0), wratio(0),
      lastout(NULL), out_copy(NULL), display_min(-1), display_max(1),
      input_bboxes(1), original_bboxes(1), mod(mod_), modadd(add) {
  if (dzpad_ && !dzpad_->empty()) {
    dzpad = new idxdim(*dzpad_);
    set_zpads(dzpad->dim(0), dzpad->dim(1));
  }
  if (!preserve_ratio) input_mode = 1;
}

template <typename T>
resize_module<T>::~resize_module() {
  if (zpad) delete zpad;
  if (dzpad) delete dzpad;
}

template <typename T>
void resize_module<T>::set_dimensions(intg height_, intg width_) {
  height = height_;
  width = width_;
  // if (dzpad) {
  //   height -= dzpad->dim(0) * 2;
  //   width -= dzpad->dim(1) * 2;
  // }
  size.setdim(0, height);
  size.setdim(1, width);
}

template <typename T>
void resize_module<T>::set_zpads(intg hpad, intg wpad) {
  // // reset height/width without current zpad
  // if (dzpad) {
  //   height += dzpad->dim(0) * 2;
  //   width += dzpad->dim(1) * 2;
  // }
  // update zpads and height/width
  // if (!dzpad)
  //   dzpad = new idxdim(hpad, wpad);
  // else {
  //   dzpad->setdim(0, hpad);
  //   dzpad->setdim(1, wpad);
  // }
  // height -= dzpad->dim(0) * 2;
  // width -= dzpad->dim(1) * 2;
  // size.setdim(0, height);
  // size.setdim(1, width);
  // update zpad module
  if (zpad) {
    delete zpad;
    zpad = NULL;
  }
  if (dzpad) {
    delete dzpad;
    dzpad = NULL;
  }
  dzpad = new idxdim(hpad, wpad);
  if (dzpad && (dzpad->dim(0) > 0 || dzpad->dim(1) > 0)) {
    zpad = new zpad_module<T>(dzpad->dim(0), dzpad->dim(1));
    EDEBUG(this->name() << ": set zpad module " << zpad
           << " to " << zpad->describe());
  }
}

template <typename T>
void resize_module<T>::set_zpad(idxdim &kernel) {
  if (kernel.empty()) return ;
  // update zpad module
  if (zpad) {
    delete zpad;
    zpad = NULL;
  }
  if (!dzpad) dzpad = new idxdim(kernel);
  else *dzpad = kernel;
  zpad = new zpad_module<T>(kernel);
}

template <typename T>
void resize_module<T>::set_zpad(midxdim &kernels) {
  if (kernels.empty()) return ;
  // update zpad module
  if (zpad) {
    delete zpad;
    zpad = NULL;
  }
  // if (!dzpad) dzpad = new idxdim(kernels);
  // else *dzpad = kernels;
  zpad = new zpad_module<T>(kernels);
}

template <typename T>
void resize_module<T>::set_jitter(int h, int w, float s, float r) {
  hjitter = h;
  wjitter = w;
  sjitter = s;
  rjitter = r;
}

template <typename T>
void resize_module<T>::set_scale_factor(double s) {
  scale_hfactor = s;
  scale_wfactor = s;
}

template <typename T>
void resize_module<T>::set_scale_factor(double sh, double sw) {
  scale_hfactor = sh;
  scale_wfactor = sw;
}

template <typename T>
void resize_module<T>::set_input_region(const rect<int> &inr) {
  inrect = inr;
  inrect_set = true;
}

template <typename T>
void resize_module<T>::set_output_region(const rect<int> &outr) {
  outrect = outr;
  // if (dzpad) {
  //   outrect.height -= dzpad->dim(0) * 2;
  //   outrect.width -= dzpad->dim(1) * 2;
  // }
  outrect_set = true;
}

template <typename T>
rect<int> resize_module<T>::get_original_bbox() {
  if (original_bboxes.size() == 0) eblerror("expected at least 1 box");
  return original_bboxes[0];
}

template <typename T>
rect<int> resize_module<T>::get_input_bbox() {
  return input_bbox;
}

template <typename T>
const std::vector<rect<int> >& resize_module<T>::get_input_bboxes() {
  input_bboxes.clear();
  input_bboxes.push_back(this->get_input_bbox());
  return input_bboxes;
}

template <typename T>
const std::vector<rect<int> >& resize_module<T>::
get_original_bboxes() {
  return original_bboxes;
}

template <typename T>
rect<int> resize_module<T>::compute_regions(idx<T> &in) {
  // set input region to entire image if no input region is given
  rect<int> r = rect<int>(0, 0, in.dim(1), in.dim(2));
  if (inrect_set) // set input region
    r = inrect;
  float hhratio = hratio, wwratio = wratio;
  if (mod) {
    idxdim d = mod->get_outdims();
    height = d.dim(1) + modadd.dim(0);
    width = d.dim(2) + modadd.dim(1);
    EDEBUG("resizing as " << mod->name() << "'s output " << d
           << " and adding " << modadd << ": height " << height
           << " width " << width);
  }
  if ((height != 0 && width != 0) && (hratio == 0 || wratio == 0)) {
    // set output region
    if (!outrect_set) outrect = rect<int>(0, 0, height, width);
    // find ratio between input box and output box
    hhratio = std::max(r.height / (float) outrect.height,
                       r.width / (float) outrect.width);
    wwratio = hhratio;
  } else {
	// no round function in vc++, so instead using floor(x+0.5) to simulate it.
    height = (intg) floor((r.height * hratio) + 0.5);
    width = (intg) floor((r.width * wratio) + 0.5);
    outrect = rect<int>(0, 0, height, width);
  }
  // apply scale jitter (keeping same center)
  if (sjitter != 1.0 || scale_hfactor != 1.0 || scale_wfactor != 1.0)
    r.scale_centered(sjitter * scale_hfactor, sjitter * scale_wfactor);
  // apply spatial jitter
  r.h0 -= (int) (hjitter * hhratio);
  r.w0 -= (int) (wjitter * wwratio);
  return r;
}

template <typename T>
void resize_module<T>::remember_regions(intg outh, intg outw, rect<int> &r) {
  // remember input box
  input_bbox = r;
  if (preserve_ratio) { // fit input ratio into target ratio
    double iratio = r.height / (double) r.width; // input ratio
    double tratio = outh / (double) outw; // target ratio
    if (tratio > iratio) input_bbox.scale_height(1 / tratio);
    else if (tratio < iratio) input_bbox.scale_width(1 / tratio);
  }
  //     double rh = outh / (double) original_bbox.height;
  //     double rw = outw / (double) (std::max)((int) 1, original_bbox.width);
  //     input_bbox.scale_centered(rh, rw);
}

template <typename T>
void resize_module<T>::set_display_range(T dmin, T dmax) {
  display_min = dmin;
  display_max = dmax;
}

template <typename T>
void resize_module<T>::get_display_range(T &dmin, T &dmax) {
  dmin = display_min;
  dmax = display_max;
}

// fprop methods /////////////////////////////////////////////////////////////

template <typename T>
void resize_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  // compute input/output regions
  rect<int> r = compute_regions(in);
  input_bboxes[0] = r;
  EDEBUG("resizing " << in << " to " << outrect << " with ROI " << r);
  rect<int> outr;
  // resize input while preserving aspect ratio
  tmp = in.shift_dim(0, 2); // resize functions expect channels in 3rd dim
  idx<T> resized;
  switch (mode) {
    case BILINEAR_RESIZE:
      if (input_mode == 2) { // use ratios
	resized = image_resize(tmp, hratio, wratio, input_mode, &r, &outr);
	EDEBUG(this->name() << ": resizing with ratios " << hratio
               << " and " << wratio);
      } else // use pixels
	resized = image_resize(tmp, (double) outrect.height,
			       (double) outrect.width, input_mode, &r, &outr);
      break ;
    case GAUSSIAN_RESIZE:
      resized = image_gaussian_resize(tmp, outrect.height,
				      outrect.width, input_mode, &r,&outr);
      break ;
    case MEAN_RESIZE:
      resized = image_mean_resize(tmp, outrect.height,
				  outrect.width, input_mode, &r, &outr);
      break ;
    default:
      eblerror("unknown resizing mode");
  }
  EDEBUG_MAT(this->name() << ": resized", resized);
  resized = resized.shift_dim(2, 0);
  // resize out to target dimensions if necessary
  if (((out.dim(1) != height) || (out.dim(2) != width)))
    out.resize(in.dim(0), height, width);
  idx_clear(out);
  resized = resized.shift_dim(0, 2);
  // apply rotation (around center of roi)
  if (rjitter != 0.0) {
    idx<T> r2 = idx_copy(resized); // make a contiguous copy
    resized = image_rotate(r2, rjitter, (int) outr.hcenter(),
                           (int) outr.wcenter());
  }
  EDEBUG_MAT(this->name() << ": resized", resized);
  // copy out region to output
  original_bboxes[0] = outr;
  tmp2 = image_region_to_rect(resized, outr, out.dim(1),
                              out.dim(2), &original_bboxes[0]);
  tmp2 = tmp2.shift_dim(2, 0);
  //idx_copy(tmp2, tmp);
  if (!zpad)
    idx_copy(tmp2, out);
  else { // zero padding
    original_bboxes[0].shift(dzpad->dim(0), dzpad->dim(1));
    tmp3.resize(tmp2.get_idxdim());
    idx_copy(tmp2, tmp3);
    zpad->fprop1(tmp3, out);
    EDEBUG("padded " << tmp3 << " with " << zpad->get_paddings() << " -> "
           << out);
  }
  remember_regions(out.dim(1), out.dim(2), r);
  EDEBUG_MAT(this->name() << ": out", out);

   // remember last output
  lout.x.clear();
  lout.add_x_new(out);
  lastout = &lout;
  copy_outputs(lout);
}

template <typename T>
void resize_module<T>::fprop(state<T> &in, midx<T> &out) {
  // expect 1D midx
  if (out.order() != 1)
    eblerror("expected a 1-dimensional midx but got order " << out.order());
  //     out.clear();
  //     out.resize(1);
  //     if (out.dim(0) != 1)
  //       eblerror("expected a 1-element midx but got dimension " << out.dim(0));
  idxdim d(in.get_idxdim());
  d.setdims(1);
  idx<T> tmp(d);
  // fprop
  this->fprop1(in, tmp);
  out.mset(tmp, 0);
}

template <typename T>
void resize_module<T>::bprop(state<T> &in, state<T> &out){
}

template <typename T>
void resize_module<T>::bbprop(state<T> &in, state<T> &out){
}

template <typename T>
module_1_1<T>* resize_module<T>::copy(parameter<T> *p) {
  return new resize_module(size, mode, dzpad);
}

template <typename T>
std::string resize_module<T>::describe() {
  std::string desc;
  desc << "resize module " << this->name() << ", resizing with method "
       << mode;
  if (input_mode == 1 || input_mode == 2) // using ratios
    desc << " with height ratio " << hratio << " and width ratio " << wratio;
  else
    desc << " to " << height << "x" << width;
  desc << " while "
       << (preserve_ratio ? "" : "not ") << "preserving aspect ratio";
  if (zpad && dzpad)
    desc << ", with zpad " << *dzpad;
  if (scale_hfactor != 1.0 || scale_wfactor != 1.0)
    desc << ", scaling input box by " << scale_hfactor << " x "
         << scale_wfactor;
  return desc;
}

template <typename T>
state<T>* resize_module<T>::last_output() {
  return lastout;
}

template <typename T>
void resize_module<T>::set_output_copy(state<T> &out) {
  out_copy = &out;
}

template <typename T>
mfidxdim resize_module<T>::fprop_size(mfidxdim &isize) {
  mfidxdim osize;
  EDEBUG(this->name() << ": " << isize << " f-> ...");
  if (zpad) osize = zpad->fprop_size(isize);
  for (uint i = 0; i < isize.size(); ++i) {
    if (isize.exists(i)) {
      fidxdim d(1, hratio, wratio);
      fidxdim o = isize[i] * d;
      osize.push_back_new(o);
    }
    else osize.push_back_empty();
  }
  EDEBUG(this->name() << ": " << isize << " f-> " << osize);
  return osize;
}

template <typename T>
fidxdim resize_module<T>::bprop1_size(const fidxdim &osize) {
  msize.clear();
  msize.push_back(osize);
  return osize;
}

template <typename T>
mfidxdim resize_module<T>::bprop_size(mfidxdim &osize) {
  msize = osize;
  // zpad is already applied via the original_bboxes offset below
  //if (zpad) osize = zpad->bprop_size(osize);
  mfidxdim isize;
  // compute inverse scaling factors
  fidxdim d(1, 1, 1);
  if (original_bboxes[0].height != 0
      && original_bboxes[0].width != 0) {
    d = fidxdim(1, input_bboxes[0].height /(float)original_bboxes[0].height,
                input_bboxes[0].width / (float) original_bboxes[0].width);
  }
  EDEBUG(this->name() << ": inverse scaling factors: " << d
         << " original_bboxes: " << original_bboxes
         << " input_bboxes: " << input_bboxes);
  // apply factors to all outputs
  for (uint i = 0; i < osize.size(); ++i) {
    if (osize.exists(i)) {
      fidxdim o = osize[i];
      o.setoffset(1, o.offset(1) - original_bboxes[0].h0);
      o.setoffset(2, o.offset(2) - original_bboxes[0].w0);
      o = o * d;
      isize.push_back_new(o);
    } else isize.push_back_empty();
  }
  EDEBUG(this->name() << ": " << osize << " b-> " << isize);
  return isize;
}

template <typename T>
mfidxdim resize_module<T>::get_msize() {
  return msize;
}

template <typename T>
uint resize_module<T>::nlayers() {
  return 1;
}

template <typename T>
void resize_module<T>::copy_outputs(state<T> &out) {
  // copy output to another copy
  if (out_copy) {
    out_copy->resize_as(out);
    out_copy->deep_copy(out);
  }
}

// resizepp_module /////////////////////////////////////////////////////////////

template <typename T>
resizepp_module<T>::
resizepp_module(idxdim &size_, uint mode_, module_1_1<T> *pp_,
                bool own_pp_, idxdim *dzpad_, bool pratio, const char *name_)
    : resize_module<T>(size_, mode_, dzpad_, pratio, name_),
      pp(pp_), own_pp(own_pp_) {
  this->set_name(name_);
}

template <typename T>
resizepp_module<T>::
resizepp_module(uint mode_, module_1_1<T> *pp_,
                bool own_pp_, idxdim *dzpad_, bool pratio, const char *name_)
    : resize_module<T>(mode_, dzpad_, pratio, name_),
      pp(pp_), own_pp(own_pp_) {
  this->set_name(name_);
}

template <typename T>
resizepp_module<T>::
resizepp_module(double hratio_, double wratio_, uint mode_,
                module_1_1<T> *pp_,
                bool own_pp_, idxdim *dzpad_, bool pratio, const char *name_)
    : resize_module<T>(hratio_, wratio_, mode_, dzpad_, pratio, name_),
      pp(pp_), own_pp(own_pp_) {
  this->set_name(name_);
}

template <typename T>
resizepp_module<T>::~resizepp_module() {
  if (pp && own_pp) delete pp;
}

template <typename T>
void resizepp_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  // compute input/output regions
  rect<int> r = this->compute_regions(in);
  input_bboxes[0] = r;
  EDEBUG("resizing " << in << " to " << outrect << " with ROI " << r);
  rect<int> outr;
  // resize input while preserving aspect ratio
  tmp = in.shift_dim(0, 2); // resize functions expect channels in 3rd dim
  idx<T> resized;
  switch (mode) {
    case BILINEAR_RESIZE:
      if (input_mode == 2) { // use ratios
	resized = image_resize(tmp, hratio, wratio, input_mode, &r, &outr);
	EDEBUG(this->name() << ": resizing with ratios " << hratio
               << " and " << wratio);
      } else // use pixels
	resized = image_resize(tmp, (double) outrect.height,
			       (double) outrect.width, input_mode, &r, &outr);
      break ;
    case GAUSSIAN_RESIZE:
      resized = image_gaussian_resize(tmp, outrect.height,
				      outrect.width, input_mode, &r,&outr);
      break ;
    case MEAN_RESIZE:
      resized = image_mean_resize(tmp, outrect.height,
				  outrect.width, input_mode, &r, &outr);
      break ;
    default:
      eblerror("unknown resizing mode");
  }
  EDEBUG_MAT(this->name() << ": resized", resized);
  resized = resized.shift_dim(2, 0);
  // call preprocessing
  if (pp) { // no preprocessing if NULL module
    EDEBUG_MAT(pp->name() << ": in", resized);
    inpp = resized;
    pp->fprop1(resized, outpp);
    resized = outpp;
    EDEBUG_MAT(pp->name() << ": out", resized);
  }
  // resize out to target dimensions if necessary
  if (((out.dim(1) != height) || (out.dim(2) != width)) && !pp)
    out.resize(in.dim(0), height, width);
  else if (((out.dim(1) != height) || (out.dim(2) != width)
            || (out.dim(0) != outpp.dim(0))) && pp)
    out.resize(outpp.dim(0), height, width);
  idx_clear(out);
  resized = resized.shift_dim(0, 2);
  // apply rotation (around center of roi)
  if (rjitter != 0.0) {
    idx<T> r2 = idx_copy(resized); // make a contiguous copy
    resized = image_rotate(r2, rjitter, (int) outr.hcenter(),
                           (int) outr.wcenter());
  }
  // copy out region to output
  original_bboxes[0] = outr;
  tmp2 = image_region_to_rect(resized, outr, out.dim(1),
                              out.dim(2), &original_bboxes[0]);
  tmp2 = tmp2.shift_dim(2, 0);
  EDEBUG_MAT(this->name() << ": resized", resized);
  this->remember_regions(out.dim(1), out.dim(2), r);
  //idx_copy(tmp2, tmp);
  if (!zpad)
    idx_copy(tmp2, out);
  else { // zero padding
    original_bboxes[0].shift(dzpad->dim(0), dzpad->dim(1));
    tmp3.resize(tmp2.get_idxdim());
    idx_copy(tmp2, tmp3);
    zpad->fprop1(tmp3, out);
    EDEBUG("padded " << tmp3 << " with " << zpad->get_paddings() << " -> "
           << out);
  }
  EDEBUG("resized " << in << " to " << out);
  EDEBUG_MAT(this->name() << ": out", out);

  // remember last output
  lout.x.clear();
  lout.add_x_new(out);
  lastout = &lout;
  this->copy_outputs(lout);
}

template <typename T>
void resizepp_module<T>::fprop(state<T> &in, midx<T> &out) {
  // expect 1D midx
  if (out.order() != 1)
    eblerror("expected a 1-dimensional midx but got order " << out.order());
  //     out.clear();
  //     out.resize(1);
  //     if (out.dim(0) != 1)
  //       eblerror("expected a 1-element midx but got dimension " << out.dim(0));
  idxdim d(in.get_idxdim());
  d.setdims(1);
  idx<T> tmp(d);
  // fprop
  this->fprop1(in, tmp);
  out.mset(tmp, 0);
}

template <typename T>
module_1_1<T>* resizepp_module<T>::copy(parameter<T> *p) {
  module_1_1<T> *newpp = NULL;
  if (pp) newpp = pp->copy(p);
  return new resizepp_module(size, mode, newpp, true, dzpad);
}

template <typename T>
std::string resizepp_module<T>::describe() {
  std::string desc;
  desc << "resizepp module " << this->name() << ", resizing with method "
       << mode;
  if (input_mode == 1 || input_mode == 2) // using ratios
    desc << " with height ratio " << hratio << " and width ratio " << wratio;
  else
    desc << " to " << height << "x" << width;
  desc << " while "
       << (preserve_ratio ? "" : "not ") << "preserving aspect ratio";
  if (zpad && dzpad)
    desc << ", with zpad " << *dzpad;
  desc << ", pp: ";
  if (pp)
    desc << pp->describe();
  else
    desc << "none";
  if (scale_hfactor != 1.0 || scale_wfactor != 1.0)
    desc << ", scaling input box by " << scale_hfactor << " x "
         << scale_wfactor;
  return desc;
}

// fovea_module ////////////////////////////////////////////////////////////////

template <typename T>
fovea_module<T>::
fovea_module(std::vector<double> &fovea_, midxdim &fovea_scales_size_,
             idxdim &size_, bool boxscale_, uint mode_,
             module_1_1<T> *pp_, bool own_pp_,
             idxdim *dzpad_, const char *name_)
    : resizepp_module<T>(size_, mode_, pp_, own_pp_, dzpad_), fovea(fovea_),
      fovea_scales_size(fovea_scales_size_), boxscale(boxscale_) {
}

template <typename T>
fovea_module<T>::
fovea_module(std::vector<double> &fovea_, bool boxscale_, uint mode_,
             module_1_1<T> *pp_, bool own_pp_, idxdim *dzpad_,
             const char *name_)
    : resizepp_module<T>(mode_, pp_, own_pp_, dzpad_), fovea(fovea_),
      boxscale(boxscale_) {
}

template <typename T>
fovea_module<T>::~fovea_module() {}

template <typename T>
void fovea_module<T>::fprop(state<T> &in, state<T> &out) {
  obboxes.clear();
  ibboxes.clear();
  // check that fovea is defined
  if (fovea.size() == 0)
    eblerror("cannot process a fovea with empty scales");
  // remember target size
  idxdim s = this->size;
  // resize if necessary and set appropriate number of scales
  out.resize_forward_orders(in, 1, fovea.size());
  rect<int> obbox, ibbox;
  // fprop all scales
  for (uint f = 0; f < fovea.size(); ++f) {
    idx<T> &o = out.x[f];
    if (boxscale) // box scaling mode
      this->set_scale_factor(fovea[f]);
    else { // image scaling mode
      idxdim p = s * (1 / (float) fovea[f]);
      this->set_dimensions(p.dim(0), p.dim(1));
      EDEBUG("applying fovea factor " << 1/fovea[f] << " to " << in);
    }
    this->set_dimensions(fovea_scales_size[f].dim(0),
                         fovea_scales_size[f].dim(1));
    resizepp_module<T>::fprop1(in, o);
    if (f == 0) { // remember first original bbox
      obbox = this->get_original_bbox();
      ibbox = this->get_input_bbox();
    }
    // remember all boxes in original input
    obboxes.push_back(this->get_original_bbox());
    ibboxes.push_back(this->get_input_bbox());
  }
  this->original_bboxes[0] = obbox; // use 1st scale as reference
  this->input_bbox = ibbox; // use 1st scale as reference
  this->lastout = &out;
}

template <typename T>
void fovea_module<T>::fprop(state<T> &in, midx<T> &out) {
  // expecting a 1D midx
  if (out.order() != 1)
    eblerror("expected a 1-dimensional midx but got order " << out.order());
  if ((uint) out.dim(0) != fovea.size())
    out.resize(fovea.size());
  state<T> mout;
  this->fprop1(in, mout);
  // convert mout (which is a vector) into out (midx)
  for (uint i = 0; i < mout.x.size(); ++i)
    out.mset(mout.x[i], i);
}

template <typename T>
void fovea_module<T>::bprop(state<T> &in, state<T> &out) {
}

template <typename T>
void fovea_module<T>::bbprop(state<T> &in, state<T> &out) {
}

template <typename T>
mfidxdim fovea_module<T>::bprop_size(mfidxdim &osize) {
  this->msize = osize;
  if (osize.size() <= 0)
    eblerror("expected at least 1 element but found " << osize.size());
  return osize;
}

template <typename T>
std::string fovea_module<T>::describe() {
  std::string desc = "fovea ";
  desc << resizepp_module<T>::describe()
       << ", fovea: " << fovea << ", resizing ";
  if (boxscale)
    desc << "box with fovea factors.";
  else
    desc << "image with inverse fovea factors.";
  return desc;
}

template <typename T>
const std::vector<rect<int> >& fovea_module<T>::get_original_bboxes() {
  return obboxes;
}

template <typename T>
const std::vector<rect<int> >& fovea_module<T>::get_input_bboxes() {
  return ibboxes;
}

template <typename T>
uint fovea_module<T>::nlayers() {
  return fovea.size();
}

// mschan_module ///////////////////////////////////////////////////////////////

template <typename T>
mschan_module<T>::mschan_module(uint nstates_, const char *name)
    : module_1_1<T>(name), nstates(nstates_) {
}

template <typename T>
mschan_module<T>::~mschan_module() {
}

template <typename T>
void mschan_module<T>::fprop(state<T> &in, state<T> &out) {
  uint nchans = in.dim(0) / nstates;
  idxdim d = in.get_idxdim();
  d.setdim(0, nchans);
  // resize out if necessary
  out.resize(d, nstates);
  // copy each channel into its state
  // TODO: handle multiple channels per state, using fovea size
  for (uint f = 0; f < in.dim(0); ++f) {
    idx<T> &o = out.x[f];
    idx<T> inx = in.narrow(0, nchans, f * nchans);
    idx_copy(inx, o);
  }
}

template <typename T>
void mschan_module<T>::bprop(state<T> &in, state<T> &out) {
}

template <typename T>
void mschan_module<T>::bbprop(state<T> &in, state<T> &out) {
}

// laplacian_pyramid_module ////////////////////////////////////////////////////

template <typename T>
laplacian_pyramid_module<T>::
laplacian_pyramid_module(uint nscales_, midxdim &sizes_, uint mode_,
                         module_1_1<T> *pp_, bool own_pp_,
                         idxdim *dzpad_, bool gnorm_, bool lnorm_,
                         bool lnorm2_, bool cnorm_, bool cacross, bool keep,
                         const char *name_)
    : resizepp_module<T>(sizes_[0], mode_, pp_, own_pp_, dzpad_),
      nscales(nscales_), sizes(sizes_), global_norm(gnorm_),
      local_norm(lnorm_), local_norm2(lnorm2_), color_lnorm(cnorm_),
      cnorm_across(cacross), keep_aspect_ratio(keep),
      tmp(1), outs(nscales) {
  kerdims.push_back(idxdim(5, 5));
  init();
}

template <typename T>
laplacian_pyramid_module<T>::
laplacian_pyramid_module(uint nscales_, idxdim &kerd, midxdim &sizes_,
                         uint mode_, module_1_1<T> *pp_, bool own_pp_,
                         idxdim *dzpad_, bool gnorm_, bool lnorm_,
                         bool lnorm2_, bool cnorm_, bool cacross, bool keep,
                         const char *name_)
    : resizepp_module<T>(sizes_[0], mode_, pp_, own_pp_, dzpad_),
      nscales(nscales_), sizes(sizes_), global_norm(gnorm_),
      local_norm(lnorm_), local_norm2(lnorm2_), color_lnorm(cnorm_),
      cnorm_across(cacross), keep_aspect_ratio(keep), tmp(1),
      outs(nscales) {
  kerdims.push_back(kerd);
  init();
}

template <typename T>
laplacian_pyramid_module<T>::
laplacian_pyramid_module(uint nscales_, midxdim &kerdims_,
                         midxdim &sizes_,
                         uint mode_, module_1_1<T> *pp_, bool own_pp_,
                         idxdim *dzpad_, bool gnorm_, bool lnorm_,
                         bool lnorm2_, bool cnorm_, bool cacross, bool keep,
                         const char *name_)
    : resizepp_module<T>(sizes_[0], mode_, pp_, own_pp_, dzpad_),
      nscales(nscales_), sizes(sizes_), kerdims(kerdims_), global_norm(gnorm_),
      local_norm(lnorm_), local_norm2(lnorm2_), color_lnorm(cnorm_),
      cnorm_across(cacross), keep_aspect_ratio(keep), tmp(1),
      outs(nscales) {
  if (kerdims.size() < nscales)
    eblerror("expected at least same number of scales and filter dims but "
             << "got " << nscales << " and " << kerdims);
  init();
}

template <typename T>
laplacian_pyramid_module<T>::
laplacian_pyramid_module(uint nscales_, uint mode_, module_1_1<T> *pp_,
                         bool own_pp_, idxdim *dzpad_, bool gnorm_,
                         bool lnorm_, bool lnorm2_, bool cnorm_, bool cacross,
                         bool keep, const char *name_)
    : resizepp_module<T>(mode_, pp_, own_pp_, dzpad_), nscales(nscales_),
      global_norm(gnorm_), local_norm(lnorm_), local_norm2(lnorm2_),
      color_lnorm(cnorm_), cnorm_across(cacross), keep_aspect_ratio(keep),
      tmp(1), outs(nscales) {
  kerdims.push_back(idxdim(5, 5));
  init();
}

template <typename T>
laplacian_pyramid_module<T>::~laplacian_pyramid_module() {
  for (uint i = 0; i < norms.size(); ++i)  delete norms[i];
  for (uint i = 0; i < cnorms.size(); ++i) delete cnorms[i];
  for (uint i = 0; i < pads.size(); ++i)   delete pads[i];
}

template <typename T>
void laplacian_pyramid_module<T>::init() {
  burt_filtering_only = false;
  use_pad = true;
  mirror = true;
  scalings.push_back(2.0); // default scaling
  // limit sizes to order 2
  for (uint i = 0; i < sizes.size(); ++i) {
    while (sizes[i].order() > 2)
      sizes[i].remove_dim(sizes[i].order() - 1);
  }
  this->size = sizes[0];
  // replicate filter sizes if just 1
  if (kerdims.size() == 1 && nscales > 1)
    for (uint i = 1; i < nscales; ++i) kerdims.push_back(kerdims[0]);
  // remove extra dimensions
  kerdims.erase(kerdims.begin() + nscales, kerdims.end());
  // 5x5 gaussian kernel for subsampling
  burt = create_burt_adelson_kernel<T>();
  // loop on all scale kernel dimensions
  bool threshold = false;
  for (uint i = 0; i < kerdims.size(); ++i) {
    idxdim d = kerdims[i];
    // high frequency kernels
    filters.push_back(create_gaussian_kernel<T>(d));
    // normalizations
    if (local_norm || local_norm2) {
      // grayscale normalizations
      layers<T> *norm = new layers<T>();
      norms.push_back(norm);
      if (local_norm)
        norm->add_module(new divisive_norm_module<T>
                         (d, 1, mirror, threshold, NULL, "", true));
      if (local_norm2)
        norm->add_module(new contrast_norm_module<T>
                         (d, 1, mirror, threshold, true, NULL, "", true));
      // color normalizations
      if (color_lnorm) {
        layers<T> *cnorm = new layers<T>();
        cnorms.push_back(cnorm);
        if (local_norm)
          cnorm->add_module(new contrast_norm_module<T>
                            (d, 2, mirror, threshold, true, NULL, "",
                             cnorm_across));
        if (local_norm2)
          cnorm->add_module(new contrast_norm_module<T>
                            (d, 2, mirror, threshold, true, NULL, "",
                             cnorm_across));
      }
    }
    // padding
    if (mirror) // switch between zero and mirror padding
      pads.push_back(new mirrorpad_module<T>
                     ((d.dim(0) - 1)/2, (d.dim(1) - 1)/2));
    else
      pads.push_back(new zpad_module<T>(d));
  }
  // resize boxes
  original_bboxes.resize(1);
  input_bboxes.resize(nlayers());
}

template <typename T>
void laplacian_pyramid_module<T>::fprop(state<T> &in, state<T> &out) {
  fprop(in, outs);
  out.x.clear();
  for (uint i = 0; i < outs.dim(0); ++i)
    out.x.push_back(new idx<T>(outs.mget(i)));
  // adding zero borders if defined
  if (zpad) {
    zpad->fprop(out, zpad_out);
    out.x.clear();
    out.x.push_back_new(zpad_out);
  }
  this->copy_outputs(out);
}

template <typename T>
void laplacian_pyramid_module<T>::bprop(state<T> &in, state<T> &out) {
}

template <typename T>
void laplacian_pyramid_module<T>::bbprop(state<T> &in, state<T> &out) {
}

template <typename T>
mfidxdim laplacian_pyramid_module<T>::fprop_size(mfidxdim &isize) {
  EDEBUG(this->name() << ": " << isize << " f-> ...");
  mfidxdim osize;
  fidxdim i0 = isize[0];
  for (uint i = 0; i < outs.dim(0); ++i)
    osize.push_back_new(i0);
  this->ninputs = isize.size();
  this->noutputs = osize.size();
  if (zpad) osize = zpad->fprop_size(osize);
  EDEBUG(this->name() << ": " << isize << " f-> " << osize);
  return osize;
}

template <typename T>
mfidxdim laplacian_pyramid_module<T>::bprop_size(mfidxdim &osize_) {
  mfidxdim osize = osize_;
  this->msize = osize;
  if (zpad) osize = zpad->bprop_size(osize);
  // return resizepp_module<T>::bprop_size(osize);
  mfidxdim isize;
  if (osize.size() != input_bboxes.size())
    eblerror("expected same size in osize (" << osize
             << ") and input_bboxes (" << input_bboxes << ")");
  // scale output given the in/out original ratios
  //    uint fact = 1;
  for (uint i = 0; i < osize.size(); ++i) {
    if (osize.exists(i)) {
      fidxdim d(1, original_bboxes[0].height / (float) input_bboxes[i].height,
                original_bboxes[0].width / (float) input_bboxes[i].width);
      //	fidxdim d(1, fact, fact);
      fidxdim o = osize[i] * d;
      isize.push_back_new(o);
    }
    else isize.push_back_empty();
    //fact *= 2;
  }
  EDEBUG(this->name() << ": " << osize << " b-> " << isize);
  return isize;
}

template <typename T>
std::string laplacian_pyramid_module<T>::describe() {
  std::string desc = "Laplacian pyramid ";
  desc << resizepp_module<T>::describe()
       << ", with " << nscales << " scales and " << (int) scalings.size()
       << " pyramids";
  return desc;
}

template <typename T>
uint laplacian_pyramid_module<T>::nlayers() {
  return nscales * scalings.size();
}

template <typename T>
void laplacian_pyramid_module<T>::set_scalings(std::vector<float> &s) {
  scalings = s;
  input_bboxes.resize(nlayers());
}

// protected methods /////////////////////////////////////////////////////////

template <typename T>
void laplacian_pyramid_module<T>::fprop(state<T> &in, midx<T> &out) {
  // expecting a 1D midx
  if (out.order() != 1)
    eblerror("expected a 1-dimensional midx but got order " << out.order());
  if (out.dim(0) != nlayers()) out.resize(nlayers());
  if (tmp.dim(0) != nlayers()) tmp.resize(nlayers());
  // only accept 2D images or 3D with channel dim to 0.
  if ((in.order() != 2) && (in.order() != 3))
    eblerror("unexpected image format" << in);
  // allocate temporary buffers if not allocated yet
  if (blurred.order() != in.order()) {
    idxdim d(in);
    d.setdims(1);
    blurred = idx<T>(d);
    blurred_high = idx<T>(d);
    padded = state<T>(d);
    outpp = state<T>(d);
  }

  // loop on pyramids
  for (uint pyr = 0; pyr < scalings.size(); ++pyr) {
    LOCAL_TIMING_START(); // profiling
    // reset scale counter
    iscale = 0;
    // compute input/output regions
    //out.clear();
    rect<int> inr = this->compute_regions(in);
    rect<int> rr = inr;
    rect<int> outr; //, &oregion = this->original_bbox;
    // remember image region of the first scale
    if (pyr == 0) original_bboxes[0] = inr;
    // start from twice the target size
    idxdim tgt = this->size * (scalings[pyr] / 2.0);
    idxdim tgt0 = tgt * 2;
    idxdim ind(inr.height, inr.width);
    // shift dim 0 to 2 and copy to have continuous data
    idx<T> im = in.shift_dim(0, 2);
    // if input is big enough start with tgt0
    if (false) {//ind >= tgt0) { //input region is big enough to start at tgt0
      // resize bilinearly to tgt0
      resize(im, tgt0, inr, outr);
      // blur im into blurred
      blur(burt, im, blurred, inr);
      // subsample
      subsample(blurred, im, inr);
    } else { // start from tgt directly (even if upsampling)
      resize(im, tgt, inr, outr);
      // no blur or subsampling since we are already at tgt
    }

    // // TMP
    // zpad_module<T> zp(20, 20, 20, 20);
    // idxdim di = im.get_idxdim(), dp(di.dim(0), di.dim(1) + 40, di.dim(2) + 40);
    // state<T> zpi(di), zpo(dp);
    // zpi = im;
    // zp.fprop(zpi, zpo);
    // im = zpo;
    // // inr = this->compute_regions(zpo);
    // // rr = inr;

    LOCAL_TIMING_REPORT("laplacian_pyramid, initial resizing");
    // loop and produce each scale
    for (uint scale = 0; scale < nscales; ++scale, ++iscale) {
      // remember input size
      input_bboxes[scale + nscales * pyr] = inr;
      // blur im into blurred
      LOCAL_TIMING2_START();
      blur(burt, im, blurred, inr);
      // use burt filtered to obtain high frequencies
      if (burt_filtering_only)
        blurred_high = blurred;
      else // or use another filter (more computation)
        blur(filters[iscale], im, blurred_high, inr);
      LOCAL_TIMING2_REPORT("blurring");
      // compute high passed target
      idx<T> high = highpass(im, blurred_high, tgt, inr, scale == 0);
      LOCAL_TIMING2_REPORT("highpass");
      // assign high pass to output
      out.mset(high, scale + nscales * pyr);
      // subsample only if more scales to come
      if (scale + 1 != nscales)
        subsample(blurred, im, inr);
      LOCAL_TIMING2_REPORT("subsampling");
      // subsample target dimensions
      if (scale + 1 < sizes.size()) // set target manually
        tgt = sizes[scale + 1];
      else // simply reduce target by at each new scale
        tgt = tgt * .5;
      LOCAL_TIMING_REPORT("laplacian_pyramid, scale " << scale);
    }
    this->remember_regions(this->height, this->width, rr);
  }
  EDEBUG("laplacian outputs: " << out.str());
  if (!this->silent)
    eblprint( "laplacian outputs: " << out.str() << std::endl);
  TIMING2(this->name());
}

template <typename T>
void laplacian_pyramid_module<T>::
resize(idx<T> &im, idxdim &tgt, rect<int> &inr, rect<int> &outr) {
  // resize bilinearly to tgt
  im = image_resize(im, (double) tgt.dim(0), (double) tgt.dim(1),
                    keep_aspect_ratio ? 0 : 1, &inr, &outr);
  inr = outr;
  im = im.shift_dim(2, 0);
  // call preprocessing
  idx<T> inn = im;
  if (this->pp) { // no preprocessing if NULL module
    // disable the channel pp's global normalization (we'll do our own)
    //      ((channels_module<T>*)this->pp)->globnorm = false;
    inpp = im;
    this->pp->fprop(inpp, outpp);
    im = outpp;
  }
  // apply rotation (around center of roi)
  if (this->rjitter != 0.0) {
    im = im.shift_dim(0, 2);
    idx<T> r2 = idx_copy(im); // make a contiguous copy
    im = image_rotate(r2, this->rjitter, (int) inr.hcenter(),
                      (int) inr.wcenter());
    im = im.shift_dim(2, 0);
    inr.rotate(this->rjitter);
  }
  // make a continuous version of im
  idx<T> cont(im.get_idxdim());
  idx_copy(im, cont);
  im = cont;
}

template <typename T>
void laplacian_pyramid_module<T>::
blur(idx<T> &filter, idx<T> &in, idx<T> &out, rect<int> &roi) {
  if (in.get_idxdim() != out.get_idxdim())
    out.resize(in.get_idxdim());
  idx<T> i = in, o = out;
  state<T> ts;
  ts = i;
  // normalize gobally
  normalize_intensity_globally(in);
  // pad input for convolution
  if (use_pad) {
    pads[iscale]->set_kernel(filter.get_idxdim());
    pads[iscale]->fprop(ts, padded);
    i = padded;
  } else { // narrow out to receive non-padded filtering
    o = o.narrow(1, o.dim(1) - filter.dim(0) + 1, (filter.dim(0) - 1) / 2);
    o = o.narrow(2, o.dim(2) - filter.dim(1) + 1, (filter.dim(1) - 1) / 2);
  }
  // convolve with filter for high frequency
  if (in.order() == 3) { // loop over each channel
    idx_bloop2(tin, i, T, tout, o, T) {
      idx<T> uin = tin.unfold(0, filter.dim(0), 1);
      uin = uin.unfold(1, filter.dim(1), 1);
      idx_m4dotm2(uin, filter, tout);
    }
  } else {
    idx<T> uin = i.unfold(0, filter.dim(0), 1);
    uin = uin.unfold(1, filter.dim(1), 1);
    idx_m4dotm2(i, filter, o); // just one channel
  }
  // reduce ROI if not padded to account for filter borders
  if (!use_pad) {
    roi.h0 += (filter.dim(0) - 1) / 2;
    roi.w0 += (filter.dim(1) - 1) / 2;
    roi.height -= filter.dim(0) - 1;
    roi.width -= filter.dim(1) - 1;
  }
}

template <typename T>
idx<T> laplacian_pyramid_module<T>::
highpass(idx<T> &in, idx<T> &blurred, idxdim &tgt, rect<int> &inr,
         bool first) {
  // TODO: this assumes brightness in channel 1 and color in remaining
  idx<T> dtgt(in.dim(0), tgt.dim(0), tgt.dim(1));
  idx<T> high(in.get_idxdim());
  idx<T> tmpin = in.narrow(0, 1, 0);
  idx<T> tmpbl = blurred.narrow(0, 1, 0);
  idx<T> tmphi = high.narrow(0, 1, 0);
  state<T> ti, to;

  // grayscale ///////////////////////////////////////////////////////////////
  // remove low-frequencies and normalize locally (1st layer only)
  if (norms.size() > 0) {
    // resize temporary buffer if necessary
    if (tmpin.get_idxdim() != high0.get_idxdim()) {
      if (tmpin.order() != high0.order()) high0 = idx<T>(tmpin.get_idxdim());
      else high0.resize(tmpin.get_idxdim());
    }
    // just subtract blurred version in the 1st layer
    idx_sub(tmpin, tmpbl, high0);
    // apply grayscale normalizations
    ti = high0;
    to = tmphi;
    norms[iscale]->fprop(ti, to);
  } else // just subtract blurred version in the 1st layer
    idx_sub(tmpin, tmpbl, tmphi);

  // color ///////////////////////////////////////////////////////////////////
  if (in.dim(0) > 1) {
    tmpin = in.narrow(0, in.dim(0) - 1, 1);
    tmphi = high.narrow(0, high.dim(0) - 1, 1);
    if (cnorms.size() > 0) { // color local normalization
      ti = tmpin;
      to = tmphi;
      cnorms[iscale]->fprop(ti, to);
    } else // simply copy remaining layers (color)
      idx_copy(tmpin, tmphi);
  }
  // normalize globally
  if (global_norm) normalize_globally(high);//, inr);
  rect<int> outr;
  // paste at center
  return image_region_to_rect(high, inr, tgt.dim(0), tgt.dim(1), &outr, 1, 2);
}

template <typename T>
void laplacian_pyramid_module<T>::subsample(idx<T> &in, idx<T> &out,
                                            rect<int> &inr) {
  idxdim dsub(in);
  dsub.setdim(1, dsub.dim(1) / 2);
  dsub.setdim(2, dsub.dim(2) / 2);
  out.resize(dsub);
  idxlooper<T> tmpin(in, 2);
  idx_eloop1(sub, out, T) {
    idxlooper<T> tmpi(tmpin, 1);
    idx_eloop1(su, sub, T) {
      idx_copy(tmpi, su);
      tmpi.next();
      tmpi.next();
    }
    tmpin.next();
    tmpin.next();
  }
  // rescale input box
  inr = inr / 2;
}

template <typename T>
void laplacian_pyramid_module<T>::normalize_globally(idx<T> &in) {
  normalize_intensity_globally(in);
  normalize_color_globally(in);
}

template <typename T>
void laplacian_pyramid_module<T>::normalize_intensity_globally(idx<T> &in) {
  idx<T> intensity = in.narrow(0, 1, 0);
  normalize_globally2(intensity);
}

template <typename T>
void laplacian_pyramid_module<T>::normalize_color_globally(idx<T> &in) {
  idx<T> color = in.narrow(0, 2, 1);
  normalize_globally2(color);
}

template <typename T>
void laplacian_pyramid_module<T>::normalize_globally2(idx<T> &in) {
  //    if (!global_norm) return ;
  image_global_normalization(in);

  //     idx<T> r = in.narrow(1, roi.height, roi.h0);
  //     r = r.narrow(2, roi.width, roi.w0);
  //     idx<T> r2(r.get_idxdim());
  //     // get mean of roi
  //     T mean = idx_mean(r);
  //     idx_addc(r, (T)-mean, r2); // remove mean
  //     // get std deviation of roi
  // #ifdef __WINDOWS__
  //     T coeff = (T) sqrt((double) (idx_sumsqr(r2) / r2.nelements()));
  // #else
  //     T coeff = (T) sqrt((T) (idx_sumsqr(r2) / r2.nelements()));
  // #endif
  //     // apply to entire image
  //     idx_addc(in, (T)-mean, in); // remove mean
  //     if (coeff != 0) idx_dotc(in, 1 / coeff, in);
}

// jitter_module ///////////////////////////////////////////////////////////////

template <typename T>
jitter_module<T>::jitter_module(const char *name_)
  : module_1_1<T>(name_), zp(NULL) {
  // no deformation defaults
  th0 = 0; th1 = 0; tw0 = 0; tw1 = 0; // translation ranges
  deg0 = 0; deg1 = 0; // rotation range
  sh0 = 1; sh1 = 1; sw0 = 1; sw1 = 1; // scaling range
  shh0 = 0; shh1 = 0; shw0 = 0; shw1 = 0; // shear range
  elsz0 = 0; elsz1 = 0; elcoeff0 = 0; elcoeff1 = 0; // elastic ranges
}

template <typename T>
jitter_module<T>::~jitter_module() {
  if (zp) delete zp;
}

template <typename T>
void jitter_module<T>::set_translations(std::vector<int> &v) {
  if (v.size() != 4) eblerror("expected 4 elements in " << v);
  th0 = v[0]; th1 = v[1]; tw0 = v[2]; tw1 = v[3];
}

template <typename T>
void jitter_module<T>::set_rotations(std::vector<float> &v) {
  if (v.size() != 2) eblerror("expected 2 elements in " << v);
  deg0 = v[0]; deg1 = v[1];
}

template <typename T>
void jitter_module<T>::set_scalings(std::vector<float> &v) {
  if (v.size() != 4) eblerror("expected 4 elements in " << v);
  sh0 = v[0]; sh1 = v[1]; sw0 = v[2]; sw1 = v[3];
}

template <typename T>
void jitter_module<T>::set_shears(std::vector<float> &v) {
  if (v.size() != 4) eblerror("expected 4 elements in " << v);
  shh0 = v[0]; shh1 = v[1]; shw0 = v[2]; shw1 = v[3];
}

template <typename T>
void jitter_module<T>::set_elastics(std::vector<float> &v) {
  if (v.size() != 4) eblerror("expected 4 elements in " << v);
  elsz0 = (uint) v[0]; elsz1 = (uint) v[1];
  elcoeff0 = v[2]; elcoeff1 = v[3];
}

template <typename T>
void jitter_module<T>::set_padding(std::vector<uint> &p) {
  if (p.size() != 4) eblerror("expected 4 elements in " << p);
  zp = new zpad_module<T>(p[0], p[1], p[2], p[3]);
}

template <typename T>
void jitter_module<T>::fprop1(idx<T> &in, idx<T> &out) {
  idx<T> *i = &in;
  if (zp) {
    zp->fprop1(in, tmp);
    i = &tmp;
  }
  this->resize_output(*i, out);
  if (this->ignored1(in, out)) return ;
  // random deformations
  int th = (int) drand(th0, th1), tw = (int) drand(tw0, tw1); // translation
  float deg = drand(deg0, deg1); // rotation
  float sh = drand(sh0, sh1), sw = drand(sw0, sw1); // scale
  float shh = drand(shh0, shh1), shw = drand(shw0, shw1); // shear
  uint elsize = (uint) drand(elsz0, elsz1);
  float elc = elsize; // elastic
  // expect channels to be in dim 2,
  // TODO: allow specifying planar or interleaved in image_deformation
  // function to avoid this inefficient and dirty code
  idx<T> i2 = i->shift_dim(0, 2);
  idx<T> a(i2.get_idxdim()), b(i2.get_idxdim());
  idx_copy(i2, a);
  // deform
  image_deformation(a, b, th, tw, sh, sw, deg, shh, shw, elsize, elc);
  // TODO: same as above
  b = b.shift_dim(2, 0);
  idx_copy(b, out);
}

template <typename T>
void jitter_module<T>::bprop1(state<T> &in, state<T> &out) {
}

template <typename T>
void jitter_module<T>::bbprop1(state<T> &in, state<T> &out){
}

template <typename T>
std::string jitter_module<T>::describe() {
  std::string s;
  s << "jitter_module " << this->name()
    << " with translation height range [" << th0 << " .. " << th1 << "]"
    << "and width range [" << tw0 << " .. " << tw1 << "]"
    << ", rotation range [" << deg0 << " .. " << deg1 << "]"
    << ", scaling height range [" << sh0 << " .. " << sh1 << "]"
    << " and width range [" << sw0 << " .. " << sw1 << "]"
    << ", shear height range [" << shh0 << " .. " << shh1 << "]"
    << " and width range [" << shw0 << " .. " << shw1 << "]"
    << ", elastic smoothing size range [" << elsz0 << " .. " << elsz1 << "]"
    << " and coeff range [" << elcoeff0 << " .. " << elcoeff1 << "]";
  if (zp) s << ", padding: " << zp->describe();
  return s;
}

template <typename T>
module_1_1<T>* jitter_module<T>::copy(parameter<T> *p) {
  jitter_module<T> *l2 = new jitter_module<T>(this->name());
  l2->th0 = th0; l2->th1 = th1; l2->tw0 = tw0; l2->tw1 = tw1;
  l2->deg0 = deg0; l2->deg1 = deg1;
  l2->sh0 = sh0; l2->sh1 = sh1; l2->sw0 = sw0; l2->sw1 = sw1;
  l2->shh0 = shh0; l2->shh1 = shh1; l2->shw0 = shw0; l2->shw1 = shw1;
  l2->elsz0 = elsz0; l2->elsz1 = elsz1;
  l2->elcoeff0 = elcoeff0; l2->elcoeff1 = elcoeff1;
  return l2;
}

} // end namespace ebl
