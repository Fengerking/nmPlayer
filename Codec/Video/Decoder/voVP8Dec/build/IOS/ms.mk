# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../../../Include \
					  ../../../Inc \
    				../../../source

# please list all objects needed by your target here
VOMSRC:= voVP8Common.c voVP8DecBoolCoder.c voVP8DecFindNearMV.c voVP8DecFrame.c  voVP8DecFront.c \
       voVP8DecIdctDq.c voVP8DecInterPolationFilter.c voVP8DecMBlock.c voVP8DecMC.c \
       voVP8DecMV.c voVP8DecNormalFilters.c voVP8DecNormalLoopFilter.c voVP8Decoder.c voVP8DecReadBits.c \
       voVP8DecReadDctTokens.c voVP8DecReadMode.c voVP8DecReconInter.c voVP8DecReconIntra.c voVP8DecSimpleLoopFilters.c \
       voVP8Memory.c voVP8Thread.c
       
			
ifeq ($(VOTT), v4)
VOMSRC+= 
VOSRCDIR+= ../../../source/asm_ios
endif				

ifeq ($(VOTT), v6)
VOMSRC+= 
VOSRCDIR+= ../../../source/asm_ios
endif				

ifeq ($(VOTT), v7)
VOMSRC+= dc_only_idct_add_neon.S dequant_dc_idct_neon.S dequant_idct_neon.S dequantizeb_neon.S idct_dequant_0_2x_neon.S idct_dequant_dc_0_2x_neon.S \
idct_dequant_dc_full_2x_neon.S idct_dequant_full_2x_neon.S iwalsh_neon.S shortidct4x4llm_1_neon.S shortidct4x4llm_neon.S \
bilinearpredict16x16_neon.S bilinearpredict4x4_neon.S bilinearpredict8x4_neon.S bilinearpredict8x8_neon.S buildintrapredictorsmby_neon.S copymem16x16_neon.S \
copymem8x4_neon.S copymem8x8_neon.S sixtappredict16x16_neon.S sixtappredict4x4_neon.S sixtappredict8x4_neon.S sixtappredict8x8_neon.S vp8_predict_intra4x4.S \
loopfilter_neon.S loopfiltersimplehorizontaledge_neon.S loopfiltersimpleverticaledge_neon.S mbloopfilter_neon.S

VOSRCDIR+= ../../../source/asm_ios/armv7/idct \
  ../../../source/asm_ios/armv7/interpolation \
  ../../../source/asm_ios/armv7/lpfilter
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= dc_only_idct_add_neon.S dequant_dc_idct_neon.S dequant_idct_neon.S dequantizeb_neon.S idct_dequant_0_2x_neon.S idct_dequant_dc_0_2x_neon.S \
idct_dequant_dc_full_2x_neon.S idct_dequant_full_2x_neon.S iwalsh_neon.S shortidct4x4llm_1_neon.S shortidct4x4llm_neon.S \
bilinearpredict16x16_neon.S bilinearpredict4x4_neon.S bilinearpredict8x4_neon.S bilinearpredict8x8_neon.S buildintrapredictorsmby_neon.S copymem16x16_neon.S \
copymem8x4_neon.S copymem8x8_neon.S sixtappredict16x16_neon.S sixtappredict4x4_neon.S sixtappredict8x4_neon.S sixtappredict8x8_neon.S vp8_predict_intra4x4.S \
loopfilter_neon.S loopfiltersimplehorizontaledge_neon.S loopfiltersimpleverticaledge_neon.S mbloopfilter_neon.S

VOSRCDIR+= ../../../source/asm_ios/armv7/idct \
  ../../../source/asm_ios/armv7/interpolation \
  ../../../source/asm_ios/armv7/lpfilter
endif		