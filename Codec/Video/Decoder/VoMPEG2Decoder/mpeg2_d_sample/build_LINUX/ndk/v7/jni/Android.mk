LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoMPEG2Dec


CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../src
CSRARMV7_PATH:=../../../../../src/asm/arm_gnu/armv7

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/idct_add.c \
	$(CSRC_PATH)/voMpeg2Decoder.c \
	$(CSRC_PATH)/voMpeg2Front.c \
	$(CSRC_PATH)/voMpeg2MB.c \
	$(CSRC_PATH)/voMpeg2Parser.c \
	$(CSRC_PATH)/voMpegBuf.c \
	$(CSRC_PATH)/voMpegMC.c \
	$(CSRC_PATH)/voMpegMem.c \
	$(CSRC_PATH)/voMpegIdct.c \
	$(CSRC_PATH)/voMpeg2Idct.c \
	$(CSRARMV7_PATH)/idct_arm_V7_ABCDEF.S \
	$(CSRARMV7_PATH)/mpeg4_MC_arm_V7.S \
	$(CSRARMV7_PATH)/mpeg4_MC_arm_V7_4x8.S \
	$(CSRARMV7_PATH)/irdeto_idct_arm_V7_ABCDEF.S\
	$(CSRARMV7_PATH)/irdeto_sat_add.S
	

LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:=-DARM -DVOARMv7 -DRVDS -DLINUX -DLICENSEFILE 

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x02090000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS:=../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

