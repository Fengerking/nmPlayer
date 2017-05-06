# please list all objects needed by your module here
VOMSRC:=voLog.c \
  biaridecod.c \
	block.c \
	cabac.c \
	context_ini.c \
	filehandle.c \
	fmo.c \
	header.c \
	image.c \
	intra16x16_pred.c \
	intra16x16_pred_normal.c \
	intra8x8_pred.c \
	intra8x8_pred_normal.c \
	intra4x4_pred.c \
	intra4x4_pred_normal.c \
	intra_chroma_pred.c \
	ldecod.c \
	loopFilter.c \
	loop_filter_normal.c \
	macroblock.c \
	mbuffer.c \
	mbuffer_mvc.c \
	mb_access.c \
	mb_prediction.c \
	mc_direct.c \
	mc_prediction.c \
	memalloc.c \
	mv_prediction.c \
	nal.c \
	nalu.c \
	nalucommon.c \
	output.c \
	parset.c \
	parsetcommon.c \
	quant.c \
	sei.c \
	transform.c \
	vlc.c \
	voH264Dec.c\
	frameThd.c \
	win32.c 

ifeq ($(VOTT), v6)
    VOMSRC +=Armv4GetBlockChromaNxNInBound.s Armv6Chroma_InPlaceDeblock.s Armv6GetBlockLumaNxNInBound.s Armv6InvTrans.s Armv6Luma_InPlaceDeblock.s Armv6_SIAVG.s
endif

ifeq ($(VOTT), v7)
   VOMSRC +=voH264Deblock_ARMV7_ios.s voH264DecDCT_ARMV7_ios.s  voH264Mc_ARMV7_ios.s  voH264IntraPrediction_ARMV7_ios.s voH264Mc_offset_ARMV7_ios.s
endif

ifeq ($(VOTT), v7s)
   VOMSRC +=voH264Deblock_ARMV7_ios.s voH264DecDCT_ARMV7_ios.s  voH264Mc_ARMV7_ios.s  voH264IntraPrediction_ARMV7_ios.s voH264Mc_offset_ARMV7_ios.s
endif

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../src \
             ../../../../../../../Include \
             ../../../../../../../Common 


ifeq ($(VOTT), v6)
   VOSRCDIR +=../../../../Sources/ARMv6/arm_mac 
endif

ifeq ($(VOTT), v7)
   VOSRCDIR +=../../../src/asm/armv7 
endif

ifeq ($(VOTT), v7s)
   VOSRCDIR +=../../../src/asm/armv7 
endif

