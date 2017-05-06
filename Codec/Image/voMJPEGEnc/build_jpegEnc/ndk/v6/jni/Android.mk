LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoJPEGEnc


CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../src

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/fdct.c \
	$(CSRC_PATH)/jcommon.c \
	$(CSRC_PATH)/jdcttrans.c \
	$(CSRC_PATH)/jencode.c \
	$(CSRC_PATH)/jexif.c \
	$(CSRC_PATH)/jhuffum.c \
	$(CSRC_PATH)/jmark.c \
	$(CSRC_PATH)/jResize.c \
	$(CSRC_PATH)/jsrcdata.c \
	$(CSRC_PATH)/jtab.c \
	$(CSRC_PATH)/mem_align.c \
	$(CSRC_PATH)/vo_jpegEnc_api.c \
	$(CSRC_PATH)/gun/get420_data.S \
	$(CSRC_PATH)/gun/fdct_armv6.S


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../inc\
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID -DARM  -DRVDS -DLINUX  -DVOARMV6  -DSTABILITY

LOCAL_CFLAGS := -D_VOMODULEID=0x03410000  -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

