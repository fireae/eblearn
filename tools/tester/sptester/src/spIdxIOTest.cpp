/*
 * spIdxIOTest.cpp
 *
 *      Author: cyril Poulet
 */

#include "spIdxIOTest.h"

#include <iostream>
#include <fstream>


void spIdxIOTest::test_saveandload_spidx(){
	spIdx<double> test(0,3,3);
	test.set(1, 0, 0);
	test.set(4, 1, 0);
	test.set(6, 1, 2);
	test.set(7, 2, 0);
	test.set(8, 2, 1);

	save_matrix(test, "../eblearn/sptester/data/test.mat");
	spIdx<double> test2(0,3,3);
	load_matrix(test2, "../eblearn/sptester/data/test.mat");
	CPPUNIT_ASSERT_EQUAL(test.dim(0), test2.dim(0));
	CPPUNIT_ASSERT_EQUAL(test.dim(1), test2.dim(1));
	for(int i = 0; i < 3; i++)
		for(int j = 0; j<3; j++)
			CPPUNIT_ASSERT_EQUAL(test.get(i,j), test2.get(i,j));

}
