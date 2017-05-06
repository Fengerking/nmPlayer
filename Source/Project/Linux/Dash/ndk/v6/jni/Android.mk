LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE :=libvoLiveSrcDASH

CMNSRC_PATH:=../../../../../../../Common

INCLUDE_PATH:=../../../../../../../Include

COMM_PATH:=../../../../../../../Common

CSRC_PATH:=../../../../../../../Source
CSCOMM_PATH:=../../../../../../../Source/File/Common

#if use new bufferManager, shall be set 1
NEW_BUFFERMANAGER :=1

ifeq ($(NEW_BUFFERMANAGER), 1)
BUFFERMANAGER := \
	$(COMM_PATH)/Buffer/voSourceBufferManager.cpp \
	$(COMM_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(COMM_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(COMM_PATH)/Buffer/voSourceVideoDataBuffer.cpp
else
BUFFERMANAGER := \
	$(CSRC_PATH)/File/Common/voSource2DataBuffer.cpp
endif

LOCAL_SRC_FILES := \
	$(CSCOMM_PATH)/tinystr.cpp \
	$(CSCOMM_PATH)/tinyxml.cpp \
	$(CSCOMM_PATH)/tinyxmlerror.cpp \
	$(CSCOMM_PATH)/tinyxmlparser.cpp \
	\
	$(COMM_PATH)/cmnVOMemory.cpp \
	$(COMM_PATH)/voCMutex.cpp \
	$(COMM_PATH)/voOSFunc.cpp \
	$(COMM_PATH)/voThread.cpp \
	$(COMM_PATH)/CvoBaseObject.cpp \
	$(COMM_PATH)/NetWork/vo_socket.cpp \
	$(COMM_PATH)/cmnFile.cpp \
	$(COMM_PATH)/CDllLoad.cpp \
	$(COMM_PATH)/CPtrList.cpp \
	$(COMM_PATH)/voHalInfo.cpp \
	$(COMM_PATH)/voLog.cpp \
	$(COMM_PATH)/fAudioHeadDataInfo.cpp \
	$(CSCOMM_PATH)/fortest.cpp \
	$(CSCOMM_PATH)/vo_thread.cpp \
	$(CSRC_PATH)/dash/vo_http_stream.cpp \
	$(CSRC_PATH)/dash/vo_mpd_manager.cpp \
	$(CSRC_PATH)/dash/vo_mpd_reader.cpp \
	$(CSRC_PATH)/dash/vo_stream.cpp \
	$(CSRC_PATH)/dash/vo_mem_stream.cpp \
	$(CSRC_PATH)/dash/vo_mpd_streaming.cpp \
	$(CSRC_PATH)/dash/vo_network_judgment.cpp \
	../../../../../../../Utility/voutf8conv/voutf8conv.c \
	$(CSRC_PATH)/ASController/CASController.cpp \
	$(CSRC_PATH)/File/Common/voASControllerAPI.cpp \
	$(CSRC_PATH)/File/Common/CBitrateMap.cpp \
	$(BUFFERMANAGER) 
	
	
LOCAL_C_INCLUDES := \
		$(INCLUDE_PATH) \
		$(INCLUDE_PATH)/vome \
		$(COMM_PATH) \
		$(COMM_PATH)/NetWork \
		../../../../../../../MFW/voME/Common \
		$(CSRC_PATH)/dash \
		$(CSRC_PATH)/File/SMTH \
		../../../../../../../Utility/voutf8conv \
		$(CSRC_PATH)/File/XML \
		$(CSRC_PATH)/File/Common \
		/opt/android-ndk-r5b/sources/android/cpufeatures \
		$(CSRC_PATH)/ASController \
		$(COMM_PATH)/Buffer


LOCAL_STATIC_LIBRARIES := cpufeatures


ifeq ($(NEW_BUFFERMANAGER), 1)
VOMM:=-DLINUX -D_LINUX -DHAVE_PTHREADS -D_LINUX_ANDROID  -D_VOLOG_FUNC -D_DASH_SOURCE_ -D__VO_NDK__ -D_NEW_SOURCEBUFFER  -D_VOLOG_WARNING -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_RUN
else
VOMM:=-DLINUX -D_LINUX -DHAVE_PTHREADS -D_LINUX_ANDROID  -D_VOLOG_FUNC -D_DASH_SOURCE_ -D__VO_NDK__ -D_POORLEON  -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOLOG_ERROR 
endif

# about info option, do not need to care it

LOCAL_CFLAGS :=  -DNDEBUG -march=armv6j -mtune=arm1136jf-s -mfpu=vfp  -mfloat-abi=softfp -msoft-float
LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a -llog
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

$(call import-module,cpufeatures)

