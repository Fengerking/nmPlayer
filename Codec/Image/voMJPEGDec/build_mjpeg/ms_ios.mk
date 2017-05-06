# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../../Include \
    ../../../code

# please list all objects needed by your target here
VOMSRC:= jcommon.c jdecode.c jdHuffum.c jdidct.c  jheader.c \
       jtab.c mem_align.c mjpeg_front.c mjpegdec.c 
			
ifeq ($(VOTT), v4)
VOMSRC+= 
VOSRCDIR+= ../../../code/asm/ios_ios/armv4
endif				

ifeq ($(VOTT), v6)
VOMSRC+= voIdctArmV6.S
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV6
endif				

ifeq ($(VOTT), v7)
VOMSRC+= JPEG_IDCT_ARMV7.S voIdctArmV6.S
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV7
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV6
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= JPEG_IDCT_ARMV7.S voIdctArmV6.S
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV7
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV6
endif			
