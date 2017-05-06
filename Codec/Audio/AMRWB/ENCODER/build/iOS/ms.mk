# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc \
          ../../../src/c_src \
	  ../../../../../../../Common 

# please list all objects needed by your target here
VOMSRC:= autocorr.c az_isp.c bits.c c2t64fx.c c4t64fx.c convolve.c cor_h_x.c decim54.c \
       deemph.c dtx.c g_pitch.c gpclip.c homing.c hp400.c hp50.c hp6k.c hp_wsp.c \
       int_lpc.c isp_az.c isp_isf.c lag_wind.c levinson.c log2.c lp_dec2.c math_op.c mem_align.c \
       oper_32b.c p_med_ol.c pit_shrp.c pitch_f4.c pred_lt4.c preemph.c q_gain2.c q_pulse.c \
       qisf_ns.c qpisf_2s.c random.c residu.c scale.c syn_filt.c updt_tar.c util.c \
       voAMRWBEnc.c voicefac.c wb_vad.c weight_a.c
			

ifeq ($(VOTT), v6)
VOMSRC+= cor_h_vec_opt.S Deemph_32_opt.S Dot_p_opt.S Filt_6k_7k_opt.S residu_asm_opt.S \
       scale_sig_opt.S sub_vq_opt.S Syn_filt_32_opt.S syn_filt_opt.S pred_lt4_1_opt.S \
       convolve_opt.S Norm_Corr_opt.S
VOSRCDIR+= ../../../src/iOS_asm/ARMv6
endif				

ifeq ($(VOTT), v7)
VOMSRC+= cor_h_vec_neon.S Deemph_32_neon.S Dot_p_neon.S Filt_6k_7k_neon.S residu_asm_neon.S \
       scale_sig_neon.S sub_vq_opt.S Syn_filt_32_neon.S syn_filt_neon.S pred_lt4_1_neon.S \
       convolve_neon.S Norm_Corr_neon.S
VOSRCDIR+= ../../../src/iOS_asm/ARMv7
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= cor_h_vec_neon.S Deemph_32_neon.S Dot_p_neon.S Filt_6k_7k_neon.S residu_asm_neon.S \
       scale_sig_neon.S sub_vq_opt.S Syn_filt_32_neon.S syn_filt_neon.S pred_lt4_1_neon.S \
       convolve_neon.S Norm_Corr_neon.S
VOSRCDIR+= ../../../src/iOS_asm/ARMv7
endif			



