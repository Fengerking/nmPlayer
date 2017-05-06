LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := voMP3Dec

CSRC_PATH:=../../../

LOCAL_SRC_FILES := \
        ../../../../../../../../Common/cmnMemory.c \
        $(CSRC_PATH)/minimad.c	 	
	
LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common


VOMM:=-DLINUX

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02220000  $(VOMM) -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -fsigned-char


include $(BUILD_EXECUTABLE)

