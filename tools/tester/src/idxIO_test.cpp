#include "idxIO_test.h"
#include "libidx.h"
#include "tools_utils.h"

#define TEST_FILE "./eblearn_tester_matrix_2.mat"

extern std::string *gl_data_dir;
extern std::string *gl_data_errmsg;

using namespace std;
using namespace ebl;

void idxIO_test::setUp() {
}

void idxIO_test::tearDown() {
}

template <typename T>
void check_loading_equal(idx<T> &ref, const std::string &fname) {
  try {
    idx<T> loaded = load_matrix<T>(fname);
    // test values
    { idx_aloop2(i, loaded, T, r, ref, T) {
	CPPUNIT_ASSERT_EQUAL(*r, *i);
      }
    }
  } catch(std::string &err) {
    std::cerr << err << std::endl;
    CPPUNIT_ASSERT(false); // error
  }
}

template<class T> void test_save_load_matrix() {
  std::string fname = TEST_FILE;
  // initialize matrix
  idx<T> m(9, 9);
  double v = 0.1;
  { idx_aloop1(i, m, T) {
      *i = (T) v;
      v++;
    }
  }
  // save and test
  rm_file(fname.c_str());
  save_matrix(m, fname);
  check_loading_equal(m, fname);
  // test matrix string saving/loading
  std::string data = save_matrix_to_string(m);
  rm_file(fname.c_str());
  string_to_file(data, fname);
  check_loading_equal(m, fname);
}

void idxIO_test::test_save_load_matrix_ubyte() {
  test_save_load_matrix<ubyte>();
}

void idxIO_test::test_save_load_matrix_int() {
  test_save_load_matrix<int>();
}

void idxIO_test::test_save_load_matrix_float() {
  test_save_load_matrix<float>();
}

void idxIO_test::test_save_load_matrix_double() {
  test_save_load_matrix<double>();
}

void idxIO_test::test_save_load_matrix_long() {
  idx<intg> test(3,3);
  test.set(2147483647, 0, 0);
  test.set(2147483646, 0, 1);
  test.set(2147483645, 0, 2);
  test.set(4, 1, 0);
  test.set(5, 1, 1);
  test.set(6, 1, 2);
  test.set(-2147483647, 2, 0);
  test.set(-2147483646, 2, 1);
  test.set(-2147483645, 2, 2);
  try {
    save_matrix(test, TEST_FILE);
    idx<intg> m = load_matrix<intg>(TEST_FILE);
    { idx_aloop2(i, test, intg, j, m, intg) {
	CPPUNIT_ASSERT_EQUAL((intg) *j, (intg) *i);
      }
    }
  } catch(std::string &err) {
    std::cerr << err << std::endl;
    CPPUNIT_ASSERT(false); // err
  }
}

void idxIO_test::test_save_load_matrix_matrix() {
  try {
    CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
    std::string root = *gl_data_dir;
    std::string sim1, sim2, sim3, sall;
    sim1 << root << "/barn.png";
    sim2 << root << "/lena.png";
    sim3 << root << "/2008_007714.jpg";
    sall << root << "/all.mat"; 
    idx<float> im1 = load_image<float>(sim1);
    idx<float> im2 = load_image<float>(sim2);
    idx<float> im3 = load_image<float>(sim3);

    midx<float> all(3);
    all.mset(im1, 0);
    all.mset(im2, 1);
    all.mset(im3, 2);

    save_matrices(all, sall);
    // midx<float> all2 = load_matrices<float>(sall, false);
    midx<float> all2 = load_matrices<float>(sall);
    idx<float> m1 = all.mget(0);
    idx<float> m2 = all.mget(1);
    idx<float> m3 = all.mget(2);
    float sum = idx_sum(im1) - idx_sum(m1) + idx_sum(im2) - idx_sum(m2)
      + idx_sum(im3) - idx_sum(m3);
    CPPUNIT_ASSERT_EQUAL(sum, (float) 0);
  } catch(std::string &err) {
    std::cerr << err << std::endl;
    CPPUNIT_ASSERT(false); // err
  }
}

void check_matrices(idx<float> &s1, idx<float> &s2, idx<float> &s3,
                    std::string &fname) {
  try {
    midx<float> all2 = load_matrices<float>(fname, true);
    // check 1st matrix is ok
    idx<float> m2 = all2.mget(0);
    CPPUNIT_ASSERT_EQUAL(idx_sum(s1) - idx_sum(m2), (float) 0);
    // check 2nd matrix is ok
    m2 = all2.mget(1);
    CPPUNIT_ASSERT_EQUAL(idx_sum(s2) - idx_sum(m2), (float) 0);
    // check 3rd matrix is ok
    m2 = all2.mget(2);
    CPPUNIT_ASSERT_EQUAL(idx_sum(s3) - idx_sum(m2), (float) 0);
    // check has_multiple_matrices
    CPPUNIT_ASSERT_EQUAL(has_multiple_matrices(fname.c_str()), true);
  } catch(std::string &err) {
    std::cerr << err << std::endl;
    CPPUNIT_ASSERT(false); // err
  }
}

void idxIO_test::test_save_load_matrices() {
  try {
    CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
    std::string root;
    root << *gl_data_dir << "/tables";
    std::string sim1, sim2, sim3, sall;
    sim1 << root <<"/table_2_6_connect_6_fanin_0_density_0.5_uv0_u3_v6.mat";
    sim2 << root <<"/table_32_96_connect_1920_fanin_20_density_0.62_random.mat";
    sim3 << root <<"/table_38_68_connect_2040_fanin_30_density_0.79_random.mat";
    sall << root << "/all.mat";
    CPPUNIT_ASSERT_EQUAL(has_multiple_matrices(sim1.c_str()), false);
    idx<float> s1 = load_matrix<float>(sim1);
    idx<float> s2 = load_matrix<float>(sim2);
    idx<float> s3 = load_matrix<float>(sim3);

    list<std::string> l;
    l.push_back(sim1);
    l.push_back(sim2);
    l.push_back(sim3);
    // regular saving
    rm_file(sall);
    save_matrices<float>(l, sall);
    check_matrices(s1, s2, s3, sall);
    // string saving
    midx<float> all = load_matrices<float>(sall, true);
    rm_file(sall);
    std::string data = save_matrices_to_string(all);
    string_to_file(data, sall);
    check_matrices(s1, s2, s3, sall);
  } catch(std::string &err) {
    std::cerr << err << std::endl;
    CPPUNIT_ASSERT(false); // err
  }
}
