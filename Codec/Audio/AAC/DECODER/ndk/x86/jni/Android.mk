LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoAACDec

CMN_SRC:=../../../../../../../Common
CMN_CODEC:=../../../../../Common
MOD_SRC:=../../../source
PS_SRC:=$(MOD_SRC)/PS
SBR_SRC:=$(MOD_SRC)/sbr
BSAC_SRC:=$(MOD_SRC)/BSAC
LOCAL_SRC_FILES := \
	\
	$(CMN_CODEC)/cmnAudioMemory.c \
	$(CMN_SRC)/voLog.c \
	$(MOD_SRC)/bitstream.c \
	$(MOD_SRC)/decframe.c \
	$(MOD_SRC)/decoder.c \
	$(MOD_SRC)/downMatrix.c \
	$(MOD_SRC)/latmheader.c \
	$(MOD_SRC)/lc_dequant.c \
	$(MOD_SRC)/lc_huff.c \
	$(MOD_SRC)/lc_hufftab.c \
	$(MOD_SRC)/lc_imdct.c \
	$(MOD_SRC)/lc_mdct.c \
	$(MOD_SRC)/lc_pns.c \
	$(MOD_SRC)/lc_stereo.c \
	$(MOD_SRC)/lc_syntax.c \
	$(MOD_SRC)/lc_tns.c \
	$(MOD_SRC)/ltp_dec.c \
	$(MOD_SRC)/ic_predict.c \
	$(MOD_SRC)/Header.c \
	$(MOD_SRC)/table.c \
	$(MOD_SRC)/unit.c \
	$(BSAC_SRC)/bsac_dec_spectra.c \
	$(BSAC_SRC)/decode_bsac.c \
	$(BSAC_SRC)/sam_decode_bsac.c \
	$(BSAC_SRC)/sam_fadecode.c \
	$(PS_SRC)/ps_dec.c \
	$(PS_SRC)/ps_syntax.c \
	$(SBR_SRC)/sbr_dec.c \
	$(SBR_SRC)/sbr_hfadj.c \
	$(SBR_SRC)/sbr_hfgen.c \
	$(SBR_SRC)/sbr_huff.c \
	$(SBR_SRC)/sbr_qmf.c \
	$(SBR_SRC)/sbr_syntax.c \
	$(SBR_SRC)/sbr_utility.c


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../Common \
	../../../include \
	../../../../../Common


VOMM:= -DLCHECK -DLINUX -D_LINUX_ANDROID -D__VO_NDK__ #-D_VOLOG_INFO


# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02210000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -fsigned-char

LOCAL_LDLIBS:=../../../../../../../Lib/ndk/x86/libvoCheck.a -llog 

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

