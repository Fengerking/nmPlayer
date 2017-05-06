LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoMP3Dec

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src

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
	$(MOD_SRC)/stream.c



LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../src \


#VOMM:=-DARM -DARMV6 -DARMV7 -DLCHECK
VOMM:=-DLCHECK

# about info option, do not need to care it

#ELOCAL_CFLAGS := -D_VOMODULEID=0x02220000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon
LOCAL_CFLAGS := -D_VOMODULEID=0x02220000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -fsigned-char

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

