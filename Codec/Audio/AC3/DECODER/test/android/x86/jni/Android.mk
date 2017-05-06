LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := voAC3Dec



CSRC_PATH:=../../../

LOCAL_SRC_FILES := \
        ../../../../../../../../Common/cmnMemory.c \
        $(CSRC_PATH)/voAC3_D_SAMPLE.c	 	
	
LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../include \
	../../../../../../../../Common

	
VOMM:= -DVOANDROID -DANDROID -DLINUX -DKCAPABLE -DHALFRATE

#LOCAL_CFLAGS := -D_VOMODULEID=0x022c0000  $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_CFLAGS := -D_VOMODULEID=0x022c0000  $(VOMM) -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -fsigned-char

include $(BUILD_EXECUTABLE)

