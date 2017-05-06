LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#LOCAL_ARM_MODE := arm

LOCAL_MODULE := voWMADec

CSRC_PATH:=../../../


LOCAL_SRC_FILES := \
        ../../../../../../../../Common/cmnMemory.c \
        $(CSRC_PATH)/main_test_hrd.c 	
	
LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../inc/audio/common/include \
	../../../../inc/audio/v10/include \
	../../../../inc/audio/v10/common \
	../../../../inc/common/include \
	../../../../../../../../Common
	
#VOMM:= -DHAVE_NEON=1 -DVOARMV7 -DVOANDROID -DANDROID -DLINUX -DBUILD_WITHOUT_C_LIB -DBUILD_INTEGER -DDISABLE_UES -DENABLE_LPC -DENABLE_ALL_ENCOPT -DWMA_DECPK_BUILD
VOMM:=-DLINUX -DBUILD_INTEGER -DDISABLE_UES -DENABLE_LPC -DENABLE_ALL_ENCOPT -DWMA_DECPK_BUILD

#LOCAL_CFLAGS := -D_VOMODULEID=0x02230000  $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_CFLAGS := -D_VOMODULEID=0x02230000  $(VOMM) -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -fsigned-char


include $(BUILD_EXECUTABLE)
