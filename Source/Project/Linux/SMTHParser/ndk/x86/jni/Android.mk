LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

LOCAL_LDLIBS:= -llog

include $(CLEAR_VARS)

LOCAL_MODULE := libvoSmthParser

CMNSRC_PATH:=../../../../../../../Common

INCLUDE_PATH:=../../../../../../../Include

COMM_PATH:=../../../../../../../Common

CSRC_PATH:=../../../../../../../Source
CSCOMM_PATH:=../../../../../../../Source/File/Common
		
LOCAL_SRC_FILES := \
	\
  $(COMM_PATH)/CvoBaseObject.cpp \
  $(COMM_PATH)/voOSFunc.cpp \
  $(COMM_PATH)/voLog.c \
	$(COMM_PATH)/voCMutex.cpp \
	$(COMM_PATH)/NetWork/vo_socket.cpp \
	$(CSRC_PATH)/File/Common/CvoBaseDrmCallback.cpp \
	$(CSRC_PATH)/File/Common/fortest.cpp \
	$(CSRC_PATH)/Common/vo_thread.cpp \
	$(CSCOMM_PATH)/base64.cpp \
	$(CSCOMM_PATH)/strutil.cpp \
	$(CSRC_PATH)/SMTHParser/CSMTHParser.cpp \
	$(CSRC_PATH)/SMTHParser/CSMTHParserCtrl.cpp \
	$(CSRC_PATH)/SMTHParser/voSmthParser.cpp  \
	$(CSRC_PATH)/SMTHParser/voDrmCallback.cpp \
	$(CSRC_PATH)/IISSmoothStreaming/vo_http_stream.cpp\
	$(CSRC_PATH)/IISSmoothStreaming/vo_mem_stream.cpp \
	$(CSRC_PATH)/IISSmoothStreaming/vo_stream.cpp
	

LOCAL_C_INCLUDES := \
	$(INCLUDE_PATH)\
	$(COMM_PATH) \
	$(COMM_PATH)/NetWork \
	$(CSRC_PATH)/SMTHParser \
	$(CSRC_PATH)/IISSmoothStreaming \
	$(CSRC_PATH)/File/Common \
	$(CSRC_PATH)/Common \
	$(CSRC_PATH)/AdaptiveStreaming/DRM \
	$(CSRC_PATH)/Include
	


VOMM:=-DHAVE_PTHREADS -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_FUNC -D__VO_NDK__ -D_SUPPORT_DISCRETIX_DRM -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOLOG_RUN

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x010b0000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS:=../../../../../../../Lib/ndk/x86/libvoCheck.a -llog -L../../../../../../../Lib/ndk/x86/ -lvodl
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)



