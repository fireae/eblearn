#ifndef IPPOPSTEST_H_
#define IPPOPSTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "ippops.h"

#ifdef __IPP__

//! Test class for ipp wrapper
class ippops_test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ippops_test);
  CPPUNIT_TEST(test_ipp_convolve_float); 
  CPPUNIT_TEST(test_ipp_convolve_ubyte);
  CPPUNIT_TEST(test_ipp_convolve_int16);
  CPPUNIT_TEST(test_ipp_copy);
  CPPUNIT_TEST(test_ipp_clear);
  CPPUNIT_TEST(test_ipp_fill);
  CPPUNIT_TEST(test_ipp_minus);
  CPPUNIT_TEST(test_ipp_add_float);
  CPPUNIT_TEST(test_ipp_add_ubyte);
  CPPUNIT_TEST(test_ipp_add_uint16);
  CPPUNIT_TEST(test_ipp_add_int16);
  CPPUNIT_TEST(test_ipp_addc_float);
  CPPUNIT_TEST(test_ipp_addc_ubyte);
  CPPUNIT_TEST(test_ipp_addc_uint16);
  CPPUNIT_TEST(test_ipp_addc_int16);
  CPPUNIT_TEST(test_ipp_bounded);
  CPPUNIT_TEST(test_ipp_sub_float);
  CPPUNIT_TEST(test_ipp_sub_ubyte);
  CPPUNIT_TEST(test_ipp_sub_uint16);
  CPPUNIT_TEST(test_ipp_sub_int16);
  CPPUNIT_TEST(test_ipp_mul_float);
  CPPUNIT_TEST(test_ipp_mul_ubyte);
  CPPUNIT_TEST(test_ipp_mul_uint16);
  CPPUNIT_TEST(test_ipp_mul_int16);
  CPPUNIT_TEST(test_ipp_dotc_float);
  CPPUNIT_TEST(test_ipp_dotc_ubyte);
  CPPUNIT_TEST(test_ipp_dotc_uint16);
  CPPUNIT_TEST(test_ipp_dotc_int16);
  CPPUNIT_TEST(test_ipp_div_float);
  CPPUNIT_TEST(test_ipp_div_ubyte);
  CPPUNIT_TEST(test_ipp_div_uint16);
  CPPUNIT_TEST(test_ipp_div_int16);
  CPPUNIT_TEST(test_ipp_inv);
  CPPUNIT_TEST(test_ipp_abs);
  CPPUNIT_TEST(test_ipp_sqrt);
  CPPUNIT_TEST(test_ipp_exp);
  CPPUNIT_TEST(test_ipp_sum);
  CPPUNIT_TEST(test_ipp_sumabs);
  CPPUNIT_TEST(test_ipp_l2norm);
  CPPUNIT_TEST(test_ipp_mean);
  CPPUNIT_TEST(test_ipp_std_normalize);
#ifdef __IPP_DOT__
  CPPUNIT_TEST(test_ipp_dot);
#endif
  CPPUNIT_TEST(test_ipp_max);
  CPPUNIT_TEST(test_ipp_min);
  CPPUNIT_TEST(test_ipp_maxevery);
  CPPUNIT_TEST(test_ipp_sqrdist);
  CPPUNIT_TEST(test_ipp_threshold);
  CPPUNIT_TEST_SUITE_END();

public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_ipp_convolve_float();
  void test_ipp_convolve_ubyte();
  void test_ipp_convolve_int16();
  void test_ipp_copy();
  void test_ipp_clear();
  void test_ipp_fill();
  void test_ipp_minus();
  void test_ipp_add_float();
  void test_ipp_add_ubyte();
  void test_ipp_add_uint16();
  void test_ipp_add_int16();
  void test_ipp_addc_float();
  void test_ipp_addc_ubyte();
  void test_ipp_addc_uint16();
  void test_ipp_addc_int16();
  void test_ipp_bounded();
  void test_ipp_sub_float();
  void test_ipp_sub_ubyte();
  void test_ipp_sub_int16();
  void test_ipp_sub_uint16();
  void test_ipp_mul_float();
  void test_ipp_mul_ubyte();
  void test_ipp_mul_int16();
  void test_ipp_mul_uint16();
  void test_ipp_dotc_float();
  void test_ipp_dotc_ubyte();
  void test_ipp_dotc_int16();
  void test_ipp_dotc_uint16();
  void test_ipp_div_float();
  void test_ipp_div_ubyte();
  void test_ipp_div_int16();
  void test_ipp_div_uint16();
  void test_ipp_inv();
  void test_ipp_abs();
  void test_ipp_sqrt();
  void test_ipp_exp();
  void test_ipp_sumabs();
  void test_ipp_sum();
  void test_ipp_l2norm();
  void test_ipp_mean();
  void test_ipp_std_normalize();
#ifdef __IPP_DOT__
  void test_ipp_dot();
#endif
  void test_ipp_max();
  void test_ipp_min();
  void test_ipp_maxevery();
  void test_ipp_sqrdist();
  void test_ipp_threshold();
};

#endif /* __IPP__ */

#endif /* IPPOPSTEST_H_ */
