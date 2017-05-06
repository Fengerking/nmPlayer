LOCAL_PATH := $(call my-dir)
VOTOP?=../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoSrcRTSPTrans

LOCAL_SRC_FILES := \
	RTSPTrans.cpp


LOCAL_CFLAGS := -D_VOMODULEID=0x01110000  -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS += -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

