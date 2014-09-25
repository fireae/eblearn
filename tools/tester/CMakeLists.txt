################################################################################
#
# CMake configuration for eblearn_tester project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

# add include directories
################################################################################
include_directories (include)
include_directories(${LIBEBLEARN_INCLUDE_DIR})
include_directories(${LIBIDX_INCLUDE_DIR})
include_directories(${LIBEBLEARNTOOLS_INCLUDE_DIR})
include_directories(${TOOLS_INCLUDE_DIR})
IF (QT_FOUND)
  include_directories(${LIBIDXGUI_INCLUDE_DIR})
  include_directories(${LIBEBLEARNGUI_INCLUDE_DIR})
ENDIF (QT_FOUND)

IF (CPPUNIT_FOUND) # compile only if cppunit is present

  # compile executable
  ##############################################################################
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET (TESTER_BINARY_NAME tester_debug)
  else (CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET (TESTER_BINARY_NAME tester)
  endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

  add_executable (${TESTER_BINARY_NAME}
    src/idxIO_test.cpp
    src/idx_test.cpp
    src/idxiter_test.cpp
    src/idxops_test.cpp
    src/idxops_test2.cpp
    src/ippops_test.cpp
    src/thops_test.cpp
    src/ClusterTest.cpp
    src/datasource_test.cpp
    src/ebl_basic_test.cpp
    src/ebl_preprocessing_test.cpp
    src/image_test.cpp
    src/detector_test.cpp
    src/ebl_machines_test.cpp
    #    src/gui3d_test.cpp
    src/main.cpp
    src/MyBriefTestProgressListener.cpp
    src/MyTextOutputter.cpp
    )

  # link executable with external libraries
  ##############################################################################
  IF (NOT WINDOWS)
    target_link_libraries (${TESTER_BINARY_NAME} dl)
  ENDIF (NOT WINDOWS)
  target_link_libraries (${TESTER_BINARY_NAME} eblearn idx)
  target_link_libraries (${TESTER_BINARY_NAME} eblearntools)
  # IF (WINDOWS)
  # if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  # target_link_libraries (${TESTER_BINARY_NAME} ${CPPUNIT_LIBRARY_DEBUG})
  # else (CMAKE_BUILD_TYPE STREQUAL "Debug")
  # target_link_libraries (${TESTER_BINARY_NAME} ${CPPUNIT_LIBRARY})
  # endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
  # ELSE (WINDOWS)
  target_link_libraries (${TESTER_BINARY_NAME} ${CPPUNIT_LIBRARY})
  #ENDIF (WINDOWS)
  IF (QT_FOUND) #---------------------------------------------------------------
    target_link_libraries (${TESTER_BINARY_NAME} idxgui)
    target_link_libraries (${TESTER_BINARY_NAME} eblearngui)
  ENDIF (QT_FOUND) #------------------------------------------------------------
  LINK_MAGICKPP(${TESTER_BINARY_NAME})

ELSE (CPPUNIT_FOUND)
  MESSAGE("Error: CppUnit required to build tester")

ENDIF (CPPUNIT_FOUND) #---------------------------------------------------------
