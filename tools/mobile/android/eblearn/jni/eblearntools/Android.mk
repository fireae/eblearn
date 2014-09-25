include $(call all-subdir-makefiles)

LOCAL_PATH  := $(call my-dir)

# libeblearn
include $(CLEAR_VARS)


LOCAL_LDLIBS     := -lm -llog -ljnigraphics
LOCAL_MODULE     := libeblearntools
SRC              := ../../libeblearntools/src
LOCAL_C_INCLUDES := $(EBLEARNTOOLS)/include/ \
		    $(CORE)/libeblearn/include/ \
		    $(CORE)/libidx/include/
LOCAL_SRC_FILES  := \
		  $(SRC)/configuration.cpp	 \
		  $(SRC)/netconf.cpp 		 \
		  $(SRC)/sort.cpp 		 \
		  $(SRC)/tools_utils.cpp 	 \
		  $(SRC)/thread.cpp 	 	 \
		  $(SRC)/dataset.cpp 	 	 \
		  $(SRC)/pascal_dataset.cpp 	 \
		  $(SRC)/pascal_xml.cpp 	\
		  entries.cpp


LOCAL_SHARED_LIBRARIES := libidx libeblearn

include $(BUILD_SHARED_LIBRARY)
