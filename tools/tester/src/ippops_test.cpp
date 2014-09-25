#include "ippops_test.h"

#ifdef __IPP__

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

void ippops_test::setUp() {
}

void ippops_test::tearDown() {
}

// tests for ipp_convolution

void ippops_test::test_ipp_convolve_float() {
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
  idx<float> revker(3, 3);
  rev_idx2_tr(ker, revker);
  ipp_convolution(A, revker, B1);
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

void ippops_test::test_ipp_convolve_ubyte() {
  idx<ubyte> A(4, 4);
  idx_clear(A);
  A.set(1, 0, 0);
  A.set(3, 1, 0);
  A.set(2, 2, 0);
  A.set(0, 3, 0);
  A.set(2, 0, 1);
  A.set(3, 1, 1);
  A.set(6, 2, 1);
  A.set(0, 3, 1);
  A.set(3, 0, 2);
  A.set(1, 1, 2);
  A.set(3, 2, 2);
  A.set(4, 3, 2);
  A.set(1, 0, 3);
  A.set(0, 1, 3);
  A.set(2, 2, 3);
  A.set(2, 3, 3);
  idx<ubyte> ker(3, 3);
  idx_clear(ker);
  ker.set(1, 0, 0);
  ker.set(2, 1, 0);
  ker.set(3, 2, 0);
  ker.set(0, 0, 1);
  ker.set(1, 1, 1);
  ker.set(3, 2, 1);
  ker.set(3, 0, 2);
  ker.set(2, 1, 2);
  ker.set(7, 2, 2);
  idx<ubyte> B1(2, 2), B2(2, 2);
  idx<ubyte> revker(3, 3);
  rev_idx2_tr(ker, revker);
  ipp_convolution(A, revker, B1);
  idx_2dconvol(A, ker, B2);
  idx<ubyte> target (2, 2);
  target.set(66, 0, 0);
  target.set(50, 1, 0);
  target.set(53, 0, 1);
  target.set(48, 1, 1);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) {
      CPPUNIT_ASSERT(B1.get(i, j) == target.get(i, j));
      CPPUNIT_ASSERT(B1.get(i, j) == B2.get(i, j));
    }
}

void ippops_test::test_ipp_convolve_int16() {
  idx<int16> A(4, 4);
  idx_clear(A);
  A.set(1, 0, 0);
  A.set(3, 1, 0);
  A.set(-16, 2, 0);
  A.set(0, 3, 0);
  A.set(2, 0, 1);
  A.set(3, 1, 1);
  A.set(6, 2, 1);
  A.set(6000, 3, 1);
  A.set(3, 0, 2);
  A.set(1, 1, 2);
  A.set(3, 2, 2);
  A.set(4, 3, 2);
  A.set(-501, 0, 3);
  A.set(0, 1, 3);
  A.set(2, 2, 3);
  A.set(2, 3, 3);
  idx<int16> ker(3, 3);
  idx_clear(ker);
  ker.set(1, 0, 0);
  ker.set(2, 1, 0);
  ker.set(3, 2, 0);
  ker.set(0, 0, 1);
  ker.set(412, 1, 1);
  ker.set(3, 2, 1);
  ker.set(3, 0, 2);
  ker.set(2, 1, 2);
  ker.set(-7, 2, 2);
  idx<int16> B1(2, 2), B2(2, 2);
  idx<int16> revker(3, 3);
  rev_idx2_tr(ker, revker);
  ipp_convolution(A, revker, B1);
  idx_2dconvol(A, ker, B2);
  idx<int16> target (2, 2);
  target.set(1203, 0, 0);
  target.set(20424, 1, 0);
  target.set(-1070, 0, 1);
  target.set(19253, 1, 1);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) {
      CPPUNIT_ASSERT(B1.get(i, j) == target.get(i, j));
      CPPUNIT_ASSERT(B1.get(i, j) == B2.get(i, j));
    }
}

// tests for ipp_copy

void ippops_test::test_ipp_copy() {
  int nx = 42, ny = 32, nz = 23;
  idx<ubyte>  A8u(nx, ny), B8u(nx, ny);
  idx<uint16> A16u(nx), B16u(nx);
  idx<int16>  A16s(nx, ny), B16s(nx, ny);
  idx<int32>  A32s(nx, ny, nz), B32s(nx, ny, nz);
  idx<float>  A32f(nx, ny), B32f(nx, ny);
  for (int i = 0; i < nx; ++i) {
    A16u.set(((184993 * i + 17) % 256 - 128), i);
    for (int j = 0; j < ny; ++j) {
      A8u.set(((184993 * i + 15493 * j + 19) % 256), i, j);
      A16s.set(((184993 * i + 15493 * j + 532) % 65536 - 32767), i, j);
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
      for (int k = 0; k < nz; ++k) {
	A32s.set((184993 * i + 15493 * j + 11217 * k + 312), i, j, k);
      }
    }
  }
  ipp_copy(A8u, B8u);
  ipp_copy(A16u, B16u);
  ipp_copy(A16s, B16s);
  ipp_copy(A32s, B32s);
  ipp_copy(A32f, B32f);
  for (int i = 0; i < nx; ++i) {
    CPPUNIT_ASSERT(A16u.get(i) == B16u.get(i));
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(A8u.get(i, j) == B8u.get(i, j));
      CPPUNIT_ASSERT(A16s.get(i, j) == B16s.get(i, j));
      CPPUNIT_ASSERT(approx_equals(A32f.get(i, j), B32f.get(i, j)));
      for (int k = 0; k < nz; ++k) {
	CPPUNIT_ASSERT(A32s.get(i, j, k) == B32s.get(i, j, k));
      }
    }
  }
}

// tests for ipp_clear

void ippops_test::test_ipp_clear() {
  int nx = 32, ny = 21;
  idx<ubyte> A8u(nx, ny);
  idx<uint16> A16u(nx, ny);
  idx<int16> A16s(nx, ny);
  idx<int32> A32s(nx, ny);
  idx<float> A32f(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A8u.set((ubyte)(3.2 * i - 2. * j), i, j);
      A16u.set((uint16)(13.2 * i - 20. * j), i, j);
      A16s.set((int16)(113.2 * i - 252. * j + 100), i, j);
      A32s.set((int32)(15231.2 * i + 2055530 * j), i, j);
      A32f.set((float)(0.32 * i + 32. * j), i, j);
    }
  idx_clear(A8u);
  idx_clear(A16u);
  idx_clear(A16s);
  idx_clear(A32s);
  idx_clear(A32f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(A8u.get(i, j) == 0);
      CPPUNIT_ASSERT(A16u.get(i, j) == 0);
      CPPUNIT_ASSERT(A16s.get(i, j) == 0);
      CPPUNIT_ASSERT(A32s.get(i, j) == 0);
      CPPUNIT_ASSERT(approx_equals(A32f.get(i, j), 0));
    }
}

// tests for ipp_fill

void ippops_test::test_ipp_fill() {
  int nx = 32, ny = 21;
  idx<ubyte> A8u(nx, ny);
  idx<uint16> A16u(nx, ny);
  idx<int16> A16s(nx, ny);
  idx<int32> A32s(nx, ny);
  idx<float> A32f(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A8u.set((ubyte)(3.2 * i - 2. * j), i, j);
      A16u.set((uint16)(13.2 * i - 20. * j), i, j);
      A16s.set((int16)(113.2 * i - 252. * j + 100), i, j);
      A32s.set((int32)(15231.2 * i + 2055530 * j), i, j);
      A32f.set((float)(0.32 * i + 32. * j), i, j);
    }
  idx_fill(A8u, (ubyte)32);
  idx_fill(A16u, (uint16)4321);
  idx_fill(A16s, (int16)(-12));
  idx_fill(A32s, (int32)(-12345678));
  idx_fill(A32f, 13.21f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(A8u.get(i, j) == 32);
      CPPUNIT_ASSERT(A16u.get(i, j) == 4321);
      CPPUNIT_ASSERT(A16s.get(i, j) == -12);
      CPPUNIT_ASSERT(A32s.get(i, j) == -12345678);
      CPPUNIT_ASSERT(approx_equals(A32f.get(i, j), 13.21f));
    }
}

// tests for ipp_minus

void ippops_test::test_ipp_minus() {
  int nx = 15, ny = 12, nz = 23, nw = 5;
  idx<int16> A16(nx, ny, nz, nw);
  idx<float32> A(nx, ny, nz, nw);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k)
	for (int l = 0; l < nw; ++l) {
	  A16.set((int16)(((184993*i + 15493*j + 122*k + 4215*l) % 7561) - 4000),
		i, j, k, l);
	  A.set(((184993*i + 15493*j + 122*k + 4215*l) % 7561)*1.43 - 4000,
		i, j, k, l);
	}
  idx<int16> B16(nx, ny, nz, nw);
  idx<float32> B(nx, ny, nz, nw);
  ipp_minus(A16, B16);
  ipp_minus(A, B);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k)
	for (int l = 0; l < nw; ++l) {
	  CPPUNIT_ASSERT(approx_equals(B16.get(i, j, k, l), -A16.get(i, j, k, l)));
	  CPPUNIT_ASSERT(approx_equals(B.get(i, j, k, l), -A.get(i, j, k, l)));
	}
}

// tests for ipp_add

void ippops_test::test_ipp_add_float() {
  int nx = 15, ny = 42;
  idx<float> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 7561) * 1.43 - 4000, i, j);
      B.set(((184993 * i + 15493 * j) % 2459) * 2.31 - 2000, i, j);
    }
  idx<float> AB(nx, ny), AB2(nx, ny);
  idx_copy(B, AB);
  ipp_add(A, AB);
  ipp_add(A, B, AB2);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(approx_equals(AB.get(i, j), A.get(i, j) + B.get(i, j)));
      CPPUNIT_ASSERT(approx_equals(AB2.get(i, j), A.get(i, j) + B.get(i, j)));
    }
}


void ippops_test::test_ipp_add_ubyte() {
  int nx = 22, ny = 54;
  idx<ubyte> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 256), i, j);
      B.set(((19 * i + 15493 * j + 17) % 256), i, j);
    }
  idx<ubyte> AB(nx, ny), AB2(nx, ny), ABtest(nx, ny);
  idx_copy(B, AB);
  ipp_add(A, AB);
  ipp_add(A, B, AB2);
  int sum;
  ubyte expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) + (int)B.get(i, j);
      expected = saturate(sum, ubyte);
      CPPUNIT_ASSERT(AB.get(i, j)  == expected);
      CPPUNIT_ASSERT(AB2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_add_uint16() {
  int nx = 51, ny = 12;
  idx<uint16> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 65536), i, j);
      B.set(((19 * i + 15493 * j + 17) % 65536), i, j);
    }
  idx<uint16> AB(nx, ny), AB2(nx, ny);
  idx_copy(B, AB);
  ipp_add(A, AB);
  ipp_add(A, B, AB2);
  int sum;
  uint16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) + (int)B.get(i, j);
      expected = saturate(sum, uint16);
      CPPUNIT_ASSERT(AB.get(i, j)  == expected);
      CPPUNIT_ASSERT(AB2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_add_int16() {
  int nx = 43, ny = 43;
  idx<int16> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j + 532) % 65536 - 32768), i, j);
      B.set(((19 * i + 15493 * j +  12) % 65536 - 32768), i, j);
    }
  A.set(-30000, 10, 10);
  B.set(-31000, 10, 10);
  A.set(30000, 10, 11);
  B.set(31000, 10, 11);
  idx<int16> AB(nx, ny), AB2(nx, ny);
  idx_copy(B, AB);
  ipp_add(A, AB);
  ipp_add(A, B, AB2);
  int sum;
  int16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) + (int)B.get(i, j);
      expected = saturate(sum, int16);
      CPPUNIT_ASSERT(AB.get(i, j)  == expected);
      CPPUNIT_ASSERT(AB2.get(i, j) == expected);
    }
}

// tests for ipp_addc

void ippops_test::test_ipp_addc_float() {
  int nx = 15, ny = 42;
  idx<float> A(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 7561) * 1.43 - 4000, i, j);
    }
  idx<float> B(nx, ny), B2(nx, ny);
  idx_copy(A, B);
  ipp_addc(B, 42.123f);
  ipp_addc(A, -123.12f, B2);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(approx_equals(B.get(i, j), A.get(i, j) + 42.123f));
      CPPUNIT_ASSERT(approx_equals(B2.get(i, j), A.get(i, j) - 123.12f));
    }
}

void ippops_test::test_ipp_addc_ubyte() {
  int nx = 15, ny = 12;
  idx<ubyte> A(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set((189393 * i + 1493 * j) % 256, i, j);
    }
  A.set(250, 5, 5);
  idx<ubyte> B(nx, ny), B2(nx, ny);
  idx_copy(A, B);
  ipp_addc(B, (ubyte)42);
  ipp_addc(A, (ubyte)42, B2);
  ubyte expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      expected = saturate((int)A.get(i, j) + 42, ubyte);
      CPPUNIT_ASSERT(B.get(i, j)  == expected);
      CPPUNIT_ASSERT(B2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_addc_uint16() {
  int nx = 15, ny = 12;
  idx<uint16> A(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set((189393 * i + 1493 * j) % 65536, i, j);
    }
  A.set(65000, 3, 3);
  idx<uint16> B(nx, ny), B2(nx, ny);
  idx_copy(A, B);
  ipp_addc(B, (uint16)4200);
  ipp_addc(A, (uint16)4200, B2);
  uint16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      expected = saturate((int)A.get(i, j) + 4200, uint16);
      CPPUNIT_ASSERT(B.get(i, j)  == expected);
      CPPUNIT_ASSERT(B2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_addc_int16() {
  int nx = 15, ny = 12;
  idx<int16> A(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set((189393 * i + 1493 * j) % 65536 - 30000, i, j);
    }
  A.set(-30000, 3, 3);
  A.set(30000, 3, 3);
  idx<int16> B(nx, ny), B2(nx, ny);
  idx_copy(A, B);
  ipp_addc(B, (int16)(-8200));
  ipp_addc(A, (int16)8200, B2);
  int16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      expected = saturate((int)A.get(i, j) - 8200, int16);
      CPPUNIT_ASSERT(B.get(i, j)  == expected);
      expected = saturate((int)A.get(i, j) + 8200, int16);
      CPPUNIT_ASSERT(B2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_bounded() {
  int nx = 15, ny = 12;
  idx<ubyte>   A8u (nx, ny);
  idx<uint16>  A16u(nx, ny);
  idx<int16>   A16s(nx, ny);
  idx<float32> A32f(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A8u.set((int16)(((184993*i + 15493*j) % 256)), i, j);
      A16u.set((int16)(((184993*i + 15493*j) % 65536)), i, j);
      A16s.set((int16)(((184993*i + 15493*j) % 7561) - 4000), i, j);
      A32f.set(((184993*i + 15493*j) % 7561) *1.43 - 4000, i, j);
    }
  idx<ubyte>   B8u (nx, ny), C8u (nx, ny);
  idx<uint16>  B16u(nx, ny), C16u(nx, ny);
  idx<int16>   B16s(nx, ny), C16s(nx, ny);
  idx<float32> B32f(nx, ny), C32f(nx, ny);
  int s8u, s16u, s16s;
  ubyte e8u;
  uint16 e16u;
  int16 e16s;
  //ipp_addc_bounded
  ipp_addc(A8u, (ubyte)42, B8u);
  ipp_addc(A16u, (uint16)42000, B16u);
  ipp_addc(A16s, (int16)(-22000), B16s);
  ipp_addc(A32f, 12351.12f, B32f);
  idx_copy(A8u, C8u);
  idx_copy(A16u, C16u);
  idx_copy(A16s, C16s);
  idx_copy(A32f, C32f);
  ipp_addc(C8u, (ubyte)42);
  ipp_addc(C16u, (uint16)42000);
  ipp_addc(C16s, (int16)(-22000));
  ipp_addc(C32f, 12351.12f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      s8u = (int)A8u.get(i, j) + 42;
      s16u = (int)A16u.get(i, j) + 42000;
      s16s = (int)A16s.get(i, j) - 22000;
      e8u = saturate(s8u, ubyte);
      e16u = saturate(s16u, uint16);
      e16s = saturate(s16s, int16);
      CPPUNIT_ASSERT(B8u.get(i, j)  == e8u);
      CPPUNIT_ASSERT(C8u.get(i, j)  == e8u);
      CPPUNIT_ASSERT(B16u.get(i, j) == e16u);
      CPPUNIT_ASSERT(C16u.get(i, j) == e16u);
      CPPUNIT_ASSERT(B16s.get(i, j) == e16s);
      CPPUNIT_ASSERT(C16s.get(i, j) == e16s);
      CPPUNIT_ASSERT(approx_equals(B32f.get(i, j), A32f.get(i, j) + 12351.12));
      CPPUNIT_ASSERT(approx_equals(C32f.get(i, j), A32f.get(i, j) + 12351.12));
    }
  //ipp_subc
  ipp_subc(A8u, (ubyte)42, B8u);
  ipp_subc(A16u, (uint16)42000, B16u);
  ipp_subc(A16s, (int16)(-22000), B16s);
  ipp_subc(A32f, 12351.12f, B32f);
  idx_copy(A8u, C8u);
  idx_copy(A16u, C16u);
  idx_copy(A16s, C16s);
  idx_copy(A32f, C32f);
  ipp_subc(C8u, (ubyte)42);
  ipp_subc(C16u, (uint16)42000);
  ipp_subc(C16s, (int16)(-22000));
  ipp_subc(C32f, 12351.12f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      s8u = (int)A8u.get(i, j) - 42;
      s16u = (int)A16u.get(i, j) - 42000;
      s16s = (int)A16s.get(i, j) + 22000;
      e8u = saturate(s8u, ubyte);
      e16u = saturate(s16u, uint16);
      e16s = saturate(s16s, int16);
      CPPUNIT_ASSERT(B8u.get(i, j)  == e8u);
      CPPUNIT_ASSERT(C8u.get(i, j)  == e8u);
      CPPUNIT_ASSERT(B16u.get(i, j) == e16u);
      CPPUNIT_ASSERT(C16u.get(i, j) == e16u);
      CPPUNIT_ASSERT(B16s.get(i, j) == e16s);
      CPPUNIT_ASSERT(C16s.get(i, j) == e16s);
      CPPUNIT_ASSERT(approx_equals(B32f.get(i, j), A32f.get(i, j) - 12351.12));
      CPPUNIT_ASSERT(approx_equals(C32f.get(i, j), A32f.get(i, j) - 12351.12));
    }
  //ipp_dotc_bounded
  ipp_dotc(A8u, (ubyte)5, B8u);
  ipp_dotc(A16u, (uint16)42, B16u);
  ipp_dotc(A16s, (int16)(-22), B16s);
  ipp_dotc(A32f, 123.12f, B32f);
  idx_copy(A8u, C8u);
  idx_copy(A16u, C16u);
  idx_copy(A16s, C16s);
  idx_copy(A32f, C32f);
  ipp_dotc(C8u, (ubyte)5);
  ipp_dotc(C16u, (uint16)42);
  ipp_dotc(C16s, (int16)(-22));
  ipp_dotc(C32f, 123.12f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      s8u = (int)A8u.get(i, j) * 5;
      s16u = (int)A16u.get(i, j) * 42;
      s16s = (int)A16s.get(i, j) * (-22);
      e8u = saturate(s8u, ubyte);
      e16u = saturate(s16u, uint16);
      e16s = saturate(s16s, int16);
      CPPUNIT_ASSERT(B8u.get(i, j)  == e8u);
      CPPUNIT_ASSERT(C8u.get(i, j)  == e8u);
      CPPUNIT_ASSERT(B16u.get(i, j) == e16u);
      CPPUNIT_ASSERT(C16u.get(i, j) == e16u);
      CPPUNIT_ASSERT(B16s.get(i, j) == e16s);
      CPPUNIT_ASSERT(C16s.get(i, j) == e16s);
      CPPUNIT_ASSERT(approx_equals(B32f.get(i, j), A32f.get(i, j) * 123.12));
      CPPUNIT_ASSERT(approx_equals(C32f.get(i, j), A32f.get(i, j) * 123.12));
    }
}

// tests for ipp_sub

void ippops_test::test_ipp_sub_float() {
  int nx = 15, ny = 42;
  idx<float> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 7561) * 1.43 - 4000, i, j);
      B.set(((184993 * i + 15493 * j) % 2459) * 2.31 - 2000, i, j);
    }
  idx<float> AB(nx, ny), AB2(nx, ny);
  idx_copy(A, AB);
  ipp_sub(AB, B);
  ipp_sub(A, B, AB2);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(approx_equals(AB.get(i, j), A.get(i, j) - B.get(i, j)));
      CPPUNIT_ASSERT(approx_equals(AB2.get(i, j), A.get(i, j) - B.get(i, j)));
    }
}

void ippops_test::test_ipp_sub_ubyte() {
  int nx = 22, ny = 54;
  idx<ubyte> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 256), i, j);
      B.set(((19 * i + 15493 * j + 17) % 256), i, j);
    }
  idx<ubyte> AB(nx, ny), AB2(nx, ny), ABtest(nx, ny);
  idx_copy(A, AB);
  ipp_sub(AB, B);
  ipp_sub(A, B, AB2);
  int sum;
  ubyte expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) - (int)B.get(i, j);
      if (sum > 255)
	expected = 255;
      else if (sum < 0)
	expected = 0;
      else
	expected = sum;
      CPPUNIT_ASSERT(AB.get(i, j)  == expected);
      CPPUNIT_ASSERT(AB2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_sub_uint16() {
  int nx = 51, ny = 12;
  idx<uint16> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 65536), i, j);
      B.set(((19 * i + 15493 * j + 17) % 65536), i, j);
    }
  idx<uint16> AB(nx, ny), AB2(nx, ny);
  idx_copy(A, AB);
  ipp_sub(AB, B);
  ipp_sub(A, B, AB2);
  int sum;
  uint16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) - (int)B.get(i, j);
      if (sum > 65535)
	expected = 65535;
      else if (sum < 0)
	expected = 0;
      else
	expected = sum;
      CPPUNIT_ASSERT(AB.get(i, j)  == expected);
      CPPUNIT_ASSERT(AB2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_sub_int16() {
  int nx = 43, ny = 43;
  idx<int16> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j + 532) % 65536 - 32768), i, j);
      B.set(((19 * i + 15493 * j +  12) % 65536 - 32768), i, j);
    }
  A.set(-30000, 10, 10);
  B.set(-31000, 10, 10);
  A.set(30000, 10, 11);
  B.set(31000, 10, 11);
  idx<int16> AB(nx, ny), AB2(nx, ny);
  idx_copy(A, AB);
  ipp_sub(AB, B);
  ipp_sub(A, B, AB2);
  int sum;
  int16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) - (int)B.get(i, j);
      if (sum > 32767)
	expected = 32767;
      else if (sum < -32768)
	expected = -32768;
      else
	expected = sum;
      CPPUNIT_ASSERT(AB.get(i, j)  == expected);
      CPPUNIT_ASSERT(AB2.get(i, j) == expected);
    }
}

// tests for ipp_mul

void ippops_test::test_ipp_mul_float() {
  int nx = 15, ny = 42;
  idx<float> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 7561) * 1.43 - 4000, i, j);
      B.set(((184993 * i + 15493 * j) % 2459) * 2.31 - 2000, i, j);
    }
  idx<float> AB(nx, ny), AB2(nx, ny);
  idx_copy(B, AB);
  ipp_mul(A, AB);
  ipp_mul(A, B, AB2);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(approx_equals(AB.get(i, j), A.get(i, j) * B.get(i, j)));
      CPPUNIT_ASSERT(approx_equals(AB2.get(i, j), A.get(i, j) * B.get(i, j)));
    }
}


void ippops_test::test_ipp_mul_ubyte() {
  int nx = 22, ny = 54;
  idx<ubyte> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 256), i, j);
      B.set(((19 * i + 15493 * j + 17) % 256), i, j);
    }
  idx<ubyte> AB(nx, ny), AB2(nx, ny), ABtest(nx, ny);
  idx_copy(B, AB);
  ipp_mul(A, AB);
  ipp_mul(A, B, AB2);
  int sum;
  ubyte expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) * (int)B.get(i, j);
      expected = (sum > 255) ? 255 : sum;
      CPPUNIT_ASSERT(AB.get(i, j)  == expected);
      CPPUNIT_ASSERT(AB2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_mul_uint16() {
  int nx = 51, ny = 12, nz = 42;
  idx<uint16> A(nx, ny, nz), B(nx, ny, nz);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k) {
	A.set(((184993 * i + 15493 * j + 213 * k) % 65536), i, j, k);
	B.set(((19 * i + 15493 * j + 17 + 256841 * k) % 65536), i, j, k);
      }
  idx<uint16> AB(nx, ny, nz), AB2(nx, ny, nz);
  idx_copy(B, AB);
  ipp_mul(A, AB);
  ipp_mul(A, B, AB2);
  long long sum;
  uint16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k) {
	sum = (long long)A.get(i, j, k) * (long long)B.get(i, j, k);
	expected = (sum > 65535) ? 65535 : sum;
	CPPUNIT_ASSERT(AB.get(i, j, k)  == expected);
	CPPUNIT_ASSERT(AB2.get(i, j, k) == expected);
      }
}

void ippops_test::test_ipp_mul_int16() {
  int nx = 43, ny = 43;
  idx<int16> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j + 532) % 65536 - 32768), i, j);
      B.set(((19 * i + 15493 * j +  12) % 65536 - 32768), i, j);
    }
  A.set(-30000, 10, 10);
  B.set(-31000, 10, 10);
  A.set(30000, 10, 11);
  B.set(31000, 10, 11);
  idx<int16> AB(nx, ny), AB2(nx, ny);
  idx_copy(B, AB);
  ipp_mul(A, AB);
  ipp_mul(A, B, AB2);
  long long sum;
  int16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (long long)A.get(i, j) * (long long)B.get(i, j);
      if (sum > 32767)
	expected = 32767;
      else if (sum < -32768)
	expected = -32768;
      else
	expected = sum;
      CPPUNIT_ASSERT(AB.get(i, j)  == expected);
      CPPUNIT_ASSERT(AB2.get(i, j) == expected);
    }
}

// tests for ipp_dotc

void ippops_test::test_ipp_dotc_float() {
  int nx = 15, ny = 42;
  idx<float> A(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 7561) * 1.43 - 4000, i, j);
    }
  idx<float> B(nx, ny), B2(nx, ny);
  idx_copy(A, B);
  ipp_dotc(B, 42.123f);
  ipp_dotc(A, -123.12f, B2);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(approx_equals(B.get(i, j), A.get(i, j) * 42.123f));
      CPPUNIT_ASSERT(approx_equals(B2.get(i, j), A.get(i, j) * (-123.12f)));
    }
}

void ippops_test::test_ipp_dotc_ubyte() {
  int nx = 15, ny = 12;
  idx<ubyte> A(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set((189393 * i + 1493 * j) % 256, i, j);
    }
  A.set(250, 5, 5);
  idx<ubyte> B(nx, ny), B2(nx, ny);
  idx_copy(A, B);
  ipp_dotc(B, (ubyte)5);
  ipp_dotc(A, (ubyte)5, B2);
  int sum;
  ubyte expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) * 5;
      expected = (sum > 255) ? 255 : sum;
      CPPUNIT_ASSERT(B.get(i, j)  == expected);
      CPPUNIT_ASSERT(B2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_dotc_uint16() {
  int nx = 15, ny = 12;
  idx<uint16> A(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set((189393 * i + 1493 * j) % 65536, i, j);
    }
  A.set(65000, 3, 3);
  idx<uint16> B(nx, ny), B2(nx, ny);
  idx_copy(A, B);
  ipp_dotc(B, (uint16)420);
  ipp_dotc(A, (uint16)420, B2);
  int sum;
  uint16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) * 420;
      expected = (sum > 65535) ? 65535 : sum;
      CPPUNIT_ASSERT(B.get(i, j)  == expected);
      CPPUNIT_ASSERT(B2.get(i, j) == expected);
    }
}

void ippops_test::test_ipp_dotc_int16() {
  int nx = 15, ny = 12;
  idx<int16> A(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set((189393 * i + 1493 * j) % 65536 - 30000, i, j);
    }
  A.set(-30000, 3, 3);
  A.set(30000, 3, 3);
  idx<int16> B(nx, ny), B2(nx, ny);
  idx_copy(A, B);
  ipp_dotc(B, (int16)520);
  ipp_dotc(A, (int16)520, B2);
  int sum;
  int16 expected;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      sum = (int)A.get(i, j) * 520;
      if (sum > 32767)
	expected = 32767;
      else if (sum < -32768)
	expected = -32768;
      else
	expected = sum;
      CPPUNIT_ASSERT(B.get(i, j)  == expected);
      CPPUNIT_ASSERT(B2.get(i, j) == expected);
    }
}

// tests for ipp_div

void ippops_test::test_ipp_div_float() {
  int nx = 15, ny = 42;
  idx<float> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 7561) * 1.43 - 4000, i, j);
      B.set(((184993 * i + 15493 * j) % 2459) * 2.31 - 2000, i, j);
      if (std::abs(B.get(i, j)) < 1e-10)
	B.set(1e-10, i, j);
    }
  idx<float> AB(nx, ny), AB2(nx, ny);
  idx_copy(A, AB);
  ipp_div(AB, B);
  ipp_div(A, B, AB2);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(approx_equals(AB.get(i, j), A.get(i, j) / B.get(i, j)));
      CPPUNIT_ASSERT(approx_equals(AB2.get(i, j), A.get(i, j) / B.get(i, j)));
    }
}


void ippops_test::test_ipp_div_ubyte() {
  int nx = 22, ny = 54;
  idx<ubyte> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 256), i, j);
      B.set(((19 * i + 15493 * j + 17) % 256), i, j);
      if (std::abs(B.get(i, j)) == 0)
	B.set(1, i, j);
    }
  idx<ubyte> AB(nx, ny), AB2(nx, ny), ABtest(nx, ny);
  idx_copy(A, AB);
  ipp_div(AB, B);
  ipp_div(A, B, AB2);
  ubyte expected;
  int temp;
  bool half;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      temp = (2 * (int)A.get(i, j)) / (int)B.get(i, j);
      if ((2 * (int)A.get(i, j) == temp * (int)B.get(i, j)) &&
	  (A.get(i, j) != (A.get(i, j) / B.get(i, j)) * B.get(i, j))) {
	expected = (ubyte)ROUND((double)A.get(i, j) / (double)B.get(i, j)) - 1;
	half = true;
      } else {
	expected = (ubyte)ROUND((double)A.get(i, j) / (double)B.get(i, j));
	half = false;
      }
      CPPUNIT_ASSERT(AB.get(i, j)  == expected ||
		     (half && (AB.get(i, j)  == expected + 1)));
      CPPUNIT_ASSERT(AB2.get(i, j)  == expected ||
		     (half && (AB2.get(i, j)  == expected + 1)));
    }
}

void ippops_test::test_ipp_div_uint16() {
  int nx = 51, ny = 12;
  idx<uint16> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j) % 65536), i, j);
      B.set(((19 * i + 15493 * j + 17) % 65536), i, j);
      if (std::abs(B.get(i, j)) == 0)
	B.set(1, i, j);
    }
  A.set(10, 10, 10);
  B.set(20, 10, 10);
  A.set(246, 11, 11);
  B.set(12, 11, 11);
  idx<uint16> AB(nx, ny), AB2(nx, ny);
  idx_copy(A, AB);
  ipp_div(AB, B);
  ipp_div(A, B, AB2);
  uint16 expected;
  int temp;
  bool half;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      temp = (2 * (int)A.get(i, j)) / (int)B.get(i, j);
      if ((2 * (int)A.get(i, j) == temp * (int)B.get(i, j)) &&
	  (A.get(i, j) != (A.get(i, j) / B.get(i, j)) * B.get(i, j))) {
	expected = (uint16)ROUND((double)A.get(i, j) / (double)B.get(i, j)) - 1;
	half = true;
      } else {
	expected = (uint16)ROUND((double)A.get(i, j) / (double)B.get(i, j));
	half = false;
      }
      CPPUNIT_ASSERT(AB.get(i, j)  == expected ||
		     (half && (AB.get(i, j)  == expected + 1)));
      CPPUNIT_ASSERT(AB2.get(i, j)  == expected ||
		     (half && (AB2.get(i, j)  == expected + 1)));
    }
}

void ippops_test::test_ipp_div_int16() {
  int nx = 43, ny = 43;
  idx<int16> A(nx, ny), B(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j + 532) % 65536 - 32768), i, j);
      B.set(((19 * i + 15493 * j +  12) % 65536 - 32768), i, j);
      if (std::abs(B.get(i, j)) == 0)
	B.set(1, i, j);
    }
  A.set(10, 10, 10);
  B.set(20, 10, 10);
  A.set(246, 11, 11);
  B.set(12, 11, 11);
  idx<int16> AB(nx, ny), AB2(nx, ny);
  idx_copy(A, AB);
  ipp_div(AB, B);
  ipp_div(A, B, AB2);
  int16 expected;
  int temp;
  bool half;
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      temp = (2 * (int)A.get(i, j)) / (int)B.get(i, j);
      if ((2 * (int)A.get(i, j) == temp * (int)B.get(i, j)) &&
	  (A.get(i, j) != (A.get(i, j) / B.get(i, j)) * B.get(i, j))) {
	expected = (int16)ROUND((double)A.get(i, j) / (double)B.get(i, j)) - 1;
	half = true;
      } else {
	expected = (int16)ROUND((double)A.get(i, j) / (double)B.get(i, j));
	half = false;
      }
      CPPUNIT_ASSERT(AB.get(i, j)  == expected ||
		     (half && (AB.get(i, j)  == expected + 1)));
      CPPUNIT_ASSERT(AB2.get(i, j)  == expected ||
		     (half && (AB2.get(i, j)  == expected + 1)));
    }
}

// tests for ipp_inv

void ippops_test::test_ipp_inv() {
  int nx = 15, ny = 12, nz = 23, nw = 5;
  idx<float32> A(nx, ny, nz, nw);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k)
	for (int l = 0; l < nw; ++l) {
	  A.set(((184993*i + 15493*j + 122*k + 4215*l) % 7561)*1.43 - 4000,
		i, j, k, l);
	}
  idx<float32> B(nx, ny, nz, nw);
  ipp_inv(A, B);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k)
	for (int l = 0; l < nw; ++l) {
	  CPPUNIT_ASSERT(approx_equals(B.get(i, j, k, l) * A.get(i, j, k, l),
				       1.0));
	}
}

// tests for ipp_abs

void ippops_test::test_ipp_abs() {
  int nx = 43, ny = 43;
  idx<int16> A(nx, ny);
  idx<float32> Af(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A.set(((184993 * i + 15493 * j + 532) % 65536 - 32768), i, j);
      Af.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123 - 32.68), i, j);
    }
  idx<int16> B(nx, ny), B2(nx, ny);
  idx<float32> Bf(nx, ny), Bf2(nx, ny);
  idx_copy(A, B2);
  idx_copy(Af, Bf2);
  ipp_abs(B2);
  ipp_abs(A, B);
  ipp_abs(Bf2);
  ipp_abs(Af, Bf);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(B.get(i, j) == std::abs(A.get(i, j)));
      CPPUNIT_ASSERT(B2.get(i, j) == std::abs(A.get(i, j)));
      CPPUNIT_ASSERT(Bf.get(i, j) == std::abs(Af.get(i, j)));
      CPPUNIT_ASSERT(Bf2.get(i, j) == std::abs(Af.get(i, j)));
    }
}

// tests for ipp_sqrt

void ippops_test::test_ipp_sqrt() {
  int nx = 42, ny = 33;
  idx<ubyte>  A8u(nx, ny);
  idx<uint16> A16u(nx, ny);
  idx<int16>  A16s(nx, ny);
  idx<float32>  A32f(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A8u.set(((184993 * i + 15493 * j + 19) % 256), i, j);
      A16u.set(((184993 * i + 15493 * j + 17) % 65536), i, j);
      A16s.set(((184993 * i + 15493 * j + 532) % 32767), i, j);
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
    }
  idx<ubyte>  B8u (nx, ny), B8u2 (nx, ny);
  idx<uint16> B16u(nx, ny), B16u2(nx, ny);
  idx<int16>  B16s(nx, ny), B16s2(nx, ny);
  idx<float32>  B32f(nx, ny), B32f2(nx, ny);
  //8u
  idx_copy(A8u, B8u2);
  ipp_sqrt(B8u2);
  ipp_sqrt(A8u, B8u);
  //16u
  idx_copy(A16u, B16u2);
  ipp_sqrt(B16u2);
  ipp_sqrt(A16u, B16u);
  //16s
  idx_copy(A16s, B16s2);
  ipp_sqrt(B16s2);
  ipp_sqrt(A16s, B16s);
  //32f
  idx_copy(A32f, B32f2);
  ipp_sqrt(B32f2);
  ipp_sqrt(A32f, B32f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(B8u.get(i, j) ==
		     (ubyte)ROUND(sqrt((float)A8u.get(i, j))));
      CPPUNIT_ASSERT(B8u2.get(i, j) ==
		     (ubyte)ROUND(sqrt((float)A8u.get(i, j))));
      CPPUNIT_ASSERT(B16u.get(i, j) ==
		     (uint16)ROUND(sqrt((float)A16u.get(i, j))));
      CPPUNIT_ASSERT(B16u2.get(i, j) ==
		     (uint16)ROUND(sqrt((float)A16u.get(i, j))));
      CPPUNIT_ASSERT(B16s.get(i, j) ==
		     (int16)ROUND(sqrt((float)A16s.get(i, j))));
      CPPUNIT_ASSERT(B16s2.get(i, j) ==
		     (int16)ROUND(sqrt((float)A16s.get(i, j))));
      CPPUNIT_ASSERT(approx_equals(B32f.get(i, j), sqrt(A32f.get(i, j))));
      CPPUNIT_ASSERT(approx_equals(B32f2.get(i, j), sqrt(A32f.get(i, j))));
    }
}

// tests for ipp_exp

void ippops_test::test_ipp_exp() {
  int nx = 42, ny = 33;
  idx<ubyte>  A8u(nx, ny);
  idx<uint16> A16u(nx, ny);
  idx<int16>  A16s(nx, ny);
  idx<float32>  A32f(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A8u.set(((184993 * i + 15493 * j + 19) % 256) / 10, i, j);
      A16u.set(((184993 * i + 15493 * j + 17) % 65536) / 100, i, j);
      A16s.set(((184993 * i + 15493 * j + 532) % 32767) / 100, i, j);
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
    }
  idx<ubyte>  B8u (nx, ny), B8u2 (nx, ny);
  idx<uint16> B16u(nx, ny), B16u2(nx, ny);
  idx<int16>  B16s(nx, ny), B16s2(nx, ny);
  idx<float32>  B32f(nx, ny), B32f2(nx, ny);
  //8u
  idx_copy(A8u, B8u2);
  ipp_exp(B8u2);
  ipp_exp(A8u, B8u);
  //16u
  idx_copy(A16u, B16u2);
  ipp_exp(B16u2);
  ipp_exp(A16u, B16u);
  //16s
  idx_copy(A16s, B16s2);
  ipp_exp(B16s2);
  ipp_exp(A16s, B16s);
  //32f
  idx_copy(A32f, B32f2);
  ipp_exp(B32f2);
  ipp_exp(A32f, B32f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(B8u.get(i, j) ==
		     saturate(ROUND(exp((float)A8u.get(i, j))), ubyte));
      CPPUNIT_ASSERT(B8u2.get(i, j) ==
		     saturate(ROUND(exp((float)A8u.get(i, j))), ubyte));
      CPPUNIT_ASSERT(B16u.get(i, j) ==
		     saturate(ROUND(exp((float)A16u.get(i, j))), uint16));
      CPPUNIT_ASSERT(B16u2.get(i, j) ==
		     saturate(ROUND(exp((float)A16u.get(i, j))), uint16));
      CPPUNIT_ASSERT(B16s.get(i, j) ==
		     saturate(ROUND(exp((float)A16s.get(i, j))), int16));
      CPPUNIT_ASSERT(B16s2.get(i, j) ==
		     saturate(ROUND(exp((float)A16s.get(i, j))), int16));
      CPPUNIT_ASSERT(approx_equals(B32f.get(i, j), exp(A32f.get(i, j))));
      CPPUNIT_ASSERT(approx_equals(B32f2.get(i, j), exp(A32f.get(i, j))));
    }
}


void ippops_test::test_ipp_sum() {
  int nx = 42, ny = 32;
  idx<ubyte>  A8u(nx);
  idx<uint16> A16u(nx, ny);
  idx<int16>  A16s(nx, ny);
  idx<float32>  A32f(nx, ny);
  for (int i = 0; i < nx; ++i) {
    A8u.set(((184993 * i + 17) % 256 - 128), i);
    for (int j = 0; j < ny; ++j) {
      A16u.set(((184993 * i + 15493 * j + 17) % 65536), i, j);
      A16s.set(((184993 * i + 15493 * j + 532) % 32767), i, j);
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
    }
  }
  float64 r8u = 0, r16u = 0, r16s = 0, r32f = 0;
  for (int i = 0; i < nx; ++i) {
    r8u += A8u.get(i);
    for (int j = 0; j < ny; ++j) {
      r16u += A16u.get(i, j);
      r16s += A16s.get(i, j);
      r32f += A32f.get(i, j);
    }
  }
  CPPUNIT_ASSERT(approx_equals(ipp_sum(A8u),  r8u ));
  CPPUNIT_ASSERT(approx_equals(ipp_sum(A16u), r16u));
  CPPUNIT_ASSERT(approx_equals(ipp_sum(A16s), r16s));
  CPPUNIT_ASSERT(approx_equals(ipp_sum(A32f), r32f));
  idx<ubyte> acc8u;
  idx<uint16> acc16u;
  idx<int16> acc16s;
  idx<float32> acc32f;
  acc8u.set(42);
  acc16u.set(414);
  acc16s.set(-1231);
  acc32f.set(1234567.89f);
  ipp_sumacc(A8u,  acc8u);
  ipp_sumacc(A16u, acc16u);
  ipp_sumacc(A16s, acc16s);
  ipp_sumacc(A32f, acc32f);
  float64 r28u = (r8u + 42 > 255) ? 255 : r8u + 42;
  float64 r216u = (r16u + 414 > 65535) ? 65535 : r16u + 414;
  float64 r216s = (r16s - 1231 > 32767) ? 32767 :
    ((r16s - 1231 < 32768) ? 32768 : r16s - 1231);
  float64 r232f = r32f + 1234567.89f;
  CPPUNIT_ASSERT(approx_equals(acc8u.get(),  r28u));
  CPPUNIT_ASSERT(approx_equals(acc16u.get(), r216u));
  CPPUNIT_ASSERT(approx_equals(acc16s.get(), r216s));
  CPPUNIT_ASSERT(approx_equals(acc32f.get(), r232f));
}

void ippops_test::test_ipp_sumabs() {
  int nx = 42, ny = 32;
  idx<ubyte>  A8u(nx);
  idx<uint16> A16u(nx, ny);
  idx<int16>  A16s(nx, ny);
  idx<float32>  A32f(nx, ny);
  for (int i = 0; i < nx; ++i) {
    A8u.set(((184993 * i + 17) % 256 - 128), i);
    for (int j = 0; j < ny; ++j) {
      A16u.set(((184993 * i + 15493 * j + 17) % 65536), i, j);
      A16s.set(((184993 * i + 15493 * j + 532) % 32767), i, j);
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
    }
  }
  float64 r8u = 0, r16u = 0, r16s = 0, r32f = 0;
  for (int i = 0; i < nx; ++i) {
    r8u += A8u.get(i);
    for (int j = 0; j < ny; ++j) {
      r16u += A16u.get(i, j);
      r16s += std::abs(A16s.get(i, j));
      r32f += std::abs(A32f.get(i, j));
    }
  }
  CPPUNIT_ASSERT(approx_equals(ipp_sumabs(A8u),  r8u ));
  CPPUNIT_ASSERT(approx_equals(ipp_sumabs(A16u), r16u));
  CPPUNIT_ASSERT(approx_equals(ipp_sumabs(A16s), r16s));
  CPPUNIT_ASSERT(approx_equals(ipp_sumabs(A32f), r32f));
}

void ippops_test::test_ipp_l2norm() {
  int nx = 42, ny = 32;
  idx<ubyte>  A8u(nx);
  idx<uint16> A16u(nx, ny);
  idx<int16>  A16s(nx, ny);
  idx<float32>  A32f(nx, ny);
  for (int i = 0; i < nx; ++i) {
    A8u.set(((184993 * i + 17) % 256), i);
    for (int j = 0; j < ny; ++j) {
      A16u.set(((184993 * i + 15493 * j + 17) % 65536), i, j);
      A16s.set(((184993 * i + 15493 * j + 532) % 65536 - 20000), i, j);
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
    }
  }
  float64 r8u = 0, r16u = 0, r16s = 0, r32f = 0;
  for (int i = 0; i < nx; ++i) {
    r8u += A8u.get(i) * A8u.get(i);
    for (int j = 0; j < ny; ++j) {
      r16u += (float64)A16u.get(i, j) * (float64)A16u.get(i, j);
      r16s += (float64)A16s.get(i, j) * (float64)A16s.get(i, j);
      r32f += A32f.get(i, j) * A32f.get(i, j);
    }
  }
  r8u = sqrt(r8u);
  r16u = sqrt(r16u);
  r16s = sqrt(r16s);
  r32f = sqrt(r32f);
  CPPUNIT_ASSERT(approx_equals(ipp_l2norm(A8u), r8u));
  CPPUNIT_ASSERT(approx_equals(ipp_l2norm(A16u), r16u));
  CPPUNIT_ASSERT(approx_equals(ipp_l2norm(A16s), r16s));
  CPPUNIT_ASSERT(approx_equals(ipp_l2norm(A32f), r32f));
}

void ippops_test::test_ipp_std_normalize() {
  int nx = 42, ny = 32;
  //idx<ubyte> A8u(nx);
  idx<float32> A32f(nx, ny);
  for (int i = 0; i < nx; ++i) {
    //A8u.set(((184993 * i + 17) % 256 - 128), i);
    for (int j = 0; j < ny; ++j) {
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
    }
  }
  //idx<ubyte> B8u(nx), C8u(nx);
  idx<float32> B32f(nx, ny), BB32f(nx, ny), C32f(nx, ny);
  //ipp_std_normalize(A8u,  B8u);
  //idx_std_normalize(A8u,  C8u);
  ipp_std_normalize(A32f, B32f);
  float32 m = ipp_mean(A32f);
  ipp_std_normalize(A32f, BB32f, &m);
  idx_std_normalize(A32f, C32f);
  for (int i = 0; i < nx; ++i) {
    //CPPUNIT_ASSERT(B8u.get(i) == C8u.get(i));
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(approx_equals(B32f.get(i, j), C32f.get(i, j)));
      //      CPPUNIT_ASSERT(approx_equals(BB32f.get(i, j) / C32f.get(i, j), 1.0f));
    }
  }
}

void ippops_test::test_ipp_mean() {
  int nx = 42, ny = 32;
  idx<ubyte>  A8u(nx);
  idx<uint16> A16u(nx, ny);
  idx<int16>  A16s(nx, ny);
  idx<float32>  A32f(nx, ny);
  for (int i = 0; i < nx; ++i) {
    A8u.set(((184993 * i + 17) % 256 - 128), i);
    for (int j = 0; j < ny; ++j) {
      A16u.set(((184993 * i + 15493 * j + 17) % 65536), i, j);
      A16s.set(((184993 * i + 15493 * j + 532) % 32767), i, j);
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
    }
  }
  float64 r8u = 0, r16u = 0, r16s = 0, r32f = 0;
  for (int i = 0; i < nx; ++i) {
    r8u += A8u.get(i);
    for (int j = 0; j < ny; ++j) {
      r16u += A16u.get(i, j);
      r16s += A16s.get(i, j);
      r32f += A32f.get(i, j);
    }
  }
  r8u /= (float64)nx;
  r16u /= (float64)(nx*ny);
  r16s /= (float64)(nx*ny);
  r32f /= (float64)(nx*ny);
  CPPUNIT_ASSERT(approx_equals(ipp_mean(A8u),  r8u ));
  CPPUNIT_ASSERT(approx_equals(ipp_mean(A16u), r16u));
  CPPUNIT_ASSERT(approx_equals(ipp_mean(A16s), r16s));
  CPPUNIT_ASSERT(approx_equals(ipp_mean(A32f), r32f));
}

#ifdef __IPP_DOT__
void ippops_test::test_ipp_dot() {
  int nx = 42, ny = 32, nz = 23;
  idx<ubyte>  A8u(nx, ny), B8u(nx, ny);
  idx<byte>  A8s(nx), B8s(nx);
  idx<uint16> A16u(nx, ny), B16u(nx, ny);
  idx<int16>  A16s(nx, ny), B16s(nx, ny);
  idx<uint32>  A32u(nx, ny, nz), B32u(nx, ny, nz);
  idx<int32>  A32s(nx, ny), B32s(nx, ny);
  idx<float>  A32f(nx, ny), B32f(nx, ny);
  for (int i = 0; i < nx; ++i) {
    A8s.set(((184993 * i + 17) % 256 - 128), i);
    B8s.set(((1843 * i + 313) % 256 - 128), i);
    for (int j = 0; j < ny; ++j) {
      A8u.set(((184993 * i + 15493 * j + 19) % 256), i, j);
      B8u.set(((1849923 * i + 1493 * j + 149) % 256), i, j);
      A16u.set(((184993 * i + 15493 * j + 17) % 65536), i, j);
      B16u.set(((1893 * i + 154293 * j + 173) % 65536), i, j);
      A16s.set(((184993 * i + 15493 * j + 532) % 32767), i, j);
      B16s.set(((1843 * i + 151493 * j + 52) % 32767), i, j);
      A32s.set((184993 * i + 15493 * j + 532), i, j);
      B32s.set((18413193 * i + 1542414153 * j + 21323232), i, j);
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
      B32f.set(((1843.2 * i + 1549123.32 * j + 5332.1) * 6.173), i, j);
      for (int k = 0; k < nz; ++k) {
	A32u.set((184993 * i + 15493 * j + 11217 * k + 312), i, j, k);
	B32u.set((1384993 * i + 154932 * j + 1721 * k + 17), i, j, k);
      }
    }
  }
  float64 r8u = 0, r8s = 0, r16u = 0, r16s = 0, r32u = 0, r32s = 0, r32f = 0;
  for (int i = 0; i < nx; ++i) {
    r8s += A8s.get(i) * B8s.get(i);
    for (int j = 0; j < ny; ++j) {
      r8u  += A8u.get(i, j)  * B8u.get(i, j);
      r16u += A16u.get(i, j) * B16u.get(i, j);
      r16s += A16s.get(i, j) * B16s.get(i, j);
      r32s += A32s.get(i, j) * B32s.get(i, j);
      r32f += A32f.get(i, j) * B32f.get(i, j);
      for (int k = 0; k < nz; ++k) {
	r32u += A32u.get(i, j, k) * B32u.get(i, j, k);
      }
    }
  }
  CPPUNIT_ASSERT(approx_equals(ipp_dot(A8u,  B8u),  r8u ));
  CPPUNIT_ASSERT(approx_equals(ipp_dot(A8s,  B8s),  r8s ));
  CPPUNIT_ASSERT(approx_equals(ipp_dot(A16u, B16u), r16u));
  CPPUNIT_ASSERT(approx_equals(ipp_dot(A16s, B16s), r16s));
  CPPUNIT_ASSERT(approx_equals(ipp_dot(A32u, B32u), r32u));
  CPPUNIT_ASSERT(approx_equals(ipp_dot(A32s, B32s), r32s));
  CPPUNIT_ASSERT(approx_equals(ipp_dot(A32f, B32f), r32f));
  idx<ubyte> acc8u;
  idx<byte> acc8s;
  idx<uint16> acc16u;
  idx<int16> acc16s;
  idx<uint32> acc32u;
  idx<int32> acc32s;
  idx<float32> acc32f;
  acc8u.set(42);
  acc8s.set(42);
  acc16u.set(123);
  acc16s.set(2345);
  acc32u.set(323456789);
  acc32s.set(-123456);
  acc32f.set(-123.456);
  CPPUNIT_ASSERT(ipp_dotacc(A8u, B8u, acc8u) == r8u + 42.0);
  CPPUNIT_ASSERT(ipp_dotacc(A8s, B8s, acc8s) == r8s + 42.0);
  CPPUNIT_ASSERT(ipp_dotacc(A16u, B16u, acc16u) == r16u + 123.0);
  CPPUNIT_ASSERT(ipp_dotacc(A16s, B16s, acc16s) == r16s + 2345.0);
  CPPUNIT_ASSERT(ipp_dotacc(A32u, B32u, acc32u) == r32u + 323456789.0);
  CPPUNIT_ASSERT(approx_equals(ipp_dotacc(A32s, B32s, acc32s),
			       (r32s - 123456.0)));
  CPPUNIT_ASSERT(approx_equals(ipp_dotacc(A32f, B32f, acc32f),
			       (r32f - 123.456)));
  ubyte  r28u  = saturate(r8u + 42, ubyte);
  byte   r28s  = saturate(r8s + 42, byte);
  uint16 r216u = saturate(r16u + 123, uint16);
  int16  r216s = saturate(r16s + 2345, int16);
  uint32 r232u  = saturate(r32u + 323456789, uint32);
  int32 r232s  = saturate(r32s - 123456, int32);
  float32 r232f = r32f - 123.456f;
  CPPUNIT_ASSERT(r28u == acc8u.get());
  CPPUNIT_ASSERT(r28s == acc8s.get());
  CPPUNIT_ASSERT(r216u == acc16u.get());
  CPPUNIT_ASSERT(r216s == acc16s.get());
  CPPUNIT_ASSERT(r232u == acc32u.get());
  CPPUNIT_ASSERT(r232s == acc32s.get());
  CPPUNIT_ASSERT(approx_equals(r232f, acc32f.get()));
}
#endif

void ippops_test::test_ipp_max() {
  int nx = 12, ny = 34, nz = 3;
  idx<ubyte> A8u (nx, ny, nz);
  idx<uint16> A16u(nx, ny, nz);
  idx<int16> A16s(nx, ny, nz);
  idx<float32> A32f(nx, ny, nz);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k) {
	A8u.set((184993 * i + 15493 * j + 11217 * k + 312) % 256, i, j, k);
	A16u.set((184993 * i + 15493 * j + 11217 * k + 312) % 65536, i, j, k);
	A16s.set((184993 * i + 15493 * j + 11217 * k + 312) % 65536 - 32767,
		 i, j, k);
	A32f.set(((184993.2 * i + 15493.3 * j + 532.1 * k + 1.2) * 2.123),
		 i, j, k);
      }
  ubyte   m8u  = A8u.get(0, 0, 0);
  uint16  m16u = A16u.get(0, 0, 0);
  int16   m16s = A16s.get(0, 0, 0);
  float32 m32f = A32f.get(0, 0, 0);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k) {
	if (A8u.get(i, j, k) > m8u)
	  m8u = A8u.get(i, j, k);
	if (A16u.get(i, j, k) > m16u)
	  m16u = A16u.get(i, j, k);
	if (A16s.get(i, j, k) > m16s)
	  m16s = A16s.get(i, j, k);
	if (A32f.get(i, j, k) > m32f)
	  m32f = A32f.get(i, j, k);
      }
  CPPUNIT_ASSERT(m8u  == ipp_max(A8u));
  CPPUNIT_ASSERT(m16u == ipp_max(A16u));
  CPPUNIT_ASSERT(m16s == ipp_max(A16s));
  CPPUNIT_ASSERT(m32f == ipp_max(A32f));
  CPPUNIT_ASSERT(m8u   == ((ubyte*)  A8u.idx_ptr() )[ipp_indexmax(A8u )]);
  CPPUNIT_ASSERT(m16u  == ((uint16*) A16u.idx_ptr())[ipp_indexmax(A16u)]);
  CPPUNIT_ASSERT(m16s  == ((int16*)  A16s.idx_ptr())[ipp_indexmax(A16s)]);
  CPPUNIT_ASSERT(m32f  == ((float32*)A32f.idx_ptr())[ipp_indexmax(A32f)]);
}

void ippops_test::test_ipp_min() {
  int nx = 12, ny = 34, nz = 3;
  idx<ubyte> A8u (nx, ny, nz);
  idx<uint16> A16u(nx, ny, nz);
  idx<int16> A16s(nx, ny, nz);
  idx<float32> A32f(nx, ny, nz);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k) {
	A8u.set((184993 * i + 15493 * j + 11217 * k + 312) % 256, i, j, k);
	A16u.set((184993 * i + 15493 * j + 11217 * k + 312) % 65536, i, j, k);
	A16s.set((184993 * i + 15493 * j + 11217 * k + 312) % 65536 - 32767,
		 i, j, k);
	A32f.set(((184993.2 * i + 15493.3 * j + 532.1 * k + 1.2) * 2.123),
		 i, j, k);
      }
  ubyte   m8u  = A8u.get(0, 0, 0);
  uint16  m16u = A16u.get(0, 0, 0);
  int16   m16s = A16s.get(0, 0, 0);
  float32 m32f = A32f.get(0, 0, 0);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nz; ++k) {
	if (A8u.get(i, j, k) < m8u)
	  m8u = A8u.get(i, j, k);
	if (A16u.get(i, j, k) < m16u)
	  m16u = A16u.get(i, j, k);
	if (A16s.get(i, j, k) < m16s)
	  m16s = A16s.get(i, j, k);
	if (A32f.get(i, j, k) < m32f)
	  m32f = A32f.get(i, j, k);
      }
  CPPUNIT_ASSERT(m8u  == ipp_min(A8u));
  CPPUNIT_ASSERT(m16u == ipp_min(A16u));
  CPPUNIT_ASSERT(m16s == ipp_min(A16s));
  CPPUNIT_ASSERT(m32f == ipp_min(A32f));
  CPPUNIT_ASSERT(m8u   == ((ubyte*)  A8u.idx_ptr() )[ipp_indexmin(A8u )]);
  CPPUNIT_ASSERT(m16u  == ((uint16*) A16u.idx_ptr())[ipp_indexmin(A16u)]);
  CPPUNIT_ASSERT(m16s  == ((int16*)  A16s.idx_ptr())[ipp_indexmin(A16s)]);
  CPPUNIT_ASSERT(m32f  == ((float32*)A32f.idx_ptr())[ipp_indexmin(A32f)]);
}

void ippops_test::test_ipp_maxevery() {
  int nx = 15, ny = 12;
  idx<ubyte>   A8u (nx, ny), B8u (nx, ny), C8u (nx, ny), C28u (nx, ny);
  idx<uint16>  A16u(nx, ny), B16u(nx, ny), C16u(nx, ny), C216u(nx, ny);
  idx<int16>   A16s(nx, ny), B16s(nx, ny), C16s(nx, ny), C216s(nx, ny);
  idx<float32> A32f(nx, ny), B32f(nx, ny), C32f(nx, ny), C232f(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A8u.set((ubyte)(((184993*i + 15493*j) % 256)), i, j);
      A16u.set((uint16)(((184993*i + 15493*j) % 65536)), i, j);
      A16s.set((int16)(((184993*i + 15493*j) % 7561) - 4000), i, j);
      A32f.set(((184993*i + 15493*j) % 7561) *1.43 - 4000, i, j);
      B8u.set((ubyte)(((182993*i + 15445*j) % 256)), i, j);
      B16u.set((uint16)(((132393*i + 151234*j) % 65536)), i, j);
      B16s.set((int16)(((18435233*i + 15493*j) % 12345) - 4000), i, j);
      B32f.set(((183293*i + 15233*j) % 7461) *1.43 - 4000, i, j);
    }
  ipp_copy(B8u, C28u);
  ipp_copy(B16u, C216u);
  ipp_copy(B16s, C216s);
  ipp_copy(B32f, C232f);
  ipp_maxevery(A8u, B8u, C8u);
  ipp_maxevery(A16u, B16u, C16u);
  ipp_maxevery(A16s, B16s, C16s);
  ipp_maxevery(A32f, B32f, C32f);
  ipp_maxevery(A8u, C28u);
  ipp_maxevery(A16u, C216u);
  ipp_maxevery(A16s, C216s);
  ipp_maxevery(A32f, C232f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(C8u.get(i, j) == std::max(A8u.get(i, j), B8u.get(i, j)));
      CPPUNIT_ASSERT(C28u.get(i, j) == std::max(A8u.get(i, j), B8u.get(i, j)));
      CPPUNIT_ASSERT(C16u.get(i, j) == std::max(A16u.get(i, j), B16u.get(i, j)));
      CPPUNIT_ASSERT(C216u.get(i, j) == std::max(A16u.get(i, j), B16u.get(i, j)));
      CPPUNIT_ASSERT(C16s.get(i, j) == std::max(A16s.get(i, j), B16s.get(i, j)));
      CPPUNIT_ASSERT(C216s.get(i, j) == std::max(A16s.get(i, j), B16s.get(i, j)));
      CPPUNIT_ASSERT(approx_equals(C32f.get(i, j),
				   std::max(A32f.get(i, j), B32f.get(i, j))));
      CPPUNIT_ASSERT(approx_equals(C232f.get(i, j),
				   std::max(A32f.get(i, j), B32f.get(i, j))));
    }
}

void ippops_test::test_ipp_sqrdist() {
  int nx = 15, ny = 12;
  idx<ubyte>   A8u (nx, ny), B8u (nx, ny), C8u (nx, ny), D8u (nx, ny);
  idx<uint16>  A16u(nx, ny), B16u(nx, ny), C16u(nx, ny), D16u(nx, ny);
  idx<int16>   A16s(nx, ny), B16s(nx, ny), C16s(nx, ny);
  idx<float32> A32f(nx, ny), B32f(nx, ny), C32f(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A8u.set((ubyte)(((184993*i + 15493*j) % 256)), i, j);
      A16u.set((uint16)(((184993*i + 15493*j) % 65536)), i, j);
      A16s.set((int16)(((184993*i + 15493*j) % 7561) - 4000), i, j);
      A32f.set(((184993*i + 15493*j) % 7561) *1.43 - 4000, i, j);
      B8u.set((ubyte)(((182993*i + 15445*j) % 256)), i, j);
      B16u.set((uint16)(((132393*i + 151234*j) % 65536)), i, j);
      B16s.set((int16)(((18435233*i + 15493*j) % 12345) - 4000), i, j);
      B32f.set(((183293*i + 15233*j) % 7461) *1.43 - 4000, i, j);
    }
  ipp_sub(A8u, B8u, C8u);
  ipp_sub(A16u, B16u, C16u);
  ipp_sub(A16s, B16s, C16s);
  ipp_sub(A32f, B32f, C32f);
  ipp_sub(B8u, A8u, D8u);
  ipp_sub(B16u, A16u, D16u);
  ipp_add(D8u, C8u);
  ipp_add(D16u, C16u);
  float64 e8u = ipp_l2norm(C8u), e16u = ipp_l2norm(C16u);
  float64 e16s = ipp_l2norm(C16s), e32f = ipp_l2norm(C32f);
  CPPUNIT_ASSERT(approx_equals(ipp_sqrdist(A8u, B8u), e8u));
  CPPUNIT_ASSERT(approx_equals(ipp_sqrdist(A16u, B16u), e16u));
  CPPUNIT_ASSERT(approx_equals(ipp_sqrdist(A16s, B16s), e16s));
  CPPUNIT_ASSERT(approx_equals(ipp_sqrdist(A32f, B32f), e32f));
  idx<ubyte> acc8u;
  idx<uint16> acc16u;
  idx<int16> acc16s;
  idx<float32> acc32f;
  CPPUNIT_ASSERT(approx_equals(ipp_sqrdist(A8u, B8u, acc8u), e8u));
  CPPUNIT_ASSERT(approx_equals(ipp_sqrdist(A16u, B16u, acc16u), e16u));
  CPPUNIT_ASSERT(approx_equals(ipp_sqrdist(A16s, B16s, acc16s), e16s));
  CPPUNIT_ASSERT(approx_equals(ipp_sqrdist(A32f, B32f, acc32f), e32f));
  ubyte  r8u  = (e8u > 255) ? 255 : (ubyte)e8u;
  uint16 r16u = (e16u > 65535) ? 65535 : (uint16)e16u;
  int16  r16s = (e16s > 32767.) ? 32767 : ((e16s < -32768.) ? -32768 : (int16)e16s);
  CPPUNIT_ASSERT(r8u  == acc8u.get());
  CPPUNIT_ASSERT(r16u == acc16u.get());
  CPPUNIT_ASSERT(r16s == acc16s.get());
  CPPUNIT_ASSERT(approx_equals(e32f, acc32f.get()));
}

void ippops_test::test_ipp_threshold() {
  int nx = 42, ny = 33;
  idx<ubyte>  A8u(nx, ny);
  idx<uint16> A16u(nx, ny);
  idx<int16>  A16s(nx, ny);
  idx<float32>  A32f(nx, ny);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      A8u.set(((184993 * i + 15493 * j + 19) % 256), i, j);
      A16u.set(((184993 * i + 15493 * j + 17) % 65536), i, j);
      A16s.set(((184993 * i + 15493 * j + 532) % 32767), i, j);
      A32f.set(((184993.2 * i + 15493.3 * j + 532.1) * 2.123), i, j);
    }
  idx<ubyte>  B8u (nx, ny), B8u2 (nx, ny);
  idx<uint16> B16u(nx, ny), B16u2(nx, ny);
  idx<int16>  B16s(nx, ny), B16s2(nx, ny);
  idx<float32>  B32f(nx, ny), B32f2(nx, ny);
  // lt
  //8u
  ubyte th8u = 42;
  idx_copy(A8u, B8u2);
  ipp_threshold_lt(B8u2, th8u);
  ipp_threshold_lt(A8u, th8u, B8u);
  //16u
  uint16 th16u = 412;
  idx_copy(A16u, B16u2);
  ipp_threshold_lt(B16u2, th16u);
  ipp_threshold_lt(A16u, th16u, B16u);
  //16s
  int16 th16s = -1231;
  idx_copy(A16s, B16s2);
  ipp_threshold_lt(B16s2, th16s);
  ipp_threshold_lt(A16s, th16s, B16s);
  //32f
  float32 th32f = 1231.12;
  idx_copy(A32f, B32f2);
  ipp_threshold_lt(B32f2, th32f);
  ipp_threshold_lt(A32f, th32f, B32f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(B8u.get(i, j) ==
		     ((A8u.get(i, j) < th8u) ? th8u : A8u.get(i, j)));
      CPPUNIT_ASSERT(B8u2.get(i, j) ==
		     ((A8u.get(i, j) < th8u) ? th8u : A8u.get(i, j)));
      CPPUNIT_ASSERT(B16u.get(i, j) ==
		     ((A16u.get(i, j) < th16u) ? th16u : A16u.get(i, j)));
      CPPUNIT_ASSERT(B16u2.get(i, j) ==
		     ((A16u.get(i, j) < th16u) ? th16u : A16u.get(i, j)));
      CPPUNIT_ASSERT(B16s.get(i, j) ==
		     ((A16s.get(i, j) < th16s) ? th16s : A16s.get(i, j)));
      CPPUNIT_ASSERT(B16s2.get(i, j) ==
		     ((A16s.get(i, j) < th16s) ? th16s : A16s.get(i, j)));
	CPPUNIT_ASSERT(approx_equals(B32f.get(i, j),
				     ((A32f.get(i, j) < th32f) ? th32f :
				      A32f.get(i, j))));
      CPPUNIT_ASSERT(approx_equals(B32f2.get(i, j),
				   ((A32f.get(i, j) < th32f) ? th32f :
				    A32f.get(i, j))));
    }

  // gt
  //8u
  idx_copy(A8u, B8u2);
  ipp_threshold_gt(B8u2, th8u);
  ipp_threshold_gt(A8u, th8u, B8u);
  //16u
  idx_copy(A16u, B16u2);
  ipp_threshold_gt(B16u2, th16u);
  ipp_threshold_gt(A16u, th16u, B16u);
  //16s
  idx_copy(A16s, B16s2);
  ipp_threshold_gt(B16s2, th16s);
  ipp_threshold_gt(A16s, th16s, B16s);
  //32f
  idx_copy(A32f, B32f2);
  ipp_threshold_gt(B32f2, th32f);
  ipp_threshold_gt(A32f, th32f, B32f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(B8u.get(i, j) ==
		     ((A8u.get(i, j) > th8u) ? th8u : A8u.get(i, j)));
      CPPUNIT_ASSERT(B8u2.get(i, j) ==
		     ((A8u.get(i, j) > th8u) ? th8u : A8u.get(i, j)));
      CPPUNIT_ASSERT(B16u.get(i, j) ==
		     ((A16u.get(i, j) > th16u) ? th16u : A16u.get(i, j)));
      CPPUNIT_ASSERT(B16u2.get(i, j) ==
		     ((A16u.get(i, j) > th16u) ? th16u : A16u.get(i, j)));
      CPPUNIT_ASSERT(B16s.get(i, j) ==
		     ((A16s.get(i, j) > th16s) ? th16s : A16s.get(i, j)));
      CPPUNIT_ASSERT(B16s2.get(i, j) ==
		     ((A16s.get(i, j) > th16s) ? th16s : A16s.get(i, j)));
	CPPUNIT_ASSERT(approx_equals(B32f.get(i, j),
				     ((A32f.get(i, j) > th32f) ? th32f :
				      A32f.get(i, j))));
      CPPUNIT_ASSERT(approx_equals(B32f2.get(i, j),
				   ((A32f.get(i, j) > th32f) ? th32f :
				    A32f.get(i, j))));
    }

  // lt val
  //8u
  ubyte v8u = 45;
  idx_copy(A8u, B8u2);
  ipp_threshold_lt(B8u2, th8u, v8u);
  ipp_threshold_lt(A8u, th8u, v8u, B8u);
  //16u
  uint16 v16u = 4;
  idx_copy(A16u, B16u2);
  ipp_threshold_lt(B16u2, th16u, v16u);
  ipp_threshold_lt(A16u, th16u, v16u, B16u);
  //16s
  int16 v16s = 231;
  idx_copy(A16s, B16s2);
  ipp_threshold_lt(B16s2, th16s, v16s);
  ipp_threshold_lt(A16s, th16s, v16s, B16s);
  //32f
  float32 v32f = -11.12;
  idx_copy(A32f, B32f2);
  ipp_threshold_lt(B32f2, th32f, v32f);
  ipp_threshold_lt(A32f, th32f, v32f, B32f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(B8u.get(i, j) ==
		     ((A8u.get(i, j) < th8u) ? v8u : A8u.get(i, j)));
      CPPUNIT_ASSERT(B8u2.get(i, j) ==
		     ((A8u.get(i, j) < th8u) ? v8u : A8u.get(i, j)));
      CPPUNIT_ASSERT(B16u.get(i, j) ==
		     ((A16u.get(i, j) < th16u) ? v16u : A16u.get(i, j)));
      CPPUNIT_ASSERT(B16u2.get(i, j) ==
		     ((A16u.get(i, j) < th16u) ? v16u : A16u.get(i, j)));
      CPPUNIT_ASSERT(B16s.get(i, j) ==
		     ((A16s.get(i, j) < th16s) ? v16s : A16s.get(i, j)));
      CPPUNIT_ASSERT(B16s2.get(i, j) ==
		     ((A16s.get(i, j) < th16s) ? v16s : A16s.get(i, j)));
	CPPUNIT_ASSERT(approx_equals(B32f.get(i, j),
				     ((A32f.get(i, j) < th32f) ? v32f :
				      A32f.get(i, j))));
      CPPUNIT_ASSERT(approx_equals(B32f2.get(i, j),
				   ((A32f.get(i, j) < th32f) ? v32f :
				    A32f.get(i, j))));
    }

  // gt val
  //8u
  idx_copy(A8u, B8u2);
  ipp_threshold_gt(B8u2, th8u, v8u);
  ipp_threshold_gt(A8u, th8u, v8u, B8u);
  //16u
  idx_copy(A16u, B16u2);
  ipp_threshold_gt(B16u2, th16u, v16u);
  ipp_threshold_gt(A16u, th16u, v16u, B16u);
  //16s
  idx_copy(A16s, B16s2);
  ipp_threshold_gt(B16s2, th16s, v16s);
  ipp_threshold_gt(A16s, th16s, v16s, B16s);
  //32f
  idx_copy(A32f, B32f2);
  ipp_threshold_gt(B32f2, th32f, v32f);
  ipp_threshold_gt(A32f, th32f, v32f, B32f);
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j) {
      CPPUNIT_ASSERT(B8u.get(i, j) ==
		     ((A8u.get(i, j) > th8u) ? v8u : A8u.get(i, j)));
      CPPUNIT_ASSERT(B8u2.get(i, j) ==
		     ((A8u.get(i, j) > th8u) ? v8u : A8u.get(i, j)));
      CPPUNIT_ASSERT(B16u.get(i, j) ==
		     ((A16u.get(i, j) > th16u) ? v16u : A16u.get(i, j)));
      CPPUNIT_ASSERT(B16u2.get(i, j) ==
		     ((A16u.get(i, j) > th16u) ? v16u : A16u.get(i, j)));
      CPPUNIT_ASSERT(B16s.get(i, j) ==
		     ((A16s.get(i, j) > th16s) ? v16s : A16s.get(i, j)));
      CPPUNIT_ASSERT(B16s2.get(i, j) ==
		     ((A16s.get(i, j) > th16s) ? v16s : A16s.get(i, j)));
	CPPUNIT_ASSERT(approx_equals(B32f.get(i, j),
				     ((A32f.get(i, j) > th32f) ? v32f :
				      A32f.get(i, j))));
      CPPUNIT_ASSERT(approx_equals(B32f2.get(i, j),
				   ((A32f.get(i, j) > th32f) ? v32f :
				    A32f.get(i, j))));
    }
}

#endif /* __IPP__ */
