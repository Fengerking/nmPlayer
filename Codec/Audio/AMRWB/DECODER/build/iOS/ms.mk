# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc \
          ../../../src/c_src \
	  ../../../../../../../Common 

# please list all objects needed by your target here
VOMSRC:=agc2_fx.c d2t64_fx.c d4t64_fx.c d_gain2_fx.c dec_if_fx.c dec_main_fx.c decim54_fx.c \
      deemph_fx.c dtx_fx.c gpclip_fx.c hf_func_fx.c homing_fx.c hp400_fx.c hp50_fx.c hp6k_fx.c \
      hp7k_fx.c hp_wsp_fx.c if_rom_fx.c int_lpc_fx.c int_lpc_p_fx.c isfextrp_fx.c isp_az_fx.c isp_isf_fx.c \
      lagconc_fx.c log2.c math_op.c mem_align.c oper_32b.c overs12k8_fx.c p_med_ol_fx.c ph_disp_fx.c \
      pit_shrp_fx.c pred_lt4_fx.c preemph_fx.c q_pulse_fx.c qisf_ns_fx.c qpisf_2s_fx.c random_fx.c \
      residu_fx.c scale_fx.c stream.c syn_filt_fx.c tables_plus_fx.c util_fx.c voAMRWBDec.cpp \
      voicefac_fx.c weight_a_fx.c cmnVOMemory.cpp

			
ifeq ($(VOTT), v4)
VOMSRC+= 
VOSRCDIR+= 
endif				

ifeq ($(VOTT), v5)
VOMSRC+= Interpol_opt.S Dot_p_opt.S Syn_filt_opt.S Syn_filt_32_opt.S Scale_sig_opt.S \
       pret_lt4_opt.S Filt_7k_opt.S Filt_6k_7k_opt.S
VOSRCDIR+= ../../../src/iOS/armv5
endif

ifeq ($(VOTT), v6)
VOMSRC+= Interpol_opt.S Dot_p_opt.S Syn_filt_opt.S Syn_filt_32_opt.S Scale_sig_opt.S \
       pret_lt4_opt.S Filt_7k_opt.S Filt_6k_7k_opt.S
VOSRCDIR+= ../../../src/iOS/armv5
endif				

ifeq ($(VOTT), v7)
VOMSRC+= Interpol_neon.S Dot_p_neon.S Syn_filt_neon.S Syn_filt_32_neon.S Scale_sig_neon.S \
       pret_lt4_neon.S Filt_7k_neon.S Filt_6k_7k_neon.S 
VOSRCDIR+= ../../../src/iOS/armv7
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= Interpol_neon.S Dot_p_neon.S Syn_filt_neon.S Syn_filt_32_neon.S Scale_sig_neon.S \
       pret_lt4_neon.S Filt_7k_neon.S Filt_6k_7k_neon.S 
VOSRCDIR+= ../../../src/iOS/armv7
endif		