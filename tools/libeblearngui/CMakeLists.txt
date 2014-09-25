################################################################################
#
# CMake configuration for eblearngui project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

include_directories (include)
include_directories(${LIBIDX_INCLUDE_DIR})
include_directories(${LIBIDXGUI_INCLUDE_DIR})
include_directories(${LIBEBLEARN_INCLUDE_DIR})
SET(LIBEBLEARNGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

IF (QT_FOUND) #-----------------------------------------------------------------

  # compile library
  ##############################################################################
  add_library (eblearngui SHARED
    src/datasource_gui.cpp
    src/detector_gui.cpp
#    src/ebl_trainer_gui.cpp
#    src/ebl_transfer_gui.cpp
#    src/ebl_layers_gui.cpp
    )

  # change target name if debugging
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(eblearngui PROPERTIES OUTPUT_NAME "eblearngui_debug")
  endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

  # link executable with external libraries
  ##############################################################################
  target_link_libraries (eblearngui idx)
  target_link_libraries (eblearngui idxgui)
  target_link_libraries (eblearngui eblearn)
  target_link_libraries (eblearngui ${QT_QTCORE_LIBRARY})
  target_link_libraries (eblearngui ${QT_QTGUI_LIBRARY})

ELSE (QT_FOUND) #---------------------------------------------------------------
  MESSAGE("__ WARNING: libeblearngui won't be compiled because of missing QT libraries.")
ENDIF (QT_FOUND) #--------------------------------------------------------------
