LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoSBCDec

CMN_SRC:=../../../../../../../../../Common
SBC_SRC:=../../../../../src
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(SBC_SRC)/decode.c \
	$(SBC_SRC)/sbccommon.c \
	$(SBC_SRC)/voSBCDec.c \
        $(SBC_SRC)/vosbcmemory.c 	



LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	../../../../../../../../../Common \
	../../../../../inc
	

VOMM:=-DLINUX -DARM -DARMV7 -D_ARM_ 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x02350000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS:=../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

