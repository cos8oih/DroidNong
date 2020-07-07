LOCAL_PATH := $(call my-dir)

#NongHack
include $(CLEAR_VARS)

LOCAL_MODULE := nong
LOCAL_CPPFLAGS += -std=c++17
LOCAL_SRC_FILES := main.cpp

include $(BUILD_SHARED_LIBRARY)