
# please list all objects needed by your module here

OBJS:=biaridecod.o \
block.o \
cabac.o \
context_ini.o \
decoder_test.o \
filehandle.o \
fmo.o \
header.o \
image.o \
intra16x16_pred.o \
intra16x16_pred_normal.o \
intra4x4_pred.o \
intra4x4_pred_normal.o \
intra8x8_pred.o \
intra8x8_pred_normal.o \
intra_chroma_pred.o \
ldecod.o \
loopFilter.o \
loop_filter_normal.o \
macroblock.o \
mbuffer.o \
mbuffer_mvc.o \
mb_access.o \
mb_prediction.o \
mc_direct.o \
mc_prediction.o \
memalloc.o \
mv_prediction.o \
nal.o \
nalu.o \
nalucommon.o \
output.o \
parset.o \
parsetcommon.o \
quant.o \
sei.o \
transform.o \
vlc.o \
voH264Dec.o\
win32.o 
#annexb.o \
#img_process.o \
#io_raw.o \
#io_tiff.o \
#input.o \
#img_io.o \
#erc_api.o \
#erc_do_i.o \
#erc_do_p.o \
#errorconcealment.o \
#leaky_bucket.o \
#transform8x8.o \
#blk_prediction.o \
#rtp.o \
#intra16x16_pred_mbaff.o \
#intra4x4_pred_mbaff.o \
#loop_filter_mbaff.o \

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../../src \
	  ../../../../../../../../Include\
#
#OBJS+= mpegvideo_arm.o \
#
#
#VOSRCDIR+= ../../../../src/arm
ifeq ($(VOTT), v7)
OBJS+= voH264Deblock_ARMV7_gnu.o voH264DecDCT_ARMV7_gnu.o voH264Mc_ARMV7_gnu.o voH264IntraPrediction_ARMV7_gnu.o
VOSRCDIR+= ../../../../src/asm/armv7
endif	

ifeq ($(VOTT), v7s)
OBJS+= voH264Deblock_ARMV7_gnu.o voH264DecDCT_ARMV7_gnu.o voH264Mc_ARMV7_gnu.o voH264IntraPrediction_ARMV7_gnu.o
VOSRCDIR+= ../../../../src/asm/armv7
endif	
