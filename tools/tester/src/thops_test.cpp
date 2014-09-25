#include "thops_test.h"

#ifdef __TH__

#include "idxops.h"
#include <cstdio>
#include <cmath>
#include <limits>

using namespace ebl;

bool approx_equals(float a, float b, float err = 1e-5) {
  if (std::abs(a) == std::numeric_limits<float>::infinity() ||
      std::abs(b) == std::numeric_limits<float>::infinity())
    return a == b;
  else if (std::abs(a) > 1e-10 && std::abs(b) > 1e-10)
    return (1.0f - err < a / b) && (a / b < 1.0f + err);
  else
    return (a - err < b) && (b < a + err);
}

bool approx_equals(double a, double b, double err = 1e-5) {
  if (std::abs(a) == std::numeric_limits<double>::infinity() ||
      std::abs(b) == std::numeric_limits<double>::infinity())
    return a == b;
  else if (std::abs(a) > 1e-10 && std::abs(b) > 1e-10)
    return (1.0f - err < a / b) && (a / b < 1.0f + err);
  else
    return (a - err < b) && (b < a + err);
}

void thops_test::setUp() {
}

void thops_test::tearDown() {
}

// tests for th_add

void thops_test::test_th_add_float() {
  int nx = 15, ny = 42;
  idx<float> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 7561) * 1.43 - 4000, i, j);
      B.set(((184993 * i + 15493 * j) % 2459) * 2.31 - 2000, i, j);
    }
  idx<float> AB(nx, ny), AB2(nx, ny);
  idx_copy(B, AB);
  th_add(A, AB);
  th_add(A, B, AB2);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(approx_equals(AB.get(i, j), A.get(i, j) + B.get(i, j)));
      CPPUNIT_ASSERT(approx_equals(AB2.get(i, j), A.get(i, j) + B.get(i, j)));
    }
}

void thops_test::test_th_add_double() {
  int nx = 15, ny = 42;
  idx<double> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 7561) * 1.43 - 4000, i, j);
      B.set(((184993 * i + 15493 * j) % 2459) * 2.31 - 2000, i, j);
    }
  idx<double> AB(nx, ny), AB2(nx, ny);
  idx_copy(B, AB);
  th_add(A, AB);
  th_add(A, B, AB2);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(approx_equals(AB.get(i, j), A.get(i, j) + B.get(i, j)));
      CPPUNIT_ASSERT(approx_equals(AB2.get(i, j), A.get(i, j) + B.get(i, j)));
    }
}

// tests for th_convolution
void thops_test::test_th_convolve_float() {
  idx<float> A(4, 4);
  idx_clear(A);
  A.set(1., 0, 0);
  A.set(3., 1, 0);
  A.set(2., 2, 0);
  A.set(0., 3, 0);
  A.set(2., 0, 1);
  A.set(3., 1, 1);
  A.set(6., 2, 1);
  A.set(0., 3, 1);
  A.set(3., 0, 2);
  A.set(-1., 1, 2);
  A.set(3., 2, 2);
  A.set(4., 3, 2);
  A.set(-1., 0, 3);
  A.set(0., 1, 3);
  A.set(-2., 2, 3);
  A.set(-2., 3, 3);
  idx<float> ker(3, 3);
  idx_clear(ker);
  ker.set(1., 0, 0);
  ker.set(2., 1, 0);
  ker.set(3., 2, 0);
  ker.set(0., 0, 1);
  ker.set(-1., 1, 1);
  ker.set(3.5, 2, 1);
  ker.set(3., 0, 2);
  ker.set(2.2, 1, 2);
  ker.set(7., 2, 2);
  idx<float> B1(2, 2), B2(2, 2);
  //idx<float> revker(3, 3);
  //rev_idx2_tr(ker, revker);
  th_convolution(A, ker, B1);
  idx_2dconvol(A, ker, B2);
  idx<float> target (2, 2);
  target.set(58.8, 0, 0);
  target.set(32.6, 1, 0);
  target.set(20.5, 0, 1);
  target.set(7.6, 1, 1);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) {
      CPPUNIT_ASSERT(approx_equals(B1.get(i, j), target.get(i, j)));
      CPPUNIT_ASSERT(approx_equals(B1.get(i, j), B2.get(i, j)));
    }
}

void thops_test::test_th_convolve_double() {
  idx<double> A(4, 4);
  idx_clear(A);
  A.set(1., 0, 0);
  A.set(3., 1, 0);
  A.set(2., 2, 0);
  A.set(0., 3, 0);
  A.set(2., 0, 1);
  A.set(3., 1, 1);
  A.set(6., 2, 1);
  A.set(0., 3, 1);
  A.set(3., 0, 2);
  A.set(-1., 1, 2);
  A.set(3., 2, 2);
  A.set(4., 3, 2);
  A.set(-1., 0, 3);
  A.set(0., 1, 3);
  A.set(-2., 2, 3);
  A.set(-2., 3, 3);
  idx<double> ker(3, 3);
  idx_clear(ker);
  ker.set(1., 0, 0);
  ker.set(2., 1, 0);
  ker.set(3., 2, 0);
  ker.set(0., 0, 1);
  ker.set(-1., 1, 1);
  ker.set(3.5, 2, 1);
  ker.set(3., 0, 2);
  ker.set(2.2, 1, 2);
  ker.set(7., 2, 2);
  idx<double> B1(2, 2), B2(2, 2);
  //idx<double> revker(3, 3);
  //rev_idx2_tr(ker, revker);
  th_convolution(A, ker, B1);
  idx_2dconvol(A, ker, B2);
  idx<double> target (2, 2);
  target.set(58.8, 0, 0);
  target.set(32.6, 1, 0);
  target.set(20.5, 0, 1);
  target.set(7.6, 1, 1);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) {
      CPPUNIT_ASSERT(approx_equals(B1.get(i, j), target.get(i, j)));
      CPPUNIT_ASSERT(approx_equals(B1.get(i, j), B2.get(i, j)));
    }
}

#endif /* __TH__ */
