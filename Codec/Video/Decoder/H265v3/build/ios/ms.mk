# please list all objects needed by your module here
VOMSRC:=h265dec_cabac.c \
h265dec_deblock.c \
h265dec_debug.c \
h265dec_dequant.c \
h265dec_frameThd.c \
h265dec_wave3d.c \
h265dec_front.c \
h265dec_idct.c \
h265dec_inter.c \
h265dec_list.c \
h265dec_mem.c \
h265dec_mvpred.c \
h265dec_parser.c \
h265dec_recon.c \
h265dec_sao.c \
h265dec_sbac.c \
h265dec_slice.c \
h265dec_vlc.c \
h265_decoder.c \
HMLog.c


ifeq ($(VOTT), v6)
   
endif

ifeq ($(VOTT), v7)
   VOMSRC +=const.S h265_Deblock_GNU.S \
H265_IntraPred_Angular_GNU.S \
IDCT16X16_4X4_GNU.S \
IDCT16X16_GNU.S \
IDCT32X32_4X4_GNU.S \
IDCT32X32_GNU.S \
IDCT4X4_GNU.S \
IDCT8X8_GNU.S \
IDCT_NZ_1X1_GNU.S \
IDST4X4_1X1_GNU.S \
IDST4X4_GNU.S \
h265_sao_GNU.S \
new_luma_mc_neon_gnu_ios.S \
new_chroma_mc_neon_gnu_ios.S
endif

ifeq ($(VOTT), v7s)
   VOMSRC +=const.S h265_Deblock_GNU.S \
H265_IntraPred_Angular_GNU.S \
IDCT16X16_4X4_GNU.S \
IDCT16X16_GNU.S \
IDCT32X32_4X4_GNU.S \
IDCT32X32_GNU.S \
IDCT4X4_GNU.S \
IDCT8X8_GNU.S \
IDCT_NZ_1X1_GNU.S \
IDST4X4_1X1_GNU.S \
IDST4X4_GNU.S \
h265_sao_GNU.S \
new_luma_mc_neon_gnu_ios.S \
new_chroma_mc_neon_gnu_ios.S
endif

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../src \
             ../../../../../../../Include \
             ../../../../../../../Common 


ifeq ($(VOTT), v6)
    
endif

ifeq ($(VOTT), v7)
   VOSRCDIR +=../../../src/asm/armv7/ios
endif

ifeq ($(VOTT), v7s)
   VOSRCDIR +=../../../src/asm/armv7/ios
endif

