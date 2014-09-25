#ifndef IDXITER_TEST_H_
#define IDXITER_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "idx.h"
#include "idxiter.h"

//! Test class for idxiter class
class idxiter_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(idxiter_test);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
};

#endif /* IDXITER_TEST_H_ */
