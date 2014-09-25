#include "MyTextOutputter.h"
#include <cppunit/TestResultCollector.h>

using namespace CppUnit;

extern bool color_print;

MyTextOutputter::MyTextOutputter( TestResultCollector *result,
                              OStream &stream )
  : TextOutputter(result, stream)
{
}

MyTextOutputter::~MyTextOutputter()
{
}

void 
MyTextOutputter::printHeader()
{
  if ( m_result->wasSuccessful() ) {
    if (color_print) m_stream << "\033[1;32m";
    m_stream << "[OK]";
    if (color_print) m_stream << "\033[0m";
    m_stream << " (";
    m_stream << m_result->runTests () << " tests)" ;
  } else {
    //    printFailureWarning();
    printStatistics();
  }
}

void 
MyTextOutputter::printStatistics()
{
  if (color_print) m_stream << "\033[1;32m";
  m_stream << "[OK:";
  if (color_print) m_stream << "\033[0m ";
  m_stream <<  m_result->runTests() 
    - m_result->testFailures() - m_result->testErrors();
  if (color_print) m_stream << "\033[1;32m";
  m_stream << "] ";
  if (color_print) m_stream << "\033[0m";
  if (color_print) m_stream << " \033[1;31m";
  m_stream << "[Failed: ";
  if (color_print) m_stream << "\033[0m";
  m_stream <<  m_result->testFailures();
  if (color_print) m_stream << "\033[1;31m";
  m_stream << "] ";
  if (color_print) m_stream << "\033[0m";
  if (color_print) m_stream << "\033[1;31m";
  m_stream << "[Errors: ";
  if (color_print) m_stream << "\033[0m";
  m_stream <<  m_result->testErrors();
  if (color_print) m_stream << "\033[1;31m";
  m_stream << "] ";
  if (color_print) m_stream << "\033[0m";
  m_stream  <<  "\n";
}

void 
MyTextOutputter::printFailureWarning()
{
  m_stream  << "\033[1;31m!!!FAILURES!!!\033[0m\n";
}
