# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
	  ../../inc/common \
	  ../../inc/encoder \
          ../../src/c_src/common \
          ../../src/c_src/encoder \
	  ../../../../../../Common 

# please list all objects needed by your target here
OBJS:= dtx_enc.o gc_pred.o gmed_n.o int_lpc.o log2.o lsfwt.o lsp_az.o lsp_lsf.o \
       mem_align.o ph_disp.o pow2.o pred_lt.o q_plsf.o reorder.o residu.o strfunc.o \
       syn_filt.o table_def.o weight_a.o \
       autocorr.o az_lsp.o c1035pf.o c2_11pf.o c2_9pf.o c3_14pf.o c4_17pf.o c8_31pf.o \
       calc_cor.o calc_en.o cbsearch.o cl_ltp.o cod_amr.o convolve.o cor_h.o e_homing.o \
       enc_lag3.o enc_lag6.o g_adapt.o g_code.o g_pitch.o gain_q.o hp_max.o inter_36.o lag_wind.o \
       levinson.o lflg_upd.o lpc.o lsp.o mac_32.o ol_ltp.o p_ol_wgh.o pitch_fr.o pitch_ol.o pre_big.o \
       pre_proc.o prm2bits.o q_gain_c.o q_gain_p.o q_plsf_3.o q_plsf_5.o qgain475.o qgain795.o qua_gain.o \
       s10_8pf.o set_sign.o sid_sync.o sp_enc.o spreproc.o spstproc.o ton_stab.o vad1.o vad2.o vadname.o \
       voAMRNBEnc.o cmnMemory.o
			
ifeq ($(VOTT), v4)
OBJS+= comp_corr_opt.o residu_asm.o 
VOSRCDIR+= ../../src/linux_asm/armv4
endif				

ifeq ($(VOTT), v5)
OBJS+=amr_memcpy.o comp_corr_opt.o residu_asm.o syn_filt_0.o syn_filt_1.o
VOSRCDIR+= ../../src/linux_asm/armv5
endif

ifeq ($(VOTT), v6)
OBJS+= amr_memcpy.o autocorr_v6.o comp_corr_opt.o convolve_asm.o cor_h_asmv6.o cor_h_x_v6.o norm_corr_v6.o \
       Pred_lt_3or6_v6.o residu_asm.o syn_filt_0.o syn_filt_1.o vq_subvec_asm.o weight_v6.o
VOSRCDIR+= ../../src/linux_asm/armv6
endif				

ifeq ($(VOTT), v7)
OBJS+= amrnb_vq_subvec_neon.o autocorr_neon.o comp_corr.o convolve_opt.o cor_h_opt.o syn_filt0_neon.o cor_h_x2_opt.o \
       G_pitch_asm.o Norm_Corr_opt.o Pred_lt_3or6_opt.o  residu_opt.o sum_armv7_xx.o syn_filt0_neon.o \
       syn_filt1_neon.o vo_postProcessEnc.o weight_asm.o\

VOSRCDIR+= ../../src/linux_asm/armv7
endif			
