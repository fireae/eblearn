#ifndef IDXOPSTEST2_H_
#define IDXOPSTEST2_H_

#include <cppunit/extensions/HelperMacros.h>
#include "idxops.h"

//! Test class for most of idxops
class idxops_test2 : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(idxops_test2);
  CPPUNIT_TEST(idx_copy2);
  CPPUNIT_TEST(idx_clear2);
  CPPUNIT_TEST(idx_fill2);
  CPPUNIT_TEST(idx_add2);
  CPPUNIT_TEST(idx_addc2);
  CPPUNIT_TEST(idx_addc_bounded2);
  CPPUNIT_TEST(idx_sub2);
  CPPUNIT_TEST(idx_subc_bounded2);
  CPPUNIT_TEST(idx_minus2);
  CPPUNIT_TEST(idx_mul2);
  CPPUNIT_TEST(idx_dotc2);
  CPPUNIT_TEST(idx_dotc_bounded2);
  CPPUNIT_TEST(idx_signdotc2);
  CPPUNIT_TEST(idx_div2);
  CPPUNIT_TEST(idx_inv2);
  CPPUNIT_TEST(idx_signdotcacc2);
  CPPUNIT_TEST(idx_subsquare2);
  CPPUNIT_TEST(idx_lincomb2);
  CPPUNIT_TEST(idx_tanh2);
  CPPUNIT_TEST(idx_dtanh2);
  CPPUNIT_TEST(idx_stdsigmoid2);
  CPPUNIT_TEST(idx_dstdsigmoid2);
  CPPUNIT_TEST(idx_abs2);
  CPPUNIT_TEST(idx_thresdotc_acc2);
  CPPUNIT_TEST(idx_threshold2);
  CPPUNIT_TEST(idx_sqrt2);
  CPPUNIT_TEST(idx_exp2);
  CPPUNIT_TEST(idx_power2);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables

public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void idx_copy2();
  void idx_clear2();
  void idx_fill2();
  void idx_add2();
  void idx_addc2();
  void idx_addc_bounded2();
  void idx_sub2();
  void idx_subc_bounded2();
  void idx_minus2();
  void idx_mul2();
  void idx_dotc2();
  void idx_dotc_bounded2();
  void idx_signdotc2();
  void idx_div2();
  void idx_inv2();
  void idx_signdotcacc2();
  void idx_subsquare2();
  void idx_lincomb2();
  void idx_tanh2();
  void idx_dtanh2();
  void idx_stdsigmoid2();
  void idx_dstdsigmoid2();
  void idx_abs2();
  void idx_thresdotc_acc2();
  void idx_threshold2();
  void idx_sqrt2();
  void idx_exp2();
  void idx_power2();
};

#endif /* IDXOPSTEST2_H_ */
