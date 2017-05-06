LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoAMRWBEnc

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src/c_src
ASM_SRC6:=../../../../src/linux_asm/ARMv6
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
        $(MOD_SRC)/autocorr.c \
	$(MOD_SRC)/az_isp.c \
	$(MOD_SRC)/bits.c \
	$(MOD_SRC)/c2t64fx.c \
	$(MOD_SRC)/c4t64fx.c \
	$(MOD_SRC)/convolve.c \
	$(MOD_SRC)/cor_h_x.c \
	$(MOD_SRC)/decim54.c \
        $(MOD_SRC)/deemph.c \
	$(MOD_SRC)/dtx.c \
	$(MOD_SRC)/g_pitch.c \
	$(MOD_SRC)/gpclip.c \
	$(MOD_SRC)/homing.c \
	$(MOD_SRC)/hp400.c \
	$(MOD_SRC)/hp50.c \
	$(MOD_SRC)/hp6k.c \
	$(MOD_SRC)/hp_wsp.c \
        $(MOD_SRC)/int_lpc.c \
	$(MOD_SRC)/isp_az.c \
	$(MOD_SRC)/isp_isf.c \
	$(MOD_SRC)/lag_wind.c \
	$(MOD_SRC)/levinson.c \
	$(MOD_SRC)/log2.c \
	$(MOD_SRC)/lp_dec2.c \
	$(MOD_SRC)/math_op.c \
	$(MOD_SRC)/mem_align.c \
        $(MOD_SRC)/oper_32b.c \
	$(MOD_SRC)/p_med_ol.c \
	$(MOD_SRC)/pit_shrp.c \
	$(MOD_SRC)/pitch_f4.c \
	$(MOD_SRC)/pred_lt4.c \
	$(MOD_SRC)/preemph.c \
	$(MOD_SRC)/q_gain2.c \
	$(MOD_SRC)/q_pulse.c \
        $(MOD_SRC)/qisf_ns.c \
	$(MOD_SRC)/qpisf_2s.c \
	$(MOD_SRC)/random.c \
	$(MOD_SRC)/residu.c \
	$(MOD_SRC)/scale.c \
	$(MOD_SRC)/syn_filt.c \
	$(MOD_SRC)/updt_tar.c \
	$(MOD_SRC)/util.c \
        $(MOD_SRC)/voAMRWBEnc.c \
	$(MOD_SRC)/voicefac.c \
	$(MOD_SRC)/wb_vad.c \
	$(MOD_SRC)/weight_a.c \
        $(ASM_SRC6)/cor_h_vec_opt.S \
	$(ASM_SRC6)/Deemph_32_opt.S \
	$(ASM_SRC6)/Dot_p_opt.S \
	$(ASM_SRC6)/Filt_6k_7k_opt.S \
	$(ASM_SRC6)/residu_asm_opt.S \
        $(ASM_SRC6)/scale_sig_opt.S \
	$(ASM_SRC6)/sub_vq_opt.S \
	$(ASM_SRC6)/Syn_filt_32_opt.S \
	$(ASM_SRC6)/syn_filt_opt.S \
	$(ASM_SRC6)/pred_lt4_1_opt.S \
        $(ASM_SRC6)/convolve_opt.S \
	$(ASM_SRC6)/Norm_Corr_opt.S


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


VOMM:=-DLINUX -DARM -DARMv6 -DASM_OPT -DARMGCC_OPT #-DLCHECK 
LOCAL_LDLIBS:= -llog
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x03260000   -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char

#LOCAL_LDLIBS+=../../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

