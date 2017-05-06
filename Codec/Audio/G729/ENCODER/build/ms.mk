# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
	  ../../inc \
          ../../src/c_src \
	  ../../../../../../Common 

# please list all objects needed by your target here
OBJS:= acelp_ca.o bits.o cod_ld8a.o cor_func.o dspfunc.o \
       filter.o g729_e_api.o gainpred.o lpc.o lpcfunc.o lspgetq.o \
       mem_align.o oper_32b.o p_parity.o pitch_a.o pre_proc.o pred_lt2.o \
       qua_gain.o qua_lsp.o tab_ld8a.o taming.o util.o cmnMemory.o
			
ifeq ($(VOTT), v4)
OBJS+=Autocorr.o Cor_h_X.o Lsp_pre.o Pitch_ol.o Pred_lt_3.o \
      Residu.o g729_memcpy.o inv_sqrt.o sum_mac_v4.o sum_mac_v4_xy.o \
      syn_filt4_0.o syn_filt4_1.o weight.o 
VOSRCDIR+= ../../src/linux_asm/armv4
endif				

ifeq ($(VOTT), v6)
OBJS+=Autocorr.o Cor_h_X.o Lsp_pre.o Pitch_ol.o Pred_lt_3.o \
      Residu.o g729_memcpy.o inv_sqrt.o sum_mac_v4.o sum_mac_v4_xy.o \
      syn_filt4_0.o syn_filt4_1.o weight.o 
VOSRCDIR+= ../../src/linux_asm/armv4
endif				

ifeq ($(VOTT), v7)
OBJS+=Autocorr.o Cor_h_X.o Lsp_pre.o Pitch_ol.o Pred_lt_3.o \
      Residu.o g729_memcpy.o inv_sqrt.o sum_mac_v4.o sum_mac_v4_xy.o \
      syn_filt4_0.o syn_filt4_1.o weight.o  
VOSRCDIR+= ../../src/linux_asm/armv4
endif			
