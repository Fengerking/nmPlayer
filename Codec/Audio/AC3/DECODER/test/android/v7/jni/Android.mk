LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := voAC3Dec



CSRC_PATH:=../../../

LOCAL_SRC_FILES := \
        ../../../../../../../../Common/cmnMemory.c \
        $(CSRC_PATH)/voAC3_D_SAMPLE.c	 	
	
LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../include \
	../../../../../../../../Common

	
VOMM:= -DHAVE_NEON=1 -DVOARMV7 -DVOANDROID -DANDROID -DLINUX -DARM -DARMV6 -DLITEND -DKCAPABLE -DAC3D_ARM_OPT -DHALFRATE

LOCAL_CFLAGS := -D_VOMODULEID=0x022c0000  $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp


include $(BUILD_EXECUTABLE)

