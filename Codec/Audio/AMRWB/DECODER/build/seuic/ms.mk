# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc \
          ../../../src/c_src \
	  ../../../../../../../Common 

# please list all objects needed by your target here
OBJS:=agc2_fx.o d2t64_fx.o d4t64_fx.o d_gain2_fx.o dec_if_fx.o dec_main_fx.o decim54_fx.o \
      deemph_fx.o dtx_fx.o gpclip_fx.o hf_func_fx.o homing_fx.o hp400_fx.o hp50_fx.o hp6k_fx.o \
      hp7k_fx.o hp_wsp_fx.o if_rom_fx.o int_lpc_fx.o int_lpc_p_fx.o isfextrp_fx.o isp_az_fx.o isp_isf_fx.o \
      lagconc_fx.o log2.o math_op.o mem_align.o oper_32b.o overs12k8_fx.o p_med_ol_fx.o ph_disp_fx.o \
      pit_shrp_fx.o pred_lt4_fx.o preemph_fx.o q_pulse_fx.o qisf_ns_fx.o qpisf_2s_fx.o random_fx.o \
      residu_fx.o scale_fx.o stream.o syn_filt_fx.o tables_plus_fx.o util_fx.o voAMRWBDec.o \
      voicefac_fx.o weight_a_fx.o cmnVOMemory.o

			

