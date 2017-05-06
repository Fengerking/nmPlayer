LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := voRADec


CSRC_PATH:=../../../

LOCAL_SRC_FILES := \
        ../../../../../../../../Common/cmnMemory.c \
        $(CSRC_PATH)/RADecSample.c	 	
	


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common


#VOMM:=-DARM -DARMV4 -DARMV6 -DARMV7 -DLCHECK -DLINUX
VOMM:=-DARM -DARMV4 -DARMV6 -DARMV7 -DLINUX -DVOANDROID -DANDROID
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02240000  $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp


include $(BUILD_EXECUTABLE)

