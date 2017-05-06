LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoRADec

CMN_SRC:=../../../../../../../Common
CMN_CODEC:=../../../../../Common
RA_INT:=../../../../src/interface
RA8LBR_INT:=../../../../src/ra8lbr
RA8LBR_SRC:=../../../../src/ra8lbr/decoder
RA8LBR_ASM_ARMV7:=$(RA8LBR_SRC)/asm/linux/Neon
AAC_INT:=../../../../src/aac
AAC_SRC:=../../../../../AAC/DECODER/source
PS_SRC:=$(AAC_SRC)/PS
SBR_SRC:=$(AAC_SRC)/sbr
BSAC_SRC:=$(AAC_SRC)/BSAC
AAC_ASM_SRC:=$(AAC_SRC)/linuxasm/
AAC_ASM_SRC7:=$(AAC_SRC)/linuxasm/armv7
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/voLog.c \
	$(CMN_CODEC)/cmnAudioMemory.c \
	$(CMN_SRC)/cmnMemory.c \
	$(RA_INT)/ra_decode.c \
	$(RA_INT)/voRABUFDec.c \
	$(RA_INT)/voRADecSDK.c \
	$(RA_INT)/voRARawDec.c \
	$(RA_INT)/voRARMDec.c \
	$(RA8LBR_INT)/ra8lbr_decode.c \
	$(RA8LBR_SRC)/bitpack.c \
	$(RA8LBR_SRC)/buffers.c \
	$(RA8LBR_SRC)/category.c \
	$(RA8LBR_SRC)/couple.c \
	$(RA8LBR_SRC)/envelope.c \
	$(RA8LBR_SRC)/fft.c \
	$(RA8LBR_SRC)/gainctrl.c \
	$(RA8LBR_SRC)/gecko2codec.c \
	$(RA8LBR_SRC)/huffman.c \
	$(RA8LBR_SRC)/hufftabs.c \
	$(RA8LBR_SRC)/mlt.c \
	$(RA8LBR_SRC)/sqvh.c \
	$(RA8LBR_SRC)/trigtabs.c \
	$(RA8LBR_ASM_ARMV7)/DecWindowNoAttacks_v7.S \
	$(RA8LBR_ASM_ARMV7)/RA_PostMultiply_V7.S \
	$(RA8LBR_ASM_ARMV7)/RA_PreMultiply_V7.S \
	$(RA8LBR_ASM_ARMV7)/RA_R4_Core_V7.S \
	$(RA8LBR_ASM_ARMV7)/RA_R8FirstPass_v7.S \
	$(AAC_INT)/aac_decode.c \
	$(AAC_INT)/aac_bitstream.c \
	$(AAC_INT)/ga_config.c \
	$(AAC_SRC)/bitstream.c \
	$(AAC_SRC)/decframe.c \
	$(AAC_SRC)/decoder.c \
	$(AAC_SRC)/downMatrix.c \
	$(AAC_SRC)/latmheader.c \
	$(AAC_SRC)/lc_dequant.c \
	$(AAC_SRC)/lc_huff.c \
	$(AAC_SRC)/lc_hufftab.c \
	$(AAC_SRC)/lc_imdct.c \
	$(AAC_SRC)/lc_mdct.c \
	$(AAC_SRC)/lc_pns.c \
	$(AAC_SRC)/ic_predict.c \
	$(AAC_SRC)/lc_stereo.c \
	$(AAC_SRC)/lc_syntax.c \
	$(AAC_SRC)/lc_tns.c \
	$(AAC_SRC)/ltp_dec.c \
	$(AAC_SRC)/Header.c \
	$(AAC_SRC)/table.c \
	$(AAC_SRC)/unit.c \
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
	$(AAC_ASM_SRC)/qmf.S \
	$(AAC_ASM_SRC7)/PostMultiply_V7.S \
	$(AAC_ASM_SRC7)/PreMultiply_V7.S \
	$(AAC_ASM_SRC7)/R4_Core_V7.S \
	$(AAC_ASM_SRC7)/R8FirstPass_v7.S \
	$(AAC_ASM_SRC7)/writePCM_ARMV6.S \
	$(AAC_ASM_SRC7)/writePCM_ARMV7.S \
	$(AAC_ASM_SRC7)/WinLong_V7.S


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../Common \
	../../../../../AAC/DECODER/include \
	../../../../include \
	../../../../src/ra8lbr \
	../../../../src/interface \
	../../../../src/aac \
	../../../../../AAC/DECODER/source \
	../../../../src/ra8lbr/decoder \
	../../../../../Common


VOMM:=-DARM -DARMV4 -DARMV6 -DARMV7 -DLCHECK -DLINUX

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02240000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon

LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

