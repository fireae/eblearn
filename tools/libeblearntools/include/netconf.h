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

#ifndef NETCONF_H_
#define NETCONF_H_

#include "libeblearn.h"
#include "configuration.h"
#include "tools_utils.h"

namespace ebl {

//! Create a new network based on a configuration.
//! The configuration should at least contain these variables:
//! 'net_type' which can contain so far 'cscscf', 'cscsc', 'cscf', etc.
//! Other variables used are convolution and subsampling kernel sizes
//! such as 'net_c1h', 'net_c1w', 'net_s1h', etc.
//! See netconf.hpp for more details.
//! 'in' and 'out' are used for memory optimization if not null, otherwise
//! independent buffers are used in between each module (required for
//! learning).
//! \param in The input buffer for memory optimization.
//! \param out The output buffer for memory optimization.
// TODO: default is not allowed for function template,
// solution is to use -std=c++0x or -std=gnu++0x but not available everywhere
// -> find test for these capabilities in cmake
//! \param shared A map of parameters-shared modules.
//! \param loaded A map all of loaded modules so far.
//! \param tid Optional thread-id for multi-thread branching
template <typename T>
module_1_1<T>*
create_network(parameter<T> &theparam, configuration &conf, intg &thick,
               int noutputs = -1, const char *varname = "arch", int tid = -1,
               std::map<std::string,module_1_1<T>*> *shared = NULL,
               std::map<std::string,module_1_1<T>*> *loaded = NULL);


//! Create a module of type 'type' (with full name 'name'),
//! e.g. 'conv' and 'conv0' by querying variables in configuration 'conf'.
//! This returns a module_1_1 or NULL if failed.
//! \param shared A map of parameters-shared modules.
//! \param loaded A map all of loaded modules so far.
//! \param tid optional thread_id to do branch optimizations
template <typename T>
module_1_1<T>* create_module(const std::string &name, parameter<T> &theparam,
                             configuration &conf, int &nout, intg &thick,
                             std::map<std::string,module_1_1<T>*> &shared,
                             std::map<std::string,module_1_1<T>*> &loaded,
                             int tid = -1);

//! Create an ebm1 module of type 'type' (with full name 'name') and returns
//! it or NULL if an error occured.
template <typename T>
ebm_1<T>* create_ebm1(const std::string &name, configuration &conf);

//! Create a module of type 'answer_module' given an existing configuration
//! conf and variable name 'varname'.
template <typename T, typename Tds1, typename Tds2>
answer_module<T,Tds1,Tds2>* create_answer(configuration &conf, uint nclasses,
                                          const char *varname = "answer");

//! Create a module of type 'ds_ebm_2' given an existing network 'net'
//! to train and a configuration variable 'varname'.
template <typename T, typename Tds1, typename Tds2>
trainable_module<T,Tds1,Tds2>*
create_trainer(configuration &conf, module_1_1<T> &net,
               answer_module<T,Tds1,Tds2> &answer,
               const char *varname = "trainer");

// preprocessing /////////////////////////////////////////////////////////////

//! Create a preprocessing module given a target 'height' and 'width'
//! and other parameters.
//! \param ppchan The channel preprocessing type 'ppchan'
//!   (e.g. "RGB" or "YnUV"), a resizing
//! \param kersz The kernel size for normalized channel preprocessings.
//! \param resize_method The resizing method (e.g. "bilinear").
//! \param keep_aspect_ratio If true, aspect ratio is kept, ignored otherwise.
//! \param lpyramid The number of Laplacian pyramid scales, 0 for no pyramid.
//! \param fovea The fovea ratios, none for no fovea.
//! \param fovea_scale_size The rectangle sizes for each fovea scale
//! \param color_norm If true, contrast-normalize color channels.
//! \param cnorm_across If true and color_norm is true, color is normalized
//!   across each other, rather than layer by layer.
//! \param epsilon Small value added in normalization to avoid 0-divisions.
template <typename T>
resizepp_module<T>*
create_preprocessing(uint height, uint width, const char *ppchan,
                     idxdim &kersz, const char *resize_method = "bilinear",
                     bool keep_aspect_ratio = true, int lpyramid = 0,
                     std::vector<double> *fovea = NULL,
                     midxdim *fovea_scale_size = NULL,
                     bool globnorm = true,
                     bool locnorm = false, bool locnorm2 = false,
                     bool color_lnorm = false, bool cnorm_across = true,
                     double hscale = 1.0, double wscale = 1.0,
                     std::vector<float> *scalings = NULL,
                     const char *name = NULL, double epsilon = NORM_EPSILON,
                     double epsilon2 = 0);

//! Create a preprocessing module given a target 'height' and 'width'
//! and other parameters. In this version, a vector of kernels dimensions
//! can be passed instead of just one.
//! \param ppchan The channel preprocessing type 'ppchan'
//!   (e.g. "RGB" or "YnUV"), a resizing
//! \param kersz The kernel size for normalized channel preprocessings.
//! \param resize_method The resizing method (e.g. "bilinear").
//! \param keep_aspect_ratio If true, aspect ratio is kept, ignored otherwise.
//! \param lpyramid The number of Laplacian pyramid scales, 0 for no pyramid.
//! \param fovea The fovea ratios, none for no fovea.
//! \param fovea_scale_size The rectangle sizes for each fovea scale
//! \param color_norm If true, contrast-normalize color channels.
//! \param cnorm_across If true and color_norm is true, color is normalized
//!   across each other, rather than layer by layer.
//! \param epsilon Small value added in normalization to avoid 0-divisions.
template <typename T>
resizepp_module<T>*
create_preprocessing(midxdim &dims, const char *ppchan,
                     midxdim &kersz, midxdim &zpads,
                     const char *resize_method = "bilinear",
                     bool keep_aspect_ratio = true, int lpyramid = 0,
                     std::vector<double> *fovea = NULL,
                     midxdim *fovea_scale_size = NULL,
                     bool globnorm = true,
                     bool locnorm = false, bool locnorm2 = false,
                     bool color_lnorm = false, bool cnorm_across = true,
                     double hscale = 1.0, double wscale = 1.0,
                     std::vector<float> *scalings = NULL,
                     const char *name = NULL,
                     double epsilon = NORM_EPSILON, double epsilon2 = 0);

//! Create a preprocessing module without target dimensions. These can be set
//! later with the set_dimensions() method.
//! In this version, a vector of kernels dimensions can be passed instead of
//! just one.
//! \param ppchan The channel preprocessing type 'ppchan'
//!   (e.g. "RGB" or "YnUV"), a resizing
//! \param kersz The kernel size for normalized channel preprocessings.
//! \param resize_method The resizing method (e.g. "bilinear").
//! \param keep_aspect_ratio If true, aspect ratio is kept, ignored otherwise.
//! \param lpyramid The number of Laplacian pyramid scales, 0 for no pyramid.
//! \param fovea The fovea ratios, none for no fovea.
//! \param fovea_scale_size The rectangle sizes for each fovea scale
//! \param color_norm If true, contrast-normalize color channels.
//! \param cnorm_across If true and color_norm is true, color is normalized
//!   across each other, rather than layer by layer.
//! \param epsilon Small value added in normalization to avoid 0-divisions.
template <typename T>
resizepp_module<T>*
create_preprocessing(const char *ppchan, midxdim &kersz, midxdim &zpads,
                     const char *resize_method = "bilinear",
                     bool keep_aspect_ratio = true, int lpyramid = 0,
                     std::vector<double> *fovea = NULL,
                     midxdim *fovea_scale_size = NULL,
                     bool globnorm = true,
                     bool locnorm = false, bool locnorm2 = false,
                     bool color_lnorm = false, bool cnorm_across = true,
                     double hscale = 1.0, double wscale = 1.0,
                     std::vector<float> *scalings = NULL,
                     const char *name = NULL,
                     double epsilon = NORM_EPSILON, double epsilon2 = 0);

/////////////////////////////////////////////////////////////////////////////

//! Create a new network based on a configuration. This is relying
//! on the old-style variables like 'net_type' and 'net_c1h'. The more
//! generic version of this function is 'create_netowrk', which calls
//! this function if none of the generic variables were found.
//! The configuration should at least contain these variables:
//! 'net_type' which can contain so far 'cscscf', 'cscsc', 'cscf', etc.
//! Other variables used are convolution and subsampling kernel sizes
//! such as 'net_c1h', 'net_c1w', 'net_s1h', etc.
//! See netconf.hpp for more details.
//! 'in' and 'out' are used for memory optimization if not null, otherwise
//! independent buffers are used in between each module (required for
//! learning).
//! \param in The input buffer for memory optimization.
//! \param out The output buffer for memory optimization.
// TODO: default is not allowed for function template,
// solution is to use -std=c++0x or -std=gnu++0x but not available everywhere
// -> find test for these capabilities in cmake
template <typename T>
module_1_1<T>* create_network_old(parameter<T> &theparam,
                                  configuration &conf,
                                  int noutputs = -1);

//! Tries to find the weights variables associated with module_name, i.e.
//! module_name"_weights" and load the corresponding matrix file
//! into module m.
template <class Tmodule, typename T>
bool load_module(configuration &conf, module_1_1<T> &m,
                 const std::string &module_name, const std::string &type);

//! Load network's modules individually based on configuration and return
//! the number of weights loaded.
template <typename T>
uint manually_load_network(layers<T> &l, configuration &conf,
                           const char *varname = "arch");

//! Load the table for module with name 'module_name'. E.g. for module42,
//! this will look for variable 'module42_table' for the table filename
//! to load.
//! If not found it will then look for 'module42_table_in' and
//! 'module42_table_out' to create a full table from in to out.
//! If none of those variables are found, it'll return false.
bool EXPORT load_table(configuration &conf, const std::string &module_name,
                       idx<intg> &table, intg thickness, intg noutputs);

//! Load mandatory and optional gradient parameters from configuration
//! 'conf' into gradient parameters object 'gdp'.
void EXPORT load_gd_param(configuration &conf, gd_param &gdp);

} // end namespace ebl

#include "netconf.hpp"

#endif /* NETCONF_H_ */
