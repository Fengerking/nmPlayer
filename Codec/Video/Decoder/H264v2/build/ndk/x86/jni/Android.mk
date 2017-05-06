LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoH264Dec


CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../src
CSRC_ASM_PATH:=../../../../src/asm/x86

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
	$(CSRC_ASM_PATH)/voH264ChromaMC.asm \
	$(CSRC_ASM_PATH)/voH264Deblock.asm \
	$(CSRC_ASM_PATH)/voH264Fpel.asm \
	$(CSRC_ASM_PATH)/voH264IDCT.asm \
	$(CSRC_ASM_PATH)/voH264Inc.asm \
	$(CSRC_ASM_PATH)/voH264Qpel.asm \
	$(CSRC_ASM_PATH)/voH264Qpel8bit.asm \
	$(CSRC_ASM_PATH)/voH264Util.asm \
	$(CSRC_ASM_PATH)/voH264Weight.asm



LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../Inc\
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -DVO_NDK_X86 -DVOSSSE3 -D_LINUX_ANDROID -DSTABILITY -DLICENSEFILE -DVOANDROID=1
LOCAL_CFLAGS := -D_VOMODULEID=0x02010000  -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_YASMFLAGS := -DANDROID -DPIC -DARCH_X86_32 -DVO_NDK_X86 -DHIGH_BIT_DEPTH=0 -DBIT_DEPTH=8 -DARCH_X86_64=0 -DHAVE_CPUNOP=0 -DARCH_X86_32=1  -DHAVE_ALIGNED_STACK=1 -DHAVE_AVX_EXTERNAL=0
LOCAL_LDLIBS := ../../../../../../../../Lib/ndk/x86/libvoCheck.a -llog


include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

