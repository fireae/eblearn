#ifndef MY_CPPUNIT_TEXTOUTPUTTER_H
#define MY_CPPUNIT_TEXTOUTPUTTER_H

#include <cppunit/TextOutputter.h>

using namespace CppUnit;

class MyTextOutputter : public TextOutputter
{
public:
  MyTextOutputter( TestResultCollector *result, OStream &stream );
  virtual ~MyTextOutputter();

  virtual void printHeader();
  virtual void printFailureWarning();
  virtual void printStatistics();
};

#endif  // MY_CPPUNIT_TEXTOUTPUTTER_H
