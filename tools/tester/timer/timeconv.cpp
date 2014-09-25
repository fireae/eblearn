#include <cstdio>
#include <ctime>
#include <sys/time.h>
#include "libeblearn.h"
using namespace ebl;

typedef float t_net;

int main(int argc, char **argv) {
  if (argc < 3) {
    cerr << "Arguments : size n_tests n_examples size_param n_cores" << endl;
    return 0;
  }
  int size = atoi(argv[1]);
  int n_tests = atoi(argv[2]);
  intg n_examples = (intg)atoi(argv[3]); // maximum training set size: 60000
  int size_param = atoi(argv[4]);
  int n_cores = atoi(argv[5]);
  ipp_init(n_cores);
  init_drand(92394); // initialize random seed

  idxdim dims(1, size, size); // get order and dimensions of sample
  
  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  idx<t_net> targets = create_target_matrix<t_net>(10, 1.0);
  idx<t_net> inputs(n_examples, size, size);

  parameter<t_net> theparam(size_param); // create trainable parameter
  lenet5<t_net> l5(theparam, size, size, 5, 5, 2, 2, 5, 5, 2, 2, 120, 10);
    // TODO: use an all-to-all connection table in second layer convolution
    // Because that's what the other packages implement.
  supervised_euclidean_machine<t_net, ubyte> thenet(
          (module_1_1<t_net>&)l5,
          targets,
          dims);
  supervised_trainer<t_net, ubyte,ubyte> thetrainer(thenet, theparam);
  classifier_meter trainmeter, testmeter;
  forget_param_linear fgp(1, 0.5);
  thenet.forget(fgp);

  // learning parameters
  gd_param gdp(/* double leta*/ 0.0001,
         /* double ln */  0.0,
         /* double l1 */  0.0,
         /* double l2 */  0.0,
         /* int dtime */  0,
         /* double iner */0.0,
         /* double a_v */ 0.0,
         /* double a_t */ 0.0,
         /* double g_t*/  0.0);
  infer_param infp;

  state_idx<t_net> dummy_input(1, size, size); 
  int J = 12000;
  struct  timeval timer_init, timer_end;
  gettimeofday(&timer_init, NULL);
  for (intg j = 0; j < n_tests; ++j)
  {
    thetrainer.learn_sample(dummy_input, j%10, gdp);
    // TODO: iterate over mock dataset to simulate more realistic
    // memaccess pattern
  }
  gettimeofday(&timer_end, NULL);
  printf("%lld %06d %lld %06d\n",
	 (long long int)timer_init.tv_sec, (int)timer_init.tv_usec,
	 (long long int)timer_end.tv_sec,  (int)timer_end.tv_usec);
  return 0;
}
