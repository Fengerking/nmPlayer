LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoMMCCRRS

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../src

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/ccConstant.c \
	$(CSRC_PATH)/ccMain.c \
	$(CSRC_PATH)/ccRGBToRGB.c \
	$(CSRC_PATH)/ccRGBToRGBMB.c \
	$(CSRC_PATH)/ccYUVToRGB.c \
	$(CSRC_PATH)/ccYUVToRGBMB.c \
	$(CSRC_PATH)/ccYUVToYUV.c \
	$(CSRC_PATH)/yuv2rgb16tab.c

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



#VOMM:=-DVOARMV6  -DLINUX -DARCH_IS_GENERIC -DARCH_IS_32BIT -DARCH_IS_LITTLE_ENDIAN
VOMM:=-DLINUX

# about info option, do not need to care it
#LOCAL_CFLAGS := -D_VOMODULEID=0x05030000   -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char -msoft-float
LOCAL_CFLAGS := -D_VOMODULEID=0x05030000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32

LOCAL_LDLIBS:=../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

