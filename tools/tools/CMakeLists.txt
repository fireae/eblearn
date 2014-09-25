################################################################################
#
# CMake configuration for tools
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#   Soumith Chintala, soumith@gmail.com, New York University
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
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   set(NAME_EXTRA "_debug")
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   set(NAME_EXTRA "")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# compile executable: train
################################################################################
# change target name if debugging	
set(TRAIN "train${NAME_EXTRA}")
add_executable (${TRAIN} src/train.cpp)
# link executable with external libraries
target_link_libraries (${TRAIN} idx eblearn eblearntools ${GPROF_LIBRARIES})
LINK_QT(${TRAIN} idxgui)
LINK_QT(${TRAIN} eblearngui)
 
# compile executable: dsfprop
################################################################################
set(DSFPROP "dsfprop${NAME_EXTRA}")
add_executable (${DSFPROP} src/dsfprop.cpp)
# link executable with external libraries
target_link_libraries (${DSFPROP} eblearn idx eblearntools)
LINK_QT(${DSFPROP} idxgui)
LINK_QT(${DSFPROP} eblearngui)
LINK_MAGICKPP(${DSFPROP})

# compile executable: imfprop
################################################################################
set(IMFPROP "imfprop${NAME_EXTRA}")
add_executable (${IMFPROP} src/imfprop.cpp)
# link executable with external libraries
target_link_libraries (${IMFPROP} eblearn idx eblearntools)
LINK_QT(${IMFPROP} idxgui)
LINK_QT(${IMFPROP} eblearngui)
LINK_MAGICKPP(${IMFPROP})

# # compile executable: stdetect
# ################################################################################
# add_executable (stdetect src/stdetect.cpp)
# # link executable with external libraries
# target_link_libraries (stdetect eblearn idx eblearntools)
# LINK_QT(stdetect idxgui)
# LINK_QT(stdetect eblearngui)
# LINK_BOOST(stdetect system)
# LINK_BOOST(stdetect filesystem)
# LINK_BOOST(stdetect regex)
# LINK_MAGICKPP(stdetect)

# compile executable: detect
################################################################################
set(DETECT "detect${NAME_EXTRA}")
add_executable (${DETECT} src/detect.cpp)
# link executable with external libraries
target_link_libraries (${DETECT} eblearn idx eblearntools)
LINK_QT(${DETECT} idxgui)
LINK_QT(${DETECT} eblearngui)
LINK_BOOST(${DETECT} system)
LINK_BOOST(${DETECT} filesystem)
LINK_BOOST(${DETECT} regex)
LINK_MAGICKPP(${DETECT})
LINK_LUA(${DETECT})

# compile executable: mpidetect
################################################################################
IF (MPI_FOUND AND Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)
IF (WINDOWS) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating mpidetect project under Windows")
ELSE (WINDOWS) # do not compile this project under windows
set(MPIDETECT "mpidetect${NAME_EXTRA}")
add_executable (${MPIDETECT} src/mpidetect.cpp)
# link executable with external libraries
target_link_libraries (${MPIDETECT} eblearn idx eblearntools)
LINK_QT(${MPIDETECT} idxgui)
LINK_QT(${MPIDETECT} eblearngui)
LINK_BOOST(${MPIDETECT} system)
LINK_BOOST(${MPIDETECT} filesystem)
LINK_BOOST(${MPIDETECT} regex)
LINK_BOOST(${MPIDETECT} serialization)
LINK_BOOST(${MPIDETECT} mpi)
#LINK_MPI(${MPIDETECT})

ENDIF (WINDOWS) # do not compile this project under windows
ELSE (MPI_FOUND AND Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)
  MESSAGE("__ WARNING: mpidetect will not be compiled because of missing MPI libraries (optional).")
ENDIF (MPI_FOUND AND Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)

# # compile executable: track
# ################################################################################
# add_executable (track
#   src/track.cpp
#   )

# # link executable with external libraries
# target_link_libraries (track eblearn idx eblearntools)
# LINK_QT(track idxgui)
# LINK_QT(track eblearngui)
# LINK_BOOST(track system)
# LINK_BOOST(track filesystem)
# LINK_BOOST(track regex)

# compile executable: dscompile
################################################################################
set(DSCOMPILE "dscompile${NAME_EXTRA}")
add_executable (${DSCOMPILE} src/dscompile.cpp)
# link executable with external libraries
target_link_libraries (${DSCOMPILE} idx eblearn eblearntools)
LINK_QT(${DSCOMPILE} idxgui)
LINK_BOOST(${DSCOMPILE} system)
LINK_BOOST(${DSCOMPILE} filesystem)
LINK_BOOST(${DSCOMPILE} regex)
LINK_MAGICKPP(${DSCOMPILE})

# compile executable: dssplit
################################################################################
set(DSSPLIT "dssplit${NAME_EXTRA}")
add_executable (${DSSPLIT} src/dssplit.cpp)
# link executable with external libraries
target_link_libraries (${DSSPLIT} eblearn idx eblearntools)
LINK_QT(${DSSPLIT} idxgui)
LINK_QT(${DSSPLIT} eblearngui)
LINK_BOOST(${DSSPLIT} system)
LINK_BOOST(${DSSPLIT} filesystem)
LINK_BOOST(${DSSPLIT} regex)
LINK_MAGICKPP(${DSSPLIT})

# compile executable: dsmerge
################################################################################
set(DSMERGE "dsmerge${NAME_EXTRA}")
add_executable (${DSMERGE} src/dsmerge.cpp)
# link executable with external libraries
target_link_libraries (${DSMERGE} eblearn idx eblearntools)
LINK_QT(${DSMERGE} idxgui)
LINK_QT(${DSMERGE} eblearngui)
LINK_BOOST(${DSMERGE} system)
LINK_BOOST(${DSMERGE} filesystem)
LINK_BOOST(${DSMERGE} regex)
LINK_MAGICKPP(${DSMERGE})

# compile executable: dsdisplay
################################################################################
set(DSDISPLAY "dsdisplay${NAME_EXTRA}")
add_executable (${DSDISPLAY} src/dsdisplay.cpp)
# link executable with external libraries
target_link_libraries (${DSDISPLAY} eblearn idx eblearntools)
LINK_QT(${DSDISPLAY} idxgui)
LINK_QT(${DSDISPLAY} eblearngui)
LINK_BOOST(${DSDISPLAY} system)
LINK_BOOST(${DSDISPLAY} filesystem)
LINK_BOOST(${DSDISPLAY} regex)

# compile executable: matshow
################################################################################
# tell compiler to build WinMain application, not a console app
IF (WINDOWS AND NOT QT_FOUND) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating matshow project because of missing QT")
ELSE (WINDOWS AND NOT QT_FOUND) # do not compile this project under windows

IF (WINDOWS)
#  SET(WINTYPE WIN32)
#  MESSAGE(STATUS "Building matshow as a windowed application (not console)")
ENDIF (WINDOWS)

set(MATSHOW "matshow${NAME_EXTRA}")
add_executable (${MATSHOW} ${WINTYPE} src/matshow.cpp)
# link executable with external libraries
target_link_libraries (${MATSHOW} idx eblearntools)
LINK_QT(${MATSHOW} idxgui)
LINK_QT(${MATSHOW} eblearngui)
LINK_MAGICKPP(${MATSHOW})

ENDIF (WINDOWS AND NOT QT_FOUND)

# compile executable: capture
################################################################################
set(CAPTURE "capture${NAME_EXTRA}")
add_executable (${CAPTURE} src/capture.cpp)
# link executable with external libraries
target_link_libraries (${CAPTURE} idx eblearntools)
LINK_QT(${CAPTURE} idxgui)

# compile executable: metarun
################################################################################
IF (WINDOWS) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating metarun project under Windows")
ELSE (WINDOWS) # do not compile this project under windows
set(METARUN "metarun${NAME_EXTRA}")
add_executable (${METARUN}
  src/metarun.cpp
  )
# link executable with external libraries
target_link_libraries (${METARUN} idx eblearntools)
#LINK_BOOST(${METARUN} system)
#LINK_BOOST(${METARUN} filesystem)
#LINK_BOOST(${METARUN} regex)
ENDIF (WINDOWS) # do not compile this project under windows

# compile executable: metaparse
################################################################################
set(METAPARSE "metaparse${NAME_EXTRA}")
add_executable (${METAPARSE} src/metaparse.cpp)
# link executable with external libraries
target_link_libraries (${METAPARSE} eblearntools)
LINK_BOOST(${METAPARSE} system)
LINK_BOOST(${METAPARSE} filesystem)
LINK_BOOST(${METAPARSE} regex)

# compile executable: maketable
################################################################################
set(MAKETABLE "maketable${NAME_EXTRA}")
add_executable (${MAKETABLE} src/maketable.cpp)
# link executable with external libraries
target_link_libraries (${MAKETABLE} idx eblearn eblearntools)

# compile executable: narrow
################################################################################
set(NARROW "narrow${NAME_EXTRA}")
add_executable(${NARROW} src/narrow.cpp)
# link executable with external libraries
target_link_libraries (${NARROW} idx eblearntools)

# compile executable: ebl2matlab
################################################################################
IF (MATIO_FOUND)
  set(EBL2MATLAB "ebl2matlab${NAME_EXTRA}")
  add_executable(${EBL2MATLAB} src/ebl2matlab.cpp)
  # link executable with external libraries
  target_link_libraries (${EBL2MATLAB} idx eblearntools)
ELSE (MATIO_FOUND)
  MESSAGE("__ WARNING: Not generating ebl2matlab because MATIO was not found")
ENDIF (MATIO_FOUND)

# compile executable: mconvert
################################################################################
set(MCONVERT "mconvert${NAME_EXTRA}")
add_executable(${MCONVERT} src/mconvert.cpp)
# link executable with external libraries
target_link_libraries (${MCONVERT} idx eblearntools)

# compile executable: videoffinder
################################################################################
# IF (OpenCV_FOUND) #-------------------------------------------------------------
# add_executable (videoffinder
#   src/video_features_finder.cpp
#   src/similar_patches.cpp
#   )

# # link executable with external libraries
# ################################################################################
# target_link_libraries (videoffinder eblearn idx)
# IF (QT_FOUND)
#   target_link_libraries (videoffinder idxgui)
# ENDIF (QT_FOUND)
# IF (OpenCV_FOUND)
#   target_link_libraries (videoffinder cv highgui cxcore)
# ENDIF (OpenCV_FOUND)

# ELSE (OpenCV_FOUND)
#   MESSAGE(STATUS "___WARNING___: videoffinder won't be compiled because of missing OpenCV libraries.")
# ENDIF (OpenCV_FOUND) #----------------------------------------------------------
