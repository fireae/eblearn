# determine machine architecture
################################################################################
IF (APPLE) # MAC OS
  SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__MAC__")
  SET (MAC true)
  SET (OS_NAME "Mac")
ELSE (APPLE)
  IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__LINUX__")
    SET (LINUX true)
    SET (OS_NAME "Linux")
  ELSE ("${CMAKE_SYSTEM}" MATCHES "Linux")
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__WINDOWS__")
    # avoid security improvements warnings
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D_CRT_SECURE_NO_WARNINGS")
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D_CRT_SECURE_NO_DEPRECATE")
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D_SCL_SECURE_NO_WARNINGS")
    SET (WINDOWS true)
    SET (OS_NAME "Windows")
  ENDIF("${CMAKE_SYSTEM}" MATCHES "Linux")
ENDIF (APPLE)

SET(BITSIZE "32")
SET(64BIT false) # default is 32 bits
IF (APPLE OR LINUX)
  EXEC_PROGRAM("uname -m" OUTPUT_VARIABLE ARCH_NAME)
  STRING(COMPARE EQUAL ${ARCH_NAME} "x86_64" 64BIT)
ELSE (APPLE OR LINUX)
  STRING(COMPARE EQUAL $ENV{PROCESSOR_ARCHITECTURE} "AMD64" 64BIT)
ENDIF (APPLE OR LINUX)
IF (64BIT)
  SET(BITSIZE "64")
ENDIF (64BIT)

MESSAGE(STATUS "Target OS is ${OS_NAME} (${BITSIZE} bits)")

MACRO(MAKE_WINDOWS_PATH pathname)
  # Enclose with UNESCAPED quotes.  This means we need to escape our
  # quotes once here, i.e. with \"
  SET(pathname "\"${pathname}\"")
ENDMACRO(MAKE_WINDOWS_PATH)

################################################################################
# custom dependencies with override
################################################################################
# Call custom dependencies, will override dependencies if overlap
################################################################################
FIND_PACKAGE(Custom)

# find boost
###############################################################################

MACRO(ADD_BOOST component)
  string(TOLOWER "${component}" name)
  IF (Boost_${component}_FOUND)
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__BOOST_${component}__")
    MESSAGE(STATUS "Boost ${name}: ${Boost_${component}_LIBRARY}")
  ELSE (Boost_${component}_FOUND)
    MESSAGE("__ WARNING: Boost ${name} not found (optional).")
  ENDIF (Boost_${component}_FOUND)
ENDMACRO(ADD_BOOST)

IF ($ENV{NOBOOST})
  MESSAGE(STATUS "BOOST DISABLED by env variable $NOBOOST=1.")
ELSE ($ENV{NOBOOST})
  IF (NOT Boost_FOUND)
    FIND_PACKAGE(Boost COMPONENTS filesystem regex system serialization mpi)
#    IF (Boost_FOUND)
#      IF (${Boost_MINOR_VERSION} GREATER 34)
#        FIND_PACKAGE(Boost COMPONENTS filesystem regex)
#        FIND_PACKAGE(Boost COMPONENTS system filesystem regex)
#      ENDIF(${Boost_MINOR_VERSION} GREATER 34)
#    ENDIF (Boost_FOUND)
  ENDIF (NOT Boost_FOUND)
  IF (Boost_SYSTEM_FOUND OR Boost_FILESYSTEM_FOUND OR Boost_REGEX_FOUND
	OR Boost_SERIALIZATION_FOUND OR Boost_MPI_FOUND)
    SET(Boost_FOUND TRUE)
  ENDIF (Boost_SYSTEM_FOUND OR Boost_FILESYSTEM_FOUND OR Boost_REGEX_FOUND
	OR Boost_SERIALIZATION_FOUND OR Boost_MPI_FOUND)

  IF (Boost_FOUND)
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__BOOST__")
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
    MESSAGE(STATUS "Found Boost")
    MESSAGE(STATUS "Boost include directory: ${Boost_INCLUDE_DIRS}")
    MESSAGE(STATUS "Boost libraries directory: ${Boost_LIBRARY_DIRS}")
    ADD_BOOST(SYSTEM)
    ADD_BOOST(FILESYSTEM)
    ADD_BOOST(REGEX)
    ADD_BOOST(SERIALIZATION)
    if ($ENV{NOMPI})
      set(Boost_MPI_FOUND FALSE)
      set(MPI_FOUND FALSE)
      message(STATUS "Boost MPI DISABLED by env variable $NOMPI=1.")
    else ($ENV{NOMPI})
      ADD_BOOST(MPI)
    endif ($ENV{NOMPI})
  ELSE(Boost_FOUND)
    MESSAGE("__ WARNING: Boost not found (optional).")
  ENDIF(Boost_FOUND)
ENDIF ($ENV{NOBOOST})

# find opencv
###############################################################################
IF ($ENV{USEOPENCV})
IF ($ENV{NOOPENCV})
  MESSAGE(STATUS "OPENCV DISABLED by env variable $NOOPENCV=1.")
ELSE ($ENV{NOOPENCV})
  FIND_PACKAGE(OpenCV)
  IF (OpenCV_FOUND)
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__OPENCV__")
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    LINK_DIRECTORIES(${OpenCV_LIBRARY_DIRS})
    MESSAGE(STATUS "Found OpenCV")
    MESSAGE(STATUS "OpenCV include directory: ${OpenCV_INCLUDE_DIRS}")
    MESSAGE(STATUS "OpenCV libraries: ${OpenCV_LIBRARIES}")
  ELSE(OpenCV_FOUND)
    MESSAGE("__ WARNING: OpenCV not found (optional).")
  ENDIF(OpenCV_FOUND)
ENDIF ($ENV{NOOPENCV})
ELSE ($ENV{USEOPENCV})
  MESSAGE(STATUS "OpenCV is disabled, export USEOPENCV=1 to activate it.")
ENDIF ($ENV{USEOPENCV})

# find XML++
##############################################################################
IF ($ENV{NOXML})
  MESSAGE(STATUS "XML DISABLED by env variable $NOXML=1.")
ELSE ($ENV{NOXML})
  FIND_PACKAGE(Xml++)
  IF (XML_FOUND)
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__XML__")
    INCLUDE_DIRECTORIES(${XML_INCLUDE_DIRS})
  ENDIF (XML_FOUND)
ENDIF ($ENV{NOXML})

# find KINECT
##############################################################################
IF ($ENV{NOKINECT})
  MESSAGE(STATUS "KINECT DISABLED by env variable $NOKINECT=1.")
ELSE ($ENV{NOKINECT})
  FIND_PACKAGE(Kinect)
  IF (KINECT_FOUND)
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__KINECT__")
    INCLUDE_DIRECTORIES(${KINECT_INCLUDE_DIRS})
  ENDIF (KINECT_FOUND)
ENDIF ($ENV{NOKINECT})

# find QT
##############################################################################

# generic paths to look in
set(paths "/usr/lib" "/usr/lib/x86_64-linux-gnu" "/usr/bin" "c:/Qt/bin" "c:/Qt/" "c:/Qt/lib")
IF ($ENV{NOQT})
  MESSAGE(STATUS "QT DISABLED by env variable $NOQT=1.")
ELSE ($ENV{NOQT})
  SET(DESIRED_QT_VERSION 4.3)
  IF (WINDOWS)
    FIND_PACKAGE(Qt4)
  ELSE(WINDOWS)
    IF (LINUX)
        FIND_PACKAGE(Qt4 QUIET PATHS ${paths})
    ELSE(LINUX)
       FIND_PACKAGE(Qt)
    ENDIF (LINUX)
  ENDIF (WINDOWS)
  IF (NOT QT4_FOUND)
      FIND_PACKAGE(Qt QUIET PATHS ${paths})
  ENDIF (NOT QT4_FOUND)

#  SET(DESIRED_QT_VERSION 4.3)
  IF (NOT QT4_FOUND)
    SET(QT_QMAKE_EXECUTABLE "/usr/local/pkg/qt/4.6.2/bin/qmake")
#    SET(QT_QMAKE_EXECUTABLE "/share/apps/qt/4.7.1/gnu/bin/qmake")

    IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
      SET(QT_QMAKE_EXECUTABLE "/usr/lib64/qt4/bin/qmake")
      IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
	SET(QT_QMAKE_EXECUTABLE "/usr/share/qt4/bin/qmake")
        IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
	  # still not found, use what's returned by 'which'
	  EXEC_PROGRAM("which" ARGS "qmake" OUTPUT_VARIABLE
	    QT_QMAKE_EXECUTABLE)
          IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
	    # give up
	    SET(QT_QMAKE_EXECUTABLE "")
	  ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
	ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
      ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
    ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})

IF (LINUX)
   FIND_PACKAGE(Qt4)
ELSE (LINUX)
   FIND_PACKAGE(Qt)
ENDIF (LINUX)
    #  INCLUDE(${QT_USE_FILE})
    SET(QT4_FOUND TRUE)
    exec_program(${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_HEADERS"
      OUTPUT_VARIABLE QT_INCLUDE_DIR)
   SET(QT_QTGUI_INCLUDE_DIR ${QT_INCLUDE_DIR})
                               SET(QT_QTGUI_LIBRARY "${QT_LIBRARY_DIR}/libQtGui.so")
  ENDIF (NOT QT4_FOUND)

  IF (QT4_FOUND)
    MESSAGE(STATUS "Qt qmake: ${QT_QMAKE_EXECUTABLE}")
    MESSAGE(STATUS "Qt moc: ${QT_MOC_EXECUTABLE}")
    MESSAGE(STATUS "Qt lib dir: ${QT_LIBRARY_DIR}")
    MESSAGE(STATUS "Qt include dir: ${QT_INCLUDE_DIR}")
    IF (WINDOWS)
      # to use DLL instead of static libraries, tell code we are using DLLs
      #SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -DQT_DLL")
      # add dll paths

    ENDIF (WINDOWS)
    IF (QT_MOC_EXECUTABLE
        AND QT_QMAKE_EXECUTABLE AND QT_LIBRARY_DIR AND QT_INCLUDE_DIR)
      IF (EXISTS "${QT_INCLUDE_DIR}")
        include_directories(${QT_INCLUDE_DIR})
      ENDIF (EXISTS "${QT_INCLUDE_DIR}")
      IF (EXISTS "${QT_INCLUDE_DIR}/Qt")
        include_directories(${QT_INCLUDE_DIR}/Qt)
      ENDIF (EXISTS "${QT_INCLUDE_DIR}/Qt")
      IF (EXISTS "${QT_INCLUDE_DIR}/QtGui")
        include_directories(${QT_INCLUDE_DIR}/QtGui)
      ENDIF (EXISTS "${QT_INCLUDE_DIR}/QtGui")
      IF (EXISTS "${QT_INCLUDE_DIR}/QtCore")
        include_directories(${QT_INCLUDE_DIR}/QtCore)
      ENDIF (EXISTS "${QT_INCLUDE_DIR}/QtCore")
      IF (EXISTS "${QT_INCLUDE_DIR}/Headers")
        include_directories(${QT_INCLUDE_DIR}/Headers/)
      ENDIF (EXISTS "${QT_INCLUDE_DIR}/Headers")
      include_directories(${QT_QTGUI_INCLUDE_DIR})
      include_directories(${QT_QTCORE_INCLUDE_DIR})
      SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__GUI__")
      MESSAGE(STATUS "Found Qt")
    ELSEIF (QT_MOC_EXECUTABLE
            AND QT_QMAKE_EXECUTABLE AND QT_LIBRARY_DIR AND QT_INCLUDE_DIR)
      MESSAGE("__ WARNING: Some Qt components are missing.")
    ENDIF (QT_MOC_EXECUTABLE
           AND QT_QMAKE_EXECUTABLE AND QT_LIBRARY_DIR AND QT_INCLUDE_DIR)
  ELSE (QT4_FOUND)
    MESSAGE("__ WARNING: QT not found (optional).")
  ENDIF (QT4_FOUND)
ENDIF ($ENV{NOQT})

# find QT3D
##############################################################################
IF ($ENV{QT3D})
IF ($ENV{NOQT3D})
  MESSAGE(STATUS "QT3D DISABLED by env variable $NOQT3D=1.")
ELSE ($ENV{NOQT3D})
  find_path(root NAMES lib/libQt3D.so PATHS
    /d/linux/installed/qt/qt-everywhere-opensource-src-4.7.1/
    /usr/local/Trolltech/Qt-4.7.1/)
  include_directories("${root}/include")
  include_directories("${root}/include/Qt")
  include_directories("${root}/include/QtGui")
  include_directories("${root}/include/QtCore")
  include_directories("${root}/include/Qt3D")
  link_directories("${root}/lib")
  SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__GUI3D__")
  message(STATUS "Qt3D includes: ${root}/include")
  message(STATUS "Qt3D libraries: ${root}/lib")
  message(STATUS "Found Qt3D")
  set(qt3d_found TRUE)
ENDIF ($ENV{NOQT3D})
ENDIF ($ENV{QT3D})

# find CBLAS
################################################################################
MESSAGE(STATUS "CBLAS DISABLED")
# IF ($ENV{NOCBLAS})
#   MESSAGE(STATUS "CBLAS DISABLED by env variable $NOCBLAS=1.")
# ELSE ($ENV{NOCBLAS})
#   FIND_PACKAGE(CBLAS)
#   IF(CBLAS_FOUND)
#     SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__CBLAS__")
#     INCLUDE_DIRECTORIES(${CBLAS_INCLUDE_DIR})
#     MESSAGE(STATUS "Found CBLAS: ${CBLAS_INCLUDE_DIR}/${CBLAS_INCLUDE_FILE}")
#   ELSE (CBLAS_FOUND)
#     MESSAGE("__ WARNING: cblas not found, install to speed up (optional).")
#   ENDIF(CBLAS_FOUND)
# ENDIF ($ENV{NOCBLAS})

# find CPPUnit
################################################################################
FIND_PACKAGE(CPPUNIT)
IF (CPPUNIT_FOUND)
  include_directories(${CPPUNIT_INCLUDE_DIR})
ENDIF (CPPUNIT_FOUND)

# find TH
################################################################################
IF ($ENV{USECUDA})
  IF (NOT CUDA_FOUND)
    FIND_PACKAGE(CUDA)
  ENDIF (NOT CUDA_FOUND)
  IF (CUDA_FOUND)
    include_directories(${CUDA_INCLUDE_DIRS})
    #link_directories(${CUDA_LIBRARIES})
    set(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__CUDA__")
    MESSAGE(STATUS "NVidia CUDA include paths: ${CUDA_INCLUDE_DIRS}")
    MESSAGE(STATUS "NVidia CUDA libraries: ${CUDA_LIBRARIES}")
    MESSAGE(STATUS "NVidia CUDA toolkit dir: ${CUDA_TOOLKIT_ROOT_DIR}")
    # MESSAGE(STATUS "NVidia CUDA SDK dir (optional): ${CUDA_SDK_ROOT_DIR}")
    MESSAGE(STATUS "NVidia CUDA version: ${CUDA_VERSION_STRING}")
    MESSAGE(STATUS "Nvidia CUDA Found.")
  ELSE (CUDA_FOUND)
    MESSAGE("__ WARNING: NVidia CUDA not found (optional).")
  ENDIF (CUDA_FOUND)
ENDIF ($ENV{USECUDA})


# find TH
################################################################################
IF ($ENV{NOTH})
  MESSAGE(STATUS "THC DISABLED by env variable $NOTH=1.")
ELSE ($ENV{NOTH})
  #IF (NOT THC_FOUND)
  #  FIND_PACKAGE(THC)
  #ENDIF (NOT THC_FOUND)
  IF (THC_FOUND)
    include_directories(${THC_INCLUDE_DIR})
    link_directories(${THC_LIBRARIES_DIR})
    FIND_PACKAGE(LAPACK)
    IF (LAPACK_FOUND)
      SET(THC_LIBRARIES  TH lapack)
    ELSE (LAPACK_FOUND)
       SET(THC_LIBRARIES  TH lapack)
    ENDIF (LAPACK_FOUND)
    set(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__TH__")
    MESSAGE(STATUS "THC include path: ${THC_INCLUDE_DIR}")
    MESSAGE(STATUS "THC library path: ${THC_LIBRARIES_DIR}")
    MESSAGE(STATUS "THC Found.")
    MESSAGE(STATUS "Disabling IPP because TH was found")
  ELSE (THC_FOUND)
    MESSAGE("__ WARNING: THC not found (optional).")
  ENDIF (THC_FOUND)
ENDIF ($ENV{NOTH})



# find IPP
################################################################################
FIND_PACKAGE(IPP)
IF ($ENV{NOIPP})
  MESSAGE(STATUS "IPP DISABLED by env variable $NOIPP=1.")
ELSEIF (THC_FOUND)
  MESSAGE(STATUS "IPP DISABLED because TH was found")
ELSE ($ENV{NOIPP})
  IF (IPP_FOUND)
    include_directories(${IPP_INCLUDE_DIR})
    LINK_DIRECTORIES(${IPP_LIBRARIES_DIR})
    IF (64BIT) # 64 bit libraries
      SET(IPP_LIBRARIES ippcoreem64t guide ippiem64t ippcvem64t ippmem64t
	ippsem64t ippccem64t pthread)
      MESSAGE(STATUS "Found 64bit Intel IPP")
    ELSE (64BIT) # 32 bit libraries
      SET(IPP_LIBRARIES ippcore guide ippi ippm ippcv ipps ippcc pthread)
      MESSAGE(STATUS "Found 32bit Intel IPP")
    ENDIF (64BIT)
    IF ($ENV{IPPFAST})
      SET(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__IPPFAST__")
    ELSE ($ENV{IPPFAST})
      SET(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__IPPACC__")
    IF ($ENV{IPPCHECKS})
      SET(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__IPP_CHECKS__")
    ENDIF ($ENV{IPPCHECKS})
    ENDIF ($ENV{IPPFAST})
    IF ($ENV{IPPCHECKS})
      SET(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__IPP_CHECKS__")
    ENDIF ($ENV{IPPCHECKS})
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__IPP__")
  ELSE (IPP_FOUND)
    MESSAGE("__ WARNING: Intel IPP not found, install to speed up (optional).")
  ENDIF (IPP_FOUND)
ENDIF ($ENV{NOIPP})

# find ImageMagick
################################################################################
# we can't use convert under Windows, so don't try
IF ($ENV{NOIMAGEMAGICK})
  MESSAGE(STATUS "ImageMagick DISABLED by env variable $NOIMAGEMAGICK=1.")
ELSE ($ENV{NOIMAGEMAGICK})
  FIND_PACKAGE(ImageMagick)
  IF (ImageMagick_FOUND)
  #  SET(IMAGEMAGICK_CONVERT_EXECUTABLE "\"${IMAGEMAGICK_CONVERT_EXECUTABLE}\"")
    MESSAGE(STATUS "ImageMagick convert: ${IMAGEMAGICK_CONVERT_EXECUTABLE}")
    MESSAGE(STATUS "ImageMagick Found.")
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__IMAGEMAGICK__")
  ELSE (ImageMagick_FOUND)
    MESSAGE("__ WARNING: ImageMagick not found.")
  ENDIF (ImageMagick_FOUND)
ENDIF ($ENV{NOIMAGEMAGICK})

# find Magick++
################################################################################
IF ($ENV{USEMAGICKPP})
IF ($ENV{NOMAGICKPP})
  MESSAGE(STATUS "Magick++ DISABLED by env variable $NOMAGICKPP=1.")
ELSE ($ENV{NOMAGICKPP})
  FIND_PACKAGE(Magick++)
  IF (Magick++_FOUND)
    include_directories(${Magick++_INCLUDE_DIR})
    LINK_DIRECTORIES(${Magick++_LIBRARIES_DIR})
    MESSAGE(STATUS "Magick++ includes: ${Magick++_INCLUDE_DIR}")
    MESSAGE(STATUS "Magick++ libraries: ${Magick++_LIBRARIES_DIR}")
    MESSAGE(STATUS "Magick++ library: ${Magick++_LIBRARY}")
    MESSAGE(STATUS "Magick++ Found.")
    INCLUDE_DIRECTORIES(${Magick++_INCLUDE_DIR})
    SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__MAGICKPP__")
  ELSE (Magick++_FOUND)
    # we don't care about this message under linux/mac if convert is found
    IF(WINDOWS OR NOT ImageMagick_FOUND)
      MESSAGE("__ WARNING: Magick++ not found (optional).")
    ENDIF(WINDOWS OR NOT ImageMagick_FOUND)
  ENDIF (Magick++_FOUND)
ENDIF ($ENV{NOMAGICKPP})
ELSE ($ENV{USEMAGICKPP})
  MESSAGE(STATUS "Magick++ is disabled, export USEMAGICKPP=1 to activate it.")
ENDIF ($ENV{USEMAGICKPP})

# find MPI
################################################################################
IF ($ENV{NOMPI})
  MESSAGE(STATUS "MPI DISABLED by env variable $NOMPI=1.")
ELSE ($ENV{NOMPI})
  IF (NOT MPI_FOUND)
    FIND_PACKAGE(MPI)
  ENDIF (NOT MPI_FOUND)
  IF (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    # SET (MPI_LIBRARIES "${MPI_LIBRARIES} libmpi.so.0")
    LINK_DIRECTORIES(${MPI_LIBRARIES})
    MESSAGE(STATUS "MPI includes: ${MPI_INCLUDE_PATH}")
    MESSAGE(STATUS "MPI library: ${MPI_LIBRARIES}")
    MESSAGE(STATUS "MPI Found.")
    if (NOT Boost_MPI_FOUND)
      message("__ WARNING: disabling MPI because Boost_MPI is not found.")
      set(MPI_FOUND FALSE)
    else (NOT Boost_MPI_FOUND)
      SET (CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__MPI__")
    endif (NOT Boost_MPI_FOUND)
  ELSE (MPI_FOUND)
    MESSAGE("__ WARNING: MPI not found (optional).")
  ENDIF (MPI_FOUND)
ENDIF ($ENV{NOMPI})

# find pthread
################################################################################
if (APPLE OR LINUX)
  set(PTHREAD_FOUND TRUE) # present by default on apple and linux
  set(PTHREAD_INCLUDE_DIR "/usr/include")
  if (APPLE)
    set(PTHREAD_LIBRARY_DIR "/usr/lib/")
    set(PTHREAD_LIBRARY "/usr/lib/libpthread.dylib")
  else (APPLE) # LINUX
    find_path(PTHREAD_LIBRARY "libpthread.so" paths
      "/usr/lib/"
      "/usr/lib64/"
      "/lib/x86_64-linux-gnu/"
      "/lib/i386-linux-gnu/"
      "/usr/lib/x86_64-linux-gnu/")
    set(PTHREAD_LIBRARY_DIR ${PTHREAD_LIBRARY})
    set(PTHREAD_LIBRARY "${PTHREAD_LIBRARY}/libpthread.so")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  endif (APPLE)
else (APPLE OR LINUX) # installed manually under windows

endif (APPLE OR LINUX)

if (PTHREAD_FOUND)
  message(STATUS "pthread Found.")
  message(STATUS "pthread include path: ${PTHREAD_INCLUDE_DIR}")
  message(STATUS "pthread library: ${PTHREAD_LIBRARY}")
  set(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__PTHREAD__")
  include_directories(${PTHREAD_INCLUDE_DIR})
  link_directories(${PTHREAD_LIBRARY_DIR})
endif (PTHREAD_FOUND)

# matlab (temporary) #############################################################
# TODO: temporary
#include_directories ("/opt/matlab/extern/include")
#link_directories("/opt/matlab/bin/glnxa64/")

# lua ############################################################################
if ($ENV{EBLUA})
  set(LUA_FOUND TRUE)
  set(LUA_INCLUDE_DIR "")
  set(LUA_LIBRARY_DIR "")
  set(LUA_LIBRARY "lua;luaT;TH;selflua")

  if (LUA_FOUND)
    message(STATUS "Lua interface Found.")
    message(STATUS "Lua include directory: ${LUA_INCLUDE_DIR}")
    message(STATUS "Lua libraries directory: ${LUA_LIBRARY_DIR}")
    message(STATUS "Lua libraries: ${LUA_LIBRARY}")
    set(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__LUA__")
    include_directories(${LUA_INCLUDE_DIR})
    link_directories(${LUA_LIBRARY_DIR})
  endif (LUA_FOUND)
endif ($ENV{EBLUA})

# find Google-Perftools
################################################################################
IF ($ENV{USE_PERFTOOLS})
  IF (NOT GPROF_FOUND)
    FIND_PACKAGE(GPROF_FOUND)
  ENDIF (NOT GPROF_FOUND)
  IF (GPROF_FOUND)
    include_directories(${GPROF_INCLUDE_DIR})
    link_directories(${GPROF_LIBRARIES_DIR})
    SET(GPROF_LIBRARIES  profiler)
    set(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__GPROF__")
    MESSAGE(STATUS "Google-perftools-profiler include path: ${GPROF_INCLUDE_DIR}")
    MESSAGE(STATUS "Google-perftools-profiler library path: ${GPROF_LIBRARIES_DIR}")
    MESSAGE(STATUS "Google-perftools-profiler Found.")
  ELSE (GPROF_FOUND)
    MESSAGE("__ WARNING: GPROF not found (optional).")
  ENDIF (GPROF_FOUND)
ENDIF ($ENV{USE_PERFTOOLS})


# # find Vlfeat
# ################################################################################
#   IF (NOT VLFEAT_FOUND)
#     FIND_PACKAGE(VLFEAT_FOUND)
#   ENDIF (NOT VLFEAT_FOUND)
#   IF (VLFEAT_FOUND)
#     include_directories(${VLFEAT_INCLUDE_DIR})
#     link_directories(${VLFEAT_LIBRARIES_DIR})
#     SET(VLFEAT_LIBRARIES  vl)
#     set(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__VLFEAT__")
#     MESSAGE(STATUS "VLFeat include path: ${VLFEAT_INCLUDE_DIR}")
#     MESSAGE(STATUS "VLFeat library path: ${VLFEAT_LIBRARIES_DIR}")
#     MESSAGE(STATUS "VLFeat Found.")
#   ELSE (VLFEAT_FOUND)
#     MESSAGE("__ WARNING: VLFEAT not found (optional).")
#   ENDIF (VLFEAT_FOUND)

IF (MATIO_FOUND)
ELSE (MATIO_FOUND)
     FIND_LIBRARY(MATIO_LIBRARIES matio)
     FIND_PATH(MATIO_INCLUDE_DIR matio.h)
ENDIF (MATIO_FOUND)

IF (MATIO_FOUND)
    FIND_LIBRARY(Z_LIBRARIES z)
    FIND_LIBRARY(HDF5_LIBRARIES hdf5)
    IF (Z_LIBRARIES STREQUAL "Z_LIBRARIES-NOTFOUND" )
    ELSE ()
      SET(MATIO_LIBRARIES ${MATIO_LIBRARIES} ${Z_LIBRARIES})
    ENDIF ()
    IF (HDF5_LIBRARIES STREQUAL "HDF5_LIBRARIES-NOTFOUND")
    ELSE ()
      SET(MATIO_LIBRARIES ${MATIO_LIBRARIES} ${HDF5_LIBRARIES})
    ENDIF ()
    include_directories(${MATIO_INCLUDE_DIR})
    set(CMAKE_CXX_DFLAGS "${CMAKE_CXX_DFLAGS} -D__MATLAB__")
    MESSAGE(STATUS "MATIO include path: ${MATIO_INCLUDE_DIR}")
    MESSAGE(STATUS "MATIO libraries: ${MATIO_LIBRARIES}")
    MESSAGE(STATUS "MATIO Found.")
ELSE (MATIO_FOUND)
    MESSAGE("__ WARNING: MATIO not found (optional).")
ENDIF (MATIO_FOUND)
