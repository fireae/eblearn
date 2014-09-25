################################################################################
#
# CMake configuration for eblearn project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

PROJECT(eblearn)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)

IF(COMMAND CMAKE_POLICY)
  #  CMAKE_POLICY(VERSION 2.6)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/scripts
  ${CMAKE_BINARY_DIR}/cmake ${CMAKE_MODULE_PATH})

# Extra compilation flags
IF (WIN32)
   SET(CMAKE_CXX_FLAGS "$ENV{EBLFLAGS}"
   CACHE STRING "CMAKE CXX flags " FORCE )
ELSE (WIN32)
     SET(CMAKE_CXX_FLAGS "$ENV{EBLFLAGS}  -Werror=overloaded-virtual"
     CACHE STRING "CMAKE CXX flags " FORCE )
ENDIF (WIN32)

# use STL or not
################################################################################
IF ($ENV{NOSTL})
  MESSAGE(STATUS "STL DISABLED by env variable $NOSTL=1.")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NOSTL__")
ENDIF ($ENV{NOSTL})

# MPI
# IF ($ENV{USEMPI})
# IF (NOT $ENV{NOMPI})
# #   SET(CMAKE_CXX_COMPILER "mpicxx")
# #   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared-intel -O3 -fPIC -align -Zp8 -axP -unroll -ip -xP -vec-report -par-report -openmp-report")
# #   MESSAGE(STATUS "Compiling with MPI compiler ${CMAKE_CXX_COMPILER}")
# ENDIF (NOT $ENV{NOMPI})
# ENDIF ($ENV{USEMPI})

# check for required and optional external packages
FIND_PACKAGE(All)
# update flags from DFLAGS
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_DFLAGS}")

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

# under mac, disable parenthesis warning because of boot regexp
IF (MAC)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses-equality")
ENDIF (MAC)

# under windows, enable c++ exceptions
IF (WINDOWS)
   MESSAGE(STATUS "Enabling C++ exceptions with /EHsc.")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ENDIF (WINDOWS)

# output directories
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
MESSAGE(STATUS "Binaries output path: ${EXECUTABLE_OUTPUT_PATH}")
MESSAGE(STATUS "Libraries output path: ${LIBRARY_OUTPUT_PATH}")

SET(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")
MESSAGE(STATUS "Data path for testing is: ${DATA_PATH}")

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
if (MAC)
SET(CMAKE_CXX_FLAGS_DEBUG
  "${EFENCE_FLAG} -D__DEBUG__ -g -Wall -gdwarf-2 -Wunused-variable -Wno-long-long -Wno-unknown-pragmas -Wno-system-headers -pedantic -O0 -fexceptions -Werror=overloaded-virtual"
  CACHE STRING "Debug builds CMAKE CXX flags " FORCE )
else (MAC)
SET(CMAKE_CXX_FLAGS_DEBUG
  "${EFENCE_FLAG} -D__DEBUG__ -g -Wall -ggdb -gdwarf-2 -Wunused-variable -Wno-long-long -Wno-unknown-pragmas -Wno-system-headers -pedantic -O0 -fsignaling-nans -fexceptions -Werror=overloaded-virtual"
  CACHE STRING "Debug builds CMAKE CXX flags " FORCE )
endif (MAC)

# display debug flags
MESSAGE(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")

endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# display regular flags
MESSAGE(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
MESSAGE(STATUS "Compilation flags: ${CMAKE_CXX_FLAGS}")

# copy gdb config file into bin directory
EXEC_PROGRAM("cp" ARGS "\"${CMAKE_SOURCE_DIR}/.gdbinit\""
		  "\"${LIBRARY_OUTPUT_PATH}\"" OUTPUT_VARIABLE OUT)

# set include directories
SET(LIBIDX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../core/libidx/include)
SET(LIBEBLEARN_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/../core/libeblearn/include)
SET(LIBEBLEARNTOOLS_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/libeblearntools/include)
SET(LIBIDXGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libidxgui/include)
SET(TOOLS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tools/include)

# link macros
################################################################################
MACRO(LINK_BOOST project component)
STRING(TOUPPER "${component}" COMP)
IF (Boost_${COMP}_FOUND)
    TARGET_LINK_LIBRARIES(${project} ${Boost_${COMP}_LIBRARY})
ENDIF (Boost_${COMP}_FOUND)
ENDMACRO(LINK_BOOST)

MACRO(LINK_QT project component)
IF (QT_FOUND)
    TARGET_LINK_LIBRARIES(${project} ${component})
ENDIF (QT_FOUND)
ENDMACRO(LINK_QT)

MACRO(LINK_OPENCV project component)
IF (OpenCV_FOUND)
    TARGET_LINK_LIBRARIES(${project} ${component})
ENDIF (OpenCV_FOUND)
ENDMACRO(LINK_OPENCV)

MACRO(LINK_XML project)
IF (XML_FOUND)
    TARGET_LINK_LIBRARIES(${project} ${XML_LIBRARY_FLAGS})
ENDIF (XML_FOUND)
ENDMACRO(LINK_XML)

MACRO(LINK_KINECT project)
IF (KINECT_FOUND)
    TARGET_LINK_LIBRARIES(${project} ${KINECT_LIBRARY_FLAGS})
ENDIF (KINECT_FOUND)
ENDMACRO(LINK_KINECT)

MACRO(LINK_PTHREAD project)
IF (PTHREAD_FOUND)
    TARGET_LINK_LIBRARIES(${project} ${PTHREAD_LIBRARY})
ENDIF (PTHREAD_FOUND)
ENDMACRO(LINK_PTHREAD)

MACRO(LINK_MAGICKPP project)
IF (Magick++_FOUND)
    TARGET_LINK_LIBRARIES(${project} Magick++)
ENDIF (Magick++_FOUND)
ENDMACRO(LINK_MAGICKPP)

MACRO(LINK_LUA project)
IF (LUA_FOUND)
    TARGET_LINK_LIBRARIES(${project} ${LUA_LIBRARY})
ENDIF (LUA_FOUND)
ENDMACRO(LINK_LUA)

MACRO(LINK_MPI project)
IF (MPI_FOUND)
    TARGET_LINK_LIBRARIES(${project} ${MPI_LIBRARIES})  # mpi_cxx mpi) #pmpich++
ENDIF (MPI_FOUND)
ENDMACRO(LINK_MPI)

# subdirectories
################################################################################
ADD_SUBDIRECTORY(../core/libidx
  ${CMAKE_CURRENT_SOURCE_DIR}/../core/build/${CMAKE_BUILD_TYPE}/libidx)
ADD_SUBDIRECTORY(../core/libeblearn
  ${CMAKE_CURRENT_SOURCE_DIR}/../core/build/${CMAKE_BUILD_TYPE}/libeblearn)
ADD_SUBDIRECTORY(libidxgui)
ADD_SUBDIRECTORY(libeblearngui)
ADD_SUBDIRECTORY(libeblearntools)
ADD_SUBDIRECTORY(tools)
ADD_SUBDIRECTORY(tester)
# ADD_SUBDIRECTORY(demos)

###ADD_SUBDIRECTORY(libspidx)
####ADD_SUBDIRECTORY(sptester)
