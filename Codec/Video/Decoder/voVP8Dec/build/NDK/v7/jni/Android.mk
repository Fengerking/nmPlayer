LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoVP8Dec


CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../source

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/voVP8Common.c \
	$(CSRC_PATH)/voVP8DecBoolCoder.c \
	$(CSRC_PATH)/voVP8DecFindNearMV.c \
	$(CSRC_PATH)/voVP8DecFrame.c \
	$(CSRC_PATH)/voVP8DecFront.c \
	$(CSRC_PATH)/voVP8DecIdctDq.c \
	$(CSRC_PATH)/voVP8DecInterPolationFilter.c \
	$(CSRC_PATH)/voVP8DecMBlock.c \
	$(CSRC_PATH)/voVP8DecMC.c \
	$(CSRC_PATH)/voVP8DecMV.c \
	$(CSRC_PATH)/voVP8DecNormalFilters.c \
	$(CSRC_PATH)/voVP8DecNormalLoopFilter.c \
	$(CSRC_PATH)/voVP8Decoder.c \
	$(CSRC_PATH)/voVP8DecReadBits.c \
	$(CSRC_PATH)/voVP8DecReadDctTokens.c \
	$(CSRC_PATH)/voVP8DecReadMode.c \
	$(CSRC_PATH)/voVP8DecReconInter.c \
	$(CSRC_PATH)/voVP8DecReconIntra.c \
	$(CSRC_PATH)/voVP8DecSimpleLoopFilters.c \
	$(CSRC_PATH)/voVP8Memory.c \
	$(CSRC_PATH)/voVP8Thread.c \
	$(CSRC_PATH)/asm_gnu/armv7/idct/dc_only_idct_add_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/dequant_dc_idct_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/dequant_idct_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/dequantizeb_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/idct_dequant_0_2x_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/idct_dequant_dc_0_2x_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/idct_dequant_dc_full_2x_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/idct_dequant_full_2x_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/iwalsh_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/shortidct4x4llm_1_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/idct/shortidct4x4llm_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/bilinearpredict16x16_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/bilinearpredict4x4_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/bilinearpredict8x4_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/bilinearpredict8x8_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/buildintrapredictorsmby_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/copymem16x16_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/copymem8x4_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/copymem8x8_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/sixtappredict16x16_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/sixtappredict4x4_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/sixtappredict8x4_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/sixtappredict8x8_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/interpolation/vp8_predict_intra4x4.S\
	$(CSRC_PATH)/asm_gnu/armv7/lpfilter/loopfilter_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/lpfilter/loopfiltersimplehorizontaledge_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/lpfilter/loopfiltersimpleverticaledge_neon.S\
	$(CSRC_PATH)/asm_gnu/armv7/lpfilter/mbloopfilter_neon.S\




LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../Inc\
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID -DVOANDROID -DCONFIG_MULTITHREAD -DFILTER_LEVEL  -DVOARMV7 -DLICENSEFILE -DSTABILITY -DMULTITHREAD_STABILITY

LOCAL_CFLAGS := -D_VOMODULEID=0x020b0000  -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS := ../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

