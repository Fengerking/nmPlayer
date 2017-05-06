LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoMMCCRRS


CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../src
CSRARMV6_PATH:=../../../../src/gun/armv6

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/ccConstant.c \
	$(CSRC_PATH)/ccMain.c \
	$(CSRC_PATH)/ccRGBToRGB.c \
	$(CSRC_PATH)/ccRGBToRGBMB.c \
	$(CSRC_PATH)/ccYUVToRGB.c \
	$(CSRC_PATH)/ccYUVToRGBMB.c \
	$(CSRC_PATH)/ccYUVToYUV.c \
	$(CSRARMV6_PATH)/voYUV420toRGB_16_v6.S \
	$(CSRARMV6_PATH)/no_resize_arm3.S \
	$(CSRARMV6_PATH)/no_resize_arm4.S \
	$(CSRARMV6_PATH)/no_resize_arm_180.S \
	$(CSRARMV6_PATH)/yuv2rgb16_mb_arm2.S \
  $(CSRARMV6_PATH)/yuv2rgb16_mb_arm3.S \
  $(CSRARMV6_PATH)/yuv2rgb16_mb_arm4.S \
  $(CSRARMV6_PATH)/yuv2rgb16_mb_arm_180.S \
  $(CSRARMV6_PATH)/yuv2yuv_arm.S \
  $(CSRARMV6_PATH)/voYUV420toRGB_24_v6.S \
  $(CSRARMV6_PATH)/yuv2argb32_mb_arm_180.S \
  $(CSRARMV6_PATH)/yuv2argb32_mb_arm2.S \
  $(CSRARMV6_PATH)/yuv2argb32_mb_arm3.S \
  $(CSRARMV6_PATH)/yuv2argb32_mb_arm4.S \
  $(CSRARMV6_PATH)/yuv2argb32_no_resize_arm_180.S \
  $(CSRARMV6_PATH)/yuv2argb32_no_resize_arm2.S \
  $(CSRARMV6_PATH)/yuv2argb32_no_resize_arm3.S \
  $(CSRARMV6_PATH)/yuv2argb32_no_resize_arm4.S \
  $(CSRARMV6_PATH)/yuv2rgb32_mb_arm_180.S \
  $(CSRARMV6_PATH)/yuv2rgb32_mb_arm2.S \
  $(CSRARMV6_PATH)/yuv2rgb32_mb_arm3.S \
  $(CSRARMV6_PATH)/yuv2rgb32_mb_arm4.S \
  $(CSRARMV6_PATH)/yuv2rgb32_no_resize_arm_180.S \
  $(CSRARMV6_PATH)/yuv2rgb32_no_resize_arm2.S \
  $(CSRARMV6_PATH)/yuv2rgb32_no_resize_arm3.S \
  $(CSRARMV6_PATH)/yuv2rgb32_no_resize_arm4.S

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:=-DVOARMV6  -DLINUX -DARCH_IS_GENERIC -DARCH_IS_32BIT -DARCH_IS_LITTLE_ENDIAN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x05030000   -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char -msoft-float

LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

