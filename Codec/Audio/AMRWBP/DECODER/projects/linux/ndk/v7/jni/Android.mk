LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoAMRWBPDec

CMN_SRC:=../../../../../../../../../Common
MOD_SRC1:=../../../../../source/basic_op
MOD_SRC2:=../../../../../source/common_fx
MOD_SRC3:=../../../../../source/decoder_lib_fx
MOD_SRC4:=../../../../../source/lib_amr_fx
MOD_SRC5:=../../../../../source/stereo_fx_eks
#ASM_SRC6:=../../../../src/linux_asm/armv5
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
        $(MOD_SRC1)/basicop2.c \
	$(MOD_SRC1)/count.c \
	$(MOD_SRC1)/log2.c \
	$(MOD_SRC1)/math_op.c \
	$(MOD_SRC1)/oper_32b.c \
        $(MOD_SRC2)/ALF_emph_fx.c \
	$(MOD_SRC2)/Bitstream_fx.c \
	$(MOD_SRC2)/bits_p_fx.c \
	$(MOD_SRC2)/decim12k8_fx.c \
	$(MOD_SRC2)/decim_split_fx.c \
	$(MOD_SRC2)/fft3_fx.c \
	$(MOD_SRC2)/fft9_fx.c \
	$(MOD_SRC2)/gaintcx_fx.c \
	$(MOD_SRC2)/hf_func_fx.c \
	$(MOD_SRC2)/int_lpc_p_fx.c \
	$(MOD_SRC2)/join_split_fx.c \
	$(MOD_SRC2)/overs12k8_fx.c \
	$(MOD_SRC2)/over_fs_fx.c \
	$(MOD_SRC2)/q_gn_hf_fx.c \
	$(MOD_SRC2)/q_isf_hf_fx.c \
	$(MOD_SRC2)/re8_dec_fx.c \
	$(MOD_SRC2)/Re8_dic_fx.c \
	$(MOD_SRC2)/re8_ppv_fx.c \
	$(MOD_SRC2)/Re8_util_fx.c \
	$(MOD_SRC2)/read_dat_fx.c \
	$(MOD_SRC2)/rnd_ph16_fx.c \
	$(MOD_SRC2)/r_fft_fx.c \
	$(MOD_SRC2)/tables_plus_fx.c \
	$(MOD_SRC2)/tables_stereo_fx.c \
	$(MOD_SRC2)/util_plus_fx.c \
	$(MOD_SRC2)/util_stereo_x_fx.c\
	$(MOD_SRC2)/wavefiletools_fx.c \
        $(MOD_SRC3)/amrwbplus_d_api.c \
	$(MOD_SRC3)/avq_dec_fx.c \
	$(MOD_SRC3)/bass_pf_fx.c \
	$(MOD_SRC3)/dec_ace_fx.c \
	$(MOD_SRC3)/dec_cp_state_fx.c \
	$(MOD_SRC3)/dec_hf_fx.c \
	$(MOD_SRC3)/dec_if_fx.c \
	$(MOD_SRC3)/dec_lf_fx.c \
	$(MOD_SRC3)/dec_main_s_fx.c \
	$(MOD_SRC3)/dec_prm_fx.c \
	$(MOD_SRC3)/dec_tcx_fx.c \
	$(MOD_SRC3)/d_gain2p_fx.c \
	$(MOD_SRC3)/d_isf_2s_fx.c \
	$(MOD_SRC3)/Scale_dec_fx.c \
	$(MOD_SRC3)/tcx_ecu_fx.c \
	$(MOD_SRC4)/agc2_fx.c \
	$(MOD_SRC4)/autocorr_fx.c \
	$(MOD_SRC4)/az_isp_fx.c \
	$(MOD_SRC4)/bits_fx.c \
	$(MOD_SRC4)/c2t64_fx.c \
	$(MOD_SRC4)/c4t64_fx.c \
	$(MOD_SRC4)/cod_main_fx.c \
	$(MOD_SRC4)/convolve_fx.c \
	$(MOD_SRC4)/cor_h_x_fx.c \
	$(MOD_SRC4)/d2t64_fx.c \
	$(MOD_SRC4)/d4t64_fx.c \
	$(MOD_SRC4)/decim54_fx.c \
	$(MOD_SRC4)/dec_main_fx.c \
	$(MOD_SRC4)/deemph_fx.c \
	$(MOD_SRC4)/dtx_fx.c \
	$(MOD_SRC4)/d_gain2_fx.c \
	$(MOD_SRC4)/gpclip_fx.c \
	$(MOD_SRC4)/g_pitch_fx.c \
	$(MOD_SRC4)/homing_fx.c \
	$(MOD_SRC4)/hp400_fx.c \
	$(MOD_SRC4)/hp50_fx.c \
	$(MOD_SRC4)/hp6k_fx.c \
	$(MOD_SRC4)/hp7k_fx.c \
	$(MOD_SRC4)/hp_wsp_fx.c \
	$(MOD_SRC4)/if_rom_fx.c \
	$(MOD_SRC4)/int_lpc_fx.c \
	$(MOD_SRC4)/isfextrp_fx.c \
	$(MOD_SRC4)/isp_az_fx.c \
	$(MOD_SRC4)/isp_isf_fx.c \
	$(MOD_SRC4)/lagconc_fx.c \
	$(MOD_SRC4)/lag_wind_fx.c \
	$(MOD_SRC4)/levinson_fx.c \
	$(MOD_SRC4)/lp_dec2_fx.c \
	$(MOD_SRC4)/ph_disp_fx.c \
	$(MOD_SRC4)/pitch_f4_fx.c \
	$(MOD_SRC4)/pit_shrp_fx.c \
	$(MOD_SRC4)/pred_lt4_fx.c \
	$(MOD_SRC4)/preemph_fx.c \
	$(MOD_SRC4)/p_med_ol_fx.c \
	$(MOD_SRC4)/qisf_ns_fx.c \
	$(MOD_SRC4)/qpisf_2s_fx.c \
	$(MOD_SRC4)/q_gain2_fx.c \
	$(MOD_SRC4)/q_pulse_fx.c \
	$(MOD_SRC4)/random_fx.c \
	$(MOD_SRC4)/residu_fx.c \
	$(MOD_SRC4)/scale_fx.c \
	$(MOD_SRC4)/syn_filt_fx.c \
	$(MOD_SRC4)/updt_tar_fx.c \
	$(MOD_SRC4)/util_fx.c \
	$(MOD_SRC4)/voicefac_fx.c \
	$(MOD_SRC4)/wb_vad_fx.c \
	$(MOD_SRC4)/weight_a_fx.c \
	$(MOD_SRC5)/dec_hi_stereo_fx.c \
	$(MOD_SRC5)/dec_tcx_stereo_fx.c \
	$(MOD_SRC5)/d_stereo_x_fx.c


LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	../../../../../../../../../Common \
	../../../../../source/basic_op \
	../../../../../source/include_fx \
	../../../../../source/lib_amr_fx


VOMM:=-DLINUX -DARM -DARMV6 -DARMV7 -DLCHECK -DMSVC_AMRWBPLUS
LOCAL_LDLIBS:= -llog
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02270000    -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS+=../../../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

