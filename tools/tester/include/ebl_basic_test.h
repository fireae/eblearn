#ifndef EBL_BASIC_TEST_H_
#define EBL_BASIC_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "ebl_basic.h"
#include "ebl_layers.h"
#include "ebl_tester.h"
#include "ebl_pooling.h"

//! Test class for Ebm class
class ebl_basic_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(ebl_basic_test);
  /* CPPUNIT_TEST(test_full_table); */
  /* CPPUNIT_TEST(test_softmax); */
  /* CPPUNIT_TEST(test_state_copy); */
  /* CPPUNIT_TEST(test_Ebm01); */
  /* CPPUNIT_TEST(test_Ebm02); */

  // layers test
  // CPPUNIT_TEST(test_convolution_layer_fprop);
  // CPPUNIT_TEST(test_convolution_layer);
  // CPPUNIT_TEST(test_subsampling_layer);
  
  // float tests
  //CPPUNIT_TEST(test_addc_module_float);
  // CPPUNIT_TEST(test_convolution_module_float);
  //CPPUNIT_TEST(test_subsampling_module_float);
  /* CPPUNIT_TEST(test_wavg_pooling_module_float); */
  /* CPPUNIT_TEST(test_l2pooling_module_float); */
  /* CPPUNIT_TEST(test_sqrt_power_module_float); */
  /* CPPUNIT_TEST(test_power2_module_float); */
  /* CPPUNIT_TEST(test_power_inv_module_float); */
  /* CPPUNIT_TEST(test_linear_module_float); */
  /* //  CPPUNIT_TEST(test_subtractive_norm_module_float); */
  /* //CPPUNIT_TEST(test_divisive_norm_module_float); */
  /* //CPPUNIT_TEST(test_contrast_norm_module_float); */
  /* CPPUNIT_TEST(test_fsum_module_float); */
  /* CPPUNIT_TEST(test_zpad_module_float); */
  /* CPPUNIT_TEST(test_mirrorpad_module_float); */
  /* CPPUNIT_TEST(test_thres_module_float); */
  /* CPPUNIT_TEST(test_tanh_shrink_module_float); */
#ifdef __CUDA__
  CPPUNIT_TEST(test_convolution_module_cuda);
#endif
  // double tests
  // CPPUNIT_TEST(test_addc_module_double); //not working
  CPPUNIT_TEST(test_convolution_module_double);
  // CPPUNIT_TEST(test_subsampling_module_double); //not working

  //CPPUNIT_TEST(test_wavg_pooling_module_double); //segfaults
  CPPUNIT_TEST(test_l2pooling_module_double); //inconsistent tensor size?
  CPPUNIT_TEST(test_sqrt_power_module_double);
  CPPUNIT_TEST(test_power2_module_double);
  CPPUNIT_TEST(test_power_inv_module_double);
  CPPUNIT_TEST(test_linear_module_double);
  CPPUNIT_TEST(test_subtractive_norm_module_double);
  CPPUNIT_TEST(test_divisive_norm_module_double);
  CPPUNIT_TEST(test_contrast_norm_module_double);
  CPPUNIT_TEST(test_fsum_module_double);
  CPPUNIT_TEST(test_zpad_module_double);
  //CPPUNIT_TEST(test_mirrorpad_module_double); //not working
  CPPUNIT_TEST(test_thres_module_double);
  CPPUNIT_TEST(test_tanh_shrink_module_double);

  //  CPPUNIT_TEST(test_softmax); // TODO: fix test
  CPPUNIT_TEST(test_state_copy);
  
  CPPUNIT_TEST(test_convolution_timing); 
  
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables

public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_convolution_layer_fprop();
/*   void test_full_table(); */
/*   void test_softmax(); */
/*   void test_state_copy(); */
/*   void test_Ebm01(); */
/*   void test_Ebm02(); */
  void test_softmax();
  void test_state_copy();
  void test_convolution_timing();
  void test_convolution_module_float();
  void test_convolution_module_cuda();
  void test_convolution_module_double();
  void test_subsampling_module_float();
  void test_subsampling_module_double();
  void test_wavg_pooling_module_float();
  void test_wavg_pooling_module_double();
  void test_l2pooling_module_float();
  void test_l2pooling_module_double();
  void test_sqrt_power_module_float();
  void test_sqrt_power_module_double();
  void test_power2_module_float();
  void test_power2_module_double();
  void test_power_inv_module_float();
  void test_power_inv_module_double();
  void test_convolution_layer();
  void test_subsampling_layer();
  void test_addc_module_float();
  void test_addc_module_double();
  void test_linear_module_float();
  void test_linear_module_double();
  /* void test_subtractive_norm_module_float(); */
  void test_subtractive_norm_module_double();
  /* void test_divisive_norm_module_float(); */
  void test_divisive_norm_module_double();
  void test_contrast_norm_module_float();
  void test_contrast_norm_module_double();
  void test_fsum_module_float();
  void test_fsum_module_double();
  void test_zpad_module_float();
  void test_zpad_module_double();
  void test_mirrorpad_module_float();
  void test_mirrorpad_module_double();
  void test_thres_module_float();
  void test_thres_module_double();
  void test_tanh_shrink_module_float();
  void test_tanh_shrink_module_double();
};

#endif /* EBL_BASIC_TEST_H_ */
