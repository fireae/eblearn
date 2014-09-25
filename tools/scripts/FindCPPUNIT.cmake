# Find CppUnit Library
################################################################
FIND_PATH(CPPUNIT_INCLUDE_DIR cppunit/extensions/HelperMacros.h 
  /share/apps/cppunit/1.12.1/gnu/include
  /usr/include
  /usr/local/include
  /sw/include
  /sw/local/include
  /opt/local/include
  /home/sermanet/installed/cppunit/usr/local/include
  )

FIND_LIBRARY(CPPUNIT_LIBRARY 
  NAMES cppunit libcppunit-1.12
  PATHS 
  /usr/lib 
  /usr/lib64/ 
  /usr/local/lib
  /sw/lib
  /sw/local/lib
  /opt/local/lib
  /share/apps/cppunit/1.12.1/gnu/lib
  /home/sermanet/installed/cppunit/usr/local/lib
  )

IF (CPPUNIT_INCLUDE_DIR)
  MESSAGE(STATUS "CPPUnit include: ${CPPUNIT_INCLUDE_DIR}")
ENDIF (CPPUNIT_INCLUDE_DIR)
IF (CPPUNIT_LIBRARY)
  MESSAGE(STATUS "CPPUnit library: ${CPPUNIT_LIBRARY}")
ENDIF (CPPUNIT_LIBRARY)

IF (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY)
  SET(CPPUNIT_FOUND TRUE)
ENDIF (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY)

IF (CPPUNIT_FOUND)
  MESSAGE(STATUS "Found CPPUnit: ${CPPUNIT_LIBRARY}")
ELSE (CPPUNIT_FOUND)
  MESSAGE("__ WARNING: CPPUnit not found.")
ENDIF (CPPUNIT_FOUND)

MARK_AS_ADVANCED(CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARY)
