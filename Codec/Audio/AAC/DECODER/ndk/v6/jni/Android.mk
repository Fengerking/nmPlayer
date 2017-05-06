LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoAACDec

CMN_SRC:=../../../../../../../Common
CMN_CODEC:=../../../../../Common
MOD_SRC:=../../../source
PS_SRC:=$(MOD_SRC)/PS
SBR_SRC:=$(MOD_SRC)/sbr
BSAC_SRC:=$(MOD_SRC)/BSAC
ASM_SRC:=$(MOD_SRC)/linuxasm/
ASM_SRC6:=$(MOD_SRC)/linuxasm/armv6
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
	$(SBR_SRC)/sbr_utility.c \
	$(ASM_SRC)/qmf.S \
	$(ASM_SRC6)/PostMultiply_V6.S \
	$(ASM_SRC6)/PreMultiply_V6.S \
	$(ASM_SRC6)/R4_Core_v6.S \
	$(ASM_SRC6)/R8FirstPass_v6.S \
	$(ASM_SRC6)/writePCM_ARMV6.S 


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../Common \
	../../../include \
	../../../../../Common


VOMM:=-DARM -DARMV6 -DLCHECK -DLINUX -D_ARM_VER -D_LINUX_ANDROID -D__VO_NDK__ #-D_VOLOG_INFO 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02210000  -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char

LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a -llog 

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

