LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoG729Dec

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src/c_src
ASM_SRC6:=../../../../src/linux_asm/ARMv4
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_SRC)/bits.c \
	$(MOD_SRC)/calcexc.c \
	$(MOD_SRC)/de_acelp.c \
	$(MOD_SRC)/dec_gain.c \
	$(MOD_SRC)/dec_lag3.c \
	$(MOD_SRC)/dec_ld8a.c \
	$(MOD_SRC)/dec_sid.c \
	$(MOD_SRC)/dspfunc.c \
	$(MOD_SRC)/filter.c \
	$(MOD_SRC)/g729_d_api.c \
	$(MOD_SRC)/gainpred.c \
	$(MOD_SRC)/lpcfunc.c \
	$(MOD_SRC)/lspdec.c \
	$(MOD_SRC)/lspgetq.c \
	$(MOD_SRC)/oper_32b.c \
	$(MOD_SRC)/p_parity.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/post_pro.c \
	$(MOD_SRC)/postfilt.c \
	$(MOD_SRC)/pred_lt3.c \
	$(MOD_SRC)/qsidgain.c \
	$(MOD_SRC)/tab_dtx.c \
	$(MOD_SRC)/tab_ld8a.c \
	$(MOD_SRC)/taming.c \
	$(MOD_SRC)/util.c \
	$(ASM_SRC6)/copy_opt.S \
	$(ASM_SRC6)/Pred_lt_3_opt.S \
	$(ASM_SRC6)/Residu.S \
	$(ASM_SRC6)/sum_mac_v4.S \
	$(ASM_SRC6)/sum_mac_v4_xy.S \
	$(ASM_SRC6)/weight.S 


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


VOMM:=-DLINUX -DARM -DARMV6 -DASM_OPT -DLCHECK

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02300000   -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

