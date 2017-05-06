
# please list all objects needed by your module here

VOMSRC:=voDivx3Haff.c voDivx3MB.c voDivx3Parser.c voDivx3Tab.c voMpeg4ACDC.c voMpeg4DecFront.c voMpeg4Decoder.c voMpeg4Gmc.c \
			voMpeg4Haff.c voMpeg4MB.c voMpeg4Parser.c voMpeg4Postprocess.c voMpeg4Rvlc.c voMpegBuf.c voMpegIdct.c \
			voMpegMC.c voMpeg4MCQpel.c voMpegMem.c voMpegReadbits.c voMpeg4Frame.c			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../src\
	  ../../../src/front\
	  ../../../../../../../Include\

# asm objects and directories.

ifeq ($(VOTT), v6)
VOMSRC+= voIdctArmV6_mac.s voMpegMCArmV4V6_mac.s voMpegFillEdgesArmV6_mac.s voMpeg4MCQpelArmV6_mac.s voMpeg4MCQpelAddArmV6_mac.s
VOSRCDIR+= ../../../src/asm/mac

endif

ifeq ($(VOTT), v7)
VOMSRC+= voIdctArmV6_mac.s voMpegMCArmV7_mac.s voMpegFillEdgesArmV6_mac.s voMpeg4MCQpelArmV6_mac.s voMpeg4MCQpelAddArmV6_mac.s
VOSRCDIR+= ../../../src/asm/mac/ARMV7 \
	../../../src/asm/mac

endif

ifeq ($(VOTT), v7s)
VOMSRC+= voIdctArmV6_mac.s voMpegMCArmV7_mac.s voMpegFillEdgesArmV6_mac.s voMpeg4MCQpelArmV6_mac.s voMpeg4MCQpelAddArmV6_mac.s
VOSRCDIR+= ../../../src/asm/mac/ARMV7 \
	../../../src/asm/mac

endif


