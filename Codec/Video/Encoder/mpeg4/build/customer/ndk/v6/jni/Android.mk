LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoMPEG4Enc


CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../src

LOCAL_SRC_FILES := \
		\
		$(CSRC_PATH)/common/src/voMpegMC.c\
	  $(CSRC_PATH)/common/src/voMpegIdct.c\
	  $(CSRC_PATH)/lowlevel/voMpegEncFastDct.c\
	  $(CSRC_PATH)/lowlevel/voMpegEncHPInterPolate.c\
	  $(CSRC_PATH)/lowlevel/voMpegEncPreProcess.c\
	  $(CSRC_PATH)/lowlevel/voMpegEncQuant.c\
	  $(CSRC_PATH)/lowlevel/voMpegEncSad.c\
	  $(CSRC_PATH)/lowlevel/voMpegEncTable.c\
	  $(CSRC_PATH)/midlevel/h263/voH263EncHuffman.c\
	  $(CSRC_PATH)/midlevel/motionestimation/voMpegEncME.c\
	  $(CSRC_PATH)/midlevel/motionestimation/voMpegEncMEMethod.c\
	  $(CSRC_PATH)/midlevel/ratecontrol/voMpegEncRC.c\
	  $(CSRC_PATH)/midlevel/voMpegEncBits.c\
	  $(CSRC_PATH)/midlevel/voMpegEncFrame.c\
	  $(CSRC_PATH)/midlevel/voMpegEncFrameProc.c\
	  $(CSRC_PATH)/midlevel/voMpegEncMB.c\
	  $(CSRC_PATH)/midlevel/voMpegEncPrediction.c\
	  $(CSRC_PATH)/midlevel/voMpegMemmory.c\
	  $(CSRC_PATH)/voMpegEncFront.c\
	  $(CSRC_PATH)/common/src/arm_asm/voIdctArmV6_gnu.S\
	  $(CSRC_PATH)/common/src/arm_asm/voMpegMCArmV4V6_gnu.S\
	  $(CSRC_PATH)/lowlevel/arm_asm/voMpegEncQuant_gnu.S\
	  $(CSRC_PATH)/lowlevel/arm_asm/voMpegFdctArmV6_gnu.S\
		$(CSRC_PATH)/lowlevel/arm_asm/voMpegEncHalfPixelArmV4_gnu.S\
		$(CSRC_PATH)/lowlevel/arm_asm/voMpegEncHalfPixelArmV6_gnu.S\
		$(CSRC_PATH)/lowlevel/arm_asm/voMpegSadArmV6_gnu.S


LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	../../../../Inc\
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID -DVOARCHGEN -DVOARCH32 -DNEW_SDK -DLINUX -DVOARMV6 -DLICENSE_CHECK

LOCAL_CFLAGS := -D_VOMODULEID=0x03020000  $(VOHIDESYM) -fPIC  -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp
LOCAL_LDLIBS := ../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

