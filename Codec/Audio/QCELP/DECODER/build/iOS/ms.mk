# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc \
          ../../../src/c_src \
	  ../../../../../../../Common 

# please list all objects needed by your target here
VOMSRC:=cb.c decode13.c filter.c log10.c lpc.c lsp.c mem_align.c pack13.c \
      pitch.c qcelp13.c quant.c quant13.c stream.c voQCELPDec.c #cmnMemory.c			


ifeq ($(VOTT), v6)
VOMSRC+= wghtfilt_asm.s 
VOSRCDIR+= ../../../src/iOS
endif				


ifeq ($(VOTT), v7)
VOMSRC+= wghtfilt_asm.s
VOSRCDIR+= ../../../src/iOS
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= wghtfilt_asm.s
VOSRCDIR+= ../../../src/iOS
endif			
