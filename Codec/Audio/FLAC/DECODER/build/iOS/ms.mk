# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc \
          ../../../src/c_src \
	  ../../../../../../../Common 

# please list all objects needed by your target here
VOMSRC:= bit.c frame.c mem_align.c stream.c subframe.c voFLACDec.c #cmnMemory.c

			
ifeq ($(VOTT), v4)
VOMSRC+= 
VOSRCDIR+= ../../../src/linuxasm/armv4
endif				

ifeq ($(VOTT), v6)
VOMSRC+= 
VOSRCDIR+= ../../../src/linux_asm
endif				

ifeq ($(VOTT), v7)
VOMSRC+= 
VOSRCDIR+= ../../../src/linux_asm
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= 
VOSRCDIR+= ../../../src/linux_asm
endif			
