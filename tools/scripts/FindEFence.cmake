# Find electric-fence Library
################################################################
FIND_LIBRARY(EFENCE_LIBRARY 
  NAMES efence 
  PATH 
  /usr/lib 
  /usr/local/lib
  /sw/lib
  /sw/local/lib
  /opt/local/lib
  )
IF (EFENCE_LIBRARY)
  SET(EFENCE_FOUND TRUE)
ENDIF (EFENCE_LIBRARY)

IF (EFENCE_FOUND)
  MESSAGE(STATUS "Found electric-fence: ${EFENCE_LIBRARY}")
ELSE (EFENCE_FOUND)
  MESSAGE("Warning:   Could not find electric-fence library.")
ENDIF (EFENCE_FOUND)

