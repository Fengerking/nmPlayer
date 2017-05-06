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


LOCAL_C_INCLUDES := \
	../../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DHAVE_NEON=1 -DNEW_SDK  -DRVDS -DVOARMV6 -DLICENSE_CHECK

LOCAL_CFLAGS := -D_VOMODULEID=0x03010000   -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp
LOCAL_LDLIBS := ../../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)


