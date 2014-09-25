#ifndef IMAGE_TEST_H_
#define IMAGE_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "libidx.h"

//! Test class for Image class
class image_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(image_test);
  CPPUNIT_TEST(test_deformations);
  CPPUNIT_TEST(test_resize);
  CPPUNIT_TEST(test_pnm_P3);
  CPPUNIT_TEST(test_pnm_P6);
  CPPUNIT_TEST(test_gaussian_pyramid);
  //  CPPUNIT_TEST(test_colorspaces);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_resize();
  void test_pnm_P3();
  void test_pnm_P6();
  void test_gaussian_pyramid();
  void test_deformations();
  void test_colorspaces();
};
#endif /*IMAGE_TEST_H_*/
