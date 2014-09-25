/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef TRAIN_UTILS_H_
#define TRAIN_UTILS_H_

#include <list>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

#include "defines.h"
#include "idx.h"
#include "job.h"
#include "ebl_trainer.h"

#ifdef __GUI__
#include "ebl_trainer_gui.h"
#endif

namespace ebl {


// training utilities //////////////////////////////////////////////////////////

//! This creates a trainable network and returns it.
template <typename T, typename Tdata, typename Tlabel>
supervised_trainer<T,Tdata,Tlabel>*
create_trainable_network(ddparameter<T> &theparam, configuration &conf,
                         uint noutputs, module_1_1<T> **net, uint &iter);
//! A function that performs tests, saves current weights and display.
//! \param iteration_seconds An optional elapsed time for the iteration,
//!   to better estimate the timeout of the training.
template <typename T, typename Tdata, typename Tlabel>
void test_and_save(uint iter, configuration &conf, std::string &conffname,
                   parameter<T> &theparam,
                   supervised_trainer<T,Tdata,Tlabel> &thetrainer,
                   labeled_datasource<T,Tdata,Tlabel> &train_ds,
                   labeled_datasource<T,Tdata,Tlabel> &test_ds,
                   classifier_meter &trainmeter, classifier_meter &testmeter,
                   infer_param &infp, gd_param &gdp, std::string &shortname,
                   long iteration_seconds = 0);

//! A function that performs tests and display.
template <typename T, typename Tdata, typename Tlabel>
void test(uint iter, configuration &conf, std::string &conffname,
          parameter<T> &theparam,
          supervised_trainer<T,Tdata,Tlabel> &thetrainer,
          labeled_datasource<T,Tdata,Tlabel> &train_ds,
          labeled_datasource<T,Tdata,Tlabel> &test_ds,
          classifier_meter &trainmeter, classifier_meter &testmeter,
          infer_param &infp, gd_param &gdp, std::string &shortname,
	  const std::string &basename);

//! A function that create/loads a validation set given configuration 'conf'.
//! \param noutputs The number of outputs will be modified according
//!   to the loaded dataset.
//! \param name This is updated to the root of all dataset filenames.
template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T,Tdata,Tlabel>*
create_validation_set(configuration &conf, uint &noutputs,
                      std::string &name);

//! A function that create/loads a training set given configuration 'conf'.
//! \param noutputs The number of outputs will be modified according
//!   to the loaded dataset.
//! \param name This is updated to the root of all dataset filenames.
template <typename T, typename Tdata, typename Tlabel>
labeled_datasource<T,Tdata,Tlabel>*
create_training_set(configuration &conf, uint &noutputs, std::string &name);

} // end namespace ebl

#include "train_utils.hpp"

#endif /* TRAIN_UTILS_ */
