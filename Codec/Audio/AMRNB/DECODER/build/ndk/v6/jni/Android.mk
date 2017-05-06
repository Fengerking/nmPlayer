LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoAMRNBDec

CMN_SRC:=../../../../../../../../Common
ACMN_SRC:=../../../../src/c_src/common
DEC_SRC:=../../../../src/c_src/decoder
ASM_SRC6:=../../../../src/linux_asm/armv6
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(ACMN_SRC)/copy.c \
	$(ACMN_SRC)/dtx_enc.c \
	$(ACMN_SRC)/gc_pred.c \
	$(ACMN_SRC)/gmed_n.c \
	$(ACMN_SRC)/int_lpc.c \
	$(ACMN_SRC)/inv_sqrt.c \
	$(ACMN_SRC)/log2.c \
	$(ACMN_SRC)/lsp_az.c \
	$(ACMN_SRC)/lsp_lsf.c \
	$(ACMN_SRC)/mem_align.c \
	$(ACMN_SRC)/oper_32b.c \
	$(ACMN_SRC)/ph_disp.c \
	$(ACMN_SRC)/pow2.c \
	$(ACMN_SRC)/pred_lt.c \
	$(ACMN_SRC)/q_plsf.c \
	$(ACMN_SRC)/reorder.c \
	$(ACMN_SRC)/residu.c \
	$(ACMN_SRC)/set_zero.c \
	$(ACMN_SRC)/sqrt_l.c \
	$(ACMN_SRC)/strfunc.c \
	$(ACMN_SRC)/syn_filt.c \
	$(ACMN_SRC)/table_def.c \
	$(ACMN_SRC)/weight_a.c \
	$(DEC_SRC)/a_refl.c \
	$(DEC_SRC)/agc.c \
	$(DEC_SRC)/b_cn_cod.c \
	$(DEC_SRC)/bgnscd.c \
	$(DEC_SRC)/bits2prm.c \
	$(DEC_SRC)/c_g_aver.c \
	$(DEC_SRC)/d1035pf.c \
	$(DEC_SRC)/d2_11pf.c \
	$(DEC_SRC)/d2_9pf.c \
	$(DEC_SRC)/d3_14pf.c \
	$(DEC_SRC)/d4_17pf.c \
	$(DEC_SRC)/d8_31pf.c \
	$(DEC_SRC)/d_gain_c.c \
	$(DEC_SRC)/d_gain_p.c \
	$(DEC_SRC)/d_homing.c \
	$(DEC_SRC)/d_plsf.c \
	$(DEC_SRC)/d_plsf_3.c \
	$(DEC_SRC)/d_plsf_5.c \
	$(DEC_SRC)/dec_amr.c \
	$(DEC_SRC)/dec_gain.c \
	$(DEC_SRC)/dec_lag3.c \
	$(DEC_SRC)/dec_lag6.c \
	$(DEC_SRC)/dtx_dec.c \
	$(DEC_SRC)/ec_gains.c \
	$(DEC_SRC)/ex_ctrl.c \
	$(DEC_SRC)/int_lsf.c \
	$(DEC_SRC)/lsp_avg.c \
	$(DEC_SRC)/post_pro.c \
	$(DEC_SRC)/preemph.c \
	$(DEC_SRC)/pstfilt.c \
	$(DEC_SRC)/sp_dec.c \
	$(DEC_SRC)/voAMRNBDec.c \
	$(ASM_SRC6)/amr_memcpy.S \
	$(ASM_SRC6)/syn_filt_0.S \
	$(ASM_SRC6)/syn_filt_1.S \
	$(ASM_SRC6)/syn_filt_2.S 


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc/common \
	../../../../inc/decoder


VOMM:=-DARM -DARMV6 -DASM_OPT -DLCHECK 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02250000   -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

