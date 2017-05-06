# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
    ../../../code/

# please list all objects needed by your target here
OBJS:= jcommon.o jdecode.o jdHuffum.o jdidct.o  jheader.o \
       jtab.o mem_align.o mjpeg_front.o mjpegdec.o 
			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../../code/asm/arm_gnu/armv4
endif				

ifeq ($(VOTT), v6)
OBJS+= voIdctArmV6.o
VOSRCDIR+= ../../../code/asm/gnu_asm/ARMV6
endif				

ifeq ($(VOTT), v7)
OBJS+= JPEG_IDCT_ARMV7.o
VOSRCDIR+= ../../../code/asm/gnu_asm/ARMV7
endif			

ifeq ($(VOTT), v7s)
OBJS+= JPEG_IDCT_ARMV7.o
VOSRCDIR+= ../../../code/asm/gnu_asm/ARMV7
endif			
