# find library directory
EXECUTE_PROCESS(COMMAND pkg-config libfreenect --libs
  OUTPUT_VARIABLE kinect_lib_output
  RESULT_VARIABLE kinect_lib_result
  ERROR_VARIABLE kinect_lib_output2)

# strip trailing whitespaces and newline
STRING(REGEX REPLACE "[ ]+\n" "" kinect_lib_output "${kinect_lib_output}")

# find include directory
EXECUTE_PROCESS(COMMAND pkg-config libfreenect --cflags
  OUTPUT_VARIABLE kinect_output
  RESULT_VARIABLE kinect_result
  ERROR_VARIABLE kinect_output2)

IF(kinect_result MATCHES 0)
  # remove -I and separate arguments (required for include_directories)
  STRING(REPLACE "-I" "" kinect_output "${kinect_output}")
  SEPARATE_ARGUMENTS(kinect_output)  

  SET(KINECT_FOUND TRUE)
  # TODO: remove setting by hand
  #SET(KINECT_INCLUDE_DIRS "/usr/include")
  #SET(KINECT_LIBRARY_FLAGS "freenect_sync freenect")
   SET(KINECT_INCLUDE_DIRS ${kinect_output})
   SET(KINECT_LIBRARY_FLAGS ${kinect_lib_output})
  MESSAGE(STATUS "Found Kinect library (freenect)")
  MESSAGE(STATUS "Kinect include directory: ${KINECT_INCLUDE_DIRS}")
  MESSAGE(STATUS "Kinect libraries: ${KINECT_LIBRARY_FLAGS}")

ELSE(kinect_result MATCHES 0)
  SET(KINECT_FOUND FALSE)
  MESSAGE("__ WARNING: Kinect library not found (optional).")
ENDIF(kinect_result MATCHES 0)
