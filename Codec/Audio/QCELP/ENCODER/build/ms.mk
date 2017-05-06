# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
	  ../../inc \
          ../../src/c_src \
	  ../../../../../../Common 

# please list all objects needed by your target here
OBJS:=cb.o cb_tabs.o coderate.o decode.o encode.o filter.o frontfil.o \
      hammtab.o init.o lpc.o lsp.o mem_align.o modetabs.o pack.o pitch.o \
      quantabs.o quantize.o ratedec.o sincos.o snr.o stream.o target.o voQCELPEnc.o cmnMemory.o

			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../src/linux_asm/armv4
endif				

ifeq ($(VOTT), v5)
OBJS+= autocorr.o comp_corr1.o comp_corr_asm.o do_pole_filter.o do_zero_filter.o \
       filter_1_tap.o filter_response1.o line_fir_filter.o pole_filter.o pole_filter_response.o \
       recursive_conv.o vo_comput_cb.o zero_filter.o #Qcelp_memcpy.o 
VOSRCDIR+= ../../src/linux_asm/armv5
endif	

ifeq ($(VOTT), v6)
OBJS+= autocorr.o comp_corr1.o comp_corr_asm.o do_pole_filter.o do_zero_filter.o \
       filter_1_tap.o filter_response1.o line_fir_filter.o pole_filter.o pole_filter_response.o \
       recursive_conv.o vo_comput_cb.o zero_filter.o #Qcelp_memcpy.o 
VOSRCDIR+= ../../src/linux_asm/armv5
endif


ifeq ($(VOTT), v7)
OBJS+= autocorr_neon.o comp_corr1_neon.o comp_corr_asm.o do_pole_filter.o do_zero_filter_neon.o \
       filter_1_tap_neon.o filter_response1.o line_fir_filter.o pole_filter.o pole_filter_response.o \
       recursive_conv_neon.o vo_comput_cb.o zero_filter_neon.o

VOSRCDIR+= ../../src/linux_asm/armv7
endif			
