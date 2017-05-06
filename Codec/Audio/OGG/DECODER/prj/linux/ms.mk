
# please list all objects needed by your target here
OBJS:=bitwise.o block.o codebook.o floor0.o floor1.o functionreg.o info.o lacedecode.o mapping.o mdct_fft.o \
			oggdec.o oggframing.o residue.o sharedbook.o tables.o vorbismemory.o window.o window_tables.o cmnMemory.o 
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../src \
	   ../../../../../inc \
	   ../../../../../../../../../Include \
	   ../../../../../../../../../Common 



ifeq ($(VOTT), v4)
OBJS+=voMemory.o 
VOSRCDIR+= ../../../../../sfile/linuxasm 
endif				

ifeq ($(VOTT), v5)
OBJS+=voMemory.o
VOSRCDIR+= ../../../../../sfile/linuxasm 
endif

ifeq ($(VOTT), v6)
OBJS+=voMemory.o #imdct_arm_v6.o
VOSRCDIR+= ../../../../../sfile/linuxasm 
endif      

ifeq ($(VOTT), v7)
OBJS+=voMemory.o
VOSRCDIR+= ../../../../../sfile/linuxasm 
endif			
