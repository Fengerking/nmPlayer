LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoRealVideoDec


CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../src
CSRARMV6_PATH:=../../../../../src/asm/gnu_asm/armv6

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/ai.c \
	$(CSRC_PATH)/basic.c \
	$(CSRC_PATH)/bsx.c \
	$(CSRC_PATH)/cinterp4.c \
	$(CSRC_PATH)/cinterp.c \
	$(CSRC_PATH)/deblock8.c \
	$(CSRC_PATH)/deblock.c \
	$(CSRC_PATH)/dec4x4md.c \
	$(CSRC_PATH)/decoder.c \
	$(CSRC_PATH)/drv.c \
	$(CSRC_PATH)/frame.c \
	$(CSRC_PATH)/idct.c \
	$(CSRC_PATH)/parsebs.c \
	$(CSRC_PATH)/recon.c \
	$(CSRC_PATH)/rv_decode.c \
	$(CSRC_PATH)/tables.c \
	$(CSRC_PATH)/voRVDecSDK.c \
	$(CSRC_PATH)/voRVThread.c \
	$(CSRARMV6_PATH)/ARMV6_rv8_cinterp.S \
	$(CSRARMV6_PATH)/IDCT_armv6.S \
	$(CSRARMV6_PATH)/deblock_armv6.S \
	$(CSRARMV6_PATH)/interp4_armv6.S \
	$(CSRARMV6_PATH)/interp4Part2_armv6.S \
	$(CSRARMV6_PATH)/interp4Part3_armv6.S \
	$(CSRARMV6_PATH)/mem_copy_armv6.S \
	$(CSRARMV6_PATH)/intraMB_pred_armv6.S 

LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:=-DARM -DARMV6 -DVOARMV6 -DVOANDROID -DLINUX -DLICENSEFILE 

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x02050000  -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp  -fsigned-char -msoft-float

LOCAL_LDLIBS:=../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

