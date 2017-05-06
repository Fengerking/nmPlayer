LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoH264Dec


CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../src

LOCAL_SRC_FILES := \
  $(CMNSRC_PATH)/voLog.c\
	$(CSRC_PATH)/biaridecod.c \
	$(CSRC_PATH)/block.c \
	$(CSRC_PATH)/cabac.c \
	$(CSRC_PATH)/context_ini.c \
	$(CSRC_PATH)/filehandle.c \
	$(CSRC_PATH)/fmo.c \
	$(CSRC_PATH)/header.c \
	$(CSRC_PATH)/image.c \
	$(CSRC_PATH)/intra16x16_pred.c \
	$(CSRC_PATH)/intra16x16_pred_normal.c \
	$(CSRC_PATH)/intra8x8_pred.c \
	$(CSRC_PATH)/intra8x8_pred_normal.c \
	$(CSRC_PATH)/intra4x4_pred.c \
	$(CSRC_PATH)/intra4x4_pred_normal.c \
	$(CSRC_PATH)/intra_chroma_pred.c \
	$(CSRC_PATH)/ldecod.c \
	$(CSRC_PATH)/loopFilter.c \
	$(CSRC_PATH)/loop_filter_normal.c \
	$(CSRC_PATH)/macroblock.c \
	$(CSRC_PATH)/mbuffer.c \
	$(CSRC_PATH)/mbuffer_mvc.c \
	$(CSRC_PATH)/mb_access.c \
	$(CSRC_PATH)/mb_prediction.c \
	$(CSRC_PATH)/mc_direct.c \
	$(CSRC_PATH)/mc_prediction.c \
	$(CSRC_PATH)/memalloc.c \
	$(CSRC_PATH)/mv_prediction.c \
	$(CSRC_PATH)/nal.c \
	$(CSRC_PATH)/nalu.c \
	$(CSRC_PATH)/nalucommon.c \
	$(CSRC_PATH)/output.c \
	$(CSRC_PATH)/parset.c \
	$(CSRC_PATH)/parsetcommon.c \
	$(CSRC_PATH)/quant.c \
	$(CSRC_PATH)/sei.c \
	$(CSRC_PATH)/transform.c \
	$(CSRC_PATH)/vlc.c \
	$(CSRC_PATH)/voH264Dec.c\
	$(CSRC_PATH)/frameThd.c \
	$(CSRC_PATH)/win32.c \
	$(CSRC_PATH)/asm/armv7/voH264Deblock_ARMV7_gnu.S \
	$(CSRC_PATH)/asm/armv7/voH264DecDCT_ARMV7_gnu.S \
	$(CSRC_PATH)/asm/armv7/voH264Mc_ARMV7_gnu.S \
	$(CSRC_PATH)/asm/armv7/voH264IntraPrediction_ARMV7_gnu.S \
	$(CSRC_PATH)/asm/armv7/voH264Mc_offset_ARMV7_gnu.S



LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../Inc\
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID  -DVOARMV7 -DSTABILITY -DLICENSEFILE -DARM -DVOANDROID=1 
LOCAL_CFLAGS := -D_VOMODULEID=0x02010000  -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS := ../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

