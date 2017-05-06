LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoMPEG4Dec


CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../src


LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/front/voMpeg4DecFront.c \
	$(CSRC_PATH)/voDivx3Haff.c \
	$(CSRC_PATH)/voDivx3Parser.c \
	$(CSRC_PATH)/voMpeg4ACDC.c \
	$(CSRC_PATH)/voMpeg4Frame.c \
	$(CSRC_PATH)/voMpeg4Haff.c \
	$(CSRC_PATH)/voMpeg4MCQpel.c \
	$(CSRC_PATH)/voMpeg4Postprocess.c \
	$(CSRC_PATH)/voMpegBuf.c \
	$(CSRC_PATH)/voMpegMC.c \
	$(CSRC_PATH)/voMpegReadbits.c \
	$(CSRC_PATH)/voDivx3MB.c \
	$(CSRC_PATH)/voDivx3Tab.c \
	$(CSRC_PATH)/voMpeg4Decoder.c \
	$(CSRC_PATH)/voMpeg4Gmc.c \
	$(CSRC_PATH)/voMpeg4MB.c \
	$(CSRC_PATH)/voMpeg4Parser.c \
	$(CSRC_PATH)/voMpeg4Rvlc.c \
	$(CSRC_PATH)/voMpegIdct.c \
	$(CSRC_PATH)/voMpegMem.c \
	$(CSRC_PATH)/asm/arm/voMpegFillEdgesArmV6_gnu.S \
	$(CSRC_PATH)/asm/arm/voMpeg4MCQpelArmV6_gnu.S \
	$(CSRC_PATH)/asm/arm/voMpeg4MCQpelAddArmV6_gnu.S \
	$(CSRC_PATH)/asm/arm/ARMV7/voIdctArmV7_gnu.S \
	$(CSRC_PATH)/asm/arm/ARMV7/voMpegMCArmV7_gnu.S 


LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -DHAVE_NEON=1 -DNEW_SDK -DMPEG4 -DRVDS -DVOARMV7 -DVOANDROID 

LOCAL_CFLAGS := -D_VOMODULEID=0x02020000  -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS := -llog ../../../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

