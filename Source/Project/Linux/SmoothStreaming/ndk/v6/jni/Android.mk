LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE :=libvoLiveSrcISS

CMNSRC_PATH:=../../../../../../../Common

INCLUDE_PATH:=../../../../../../../Include

COMM_PATH:=../../../../../../../Common

CSRC_PATH:=../../../../../../../Source

		
LOCAL_SRC_FILES := \
	\
	$(COMM_PATH)/cmnVOMemory.cpp \
	$(COMM_PATH)/voCMutex.cpp \
	$(COMM_PATH)/voOSFunc.cpp \
	$(COMM_PATH)/voThread.cpp \
	$(COMM_PATH)/voHalInfo.cpp \
	$(COMM_PATH)/CvoBaseObject.cpp \
	$(COMM_PATH)/NetWork/vo_socket.cpp \
	$(COMM_PATH)/cmnFile.cpp \
	$(COMM_PATH)/voLog.cpp \
	$(COMM_PATH)/CDllLoad.cpp \
	$(COMM_PATH)/voXMLLoad.cpp \
	$(COMM_PATH)/CPtrList.cpp \
	$(CSRC_PATH)/File/Common/fortest.cpp \
	$(CSRC_PATH)/IISSmoothStreaming/vo_http_stream.cpp \
	$(CSRC_PATH)/IISSmoothStreaming/vo_manifest_manager.cpp \
	$(CSRC_PATH)/IISSmoothStreaming/vo_manifest_reader.cpp \
	$(CSRC_PATH)/IISSmoothStreaming/vo_stream.cpp \
	$(CSRC_PATH)/IISSmoothStreaming/vo_mem_stream.cpp \
	$(CSRC_PATH)/IISSmoothStreaming/vo_smooth_streaming.cpp \
	$(CSRC_PATH)/Common/vo_thread.cpp \
	$(CSRC_PATH)/File/Common/CCodeCC.cpp \
	$(CSRC_PATH)/File/Common/fCodec.cpp \
	$(CSRC_PATH)/IISSmoothStreaming/vo_network_judgment.cpp \
	../../../../../../../Utility/voutf8conv/voutf8conv.c \
	$(CSRC_PATH)/ASController/CASController.cpp \
	$(CSRC_PATH)/File/Common/voASControllerAPI.cpp \
	$(COMM_PATH)/Buffer/voSourceBufferManager.cpp \
	$(COMM_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(COMM_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(COMM_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	$(CSRC_PATH)/File/Common/CBitrateMap.cpp 	

	
	
LOCAL_C_INCLUDES := \
		$(INCLUDE_PATH) \
		$(INCLUDE_PATH)/vome \
		$(COMM_PATH) \
		$(COMM_PATH)/NetWork \
		../../../../../../../MFW/voME/Common \
		$(CSRC_PATH)/IISSmoothStreaming \
		$(CSRC_PATH)/File/SMTH \
		../../../../../../../Utility/voutf8conv \
		$(CSRC_PATH)/File/XML \
		$(CSRC_PATH)/File/Common \
		/opt/android-ndk-r5b/sources/android/cpufeatures \
		$(CSRC_PATH)/ASController \
		$(COMM_PATH)/Buffer \
		$(CSRC_PATH)/Common



LOCAL_STATIC_LIBRARIES := cpufeatures

VOMM:=-DHAVE_PTHREADS -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_FUNC -D_ISS_SOURCE_ -D__VO_NDK__ -D_NEW_SOURCEBUFFER  -D_VOLOG_WARNING -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_RUN 

# about info option, do not need to care it

LOCAL_CFLAGS :=  -DNDEBUG -march=armv6j -mtune=arm1136jf-s -mfpu=vfp  -mfloat-abi=softfp -msoft-float
LOCAL_LDLIBS:=../../../../../../../Lib/ndk/libvoCheck.a  -llog -L../../../../../../../Lib/ndk/ -lvodl
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

$(call import-module,cpufeatures)

