LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoJPEGDec


CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../code

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/jcommon.c \
	$(CSRC_PATH)/jdecode.c \
	$(CSRC_PATH)/jdHuffum.c \
	$(CSRC_PATH)/jdidct.c \
	$(CSRC_PATH)/jexif.c \
	$(CSRC_PATH)/jheader.c \
	$(CSRC_PATH)/jtab.c \
	$(CSRC_PATH)/mem_align.c \
	$(CSRC_PATH)/JPEG_D_API.c \
	$(CSRC_PATH)/asm/gnu_asm/ARMV6/voIdctArmV6.S 
	


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../code\
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID -DARM  -DRVDS -DLINUX  -DVOARMV6 -DLICENSEFILE -DSTABILITY -DVOJPEGFLAGE

LOCAL_CFLAGS := -D_VOMODULEID=0x02410000   -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

