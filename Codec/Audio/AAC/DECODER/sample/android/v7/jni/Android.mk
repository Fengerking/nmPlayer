LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := voAACDec

CSRC_PATH:=../../../

LOCAL_SRC_FILES := \
        ../../../../../../Common/cmnAudioMemory.c \
        $(CSRC_PATH)/AAC_D_SAMPLE.c	 	
	
LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../include \
	../../../../../../Common

VOMM:= -DHAVE_NEON=1 -DVOARMV7 -DVOANDROID -DANDROID -DLINUX 

LOCAL_CFLAGS := -D_VOMODULEID=0x02210000  $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp


include $(BUILD_EXECUTABLE)
