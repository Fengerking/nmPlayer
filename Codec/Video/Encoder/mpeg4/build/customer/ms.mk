
# please list all objects needed by your module here

OBJS:=voMpegMC.o voMpegIdct.o voMpegEncTable.o voMpegEncFastDct.o voMpegEncHPInterPolate.o voMpegEncPreProcess.o \
			voMpegEncQuant.o voMpegEncSad.o voH263EncHuffman.o voMpegEncME.o voMpegEncMEMethod.o voMpegEncRC.o voMpegEncBits.o\
			voMpegEncFrame.o voMpegEncFrameProc.o voMpegEncMB.o voMpegEncPrediction.o voMpegMemmory.o voMpegEncFront.o
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../../src \
	  ../../../../src/midlevel\
	  ../../../../src/midlevel/h263\
	  ../../../../src/midlevel/motionestimation\
	  ../../../../src/midlevel/ratecontrol\
	  ../../../../src/lowlevel\
	  ../../../../src/common/src\
	  ../../../../../../../../Include\

				
ifeq ($(VOTT), v4)
OBJS+= voIdctArmV4_gnu.o voMpegMCArmV4V6_gnu.o voMpegEncQuant_gnu.o voMpegFdctArmV4_gnu.o\
	voMpegEncHalfPixelArmV4_gnu.o voMpegSadArmV4_gnu.o
VOSRCDIR+= ../../../../src/common/src/arm_asm\
	   ../../../../src/lowlevel/arm_asm
endif				

ifeq ($(VOTT), v6)
OBJS+= voIdctArmV6_gnu.o voMpegMCArmV4V6_gnu.o voMpegEncQuant_gnu.o voMpegFdctArmV6_gnu.o\
	voMpegEncHalfPixelArmV4_gnu.o voMpegEncHalfPixelArmV6_gnu.o voMpegSadArmV6_gnu.o
VOSRCDIR+= ../../../../src/common/src/arm_asm\
	   ../../../../src/lowlevel/arm_asm
endif				

ifeq ($(VOTT), v7)
OBJS+= voIdctArmV6_gnu.o voMpegIdctArmV7_gnu.o voMpegMCArmV7_gnu.o voMpegEncHalfPixelArmV7_gnu.o \
		voMpegFastdctArmV7_gnu.o voMpegQuantArmV7_gnu.o voMpegSadArmV7_gnu.o
VOSRCDIR+= ../../../../src/common/src/arm_asm\
     ../../../../src/lowlevel/arm_asm
endif	 
