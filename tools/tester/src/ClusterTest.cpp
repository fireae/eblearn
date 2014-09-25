#include "libidx.h"
#include "ClusterTest.h"

#include <iostream>

using namespace std;
using namespace ebl;

void ClusterTest::setUp() {
}

void ClusterTest::tearDown() {
}

// // compute distance between means and groundtruth
// template <typename T> double dist2groundtruth(idx<T> means, 
// 					      idx<T> groundtruth) {
//   idx_checkorder2(means, 2, groundtruth, 2);

//   idx<T> gt(groundtruth.dim(0), groundtruth.dim(1));
//   idx_copy(groundtruth, gt);
//   double total_dist = 0.0;
//   double dist;
//   int index;

//   { idx_bloop1(m, means, int) {
//       index = find_closest(m, gt, &dist);
//       if (index < gt.dim(0) - 1) {
// 	idx<int> shift(gt.dim(0) - index -1, gt.dim(1));
// 	idx<int> in(gt.narrow(0, gt.dim(0) - index - 1, index + 1));
// 	idx_copy(in, shift);
// 	idx<int> tgt = gt.narrow(0, gt.dim(0) - index - 1, index);
// 	idx_copy(shift, tgt);
//       }
//       if (gt.dim(0) > 1)
// 	gt = gt.narrow(0, gt.dim(0) - 1, 0);
//       total_dist += dist;
//     }}
//   return total_dist;
// }

void ClusterTest::test_ints() {
  CPPUNIT_ASSERT_MESSAGE("TODO: Implement automatic test", false);

//   const int K = 4;
//   // generate 16 data points
//   const int N = 2;
//   double dist, total_dist;
//   int test_data[] = { -4, -2, 2, 4 };
//   idx<int> vectors(16, N);
//   for (int i = 0; i < 4; ++i) {
//     for (int j = 0; j < 4; ++j) {
//       vectors.set(test_data[i], i * 4 + j, 0);
//       vectors.set(test_data[j], i * 4 + j, 1);
//     }
//   }
//   // manually set groundtruth centers for set {-4,-2,2,4}
//   idx<int> gt_centers(K, N);
//   double gtdist;
//   gt_centers.set(-3, 0, 0); gt_centers.set(-3, 0, 1);
//   gt_centers.set(-3, 1, 0); gt_centers.set( 3, 1, 1);
//   gt_centers.set( 3, 2, 0); gt_centers.set(-3, 2, 1);
//   gt_centers.set( 3, 3, 0); gt_centers.set( 3, 3, 1);

//   idx<int> a_means(K, N);
//   k_means(vectors, K, a_means);
//   gtdist = dist2groundtruth(a_means, gt_centers);
//   //  a_means.printElems();
//   //  cout << "kmeans gt distance: " << gtdist << endl;
//   CPPUNIT_ASSERT(gtdist < 90);

//   idx<int> b_means(K, N);
//   hierarchical_k_means(vectors, K, b_means);
//   gtdist = dist2groundtruth(b_means, gt_centers);
//   //  b_means.printElems();
//   //  cout << "hierar kmeans gt distance: " << gtdist << endl;
//   CPPUNIT_ASSERT(gtdist < 90);

//   idx<int> c_means(K, N);
//   k_means_plusplus(vectors, K, c_means);
//   gtdist = dist2groundtruth(c_means, gt_centers);
//   //  c_means.printElems();
//   //  cout << "kmeans++ gt distance: " << gtdist << endl;
//   CPPUNIT_ASSERT(gtdist < 90);
}
  
void ClusterTest::test_doubles() {
  CPPUNIT_ASSERT_MESSAGE("TODO: Implement automatic test", false);

//   const int K = 4;
//   // generate 100 data points with dimension 16
//   idx<double> vectors(100, 16);
//   for (int i = 0; i < 100; ++i) {
//     const intg k = i % 4;
//     for (int j = 0; j < 16; ++j) {
//       vectors.set(rand() % 16, i, j);
//     }
//     vectors.set(45000 + rand() % 14, i, k);
//   }
  
//   idx<double> a_means(K, 16);
//   k_means(vectors, K, a_means);
//   //  a_means.printElems();
//   //  cout << "kmeans gt distance: " << gtdist << endl;

//   idx<double> b_means(K, 16);
//   hierarchical_k_means(vectors, K, b_means);
//   //  b_means.printElems();
//   //  cout << "hierar kmeans gt distance: " << gtdist << endl;

//   idx<double> c_means(K, 16);
//   k_means_plusplus(vectors, K, c_means);
//   //  c_means.printElems();
//   //  cout << "kmeans++ gt distance: " << gtdist << endl;
  
//   // TODO: find out what the test is meant to be here
//   // and find a reasonable success measure.
}
