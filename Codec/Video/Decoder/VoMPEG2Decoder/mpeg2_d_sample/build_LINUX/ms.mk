# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
    ../../../src/

# please list all objects needed by your target here
OBJS:= idct_add.o voMpeg2Decoder.o voMpeg2Front.o voMpeg2MB.o voMpeg2Parser.o voMpegBuf.o \
       voMpegMC.o voMpegMem.o voMpeg2Idct.o voMpegIdct.o
			
ifeq ($(VOTT), v4)
OBJS+= idct_arm9.o mpeg2_MC2_arm.o mpeg2_MC3_arm.o mpeg2_MC_arm.o
VOSRCDIR+= ../../../src/asm/arm_gnu/armv4
endif	

ifeq ($(VOTT), v5)
OBJS+= idct_arm9.o mpeg2_MC2_arm.o mpeg2_MC3_arm.o mpeg2_MC_arm.o
VOSRCDIR+= ../../../src/asm/arm_gnu/armv4
endif				

ifeq ($(VOTT), v6)
OBJS+= idct_arm.o mpeg2_MC2_arm.o mpeg2_MC3_arm.o mpeg2_MC_arm.o irdeto_idct_arm.o irdeto_sat_add.o voIdctArmV6_gnu.o
VOSRCDIR+= ../../../src/asm/arm_gnu/armv6
endif				

ifeq ($(VOTT), v7)
OBJS+= idct_arm_V7_ABCDEF.o mpeg4_MC_arm_V7.o mpeg4_MC_arm_V7_4x8.o irdeto_idct_arm_V7_ABCDEF.o irdeto_sat_add.o
VOSRCDIR+= ../../../src/asm/arm_gnu/armv7
endif		

ifeq ($(VOTT), v7s)
OBJS+= idct_arm_V7_ABCDEF.o mpeg4_MC_arm_V7.o mpeg4_MC_arm_V7_4x8.o irdeto_idct_arm_V7_ABCDEF.o irdeto_sat_add.o
VOSRCDIR+= ../../../src/asm/arm_gnu/armv7
endif		
