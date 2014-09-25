# Custom cmake configuration
# Put here paths to manually installed libraries if necessary
################################################################################

################################################################################
# matio (http://sourceforge.net/projects/matio/)
#SET(MATIO_FOUND TRUE)
#SET(MATIO_INCLUDE_DIR "$ENV{HOME}/matio/include")
#SET(MATIO_LIBRARIES "$ENV{HOME}/matio/lib/libmatio.so")




################################################################################
# SET(VLFEAT_FOUND TRUE)
# SET(VLFEAT_INCLUDE_DIR "/home/sc3104/vlfeat/vlfeat-0.9.14/")
# SET(VLFEAT_LIBRARIES_DIR "/home/sc3104/vlfeat/vlfeat-0.9.14/bin/glnxa64/")



################################################################################
# vlfeat (http://www.vlfeat.org/)
################################################################################
# SET(VLFEAT_FOUND TRUE)
# SET(VLFEAT_INCLUDE_DIR "/home/sc3104/vlfeat/vlfeat-0.9.14/")
# SET(VLFEAT_LIBRARIES_DIR "/home/sc3104/vlfeat/vlfeat-0.9.14/bin/glnxa64/")

################################################################################
# google-perftools-profiler(cpu)
################################################################################
# SET(GPROF_FOUND TRUE)
# SET(GPROF_INCLUDE_DIR "/home/sc3104/include/")
# SET(GPROF_LIBRARIES_DIR "/home/sc3104/lib/")

################################################################################
# Torch Cuda library(THC)
################################################################################
#SET(THC_FOUND TRUE)
#SET(THC_DIR "/home/sc3104/th")
#SET(THC_INCLUDE_DIR "${THC_DIR}/include/")
#SET(THC_LIBRARIES_DIR "${THC_DIR}/lib/")

################################################################################
# Intel IPP
################################################################################
# SET(IPP_FOUND TRUE)
# SET(IPP_INCLUDE_DIR "/home/pierre/ipp/include/")
# SET(IPP_LIBRARIES_DIR "/home/pierre/ipp/sharedlib/")

################################################################################
#SET(Boost_FOUND TRUE)
#SET(Boost_INCLUDE_DIRS "$ENV{HOME}/installed/boost/include")
#SET(Boost_LIBRARY_DIRS "$ENV{HOME}/installed/boost/lib")
#SET(Boost_MINOR_VERSION 35)

# SET(CPPUNIT_FOUND TRUE)
# SET(CPPUNIT_INCLUDE_DIR "C:/eblinstall/cppunit-1.12.1/include")
# SET(CPPUNIT_LIBRARY "C:/eblinstall/cppunit-1.12.1/lib/cppunitd.lib")
# MESSAGE(STATUS "Found CPPUNIT")
  
#SET(PTHREAD_FOUND TRUE)
#SET(PTHREAD_INCLUDE_DIR "c:/eblinstall/pthreads/Pre-built.2/include")
#SET(PTHREAD_LIBRARY "c:/eblinstall/pthreads/Pre-built.2/lib/pthreadVC2.lib")

#SET(QT_DIR "c:/Qt")
#SET(QT_QMAKE_EXECUTABLE "c:/Qt/bin/qmake.exe")
#SET(QT_MOC_EXECUTABLE "c:/Qt/bin/moc.exe")
#SET(QT_RCC_EXECUTABLE "c:/Qt/bin/rcc.exe")
#SET(QT_UIC_EXECUTABLE "c:/Qt/bin/uic.exe")
#SET(QT_INCLUDE_DIR "c:/Qt/include")
#SET(QT_LIBRARY_DIR "c:/Qt/lib")
#SET(QT_QTCORE_LIBRARY "c:/Qt/lib/QtCore4.lib")
#SET (QT_FOUND TRUE)
  
IF ($ENV{USEMPI})
  SET(MPI_FOUND TRUE)
#  SET(MPI_INCLUDE_PATH "/share/apps/mvapich/1.1-2009-12-05/intel/include/")
#  SET(MPI_LIBRARIES "/share/apps/mvapich/1.1-2009-12-05/intel/lib/")
#  SET(MPI_INCLUDE_PATH "/share/apps/openmpi/1.3.3/intel/include/")
#  SET(MPI_LIBRARIES "/share/apps/openmpi/1.3.3/intel/lib/")
set(MPI_INCLUDE_PATH "/share/apps/openmpi/1.4.3/intel/include")
set(MPI_LIBRARIES "/share/apps/openmpi/1.4.3/intel/lib")
set(Boost_INCLUDE_DIRS "/share/apps/boost/1.44.0/intel/all/include")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}) 

ENDIF ($ENV{USEMPI})

IF ($ENV{HPCNYU})
IF ($ENV{USEMAGICKPP})

IF ($ENV{HPCUSQ})
  set(IMVERSION "6.6.2-0")
ELSE ($ENV{HPCUSQ})
  set(IMVERSION "6.6.7-7")
ENDIF ($ENV{HPCUSQ})

  set(Magick++_LIBRARY "Magick++")
  set(Magick++_LIBRARIES_DIR "/share/apps/imagemagick/${IMVERSION}/gnu/lib")
  set(Magick++_INCLUDE_DIR "/share/apps/imagemagick/${IMVERSION}/gnu/include/ImageMagick/")
  set(Magick++_FOUND TRUE)
  include_directories(${Magick++_INCLUDE_DIR})
  link_directories(${Magick++_LIBRARIES_DIR})
  MESSAGE(STATUS "Magick++ includes: ${Magick++_INCLUDE_DIR}")
  MESSAGE(STATUS "Magick++ libraries: ${Magick++_LIBRARIES_DIR}")
  MESSAGE(STATUS "Magick++ library: ${Magick++_LIBRARY}")
  MESSAGE(STATUS "Magick++ Found.")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__MAGICKPP__")  
ENDIF ($ENV{USEMAGICKPP})
ENDIF ($ENV{HPCNYU})


#IF ($ENV{USEMPI})
#  SET(MPI_FOUND TRUE)
#set(MPI_INCLUDE_PATH "/usr/include/openmpi-x86_64/")
#set(MPI_LIBRARIES "/usr/lib64/compat-openmpi/lib/")
#set(Boost_INCLUDE_DIRS "/usr/include/")   
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
#ENDIF ($ENV{USEMPI})
