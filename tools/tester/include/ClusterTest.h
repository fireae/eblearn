#ifndef CLUSTERTEST_H_
#define CLUSTERTEST_H_

#include <cppunit/extensions/HelperMacros.h>
//#include "KMeans.h"

//! Test class for Cluster methods
class ClusterTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(ClusterTest);
  //  CPPUNIT_TEST(test_ints);
  //  CPPUNIT_TEST(test_doubles);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables

public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_sanity();
  void test_ints();
  void test_doubles();
};

#endif /* CLUSTERTEST_H_ */
