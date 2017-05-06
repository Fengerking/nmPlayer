LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

LOCAL_LDLIBS:= -llog

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoSMILParser

CMNSRC_PATH:=../../../../../../../Common

INCLUDE_PATH:=../../../../../../../Include

COMM_PATH:=../../../../../../../Common

CSRC_PATH:=../../../../../../../Source
CSCOMM_PATH:=../../../../../../../Source/File/Common
		
LOCAL_SRC_FILES := \
  $(COMM_PATH)/CvoBaseObject.cpp \
  $(COMM_PATH)/voOSFunc.cpp \
  $(COMM_PATH)/voLog.c \
  $(COMM_PATH)/CDllLoad.cpp \
  $(COMM_PATH)/voHalInfo.cpp \
  $(COMM_PATH)/voXMLLoad.cpp \
  $(CSRC_PATH)/File/Smil/CBS_SMIL.cpp \
  $(CSRC_PATH)/File/Smil/Smil.cpp \
  $(CSRC_PATH)/File/Smil/voGetSmilAPI.cpp \

LOCAL_C_INCLUDES := \
	$(INCLUDE_PATH)\
	$(COMM_PATH) \
	$(CSRC_PATH)/File/Common \
	$(CSRC_PATH)/Common \
	$(CSRC_PATH)/Include


VOMM:=-DHAVE_PTHREADS -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_FUNC -D__VO_NDK__ -D_SUPPORT_DISCRETIX_DRM -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOLOG_RUN

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x010e0000   -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp
LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a -llog -L../../../../../../../Lib/ndk/ -lvodl
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)



