
# please list all objects needed by your target here
VOMSRC:= ac3_dec.c ac3_dec_init.c ac3_info.c ac3d_tab.c ac3d_var.c bsi_d.c \
       cifft.c crc_calc.c crc_tab.c dolbytab.c downmix.c dsp_misc.c idctsc.c \
       mants_d.c matrix_d.c mem_align.c stream.c unpmants.c upk_subs.c voAC3Dec.c \
       window_d.c xdcall.c

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc \
          ../../../src/c_src \
	  ../../../../../../../Common 

ifeq ($(VOTT), v6)
VOMSRC+= upk_subs_asm.S crc_calc_asm.S cifft_asm.S window_d_asm.S  #downmix_asm.S #idctsc_asm.S have align3 issue 
VOSRCDIR+= ../../../src/ios/armv6
endif				

#ifeq ($(VOTT), v7)
#VOMSRC+= cifft_asm.S crc_calc_asm.S upk_subs_asm.S window_d_asm.S #idctsc_asm.S downmix_asm.S 
#VOSRCDIR+= ../../../src/ios/armv6
#endif			

#ifeq ($(VOTT), v7s)
#VOMSRC+= cifft_asm.S crc_calc_asm.S upk_subs_asm.S window_d_asm.S #idctsc_asm.S downmix_asm.S 
#VOSRCDIR+= ../../../src/ios/armv6
#endif			
