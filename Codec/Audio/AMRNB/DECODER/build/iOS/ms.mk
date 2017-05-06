# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc/common \
	  ../../../inc/decoder\
          ../../../src/c_src/common \
          ../../../src/c_src/decoder \
	  ../../../../../../../Common 

# please list all objects needed by your target here
VOMSRC:= copy.c dtx_enc.c gc_pred.c gmed_n.c int_lpc.c inv_sqrt.c \
       log2.c lsp_az.c lsp_lsf.c mem_align.c oper_32b.c ph_disp.c \
       pow2.c pred_lt.c q_plsf.c reorder.c residu.c set_zero.c sqrt_l.c strfunc.c \
       syn_filt.c table_def.c weight_a.c \
       a_refl.c agc.c b_cn_cod.c bgnscd.c bits2prm.c c_g_aver.c d1035pf.c d2_11pf.c \
       d2_9pf.c d3_14pf.c d4_17pf.c d8_31pf.c d_gain_c.c d_gain_p.c d_homing.c d_plsf.c \
       d_plsf_3.c d_plsf_5.c dec_amr.c dec_gain.c dec_lag3.c dec_lag6.c dtx_dec.c ec_gains.c \
       ex_ctrl.c int_lsf.c lsp_avg.c post_pro.c preemph.c pstfilt.c sp_dec.c voAMRNBDec.c 
			
ifeq ($(VOTT), v4)
VOMSRC+= 
VOSRCDIR+= ../../../src/linux_asm/armv4
endif				

ifeq ($(VOTT), v5)
VOMSRC+= amr_memcpy.S syn_filt_0.S syn_filt_1.S syn_filt_2.S 
VOSRCDIR+= ../../../src/iOS/armv5
endif

ifeq ($(VOTT), v6)
VOMSRC+= amr_memcpy.S syn_filt_0.S syn_filt_1.S syn_filt_2.S 
VOSRCDIR+= ../../../src/iOS/armv6
endif				

ifeq ($(VOTT), v7)
VOMSRC+= agc2_neon.S agc_neon.S amr_memcpy.S amrnb_residu_neon.S ex_ltp_neon.S \
       excp_fun_neon.S sum_armv7_xx.S syn_filt_0.S syn_filt_1.S syn_filt_2.S weight_ai.S
VOSRCDIR+= ../../../src/iOS/armv7
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= agc2_neon.S agc_neon.S amr_memcpy.S amrnb_residu_neon.S ex_ltp_neon.S \
       excp_fun_neon.S sum_armv7_xx.S syn_filt_0.S syn_filt_1.S syn_filt_2.S weight_ai.S
VOSRCDIR+= ../../../src/iOS/armv7
endif			
