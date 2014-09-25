#ifndef GUI3D_TEST_H_
#define GUI3D_TEST_H_

#include <cppunit/extensions/HelperMacros.h>

//! Test class for Ebm class
class gui3d_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(gui3d_test);
  CPPUNIT_TEST(test_all);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables

public:
  // Test functions
  void test_all();
};

#endif /* GUI3D_TEST_H_ */
