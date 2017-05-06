LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoQCELPEnc

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src/c_src
ASM_SRC6:=../../../../src/linux_asm/armv7

LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
        $(MOD_SRC)/cb.c \
	$(MOD_SRC)/cb_tabs.c \
	$(MOD_SRC)/coderate.c \
	$(MOD_SRC)/decode.c \
	$(MOD_SRC)/encode.c \
	$(MOD_SRC)/filter.c \
	$(MOD_SRC)/frontfil.c \
        $(MOD_SRC)/hammtab.c \
	$(MOD_SRC)/init.c \
	$(MOD_SRC)/lpc.c \
	$(MOD_SRC)/lsp.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/modetabs.c \
	$(MOD_SRC)/pack.c \
	$(MOD_SRC)/pitch.c \
        $(MOD_SRC)/quantabs.c \
	$(MOD_SRC)/quantize.c \
	$(MOD_SRC)/ratedec.c \
	$(MOD_SRC)/sincos.c \
	$(MOD_SRC)/snr.c \
	$(MOD_SRC)/stream.c \
	$(MOD_SRC)/target.c \
	$(MOD_SRC)/voQCELPEnc.c \
        $(ASM_SRC6)/autocorr_neon.S \
	$(ASM_SRC6)/comp_corr1_neon.S \
	$(ASM_SRC6)/comp_corr_asm.S \
	$(ASM_SRC6)/do_pole_filter.S \
	$(ASM_SRC6)/do_zero_filter_neon.S \
        $(ASM_SRC6)/filter_1_tap_neon.S \
	$(ASM_SRC6)/filter_response1.S \
	$(ASM_SRC6)/line_fir_filter.S \
	$(ASM_SRC6)/pole_filter.S \
	$(ASM_SRC6)/pole_filter_response.S \
        $(ASM_SRC6)/recursive_conv_neon.S \
	$(ASM_SRC6)/vo_comput_cb.S \
	$(ASM_SRC6)/zero_filter_neon.S



LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


VOMM:=-DLINUX -DARM -DARMV6 -DARMV7 -DASM_OPT -DARMv7_OPT -DLCHECK
LOCAL_LDLIBS:= -llog
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x03280000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS+=../../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

