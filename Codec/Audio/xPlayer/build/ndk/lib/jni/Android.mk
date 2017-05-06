LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoAudioSpeed

CMM_SRC:=../../../../src
CMM_SRC1:=../../../../../../../Common

LOCAL_SRC_FILES := \
		$(CMM_SRC)/voxPlayerApis.c \
		$(CMM_SRC)/voRingBuffer.c \
		$(CMM_SRC)/voxPlayerStrech.c \
		$(CMM_SRC)/mem_align.c \
	        $(CMM_SRC1)/cmnMemory.c 	


LOCAL_C_INCLUDES := \
	../../../../inc \
	../../../../../../../Include \
	../../../../../../../Common

JPDEFINE:=-DLINUX -DARM -DARMV6 #-DLCHECK 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x04030000  $(JPDEFINE) -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char 

LOCAL_LDLIBS:=-llog ../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

