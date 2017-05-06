LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoMP3Enc

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_SRC)/Bits.c \
	$(MOD_SRC)/Frame.c \
	$(MOD_SRC)/Lay3.c \
	$(MOD_SRC)/Loop.c \
	$(MOD_SRC)/Mdct.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/MP3Enc.c \
	$(MOD_SRC)/Subband.c \
	$(MOD_SRC)/Tab.c 
	

LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../src \


VOMM:=-DLCHECK 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x03220000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

