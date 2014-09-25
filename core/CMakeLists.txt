################################################################################
#
# CMake configuration for eblearn project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

PROJECT(eblearn_core)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)

IF(COMMAND CMAKE_POLICY)
  #  CMAKE_POLICY(VERSION 2.6)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# output directories
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/scripts
  ${CMAKE_BINARY_DIR}/cmake ${CMAKE_MODULE_PATH})
SET(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../tools/data")
MESSAGE(STATUS "Data path for testing is: ${DATA_PATH}")

# Extra compilation flags
SET(CMAKE_CXX_FLAGS "$ENV{EBLFLAGS}"
  CACHE STRING "CMAKE CXX flags " FORCE )

# use STL or not
################################################################################
IF ($ENV{NOSTL})
  MESSAGE(STATUS "STL DISABLED by env variable $NOSTL=1.")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NOSTL__")
ENDIF ($ENV{NOSTL})

# MPI
IF ($ENV{USEMPI})
   MESSAGE(STATUS "Compiling with MPI compiler")
   SET(CMAKE_CXX_COMPILER "mpicxx")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared-intel -O3 -fPIC -align -Zp8 -axP -unroll -ip -xP -vec-report -par-report -openmp-report -Werror=overloaded-virtual")
ENDIF ($ENV{USEMPI})

# check for required and optional external packages
FIND_PACKAGE(All)

# check OpenMP flag
IF ($ENV{USEOPENMP})
   MESSAGE(STATUS "Using OpenMP.")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__OPENMP__ --openmp")
ENDIF ($ENV{USEOPENMP})

# check SSE flag
IF ($ENV{USESSE})
   MESSAGE(STATUS "Using SSE.")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__USE_SSE__ -msse")
ENDIF ($ENV{USESSE})

# under windows, enable c++ exceptions
IF (WINDOWS)
   MESSAGE(STATUS "Enabling C++ exceptions with /EHsc.")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ENDIF (WINDOWS)

# IF (NOT WINDOWS)
#   # Allow default templates in templated functions
#   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x"
#     CACHE STRING "CMAKE CXX flags " FORCE )
# ENDIF (NOT WINDOWS)

# Check that electric-fence and valgrind are installed for (debug mode only)
SET(EFENCE_FLAG "")
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
  FIND_PACKAGE(EFence)
  IF (NOT EFENCE_FOUND)
    MESSAGE("*** Please install electric-fence to catch memory-related bugs in debug mode ***\n")
  ELSEIF (EFENCE_FOUND)
    SET(EFENCE_FLAG "-lefence")
  ENDIF (NOT EFENCE_FOUND)

  FIND_PACKAGE(Valgrind)
  IF (NOT VALGRIND_FOUND)
    MESSAGE("*** Please install valgrind to catch memory-related bugs in debug mode ***")
    MESSAGE("*** Call 'valgrind ./myprogram' at least once before committing ***\n")
  ENDIF (NOT VALGRIND_FOUND)

# Debug compilation flags
SET(EFENCE_FLAG "")
SET(CMAKE_CXX_FLAGS_DEBUG
  "${EFENCE_FLAG} -D__DEBUG__ -g -Wall -ggdb -gdwarf-2 -Wunused-variable -Wno-long-long -Wno-unknown-pragmas -Wno-system-headers -pedantic -O0 -fsignaling-nans -fexceptions"
  CACHE STRING "Debug builds CMAKE CXX flags " FORCE )

# display debug flags
MESSAGE(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")

endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# display regular flags
MESSAGE(STATUS "Compilation flags: ${CMAKE_CXX_FLAGS}")

# set include directories
SET(LIBIDX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libidx/include)
SET(LIBEBLEARN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libeblearn/include)

# compile subdirectories
ADD_SUBDIRECTORY(libidx)
ADD_SUBDIRECTORY(libeblearn)
