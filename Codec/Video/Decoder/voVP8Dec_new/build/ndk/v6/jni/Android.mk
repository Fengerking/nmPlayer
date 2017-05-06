LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoVP8Dec


CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../source

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/voVP8Common.c \
	$(CSRC_PATH)/voVP8DecBoolCoder.c \
	$(CSRC_PATH)/voVP8DecFindNearMV.c \
	$(CSRC_PATH)/voVP8DecFrame.c \
	$(CSRC_PATH)/voVP8DecFront.c \
	$(CSRC_PATH)/voVP8DecIdctDq.c \
	$(CSRC_PATH)/voVP8DecInterPolationFilter.c \
	$(CSRC_PATH)/voVP8DecMBlock.c \
	$(CSRC_PATH)/voVP8DecMC.c \
	$(CSRC_PATH)/voVP8DecMV.c \
	$(CSRC_PATH)/voVP8DecNormalFilters.c \
	$(CSRC_PATH)/voVP8DecNormalLoopFilter.c \
	$(CSRC_PATH)/voVP8Decoder.c \
	$(CSRC_PATH)/voVP8DecReadBits.c \
	$(CSRC_PATH)/voVP8DecReadDctTokens.c \
	$(CSRC_PATH)/voVP8DecReadMode.c \
	$(CSRC_PATH)/voVP8DecReconInter.c \
	$(CSRC_PATH)/voVP8DecReconIntra.c \
	$(CSRC_PATH)/voVP8DecSimpleLoopFilters.c \
	$(CSRC_PATH)/voVP8Memory.c \
	$(CSRC_PATH)/voVP8Thread.c \


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../Inc\
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID -DVOANDROID -DCONFIG_MULTITHREAD -DFILTER_LEVEL -DLICENSEFILE -DSTABILITY -DMULTITHREAD_STABILITY

LOCAL_CFLAGS := -D_VOMODULEID=0x020b0000   -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp
LOCAL_LDLIBS := ../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

