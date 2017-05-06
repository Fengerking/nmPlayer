BRANCH_INCLUDE_PATH:=../../../../../../../Include
BRANCH_COMMON_PATH:=../../../../../../../Common
CMNSRC_PATH:=../../../src

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
VOMSRC:=\
	$(BRANCH_COMMON_PATH)/voLog.c\
	$(CMNSRC_PATH)/biaridecod.c \
	$(CMNSRC_PATH)/block.c \
	$(CMNSRC_PATH)/cabac.c \
	$(CMNSRC_PATH)/context_ini.c \
	$(CMNSRC_PATH)/filehandle.c \
	$(CMNSRC_PATH)/fmo.c \
	$(CMNSRC_PATH)/header.c \
	$(CMNSRC_PATH)/image.c \
	$(CMNSRC_PATH)/intra16x16_pred.c \
	$(CMNSRC_PATH)/intra16x16_pred_normal.c \
	$(CMNSRC_PATH)/intra8x8_pred.c \
	$(CMNSRC_PATH)/intra8x8_pred_normal.c \
	$(CMNSRC_PATH)/intra4x4_pred.c \
	$(CMNSRC_PATH)/intra4x4_pred_normal.c \
	$(CMNSRC_PATH)/intra_chroma_pred.c \
	$(CMNSRC_PATH)/ldecod.c \
	$(CMNSRC_PATH)/loopFilter.c \
	$(CMNSRC_PATH)/loop_filter_normal.c \
	$(CMNSRC_PATH)/macroblock.c \
	$(CMNSRC_PATH)/mbuffer.c \
	$(CMNSRC_PATH)/mbuffer_mvc.c \
	$(CMNSRC_PATH)/mb_access.c \
	$(CMNSRC_PATH)/mb_prediction.c \
	$(CMNSRC_PATH)/mc_direct.c \
	$(CMNSRC_PATH)/mc_prediction.c \
	$(CMNSRC_PATH)/memalloc.c \
	$(CMNSRC_PATH)/mv_prediction.c \
	$(CMNSRC_PATH)/nal.c \
	$(CMNSRC_PATH)/nalu.c \
	$(CMNSRC_PATH)/nalucommon.c \
	$(CMNSRC_PATH)/output.c \
	$(CMNSRC_PATH)/parset.c \
	$(CMNSRC_PATH)/parsetcommon.c \
	$(CMNSRC_PATH)/quant.c \
	$(CMNSRC_PATH)/sei.c \
	$(CMNSRC_PATH)/transform.c \
	$(CMNSRC_PATH)/vlc.c \
	$(CMNSRC_PATH)/voH264Dec.c\
	$(CMNSRC_PATH)/frameThd.c \
	$(CMNSRC_PATH)/win32.c\
	$(CMNSRC_PATH)/asm/x86/voH264Inc.asm\
	$(CMNSRC_PATH)/asm/x86/voH264Util.asm\
	$(CMNSRC_PATH)/asm/x86/voH264ChromaMC.asm\
	$(CMNSRC_PATH)/asm/x86/voH264Deblock.asm\
	$(CMNSRC_PATH)/asm/x86/voH264Fpel.asm\
	$(CMNSRC_PATH)/asm/x86/voH264Qpel.asm\
	$(CMNSRC_PATH)/asm/x86/voH264Qpel8bit.asm\
	$(CMNSRC_PATH)/asm/x86/voH264Weight.asm\
	$(CMNSRC_PATH)/asm/x86/voH264IDCT.asm

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=$(BRANCH_INCLUDE_PATH) \
			$(BRANCH_COMMON_PATH) \
			$(CMNSRC_PATH)/asm/x86 \
    		$(CMNSRC_PATH)