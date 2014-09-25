# Find valgind
################################################################
FIND_PROGRAM(VALGRIND_EXECUTABLE 
  NAMES valgrind
  PATHS /usr/bin)
IF (VALGRIND_EXECUTABLE)
  SET(VALGRIND_FOUND TRUE)
ENDIF (VALGRIND_EXECUTABLE)

IF (VALGRIND_FOUND)
  MESSAGE(STATUS "Found valgrind: ${VALGRIND_EXECUTABLE}")
ELSE (VALGRIND_FOUND)
  MESSAGE("Warning:   Could not find valgrind.")
ENDIF (VALGRIND_FOUND)

