LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoColorConversion


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
	$(CSRC_PATH)/ccYUVToYUV.c

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)
		  
VOMM:=-DLINUX -DARCH_IS_GENERIC -DARCH_IS_32BIT -DARCH_IS_LITTLE_ENDIAN -DCOLCONVENABLE -D__VO_NDK__ -D_VOLOG_INFO -D_LINUX -D_LINUX_ANDROID -DSTABILITY -DLICENSEFILE -DVOANDROID=1

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x05040000   -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32

LOCAL_LDLIBS:=../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

