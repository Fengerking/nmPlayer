LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := voMP3Dec

CSRC_PATH:=../../../

LOCAL_SRC_FILES := \
        ../../../../../../../../Common/cmnMemory.c \
        $(CSRC_PATH)/minimad.c	 	
	
LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common


VOMM:=-DARM -DARMV6 -DARMV7 -DLINUX

# about info option, do not need to care it

#LOCAL_CFLAGS := -D_VOMODULEID=0x02220000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon

LOCAL_CFLAGS := -D_VOMODULEID=0x02220000  $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp


include $(BUILD_EXECUTABLE)

