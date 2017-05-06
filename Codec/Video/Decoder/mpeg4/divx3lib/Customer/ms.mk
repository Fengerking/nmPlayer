
# please list all objects needed by your module here

OBJS:=voDivx3Haff.o voDivx3MB.o voDivx3Parser.o voDivx3Tab.o voMpeg4ACDC.o voMpeg4DecFront.o voMpeg4Decoder.o voMpeg4Gmc.o \
			voMpeg4Haff.o voMpeg4MB.o voMpeg4Parser.o voMpeg4Postprocess.o voMpeg4Rvlc.o voMpegBuf.o voMpegIdct.o\
			voMpegMC.o voMpeg4MCQpel.o voMpegMem.o voMpegReadbits.o voMpeg4Frame.o
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../../src \
	  ../../../../src/front\
	  ../../../../../../../../Include\

				
ifeq ($(VOTT), v4)
OBJS+= voIdctArmV4_gnu.o voMpegMCArmV4V6_gnu.o 
VOSRCDIR+= ../../../../src/asm/arm

endif				

ifeq ($(VOTT), v6)
OBJS+= voIdctArmV6_gnu.o voMpegMCArmV4V6_gnu.o voMpegFillEdgesArmV6_gnu.o voMpeg4MCQpelArmV6_gnu.o voMpeg4MCQpelAddArmV6_gnu.o
VOSRCDIR+= ../../../../src/asm/arm

endif				

ifeq ($(VOTT), v7)
OBJS+= voIdctArmV7_gnu.o voMpegMCArmV7_gnu.o voMpegFillEdgesArmV6_gnu.o voMpeg4MCQpelArmV6_gnu.o voMpeg4MCQpelAddArmV6_gnu.o
VOSRCDIR+= ../../../../src/asm/arm/ARMV7 \
		../../../../src/asm/arm
		

endif	

ifeq ($(VOTT), v7s)
OBJS+= voIdctArmV7_gnu.o voMpegMCArmV7_gnu.o voMpegFillEdgesArmV6_gnu.o voMpeg4MCQpelArmV6_gnu.o voMpeg4MCQpelAddArmV6_gnu.o
VOSRCDIR+= ../../../../src/asm/arm/ARMV7 \
		../../../../src/asm/arm
		

endif	
VOSRCNO:=9666