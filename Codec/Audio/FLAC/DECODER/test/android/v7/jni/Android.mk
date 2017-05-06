LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := voFLACDec

CSRC_PATH:=../../../

LOCAL_SRC_FILES := \
        ../../../../../../../../Common/cmnMemory.c \
        $(CSRC_PATH)/voFLAC_D_SAMPLE.c	 


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


#VOMM:=-DLINUX -DARM -DARMV6 -DARMV7 -DASM_OPT -DLCHECK
VOMM:=-DLINUX -DARM -DARMV6 -DARMV7 -DASM_OPT
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x022d0000  $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp


include $(BUILD_EXECUTABLE)

