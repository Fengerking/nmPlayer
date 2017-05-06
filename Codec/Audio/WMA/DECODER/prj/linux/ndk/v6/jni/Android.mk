LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoWMADec

CMN_SRC:=../../../../../../../../../Common
MTX_SRC:=../../../../../src/chanmtx
WCMN_SRC:=../../../../../src/common
DEC_SRC:=../../../../../src/decoder
FMT_SRC:=../../../../../src/fmthlpr
PCM_SRC:=../../../../../src/pcmfmt
PRI_SRC:=../../../../../src/primitive
ASM_SRC:=../../../../../asm/linuxasm
ASM_SRC6:=../../../../../asm/linuxasm/armv6
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MTX_SRC)/wmachmtx.c \
	$(MTX_SRC)/wmabuffilt.c \
	$(MTX_SRC)/wmaltrt.c \
	$(WCMN_SRC)/AutoProfile.c \
	$(WCMN_SRC)/basepluspro.c \
	$(WCMN_SRC)/chexpro.c \
	$(WCMN_SRC)/configcommon.c \
	$(WCMN_SRC)/dectables.c \
	$(WCMN_SRC)/dectablespro.c \
	$(WCMN_SRC)/dectablesstd.c \
	$(WCMN_SRC)/dectablesstdpro.c \
	$(WCMN_SRC)/drccommonpro.c \
	$(WCMN_SRC)/downmix.c \
	$(WCMN_SRC)/fex.c \
	$(WCMN_SRC)/fft.c \
	$(WCMN_SRC)/float.c \
	$(WCMN_SRC)/lowrate_common.c \
	$(WCMN_SRC)/lowrate_commonstd.c \
	$(WCMN_SRC)/lpc.c \
	$(WCMN_SRC)/lpclsl.c \
	$(WCMN_SRC)/lpcprolsl.c \
	$(WCMN_SRC)/lpcstd.c \
	$(WCMN_SRC)/msaudio.c \
	$(WCMN_SRC)/msaudiolsl.c \
	$(WCMN_SRC)/msaudiopro.c \
	$(WCMN_SRC)/msaudiostd.c \
	$(WCMN_SRC)/msaudiostdpro.c \
	$(WCMN_SRC)/msaudiotemplate.c \
	$(WCMN_SRC)/msaudioprotemplate.c \
	$(DEC_SRC)/arm_stub.c \
	$(DEC_SRC)/baseplusdecpro.c \
	$(DEC_SRC)/chexdecpro.c \
	$(DEC_SRC)/entropydec.c \
	$(DEC_SRC)/entropydecpro.c \
	$(DEC_SRC)/entropydecprolsl.c \
	$(DEC_SRC)/entropydecstd.c \
	$(DEC_SRC)/fexdec.c \
	$(DEC_SRC)/huffdec.c \
	$(DEC_SRC)/losslessdeclsl.c \
	$(DEC_SRC)/losslessdecpro.c \
	$(DEC_SRC)/losslessdecprolsl.c \
	$(DEC_SRC)/msaudiodec.c \
	$(DEC_SRC)/msaudiodectemplate.c \
	$(DEC_SRC)/strmdec_wma.c \
	$(DEC_SRC)/wmadec.c \
	$(DEC_SRC)/wmaprodecS_api.c \
	$(FMT_SRC)/wmafmt.c \
	$(PRI_SRC)/commontables.c \
	$(PRI_SRC)/wmaerror.c \
	$(PRI_SRC)/wmamalloc.c \
	$(ASM_SRC)/huffdec_arm.S \
	$(ASM_SRC)/losslessdeclsl_arm.S \
	$(ASM_SRC)/losslessdecprolsl_arm.S \
	$(ASM_SRC)/lowrate_arm.S \
	$(ASM_SRC)/lpcstd_arm.S \
	$(ASM_SRC)/lpclsl_arm.S \
	$(ASM_SRC)/msaudiodec_arm.S \
	$(ASM_SRC)/strmdec_arm.S \
	$(ASM_SRC)/voMemory_arm.S \
	$(ASM_SRC6)/fft_arm_v6.S \
	$(ASM_SRC6)/msaudio_arm_v6.S \
	$(ASM_SRC6)/msaudiopro_arm_v6.S \
	$(ASM_SRC6)/msaudiostdpro_arm_v6.S 


LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	../../../../../../../../../Common \
	../../../../../src/common \
	../../../../../inc/audio/common/include \
	../../../../../inc/audio/v10/include \
	../../../../../inc/audio/v10/common \
	../../../../../inc/audio/v10/asfparse \
	../../../../../inc/audio/v10/decoder \
	../../../../../inc/audio/v10/win32 \
	../../../../../inc/common/include \
	../../../../../inc/common/logging \
	../../../../../asm/linuxasm \
	../../../../../asm/linuxasm/armv6
	

VOMM:=-DLINUX -DARM -DARMV6 -D_ARM_ -DBUILD_WITHOUT_C_LIB -DBUILD_INTEGER -DDISABLE_UES -DENABLE_LPC -DENABLE_ALL_ENCOPT -DWMA_DECPK_BUILD 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02230000   -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char -msoft-float

LOCAL_LDLIBS:=../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

