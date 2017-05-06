# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
	  ../../inc \
          ../../src/c_src \
	  ../../../../../../Common 

# please list all objects needed by your target here
OBJS:= bits.o calcexc.o de_acelp.o dec_gain.o dec_lag3.o dec_ld8a.o \
       dec_sid.o dspfunc.o filter.o g729_d_api.o gainpred.o \
       lpcfunc.o lspdec.o lspgetq.o mem_align.o oper_32b.o p_parity.o post_pro.o \
       postfilt.o pred_lt3.o qsidgain.o tab_dtx.o tab_ld8a.o taming.o \
       util.o  cmnMemory.o
			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../src/linux_asm/armv4
endif				

ifeq ($(VOTT), v6)
OBJS+= copy_opt.o Pred_lt_3_opt.o Residu.o sum_mac_v4.o sum_mac_v4_xy.o weight.o
VOSRCDIR+= ../../src/linux_asm/armv4
endif				

ifeq ($(VOTT), v7)
OBJS+= copy_opt.o Pred_lt_3_opt.o Residu.o sum_mac_v4.o sum_mac_v4_xy.o weight.o 
VOSRCDIR+= ../../src/linux_asm/armv4
endif			
