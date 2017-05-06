LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := voH265DecSample

CSRC_PATH:=../../sampleCode

LOCAL_SRC_FILES := \
        $(CSRC_PATH)/voH265DecSample.c	 	
	
LOCAL_C_INCLUDES := \
	../../Include \
	$(CSRC_PATH)

VOMM:= -DHAVE_NEON=1 -DVOARMV7 -DVOANDROID -DANDROID -DLINUX 

LOCAL_CFLAGS := -D_VOMODULEID=0x020e0000  $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp


include $(BUILD_EXECUTABLE)

