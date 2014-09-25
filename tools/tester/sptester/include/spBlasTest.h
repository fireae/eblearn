/***************************************************************************
 *   Copyright (C) 2008 by Cyril Poulet   *
 *   cyril.poulet@centraliens.net   *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef SPBLASTEST_H_
#define SPBLASTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "libspidx.h"

using namespace std;
using namespace ebl;

//! Test class for spIdx class
class spBlasTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(spBlasTest);
  CPPUNIT_TEST(test_copy);
  CPPUNIT_TEST(test_copy2);
  CPPUNIT_TEST(test_copy3);
  CPPUNIT_TEST(test_clear);
  CPPUNIT_TEST(test_minus);
  CPPUNIT_TEST(test_inv);
  CPPUNIT_TEST(test_add);
  CPPUNIT_TEST(test_sub);
  CPPUNIT_TEST(test_mul);
  CPPUNIT_TEST(test_addc);
  CPPUNIT_TEST(test_addcacc);
  CPPUNIT_TEST(test_dotc);
  CPPUNIT_TEST(test_dotcacc);
  CPPUNIT_TEST(test_subsquare);
  CPPUNIT_TEST(test_lincomb);
  CPPUNIT_TEST(test_abs);
  CPPUNIT_TEST(test_indexmax);
  CPPUNIT_TEST(test_sqrdist);
  CPPUNIT_TEST(test_m2dotm1);
  CPPUNIT_TEST(test_m2dotm1sp);
  CPPUNIT_TEST(test_m2dotm1acc);
  CPPUNIT_TEST(test_m2dotm1accsp);
  //! the following tests are working fine but haven't been
  //! implemented with CPPUNIT_ASSERT, so you'll have to verify
  //! by hand if you uncomment them
  //CPPUNIT_TEST(test_m2extm2);
  //CPPUNIT_TEST(test_m2extm2acc);
  //CPPUNIT_TEST(test_m2squextm2acc);
  //CPPUNIT_TEST(test_m2squdotm2);
  //CPPUNIT_TEST(test_m2squdotm2acc);
  //CPPUNIT_TEST(test_m1extm1);
  //CPPUNIT_TEST(test_m1extm1acc);
  //CPPUNIT_TEST(test_m2squdotm1);
  //CPPUNIT_TEST(test_m2squdotm1sp);
  //CPPUNIT_TEST(test_m2squdotm1acc);
  //CPPUNIT_TEST(test_m2squdotm1accsp);
  //CPPUNIT_TEST(test_m1squextm1);
  //CPPUNIT_TEST(test_m1squextm1acc);
  //CPPUNIT_TEST(test_normcolumn);
  //CPPUNIT_TEST(test_2dconvol);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables

public:
  //! This function is called before each test function is called.
  void setUp(){};
  //! This function is called after each test function is called.
  void tearDown(){};

  // Test functions
  void test_copy();
  void test_copy2();
  void test_copy3();
  void test_clear();
  void test_minus();
  void test_inv();
  void test_add();
  void test_sub();
  void test_mul();
  void test_addc();
  void test_addcacc();
  void test_dotc();
  void test_dotcacc();
  void test_subsquare();
  void test_lincomb();
  void test_abs();
  void test_indexmax();
  void test_sqrdist();
  void test_m2dotm1();
  void test_m2dotm1sp();
  void test_m2dotm1acc();
  void test_m2dotm1accsp();
  void test_m2extm2();
  void test_m2extm2acc();
  void test_m2squextm2acc();
  void test_m2squdotm2();
  void test_m2squdotm2acc();
  void test_m1extm1();
  void test_m1extm1acc();
  void test_m2squdotm1();
  void test_m2squdotm1sp();
  void test_m2squdotm1acc();
  void test_m2squdotm1accsp();
  void test_m1squextm1();
  void test_m1squextm1acc();
  void test_normcolumn();

  void test_2dconvol();

};
#endif /* SPBLASTEST_H_ */
