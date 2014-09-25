################################################################################
#
# CMake configuration for libeblearn project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#   Soumith Chintala, soumith@gmail.com, New York University
#
################################################################################

# set include directories
################################################################################
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(${LIBIDX_INCLUDE_DIR})
SET(LIBEBLEARN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

# compile library
################################################################################
ADD_LIBRARY (eblearn SHARED
		     src/bbox.cpp
#		     src/ebl_arch.cpp
		     src/ebl_logger.cpp
		     src/ebl_module.cpp
		     src/ebl_parameters.cpp
#		     src/ebl_state.cpp
		     src/ebl_utils.cpp
		     src/nms.cpp
		     )

# change target name if debugging
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set_target_properties(eblearn PROPERTIES OUTPUT_NAME "eblearn_debug")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# linking
################################################################################
TARGET_LINK_LIBRARIES(eblearn idx)

if (LUA_FOUND)
  TARGET_LINK_LIBRARIES(eblearn ${LUA_LIBRARIES})
endif (LUA_FOUND)

if (CUDA_FOUND)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET(CUDA_NVCC_FLAGS " -g -G")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
  CUDA_ADD_LIBRARY(eblearn_cuda SHARED src/ebl_cudaops.cu src/ebl_cudautils.cu)
  #src/ebl_cudapooling.cu src/ebl_cudanormalization.cu)
  TARGET_LINK_LIBRARIES(eblearn_cuda ${CUDA_LIBRARIES})
  TARGET_LINK_LIBRARIES(eblearn eblearn_cuda)
endif (CUDA_FOUND)
