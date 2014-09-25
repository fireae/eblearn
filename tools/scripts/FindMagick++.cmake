# - Try to find ImageMagick++
# Once done, this will define
#
#  Magick++_FOUND - system has Magick++
#  Magick++_INCLUDE_DIRS - the Magick++ include directories
#  Magick++_LIBRARIES - link these to use Magick++

include(LibFindMacros)

# Dependencies
libfind_package(Magick++ ImageMagick)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Magick++_PKGCONF ImageMagick++)

set(SEARCH_PATHS 
  "C:\\Program Files\\ImageMagick-6.6.3-Q8\\include"
  "C:\\Program Files\\ImageMagick-6.6.3-Q8\\lib")

# Include dir
find_path(Magick++_INCLUDE_DIR
  NAMES Magick++.h
  PATHS ${Magick++_PKGCONF_INCLUDE_DIRS} ${SEARCH_PATHS}
)

# Finally the library itself
find_library(Magick++_LIBRARY
  NAMES Magick++ CORE_RL_Magick++_ 
  PATHS ${Magick++_PKGCONF_LIBRARY_DIRS} ${SEARCH_PATHS}
)

# Set the include dir variables and the libraries and let libfind_process do
# the rest.
# NOTE: Singular variables for this library, plural for libraries this this
# lib depends on.
set(Magick++_PROCESS_INCLUDES Magick++_INCLUDE_DIR Magick_INCLUDE_DIRS)
set(Magick++_PROCESS_LIBS Magick++_LIBRARY Magick_LIBRARIES)
libfind_process(Magick++)

if(Magick++_INCLUDE_DIR AND Magick++_LIBRARY)
  set(Magick++_FOUND true)
endif(Magick++_INCLUDE_DIR AND Magick++_LIBRARY)
  