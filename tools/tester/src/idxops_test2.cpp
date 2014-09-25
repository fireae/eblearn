#include "idxops_test2.h"

using namespace ebl;
using namespace std;

void idxops_test2::setUp() {
}

void idxops_test2::tearDown() {
}

const float64 epsilon = 1e-4;
template <typename T> bool EQUAL_UTEST(T a1, T a2) {
  float64 amax, amin;
  if (std::abs((float64) a1) < std::abs((float64)a2)) {
    amax = (float64)a2;
    amin = (float64)a1;
  } else {
    amax = (float64)a1;
    amin = (float64)a2;
  }
  if (isinf(std::abs(amax)))
    return amin == amax;
  if (std::abs(amax) > 1e-30)
    return (1.0 - epsilon < amin / amax) && (amin / amax < 1.0 + epsilon);
  return true;
}

template <typename T> void ASSERT_EQUAL(T a1, T a2) {
  if (!EQUAL_UTEST(a1, a2)) {
      std::cout << std::endl
	   << a1 << " (" << (int)a1 << ") "
	   << a2 << " (" << (int)a2 << ")" << std::endl;
      CPPUNIT_ASSERT(false);
  }
}

template <typename T> void ASSERT_EQUAL2(T a1, T a2, T a3) {
  if (!(EQUAL_UTEST(a1, a2) || EQUAL_UTEST(a1, a3))) {
    std::cout << std::endl
	 << a1 << " (" << (int)a1 << ") "
	 << a2 << " (" << (int)a2 << ") "
	 << a3 << " (" << (int)a3 << ")" << std::endl;
    CPPUNIT_ASSERT(false);
  }
}

template <typename T> T test_rand (intg it1, intg it2, intg it3, intg it4) {
  return (T)(drand() * 255);
}

template <> ubyte test_rand<ubyte> (intg it1, intg it2, intg it3, intg it4) {
  return (ubyte)((it1 + 20*it2 + 300*it3 + 4000*it4 + (int)(4242*drand())) % 256);
}

template <> byte test_rand<byte> (intg it1, intg it2, intg it3, intg it4) {
  return (byte)(((it1 + 20*it2 + 300*it3 + 4000*it4 +
		  (int)(4242*drand())) % 256) - 128);
}

template <> uint16 test_rand<uint16> (intg it1, intg it2, intg it3, intg it4) {
  return (uint16)((it1 + 20*it2 + 300*it3 + 4000*it4 + (int)(4242*drand())) % 65536);
}

template <> int16 test_rand<int16> (intg it1, intg it2, intg it3, intg it4) {
  return (int16)(((it1 + 20*it2 + 300*it3 + 4000*it4 +
		   (int)(4242*drand())) % 65536) - 32768);
}

template <> uint32 test_rand<uint32> (intg it1, intg it2, intg it3, intg it4) {
  return (uint32)((uint32)it1 + 200u*(uint32)it2 + 30000u*(uint32)it3 +
		   500000000u*(int32)it4 + (uint32)(424242*drand()));
}

template <> int32 test_rand<int32> (intg it1, intg it2, intg it3, intg it4) {
  return (int32)(it1 + 200*it2 + 30000*it3 + 100000000*it4 + (int32)(424242*drand()));
}

template <> float32 test_rand<float32> (intg it1, intg it2, intg it3, intg it4) {
  return (float32)((12.3f * (float32)it1 + 2023.423f*(float32)it2 +
		    3002142.5f*(float32)it3 + 40043.32f*(float32)it4 +
		    4242.4f*(float32)drand()) * ((float32)drand() - 0.5f));
}

template <> float64 test_rand<float64> (intg it1, intg it2, intg it3, intg it4) {
  return (float64)((12.3 * (float64)it1 + 2023.423*(float64)it2 +
		    3002142.5*(float64)it3 + 40043.32e4*(float64)it4 +
		    4242.4*(float64)drand()) * ((float64)drand() - 0.5));
}

template <typename T> idx<T> get_test_idx(intg i1, intg i2, intg i3, intg i4) {
  idx<T> m1(i1, i2, i3, i4);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  m1.set(test_rand<T>(it1, it2, it3, it4), it1, it2, it3, it4);
	}
  return m1;
}

template <typename T> idx<T> get_test_idx_nc(intg i1, intg i2, intg i3, intg i4) {
  idx<T> m1(i1 + 4, i2 + 6, i3 + 2, i4 + 4);
  for (intg it1 = 0; it1 < i1+4; ++it1)
    for (intg it2 = 0; it2 < i2+6; ++it2)
      for (intg it3 = 0; it3 < i3+2; ++it3)
	for (intg it4 = 0; it4 < i4+4; ++it4) {
	  m1.set(test_rand<T>(it1, it2, it3, it4), it1, it2, it3, it4);
	}
  return m1.narrow(0, i1, 2).narrow(1, i2, 3).narrow(2, i3, 0).narrow(3, i4, 1);
}



template <typename T> void test_copy(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4);
  idx_copy(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(m1.get(it1, it2, it3, it4), mt.get(it1, it2, it3, it4));
	}
}

void idxops_test2::idx_copy2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_copy(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_copy(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}

template <typename T> void test_clear(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4);
  idx_copy(m1, mt);
  idx_clear(mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4), (T)0);
	}
}

void idxops_test2::idx_clear2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_clear(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_clear(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));

}

template <typename T> void test_fill(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1, T p) {
  idx<T> mt(i1, i2, i3, i4);
  idx_copy(m1, mt);
  idx_fill(mt, p);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4), p);
	}
}

void idxops_test2::idx_fill2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_fill(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)42);
  test_fill(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)42);
  test_fill(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4), (uint16)123);
  test_fill(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)(-3211)) ;
  test_fill(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 3000000000u);
  test_fill(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 13246543);
  test_fill(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4), 12341.423f);
  test_fill(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4), 1324231.54e42);
  test_fill(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)42);
  test_fill(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)42);
  test_fill(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4), (uint16)123);
  test_fill(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)(-3211)) ;
  test_fill(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 3000000000u);
  test_fill(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 13246543);
  test_fill(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4), 12341.423f);
  test_fill(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4), 1324231.54e42);

}


template <typename T> void test_add(intg i1, intg i2, intg i3, intg i4,
				    idx<T> m1, idx<T> m2) {
  idx<T> mt(i1, i2, i3, i4), mt2(i1, i2, i3, i4);
  idx_copy(m2, mt);
  idx_add(m1, mt);
  idx_add(m1, m2, mt2);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
#ifdef __IPP__
	  ASSERT_EQUAL2(mt.get(it1, it2, it3, it4),
			saturate(m1.get(it1,it2,it3,it4)+m2.get(it1,it2,it3,it4), T),
			(T)(m1.get(it1,it2,it3,it4)+m2.get(it1,it2,it3,it4)));
	  ASSERT_EQUAL2(mt2.get(it1, it2, it3, it4),
			saturate(m1.get(it1,it2,it3,it4)+m2.get(it1,it2,it3,it4), T),
			(T)(m1.get(it1,it2,it3,it4)+m2.get(it1,it2,it3,it4)));
#else
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)(m1.get(it1,it2,it3,it4)+m2.get(it1,it2,it3,it4)));
	  ASSERT_EQUAL(mt2.get(it1, it2, it3, it4),
		       (T)(m1.get(it1,it2,it3,it4)+m2.get(it1,it2,it3,it4)));
#endif
	}
}

void idxops_test2::idx_add2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_add(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4),
	    get_test_idx<ubyte>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
	    get_test_idx<byte>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4),
	    get_test_idx<uint16>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),
	    get_test_idx<int16>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4),
	    get_test_idx<uint32>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4),
	    get_test_idx<int32>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4),
	   get_test_idx<float32>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4),
	    get_test_idx<float64>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4),
	    get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
	    get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4),
	    get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),
	    get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4),
	    get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4),
	    get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4),
	   get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_add(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4),
	    get_test_idx_nc<float64>(i1, i2, i3, i4));
}

template <typename T> void test_addc(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1, T p) {
  idx<T> mt(i1, i2, i3, i4);
  idx_addc(m1, p, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
#ifdef __IPP__
	  ASSERT_EQUAL2(mt.get(it1, it2, it3, it4),
			saturate(m1.get(it1,it2,it3,it4) + p, T),
			(T)(m1.get(it1,it2,it3,it4) + p));
#else
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)(m1.get(it1,it2,it3,it4) + p));
#endif
	}
}

void idxops_test2::idx_addc2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_addc(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)42);
  test_addc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)80);
  test_addc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)(-120));
  test_addc(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4), (uint16)123);
  test_addc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)40000);
  test_addc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)(-3211));
  test_addc(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 3000000u);
  test_addc(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 3000000000u);
  test_addc(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 13246543);
  test_addc(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 2000000000);
  test_addc(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4), 12341.423f);
  test_addc(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4), 1324231.54e42);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)42);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)80);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)(-120));
  test_addc(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4), (uint16)123);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)40000);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)(-3211));
  test_addc(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 3000000u);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 3000000000u);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 13246543);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 2000000000);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4), 12341.423f);
  test_addc(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4), 1324231.54e42);
}

template <typename T> void test_addc_bounded(intg i1, intg i2, intg i3, intg i4,
					     idx<T> m1, T p) {
  idx<T> mt(i1, i2, i3, i4);
  idx_addc_bounded(m1, p, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       saturate(m1.get(it1,it2,it3,it4) + p, T));
	}
}

void idxops_test2::idx_addc_bounded2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)42);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)80);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)(-120));
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4), (uint16)123);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)40000);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),(int16)(-3211));
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 3000000u);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 3000000000u);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 13246543);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 2000000000);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4), 12341.423f);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3,i4),1324231.54e42);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)42);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)80);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2,i3,i4), (byte)(-120));
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2,i3,i4), (uint16)123);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2,i3,i4), (int16)40000);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<int16>(i1,i2,i3,i4),(int16)(-3211));
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 3000000u);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1,i2,i3, i4), 3000000000u);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 13246543);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 2000000000);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<float32>(i1,i2,i3, i4), 12341.423f);
  test_addc_bounded(i1, i2, i3, i4, get_test_idx_nc<float64>(i1,i2,i3,i4),132231.54e42);
}

template <typename T> void test_sub(intg i1, intg i2, intg i3, intg i4,
				    idx<T> m1, idx<T> m2) {
  idx<T> mt(i1, i2, i3, i4);
  idx_sub(m1, m2, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
#ifdef __IPP__
	  ASSERT_EQUAL2(mt.get(it1, it2, it3, it4),
			saturate(m1.get(it1,it2,it3,it4)-m2.get(it1,it2,it3,it4), T),
			(T)(m1.get(it1,it2,it3,it4)-m2.get(it1,it2,it3,it4)));
#else
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)(m1.get(it1,it2,it3,it4)-m2.get(it1,it2,it3,it4)));
#endif
	}
}

void idxops_test2::idx_sub2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_sub(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4),
	    get_test_idx<ubyte>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
	    get_test_idx<byte>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4),
	    get_test_idx<uint16>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),
	    get_test_idx<int16>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4),
	    get_test_idx<uint32>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4),
	    get_test_idx<int32>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4),
	   get_test_idx<float32>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4),
	    get_test_idx<float64>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4),
	    get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
	    get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4),
	    get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),
	    get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4),
	    get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4),
	    get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4),
	   get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_sub(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4),
	    get_test_idx_nc<float64>(i1, i2, i3, i4));
}


template <typename T> void test_subc_bounded(intg i1, intg i2, intg i3, intg i4,
					     idx<T> m1, T p) {
  idx<T> mt(i1, i2, i3, i4);
  idx_subc_bounded(m1, p, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       saturate(m1.get(it1,it2,it3,it4)-p, T));
	}
}

void idxops_test2::idx_subc_bounded2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)42);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)80);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)(-120));
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4), (uint16)123);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)40000);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),(int16)(-3211));
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 3000000u);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 3000000000u);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 13246543);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 2000000000);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4), 12341.423f);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3,i4),1324231.54e42);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)42);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)80);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<byte>(i1,i2, i3, i4), (byte)(-120));
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1,i2,i3, i4), (uint16)123);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<int16>(i1,i2,i3, i4), (int16)40000);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<int16>(i1,i2,i3,i4),(int16)(-3211));
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 3000000u);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1,i2,i3, i4), 3000000000u);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 13246543);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 2000000000);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<float32>(i1,i2,i3, i4), 12341.423f);
  test_subc_bounded(i1, i2, i3, i4, get_test_idx_nc<float64>(i1,i2,i3,i4),134231.54e42);
}


template <typename T> void test_minus(intg i1, intg i2, intg i3, intg i4,
				      idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4);
  idx_minus(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)(-m1.get(it1,it2,it3,it4)));
	}
}

void idxops_test2::idx_minus2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_minus(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_minus(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}


template <typename T> void test_mul(intg i1, intg i2, intg i3, intg i4,
				    idx<T> m1, idx<T> m2) {
  idx<T> mt(i1, i2, i3, i4);
  idx_mul(m1, m2, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
#ifdef __IPP__
	  ASSERT_EQUAL2(mt.get(it1, it2, it3, it4),
			saturate((float64)m1.get(it1,it2,it3,it4)*(float64)m2.get(it1,it2,it3,it4), T),
		       (T)(m1.get(it1,it2,it3,it4)*m2.get(it1,it2,it3,it4)));
#else
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)(m1.get(it1,it2,it3,it4)*m2.get(it1,it2,it3,it4)));
#endif
	}
}

void idxops_test2::idx_mul2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_mul(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4),
	    get_test_idx<ubyte>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
	    get_test_idx<byte>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4),
	    get_test_idx<uint16>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),
	    get_test_idx<int16>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4),
	    get_test_idx<uint32>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4),
	    get_test_idx<int32>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4),
	   get_test_idx<float32>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4),
	    get_test_idx<float64>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4),
	    get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
	    get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4),
	    get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),
	    get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4),
	    get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4),
	    get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4),
	   get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_mul(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4),
	    get_test_idx_nc<float64>(i1, i2, i3, i4));
}


template <typename T> void test_dotc(intg i1, intg i2, intg i3, intg i4,
				    idx<T> m1, T p) {
  idx<T> mt(i1, i2, i3, i4);
  idx_dotc(m1, p, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
#ifdef __IPP__
	  ASSERT_EQUAL2(mt.get(it1, it2, it3, it4),
			saturate(m1.get(it1,it2,it3,it4)*p, T),
			(T)(m1.get(it1,it2,it3,it4)*p));
#else
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)(m1.get(it1,it2,it3,it4)*p));
#endif
	}
}

void idxops_test2::idx_dotc2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_dotc(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)5);
  test_dotc(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)50);
  test_dotc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)2);
  test_dotc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)(-12));
  test_dotc(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4), (uint16)13);
  test_dotc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)400);
  test_dotc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),(int16)(-31));
  test_dotc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),(int16)(-3112));
  test_dotc(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 300u);
  test_dotc(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 300000000u);
  test_dotc(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 13246543);
  test_dotc(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), -200000);
  test_dotc(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4), 12341.423f);
  test_dotc(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3,i4),-1324231.54e42);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)5);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)50);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)2);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)(-12));
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4), (uint16)13);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)400);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),(int16)(-31));
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),(int16)(-3112));
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 300u);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 300000000u);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 13246543);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), -200000);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4), 12341.423f);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3,i4),-1324231.54e42);
}


template <typename T> void test_dotc_bounded(intg i1, intg i2, intg i3, intg i4,
				    idx<T> m1, T p) {
  idx<T> mt(i1, i2, i3, i4);
  idx_dotc_bounded(m1, p, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       saturate(m1.get(it1,it2,it3,it4)*p, T));
	}
}

void idxops_test2::idx_dotc_bounded2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_dotc(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)5);
  test_dotc(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)50);
  test_dotc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)2);
  test_dotc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)(-12));
  test_dotc(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4), (uint16)13);
  test_dotc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)400);
  test_dotc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),(int16)(-31));
  test_dotc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),(int16)(-3112));
  test_dotc(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 300u);
  test_dotc(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 300000000u);
  test_dotc(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 13246543);
  test_dotc(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), -200000);
  test_dotc(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4), 12341.423f);
  test_dotc(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3,i4),-1324231.54e42);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)5);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)50);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)2);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)(-12));
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4), (uint16)13);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)400);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),(int16)(-31));
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),(int16)(-3112));
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 300u);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 300000000u);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 13246543);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), -200000);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4), 12341.423f);
  test_dotc(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3,i4),-1324231.54e42);
}


template <typename T> void test_signdotc(intg i1, intg i2, intg i3, intg i4,
					 idx<T> m1, T p) {
  idx<T> mt(i1, i2, i3, i4);
  idx_signdotc(m1, p, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (m1.get(it1,it2,it3,it4) >= 0) ? p : (T)(-p));
	}
}

void idxops_test2::idx_signdotc2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_signdotc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)(-120));
  test_signdotc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)40000);
  test_signdotc(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 13246543);
  test_signdotc(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4), 12341.423f);
  test_signdotc(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4), 1324231.54e42);
  test_signdotc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)(-120));
  test_signdotc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)40000);
  test_signdotc(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 13246543);
  test_signdotc(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4), 12341.423f);
  test_signdotc(i1, i2, i3, i4, get_test_idx_nc<float64>(i1,i2, i3, i4), 1324231.54e42);
}

template <typename T> void test_div(intg i1, intg i2, intg i3, intg i4,
					 idx<T> m1, idx<T> m2) {
  idx<T> mt(i1, i2, i3, i4), mt2(i1, i2, i3, i4);
  idx_copy(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  if (mt.get(it1, it2, it3, it4) == 0)
	    mt.set((T)1, it1, it2, it3, it4);
	}
  idx_div(m1, mt, mt2);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt2.get(it1, it2, it3, it4),
		       (T)(m1.get(it1,it2,it3,it4)/mt.get(it1,it2,it3,it4)));
	}
}

void idxops_test2::idx_div2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_div(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4),
	    get_test_idx<ubyte>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
	    get_test_idx<byte>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4),
	    get_test_idx<uint16>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),
	    get_test_idx<int16>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4),
	    get_test_idx<uint32>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4),
	    get_test_idx<int32>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4),
	    get_test_idx<float32>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4),
	    get_test_idx<float64>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4),
	    get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
	    get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4),
	    get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),
	    get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4),
	    get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4),
	    get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4),
	    get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_div(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4),
	    get_test_idx_nc<float64>(i1, i2, i3, i4));
}


template <typename T> void test_inv(intg i1, intg i2, intg i3, intg i4,
					 idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4), mt2(i1, i2, i3, i4);
  idx_copy(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  if (mt.get(it1, it2, it3, it4) == 0)
	    mt.set((T)1, it1, it2, it3, it4);
	}
  idx_inv(mt, mt2);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt2.get(it1, it2, it3, it4),
		       (T)((T)1 / mt.get(it1,it2,it3,it4)));
	}
}

void idxops_test2::idx_inv2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_inv(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_inv(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}

template <typename T> void test_signdotcacc(intg i1, intg i2, intg i3, intg i4,
					    idx<T> m1, idx<T> m2, T p) {
  idx<T> mt(i1, i2, i3, i4);
  idx_copy(m2, mt);
  idx_signdotcacc(m1, p, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  if (m1.get(it1, it2, it3, it4) >= 0)
	    ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
			 (T)(m2.get(it1, it2, it3, it4) + p));
	  else
	    ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
			 (T)(m2.get(it1, it2, it3, it4) - p));
	}
}

void idxops_test2::idx_signdotcacc2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_signdotcacc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
	   get_test_idx<byte>(i1, i2, i3, i4), (byte)65);
  test_signdotcacc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
	   get_test_idx<byte>(i1, i2, i3, i4), (byte)(-65));
  test_signdotcacc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),
	   get_test_idx<int16>(i1, i2, i3, i4), (int16)6531);
  test_signdotcacc(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4),
	   get_test_idx<int32>(i1, i2, i3, i4), (int32)6554321);
  test_signdotcacc(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4),
	   get_test_idx<float32>(i1, i2, i3, i4), (float32)65.332f);
  test_signdotcacc(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4),
	   get_test_idx<float64>(i1, i2, i3, i4), (float64)65.12);

  test_signdotcacc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
	   get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)65);
  test_signdotcacc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
	   get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)(-65));
  test_signdotcacc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),
	   get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)6531);
  test_signdotcacc(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4),
	   get_test_idx_nc<int32>(i1, i2, i3, i4), (int32)6554321);
  test_signdotcacc(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4),
	   get_test_idx_nc<float32>(i1, i2, i3, i4), (float32)65.332f);
  test_signdotcacc(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4),
	   get_test_idx_nc<float64>(i1, i2, i3, i4), (float64)65.12);
}

template <typename T> void test_subsquare(intg i1, intg i2, intg i3, intg i4,
					  idx<T> m1, idx<T> m2) {
  idx<T> mt(i1, i2, i3, i4);
  idx_subsquare(m1, m2, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  T t = m1.get(it1, it2, it3, it4) - m2.get(it1, it2, it3, it4);
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4), (T)(t*t));
	}
}

void idxops_test2::idx_subsquare2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_subsquare(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4),
	    get_test_idx<ubyte>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
	    get_test_idx<byte>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4),
	    get_test_idx<uint16>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),
	    get_test_idx<int16>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4),
	    get_test_idx<uint32>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4),
	    get_test_idx<int32>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4),
	   get_test_idx<float32>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4),
	    get_test_idx<float64>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4),
	    get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
	    get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4),
	    get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),
	    get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4),
	    get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4),
	    get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4),
	   get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_subsquare(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4),
	    get_test_idx_nc<float64>(i1, i2, i3, i4));
}

template <typename T> void test_lincomb(intg i1, intg i2, intg i3, intg i4,
					idx<T> m1, idx<T> m2, T p1, T p2) {
  idx<T> mt(i1, i2, i3, i4);
  idx_lincomb(m1, p1, m2, p2, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)(p1 * m1.get(it1, it2, it3, it4) +
			   p2 * m2.get(it1, it2, it3, it4)));
	}
}

void idxops_test2::idx_lincomb2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_lincomb(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4),
	       get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)5, (ubyte)12);
  test_lincomb(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
	       get_test_idx<byte>(i1, i2, i3, i4), (byte)(-4), (byte)52);
  test_lincomb(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4),
	       get_test_idx<uint16>(i1, i2, i3, i4), (uint16)423, (uint16)21);
  test_lincomb(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),
	       get_test_idx<int16>(i1, i2, i3, i4), (int16)3, (int16)124);
  test_lincomb(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4),
	       get_test_idx<uint32>(i1, i2, i3, i4), 54325u, 12356u);
  test_lincomb(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4),
	       get_test_idx<int32>(i1, i2, i3, i4), 543, 135);
  test_lincomb(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4),
	       get_test_idx<float32>(i1, i2, i3, i4), 1231.12f, -4.4f);
  test_lincomb(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4),
	       get_test_idx<float64>(i1, i2, i3, i4), -1241.1, -12.2);
  test_lincomb(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4),
	       get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)43, (ubyte)1);
  test_lincomb(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
	       get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)(-5), (byte)2);
  test_lincomb(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4),
	       get_test_idx_nc<uint16>(i1, i2, i3, i4), (uint16)12, (uint16)421);
  test_lincomb(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),
	       get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)423, (int16)543);
  test_lincomb(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4),
	       get_test_idx_nc<uint32>(i1, i2, i3, i4), 123453u, 3u);
  test_lincomb(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4),
	       get_test_idx_nc<int32>(i1, i2, i3, i4), 5432, 33);
  test_lincomb(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4),
	       get_test_idx_nc<float32>(i1, i2, i3, i4), 2.2f, 5.43454f);
  test_lincomb(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4),
	       get_test_idx_nc<float64>(i1, i2, i3, i4), 15432.4e5, 5432.4e42);
}

template <typename T> void test_tanh(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4);
  idx_tanh(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)tanh((double)m1.get(it1,it2,it3,it4)));
	}
}

void idxops_test2::idx_tanh2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_tanh(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_tanh(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}


template <typename T> void test_dtanh(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4);
  idx_dtanh(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)dtanh((double)m1.get(it1,it2,it3,it4)));
	}
}

void idxops_test2::idx_dtanh2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_dtanh(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_dtanh(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}


template <typename T> void test_stdsigmoid(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4);
  idx_stdsigmoid(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)stdsigmoid((double)m1.get(it1,it2,it3,it4)));
	}
}

void idxops_test2::idx_stdsigmoid2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_stdsigmoid(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}


template <typename T> void test_dstdsigmoid(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4);
  idx_dstdsigmoid(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (T)dstdsigmoid((double)m1.get(it1,it2,it3,it4)));
	}
}

void idxops_test2::idx_dstdsigmoid2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_dstdsigmoid(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}


template <typename T> void test_abs(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4);
  idx_abs(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  if (mt.get(it1, it2, it3, it4) !=
	      (T)std::abs((double)m1.get(it1, it2, it3, it4))) {
	    CPPUNIT_ASSERT(mt.get(it1, it2, it3, it4) ==
			   std::numeric_limits<T>::max());
	    CPPUNIT_ASSERT(m1.get(it1, it2, it3, it4) ==
			   std::numeric_limits<T>::min());
	  }
	}
}

void idxops_test2::idx_abs2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_abs(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_abs(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}

template <typename T> void test_thresdotc_acc(intg i1, intg i2, intg i3, intg i4,
					      idx<T> m1, idx<T> m2, T c, T th) {
  idx<T> mt(i1, i2, i3, i4);
  idx_copy(m2, mt);
  idx_thresdotc_acc(m1, c, th, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  if (m1.get(it1, it2, it3, it4) < -th)
	    ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
			 (T)(m2.get(it1, it2, it3, it4) - c));
	  else if (m1.get(it1, it2, it3, it4) > th)
	    ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
			 (T)(m2.get(it1, it2, it3, it4) + c));
	  else
	    ASSERT_EQUAL(mt.get(it1, it2, it3, it4), m2.get(it1, it2, it3, it4));
	}
}

void idxops_test2::idx_thresdotc_acc2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
		     get_test_idx<byte>(i1, i2, i3, i4), (byte)65, (byte)(-3));
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4),
		     get_test_idx<byte>(i1, i2, i3, i4), (byte)(-65), (byte)42);
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4),
		     get_test_idx<int16>(i1, i2, i3, i4), (int16)6531, (int16)543);
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4),
		     get_test_idx<int32>(i1, i2, i3, i4), 6554321, -1232);
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4),
		     get_test_idx<float32>(i1, i2, i3, i4), 65.332f, 12.2f);
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4),
		     get_test_idx<float64>(i1, i2, i3, i4), 65.12, 41231e4);

  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
		     get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)65, (byte)3);
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4),
		     get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)(-65), (byte)0);
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4),
		     get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)6531, (int16)(-43));
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4),
		     get_test_idx_nc<int32>(i1, i2, i3, i4), 6554321, 54);
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4),
		     get_test_idx_nc<float32>(i1, i2, i3, i4), 65.332f, 0.0f);
  test_thresdotc_acc(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4),
		     get_test_idx_nc<float64>(i1, i2, i3, i4), 65.12, 1e-21);
}

template <typename T> void test_threshold(intg i1, intg i2, intg i3, intg i4,
					  idx<T> m1, T p, T v, T v2) {
  idx<T> mt (i1, i2, i3, i4), mt2(i1, i2, i3, i4);
  idx<T> mt3(i1, i2, i3, i4), mt4(i1, i2, i3, i4);
  idx<T> mt5(i1, i2, i3, i4);
  idx_copy(m1, mt);
  idx_threshold(mt, p);
  idx_threshold(m1, p, mt2);
  idx_copy(m1, mt3);
  idx_threshold(mt3, p, v);
  idx_threshold(m1, p, v, mt4);
  idx_threshold(m1, p, v, v2, mt5);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt.get(it1, it2, it3, it4),
		       (m1.get(it1,it2,it3,it4) < p) ? p : m1.get(it1, it2, it3, it4));
	  ASSERT_EQUAL(mt2.get(it1, it2, it3, it4), mt.get(it1, it2, it3, it4));
	  ASSERT_EQUAL(mt3.get(it1, it2, it3, it4),
		       (m1.get(it1,it2,it3,it4) < p) ? v : m1.get(it1, it2, it3, it4));
	  ASSERT_EQUAL(mt4.get(it1, it2, it3, it4), mt3.get(it1, it2, it3, it4));
	  ASSERT_EQUAL(mt5.get(it1, it2, it3, it4),
		       (m1.get(it1,it2,it3,it4) < p) ? v : v2);
	}
}

void idxops_test2::idx_threshold2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_threshold(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)42,
		 (ubyte)12, (ubyte)54);
  test_threshold(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)(-120),
		 (byte)(-3), (byte)32);
  test_threshold(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4), (uint16)123,
		 (uint16)432, (uint16)43);
  test_threshold(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)40000,
		 (int16)43, (int16)(-4323));
  test_threshold(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 3000000000u,
		 1231u, 2u);
  test_threshold(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 13246543,
		 -1341, -54321234);
  test_threshold(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4), 12341.423f,
		 0.0f, 342.04f);
  test_threshold(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4), 1324231.54e42,
		 3.4, 12.4e3);
  test_threshold(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)42,
		 (ubyte)12, (ubyte)32);
  test_threshold(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)(-120),
		 (byte)(-3), (byte)32);
  test_threshold(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4), (uint16)123,
		 (uint16)432, (uint16)43);
  test_threshold(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)40000,
		 (int16)43, (int16)(-4323));
  test_threshold(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 3000000000u,
		 321u, 4321u);
  test_threshold(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 13246543,
		 41234, 4231);
  test_threshold(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4), 12341.423f,
		 21.43f, 112.4e3f);
  test_threshold(i1, i2, i3, i4, get_test_idx_nc<float64>(i1,i2, i3, i4), 132423.54e42,
		 32141.4, 12.0);
}

template <typename T> void test_sqrt(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4), mt2(i1, i2, i3, i4);
  idx_abs(m1, mt);
  idx_sqrt(mt, mt2);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
#ifdef __IPP__
	  ASSERT_EQUAL2(mt2.get(it1, it2, it3, it4),
			(T)sqrt((double)mt.get(it1,it2,it3,it4)),
			(T)ROUND(sqrt((double)mt.get(it1,it2,it3,it4))));
#else
	  ASSERT_EQUAL(mt2.get(it1, it2, it3, it4),
		       (T)ROUND(sqrt((double)mt.get(it1,it2,it3,it4))));
#endif
	}
}

void idxops_test2::idx_sqrt2() {
  //TODO: test is failing, fix it.
  // intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  // test_sqrt(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  // test_sqrt(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}

template <typename T> void test_exp(intg i1, intg i2, intg i3, intg i4,
				     idx<T> m1) {
  idx<T> mt(i1, i2, i3, i4), mt2(i1, i2, i3, i4);
  idx_copy(m1, mt);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4)
	  if (mt.get(it1, it2, it3, it4) > 50)
	    mt.set(1, it1, it2, it3, it4);
  idx_copy(mt, mt2);
  idx_exp(mt2);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
#ifdef __IPP__
	  ASSERT_EQUAL2(mt2.get(it1, it2, it3, it4),
			saturate(ROUND(exp((double)mt.get(it1,it2,it3,it4))),
				 T),
			saturate(exp((double)mt.get(it1,it2,it3,it4)), T));
#else
	  ASSERT_EQUAL(mt2.get(it1, it2, it3, it4),
		       saturate(exp((double)mt.get(it1,it2,it3,it4)), T));
#endif
	}
}

void idxops_test2::idx_exp2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_exp(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4));
  test_exp(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4));
}


template <typename T> void test_power(intg i1, intg i2, intg i3, intg i4,
				      idx<T> m1, T p) {
  idx<T> mt(i1, i2, i3, i4), mt2(i1, i2, i3, i4);
  idx_abs(m1, mt);
  idx_addc(mt, (T)1, mt);
  idx_power(mt, p, mt2);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt2.get(it1, it2, it3, it4),
		       saturate(pow((double)mt.get(it1,it2,it3,it4), (double)p), T));
	}
}

template <> void test_power(intg i1, intg i2, intg i3, intg i4,
			    idx<float> m1, float p) {
  idx<float> mt(i1, i2, i3, i4), mt2(i1, i2, i3, i4);
  idx_abs(m1, mt);
  idx_addc(mt, 1.0f, mt);
  idx_power(mt, p, mt2);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt2.get(it1, it2, it3, it4),
		       (float) pow(mt.get(it1,it2,it3,it4), p));
	}
}

template <> void test_power(intg i1, intg i2, intg i3, intg i4,
			    idx<double> m1, double p) {
  idx<double> mt(i1, i2, i3, i4), mt2(i1, i2, i3, i4);
  idx_abs(m1, mt);
  idx_addc(mt, 1.0f, mt);
  idx_power(mt, p, mt2);
  for (intg it1 = 0; it1 < i1; ++it1)
    for (intg it2 = 0; it2 < i2; ++it2)
      for (intg it3 = 0; it3 < i3; ++it3)
	for (intg it4 = 0; it4 < i4; ++it4) {
	  ASSERT_EQUAL(mt2.get(it1, it2, it3, it4),
		       pow(mt.get(it1,it2,it3,it4), p));
	}
}

void idxops_test2::idx_power2() {
  intg i1 = 42, i2 = 3, i3 = 5, i4 = 12;
  test_power(i1, i2, i3, i4, get_test_idx<ubyte>(i1, i2, i3, i4), (ubyte)4);
  test_power(i1, i2, i3, i4, get_test_idx<byte>(i1, i2, i3, i4), (byte)(-3));
  test_power(i1, i2, i3, i4, get_test_idx<uint16>(i1, i2, i3, i4), (uint16)3);
  test_power(i1, i2, i3, i4, get_test_idx<int16>(i1, i2, i3, i4), (int16)42);
  test_power(i1, i2, i3, i4, get_test_idx<uint32>(i1, i2, i3, i4), 12u);
  test_power(i1, i2, i3, i4, get_test_idx<int32>(i1, i2, i3, i4), 23);
  test_power(i1, i2, i3, i4, get_test_idx<float32>(i1, i2, i3, i4), 4.234f);
  test_power(i1, i2, i3, i4, get_test_idx<float64>(i1, i2, i3, i4), 12.5);
  test_power(i1, i2, i3, i4, get_test_idx_nc<ubyte>(i1, i2, i3, i4), (ubyte)3);
  test_power(i1, i2, i3, i4, get_test_idx_nc<byte>(i1, i2, i3, i4), (byte)6);
  test_power(i1, i2, i3, i4, get_test_idx_nc<uint16>(i1, i2, i3, i4), (uint16)32);
  test_power(i1, i2, i3, i4, get_test_idx_nc<int16>(i1, i2, i3, i4), (int16)2);
  test_power(i1, i2, i3, i4, get_test_idx_nc<uint32>(i1, i2, i3, i4), 44u);
  test_power(i1, i2, i3, i4, get_test_idx_nc<int32>(i1, i2, i3, i4), 9);
  // test_power(i1, i2, i3, i4, get_test_idx_nc<float32>(i1, i2, i3, i4), 9.5f);
  test_power(i1, i2, i3, i4, get_test_idx_nc<float64>(i1, i2, i3, i4), 3.23e-6);
}
