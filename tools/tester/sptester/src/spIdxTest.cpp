/*
 * spIdxTest.cpp
 *
 *      Author: cyril Poulet
 */

#include "spIdxTest.h"

void spIdxTest::test_set_get(){
	intg dims[2] = {4, 4};
	spIdx<intg> sptest(0, 2, dims);

	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++){
			CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(i,j));
		};

	sptest.set(36, 3, 2);
	CPPUNIT_ASSERT_EQUAL((intg)1, sptest.nelements());
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++){
			if((i==3)&& (j==2)) CPPUNIT_ASSERT_EQUAL((intg)36, sptest.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(i,j));
		};

	sptest.set(36, 2, 1);
	CPPUNIT_ASSERT_EQUAL((intg)2, sptest.nelements());
	for(int i = 0; i<4; i++){
		for(int j = 0; j<4; j++){
			if((i==3)&&(j==2)) CPPUNIT_ASSERT_EQUAL( (intg)36, sptest.get(i,j));
			else if((i==2)&&(j==1)) CPPUNIT_ASSERT_EQUAL( (intg)36, sptest.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(i,j));
		};
	};

	sptest.set(27, 3, 2);
	CPPUNIT_ASSERT_EQUAL((intg)2, sptest.nelements());
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++){
			if((i==3)&& (j==2)) CPPUNIT_ASSERT_EQUAL((intg)27, sptest.get(i,j));
			else if((i==2)&& (j==1)) CPPUNIT_ASSERT_EQUAL((intg)36, sptest.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(i,j));
		};

	sptest.set(0, 3, 2);
	CPPUNIT_ASSERT_EQUAL((intg)1, sptest.nelements());
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++){
			if((i==2)&& (j==1)) CPPUNIT_ASSERT_EQUAL((intg)36, sptest.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(i,j));
		};


	sptest.set(0, 2, 1);
	CPPUNIT_ASSERT_EQUAL( (intg)0, sptest.nelements());
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++){
			CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(i,j));
		};

	sptest.set(0, 2, 1);
	CPPUNIT_ASSERT_EQUAL((intg)0, sptest.nelements());
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++){
			CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(i,j));
		};
}

void spIdxTest::test_const(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(36, 3, 2);
	sptest.set(24, 1, 2);

	spIdx<intg> sptest2(sptest);
	CPPUNIT_ASSERT_EQUAL((intg)36, sptest2.get(3,2));
	sptest2.set(3, 2, 2);
	CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(2,2));

}

void spIdxTest::test_narrow(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 1, 1);
	sptest.set(2, 2, 2);
	sptest.set(3, 3, 3);
	sptest.set(4, 0, 0);

	spIdx<intg> sptest2 = sptest.narrow(0, 1, 1);
	CPPUNIT_ASSERT_EQUAL((intg)1, sptest2.nelements());
	for(int i = 0; i<4; i++){
		if(i==1) CPPUNIT_ASSERT_EQUAL((intg)1, sptest2.get(0,i));
		else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest2.get(0,i));
	};

	sptest2 = sptest.narrow(0, 1, 2);
	CPPUNIT_ASSERT_EQUAL((intg)1, sptest2.nelements());
	for(int i = 0; i<4; i++){
		if(i==2) CPPUNIT_ASSERT_EQUAL((intg)2, sptest2.get(0,i));
		else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest2.get(0,i));
	};

	sptest2 = sptest.narrow(0, 3, 1);
	CPPUNIT_ASSERT_EQUAL((intg)3, sptest2.nelements());
	for(int i = 0; i<3; i++){
		for(int j = 0; j<4; j++){
			if((i==0)&&(j==1)) CPPUNIT_ASSERT_EQUAL((intg)1, sptest2.get(i,j));
			else if((i==1)&&(j==2)) CPPUNIT_ASSERT_EQUAL((intg)2, sptest2.get(i,j));
			else if((i==2)&&(j==3)) CPPUNIT_ASSERT_EQUAL((intg)3, sptest2.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest2.get(i,j));
		};
	};

	sptest2 = sptest.narrow(1, 2, 1);
	CPPUNIT_ASSERT_EQUAL((intg)2, sptest2.nelements());
	for(int i = 0; i<3; i++){
		for(int j = 0; j<2; j++){
			if((i==1)&&(j==0)) CPPUNIT_ASSERT_EQUAL((intg)1, sptest2.get(i,j));
			else if((i==2)&&(j==1)) CPPUNIT_ASSERT_EQUAL((intg)2, sptest2.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest2.get(i,j));
		};
	};
}

void spIdxTest::test_select(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 1, 1);
	sptest.set(2, 2, 2);
	sptest.set(3, 3, 3);
	sptest.set(4, 0, 0);
	sptest.set(10, 3, 2);

	spIdx<intg> sptest2 = sptest.select(0, 1);
	CPPUNIT_ASSERT_EQUAL((intg)1, sptest2.nelements());
	for(int i = 0; i<4; i++){
		if(i==1) CPPUNIT_ASSERT_EQUAL((intg)1, sptest2.get(i));
		else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest2.get(i));
	};

	sptest2 = sptest.select(0, 3);
	CPPUNIT_ASSERT_EQUAL((intg)2, sptest2.nelements());
	for(int i = 0; i<4; i++){
		if(i==2) CPPUNIT_ASSERT_EQUAL((intg)10, sptest2.get(i));
		else if(i==3) CPPUNIT_ASSERT_EQUAL((intg)3, sptest2.get(i));
		else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest2.get(i));
	};

	sptest2 = sptest.select(1, 3);
	CPPUNIT_ASSERT_EQUAL((intg)1, sptest2.nelements());
	for(int i = 0; i<4; i++){
		if(i==3) CPPUNIT_ASSERT_EQUAL((intg)3, sptest2.get(i));
		else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest2.get(i));
	};

	sptest2 = sptest.select(1, 2);
	CPPUNIT_ASSERT_EQUAL((intg)2, sptest2.nelements());
	for(int i = 0; i<4; i++){
		if(i==2) CPPUNIT_ASSERT_EQUAL((intg)2, sptest2.get(i));
		else if(i==3) CPPUNIT_ASSERT_EQUAL((intg)10, sptest2.get(i));
		else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest2.get(i));
	};

}

void spIdxTest::test_transpose(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<intg> sptest2 = sptest.transpose(0, 1);
	CPPUNIT_ASSERT_EQUAL((intg)4, sptest2.nelements());

	for(int i = 0; i<4; i++){
		for(int j = 0; j<4; j++){
			if((i==1)&&(j==0)) CPPUNIT_ASSERT_EQUAL((intg)1, sptest2.get(i,j));
			else if((i==2)&&(j==1)) CPPUNIT_ASSERT_EQUAL((intg)2, sptest2.get(i,j));
			else if((i==3)&&(j==2)) CPPUNIT_ASSERT_EQUAL((intg)3, sptest2.get(i,j));
			else if((i==0)&&(j==3)) CPPUNIT_ASSERT_EQUAL((intg)4, sptest2.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest2.get(i, j));
		};
	};

	int p[2] = {1, 0};
	spIdx<intg> sptest4 = sptest.transpose(p);
	for(int i = 0; i<4; i++){
		for(int j = 0; j<4; j++){
			if((i==1)&&(j==0)) CPPUNIT_ASSERT_EQUAL((intg)1, sptest4.get(i,j));
			else if((i==2)&&(j==1)) CPPUNIT_ASSERT_EQUAL((intg)2, sptest4.get(i,j));
			else if((i==3)&&(j==2)) CPPUNIT_ASSERT_EQUAL((intg)3, sptest4.get(i,j));
			else if((i==0)&&(j==3)) CPPUNIT_ASSERT_EQUAL((intg)4, sptest4.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest4.get(i, j));
		};
	};
}

void spIdxTest::test_resize(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	sptest.resize(2, 3);
	CPPUNIT_ASSERT_EQUAL((intg)2, sptest.nelements());
	for(int i = 0; i<2; i++){
		for(int j = 0; j<3; j++){
			if((i==0)&&(j==1)) CPPUNIT_ASSERT_EQUAL((intg)1, sptest.get(i,j));
			else if((i==1)&&(j==2)) CPPUNIT_ASSERT_EQUAL((intg)2, sptest.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(i, j));
		};
	};

	sptest.resize( 4, 4);
	CPPUNIT_ASSERT_EQUAL((intg)2, sptest.nelements());
	for(int i = 0; i<2; i++){
		for(int j = 0; j<3; j++){
			if((i==0)&&(j==1)) CPPUNIT_ASSERT_EQUAL((intg)1, sptest.get(i,j));
			else if((i==1)&&(j==2)) CPPUNIT_ASSERT_EQUAL((intg)2, sptest.get(i,j));
			else CPPUNIT_ASSERT_EQUAL((intg)BACKGROUND, sptest.get(i, j));
		};
	};
}

void spIdxTest::test_sort(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);
	sptest.set(2, 1, 2);
	sptest.sort();
	for(int i = 0; i<4; i++){
		CPPUNIT_ASSERT_EQUAL((intg)(i+1), sptest.values()->get(i));
	};
}
