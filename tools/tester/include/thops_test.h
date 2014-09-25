#ifndef THOPSTEST_H_
#define THOPSTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#ifdef __TH__
#include "thops.h"

//! Test class for th wrapper
class thops_test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(thops_test);
  CPPUNIT_TEST(test_th_add_float);
  CPPUNIT_TEST(test_th_add_double);
  CPPUNIT_TEST(test_th_convolve_float);
  CPPUNIT_TEST(test_th_convolve_double);
  CPPUNIT_TEST_SUITE_END();

public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_th_add_float();
  void test_th_add_double();
  void test_th_convolve_float();
  void test_th_convolve_double();

};

#endif /* __TH__ */

#endif /* THOPSTEST_H_ */
