# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
    ../../../src/

# please list all objects needed by your target here
VOMSRC:= idct_add.c voMpeg2Decoder.c voMpeg2Front.c voMpeg2MB.c voMpeg2Parser.c voMpegBuf.c \
       voMpegMC.c voMpegMem.c voMpegIdct.c voMpeg2Idct.c

ifeq ($(VOTT), v6)
VOMSRC += idct_arm.s mpeg2_MC2_arm.s mpeg2_MC3_arm.s mpeg2_MC_arm.s irdeto_idct_arm.s irdeto_sat_add.s voIdctArmV6_mac.S
VOSRCDIR+= ../../../src/asm/arm_mac/armv6
endif				

ifeq ($(VOTT), v7)
VOMSRC += idct_arm_V7_ABCDEF.s mpeg4_MC_arm_V7.s mpeg4_MC_arm_V7_4x8.s irdeto_idct_arm_V7_ABCDEF.s irdeto_sat_add.s
VOSRCDIR+= ../../../src/asm/arm_mac/armv7
endif			

ifeq ($(VOTT), v7s)
VOMSRC += idct_arm_V7_ABCDEF.s mpeg4_MC_arm_V7.s mpeg4_MC_arm_V7_4x8.s irdeto_idct_arm_V7_ABCDEF.s irdeto_sat_add.s
VOSRCDIR+= ../../../src/asm/arm_mac/armv7
endif			
