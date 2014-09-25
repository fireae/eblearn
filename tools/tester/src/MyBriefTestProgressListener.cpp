#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <cppunit/portability/Stream.h>

extern bool color_print;

CPPUNIT_NS_BEGIN

BriefTestProgressListener::BriefTestProgressListener()
    : m_lastTestFailed( false )
{
}


BriefTestProgressListener::~BriefTestProgressListener()
{
}


void 
BriefTestProgressListener::startTest( Test *test )
{
  stdCOut() << test->getName();
  stdCOut().flush();

  m_lastTestFailed = false;
}


void 
BriefTestProgressListener::addFailure( const TestFailure &failure )
{
  stdCOut() << " : " << (failure.isError() ? "error" : "assertion");
  m_lastTestFailed  = true;
}


void 
BriefTestProgressListener::endTest( Test *test )
{
  if ( !m_lastTestFailed ) {
    stdCOut() << " : ";
    if (color_print) stdCOut() << "\033[1;32m";
    stdCOut() << "[OK]";
    if (color_print) stdCOut() << "\033[0m";
  }
  else { 
    if (color_print) stdCOut() << "\033[1;31m";
    stdCOut()  <<  " [Failed]";
    if (color_print) stdCOut() << "\033[0m";
  }
  stdCOut() << "\n";
}


CPPUNIT_NS_END

