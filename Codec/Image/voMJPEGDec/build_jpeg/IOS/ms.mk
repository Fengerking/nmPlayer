# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../../Include \
    ../../../code/

# please list all objects needed by your target here
VOMSRC:= jcommon.c jdecode.c jdHuffum.c jdidct.c jexif.c jheader.c \
       jtab.c mem_align.c JPEG_D_API.c 
			
ifeq ($(VOTT), v4)
VOMSRC+= 
VOSRCDIR+= ../../../code/asm/arm_gnu/armv4
endif				

ifeq ($(VOTT), v6)
VOMSRC+= voIdctArmV6.S
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV6
endif				

ifeq ($(VOTT), v7)
VOMSRC+= voIdctArmV6.S
VOMSRC+= JPEG_IDCT_ARMV7.S
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV7
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV6
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= voIdctArmV6.S
VOMSRC+= JPEG_IDCT_ARMV7.S
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV7
VOSRCDIR+= ../../../code/asm/ios_asm/ARMV6
endif			
