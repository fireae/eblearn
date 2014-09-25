################################################################################
#
# CMake configuration for idxgui library
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

include_directories (include)
include_directories(${LIBIDX_INCLUDE_DIR})
SET(LIBIDXGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

IF (QT_FOUND) #-----------------------------------------------------------------

  # QT specific: produce moc files from headers
  ##############################################################################
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

  # convert flags string to list
  string(REPLACE " " ";" dflags ${CMAKE_CXX_DFLAGS})

  if (qt3d_found)
    QT4_WRAP_CPP(IDXGUI_SRC_MOC
      include/gui_thread.h include/idxgui.h include/win2d.h include/win3d.h
      OPTIONS  ${dflags})
    set(MOCSRC "include/")
    SET_SOURCE_FILES_PROPERTIES(src/win3d.cpp PROPERTIES
      OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${MOCSRC}moc_win3d.cxx)
  else (qt3d_found)
    QT_WRAP_CPP(idxgui IDXGUI_SRC_MOC
      include/gui_thread.h include/idxgui.h include/win2d.h)
    set(MOCSRC "")
  endif (qt3d_found)

  SET_SOURCE_FILES_PROPERTIES(src/gui_thread.cpp PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${MOCSRC}moc_gui_thread.cxx)
  SET_SOURCE_FILES_PROPERTIES(src/idxgui.cpp PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${MOCSRC}moc_idxgui.cxx)
  SET_SOURCE_FILES_PROPERTIES(src/win2d.cpp PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${MOCSRC}moc_win2d.cxx)

  # compile library
  ##############################################################################
  add_library (idxgui SHARED
    src/gui_thread.cpp
    src/idxgui.cpp
    src/win.cpp
    src/win2d.cpp
    src/win3d.cpp
    src/gui.cpp
    src/scroll_box0.cpp
    src/scroll_box.cpp
    )


  # change target name if debugging
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(idxgui PROPERTIES OUTPUT_NAME "idxgui_debug")
  endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
  # tell the compiled files that we are building libidxgui
  SET_PROPERTY(TARGET idxgui PROPERTY COMPILE_DEFINITIONS LIBIDXGUI)

  # link executable with external libraries
  ##############################################################################
  target_link_libraries (idxgui idx)
  target_link_libraries (idxgui ${QT_QTCORE_LIBRARY})
  target_link_libraries (idxgui ${QT_QTGUI_LIBRARY})

  if (qt3d_found)
    target_link_libraries (idxgui QtCore Qt3D QtOpenGL)
  endif (qt3d_found)

ELSE (QT_FOUND) #---------------------------------------------------------------
  MESSAGE("__ WARNING: libidxgui won't be compiled because of missing QT libraries.")
ENDIF (QT_FOUND) #--------------------------------------------------------------
