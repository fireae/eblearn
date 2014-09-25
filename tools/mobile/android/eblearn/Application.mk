APP_PROJECT_PATH := $(call my-dir)
APP_MODULES      := libidx libeblearn libeblearntools
#APP_OPTIM        := release # debug
APP_CFLAGS       := -D__ANDROID__ -D__LINUX__ # -D__DEBUG__ # -D__DUMP_STATES__
CORE             := $(APP_PROJECT_PATH)/core
EBLEARNTOOLS     := $(APP_PROJECT_PATH)/libeblearntools
APP_ABI          := armeabi-v7a
