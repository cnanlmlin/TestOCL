LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_LDLIBS := -ldl -llog
LOCAL_MODULE    := helloOCL  
LOCAL_CXXFLAGS += -fno-exceptions
LOCAL_SRC_FILES := helloCLcore.c helloCL.c
include $(BUILD_SHARED_LIBRARY)  
