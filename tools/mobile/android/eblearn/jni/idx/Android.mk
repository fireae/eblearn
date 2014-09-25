include $(call all-subdir-makefiles)

LOCAL_PATH  := $(call my-dir)

# libidx
include $(CLEAR_VARS)
LOCAL_LDLIBS     := -lm -llog -ljnigraphics
LOCAL_MODULE     := libidx
SRC              := ../../core/libidx/src
LOCAL_C_INCLUDES := $(CORE)/libidx/include/
LOCAL_SRC_FILES  := $(SRC)/color_spaces.cpp \
		    $(SRC)/idx.cpp \
		    $(SRC)/idxIO.cpp \
		    $(SRC)/idxops.cpp \
		    $(SRC)/idxspec.cpp \
		    $(SRC)/image.cpp \
		    $(SRC)/imageIO.cpp \
		    $(SRC)/ippops.cpp \
		    $(SRC)/numerics.cpp \
		    $(SRC)/random.cpp \
		    $(SRC)/smart.cpp \
		    $(SRC)/srg.cpp \
		    $(SRC)/stl.cpp \
		    $(SRC)/utils.cpp \
		    entries.cpp

include $(BUILD_SHARED_LIBRARY)
