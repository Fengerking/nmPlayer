LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoDolbyEffect

CMN_SRC:=../../../../../../Common
MOD_SRC:=../../../
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_SRC)/voEFTDolby.cpp 
	

LOCAL_C_INCLUDES := \
	../../../../../../Include \
	../../../../../../Common \
	../../../


VOMM:=-D_ARM_VER -DLINUX 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x04020000  -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp

LOCAL_LDLIBS:=../../../ARMV5/dolbymobile.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

