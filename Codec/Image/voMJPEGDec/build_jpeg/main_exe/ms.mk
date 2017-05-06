# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../Include \
../../testcode

# please list all objects needed by your target here
OBJS:= decode_wholeimage_jpeg.o
			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../../src/asm/gnu_asm/armv4
endif				

ifeq ($(VOTT), v6)
OBJS+= #deblock_armv6.o
VOSRCDIR+= ../../../src/asm/gnu_asm/armv6
endif				

ifeq ($(VOTT), v7)
OBJS+= 
VOSRCDIR+= ../../../src/asm/gnu_asm/armv7
endif			
