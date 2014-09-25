#ifndef DATASOURCE_TEST_H_
#define DATASOURCE_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "datasource.h"

//! Test class for IdxIO class
class datasource_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(datasource_test);
  //  CPPUNIT_TEST(test_mnist_LabeledDataSource); // TODO: fix test
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_mnist_LabeledDataSource();
};

#endif /* DATASOURCE_TEST_H_ */
