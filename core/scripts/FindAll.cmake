# determine machine architecture
################################################################################
IF (APPLE) # MAC OS
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__MAC__ -pthread")
  SET (MAC true)
  SET (OS_NAME "Mac")
ELSE (APPLE)
  IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__LINUX__ -pthread")
    SET (LINUX true)
    SET (OS_NAME "Linux")
  ELSE ("${CMAKE_SYSTEM}" MATCHES "Linux")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__WINDOWS__")    
    # avoid security improvements warnings
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_DEPRECATE")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
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

# find CBLAS
################################################################################
MESSAGE(STATUS "CBLAS DISABLED")
# IF ($ENV{NOCBLAS})
#   MESSAGE(STATUS "CBLAS DISABLED by env variable $NOCBLAS=1.")
# ELSE ($ENV{NOCBLAS})
#   FIND_PACKAGE(CBLAS)
#   IF(CBLAS_FOUND)
#     SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__CBLAS__")
#     INCLUDE_DIRECTORIES(${CBLAS_INCLUDE_DIR})
#     MESSAGE(STATUS "Found CBLAS: ${CBLAS_INCLUDE_DIR}/${CBLAS_INCLUDE_FILE}")
#   ELSE (CBLAS_FOUND)
#     MESSAGE("__ WARNING: cblas not found, install to speed up (optional).")
#   ENDIF(CBLAS_FOUND)
# ENDIF ($ENV{NOCBLAS})

# find IPP
################################################################################
FIND_PACKAGE(IPP)
IF ($ENV{NOIPP})
  MESSAGE(STATUS "IPP DISABLED by env variable $NOIPP=1.")
ELSE ($ENV{NOIPP})
  IF (IPP_FOUND)
    include_directories(${IPP_INCLUDE_DIR})
    LINK_DIRECTORIES(${IPP_LIBRARIES_DIR})
    IF (64BIT) # 64 bit libraries
      SET(IPP_LIBRARIES
	ippcoreem64t guide ippiem64t ippcvem64t ippsem64t ippccem64t pthread)
      MESSAGE(STATUS "Found 64bit Intel IPP")
    ELSE (64BIT) # 32 bit libraries
      SET(IPP_LIBRARIES ippcore guide ippi ippcv ipps ippcc pthread)
      MESSAGE(STATUS "Found 32bit Intel IPP")
    ENDIF (64BIT)
    IF ($ENV{IPPFAST})
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPPFAST__")
    ELSE ($ENV{IPPFAST})
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPPACC__")
    IF ($ENV{IPPCHECKS})
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPP_CHECKS__")
    ENDIF ($ENV{IPPCHECKS})
    ENDIF ($ENV{IPPFAST})
    IF ($ENV{IPPCHECKS})
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPP_CHECKS__")
    ENDIF ($ENV{IPPCHECKS})
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPP__")  
  ELSE (IPP_FOUND)
    MESSAGE("__ WARNING: Intel IPP not found, install to speed up (optional).")
  ENDIF (IPP_FOUND)
ENDIF ($ENV{NOIPP})

# find ImageMagick
################################################################################
# we can't use convert under Windows, so don't try
IF ($ENV{NOIMAGEIMAGICK})
  MESSAGE(STATUS "ImageMagick DISABLED by env variable $NOIMAGEMAGICK=1.")
ELSE ($ENV{NOIMAGEIMAGICK})
  FIND_PACKAGE(ImageMagick)
  IF (ImageMagick_FOUND)
  #  SET(IMAGEMAGICK_CONVERT_EXECUTABLE "\"${IMAGEMAGICK_CONVERT_EXECUTABLE}\"")
    MESSAGE(STATUS "ImageMagick convert: ${IMAGEMAGICK_CONVERT_EXECUTABLE}")
    MESSAGE(STATUS "ImageMagick Found.")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IMAGEMAGICK__")  
  ELSE (ImageMagick_FOUND)
    MESSAGE("__ WARNING: ImageMagick not found.")
  ENDIF (ImageMagick_FOUND)
ENDIF ($ENV{NOIMAGEIMAGICK})

# find Magick++
################################################################################
IF ($ENV{NOMAGICKPP})
  MESSAGE(STATUS "Magick++ DISABLED by env variable $NOMAGICKPP=1.")
ELSE ($ENV{NOMAGICKPP})
  FIND_PACKAGE(Magick++)
  IF (Magick++_FOUND)
    include_directories(${Magick++_INCLUDE_DIR})
    LINK_DIRECTORIES(${Magick++_LIBRARIES_DIR})
    MESSAGE(STATUS "Magick++ includes: ${Magick++_INCLUDE_DIR}")
    MESSAGE(STATUS "Magick++ library: ${Magick++_LIBRARY}")
    MESSAGE(STATUS "Magick++ Found.")
    INCLUDE_DIRECTORIES(${Magick++_INCLUDE_DIR})
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__MAGICKPP__")  
  ELSE (Magick++_FOUND)
    # we don't care about this message under linux/mac if convert is found
    IF(WINDOWS OR NOT ImageMagick_FOUND) 
      MESSAGE("__ WARNING: Magick++ not found (optional).")
    ENDIF(WINDOWS OR NOT ImageMagick_FOUND) 
  ENDIF (Magick++_FOUND)
ENDIF ($ENV{NOMAGICKPP})

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
    LINK_DIRECTORIES(${MPI_LIBRARIES})
    MESSAGE(STATUS "MPI includes: ${MPI_INCLUDE_PATH}")
    MESSAGE(STATUS "MPI library: ${MPI_LIBRARIES}")
    MESSAGE(STATUS "MPI Found.")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__MPI__")  
  ELSE (MPI_FOUND)
    MESSAGE("__ WARNING: MPI not found (optional).")
  ENDIF (MPI_FOUND)
ENDIF ($ENV{NOMPI})
