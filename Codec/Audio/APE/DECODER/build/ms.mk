# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
	  ../../inc \
          ../../src \
	  ../../../../../../Common 

# please list all objects needed by your target here
OBJS:= voAPE_D_Api.o mem_align.o voAPE_HeadParser.o voAPE_Predictor.o \
       voAPEBits.o voAPEDecompress.o cmnMemory.o stream.o
			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../src/linux_asm/armv4
endif				

ifeq ($(VOTT), v6)
OBJS+= 
VOSRCDIR+= ../../src/linux_asm/armv6
endif				

ifeq ($(VOTT), v7)
OBJS+=   
VOSRCDIR+= ../../src/linux_asm/armv6
endif


