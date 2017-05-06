LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyHEVCDecLib

CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../src


LOCAL_SRC_FILES := \
  $(CMNSRC_PATH)/voLog.c\
  $(CSRC_PATH)/h265dec_cabac.c\
	$(CSRC_PATH)/h265dec_deblock.c\
	$(CSRC_PATH)/h265dec_debug.c\
	$(CSRC_PATH)/h265dec_frameThd.c\
	$(CSRC_PATH)/h265dec_front.c\
	$(CSRC_PATH)/h265dec_idct.c\
	$(CSRC_PATH)/h265dec_inter.c\
	$(CSRC_PATH)/h265dec_list.c\
	$(CSRC_PATH)/h265dec_mem.c \
	$(CSRC_PATH)/h265dec_mvpred.c\
	$(CSRC_PATH)/h265_decoder.c\
	$(CSRC_PATH)/h265dec_parser.c\
	$(CSRC_PATH)/h265dec_recon.c\
	$(CSRC_PATH)/h265dec_sao.c\
	$(CSRC_PATH)/h265dec_sbac.c\
	$(CSRC_PATH)/h265dec_slice.c\
	$(CSRC_PATH)/h265dec_vlc.c\
	$(CSRC_PATH)/h265dec_wave3d.c\
	$(CSRC_PATH)/h265dec_dequant.c\
	$(CSRC_PATH)/HMLog.c\
	$(CSRC_PATH)/asm/armv7/gnu/h265_Deblock_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/h265_sao_GNU.S \
	$(CSRC_PATH)/asm/armv7/gnu/new_luma_mc_neon_gnu.S \
	$(CSRC_PATH)/asm/armv7/gnu/H265_IntraPred_Angular_GNU.S \
	$(CSRC_PATH)/asm/armv7/gnu/IDCT_NZ_1X1_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDCT16X16_4X4_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDCT32X32_4X4_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDST4X4_1X1_GNU.S \
	$(CSRC_PATH)/asm/armv7/gnu/IDCT4X4_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDCT8X8_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDCT8X8_4X4_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDCT16X16_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDCT16X16_8X8_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDCT32X32_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDCT32X32_8X8_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDCT32X32_16X16_GNU.S\
	$(CSRC_PATH)/asm/armv7/gnu/IDST4X4_GNU.S
	  



LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)


VOMM:= -DHAVE_NEON=1 -DVOARMV7 -DVOANDROID -DANDROID -DASM_OPT  -DNDEBUG -D_VOLOG_ERROR -D_VONDBG

LOCAL_CFLAGS := -D_VOMODULEID=0x020e0000 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -mtpcs-frame -mthumb -g

include $(VOTOP)/build/vondk.mk
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyHEVCDec


LOCAL_STATIC_LIBRARIES := libyyHEVCDecLib
include $(BUILD_SHARED_LIBRARY)

