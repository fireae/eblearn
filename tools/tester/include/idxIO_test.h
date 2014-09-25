#ifndef IDXIOTEST_H_
#define IDXIOTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "idxIO.h"

//! Test class for IdxIO class
class idxIO_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(idxIO_test);
  CPPUNIT_TEST(test_save_load_matrix_ubyte);
  CPPUNIT_TEST(test_save_load_matrix_int);
  CPPUNIT_TEST(test_save_load_matrix_float);
  CPPUNIT_TEST(test_save_load_matrix_double);
  CPPUNIT_TEST(test_save_load_matrix_long);
  CPPUNIT_TEST(test_save_load_matrix_matrix);
  CPPUNIT_TEST(test_save_load_matrices);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables

public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_save_load_matrix_ubyte();
  void test_save_load_matrix_int();
  void test_save_load_matrix_float();
  void test_save_load_matrix_double();
  void test_save_load_matrix_long();
  void test_save_load_matrix_matrix();
  void test_save_load_matrices();
};

#endif /* IDXIOTEST_H_ */
