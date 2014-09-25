/*
 * testspidx.cpp
 *
 *      Author: cyril Poulet
 */

#include <fstream>
#include <iostream>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/BriefTestProgressListener.h>

#include "spIdxTest.h"
#include "spBlasTest.h"
#include "spIdxIOTest.h"

using namespace std;

int main(){
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(spIdxTest::suite());
	runner.addTest(spBlasTest::suite());
	runner.addTest(spIdxIOTest::suite());

	CppUnit::BriefTestProgressListener listener;
	runner.eventManager().addListener(&listener);
	// Run all tests
	runner.run();
	return 0;
}

