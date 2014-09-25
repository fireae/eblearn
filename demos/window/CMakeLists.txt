################################################################################
#
# CMake configuration for the window demo project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

include_directories(include)
include_directories(${LIBEBLEARN_INCLUDE_DIR})
include_directories(${LIBEBLEARNTOOLS_INCLUDE_DIR})
include_directories(${LIBIDX_INCLUDE_DIR})
IF (QT_FOUND)
  include_directories(${LIBIDXGUI_INCLUDE_DIR})
  include_directories(${LIBEBLEARNGUI_INCLUDE_DIR})
ENDIF (QT_FOUND)

# compile executable: window
################################################################################
add_executable (window
  src/window.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (window eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (window idxgui)
  target_link_libraries (window eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(window ${Boost_SYSTEM_LIBRARY}
      ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
  ELSE(${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(window ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ENDIF (${Boost_MINOR_VERSION} GREATER 34)
ENDIF (Boost_FOUND)
