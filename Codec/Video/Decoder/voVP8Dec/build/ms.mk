# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../../Include \
					  ../../Inc \
    				../../source

# please list all objects needed by your target here
OBJS:= voVP8Common.o voVP8DecBoolCoder.o voVP8DecFindNearMV.o voVP8DecFrame.o  voVP8DecFront.o \
       voVP8DecIdctDq.o voVP8DecInterPolationFilter.o voVP8DecMBlock.o voVP8DecMC.o \
       voVP8DecMV.o voVP8DecNormalFilters.o voVP8DecNormalLoopFilter.o voVP8Decoder.o voVP8DecReadBits.o \
       voVP8DecReadDctTokens.o voVP8DecReadMode.o voVP8DecReconInter.o voVP8DecReconIntra.o voVP8DecSimpleLoopFilters.o \
       voVP8Memory.o voVP8Thread.o
       
			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../source/asm_gnu
endif				

ifeq ($(VOTT), v6)
OBJS+= 
VOSRCDIR+= ../../source/asm_gnu
endif				

ifeq ($(VOTT), v7)
OBJS+= dc_only_idct_add_neon.o dequant_dc_idct_neon.o dequant_idct_neon.o dequantizeb_neon.o idct_dequant_0_2x_neon.o idct_dequant_dc_0_2x_neon.o \
idct_dequant_dc_full_2x_neon.o idct_dequant_full_2x_neon.o iwalsh_neon.o shortidct4x4llm_1_neon.o shortidct4x4llm_neon.o \
bilinearpredict16x16_neon.o bilinearpredict4x4_neon.o bilinearpredict8x4_neon.o bilinearpredict8x8_neon.o buildintrapredictorsmby_neon.o copymem16x16_neon.o \
copymem8x4_neon.o copymem8x8_neon.o sixtappredict16x16_neon.o sixtappredict4x4_neon.o sixtappredict8x4_neon.o sixtappredict8x8_neon.o vp8_predict_intra4x4.o \
loopfilter_neon.o loopfiltersimplehorizontaledge_neon.o loopfiltersimpleverticaledge_neon.o mbloopfilter_neon.o

VOSRCDIR+= ../../source/asm_gnu/armv7/idct \
  ../../source/asm_gnu/armv7/interpolation \
  ../../source/asm_gnu/armv7/lpfilter
endif			

ifeq ($(VOTT), v7s)
OBJS+= dc_only_idct_add_neon.o dequant_dc_idct_neon.o dequant_idct_neon.o dequantizeb_neon.o idct_dequant_0_2x_neon.o idct_dequant_dc_0_2x_neon.o \
idct_dequant_dc_full_2x_neon.o idct_dequant_full_2x_neon.o iwalsh_neon.o shortidct4x4llm_1_neon.o shortidct4x4llm_neon.o \
bilinearpredict16x16_neon.o bilinearpredict4x4_neon.o bilinearpredict8x4_neon.o bilinearpredict8x8_neon.o buildintrapredictorsmby_neon.o copymem16x16_neon.o \
copymem8x4_neon.o copymem8x8_neon.o sixtappredict16x16_neon.o sixtappredict4x4_neon.o sixtappredict8x4_neon.o sixtappredict8x8_neon.o vp8_predict_intra4x4.o \
loopfilter_neon.o loopfiltersimplehorizontaledge_neon.o loopfiltersimpleverticaledge_neon.o mbloopfilter_neon.o

VOSRCDIR+= ../../source/asm_gnu/armv7/idct \
  ../../source/asm_gnu/armv7/interpolation \
  ../../source/asm_gnu/armv7/lpfilter
endif	