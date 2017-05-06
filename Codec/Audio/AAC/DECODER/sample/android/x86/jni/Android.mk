LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := voAACDec

CSRC_PATH:=../../../

LOCAL_SRC_FILES := \
        ../../../../../../Common/cmnAudioMemory.c \
        $(CSRC_PATH)/AAC_D_SAMPLE.c	 	
	
LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../include \
	../../../../../../Common

VOMM:= -DVOANDROID -DANDROID -DLINUX 

LOCAL_CFLAGS := -D_VOMODULEID=0x02210000  $(VOMM) -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -fsigned-char


include $(BUILD_EXECUTABLE)
