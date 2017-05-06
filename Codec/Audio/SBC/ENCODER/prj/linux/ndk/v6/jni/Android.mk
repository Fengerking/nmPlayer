LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoSBCEnc

CMN_SRC:=../../../../../../../../../Common
SBC_SRC:=../../../../../src
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(SBC_SRC)/encode.c \
	$(SBC_SRC)/sbccommon.c \
	$(SBC_SRC)/voSBCEnc.c \
        $(SBC_SRC)/vosbcmemory.c 	



LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	../../../../../../../../../Common \
	../../../../../inc
	

VOMM:=-DLINUX -DARM -DARMV6 -D_ARM_ 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x03350000   -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char -msoft-float

LOCAL_LDLIBS:=../../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

