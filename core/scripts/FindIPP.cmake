################################################################################
# Intel IPP
################################################################################

FIND_PATH(IPP_INCLUDE_DIR "ipp.h" PATHS
  "$ENV{ProgramFiles}/Intel/IPP/*/ia32/include"
  "/opt/intel/ipp/*/ia32/include"
  "/opt/intel/ipp/*/em64t/include"
  "/usr/local/intel/ipp/*/ia32/include"
  "$ENV{IPPROOT}/include")

FIND_PATH(IPP_LIBRARIES_DIR "libguide.so" PATHS
  "$ENV{ProgramFiles}/Intel/IPP/*.*/ia32/sharedlib"
  "/opt/intel/ipp/*/ia32/sharedlib"
  "/opt/intel/ipp/*/em64t/sharedlib"
  "/usr/local/intel/ipp/*/ia32/sharedlib"
  "$ENV{IPPROOT}/sharedlib")

IF (DEFINED ENV{FORCE_IPP_DIR})
  SET(IPP_INCLUDE_DIR "$ENV{FORCE_IPP_DIR}/include")
  SET(IPP_LIBRARIES_DIR "$ENV{FORCE_IPP_DIR}/sharedlib")
ENDIF (DEFINED ENV{FORCE_IPP_DIR})

IF (IPP_INCLUDE_DIR)
  MESSAGE(STATUS "Found Intel IPP include: ${IPP_INCLUDE_DIR}")
ENDIF (IPP_INCLUDE_DIR)
IF (IPP_LIBRARIES_DIR)
  MESSAGE(STATUS "Found Intel IPP libraries: ${IPP_LIBRARIES_DIR}")
ENDIF (IPP_LIBRARIES_DIR)

IF (IPP_LIBRARIES_DIR AND IPP_INCLUDE_DIR)
  SET(IPP_FOUND TRUE)
ENDIF (IPP_LIBRARIES_DIR AND IPP_INCLUDE_DIR)


################################################################################
