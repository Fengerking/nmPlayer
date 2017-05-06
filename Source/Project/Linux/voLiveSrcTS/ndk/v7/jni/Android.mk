LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoLiveSrcTS

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../TSStreaming
CSRCMN_PATH:=../../../../../../MTV/LiveSource/Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CSRC_PATH)/CLiveSrcTS.cpp \
	$(CSRCMN_PATH)/ChannelInfo.cpp \
	$(CSRCMN_PATH)/CLiveParserBase.cpp \
	$(CSRCMN_PATH)/CLiveSrcBase.cpp \
	$(CSRCMN_PATH)/ConfigFile.cpp \
	$(CSRCMN_PATH)/voLiveSource.cpp

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH)

LOCAL_STATIC_LIBRARIES := cpufeatures



VOMM:= -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_FUNC -D_VOLOG_INFO -D_VOLOG_WARNING -D_LIVE_TS_SOURCE_

# about info option, do not need to care it
LOCAL_CFLAGS := -DHAVE_NEON=1 -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
$(call import-module,cpufeatures)
