LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoMP3Dec

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src
ASM_SRC7:=$(MOD_SRC)/linuxasm/armv7
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_SRC)/bit.c \
	$(MOD_SRC)/dct32.c \
	$(MOD_SRC)/frame.c \
	$(MOD_SRC)/huffman.c \
	$(MOD_SRC)/layer3.c \
	$(MOD_SRC)/layer12.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/mp3dec.c \
	$(MOD_SRC)/polyphase.c \
	$(MOD_SRC)/rq_table.c \
	$(MOD_SRC)/stream.c \
	$(ASM_SRC7)/dct32_asm.S \
	$(ASM_SRC7)/layer3_asm.S \
	$(ASM_SRC7)/Synth_asm.S


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../src \


VOMM:=-DARM -DARMV6 -DARMV7 -DLCHECK

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02220000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

