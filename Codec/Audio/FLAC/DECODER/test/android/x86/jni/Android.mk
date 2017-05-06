LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


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
VOMM:=-DLINUX -DVOANDROID -DANDROID
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x022d0000  $(VOMM) -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -fsigned-char

include $(BUILD_EXECUTABLE)

