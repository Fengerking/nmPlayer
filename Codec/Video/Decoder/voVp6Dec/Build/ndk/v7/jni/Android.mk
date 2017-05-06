LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoVP6Dec


CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../Src

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/com_src/build.c \
	$(CSRC_PATH)/com_src/com_globals.c \
	$(CSRC_PATH)/com_src/context.c \
	$(CSRC_PATH)/com_src/crecon.c \
	$(CSRC_PATH)/com_src/dquantize.c \
	$(CSRC_PATH)/com_src/entropysetup.c \
	$(CSRC_PATH)/com_src/gconst.c \
	$(CSRC_PATH)/com_src/huffman.c \
	$(CSRC_PATH)/com_src/loopfilter.c \
	$(CSRC_PATH)/com_src/ModeMv.c \
	$(CSRC_PATH)/com_src/TokenEntropy.c \
	$(CSRC_PATH)/dec_src/boolhuff.c \
	$(CSRC_PATH)/dec_src/decodembs.c \
	$(CSRC_PATH)/dec_src/decodemode.c \
	$(CSRC_PATH)/dec_src/decodemv.c \
	$(CSRC_PATH)/dec_src/DFrameR.c \
	$(CSRC_PATH)/dec_src/DSystemDependant.c \
	$(CSRC_PATH)/dec_src/idct.c \
	$(CSRC_PATH)/dec_src/pb_globals.c \
	$(CSRC_PATH)/dec_src/readtokens_arith.c \
	$(CSRC_PATH)/dec_src/readtokens_data.c \
	$(CSRC_PATH)/dec_src/readtokens_huff.c \
	$(CSRC_PATH)/dec_src/reconmb.c \
	$(CSRC_PATH)/dec_src/vfwpbdll_if.c \
	$(CSRC_PATH)/dec_src/voVideoFrameBufManager.c \
	$(CSRC_PATH)/voVP6DecFront.c \
	$(CSRC_PATH)/plugin_src/on2_mem.c \
	$(CSRC_PATH)/plugin_src/yv12config.c \
	$(CSRC_PATH)/plugin_src/yv12extend.c \
	$(CSRC_PATH)/armasm/ARMV7/gnu/voFilterVP6ArmV7.S \
	$(CSRC_PATH)/armasm/ARMV7/gnu/voIdctVP6ArmV7.S \
	$(CSRC_PATH)/armasm/ARMV7/gnu/voMcVP6ArmV7.S \



LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../Inc\
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID -DRVDS -DVOARMV7 -DSTABILITY -DLICENSEFILE

LOCAL_CFLAGS := -D_VOMODULEID=0x020a0000  -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS := ../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

