LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoG722Enc

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_SRC)/common.c \
	$(MOD_SRC)/dct4_a.c \
	$(MOD_SRC)/encoder.c \
	$(MOD_SRC)/g722_enc_api.c \
	$(MOD_SRC)/huff_tab.c \
	$(MOD_SRC)/sam2coef.c \
	$(MOD_SRC)/tables.c \
	$(MOD_SRC)/mem_align.c 


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


VOMM:=-DLINUX -DARM -DARMV6 -DARMV7 -DLCHECK

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x03360000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

