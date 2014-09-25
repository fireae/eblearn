# Custom cmake configuration
# Put here paths to manually installed libraries if necessary
################################################################################

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
 # SET(CPPUNIT_INCLUDE_DIR "B:/windows/installed/cppunit-1.12.1/include")
 # SET(CPPUNIT_LIBRARY "B:/windows/installed/cppunit-1.12.1/lib/cppunitd.lib")
 # MESSAGE(STATUS "Found CPPUNIT")

IF ($ENV{USEMPI})
  SET(MPI_FOUND TRUE)
  SET(MPI_INCLUDE_PATH "/share/apps/mvapich/1.1-2009-12-05/intel/include/")
  SET(MPI_LIBRARIES "/share/apps/mvapich/1.1-2009-12-05/intel/lib/")
#  SET(MPI_INCLUDE_PATH "/share/apps/openmpi/1.3.3/intel/include/")
#  SET(MPI_LIBRARIES "/share/apps/openmpi/1.3.3/intel/lib/")
ENDIF ($ENV{USEMPI})
