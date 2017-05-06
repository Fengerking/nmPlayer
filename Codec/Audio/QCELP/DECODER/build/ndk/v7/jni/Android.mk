LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoQCELPDec

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src/c_src
ASM_SRC6:=../../../../src/linux_asm

LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
        $(MOD_SRC)/cb.c \
	$(MOD_SRC)/decode13.c \
	$(MOD_SRC)/filter.c \
	$(MOD_SRC)/log10.c \
	$(MOD_SRC)/lpc.c \
	$(MOD_SRC)/lsp.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/pack13.c \
        $(MOD_SRC)/pitch.c \
	$(MOD_SRC)/qcelp13.c \
	$(MOD_SRC)/quant.c \
	$(MOD_SRC)/quant13.c \
	$(MOD_SRC)/stream.c \
	$(MOD_SRC)/voQCELPDec.c \
	$(ASM_SRC6)/wghtfilt_asm.S


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


VOMM:=-DLINUX -DARM -DARMV6 -DARMV7 -DASM_OPT -DLCHECK

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02280000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

