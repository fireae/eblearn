################################################################################
#
# CMake configuration for spidxtester project
#
# Author(s):
#   Cyril Poulet, cyril.poulet@centraliens.net, New York University
#
################################################################################

# add include directories
################################################################################
include_directories (include)
include_directories(${LIBIDX_INCLUDE_DIR})
include_directories(${LIBSPIDX_INCLUDE_DIR})

# check for external libraries
################################################################################
# CPPUnit
################################################################################
FIND_PACKAGE(CPPUNIT)

IF (CPPUNIT_FOUND) #------------------------------------------------------------
  include_directories(${CPPUNIT_INCLUDE_DIR})
  
  # compile executable
  ################################################################################
  add_executable (spidxtester
    src/spBlasTest.cpp
    src/spIdxIOTest.cpp
    src/spIdxTest.cpp
    src/testspidx.cpp
    )
  
  # link executable with external libraries
  ################################################################################
  target_link_libraries (spidxtester idx spidx)
  target_link_libraries (spidxtester ${CPPUNIT_LIBRARY})
  
  # write parameters for execution in run.init
  ################################################################################
  FILE(APPEND "${EXECUTABLE_OUTPUT_PATH}/run.init"
    "-data ${CMAKE_CURRENT_SOURCE_DIR}/../data\n")
  
ENDIF (CPPUNIT_FOUND) #---------------------------------------------------------
