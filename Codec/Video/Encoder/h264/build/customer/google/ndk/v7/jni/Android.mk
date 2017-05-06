LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoH264Enc


CMNSRC_PATH:=../../../../../../../../../../Common
CSRC_PATH:=../../../../../../src

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/voH264Dct.c \
	$(CSRC_PATH)/voH264Deblock.c \
	$(CSRC_PATH)/voH264EncBitStream.c \
	$(CSRC_PATH)/voH264EncCAVLC.c \
	$(CSRC_PATH)/voH264EncMB.c \
	$(CSRC_PATH)/voH264EncME.c \
	$(CSRC_PATH)/voH264EncPicSlice.c \
	$(CSRC_PATH)/voH264EncRC.c \
	$(CSRC_PATH)/voH264EncSAD.c \
	$(CSRC_PATH)/voH264IntraPrediction.c \
	$(CSRC_PATH)/voH264MC.c \
	$(CSRC_PATH)/voH264Quant.c \
	$(CSRC_PATH)/voH264Frame.c \
	$(CSRC_PATH)/asm/armv7/voH264Dct_ARMV7_gnu.S \
	$(CSRC_PATH)/asm/armv7/voH264EncSAD_ARMV7_gnu.S \
	$(CSRC_PATH)/asm/armv7/voH264IntraPrediction_ARMV7_gnu.S \
	$(CSRC_PATH)/asm/armv7/voH264Quant_ARMV7_gnu.S \
	$(CSRC_PATH)/asm/armv7/voH264Mc_ARMV7_gnu.S\
	$(CSRC_PATH)/asm/armv7/voH264HPel_ARMV7_gnu.S\
	$(CSRC_PATH)/asm/armv7/voH264Deblock_ARMV7_gnu.S\
	$(CSRC_PATH)/asm/armv7/voH264Pic_ARMV7_gnu.S

LOCAL_C_INCLUDES := \
	../../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DHAVE_NEON=1 -DNEW_SDK  -DRVDS -DVOARMV7 -DLICENSE_CHECK

LOCAL_CFLAGS := -D_VOMODULEID=0x03010000  -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS := ../../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)


