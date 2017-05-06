LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoSrcWMSP

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/WindowsMediaHTTPStreaming
CSRCMN_PATH:=../../../../../../Common
CSRCLOCALMN_PATH:=../../../../../../File/Common
SRCINCLUDE_PATH:=../../../../../../Include


LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceBufferManager.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	$(CSRCLOCALMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRC_PATH)/HTTP_Transaction.cpp \
	$(CSRC_PATH)/StreamCache.cpp \
	$(CSRC_PATH)/TCPClientSocket.cpp \
	$(CSRC_PATH)/Thread.cpp \
	$(CSRC_PATH)/voWMSP2.cpp \
	$(CSRC_PATH)/WMHTTP.cpp \
	$(CSRC_PATH)/WMSP2Manager.cpp

LOCAL_C_INCLUDES := \
	../../../../../../Include \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CSRCMN_PATH)/Buffer \
	$(CSRCLOCALMN_PATH) \
	$(CSRC_PATH)



VOMM:= -D__arm -DLINUX -D_LINUX -D__VO_NDK__ -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_WARNING
# -D_VOLOG_RUN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01124000  -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=soft -fsigned-char
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog -L../../../../../../../Lib/ndk -lvodl

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

