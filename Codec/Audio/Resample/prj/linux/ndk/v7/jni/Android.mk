LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoResample

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../../src
ASM_SRC6:=../../../../../sfile/linuxasm
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_SRC)/ChanDnUp.c \
	$(MOD_SRC)/commontables.c \
	$(MOD_SRC)/FirFilter.c \
	$(MOD_SRC)/RateDU.c \
	$(MOD_SRC)/voResample.c \
	$(MOD_SRC)/Resample.c \
	$(MOD_SRC)/voMalloc.c \
	$(MOD_SRC)/ReSampleBy2.c \
	$(ASM_SRC6)/voMemory.S


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../../inc \
	../../../../../sfile/linuxasm


VOMM:=-DLINUX -DARM -DARMV6 #-DG1_LOG 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x04040000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

