# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc \
          ../../../src/c_src \
	  ../../../../../../../Common 

# please list all objects needed by your target here
OBJS:= bit.o frame.o mem_align.o stream.o subframe.o voFLACDec.o cmnMemory.o

			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../../src/linuxasm/armv4
endif				

ifeq ($(VOTT), v6)
OBJS+= 
VOSRCDIR+= ../../../src/linux_asm
endif				

ifeq ($(VOTT), v7)
OBJS+= 
VOSRCDIR+= ../../../src/linux_asm
endif			
