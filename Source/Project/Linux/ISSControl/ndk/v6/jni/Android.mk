LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

LOCAL_LDLIBS:= -llog

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoISSControl

CMNSRC_PATH:=../../../../../../../Common

INCLUDE_PATH:=../../../../../../../Include

COMM_PATH:=../../../../../../../Common

CSRC_PATH:=../../../../../../../Source
		
LOCAL_SRC_FILES := \
	\
  $(COMM_PATH)/CvoBaseObject.cpp \
  $(COMM_PATH)/voOSFunc.cpp \
  $(COMM_PATH)/CDllLoad.cpp \
  $(COMM_PATH)/CPtrList.cpp \
	$(COMM_PATH)/voCMutex.cpp \
	$(CSRC_PATH)/File/Common/CvoBaseDrmCallback.cpp \
	$(CSRC_PATH)/File/Common/vo_thread.cpp \
	$(CSRC_PATH)/ISSControl/CISSControl.cpp \
	$(CSRC_PATH)/ISSControl/CISSControl_Imp.cpp \
	$(CSRC_PATH)/ISSControl/CManifestManager.cpp \
	$(CSRC_PATH)/ISSControl/voSource2DataBuffer.cpp
	

LOCAL_C_INCLUDES := \
	$(INCLUDE_PATH)\
	$(COMM_PATH) \
	$(COMM_PATH)/NetWork \
	$(CSRC_PATH)/ISSControl \
	$(CSRC_PATH)/File/Common 
	


VOMM:=-DHAVE_PTHREADS -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_FUNC -D__VO_NDK__ #-D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO 

# about info option, do not need to care it

LOCAL_CFLAGS :=  -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp
LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a -llog
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)



