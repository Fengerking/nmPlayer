LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

ifeq ($(TYPE), Dec)
LOCAL_MODULE:= libvoEVRCDec
else
LOCAL_MODULE:= libvoEVRCEnc
endif

CMN_SRC:=../../../../../../../Common
MOD_SRC:=../../../../src/c_src
ASM_SRC6:=../../../../src/linux_asm

LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
        $(MOD_SRC)/a2lsp.c \
	$(MOD_SRC)/acb_ex.c \
	$(MOD_SRC)/acelp_pf.c \
	$(MOD_SRC)/apf.c \
	$(MOD_SRC)/auto.c \
	$(MOD_SRC)/bitpack.c \
	$(MOD_SRC)/bitupack.c \
	$(MOD_SRC)/bl_intrp.c \
        $(MOD_SRC)/bqiir.c \
	$(MOD_SRC)/c3_10pf.c \
	$(MOD_SRC)/c8_35pf.c \
	$(MOD_SRC)/comacb.c \
	$(MOD_SRC)/convh.c \
	$(MOD_SRC)/cshift.c \
	$(MOD_SRC)/d3_10pf.c \
	$(MOD_SRC)/d8_35pf.c \
        $(MOD_SRC)/d_fer.c \
	$(MOD_SRC)/d_no_fer.c \
	$(MOD_SRC)/d_rate_1.c \
	$(MOD_SRC)/decode.c \
	$(MOD_SRC)/durbin.c \
	$(MOD_SRC)/encode.c \
	$(MOD_SRC)/fcbgq.c \
	$(MOD_SRC)/fer.c \
	$(MOD_SRC)/filter.c \
        $(MOD_SRC)/fndppf.c \
	$(MOD_SRC)/getext1k.c \
	$(MOD_SRC)/getgain.c \
	$(MOD_SRC)/getres.c \
	$(MOD_SRC)/impulser.c \
	$(MOD_SRC)/interpol.c \
	$(MOD_SRC)/intr_cos.c \
	$(MOD_SRC)/inv_sqrt.c \
        $(MOD_SRC)/lpcana.c \
	$(MOD_SRC)/lsp2a.c \
	$(MOD_SRC)/lspmaq.c \
	$(MOD_SRC)/math_adv.c \
	$(MOD_SRC)/math_ext32.c \
	$(MOD_SRC)/maxeloc.c \
	$(MOD_SRC)/mdfyorig.c \
	$(MOD_SRC)/mem_align.c \
        $(MOD_SRC)/mod.c \
	$(MOD_SRC)/ns127.c \
	$(MOD_SRC)/pit_shrp.c \
	$(MOD_SRC)/pktoav.c \
	$(MOD_SRC)/pre_enc.c \
	$(MOD_SRC)/putacbc.c \
	$(MOD_SRC)/r_fft.c \
	$(MOD_SRC)/rda.c \
	$(MOD_SRC)/rom.c \
	$(MOD_SRC)/stream.c \
        $(MOD_SRC)/synfltr.c \
	$(MOD_SRC)/w2res.c \
	$(MOD_SRC)/weight.c \
	$(MOD_SRC)/zeroinpt.c \
        $(ASM_SRC6)/ConIR.S \
	$(ASM_SRC6)/GetResidual.S \
	$(ASM_SRC6)/Impulse_asm.S \
	$(ASM_SRC6)/bl_intrp_asm.S \
	$(ASM_SRC6)/cor_h_vec.S \
	$(ASM_SRC6)/iir_asm.S \
        $(ASM_SRC6)/cor_h_x_v6.S \
	$(ASM_SRC6)/fir_asm.S \

ifeq ($(TYPE), Dec)
LOCAL_SRC_FILES += $(MOD_SRC)/voEVRCDec.c
else
LOCAL_SRC_FILES += $(MOD_SRC)/voEVRCEnc.c
endif


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../Common \
	../../../../inc \


VOMM:=-DLINUX -DARM -DARMV6 -DASM_OPT -DLCHECK
LOCAL_LDLIBS:= -llog
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02290000   -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char

LOCAL_LDLIBS+=../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

