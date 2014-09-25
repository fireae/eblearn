#include "ebl_basic_test.h"

using namespace std;
using namespace ebl;

void ebl_basic_test::setUp() {
}

void ebl_basic_test::tearDown() {
}

void ebl_basic_test::test_convolution_layer_fprop() {
  intg ini = 3;
  intg inj = 3;
  idxdim ker(2,2);
  idxdim stride(1,1);
  intg si = 1 + ini - ker.dim(0);
  intg sj = 1 + inj - ker.dim(1);
  state<double> in(1, ini, inj);
  state<double> out(1, si, sj);
  idx<intg> table(1, 2);
  idx_clear(table);
  idx<intg> tableout = table.select(1, 1);
  ddparameter<double> prm(10000);
  convolution_layer<double> c(&prm, ker, stride, table);
  double fact = 0.05;

  in.set(1, 0, 0, 0);
  in.set(2, 0, 0, 1);
  in.set(3, 0, 0, 2);
  in.set(4, 0, 1, 0);
  in.set(5, 0, 1, 1);
  in.set(6, 0, 1, 2);
  in.set(7, 0, 2, 0);
  in.set(8, 0, 2, 1);
  in.set(9, 0, 2, 2);
  c.convol.kernel.set(1 * fact, 0, 0, 0);
  c.convol.kernel.set(2 * fact, 0, 0, 1);
  c.convol.kernel.set(3 * fact, 0, 1, 0);
  c.convol.kernel.set(4 * fact, 0, 1, 1);
  c.adder.bias.set(-2.85, 0);

  c.fprop(in, out);

  // different values than c_layer because here we use tanh activation
  // function and c_layer uses the stdsigmoid.
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.761594, (out).get(0, 0, 0), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.462117, (out).get(0, 0, 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.462117, (out).get(0, 1, 0), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.761594, (out).get(0, 1, 1), 0.000001);
}

void ebl_basic_test::test_state_copy() {
  state<double> a(4,4);
  a.resize_dx();
  a.resize_ddx();

  dseed(32);
  idx_aloop3(xx,a,double,xd,a.dx[0],double,xdd,a.ddx[0],double){
    *xx = drand(2);
    *xd = drand(2);
    *xdd = drand(2);
  }

  state<double> b = a.deep_copy();
  //	a.pretty(std::cout);
  //	a.printElems(std::cout);
  //	b.pretty(std::cout);
  //	b.printElems(std::cout);
  CPPUNIT_ASSERT(0 == idx_sqrdist(a,b));
  CPPUNIT_ASSERT(0 == idx_sqrdist(a.dx[0],b.dx[0]));
  CPPUNIT_ASSERT(0 == idx_sqrdist(a.ddx[0],b.ddx[0]));

  idx_addc(a,1.,a);
  idx_addc(a.dx[0],2.,a.dx[0]);
  idx_addc(a.ddx[0],3.,a.ddx[0]);
  CPPUNIT_ASSERT(a.footprint() == idx_sqrdist(a,b));
  CPPUNIT_ASSERT(4*a.dx[0].footprint() == idx_sqrdist(a.dx[0],b.dx[0]));
  CPPUNIT_ASSERT(9*a.ddx[0].footprint() == idx_sqrdist(a.ddx[0],b.ddx[0]));
}


void ebl_basic_test::test_softmax(){
  CPPUNIT_ASSERT_MESSAGE("TODO: Implement automatic test", false);

  state<double> in(2,2,2,2,2,2);
  state<double> out(1,1,1,1,1,1);
  double beta = 1;
  softmax_module<double> module(beta);

  // init
  dseed(1);
  module.fprop(in, out);
  dynamic_init_drand();
  idx_bloop2(i, in, double, o, out, double){
    idx_bloop2(ii, i, double, oo, o, double){
      idx_bloop2(iii, ii, double, ooo, oo, double){
	idx_bloop2(iiii, iii, double, oooo, ooo, double){
          idx_bloop2(iiiii, iiii, double, ooooo, oooo, double){
            idx_bloop2(iiiiii, iiiii, double, oooooo, ooooo, double){
              iiiiii.set(drand((double)1));
              oooooo.set(drand((double)1));
            }
          }
	}
      }
    }
  }

  // fprop, bprop, bbprop
  module.fprop(in, out);
  in.zero_dx();
  module.bprop(in, out);
  in.zero_ddx();
  module.bbprop(in, out);

  /*print
    printf(" Input\n");
    in.pretty(stdout);
    printf(" Output\n");
    out.pretty(stdout);
    printf(" Input dx\n");
    in.dx.pretty(stdout);
    printf(" Output dx\n");
    out.dx.pretty(stdout);
    printf(" Input ddx\n");
    in.ddx.pretty(stdout);
    printf(" Output ddx\n");
    out.ddx.pretty(stdout);
    printf("\n");
  */
  idx<double> ib3 = in.select(0,0).select(0,0),
      calc_out = out.select(0,0).select(0,0);
  idx<double> ib(new srg<double>(), ib3.spec),
      des_out(new srg<double>(), ib3.spec);
  idx_dotc(ib3, module.beta, ib);
  idx_exp(ib);
  double ib2 = 1/idx_sum(ib);
  idx_dotc(ib, ib2, des_out);
  //printf("Fprop error 1 : %3.3e \n", idx_sqrdist(calc_out, des_out));

  ib3 = in.select(0,1).select(0,0);
  calc_out = out.select(0,1).select(0,0);
  idx_dotc(ib3, module.beta, ib);
  idx_exp(ib);
  ib2 = 1/idx_sum(ib);
  idx_dotc(ib, ib2, des_out);
  //printf("Fprop error 2 :  %3.3e \n", idx_sqrdist(calc_out, des_out));

  ib3 = in.select(0,0).select(0,1);
  calc_out = out.select(0,0).select(0,1);
  idx_dotc(ib3, module.beta, ib);
  idx_exp(ib);
  ib2 = 1/idx_sum(ib);
  idx_dotc(ib, ib2, des_out);
  //printf("Fprop error 3 : %3.3e \n", idx_sqrdist(calc_out, des_out));

  ib3 = in.select(0,1).select(0,1);
  calc_out = out.select(0,1).select(0,1);
  idx_dotc(ib3, module.beta, ib);
  idx_exp(ib);
  ib2 = 1/idx_sum(ib);
  idx_dotc(ib, ib2, des_out);
  //printf("Fprop error 4 :  %3.3e \n", idx_sqrdist(calc_out, des_out));
  /*
    Bprop_tester *bproptest = new Bprop_tester();
    bproptest->test(module);

    Bbprop_tester *bbproptest = new Bbprop_tester();
    bbproptest->test(module);

    Jacobian_tester *test= new Jacobian_tester();
    test->test(module);
  */
}

void ebl_basic_test::test_convolution_timing() {
  layers<double> l(true);
  idx<intg> tbl = full_table(1, 8);
  idx<intg> tbl2 = full_table(8, 16);
  idxdim ker(9,9);
  idxdim stride(1,1);
  l.add_module(new convolution_module<double>(NULL, ker, stride, tbl));
  l.add_module(new tanh_module<double>());
  l.add_module(new convolution_module<double>(NULL, ker, stride, tbl2));
  l.add_module(new tanh_module<double>());
  //convolution_module<double> l2(NULL, ker, stride, tbl);
  state<double> in(1, 512, 512), out(16, 496, 496);
  timer t;
  t.start();
  for (uint i = 0; i < 10; ++i) {
    l.fprop(in, out);
  }
  long tim = t.elapsed_milliseconds();
  cout << " big convolution time: " << tim/10 << "ms";
}

#define FLOAT_THRESHOLD 1e-3
#define DOUBLE_THRESHOLD 1e-5

#define TEST_DERIVATIVES(module, in, out, T, thresh) {	\
    module_tester<T> mt(thresh);			\
    TEST_DERIVATIVES_MT(mt, module, in, out) }

#define TEST_DERIVATIVES_MT(mtester, module, in, out)			\
  idx<double> errs = mtester.test_jacobian(module, in, out);            \
  EDEBUG("max err: " << errs.get(0) << " total err: " << errs.get(1));	\
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0, errs.get(0), mtester.get_acc_thres()); \
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0, errs.get(1), mtester.get_acc_thres()); \
  //   errs = mtester.test_hessian(s, in, out);
//   CPPUNIT_ASSERT(errs.get(0) < mtester.get_acc_thres());
//   CPPUNIT_ASSERT(errs.get(1) < mtester.get_acc_thres());

void ebl_basic_test::test_convolution_module_float() {
  typedef float T;
  idxdim ker(7,7);
  idxdim stride(1,1);
  state<T> in(2, 32, 32), out;
  idx<intg> table = full_table(2, 6);
  ddparameter<T> prm(100000);
  convolution_module<T> c(&prm, ker, stride, table);
  TEST_DERIVATIVES(c, in, out, T, FLOAT_THRESHOLD)

      // idxdim ker(2,2);
      // idxdim stride(1,1);
      // state<T> in(1, 2, 2), out;
      // idx<intg> table = full_table(1, 1);
      // ddparameter<T> prm(10000);
      // convolution_module<T> c(&prm, ker, stride, table);
      // TEST_DERIVATIVES(c, in, out, T, FLOAT_THRESHOLD)
      }

#ifdef __CUDA__
inline void test_conv_cuda(int inx, int iny, int infeat, int outfeat,
                           int kerx, int kery, bool full, int fanin,
                           int stridex, int stridey) {
  typedef float T;
  idxdim ker(kerx,kery);
  idxdim stride(stridex,stridey);
  state<T> in(infeat, inx, iny), out1, out2;
  idx<intg> table;
  if(full)
    table = full_table(infeat, outfeat);
  else {
    std::vector<intg> faninv;
    faninv.push_back(fanin);
    table = random_table(infeat, outfeat, faninv);
  }
  ddparameter<T> prm(100000);
  double rrange_min = -1.0;
  double rrange_max = 1.0;
  // cpu version
  convolution_module<T> c1(&prm, ker, stride, table, "cpuconv");
  cuda_convolution_module<T> c2(&prm, ker, stride, table, "gpuconv");
  forget_param_linear fp(2,0.5);
  idx_random(in, rrange_min, rrange_max); // randomize input for fprop
  c1.fprop(in,out1); // just to resize states
  c1.forget(fp); // randomize ddparameters if there are any
  c2.fprop(in,out2); // just to resize states
  c2.forget(fp); // randomize ddparameters if there are any
  idx_copy(c1.kernel, c2.kernel); // share the kernels between both modules
  // clear all input and output
  in.clear();
  out1.clear();
  out2.clear();
  idx_random(in, rrange_min, rrange_max); // randomize input for fprop
  c1.fprop(in, out1);
  c1.fprop(in, out1);
  c2.fprop(in, out2);
  c2.fprop(in, out2);
  double maxdist;
  // max distance
  idx_sub(out1,out2,out1);
  idx_abs(out1,out1);
  double totdist = idx_sum(out1);
  maxdist = (double) idx_max(out1);
  idx<double> errs(2);
  errs.set(maxdist, 0);
  errs.set(totdist, 1);
  // cout<<endl<<" FANIN: "<<fanin<<"\tTotal Distance:" << errs.get(0) <<
  // "\tMax Distance:" << errs.get(1) << endl;
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0, errs.get(0), 1e-5);           \
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0, errs.get(1), 100);            \
}

void ebl_basic_test::test_convolution_module_cuda() {
  // test_conv_cuda(16,16,1,1,3,3, true, 4,2,2);
  // test_conv_cuda(32,32,64,96,3,3, false, 4,1,1);
  // test_conv_cuda(64,64,64,96,3,3, false, 4,1,1);
  // test_conv_cuda(96,96,64,96,3,3, false, 4,1,1);

  // test_conv_cuda(16,16,64,96,5,5, false, 4,1,1);
  // test_conv_cuda(32,32,64,96,5,5, false, 4,1,1);
  // test_conv_cuda(64,64,64,96,5,5, false, 4,1,1);
  // test_conv_cuda(96,96,64,96,5,5, false, 4,1,1);

  // test_conv_cuda(16,16,64,96,7,7, false, 4,1,1);

  // // test_conv_cuda(64,64,64,96,7,7, true,1,1);
  // test_conv_cuda(64,64,64,96,7,7, false, 4,1,1);

  // test_conv_cuda(64,64,64,96,7,7, false, 4,1,1);
  // test_conv_cuda(96,96,64,96,7,7, false, 4,1,1);

  // test_conv_cuda(16,16,64,96,9,9, false, 4,1,1);
  // test_conv_cuda(32,32,64,96,9,9, false, 4,1,1);
  // test_conv_cuda(64,64,64,96,9,9, false, 4,1,1);
  // test_conv_cuda(96,96,64,96,9,9, false, 4,1,1);
  //  test_conv_cuda(500,500,64,96,5,5, false, 1,2,2);
  test_conv_cuda(432,642,37,45,5,5, false, 2,3,3);
  // test_conv_cuda(823,495,23,74,5,5, false, 3,4,4);
  // test_conv_cuda(384,435,64,96,5,5, false, 4,5,5);
  // test_conv_cuda(536,349,64,96,5,5, false, 5,1,1);
  // test_conv_cuda(536,349,64,96,5,5, false, 6,1,1);
  // test_conv_cuda(536,349,64,96,5,5, false, 7,1,1);
  // test_conv_cuda(536,349,64,96,5,5, false, 8,1,1);
  // test_conv_cuda(536,349,64,96,5,5, false, 9,1,1);

}

#endif

void ebl_basic_test::test_convolution_module_double() {
  typedef double T;
  // idxdim ker(7,7);
  // idxdim stride(1,1);
  // state<T> in(2, 10, 10), out;
  // idx<intg> table = full_table(2, 3);
  // ddparameter<T> prm(10000);
  // convolution_module<T> c(&prm, ker, stride, table);
  // TEST_DERIVATIVES(c, in, out, T, DOUBLE_THRESHOLD)

  idxdim ker(2,2);
  idxdim stride(1,1);
  state<T> in(1, 2, 2), out;
  idx<intg> table = full_table(1, 1);
  ddparameter<T> prm(10000);
  convolution_module<T> c(&prm, ker, stride, table);
  TEST_DERIVATIVES(c, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_subsampling_module_float() {
  typedef float T;
  ddparameter<T> p(10000);
  idxdim kd(4, 4), sd(2, 2);
  subsampling_module<T> s(&p, 2, kd, sd);
  state<T> in(2, 8, 8), out;
  TEST_DERIVATIVES(s, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_subsampling_module_double() {
  typedef double T;
  ddparameter<T> p(10000);
  idxdim kd(4, 4), sd(2, 2);
  subsampling_module<T> s(&p, 2, kd, sd);
  state<T> in(2, 8, 8), out;
  TEST_DERIVATIVES(s, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_wavg_pooling_module_float() {
  typedef double T;
  ddparameter<T> p(10000);
  idxdim kd(5, 5), sd(3, 3);
  wavg_pooling_module<T> s(2, kd, sd);
  state<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(s, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_wavg_pooling_module_double() {
  typedef double T;
  ddparameter<T> p(10000);
  idxdim kd(5, 5), sd(3, 3);
  wavg_pooling_module<T> s(2, kd, sd);
  state<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(s, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_l2pooling_module_float() {
  typedef double T;
  ddparameter<T> p(10000);
  idxdim kd(5, 5), sd(3, 3);
  lppooling_module<T> s(2, kd, sd);
  state<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(s, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_l2pooling_module_double() {
  typedef double T;
  ddparameter<T> p(10000);
  idxdim kd(5, 5), sd(3, 3);
  lppooling_module<T> s(2, kd, sd);
  state<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(s, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_sqrt_power_module_float() {
  typedef float T;
  // test by hand
  state<T> in(1);
  state<T> out(1);
  power_module<T> pw(.5);
  in.resize_dx();
  in.resize_ddx();
  out.resize_dx();
  out.resize_ddx();
  in.zero_all();
  out.zero_all();
  in.set(2, 0);
  out.dx[0].set(1, 0);
  out.ddx[0].set(1, 0);
  pw.fprop(in, out);
  pw.bprop(in, out);
  pw.bbprop(in, out);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.4142, out.get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3536, in.dx[0].get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.125, in.ddx[0].get(0), 0.0001);
  // sqrt
  { ddparameter<T> p(10000);
    power_module<T> m(.5);
    state<T> in(2, 11, 11), out;
    module_tester<T> mt(FLOAT_THRESHOLD, 1e-4, 2);
    TEST_DERIVATIVES_MT(mt, m, in, out)
	}
}

void ebl_basic_test::test_sqrt_power_module_double() {
  // test by hand
  state<double> in(1);
  state<double> out(1);
  power_module<double> pw(.5);
  in.resize_dx();
  in.resize_ddx();
  out.resize_dx();
  out.resize_ddx();
  in.zero_all();
  out.zero_all();
  in.set(2, 0);
  out.dx[0].set(1, 0);
  out.ddx[0].set(1, 0);
  pw.fprop(in, out);
  pw.bprop(in, out);
  pw.bbprop(in, out);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.4142, out.get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3536, in.dx[0].get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.125, in.ddx[0].get(0), 0.0001);
  // sqrt
  { typedef double T;
    ddparameter<T> p(10000);
    power_module<T> m(.5);
    state<T> in(2, 11, 11), out;
    module_tester<T> mt(DOUBLE_THRESHOLD, 1e-4, 2);
    TEST_DERIVATIVES_MT(mt, m, in, out)
	}
}

void ebl_basic_test::test_power2_module_float() {
  // square
  typedef float T;
  ddparameter<T> p(10000);
  power_module<T> m(2);
  state<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_power2_module_double() {
  // square
  typedef double T;
  ddparameter<T> p(10000);
  power_module<T> m(2);
  state<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_power_inv_module_float() {
  // inverse
  typedef float T;
  ddparameter<T> p(10000);
  power_module<T> m(-1);
  state<T> in(2, 11, 11), out;
  module_tester<T> mt(FLOAT_THRESHOLD, .1, 2);
  TEST_DERIVATIVES_MT(mt, m, in, out)
      }

void ebl_basic_test::test_power_inv_module_double() {
  // inverse
  typedef double T;
  ddparameter<T> p(10000);
  power_module<T> m(-1);
  state<T> in(2, 11, 11), out;
  module_tester<T> mt(DOUBLE_THRESHOLD, .1, 2);
  TEST_DERIVATIVES_MT(mt, m, in, out)
      }

void ebl_basic_test::test_convolution_layer() {
  intg ini = 3;
  intg inj = 3;
  idxdim ker(2,2);
  idxdim stride(1,1);
  intg si = 1 + ini - ker.dim(0);
  intg sj = 1 + inj - ker.dim(1);
  state<double> in(1, ini, inj);
  state<double> out(1, si, sj);
  idx<intg> table(1, 2);
  idx_clear(table);
  idx<intg> tableout = table.select(1, 1);
  ddparameter<double> prm(10000);
  convolution_layer<double> c(&prm, ker, stride, table);
  TEST_DERIVATIVES(c, in, out, double, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_subsampling_layer() {
  ddparameter<double> p(10000);
  idxdim kd(4, 4), sd(2, 2);
  subsampling_layer<double> s(&p, 1, kd, sd);
  state<double> in(1, 8, 8);
  state<double> out(1, 1, 1);
  TEST_DERIVATIVES(s, in, out, double, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_addc_module_float() {
  typedef float T;
  ddparameter<T> p(10000);
  addc_module<T> m(&p, 10);
  state<T> in(10, 11, 11), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_addc_module_double() {
  typedef double T;
  ddparameter<T> p(10000);
  addc_module<T> m(&p, 10);
  state<T> in(10, 11, 11), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_linear_module_float() {
  typedef float T;
  ddparameter<T> p(10000);
  linear_module<T> m(&p, 10, 100);
  state<T> in(10, 1, 1), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_linear_module_double() {
  typedef double T;
  ddparameter<T> p(10000);
  linear_module<T> m(&p, 10, 100);
  state<T> in(10, 1, 1), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_subtractive_norm_module_double() {
  typedef double T;
  ddparameter<T> p(10000);
  idxdim kd(7, 7);
  // subtractive_norm_module<T> m1(kd, 2);
  subtractive_norm_module<T> m2(kd, 2, false, false, NULL, "", false);
  // subtractive_norm_module<T> m2(kd, 2, false);
  // subtractive_norm_module<T> m3(kd, 2, true, false);
  state<T> in(2, 11, 11), out;
  // TEST_DERIVATIVES(m1, in, out, T, DOUBLE_THRESHOLD)
  TEST_DERIVATIVES(m2, in, out, T, DOUBLE_THRESHOLD)
      // TEST_DERIVATIVES(m3, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_divisive_norm_module_double() {
  typedef double T;
  ddparameter<T> p(10000);
  idxdim kd(7, 7);
  divisive_norm_module<T> m1(kd, 2, false, false, NULL, "", false);
  divisive_norm_module<T> m2(kd, 2, true, false, NULL, "", false);
  divisive_norm_module<T> m3(kd, 2, true, false, NULL, "", true);
  state<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(m1, in, out, T, DOUBLE_THRESHOLD)
      TEST_DERIVATIVES(m2, in, out, T, DOUBLE_THRESHOLD)
      TEST_DERIVATIVES(m3, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_contrast_norm_module_double() {
  typedef double T;
  ddparameter<T> p(10000);
  idxdim kd(7, 7);
  contrast_norm_module<T> m1(kd, 2, false, false, false, NULL, "", false);
  contrast_norm_module<T> m2(kd, 2, false, false, false, NULL, "", true);
  state<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(m1, in, out, T, DOUBLE_THRESHOLD)
      TEST_DERIVATIVES(m2, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_fsum_module_float() {
  typedef float T;
  fsum_module<T> m;
  state<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_fsum_module_double() {
  typedef double T;
  fsum_module<T> m1(false);
  fsum_module<T> m2(true);
  fsum_module<T> m3(false, 3);
  fsum_module<T> m4(true, 3);
  state<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m1, in, out, T, DOUBLE_THRESHOLD)
      TEST_DERIVATIVES(m2, in, out, T, DOUBLE_THRESHOLD)
      TEST_DERIVATIVES(m3, in, out, T, DOUBLE_THRESHOLD)
      TEST_DERIVATIVES(m4, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_zpad_module_float() {
  typedef float T;
  idxdim d(7, 7);
  zpad_module<T> m(d);
  state<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_zpad_module_double() {
  typedef double T;
  idxdim d(7, 7);
  zpad_module<T> m(d);
  state<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_mirrorpad_module_float() {
  typedef float T;
  idxdim d(7, 7);
  mirrorpad_module<T> m(d);
  state<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_mirrorpad_module_double() {
  typedef double T;
  idxdim d(7, 7);
  mirrorpad_module<T> m(d);
  state<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_thres_module_float() {
  typedef float T;
  thres_module<T> m(.2, .5);
  state<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
      }

void ebl_basic_test::test_thres_module_double() {
  typedef double T;
  thres_module<T> m(.2, .5);
  state<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
      }

void ebl_basic_test::test_tanh_shrink_module_float() {
  eblerror("not implemented");
}

void ebl_basic_test::test_tanh_shrink_module_double() {
  typedef double T;
  tanh_shrink_module<T> m(NULL, 5);
  state<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
      }
