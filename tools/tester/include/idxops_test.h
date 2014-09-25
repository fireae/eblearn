#ifndef IDXOPSTEST_H_
#define IDXOPSTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "idxops.h"

//! Test class for Blas class
class idxops_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(idxops_test);
  CPPUNIT_TEST(test_idx_min_max);
  CPPUNIT_TEST(test_idx_sums);
  CPPUNIT_TEST(test_idx_sortdown);
  CPPUNIT_TEST(test_idx_sqrdist);
  CPPUNIT_TEST(test_idx_exp);
  CPPUNIT_TEST(test_idx_m2oversample);
  CPPUNIT_TEST(test_idx_m2squdotm1);
  CPPUNIT_TEST(test_idx_m2extm2acc);
  CPPUNIT_TEST(test_idx_m2dotm1);
  CPPUNIT_TEST(test_idx_copy);
  CPPUNIT_TEST(test_idx_copy2);
  CPPUNIT_TEST(test_idx_abs);
  CPPUNIT_TEST(test_huge_vec);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables

public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_idx_min_max();
  void test_idx_sums();
  void test_idx_sortdown();
  void test_idx_sqrdist();
  void test_idx_exp();
  void test_idx_m2oversample();
  void test_idx_m2squdotm1();
  void test_idx_m2extm2acc();
  void test_idx_m2dotm1();
  void test_idx_copy();
  void test_idx_copy2();
  void test_idx_abs();
  void test_huge_vec();
};

#endif /* IDXOPSTEST_H_ */
