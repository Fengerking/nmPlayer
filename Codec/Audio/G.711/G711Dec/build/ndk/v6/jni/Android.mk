LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoG711Dec

CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../src


LOCAL_SRC_FILES := \
        $(CMNSRC_PATH)/cmnMemory.c\
	$(CSRC_PATH)/mem_align.c\
	$(CSRC_PATH)/g711.c\
	$(CSRC_PATH)/g711_dec_api.c
	  
LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	$(CMNSRC_PATH) \
	../../../../inc

VOMM:= -DHAVE_NEON=1 -DVOARMV6 -DVOANDROID -DANDROID -DNDEBUG -DLCHECK

LOCAL_CFLAGS := -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -mtpcs-frame -mthumb
LOCAL_LDLIBS := -llog ../../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

