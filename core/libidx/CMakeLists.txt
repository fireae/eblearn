################################################################################
#
# CMake configuration for libeblearn project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################


# set include directories
################################################################################
include_directories (include)
SET(LIBIDX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

# compile library
################################################################################
add_library (idx SHARED
  src/idxspec.cpp
  src/idx.cpp
  src/srg.cpp
  src/stl.cpp
  src/idxops.cpp
  src/idxIO.cpp
  src/ipp.cpp
  src/ippops.cpp
  src/th.cpp
  src/thops.cpp
  src/blasops.cpp
  src/color_spaces.cpp
  src/image.cpp
  src/imageIO.cpp
  src/numerics.cpp
  src/utils.cpp
  src/matlab.cpp
  src/smart.cpp
  src/random.cpp
  src/string_utils.cpp
  )

# change target name if debugging
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set_target_properties(idx PROPERTIES OUTPUT_NAME "idx_debug")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# link
################################################################################

IF(CBLAS_FOUND)
  TARGET_LINK_LIBRARIES(idx ${CBLAS_LIBRARIES})
ENDIF(CBLAS_FOUND)

IF(IPP_FOUND)
  TARGET_LINK_LIBRARIES(idx ${IPP_LIBRARIES})
ENDIF(IPP_FOUND)

IF (Magick++_FOUND)
  TARGET_LINK_LIBRARIES(idx ${Magick++_LIBRARY})
ENDIF (Magick++_FOUND)

IF(THC_FOUND)
  TARGET_LINK_LIBRARIES(idx ${THC_LIBRARIES})
ENDIF(THC_FOUND)

IF(MATIO_FOUND)
  TARGET_LINK_LIBRARIES(idx ${MATIO_LIBRARIES})
ENDIF(MATIO_FOUND)

# configuration
################################################################################
CONFIGURE_FILE("include/config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h")
