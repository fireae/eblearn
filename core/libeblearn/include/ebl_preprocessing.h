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

#ifndef EBL_PREPROCESSING_H_
#define EBL_PREPROCESSING_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_march.h"
#include "ebl_normalization.h"
#include "ebl_basic.h"

namespace ebl {

//! Normalization types.
enum t_norm {
  WSTD_NORM      = 0,                   // local contrast normalization
  LAPLACIAN_NORM = 1                    // laplacian normalization
};

//! Use mirror padding for preprocessing or not
#define DEFAULT_PP_MIRROR true

// channels_module /////////////////////////////////////////////////////////////

//! Abstract class for image channels preprocessing.
template <typename T> class channels_module: public module_1_1<T> {
 public:
  channels_module(bool global_norm = true, const char *name = "channels");
  virtual ~channels_module();
  //! Implement empty backprop, assuming training is not needed prior
  //! preprocessing.
  virtual void bprop1(state<T> &in, state<T> &out) {};
  virtual void bbprop1(state<T> &in, state<T> &out) {};

  //! Friends.
  template <typename T1> friend class laplacian_pyramid_module;

 protected:
  bool globnorm; //!< Normalize globally or not.
};

// channorm_module /////////////////////////////////////////////////////////////

//! Abstract class for normalization of image channels.
template <typename T> class channorm_module: public channels_module<T> {
 public:
  //! Constructor.
  //! \param kerdim Kernel dimensions for local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param nf The number of features to normalize across.
  //! \param globnorm Normalize channels globally or not.
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  channorm_module(idxdim &kerdim, bool mirror = DEFAULT_PP_MIRROR,
                  t_norm norm_mode = WSTD_NORM,
                  const char *name = "channorm", int nf = 1,
                  bool globnorm = true, double epsilon = NORM_EPSILON,
                  double epsilon2 = 0);
  //! Destructor
  virtual ~channorm_module();
  //! Forward propagation from in to out (abstract).
  virtual void fprop1(idx<T> &in, idx<T> &out) = 0;
  //! Returns a deep copy of this module (abstract).
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL) = 0;
  //! Returns a string containing describe() string of all modules.
  virtual std::string describe();
  //! Returns a new norm module.
  module_1_1<T>* new_norm(idxdim &normker, bool mirror, t_norm norm_mode,
                          int nf, double epsilon, double epsilon2 = 0);
  //! Resize the output based on input dimensions
  //! \param dim0 An optional size for the first dimension. Set it to 1
  //!             when converting from color to greyscale.
  virtual bool resize_output(state<T> &in, state<T> &out, idxdim *d = NULL);
  //! Resize the output based on input dimensions
  //! \param dim0 An optional size for the first dimension. Set it to 1
  //!             when converting from color to greyscale.
  virtual bool resize_output(idx<T> &in, idx<T> &out, idxdim *d = NULL);

 protected:
  idxdim	 normker;               //!< norm kernel size
  state<T>       tmp;                   //!< temporary buffer
  module_1_1<T>	*norm;                  //!< contrast normalization module
  bool           mirror;                //!< Mirror padding or not.
  t_norm         norm_mode;             //!< Normalization mode.
};

// rgb_to_ynuv_module //////////////////////////////////////////////////////////

//! convert an RGB input into a YpUV output, Yp being a Y channel
//! with a local normaliztion.
template <typename T> class rgb_to_ynuv_module: public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Yp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  rgb_to_ynuv_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                     t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                     double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~rgb_to_ynuv_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// rgb_to_ynuvn_module /////////////////////////////////////////////////////////

//! convert an RGB input into a YnUVn output, Yn being a Y channel
//! with a local normaliztion, and UV normalized locally together.
template <typename T> class rgb_to_ynuvn_module: public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Yp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  rgb_to_ynuvn_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                      t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                      double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~rgb_to_ynuvn_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
 protected:
  module_1_1<T> *norm2; //!< Normalization for UV.
};

// rgb_to_ynunvn_module ////////////////////////////////////////////////////////

//! convert an RGB input into a Ynunvn output, Yn being a Y channel
//! with a local normaliztion, and UV normalized locally together.
template <typename T> class rgb_to_ynunvn_module: public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Yp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  rgb_to_ynunvn_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                       t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                       double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~rgb_to_ynunvn_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// rgb_to_yuv_module ///////////////////////////////////////////////////////////

//! convert an RGB input into a YUV output.
template <typename T> class rgb_to_yuv_module: public channels_module<T> {
 public:
  //! Constructor.
  rgb_to_yuv_module(bool globnorm = true);
  //! destructor
  virtual ~rgb_to_yuv_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// rgb_to_yuvn_module //////////////////////////////////////////////////////////

//! Convert an RGB input into a YUVn output, where YUV is normalized
//! locally across all channels.
template <typename T> class rgb_to_yuvn_module: public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Yp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  rgb_to_yuvn_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                     t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                     double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~rgb_to_yuvn_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// rgb_to_rgb_module ///////////////////////////////////////////////////////////

//! Simple copy of RGB and global normalization if specified.
template <typename T> class rgb_to_rgb_module: public channels_module<T> {
 public:
  //! Constructor.
  rgb_to_rgb_module(bool globnorm = true, const char *name = "rgb_to_rgb");
  //! destructor
  virtual ~rgb_to_rgb_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// y_to_y_module ///////////////////////////////////////////////////////////

//! Simple copy of Y and global normalization if specified.
template <typename T> class y_to_y_module: public channels_module<T> {
 public:
  //! Constructor.
  y_to_y_module(bool globnorm = true);
  //! destructor
  virtual ~y_to_y_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// rgb_to_rgbn_module //////////////////////////////////////////////////////////

//! Convert an RGB input into a YUVn output, where YUV is normalized
//! locally across all channels.
template <typename T> class rgb_to_rgbn_module: public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Yp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  rgb_to_rgbn_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                     t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                     double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~rgb_to_rgbn_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// rgb_to_y_module /////////////////////////////////////////////////////////////

//! convert an RGB input into a Y channel.
template <typename T> class rgb_to_y_module: public channels_module<T> {
 public:
  //! Constructor.
  rgb_to_y_module(bool globnorm = true);
  //! destructor
  virtual ~rgb_to_y_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// rgb_to_yn_module ////////////////////////////////////////////////////////////

//! convert an RGB input into a Yp output, Yp being a Y channel
//! with a local normaliztion.
template <typename T> class rgb_to_yn_module: public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Yp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  rgb_to_yn_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                   t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                   double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~rgb_to_yn_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// y_to_yp_module //////////////////////////////////////////////////////////////

//! convert an Y input into a Yp output, Yp being a Y channel
//! with a local normaliztion.
template <typename T> class y_to_yp_module: public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Yp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  y_to_yp_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                 t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                 double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~y_to_yp_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// bgr_to_ypuv_module //////////////////////////////////////////////////////////

//! convert an BGR input into a YpUV output, Yp being a Y channel
//! with a local normaliztion.
template <typename T> class bgr_to_ypuv_module: public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Yp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  bgr_to_ypuv_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                     t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                     double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~bgr_to_ypuv_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// bgr_to_yp_module ////////////////////////////////////////////////////////////

//! convert an BGR input into a Yp output, Yp being a Y channel
//! with a local normaliztion.
template <typename T> class bgr_to_yp_module : public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Yp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  bgr_to_yp_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                   t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                   double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~bgr_to_yp_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// rgb_to_hp_module ////////////////////////////////////////////////////////////

//! convert an RGB input into a Hp output, Hp being a H channel (from HSV)
//! with a local normaliztion.
template <typename T> class rgb_to_hp_module: public channorm_module<T> {
 public:
  //! Constructor.
  //! \param normalization_size is the size of the kernel used for Hp's
  //!        local normalization.
  //! \param mirror If true, pad normalization if a mirror of the image
  //!   instead of with zeros. This can be useful in object detection when
  //!   objects are close to borders.
  //! \param norm_mode The type of normalization (WSTD_NORM by default).
  //! \param epsilon Small value added in normalization to avoid 0-divisions.
  rgb_to_hp_module(idxdim &norm_kernel, bool mirror = DEFAULT_PP_MIRROR,
                   t_norm norm_mode = WSTD_NORM, bool globnorm = true,
                   double epsilon = NORM_EPSILON, double epsilon2 = 0);
  //! destructor
  virtual ~rgb_to_hp_module();
  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
};

// resize_module ///////////////////////////////////////////////////////////////

//! Resize the input to the desired output (while preserving aspect ratio)
//! and apply a preprocessing module.
//! This is useful because in some situations preprocessing needs
//! to be done within the resizing operation. e.g. when resizing
//! while preserving aspect ratio, the output must eventually be copied into
//! the true desired output dimensions, but preprocessing must be done before
//! copying it to avoid edge detection between the empty parts of the image.
template <typename T> class resize_module: virtual public module_1_1<T> {
 public:
  //! Constructor. Preprocessing module pp will be deleted upon destruction.
  //! \param size The target dimensions (heightxwidth)
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param zpad Optional zero-padding is added on each side
  //! \param preserve_ratio If true, fit the image into target size while
  //!   keeping aspect ratio, potential empty areas are filled with zeros.
  resize_module(idxdim &size, uint mode = MEAN_RESIZE, idxdim *zpad = NULL,
                bool preserve_ratio = true, const char *name = "resize");
  //! Constructor without target dimensions. set_dimensions should be called
  //! later.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param size The target dimensions (heightxwidth)
  //! \param zpad Optional zero-padding is added on each side
  //! \param preserve_ratio If true, fit the image into target size while
  //!   keeping aspect ratio, potential empty areas are filled with zeros.
  resize_module(uint mode = MEAN_RESIZE, idxdim *zpad = NULL,
                bool preserve_ratio = true, const char *name = "resize");
  //! This constructor specifies resizing ratio for each dimension instead
  //! of fixed target sizes. The default resizing method is bilinear, as
  //! as other methods do not currently implement ratio inputs.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param size The target dimensions (heightxwidth)
  //! \param zpad Optional zero-padding is added on each side
  //! \param preserve_ratio If true, fit the image into target size while
  //!   keeping aspect ratio, potential empty areas are filled with zeros.
  resize_module(double hratio, double wratio, uint mode = MEAN_RESIZE,
                idxdim *zpad = NULL, bool preserve_ratio = true,
                const char *name = "resize");
  //! Resizes as the last outputs of module mod, plus "add" pixels.
  resize_module(module_1_1<T> *mod, idxdim &add,
                uint mode = MEAN_RESIZE, idxdim *zpad = NULL,
                const char *name = "resize");
  //! destructor
  virtual ~resize_module();
  //! sets the desired output dimensions.
  virtual void set_dimensions(intg height_, intg width_);
  //! set the region to use in the input image.
  //! by default, the input region is the entire image.
  virtual void set_input_region(const rect<int> &inr);
  //! set the region to use in the output image.
  //! by default, the output region is the entire size defined by
  //! set_dimensions().
  virtual void set_output_region(const rect<int> &outr);
  //! Shift input region by h and w pixels, multiply scale by s and
  //! rotate by r.
  virtual void set_jitter(int h, int w, float s, float r);
  //! Scale input region by factor s.
  virtual void set_scale_factor(double s);
  //! Scale input region hxw by factors shxsw.
  virtual void set_scale_factor(double sh, double sw);
  //! Set zero padding on each side for each dimension.
  virtual void set_zpads(intg hpad, intg wpad);
  //! Set zero padding based on 'kernel'.
  virtual void set_zpad(idxdim &kernel);
  //! Set zero padding based on 'kernel'.
  virtual void set_zpad(midxdim &kernels);
  //! Returns the input box in output space.
  virtual rect<int> get_original_bbox();
  //! Returns the input box in input image space.
  virtual rect<int> get_input_bbox();
  //! Returns all bounding boxes extracted in input space.
  virtual const std::vector<rect<int> >& get_input_bboxes();
  //! Returns all bounding boxes extracted in the output space.
  virtual const std::vector<rect<int> >& get_original_bboxes();
  //! Compute the input and output regions given 'in' and return input region.
  virtual rect<int> compute_regions(idx<T> &in);
  //! Set the input_bbox given output height and width.
  virtual void remember_regions(intg outh, intg outw, rect<int> &r);
  //! Set the displayable range of values for outputs of this module.
  virtual void set_display_range(T min, T max);
  //! Set 'min' and 'max' to the displayable range of this module's outputs.
  virtual void get_display_range(T &min, T &max);

  // fprop methods ///////////////////////////////////////////////////////////

  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Declare using module_1_1's regular fprop.
  using module_1_1<T>::fprop;
  //! Process 'in' into 'out' which will contain an array of idx, where each
  //! idx has different scale with different dimensions.
  virtual void fprop(state<T> &in, midx<T> &out);
  //! backward propagation from in to out (empty)
  virtual void bprop(state<T> &in, state<T> &out);
  //! bbackward propagation from in to out (empty)
  virtual void bbprop(state<T> &in, state<T> &out);

  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns a reference to the last output state set by fprop.
  virtual state<T>* last_output();
  //! Set a buffer in which to copy the output of future fprop() calls.
  //! This is useful to keep the preprocessed input around when discarding
  //! intermediate buffers for memory optimization.
  virtual void set_output_copy(state<T> &out);
  //! Modifies multi-input dimensions 'isize' to be compliant with module's
  //! architecture, and returns corresponding output dimensions.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim fprop_size(mfidxdim &isize);
  //! Returns input dimensions corresponding to output dimensions 'osize'.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual fidxdim bprop1_size(const fidxdim &osize);
  //! Returns input dimensions corresponding to output dimensions 'osize'.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim bprop_size(mfidxdim &osize);
  //! Returns a vector of idxdim stored after a call to fprop_size() or
  //! bprop_size() on this module. If containing multiple elements, they
  //! correspond to each state size contained in last_output().
  virtual mfidxdim get_msize();
  //! Returns the number of layers produced by this resizing module.
  virtual uint nlayers();
  //! Copy outputs 'out' into internal buffers.
  virtual void copy_outputs(state<T> &out);

  // members /////////////////////////////////////////////////////////////////
 protected:
  idxdim          size;                 //!< target sizes
  intg            height;               //!< target height
  intg            width;                //!< target width
  idx<T>          tmp;                  //!< temporary buffer
  idx<T>          tmp2;                 //!< temporary buffer
  state<T>        tmp3;                 //!< temporary buffer
  rect<int>       input_bbox;           //!< bbox of last extracted box in input
  uint            mode;                 //!< resizing mode.
  int             input_mode;           //!< mode parameter to resize function.
  rect<int>       inrect;               //!< input region of image
  rect<int>       outrect;              //!< input region in output image
  bool            inrect_set;           //!< use input region or not.
  bool            outrect_set;          //!< use output region or not.
  idxdim         *dzpad;                //!< zero-padding for each side
  zpad_module<T> *zpad;                 //!< Zero padding module.
  int             hjitter;              //!< Shift output by this many pixels
  int             wjitter;              //!< Shift output by this many pixels
  float           sjitter;              //!< Multiply scale by this
  float           rjitter;              //!< Rotate by this degrees.
  double          scale_hfactor;        //!< Input region scale h factor.
  double          scale_wfactor;        //!< Input region scale w factor.
  bool            preserve_ratio;       //!< Preserve aspect ratio or not.
  double          hratio;               //!< Resizing ratio in height dim.
  double          wratio;               //!< Resizing ratio in width dim.
  state<T>       *lastout;              //!< Pointer to last out set by fprop.
  state<T>        lout;                 //!< Container for last out of fprop.
  mfidxdim        msize;                //!< Resulting dims of bprop_size.
  state<T>       *out_copy;             //!< A copy of last fprop output.
  T               display_min;          //!< Lower bound of displayable range.
  T               display_max;          //!< Higher bound of displayable range.
  std::vector<rect<int> >   input_bboxes; //!< Vector of input bboxes.
  std::vector<rect<int> >   original_bboxes; //!< bbox of original inputs in out
  module_1_1<T>  *mod;                  //!< Module to mimic outputs sizes.
  idxdim          modadd;               //!< Pixels to add to mod's mimiced size
};

// resizepp_module /////////////////////////////////////////////////////////////

//! Resize the input to the desired output (while preserving aspect ratio)
//! and apply a preprocessing module.
//! This is useful because in some situations preprocessing needs
//! to be done within the resizing operation. e.g. when resizing
//! while preserving aspect ratio, the output must eventually be copied into
//! the true desired output dimensions, but preprocessing must be done before
//! copying it to avoid edge detection between the empty parts of the image.
template <typename T> class resizepp_module: public resize_module<T> {
 public:
  //! Constructor. Preprocessing module pp will be deleted upon destruction.
  //! \param size The target dimensions (heightxwidth)
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param zpad Optional zero-padding is added on each side
  //! \param preserve_ratio If true, fit the image into target size while
  //!   keeping aspect ratio, potential empty areas are filled with zeros.
  resizepp_module(idxdim &size, uint mode = MEAN_RESIZE,
                  module_1_1<T> *pp = NULL,
                  bool own_pp = true, idxdim *zpad = NULL,
                  bool preserve_ratio = true, const char *name = "resizepp");
  //! Constructor without target dimensions. set_dimensions should be called
  //! later. Preprocessing module pp will be deleted upon destruction.
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param size The target dimensions (heightxwidth)
  //! \param zpad Optional zero-padding is added on each side
  //! \param preserve_ratio If true, fit the image into target size while
  //!   keeping aspect ratio, potential empty areas are filled with zeros.
  resizepp_module(uint mode = MEAN_RESIZE, module_1_1<T> *pp = NULL,
                  bool own_pp = true, idxdim *zpad = NULL,
                  bool preserve_ratio = true, const char *name = "resizepp");
  //! This constructor specifies resizing ratio for each dimension instead
  //! of fixed target sizes. The default resizing method is bilinear, as
  //! as other methods do not currently implement ratio inputs.
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param size The target dimensions (heightxwidth)
  //! \param zpad Optional zero-padding is added on each side
  //! \param preserve_ratio If true, fit the image into target size while
  //!   keeping aspect ratio, potential empty areas are filled with zeros.
  resizepp_module(double hratio, double wratio,
                  uint mode = MEAN_RESIZE, module_1_1<T> *pp = NULL,
                  bool own_pp = true, idxdim *zpad = NULL,
                  bool preserve_ratio = true, const char *name = "resizepp");
  //! destructor
  virtual ~resizepp_module();
  // fprop methods ///////////////////////////////////////////////////////////

  //! forward propagation from in to out
  virtual void fprop1(idx<T> &in, idx<T> &out);
  //! Declare using module_1_1's regular fprop.
  using resize_module<T>::fprop;
  //! Process 'in' into 'out' which will contain an array of idx, where each
  //! idx has different scale with different dimensions.
  virtual void fprop(state<T> &in, midx<T> &out);

  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // members /////////////////////////////////////////////////////////////////
 protected:
  module_1_1<T> *pp;                    //!< preprocessing module
  bool           own_pp;                //!< responsible for pp's deletion
  state<T>       inpp, outpp;           //!< input/output buffers for pp

  using resize_module<T>::size;
  using resize_module<T>::height;
  using resize_module<T>::width;
  using resize_module<T>::tmp;
  using resize_module<T>::tmp2;
  using resize_module<T>::tmp3;
  using resize_module<T>::input_bbox;
  using resize_module<T>::input_bboxes;
  using resize_module<T>::original_bboxes;
  using resize_module<T>::preserve_ratio;
  using resize_module<T>::mode;
  using resize_module<T>::input_mode;
  using resize_module<T>::inrect;
  using resize_module<T>::outrect;
  using resize_module<T>::inrect_set;
  using resize_module<T>::outrect_set;
  using resize_module<T>::dzpad;
  using resize_module<T>::zpad;
  using resize_module<T>::rjitter;
  using resize_module<T>::hratio;
  using resize_module<T>::wratio;
  using resize_module<T>::scale_hfactor;
  using resize_module<T>::scale_wfactor;
  using resize_module<T>::lout;
  using resize_module<T>::lastout;
};

// fovea_module ////////////////////////////////////////////////////////////////

//! Same as resizepp_module, except that it replicates the output at
//! different scales.
template <typename T> class fovea_module : public resizepp_module<T> {
 public:
  //! Constructor. Preprocessing module pp will be deleted upon destruction.
  //! \param boxscale If true, rescale input box by fovea factors and input is
  //!   unchanged, otherwise resize input by 1/(fovea factors) and box is
  //!   unchanged. Box scaling should be used for training extraction
  //!   and image scaling for detection (default is false).
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param size The target dimensions (heightxwidth)
  //! \param zpad Optional zero-padding is added on each side
  // TODO: the comments dont match the constructor arguments anymore
  fovea_module(std::vector<double> &fovea,
               midxdim &fovea_scales_size, idxdim &dsize,
               bool boxscale = false, uint mode = MEAN_RESIZE,
	       module_1_1<T> *pp = NULL,
               bool own_pp = true, idxdim *dzpad = NULL,
               const char *name = "fovea");
  //! Constructor without target dimensions. set_dimensions should be called
  //! later. Preprocessing module pp will be deleted upon destruction.
  //! \param boxscale If true, rescale input box by fovea factors and input is
  //!   unchanged, otherwise resize input by 1/(fovea factors) and box is
  //!   unchanged. Box scaling should be used for training extraction
  //!   and image scaling for detection (default is false).
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param zpad Optional zero-padding is added on each side
  fovea_module(std::vector<double> &fovea, bool boxscale = false,
               uint mode = MEAN_RESIZE,
               module_1_1<T> *pp = NULL,
               bool own_pp = true, idxdim *dzpad = NULL,
               const char *name = "fovea");
  virtual ~fovea_module();
  //! Process 'in' into 'out' which will contain all foveas stacked in first
  //! dimensions.
  virtual void fprop(state<T> &in, state<T> &out);
  //! Process 'in' into 'out' which will contain an array of idx, where each
  //! idx is a fovea scale
  virtual void fprop(state<T> &in, midx<T> &out);
  //! backward propagation from in to out (empty)
  virtual void bprop(state<T> &in, state<T> &out);
  //! bbackward propagation from in to out (empty)
  virtual void bbprop(state<T> &in, state<T> &out);

  //! Returns input dimensions corresponding to output dimensions 'osize'.
  virtual mfidxdim bprop_size(mfidxdim &osize);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns bounding boxes of each scale in the input space.
  const std::vector<rect<int> >& get_input_bboxes();
  //! Returns bounding boxes of each scale in the output space.
  const std::vector<rect<int> >& get_original_bboxes();
  //! Returns the number of layers produced by this resizing module.
  virtual uint nlayers();

  // members ////////////////////////////////////////////////////////
 protected:
  std::vector<double> fovea;            //!< A vector of all fovea scales.
  midxdim             fovea_scales_size;//!< A vector for sizes of each scale
  bool                boxscale;         //!< Scaling input box or input image.
  std::vector<rect<int> > obboxes;      //!< Boxes in output.
  std::vector<rect<int> > ibboxes;      //!< Boxes in input.
};

// laplacian_pyramid_module ////////////////////////////////////////////////////

//! Creates a laplacian pyramid of the input.
template <typename T> class laplacian_pyramid_module : public resizepp_module<T> {
 public:
  //! Constructor. Preprocessing module pp will be deleted upon destruction.
  //! \param nscales Number of scales of pyramid, starting from target
  //!   dimensions down with a subsampling ratio of 2.
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param pp_original Preprocessing for original channel.
  //! \param add_original If true, the first object in the output will
  //!   be the original preprocessed patch of target size, without Laplacian.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param sizes A vector of target dimensions (heightxwidth). If it
  //!   contains 1 target only, simply scale by .5 at each scale. Otherwise,
  //!   manually set each target to the ones found in this vector.
  //! \param zpad Optional zero-padding is added on each side
  //! \param global_norm If true (default), removes global mean from output
  //!   and divides it by standard deviation.
  //! \param local_norm If true, removes local mean and divide by std dev
  //!   in 5x5 neighborhood.
  //! \param color_norm If true, contrast-normalize color channels.
  //! \param cnorm_across If true and color_norm is true, color is normalized
  //!   across each other, rather than layer by layer.
  //! \param keep_aspect_ratio If true (default), aspect ratio is kept.
  laplacian_pyramid_module(uint nscales, midxdim &sizes,
                           uint mode = MEAN_RESIZE,
                           module_1_1<T> *pp = NULL,
                           bool own_pp = true, idxdim *dzpad = NULL,
                           bool global_norm = true, bool local_norm = false,
                           bool local_norm2 = false, bool color_norm = false,
                           bool cnorm_across = true,
                           bool keep_aspect_ratio = true,
                           const char *name = "laplacian_pyramid");
  //! Constructor. Preprocessing module pp will be deleted upon destruction.
  //! \param nscales Number of scales of pyramid, starting from target
  //!   dimensions down with a subsampling ratio of 2.
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param pp_original Preprocessing for original channel.
  //! \param add_original If true, the first object in the output will
  //!   be the original preprocessed patch of target size, without Laplacian.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param sizes A vector of target dimensions (heightxwidth). If it
  //!   contains 1 target only, simply scale by .5 at each scale. Otherwise,
  //!   manually set each target to the ones found in this vector.
  //! \param zpad Optional zero-padding is added on each side
  //! \param global_norm If true (default), removes global mean from output
  //!   and divides it by standard deviation.
  //! \param local_norm If true, removes local mean and divide by std dev
  //!   in 5x5 neighborhood.
  //! \param color_norm If true, contrast-normalize color channels.
  //! \param cnorm_across If true and color_norm is true, color is normalized
  //!   across each other, rather than layer by layer.
  //! \param keep_aspect_ratio If true (default), aspect ratio is kept.
  laplacian_pyramid_module(uint nscales, idxdim &kerdims,
                           midxdim &sizes, uint mode = MEAN_RESIZE,
                           module_1_1<T> *pp = NULL,
                           bool own_pp = true, idxdim *dzpad = NULL,
                           bool global_norm = true, bool local_norm = false,
                           bool local_norm2 = false, bool color_norm = false,
                           bool cnorm_across = true,
                           bool keep_aspect_ratio = true,
                           const char *name = "laplacian_pyramid");
  //! Constructor. Preprocessing module pp will be deleted upon destruction.
  //! \param nscales Number of scales of pyramid, starting from target
  //!   dimensions down with a subsampling ratio of 2.
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param pp_original Preprocessing for original channel.
  //! \param add_original If true, the first object in the output will
  //!   be the original preprocessed patch of target size, without Laplacian.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param sizes A vector of target dimensions (heightxwidth). If it
  //!   contains 1 target only, simply scale by .5 at each scale. Otherwise,
  //!   manually set each target to the ones found in this vector.
  //! \param zpad Optional zero-padding is added on each side
  //! \param global_norm If true (default), removes global mean from output
  //!   and divides it by standard deviation.
  //! \param local_norm If true, removes local mean and divide by std dev
  //!   in 5x5 neighborhood.
  //! \param color_norm If true, contrast-normalize color channels.
  //! \param cnorm_across If true and color_norm is true, color is normalized
  //!   across each other, rather than layer by layer.
  //! \param keep_aspect_ratio If true (default), aspect ratio is kept.
  laplacian_pyramid_module(uint nscales, midxdim &kerdims,
                           midxdim &sizes, uint mode = MEAN_RESIZE,
                           module_1_1<T> *pp = NULL,
                           bool own_pp = true, idxdim *dzpad = NULL,
                           bool global_norm = true, bool local_norm = false,
                           bool local_norm2 = false, bool color_norm = false,
                           bool cnorm_across = true,
                           bool keep_aspect_ratio = true,
                           const char *name = "laplacian_pyramid");
  //! Constructor without target dimensions. set_dimensions should be called
  //! later. Preprocessing module pp will be deleted upon destruction.
  //! \param nscales Number of scales of pyramid, starting from target
  //!   dimensions down with a subsampling ratio of 2.
  //! \param pp An optional pointer to a  preprocessing module. If NULL, no
  //!           preprocessing is performed. This module is responsible for
  //!           destroying the preprocessing module.
  //! \param pp_original Preprocessing for original channel.
  //! \param add_original If true, the first object in the output will
  //!   be the original preprocessed patch of target size, without Laplacian.
  //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
  //!             GAUSSIAN_RESIZE).
  //! \param zpad Optional zero-padding is added on each side
  //! \param global_norm If true (default), removes global mean from output
  //!   and divides it by standard deviation.
  //! \param local_norm If true, removes local mean and divide by std dev
  //!   in 5x5 neighborhood.
  //! \param color_norm If true, contrast-normalize color channels.
  //! \param cnorm_across If true and color_norm is true, color is normalized
  //!   across each other, rather than layer by layer.
  //! \param keep_aspect_ratio If true (default), aspect ratio is kept.
  laplacian_pyramid_module(uint nscales,
                           uint mode = MEAN_RESIZE,
                           module_1_1<T> *pp = NULL,
                           bool own_pp = true, idxdim *dzpad = NULL,
                           bool global_norm = true, bool local_norm = false,
                           bool local_norm2 = false, bool color_norm = false,
                           bool cnorm_across = true,
                           bool keep_aspect_ratio = true,
                           const char *name = "laplacian_pyramid");
  virtual ~laplacian_pyramid_module();
  //! Init filters and normalizations.
  virtual void init();

  // processing //////////////////////////////////////////////////////////////

  //! Process 'in' into 'out' which will contain all scales separated in
  //! each state of the multi-state 'out'.
  virtual void fprop(state<T> &in, state<T> &out);
  //! Process 'in' into 'out' which will contain an array of idx, where each
  //! idx has different scale with different dimensions.
  virtual void fprop(state<T> &in, midx<T> &out);
  //! backward propagation from in to out (empty)
  virtual void bprop(state<T> &in, state<T> &out);
  //! bbackward propagation from in to out (empty)
  virtual void bbprop(state<T> &in, state<T> &out);

  // size propagation ////////////////////////////////////////////////////////

  //! Modifies multi-input dimensions 'isize' to be compliant with module's
  //! architecture, and returns corresponding output dimensions.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim fprop_size(mfidxdim &isize);
  //! Returns input dimensions corresponding to output dimensions 'osize'.
  //! Implementation of this method helps automatic scaling of input data
  //! but is optional.
  virtual mfidxdim bprop_size(mfidxdim &osize);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns the number of layers produced by this resizing module.
  virtual uint nlayers();
  //! Produce multiple pyramids, each starting at a scaling of the original
  //! input.
  virtual void set_scalings(std::vector<float> &scalings);

  // internal methods ////////////////////////////////////////////////////////
 protected:
  //! Resize 'im' to target 'tgt' and update 'inr' and 'outr'.
  void resize(idx<T> &im, idxdim &tgt, rect<int> &inr, rect<int> &outr);
  //! Blur 'in' into 'out' using blurring filter 'filter'.
  //! \param roi The region of interest, to be reduced if no padding is used.
  void blur(idx<T> &filter, idx<T> &in, idx<T> &out, rect<int> &roi);
  //! Highpass 'in' by subtracting 'blurred' version of the image
  //! into a new buffer and return it.
  //! The input region 'inr' will be centered in the returned buffer.
  //! The returned buffer will have 'tgt' dimensions.
  //! \param first It is the first time highpass is called or not.
  idx<T> highpass(idx<T> &in, idx<T> &blurred, idxdim &tgt, rect<int> &inr,
                  bool first);
  //! Subsample 'in' into 'out' with a factor of 2. If 'in' dimensions are not
  //! a factor of 2, the last odd row or column are simply dropped.
  //! This downsamples 'inr' accordingly.
  void subsample(idx<T> &in, idx<T> &out, rect<int> &inr);
  //! Normalize entire 'in' image, using mean and standard deviation of the
  //! region of interest of 'roi' of the image. Intensity (0) and color
  //! channels (1 & 2) are normalized independently.
  //! Normalizing from the ROI only can be important, if the non-ROI contains
  //! zero-padding.
  void normalize_globally(idx<T> &in); //, rect<int> &roi);
  //! Called by normalize_globally().
  void normalize_intensity_globally(idx<T> &in); // , rect<int> &roi);
  //! Called by normalize_globally().
  void normalize_color_globally(idx<T> &in); // , rect<int> &roi);
  //! Called by normalize_globally().
  void normalize_globally2(idx<T> &in); // , rect<int> &roi);

  // members ////////////////////////////////////////////////////////
 protected:
  uint      nscales; //!< Number of scales in the pyramid.
  uint      iscale;  //!< Current scale being produced during fprop.
  std::vector<float> scalings; //!< Scaling factor for multiple pyramids.
  midxdim   sizes;   //!< The target dimensions for each scale.
  midxdim   kerdims; //!< The kernel dimensions for each scale.
  idx<T>    burt;    //!< The Burt-Adelson blurring kernel.
  std::vector<idx<T> > filters;//!< The filtering gauss kernels for each scale.
  std::vector<zpad_module<T>*> pads;    //!< Padding class for input.
  bool      use_pad; //!< Pad before filtering or not.
  bool      global_norm; //!< Globally normalize outputs or not.
  bool      local_norm; //!< Locally normalize outputs or not.
  bool      local_norm2; //!< Locally normalize outputs or not.
  bool      color_lnorm; //!< Locally normalize color or not.
  bool      cnorm_across; //!< Locally color across each other or separately.
  bool      keep_aspect_ratio; //!< Keep aspect ratio or not.
  bool      mirror;  //!< Use mirror padding instead of zero padding.
  idx<T>    blurred; //!< The blurred buffer.
  idx<T>    blurred_high; //!< The blurred buffer for high frequency extraction.
  idx<T>    high0;   //!< Temporary buffer for high frequencies.
  state<T>  padded;  //!< A temporary buffer for padding.
  std::vector<layers<T>*>      norms;   //!< Brightness normalization modules.
  std::vector<layers<T>*>      cnorms;  //!< Color normalization modules.
  midx<T>   tmp;
  state<T>  tmp2, tmp3;
  state<T>  inpp, outpp; //!< Temporary buffers for preprocessing.
  midx<T>   outs;    //!< Latest outputs.
  state<T>  zpad_out; //!< Zero-padded outputs.
  bool      burt_filtering_only; //!< Only use Burt-Adelson for filtering.
  using resizepp_module<T>::zpad;
  using resizepp_module<T>::input_bboxes;
  using resizepp_module<T>::original_bboxes;
};

// mschan_module ///////////////////////////////////////////////////////////////

//! This module takes each channel of the input and create a state
//! where each tensor contains a different channel.
//! This is useful for fovea processing, when preprocessing was already
//! performed by outputting each scale in a different channel.
template <typename T> class mschan_module : public module_1_1<T> {
 public:
  //! \param nstates The number of states to produce, i.e. in how many
  //!   groups to output the channels. E.g. if input is RGBRGB and nstates = 2
  //!   then it will separate the input into 2 states of RGB and RGB.
  mschan_module(uint nstates, const char *name = "mschan");
  virtual ~mschan_module();
  //! forward propagation from in to out
  virtual void fprop(state<T> &in, state<T> &out);
  //! backward propagation from in to out (empty)
  virtual void bprop(state<T> &in, state<T> &out);
  //! bbackward propagation from in to out (empty)
  virtual void bbprop(state<T> &in, state<T> &out);

 protected:
  uint nstates;
};

// jitter //////////////////////////////////////////////////////////////////////

//! This module jitters inputs into outputs.
template<typename T> class EXPORT jitter_module : public module_1_1<T> {
public:
  //! Creates a jitter module.
  jitter_module(const char *name = "jitter_module");
  //! Destructor.
  virtual ~jitter_module();

  // jitters setting /////////////////////////////////////////////////////////

  //! Set translation jitter between v[0] and v[1] for height
  //! and v[2] and v[3] for width (in pixels).
  void set_translations(std::vector<int> &v);
  //! Set rotation jitter between v[0] and v[1] (in degrees).
  void set_rotations(std::vector<float> &v);
  //! Set scaling jitter between v[0] and v[1] for height
  //! and v[2] and v[3] for width.
  void set_scalings(std::vector<float> &v);
  //! Set shearing jitter between v[0] and v[1] for height
  //! and v[2] and v[3] for width.
  void set_shears(std::vector<float> &v);
  //! Set elastic smoothing size between v[0] and v[1] and output
  //! factors between v[2] and v[3].
  void set_elastics(std::vector<float> &v);
  //! Adds padding on top, left, bottom and right from pads[0], pads[1],
  //! pads[2], pads[3].
  void set_padding(std::vector<uint> &pads);

  // multi-state inputs and outputs //////////////////////////////////////////
  virtual void fprop1(idx<T> &in, idx<T> &out);
  virtual void bprop1(state<T> &in, state<T> &out);
  virtual void bbprop1(state<T> &in, state<T> &out);

  //! Returns a string describing this module and its parameters.
  virtual std::string describe();
  //! Returns a deep copy of this module.
  //! \param p If NULL, the copy points to the same weights as this module.
  virtual module_1_1<T>* copy(parameter<T> *p = NULL);
protected:
  // variable members ////////////////////////////////////////////////////////
  int th0, th1, tw0, tw1; //!< Min and max height and widths translations.
  float deg0, deg1; //!< Min and max rotation in degrees.
  float sh0, sh1, sw0, sw1; //!< Min and max height/width scaling ratios.
  float shh0, shh1, shw0, shw1; //!< Min and max height/width shearing ratios.
  uint elsz0, elsz1; //!< Min/max elastic smoothing size.
  float elcoeff0, elcoeff1; //!< Min/max elastic coefficient.
  zpad_module<T> *zp; //!< Zero-padding.
  state<T> tmp;
};

} // namespace ebl {

#include "ebl_preprocessing.hpp"

#endif /* EBL_PREPROCESSING_H_ */
