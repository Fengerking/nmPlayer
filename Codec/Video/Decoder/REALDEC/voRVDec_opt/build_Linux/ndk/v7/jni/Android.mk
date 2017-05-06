LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoRealVideoDec


CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../src
CSRARMV7_PATH:=../../../../../src/asm/gnu_asm/armv7

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
	$(CSRARMV7_PATH)/ARMV7_rv8_edge_filter.S \
	$(CSRARMV7_PATH)/cinterp_rv8.S \
	$(CSRARMV7_PATH)/deblock_CtxA8.S \
	$(CSRARMV7_PATH)/IDCT_CtxA8.S \
	$(CSRARMV7_PATH)/interpolate4_Chroma_CtxA8.S \
	$(CSRARMV7_PATH)/interpolate4_Luma_CtxA8.S \
	$(CSRARMV7_PATH)/intraMB_Pred_CtxA8.S \
	$(CSRARMV7_PATH)/memcopy_CtxA8.S \
	$(CSRARMV7_PATH)/UVPackMB.S \
	$(CSRARMV7_PATH)/fillEdge_CtxA8.S 

LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:=-DARM -DARMV7 -DVOARMV7 -DVOANDROID -DLINUX -DLICENSEFILE

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x02050000  -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS:=../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

