/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
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

using namespace ebl;
using namespace std;

#ifdef __GUI__
#include "libeblearngui.h"
#endif

template <typename Tnet>
void test_lenet5_mnist(string *dir, string *errmsg, double eta) {
  cout << "initializing random seed with " << fixed_init_drand() << endl;
  typedef ubyte Tdata;
  typedef ubyte Tlab;
#ifdef __GUI__
  bool display = true;
#endif
  //uint ninternals = 1;
  cout << endl;
  // for testing purposes, we always initialize the randomization with 0 so
  // that we know the exact results.
  // in the real world, init_drand should be called with time(NULL) as argument.
  CPPUNIT_ASSERT_MESSAGE(*errmsg, dir != NULL);

  // load MNIST dataset
  mnist_datasource<Tnet,Tdata,Tlab>
    test_ds(dir->c_str(), false, 1000),
    train_ds(dir->c_str(), true, 2000);
  train_ds.set_balanced(true);
  train_ds.set_shuffle_passes(true);
  // set 2nd argument to true for focusing on hardest examples
  //  train_ds.set_weigh_samples(true, false, false, 0.01);
  train_ds.set_weigh_samples(false, false, false, 0.01);
  test_ds.set_epoch_show(500); // show progress every 100 samples
  train_ds.set_epoch_show(500); // show progress every 100 samples
  train_ds.ignore_correct(true);

  // create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  idx<Tnet> targets =
    create_target_matrix<Tnet>(1+idx_max(train_ds.labels), 1.0);
  uint nclasses = targets.dim(0);

  // create the network weights, network and trainer
  idxdim dims(train_ds.sample_dims()); // get order and dimensions of sample
  ddparameter<Tnet> theparam(60000); // create trainable parameter
  lenet5<Tnet> net(theparam, 32, 32, 5, 5, 2, 2, 5, 5, 2, 2, 120,
		   nclasses, true, false, true, false);
  cout << net.describe() << endl;

  l2_energy<Tnet> energy;
  class_answer<Tnet,Tdata,Tlab> answer(nclasses);
  trainable_module<Tnet,Tdata,Tlab> trainable(energy, net, NULL, &answer);
  supervised_trainer<Tnet, ubyte, ubyte> thetrainer(trainable, theparam);

#ifdef __GUI__
  //  labeled_datasource_gui<Tnet, ubyte, ubyte> dsgui(true);
//   dsgui.display(test_ds, 10, 10);
  supervised_trainer_gui<Tnet,Tdata,Tlab> stgui;
#endif

  // a classifier-meter measures classification errors
  classifier_meter trainmeter;
  classifier_meter testmeter;

  // initialize the network weights
  forget_param_linear fgp(1, 0.5, (int) 0 /* fixed seed */);
  trainable.forget(fgp);

  // gradient parameters
  gd_param gdp(/* double eta*/ eta,
	       /* double ln */ 	0.0,
	       /* double l1 */ 	0.0,
	       /* double l2 */ 	0.0,
	       /* intg dtime */ 	0,
	       /* double iner */0.0,
	       /* double anneal_value */ 0.001,
	       /* intg anneal_period */ 2000,
	       /* double g_t*/ 	0.0);
  cout << gdp << endl;
  infer_param infp;

  DEBUGMEM_PRETTY("before training");
  // estimate second derivative on 100 iterations, using mu=0.02
  // and set individual espilons
  // //printf("computing diagonal hessian and learning rates\n");
  thetrainer.compute_diaghessian(train_ds, 100, 0.02);

  EDEBUG(net.info());
//   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.985363,
// 			       idx_min(thetrainer.param.epsilons), 0.000001);
//   CPPUNIT_ASSERT_DOUBLES_EQUAL(49.851524,
// 			       idx_max(thetrainer.param.epsilons), 0.000001);


#ifdef __GUI__
  if (display) {
    stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
    // TODO: this causes compilation error now
    //    stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
  }
#endif

  // do training iterations
  cout << "training with " << train_ds.size() << " training samples and ";
  cout << test_ds.size() << " test samples" << endl;

  thetrainer.test(train_ds, trainmeter, infp);
  thetrainer.test(test_ds, testmeter, infp);
#ifdef __GUI__
  if (display) {
    stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
    // TODO: this causes compilation error now
    //    stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
  }
#endif
  // this goes at about 25 examples per second on a PIIIM 800MHz
  for (int i = 0; i < 5; ++i) {
    cout << "__ epoch " << i + 1 << " ______________"
	 << "_________________________________________________________" << endl;
    DEBUGMEM_PRETTY("before hessian");
    //thetrainer.compute_diaghessian(train_ds, 100, 0.02);
    thetrainer.train(train_ds, trainmeter, gdp, 1, infp, 1000, 100, .02);
    // ostringstream name(""); name << "pickings_" << i+1;
    // train_ds.save_pickings(name.str().c_str());
    thetrainer.test(train_ds, trainmeter, infp);
    thetrainer.test(test_ds, testmeter, infp);

#ifdef __GUI__
    if (display) {
      //      dsgui.display_pickings(train_ds, 3, 3);
      stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
      stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
    // TODO: this causes compilation error now
      //      stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
    }
#endif
  }
  CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0, // old: 97.00
			       ((trainmeter.total_correct * 100)
				/ (double) trainmeter.size), 1.5);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(95.4262, // 96.4630, // old: 95.90
			       ((testmeter.total_correct * 100)
				/ (double) testmeter.size), 1.5);
}
