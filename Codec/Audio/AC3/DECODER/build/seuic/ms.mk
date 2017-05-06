# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc \
          ../../../src/c_src \
	  ../../../../../../../Common 

# please list all objects needed by your target here
OBJS:= ac3_dec.o ac3_dec_init.o ac3_info.o ac3d_tab.o ac3d_var.o bsi_d.o \
       cifft.o crc_calc.o crc_tab.o dolbytab.o downmix.o dsp_misc.o idctsc.o \
       mants_d.o matrix_d.o mem_align.o stream.o unpmants.o upk_subs.o voAC3Dec.o \
       window_d.o xdcall.o cmnMemory.o
			
	
