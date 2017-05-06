LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoAACEnc

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src
MOD_BASSRC:=../../../../basic_op
ASM_SRC6:=$(MOD_SRC)/linuxasm/ARMV5E/
ASM_SRC7:=$(MOD_SRC)/linuxasm/ARMV7Neon/
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_BASSRC)/basicop2.c \
	$(MOD_BASSRC)/oper_32b.c \
	$(MOD_SRC)/aac_ram.c \
	$(MOD_SRC)/aac_rom.c \
	$(MOD_SRC)/aacenc.c \
	$(MOD_SRC)/aacenc_core.c \
	$(MOD_SRC)/adj_thr.c \
	$(MOD_SRC)/band_nrg.c \
	$(MOD_SRC)/bit_cnt.c \
	$(MOD_SRC)/bitbuffer.c \
	$(MOD_SRC)/bitenc.c \
	$(MOD_SRC)/block_switch.c \
	$(MOD_SRC)/channel_map.c \
	$(MOD_SRC)/dyn_bits.c \
	$(MOD_SRC)/grp_data.c \
	$(MOD_SRC)/interface.c \
	$(MOD_SRC)/line_pe.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/ms_stereo.c \
	$(MOD_SRC)/pre_echo_control.c \
	$(MOD_SRC)/psy_configuration.c \
	$(MOD_SRC)/psy_main.c \
	$(MOD_SRC)/qc_main.c \
	$(MOD_SRC)/quantize.c \
	$(MOD_SRC)/sf_estim.c \
	$(MOD_SRC)/spreading.c \
	$(MOD_SRC)/stat_bits.c \
	$(MOD_SRC)/tns.c \
	$(MOD_SRC)/transform.c \
	$(ASM_SRC6)/band_nrg_asm.S \
	$(ASM_SRC6)/CalcWindowEnergy_asm.S \
	$(ASM_SRC6)/TNS_ASM.S \
	$(ASM_SRC7)/PPMdct.S \
	$(ASM_SRC7)/R4_Core_V7.S \
	$(ASM_SRC7)/R8FirstPass_v7.S 
	



LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../src \
	../../../../basic_op


VOMM:=-DLINUX -DARMV5E -DARMV7Neon -DARM_INASM -DARMV5_INASM -DLCHECK 
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x03210000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

