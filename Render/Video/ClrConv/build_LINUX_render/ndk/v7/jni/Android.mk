LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyClrCvtLib


CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../src
CSRARMV7_PATH:=../../../../src/gun/armv7

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/ccConstant.c \
	$(CSRC_PATH)/ccMain.c \
	$(CSRC_PATH)/ccRGBToRGB.c \
	$(CSRC_PATH)/ccRGBToRGBMB.c \
	$(CSRC_PATH)/ccYUVToRGB.c \
	$(CSRC_PATH)/ccYUVToRGBMB.c \
	$(CSRC_PATH)/ccYUVToYUV.c \
	$(CSRARMV7_PATH)/voRGB888toRGB16_8x2n_armv7.S \
	$(CSRARMV7_PATH)/voYUV2YUV_8x2n_armv7.S \
	$(CSRARMV7_PATH)/voYUV2YUV_8x2n_armv7L90.S \
	$(CSRARMV7_PATH)/voYUV2YUV_8x2n_armv7R90.S \
	$(CSRARMV7_PATH)/voYUV2YUV_8x2n_armv7R180.S \
	$(CSRARMV7_PATH)/voYUV2YUV_8x8_armv7R90L90R180.S \
	$(CSRARMV7_PATH)/voYUV420toRGB16_8nx2n_armv7.S \
	$(CSRARMV7_PATH)/voYUV420toRGB_24_v7.S \
	$(CSRARMV7_PATH)/yuv2yuv_arm.S \
	$(CSRARMV7_PATH)/YUV420_YUV422.S \
	$(CSRARMV7_PATH)/voYUV420toRGB32_8nx2n_armv7.S

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:=-DVOARMV7  -DLINUX -DARCH_IS_GENERIC -DARCH_IS_32BIT -DARCH_IS_LITTLE_ENDIAN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x05030000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyClrCvt


LOCAL_STATIC_LIBRARIES := libyyClrCvtLib


include $(BUILD_SHARED_LIBRARY)
