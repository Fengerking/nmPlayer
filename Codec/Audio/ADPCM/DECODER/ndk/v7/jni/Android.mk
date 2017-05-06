LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoADPCMDec

CMN_SRC:=../../../../../../../Common
MOD_SRC:=../../../src
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_SRC)/ADPCMDec.c \
	$(MOD_SRC)/bit.c \
	$(MOD_SRC)/Frame.c \
	$(MOD_SRC)/IMAAdpcm.c \
	$(MOD_SRC)/ITUG726.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/MSAdpcm.c \
	$(MOD_SRC)/Stream.c \
	$(MOD_SRC)/aLawdpcm.c \
	$(MOD_SRC)/uLawdpcm.c \
	$(MOD_SRC)/swfadpcm.c 



LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../Common \
	../../../src \


VOMM:=-DLCHECK 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x022a0000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon

LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
