/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
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

#ifndef EBL_TRAINER_H_
#define EBL_TRAINER_H_

#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_machines.h"
#include "ebl_logger.h"
#include "datasource.h"
#include "ebl_answer.h"

namespace ebl {

// supervised_trainer //////////////////////////////////////////////////////////

//! Supervised Trainer. A specialisation of the generic trainer, taking
//! samples (of type T) and labels (of type Tlabel) as training input.
//! Template T is the network's type and also the input data's type.
//! However datasources with different data type may be provided in which
//! case a conversion will occur after each sample extraction from the
//! datasource (via a deep idx_copy).
template <typename T, typename Tdata, typename Tlabel>
class supervised_trainer {
 public:
  //! constructor.
  supervised_trainer(trainable_module<T,Tdata,Tlabel> &m,
                     ddparameter<T> &p);
  //! destructor.
  virtual ~supervised_trainer();

  // per-sample methods //////////////////////////////////////////////////////

  //! Test the current sample of 'ds', put the answers in 'answers' and
  //! return true if the infered label equals the groundtruth 'label'.
  bool test_sample(labeled_datasource<T,Tdata,Tlabel> &ds,
                   state<T> &label, state<T> &answers,
                   infer_param &infp);
  //! Perform a learning update on the current sample of 'ds', using
  //! 'arguments arg' for the parameter update method
  //! (e.g. learning rate and weight decay).
  T train_sample(labeled_datasource<T,Tdata,Tlabel> &ds,
                    gd_param &arg);
  //! Perform a learning update on the 'sample' and 'label', using
  //! 'arguments arg' for the parameter update method
  //! (e.g. learning rate and weight decay).
  //! This assumes label is a unique value and its corresponding target
  //! can be retrieve by the trainable module.
  T train_sample(state<T> &sample, const Tlabel label, gd_param &arg);

  // epoch methods ///////////////////////////////////////////////////////////

  //! Measure the average energy and classification error rate
  //! on a dataset.
  //! \param max_test If > 0, limit the number of tests to this number.
  void test(labeled_datasource<T, Tdata, Tlabel> &ds,
            classifier_meter &log, infer_param &infp,
            uint max_test = 0);
  //! train for <niter> sweeps over the training set. <samples> contains the
  //! inputs samples, and <labels> the corresponding desired categories
  //! <labels>.
  //! return the average energy computed on-the-fly.
  //! <update-args> is a list of arguments for the parameter
  //! update method (e.g. learning rate and weight decay).
  //! \param hessian_period Recompute 2nd order derivatives at every
  //!   'hessian_period' samples if > 0.
  //! \param nhessian Estimate 2nd order derivatives on 'nhessian' samples.
  void train(labeled_datasource<T, Tdata, Tlabel> &ds,
             classifier_meter &log, gd_param &args, int niter,
             infer_param &infp,
             intg hessian_period = 0, intg nhessian = 0, double mu = .02);
  //! compute hessian
  void compute_diaghessian(labeled_datasource<T, Tdata, Tlabel> &ds,
                           intg niter, double mu);

  // accessors ///////////////////////////////////////////////////////////////

  //! Set iteration id to i. This can be useful when resuming a training
  //! to a certain iteration.
  void set_iteration(int i);
  //! Set the modulo at which test results are displayed (0 by default).
  void set_test_display_modulo(intg modulo);
  //! pretty some information about training, e.g. input and network sizes.
  void pretty(labeled_datasource<T, Tdata, Tlabel> &ds);
  //! Sets the name of the file indicating progress of training.
  //! If set, this file will be 'touched' after each sample is trained
  //! or tested to indicate that training is still going on.
  void set_progress_file(const std::string &s);
  //! If progress file is defined, touch the file to let outside world
  //! know that training is still alive.
  void update_progress();

  // friends /////////////////////////////////////////////////////////////////

  // template <class Tdata, class Tlabel> friend class supervised_trainer_gui;
  template <class T1, class T2, class T3> friend class supervised_trainer_gui;

  // internal methods ////////////////////////////////////////////////////////
 protected:

  //! init datasource to begining and assign indata to a buffer
  //! corresponding to ds's sample size. also increment iteration counter,
  //! unless new_iteration is false.
  void init(labeled_datasource<T, Tdata, Tlabel> &ds,
            classifier_meter *log = NULL, bool new_iteration = false);

  // members /////////////////////////////////////////////////////////////////
 protected:
  trainable_module<T,Tdata,Tlabel> &machine;
  ddparameter<T> &param;                //!< the learned params
  state<T>        energy;               //!< Tmp energy buffer.
  state<T>       *answers;              //!< Tmp answer buffer.
  state<T>       *label;                //!< Tmp label buffer.
  intg            age;
  int             iteration;
  void           *iteration_ptr;
  bool            prettied;             //!< Flag used to pretty info just once.
  std::string     progress_file;        //!< Name of progress file.
  intg            progress_cnt;         //!< A count for updating progress.
  bool            test_running;         //!< Show test on trained.
  intg            test_display_modulo;  //!< Modulo at which to display.
};

} // namespace ebl {

#include "ebl_trainer.hpp"

#endif /* EBL_TRAINER_H_ */
