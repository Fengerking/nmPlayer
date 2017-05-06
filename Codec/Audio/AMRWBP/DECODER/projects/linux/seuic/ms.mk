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
OBJS:=basicop2.o count.o log2.o math_op.o oper_32b.o cmnMemory.o \
      ALF_emph_fx.o Bitstream_fx.o bits_p_fx.o decim12k8_fx.o decim_split_fx.o fft3_fx.o fft9_fx.o gaintcx_fx.o hf_func_fx.o \
			int_lpc_p_fx.o join_split_fx.o overs12k8_fx.o over_fs_fx.o q_gn_hf_fx.o q_isf_hf_fx.o re8_dec_fx.o Re8_dic_fx.o re8_ppv_fx.o \
			Re8_util_fx.o read_dat_fx.o rnd_ph16_fx.o r_fft_fx.o tables_plus_fx.o tables_stereo_fx.o util_plus_fx.o util_stereo_x_fx.o\
			wavefiletools_fx.o \
			amrwbplus_d_api.o avq_dec_fx.o bass_pf_fx.o dec_ace_fx.o dec_cp_state_fx.o dec_hf_fx.o dec_if_fx.o dec_lf_fx.o \
			dec_main_s_fx.o dec_prm_fx.o dec_tcx_fx.o d_gain2p_fx.o d_isf_2s_fx.o Scale_dec_fx.o tcx_ecu_fx.o \
			agc2_fx.o autocorr_fx.o az_isp_fx.o bits_fx.o c2t64_fx.o c4t64_fx.o cod_main_fx.o convolve_fx.o cor_h_x_fx.o \
			d2t64_fx.o d4t64_fx.o decim54_fx.o dec_main_fx.o deemph_fx.o dtx_fx.o d_gain2_fx.o gpclip_fx.o g_pitch_fx.o \
			homing_fx.o hp400_fx.o hp50_fx.o hp6k_fx.o hp7k_fx.o hp_wsp_fx.o if_rom_fx.o int_lpc_fx.o isfextrp_fx.o isp_az_fx.o \
			isp_isf_fx.o lagconc_fx.o lag_wind_fx.o levinson_fx.o lp_dec2_fx.o ph_disp_fx.o pitch_f4_fx.o pit_shrp_fx.o \
			pred_lt4_fx.o preemph_fx.o p_med_ol_fx.o qisf_ns_fx.o qpisf_2s_fx.o q_gain2_fx.o q_pulse_fx.o random_fx.o \
			residu_fx.o scale_fx.o syn_filt_fx.o updt_tar_fx.o util_fx.o voicefac_fx.o wb_vad_fx.o weight_a_fx.o \
			dec_hi_stereo_fx.o dec_tcx_stereo_fx.o d_stereo_x_fx.o
			
			
      

