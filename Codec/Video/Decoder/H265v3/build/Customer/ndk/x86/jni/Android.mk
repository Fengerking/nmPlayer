LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoH265Dec

CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../src
CSRC_ASM_PATH:=../../../../../src/asm/x86


LOCAL_SRC_FILES := \
        $(CMNSRC_PATH)/voLog.c\
        $(CSRC_PATH)/h265dec_cabac.c\
	$(CSRC_PATH)/h265dec_deblock.c\
	$(CSRC_PATH)/h265dec_debug.c\
	$(CSRC_PATH)/h265dec_wave3d.c\
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
	$(CSRC_PATH)/h265dec_dequant.c\
	$(CSRC_PATH)/x86const.c\
	$(CSRC_ASM_PATH)/x86util.asm\
	$(CSRC_ASM_PATH)/x86inc.asm\
	$(CSRC_ASM_PATH)/h265dec_ASM_config.asm\
    $(CSRC_ASM_PATH)/new_luma_mc_neon.asm\
    $(CSRC_ASM_PATH)/h265dec_IDCT_x86.asm\
    $(CSRC_ASM_PATH)/h265_Deblock_x86.asm\
	$(CSRC_ASM_PATH)/h265_sao_x86.asm\
	$(CSRC_ASM_PATH)/h265dec_Intra_x86.asm\
	$(CSRC_PATH)/HMLog.c


LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)

			  
VOMM:= -DVOANDROID -DANDROID  -DNDEBUG -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID -DENABLE_LICENSE_CHECK -DARCH_X86 -DASM_OPT 

LOCAL_CFLAGS := -D_VOMODULEID=0x020e0000  -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 
LOCAL_YASMFLAGS := -DANDROID -DPIC -DARCH_X86_32 -DVO_NDK_X86 -DHIGH_BIT_DEPTH=0 -DBIT_DEPTH=8 -DARCH_X86_64=0 -DHAVE_CPUNOP=0 -DARCH_X86_32=1 -DHAVE_ALIGNED_STACK=1 -DHAVE_AVX_EXTERNAL=0
LOCAL_LDLIBS := -llog ../../../../../../../../../Lib/ndk/x86/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

