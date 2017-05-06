LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoAMRNBEnc

CMN_SRC:=../../../../../../../../Common
ACMN_SRC:=../../../../src/c_src/common
ENC_SRC:=../../../../src/c_src/encoder
ASM_SRC7:=../../../../src/linux_asm/armv7
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(ACMN_SRC)/dtx_enc.c \
	$(ACMN_SRC)/gc_pred.c \
	$(ACMN_SRC)/gmed_n.c \
	$(ACMN_SRC)/int_lpc.c \
	$(ACMN_SRC)/log2.c \
	$(ACMN_SRC)/lsfwt.c \
	$(ACMN_SRC)/lsp_az.c \
	$(ACMN_SRC)/lsp_lsf.c \
	$(ACMN_SRC)/mem_align.c \
	$(ACMN_SRC)/ph_disp.c \
	$(ACMN_SRC)/pow2.c \
	$(ACMN_SRC)/pred_lt.c \
	$(ACMN_SRC)/q_plsf.c \
	$(ACMN_SRC)/reorder.c \
	$(ACMN_SRC)/residu.c \
	$(ACMN_SRC)/strfunc.c \
	$(ACMN_SRC)/syn_filt.c \
	$(ACMN_SRC)/table_def.c \
	$(ACMN_SRC)/weight_a.c \
	$(ENC_SRC)/autocorr.c \
	$(ENC_SRC)/az_lsp.c \
	$(ENC_SRC)/c1035pf.c \
	$(ENC_SRC)/c2_11pf.c \
	$(ENC_SRC)/c2_9pf.c \
	$(ENC_SRC)/c3_14pf.c \
	$(ENC_SRC)/c4_17pf.c \
	$(ENC_SRC)/c8_31pf.c \
	$(ENC_SRC)/calc_cor.c \
	$(ENC_SRC)/calc_en.c \
	$(ENC_SRC)/cbsearch.c \
	$(ENC_SRC)/cl_ltp.c \
	$(ENC_SRC)/cod_amr.c \
	$(ENC_SRC)/convolve.c \
	$(ENC_SRC)/cor_h.c \
	$(ENC_SRC)/e_homing.c \
	$(ENC_SRC)/enc_lag3.c \
	$(ENC_SRC)/enc_lag6.c \
	$(ENC_SRC)/g_adapt.c \
	$(ENC_SRC)/g_code.c \
	$(ENC_SRC)/g_pitch.c \
	$(ENC_SRC)/gain_q.c \
	$(ENC_SRC)/hp_max.c \
	$(ENC_SRC)/inter_36.c \
	$(ENC_SRC)/lag_wind.c \
	$(ENC_SRC)/levinson.c \
	$(ENC_SRC)/lflg_upd.c \
	$(ENC_SRC)/lpc.c \
	$(ENC_SRC)/lsp.c \
	$(ENC_SRC)/mac_32.c \
	$(ENC_SRC)/ol_ltp.c \
	$(ENC_SRC)/p_ol_wgh.c \
	$(ENC_SRC)/pitch_fr.c \
	$(ENC_SRC)/pitch_ol.c \
	$(ENC_SRC)/pre_big.c \
	$(ENC_SRC)/pre_proc.c \
	$(ENC_SRC)/prm2bits.c \
	$(ENC_SRC)/q_gain_c.c \
	$(ENC_SRC)/q_gain_p.c \
	$(ENC_SRC)/q_plsf_3.c \
	$(ENC_SRC)/q_plsf_5.c \
	$(ENC_SRC)/qgain475.c \
	$(ENC_SRC)/qgain795.c \
	$(ENC_SRC)/qua_gain.c \
	$(ENC_SRC)/s10_8pf.c \
	$(ENC_SRC)/set_sign.c \
	$(ENC_SRC)/sid_sync.c \
	$(ENC_SRC)/sp_enc.c \
	$(ENC_SRC)/spreproc.c \
	$(ENC_SRC)/spstproc.c \
	$(ENC_SRC)/ton_stab.c \
	$(ENC_SRC)/vad1.c \
	$(ENC_SRC)/vad2.c \
	$(ENC_SRC)/vadname.c \
	$(ENC_SRC)/voAMRNBEnc.c \
	$(ASM_SRC7)/autocorr_neon.S \
	$(ASM_SRC7)/comp_corr.S \
	$(ASM_SRC7)/convolve_opt.S \
	$(ASM_SRC7)/cor_h_opt.S \
	$(ASM_SRC7)/cor_h_x2_opt.S \
	$(ASM_SRC7)/G_pitch_asm.S \
	$(ASM_SRC7)/Norm_Corr_opt.S \
	$(ASM_SRC7)/Pred_lt_3or6_opt.S \
	$(ASM_SRC7)/residu_opt.S \
	$(ASM_SRC7)/sum_armv7_xx.S \
	$(ASM_SRC7)/syn_filt0_neon.S \
	$(ASM_SRC7)/syn_filt1_neon.S \
	$(ASM_SRC7)/vo_postProcessEnc.S \
        $(ASM_SRC7)/amrnb_vq_subvec_neon.S \
	$(ASM_SRC7)/weight_asm.S	


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc/common \
	../../../../inc/encoder 


VOMM:=-DARM -DARMV6 -DARMV7 -DASMv5_OPT -DARMv6_OPT -DARMv7_OPT -DLCHECK 
LOCAL_LDLIBS:= -llog
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x03250000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS+=../../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

