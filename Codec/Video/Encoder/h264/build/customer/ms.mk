
# please list all objects needed by your module here

OBJS:=voH264Dct.o\
voH264Deblock.o\
voH264Frame.o\
voH264EncBitStream.o\
voH264EncCAVLC.o\
voH264EncMB.o\
voH264EncME.o\
voH264EncPicSlice.o\
voH264EncRC.o\
voH264EncSAD.o\
voH264IntraPrediction.o\
voH264MC.o\
voH264Quant.o
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../../src \
	  ../../../../../../../../Include\

				
ifeq ($(VOTT), v4)
#OBJS+= voIdctArmV4_gnu.o voMpegMCArmV4V6_gnu.o voMPEG4EncQuant_gnu.o voMPEGFdctArmV4_gnu.o\
#	voMPEG4EncHalfPixelArmV4_gnu.o voMPEGSadArmV4_gnu.o
#VOSRCDIR+= ../../../../src/common/src/arm_asm\
#	   ../../../../src/lowlevel/arm_asm
endif				

ifeq ($(VOTT), v6)
#OBJS+= voIdctArmV6_gnu.o voMpegMCArmV4V6_gnu.o voMPEG4EncQuant_gnu.o voMPEGFdctArmV6_gnu.o\
#	voMPEG4EncHalfPixelArmV4_gnu.o voMPEG4EncHalfPixelArmV6_gnu.o voMPEGSadArmV6_gnu.o
#VOSRCDIR+= ../../../../src/common/src/arm_asm\
#	   ../../../../src/lowlevel/arm_asm
endif				

ifeq ($(VOTT), v7)
OBJS+= voH264Dct_ARMV7_gnu.o voH264EncSAD_ARMV7_gnu.o voH264IntraPrediction_ARMV7_gnu.o voH264Quant_ARMV7_gnu.o voH264Mc_ARMV7_gnu.o\
	voH264HPel_ARMV7_gnu.o voH264Deblock_ARMV7_gnu.o voH264Pic_ARMV7_gnu.o

VOSRCDIR+= ../../../../src/asm/armv7
endif	 
