LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoAMRWBDec

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src/c_src
ASM_SRC6:=../../../../src/linux_asm/armv7
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnVOMemory.cpp \
        $(MOD_SRC)/agc2_fx.c \
       	$(MOD_SRC)/d2t64_fx.c \
       	$(MOD_SRC)/d4t64_fx.c \
	$(MOD_SRC)/d_gain2_fx.c \
	$(MOD_SRC)/dec_if_fx.c \
	$(MOD_SRC)/dec_main_fx.c \
	$(MOD_SRC)/decim54_fx.c \
        $(MOD_SRC)/deemph_fx.c \
	$(MOD_SRC)/dtx_fx.c \
	$(MOD_SRC)/gpclip_fx.c \
	$(MOD_SRC)/hf_func_fx.c \
	$(MOD_SRC)/homing_fx.c \
	$(MOD_SRC)/hp400_fx.c \
	$(MOD_SRC)/hp50_fx.c \
	$(MOD_SRC)/hp6k_fx.c \
        $(MOD_SRC)/hp7k_fx.c \
	$(MOD_SRC)/hp_wsp_fx.c \
	$(MOD_SRC)/if_rom_fx.c \
	$(MOD_SRC)/int_lpc_fx.c \
	$(MOD_SRC)/int_lpc_p_fx.c \
	$(MOD_SRC)/isfextrp_fx.c \
	$(MOD_SRC)/isp_az_fx.c \
	$(MOD_SRC)/isp_isf_fx.c \
        $(MOD_SRC)/lagconc_fx.c \
	$(MOD_SRC)/log2.c \
	$(MOD_SRC)/math_op.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/oper_32b.c \
	$(MOD_SRC)/overs12k8_fx.c \
	$(MOD_SRC)/p_med_ol_fx.c \
	$(MOD_SRC)/ph_disp_fx.c \
        $(MOD_SRC)/pit_shrp_fx.c \
	$(MOD_SRC)/pred_lt4_fx.c \
	$(MOD_SRC)/preemph_fx.c \
	$(MOD_SRC)/q_pulse_fx.c \
	$(MOD_SRC)/qisf_ns_fx.c \
	$(MOD_SRC)/qpisf_2s_fx.c \
	$(MOD_SRC)/random_fx.c \
        $(MOD_SRC)/residu_fx.c \
	$(MOD_SRC)/scale_fx.c \
	$(MOD_SRC)/stream.c \
        $(MOD_SRC)/syn_filt_fx.c \
	$(MOD_SRC)/tables_plus_fx.c \
	$(MOD_SRC)/util_fx.c \
	$(MOD_SRC)/voAMRWBDec.cpp \
        $(MOD_SRC)/voicefac_fx.c \
	$(MOD_SRC)/weight_a_fx.c \
        $(ASM_SRC6)/Interpol_neon.S \
	$(ASM_SRC6)/Dot_p_neon.S \
	$(ASM_SRC6)/Syn_filt_neon.S \
	$(ASM_SRC6)/Syn_filt_32_neon.S \
	$(ASM_SRC6)/Scale_sig_neon.S \
        $(ASM_SRC6)/pret_lt4_neon.S \
	$(ASM_SRC6)/Filt_7k_neon.S \
	$(ASM_SRC6)/Filt_6k_7k_neon.S


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


VOMM:=-DLINUX -DARM -DARMV6 -DARMV7 -DASM_OPT -DLCHECK
LOCAL_LDLIBS:= -llog
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02260000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS+=../../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

