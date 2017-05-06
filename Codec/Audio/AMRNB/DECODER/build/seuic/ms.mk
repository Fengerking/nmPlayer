# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc/common \
	  ../../../inc/decoder\
          ../../../src/c_src/common \
          ../../../src/c_src/decoder \
	  ../../../../../../../Common 

# please list all objects needed by your target here
OBJS:= copy.o dtx_enc.o gc_pred.o gmed_n.o int_lpc.o inv_sqrt.o \
       log2.o lsp_az.o lsp_lsf.o mem_align.o oper_32b.o ph_disp.o \
       pow2.o pred_lt.o q_plsf.o reorder.o residu.o set_zero.o sqrt_l.o strfunc.o \
       syn_filt.o table_def.o weight_a.o \
       a_refl.o agc.o b_cn_cod.o bgnscd.o bits2prm.o c_g_aver.o d1035pf.o d2_11pf.o \
       d2_9pf.o d3_14pf.o d4_17pf.o d8_31pf.o d_gain_c.o d_gain_p.o d_homing.o d_plsf.o \
       d_plsf_3.o d_plsf_5.o dec_amr.o dec_gain.o dec_lag3.o dec_lag6.o dtx_dec.o ec_gains.o \
       ex_ctrl.o int_lsf.o lsp_avg.o post_pro.o preemph.o pstfilt.o sp_dec.o voAMRNBDec.o cmnMemory.o
			
	
