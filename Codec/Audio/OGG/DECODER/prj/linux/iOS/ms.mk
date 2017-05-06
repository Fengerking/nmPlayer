
# please list all objects needed by your target here
VOMSRC:=bitwise.c block.c codebook.c floor0.c floor1.c functionreg.c info.c lacedecode.c mapping.c mdct_fft.c \
			oggdec.c oggframing.c residue.c sharedbook.c tables.c vorbismemory.c window.c window_tables.c #cmnMemory.c 
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../src \
	   ../../../../inc \
	   ../../../../../../../../Include \
	   ../../../../../../../../Common 



ifeq ($(VOTT), v4)
VOMSRC+= 
VOSRCDIR+= ../../../../../sfile/linuxasm 
endif				

ifeq ($(VOTT), v5)
VOMSRC+=
VOSRCDIR+= ../../../../../sfile/linuxasm 
endif

ifeq ($(VOTT), v6)
VOMSRC+=#voMemory.s #imdct_arm_v6.s
VOSRCDIR+= ../../../../../sfile/linuxasm 
endif      

ifeq ($(VOTT), v7)
VOMSRC+= #voMemory.s
VOSRCDIR+= ../../../../../sfile/linuxasm 
endif			


ifeq ($(VOTT), v7s)
VOMSRC+= #voMemory.s
VOSRCDIR+= ../../../../../sfile/linuxasm 
endif			
