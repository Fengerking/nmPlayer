# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../../Include \
	  ../../../../source/include_fx \
	  ../../../../source/basic_op \
	  ../../../../source/stereo_fx_eks  \
          ../../../../source/common_fx \
	  ../../../../source/decoder_lib_fx \
	  ../../../../source/lib_amr_fx \
	  ../../../../../../../../Common 

# please list all objects needed by your target here
VOMSRC:=basicop2.c count.c log2.c math_op.c oper_32b.c  \
      ALF_emph_fx.c Bitstream_fx.c bits_p_fx.c decim12k8_fx.c decim_split_fx.c fft3_fx.c fft9_fx.c gaintcx_fx.c hf_func_fx.c \
			int_lpc_p_fx.c join_split_fx.c overs12k8_fx.c over_fs_fx.c q_gn_hf_fx.c q_isf_hf_fx.c re8_dec_fx.c Re8_dic_fx.c re8_ppv_fx.c \
			Re8_util_fx.c read_dat_fx.c rnd_ph16_fx.c r_fft_fx.c tables_plus_fx.c tables_stereo_fx.c util_plus_fx.c util_stereo_x_fx.c\
			wavefiletools_fx.c \
			amrwbplus_d_api.c avq_dec_fx.c bass_pf_fx.c dec_ace_fx.c dec_cp_state_fx.c dec_hf_fx.c dec_if_fx.c dec_lf_fx.c \
			dec_main_s_fx.c dec_prm_fx.c dec_tcx_fx.c d_gain2p_fx.c d_isf_2s_fx.c Scale_dec_fx.c tcx_ecu_fx.c \
			agc2_fx.c autocorr_fx.c az_isp_fx.c bits_fx.c c2t64_fx.c c4t64_fx.c cod_main_fx.c convolve_fx.c cor_h_x_fx.c \
			d2t64_fx.c d4t64_fx.c decim54_fx.c dec_main_fx.c deemph_fx.c dtx_fx.c d_gain2_fx.c gpclip_fx.c g_pitch_fx.c \
			homing_fx.c hp400_fx.c hp50_fx.c hp6k_fx.c hp7k_fx.c hp_wsp_fx.c if_rom_fx.c int_lpc_fx.c isfextrp_fx.c isp_az_fx.c \
			isp_isf_fx.c lagconc_fx.c lag_wind_fx.c levinson_fx.c lp_dec2_fx.c ph_disp_fx.c pitch_f4_fx.c pit_shrp_fx.c \
			pred_lt4_fx.c preemph_fx.c p_med_ol_fx.c qisf_ns_fx.c qpisf_2s_fx.c q_gain2_fx.c q_pulse_fx.c random_fx.c \
			residu_fx.c scale_fx.c syn_filt_fx.c updt_tar_fx.c util_fx.c voicefac_fx.c wb_vad_fx.c weight_a_fx.c \
			dec_hi_stereo_fx.c dec_tcx_stereo_fx.c d_stereo_x_fx.c
			
			
      

