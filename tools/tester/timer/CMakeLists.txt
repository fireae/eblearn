################################################################################
#
# CMake configuration for eblearn_timer project
#
# Author(s):
#   Michael Mathieu, michael.mathieu@ens.fr
#
################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{EBL_ROOT}/scripts)

FIND_PACKAGE(All)

LINK_DIRECTORIES($ENV{TMP_DIR}/lib)

# add include directories
################################################################################
include_directories(include)

# check OpenMP flag
IF ($ENV{USEOPENMP})
   MESSAGE(STATUS "Using OpenMP.")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__OPENMP__ --openmp")
ENDIF ($ENV{USEOPENMP})
   

IF ($ENV{TIMEFUN})
  add_executable (timefun timefun.cpp)
  target_link_libraries (timefun idx)
  target_link_libraries (timefun pthread)
ENDIF ($ENV{TIMEFUN})

IF ($ENV{TIMECONV})
  add_executable (timeconv timeconv.cpp)
  target_link_libraries (timeconv idx)
  target_link_libraries (timeconv eblearn)
  target_link_libraries (timeconv pthread)
ENDIF ($ENV{TIMECONV})
