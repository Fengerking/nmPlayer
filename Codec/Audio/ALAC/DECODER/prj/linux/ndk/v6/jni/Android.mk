LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoALACDec

CMN_SRC:=../../../../../../../../../Common
MOD_SRC:=../../../../../src
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_SRC)/alac.c \
	$(MOD_SRC)/alacdec.c \
	$(MOD_SRC)/alacstrmdec.c \
	$(MOD_SRC)/headerparse.c \
	$(MOD_SRC)/vomemory.c 



LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	../../../../../../../../../Common \
	../../../../../inc \
	../../../../../parser 


VOMM:=-DLINUX -DARM -DLCHECK

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02320000   -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char

LOCAL_LDLIBS:=../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

