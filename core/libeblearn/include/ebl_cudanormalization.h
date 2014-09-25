/***************************************************************************
 *   Copyright (C) 2011 by Soumith Chintala and Pierre Sermanet *
 *   soumith@gmail.com, pierre.sermanet@gmail.com *
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

#ifndef EBL_CUDANORMALIZATION_H_
#define EBL_CUDANORMALIZATION_H_

#include "ebl_normalization.h"
#include "ebl_cudaops.h"
#include "ebl_cudabasic.h"

#ifdef __CUDA__

namespace ebl {

  //////////////////////////////////////////////////////////////////////////////
  //! This module divides local neighborhoods by they standard deviation.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_divisive_norm_module : public divisive_norm_module<T, Tstate> {
  public:
    //! \param kerdim The neighborhood dimensions.
    //! \param nf The number of feature maps input to this module.
    //! \param mirror Use mirroring of the input to pad border if true,
    //!               or use zero-padding otherwise (default).
    //! \param p If specified, parameter p holds learned weights.
    //! \param across_features If true, normalize across feature dimensions
    //!   in addition to spatial dimensions.
    //! \param cgauss Gaussian kernel coefficient.
    cuda_divisive_norm_module(idxdim &kerdim, int nf, bool mirror = false,
			      bool threshold = true, parameter<T,Tstate> *p = NULL,
			      const char *name = "divisive_norm",
			      bool across_features = true, double cgauss = 2.0,
			      bool fsum_div = false, float fsum_split = 1.0,
			      double epsilon = NORM_EPSILON,
			      double espilon2 = 0, int gpu_id_ = -1);
    //! destructor
    virtual ~cuda_divisive_norm_module();
    //! Returns a deep copy of this module.
    //! \param p If NULL, reuse current parameter space, otherwise allocate new
    //!   weights on parameter 'p'.
    virtual cuda_divisive_norm_module<T,Tstate>*
      copy(parameter<T,Tstate> *p = NULL);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();

    // friends
    template <typename T1, class Tstate1> friend class cuda_contrast_norm_module;

    // internal methods ////////////////////////////////////////////////////////
  protected:
    //! out = in / thstd
    virtual void invert(Tstate &in, Tstate &thstd, Tstate &out);

    // members /////////////////////////////////////////////////////////////////
  protected:
    using divisive_norm_module<T,Tstate>::convvar;
    using divisive_norm_module<T,Tstate>::sqrtmod;
    using divisive_norm_module<T,Tstate>::sqmod;
    using divisive_norm_module<T,Tstate>::divconv;
    using divisive_norm_module<T,Tstate>::gaussian_kernel;
    using divisive_norm_module<T,Tstate>::conv_table;
    using divisive_norm_module<T,Tstate>::stride;
    using divisive_norm_module<T,Tstate>::mirror;
    using divisive_norm_module<T,Tstate>::insq;
    using divisive_norm_module<T,Tstate>::invar;
    using divisive_norm_module<T,Tstate>::instd;
    using divisive_norm_module<T,Tstate>::thstd;
    using divisive_norm_module<T,Tstate>::threshold;
    using divisive_norm_module<T,Tstate>::nfeatures;
    using divisive_norm_module<T,Tstate>::kerdim;
    using divisive_norm_module<T,Tstate>::across_features;//!< Norm across feats
    using divisive_norm_module<T,Tstate>::epsilon; //!< bias to avoid div by 0.
    using divisive_norm_module<T,Tstate>::epsilon2; //!< bias to avoid div by 0.
    using divisive_norm_module<T,Tstate>::cgauss;
    using divisive_norm_module<T,Tstate>::fsum_div;
    using divisive_norm_module<T,Tstate>::fsum_split;
  public:
    // GPU members /////////////////////////////////////////////////////////////
    int                 gpu_id; //!< Whether to use gpu or not
  };

  //////////////////////////////////////////////////////////////////////////////
  //! Subtractive normalization operation using a weighted expectation
  //! over a local neighborhood. An input set of feature maps is locally
  //! normalized to be zero mean.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_subtractive_norm_module
    : public subtractive_norm_module<T, Tstate> {
  public:
    //! \param kerdim The kernel dimensions.
    //! \param nf The number of feature maps input to this module.
    //! \param mirror Use mirroring of the input to pad border if true,
    //!               or use zero-padding otherwise (default).
    //! \param global_norm If true, apply global normalization first.
    //! \param p If specified, parameter p holds learned weights.
    //! \param across_features If true, normalize across feature dimensions
    //!   in addition to spatial dimensions.
    //! \param learn_mean If true, learn mean weighting.
    //! \param cgauss Gaussian kernel coefficient.
    //! \param valid If true, perform a valid convolution rather than a 'same' one.
    cuda_subtractive_norm_module(idxdim &kerdim, int nf, bool mirror = false,
				 bool global_norm = false,
				 parameter<T,Tstate> *p = NULL,
				 const char *name = "subtractive_norm",
				 bool across_features = true,
				 double cgauss = 2.0, bool fsum_div = false,
				 float fsum_split = 1.0, int gpu_id_ = -1,
				 bool valid = false);
    //! destructor
    virtual ~cuda_subtractive_norm_module();
    //! Returns a deep copy of this module.
    //! \param p If NULL, reuse current parameter space, otherwise allocate new
    //!   weights on parameter 'p'.
    virtual cuda_subtractive_norm_module<T,Tstate>*
      copy(parameter<T,Tstate> *p = NULL);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();

    // friends
    template <typename T1, class Tstate1> friend class cuda_contrast_norm_module;

    // members /////////////////////////////////////////////////////////////////
  protected:
    using subtractive_norm_module<T,Tstate>::param;
    using subtractive_norm_module<T,Tstate>::convmean;
    using subtractive_norm_module<T,Tstate>::meanconv;
    using subtractive_norm_module<T,Tstate>::padding;
    using subtractive_norm_module<T,Tstate>::gaussian_kernel;
    using subtractive_norm_module<T,Tstate>::conv_table;
    using subtractive_norm_module<T,Tstate>::stride;
    using subtractive_norm_module<T,Tstate>::difmod;
    using subtractive_norm_module<T,Tstate>::inmean;
    using subtractive_norm_module<T,Tstate>::global_norm;//!< global norm first
    using subtractive_norm_module<T,Tstate>::nfeatures;
    using subtractive_norm_module<T,Tstate>::kerdim;
    using subtractive_norm_module<T,Tstate>::across_features;//!< Norm across feats.
    using subtractive_norm_module<T,Tstate>::cgauss;
    using subtractive_norm_module<T,Tstate>::fsum_div;
    using subtractive_norm_module<T,Tstate>::fsum_split;
    using subtractive_norm_module<T,Tstate>::mirror;
  public:
    // GPU members /////////////////////////////////////////////////////////////
    int                 gpu_id; //!< Whether to use gpu or not
  };

  //////////////////////////////////////////////////////////////////////////////
  //! Local contrast normalization operation using a weighted expectation
  //! over a local neighborhood. An input set of feature maps is locally
  //! normalized to be zero mean and unit standard deviation.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_contrast_norm_module : public contrast_norm_module<T,Tstate> {
  public:
    //! \param kerdim The kernel dimensions.
    //! \param nf The number of feature maps input to this module.
    //! \param mirror Use mirroring of the input to pad border if true,
    //!               or use zero-padding otherwise (default).
    //! \param global_norm If true, apply global normalization first.
    //! \param p If specified, parameter p holds learned weights.
    //! \param across_features If true, normalize across feature dimensions
    //!   in addition to spatial dimensions.
    //! \param learn_mean If true, learn mean weighting.
    //! \param cgauss Gaussian kernel coefficient.
    //! \param valid If true, perform a valid convolution rather than a 'same' one.
    cuda_contrast_norm_module(idxdim &kerdim, int nf, bool mirror = false,
			      bool threshold = true, bool global_norm = false,
			      parameter<T,Tstate> *p = NULL,
			      const char *name = "contrast_norm",
			      bool across_features = true,
			      bool learn_mean = false,
			      double cnorm = 2.0, bool fsum_div = false,
			      float fsum_split = 1.0,
			      double epsilon = NORM_EPSILON,
			      double epsilon2 = 0, int gpu_id = -1,
			      bool valid = false);
    //! destructor
    virtual ~cuda_contrast_norm_module();
    //! Returns a deep copy of this module.
    //! \param p If NULL, reuse current parameter space, otherwise allocate new
    //!   weights on parameter 'p'.
    virtual cuda_contrast_norm_module<T,Tstate>*
      copy(parameter<T,Tstate> *p = NULL);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();

    // members ////////////////////////////////////////////////////////
  protected:
    using contrast_norm_module<T, Tstate>::subnorm;
    using contrast_norm_module<T, Tstate>::divnorm;
    using contrast_norm_module<T, Tstate>::tmp;
    using contrast_norm_module<T, Tstate>::global_norm;//!< global norm first
    using contrast_norm_module<T, Tstate>::learn_mean;//!< Learn mean weighting.
  public:
    // GPU members /////////////////////////////////////////////////////////////
    int                 gpu_id; //!< Whether to use gpu or not
  };

} // namespace ebl

#include "ebl_cudanormalization.hpp"

#endif // __CUDA__
#endif /* EBL_CUDANORMALIZATION_H_ */
