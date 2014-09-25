EXECUTE_PROCESS(COMMAND pkg-config libxml++-2.6 --libs
  OUTPUT_VARIABLE xml_lib_output
  RESULT_VARIABLE xml_lib_result
  ERROR_VARIABLE xml_lib_output2)

# strip trailing whitespaces and newline
STRING(REGEX REPLACE "[ ]+\n" "" xml_lib_output "${xml_lib_output}")

EXECUTE_PROCESS(COMMAND pkg-config libxml++-2.6 --cflags
  OUTPUT_VARIABLE xml_output
  RESULT_VARIABLE xml_result
  ERROR_VARIABLE xml_output2)

IF(xml_result MATCHES 0)
  # remove -I and separate arguments (required for include_directories)
  STRING(REPLACE "-I" "" xml_output "${xml_output}")
  SEPARATE_ARGUMENTS(xml_output)  

  SET(XML_FOUND TRUE)
  MESSAGE(STATUS "Found libXml++")
  SET(XML_INCLUDE_DIRS ${xml_output})
  SET(XML_LIBRARY_FLAGS ${xml_lib_output})

ELSE(xml_result MATCHES 0)
  SET(XML_FOUND FALSE)
  MESSAGE("__ WARNING: libXml++ not found (optional).")
ENDIF(xml_result MATCHES 0)
