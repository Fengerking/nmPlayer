LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoOGGDec

CMN_SRC:=../../../../../../../../../Common
MOD_SRC:=../../../../../src
ASM_SRC6:=../../../../../sfile/linuxasm

LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
        $(MOD_SRC)/bitwise.c \
	$(MOD_SRC)/block.c \
	$(MOD_SRC)/codebook.c \
	$(MOD_SRC)/floor0.c \
	$(MOD_SRC)/floor1.c \
	$(MOD_SRC)/functionreg.c \
	$(MOD_SRC)/info.c \
	$(MOD_SRC)/lacedecode.c \
	$(MOD_SRC)/mapping.c \
	$(MOD_SRC)/mdct_fft.c \
	$(MOD_SRC)/oggdec.c \
	$(MOD_SRC)/oggframing.c \
	$(MOD_SRC)/residue.c \
	$(MOD_SRC)/sharedbook.c \
	$(MOD_SRC)/tables.c \
	$(MOD_SRC)/vorbismemory.c \
	$(MOD_SRC)/window.c \
	$(MOD_SRC)/window_tables.c \
        $(ASM_SRC6)/voMemory.S 	


LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	../../../../../../../../../Common \
	../../../../../inc \
	../../../../../sfile/linuxasm


VOMM:=-DLINUX -DARM -D_ARM_ -DARM_OPT -DLCHECK
LOCAL_LDLIBS:= -llog
# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x022f0000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon

LOCAL_LDLIBS+=../../../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

