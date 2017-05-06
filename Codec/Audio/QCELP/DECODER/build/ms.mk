# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
	  ../../inc \
          ../../src/c_src \
	  ../../../../../../Common 

# please list all objects needed by your target here
OBJS:=cb.o decode13.o filter.o log10.o lpc.o lsp.o mem_align.o pack13.o \
      pitch.o qcelp13.o quant.o quant13.o stream.o voQCELPDec.o cmnMemory.o

			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../src/linuxasm
endif				

ifeq ($(VOTT), v5)
OBJS+= wghtfilt_asm.o 
VOSRCDIR+= ../../src/linux_asm
endif

ifeq ($(VOTT), v6)
OBJS+= wghtfilt_asm.o 
VOSRCDIR+= ../../src/linux_asm
endif				


ifeq ($(VOTT), v7)
OBJS+= wghtfilt_asm.o 
VOSRCDIR+= ../../src/linux_asm
endif			
