BRANCH_INCLUDE_PATH:=../../../../../../Include
BRANCH_COMMON_PATH:=../../../../../../Common
AUDIO_COMMON_PATH:=../../../../Common
INC_PATH:=../../include
CMNSRC_PATH:=../../source

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
VOMSRC:=\
	$(BRANCH_COMMON_PATH)/voLog.c \
	$(AUDIO_COMMON_PATH)/cmnAudioMemory.c\
	$(CMNSRC_PATH)/bitstream.c \
	$(CMNSRC_PATH)/decframe.c \
	$(CMNSRC_PATH)/downMatrix.c \
	$(CMNSRC_PATH)/decoder.c \
	$(CMNSRC_PATH)/Header.c \
	$(CMNSRC_PATH)/ic_predict.c \
	$(CMNSRC_PATH)/latmheader.c \
	$(CMNSRC_PATH)/lc_dequant.c \
	$(CMNSRC_PATH)/lc_huff.c \
	$(CMNSRC_PATH)/lc_hufftab.c \
	$(CMNSRC_PATH)/lc_imdct.c \
	$(CMNSRC_PATH)/lc_mdct.c \
	$(CMNSRC_PATH)/lc_pns.c \
	$(CMNSRC_PATH)/lc_stereo.c \
	$(CMNSRC_PATH)/lc_syntax.c \
	$(CMNSRC_PATH)/lc_tns.c \
	$(CMNSRC_PATH)/ltp_dec.c \
	$(CMNSRC_PATH)/table.c \
	$(CMNSRC_PATH)/unit.c\
	$(CMNSRC_PATH)/sbr/sbr_dec.c\
	$(CMNSRC_PATH)/sbr/sbr_hfadj.c\
	$(CMNSRC_PATH)/sbr/sbr_hfgen.c\
	$(CMNSRC_PATH)/sbr/sbr_huff.c\
	$(CMNSRC_PATH)/sbr/sbr_qmf.c\
	$(CMNSRC_PATH)/sbr/sbr_syntax.c\
	$(CMNSRC_PATH)/sbr/sbr_utility.c\
	$(CMNSRC_PATH)/BSAC/bsac_dec_spectra.c\
	$(CMNSRC_PATH)/BSAC/decode_bsac.c\
	$(CMNSRC_PATH)/BSAC/sam_decode_bsac.c\
	$(CMNSRC_PATH)/BSAC/sam_fadecode.c\
	$(CMNSRC_PATH)/PS/ps_dec.c\
	$(CMNSRC_PATH)/PS/ps_syntax.c
	
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=$(BRANCH_COMMON_PATH) \
			$(BRANCH_INCLUDE_PATH) \
			$(AUDIO_COMMON_PATH)\
			$(INC_PATH) \
			$(CMNSRC_PATH) \
			$(CMNSRC_PATH)/sbr\
			$(CMNSRC_PATH)/BSAC\
			$(CMNSRC_PATH)/PS
    			