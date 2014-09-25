#ifndef IDXTEST_
#define IDXTEST_

#include <cppunit/extensions/HelperMacros.h>
#include "idx.h"

//! Test class for IdxIO class
class idx_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(idx_test);
  CPPUNIT_TEST(test_Idx_get);
  CPPUNIT_TEST(test_Srg);
  CPPUNIT_TEST(test_IdxSpec);
  CPPUNIT_TEST(test_Idx_operations);
  CPPUNIT_TEST(test_Idx_resize);
  CPPUNIT_TEST(test_IdxIter);
  CPPUNIT_TEST(test_IdxIter2);
  CPPUNIT_TEST(test_Idx_macros);
  CPPUNIT_TEST(test_view_as_order);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_Idx_get();
  void test_Srg();
  void test_IdxSpec();
  void test_Idx_operations();
  void test_Idx_resize();
  void test_IdxIter();
  void test_IdxIter2();
  void test_Idx_macros();
  void test_view_as_order();
};
#endif /*IDXTEST_*/
