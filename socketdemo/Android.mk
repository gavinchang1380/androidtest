LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS :=optional
LOCAL_C_INCLUDES := $(KERNEL_HEADERS)
LOCAL_SHARED_LIBRARIES := libcutils liblog  libutils
LOCAL_LDLIBS	:= -lm -llog 

LOCAL_MODULE:= socketdemo_server
LOCAL_SRC_FILES:= server.c
include $(BUILD_EXECUTABLE)
