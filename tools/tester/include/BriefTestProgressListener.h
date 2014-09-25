#ifndef MY_CPPUNIT_BRIEFTESTPROGRESSLISTENER_H
#define MY_CPPUNIT_BRIEFTESTPROGRESSLISTENER_H

#include <cppunit/BriefTestProgressListener.h>

using namespace CppUnit;

class MyBriefTestProgressListener : public BriefTestProgressListener
{
public:
  BriefTestProgressListener();
  virtual ~BriefTestProgressListener();

  void endTest( Test *test );
};

#endif  // MY_CPPUNIT_BRIEFTESTPROGRESSLISTENER_H
