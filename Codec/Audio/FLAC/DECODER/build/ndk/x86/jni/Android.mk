LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoFLACDec

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src/c_src

LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
        $(MOD_SRC)/bit.c \
	$(MOD_SRC)/frame.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/stream.c \
	$(MOD_SRC)/subframe.c \
	$(MOD_SRC)/voFLACDec.c \


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


#VOMM:=-DLINUX -DARM -DARMV6 -DARMV7 -DASM_OPT -DLCHECK
VOMM:=-DLINUX -DLCHECK
LOCAL_LDLIBS:= -llog
# about info option, do not need to care it

#LOCAL_CFLAGS := -D_VOMODULEID=0x022d0000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char
LOCAL_CFLAGS := -D_VOMODULEID=0x022d0000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -fsigned-char

LOCAL_LDLIBS+=../../../../../../../../Lib/ndk/x86/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

