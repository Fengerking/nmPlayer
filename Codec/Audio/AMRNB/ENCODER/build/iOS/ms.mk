# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc/common \
	  ../../../inc/encoder \
          ../../../src/c_src/common \
          ../../../src/c_src/encoder \
	  ../../../../../../../Common 

# please list all objects needed by your target here
VOMSRC:= dtx_enc.c gc_pred.c gmed_n.c int_lpc.c log2.c lsfwt.c lsp_az.c lsp_lsf.c \
       mem_align.c ph_disp.c pow2.c pred_lt.c q_plsf.c reorder.c residu.c strfunc.c \
       syn_filt.c table_def.c weight_a.c \
       autocorr.c az_lsp.c c1035pf.c c2_11pf.c c2_9pf.c c3_14pf.c c4_17pf.c c8_31pf.c \
       calc_cor.c calc_en.c cbsearch.c cl_ltp.c cod_amr.c convolve.c cor_h.c e_homing.c \
       enc_lag3.c enc_lag6.c g_adapt.c g_code.c g_pitch.c gain_q.c hp_max.c inter_36.c lag_wind.c \
       levinson.c lflg_upd.c lpc.c lsp.c mac_32.c ol_ltp.c p_ol_wgh.c pitch_fr.c pitch_ol.c pre_big.c \
       pre_proc.c prm2bits.c q_gain_c.c q_gain_p.c q_plsf_3.c q_plsf_5.c qgain475.c qgain795.c qua_gain.c \
       s10_8pf.c set_sign.c sid_sync.c sp_enc.c spreproc.c spstproc.c ton_stab.c vad1.c vad2.c vadname.c \
       voAMRNBEnc.c 
			


ifeq ($(VOTT), v6)
VOMSRC+= amr_memcpy.S autocorr_v6.S comp_corr_opt.S convolve_asm.S cor_h_asmv6.S cor_h_x_v6.S norm_corr_v6.S \
       Pred_lt_3or6_v6.S residu_asm.S syn_filt_0.S syn_filt_1.S vq_subvec_asm.S weight_v6.S
VOSRCDIR+= ../../../src/iOS_asm/armv6
endif				

ifeq ($(VOTT), v7)
VOMSRC+= autocorr_neon.S comp_corr.S convolve_opt.S cor_h_asmv6.S syn_filt0_neon.S cor_h_x_v6.S amrnb_vq_subvec_neon.S \
       G_pitch_asm.S Norm_Corr_opt.S Pred_lt_3or6_v6.S  residu_opt.S sum_armv7_xx.S syn_filt0_neon.S \
       syn_filt1_neon.S vo_postProcessEnc.S weight_asm.S 
VOSRCDIR+= ../../../src/iOS_asm/armv7
VOSRCDIR+= ../../../src/iOS_asm/armv6
endif			


ifeq ($(VOTT), v7s)
VOMSRC+= autocorr_neon.S comp_corr.S convolve_opt.S cor_h_asmv6.S syn_filt0_neon.S cor_h_x_v6.S amrnb_vq_subvec_neon.S \
       G_pitch_asm.S Norm_Corr_opt.S Pred_lt_3or6_v6.S  residu_opt.S sum_armv7_xx.S syn_filt0_neon.S \
       syn_filt1_neon.S vo_postProcessEnc.S weight_asm.S 
VOSRCDIR+= ../../../src/iOS_asm/armv7
VOSRCDIR+= ../../../src/iOS_asm/armv6
endif			
