#ifndef DETECTOR_TEST_H_
#define DETECTOR_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "libeblearn.h"

//! Test class for Classifier2D class
class detector_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(detector_test);
  CPPUNIT_TEST(test_face);
  /* CPPUNIT_TEST(test_norb); */
  //CPPUNIT_TEST(test_norb_binoc);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_face();
  //  void test_norb();
  void test_norb_binoc();
};

#endif /* DETECTOR_TEST_H_ */
