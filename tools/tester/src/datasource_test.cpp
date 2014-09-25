#include "datasource_test.h"
#include <iostream>
#include <algorithm>
#include <string>

#ifdef __GUI__
#include "datasource_gui.h"
#endif

using namespace std;
using namespace ebl;

extern string *gl_mnist_dir;
extern string *gl_data_dir;
extern string *gl_mnist_errmsg;
extern string *gl_data_errmsg;

void datasource_test::setUp() {
}

void datasource_test::tearDown() {
}

// test function for mnist data source (requires special matrix header reading).
void datasource_test::test_mnist_LabeledDataSource() {
  CPPUNIT_ASSERT_MESSAGE("TODO: fixme", false);
  CPPUNIT_ASSERT_MESSAGE(*gl_mnist_errmsg, gl_mnist_dir != NULL);
  string datafile = *gl_mnist_dir;
  string labelfile = *gl_mnist_dir;
  datafile += "/t10k-images-idx3-ubyte";
  labelfile += "/t10k-labels-idx1-ubyte";
  try {
    idx<ubyte> data = load_matrix<ubyte>(datafile);
    idx<ubyte> labels = load_matrix<ubyte>(labelfile);
    labeled_datasource<double,ubyte,ubyte> ds(data, labels);
    state<double> datum(28, 28);
    state<ubyte> label;
    for (int i = 0; i < 5; i++) {
      ds.fprop_data(datum);
      ds.fprop_label(label);
      /* cout<<"Datum:"<<endl;
	 datum.x.printElems();
	 cout<<"Label: ";
	 label.x.printElems();
	 cout<<endl; */
      ds.next();
    }
  // briefly test some values of the 5th element of mnist
    CPPUNIT_ASSERT_EQUAL((unsigned int) 4, (unsigned int) label.get());
    CPPUNIT_ASSERT_EQUAL((unsigned int) 236, (unsigned int) datum.get(9, 9));
  } catch(string &err) {
    err = err;
    CPPUNIT_ASSERT(false); // error
  }
}
