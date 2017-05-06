LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

LOCAL_LDLIBS:= -llog

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoXMLParser

CMNSRC_PATH:=../../../../../../../Common

INCLUDE_PATH:=../../../../../../../Include

COMM_PATH:=../../../../../../../Common

CSRC_PATH:=../../../../../../../Source/File/XML
		
LOCAL_SRC_FILES := \
	\
	$(COMM_PATH)/voLog.c \
  $(CSRC_PATH)/CXMLParser.h \
	$(CSRC_PATH)/XMLOP.cpp \
	$(COMM_PATH)/CPtrList.cpp \
	$(CSRC_PATH)/voXMLParser.cpp
	

LOCAL_C_INCLUDES := \
	$(CMNSRC_PATH)\
	$(INCLUDE_PATH) \
	$(COMM_PATH) \
	$(CSRC_PATH)



VOMM:=-DHAVE_PTHREADS -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOLOG_FUNC -D__VO_NDK__ 

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x010f0000   -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp
LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a -llog
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)



