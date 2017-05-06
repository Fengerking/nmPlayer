LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoWVParser

CMNSRC_PATH:=../../../../../../../Common
CNWSRC_PATH:=../../../../../../../Common/NetWork
CSRC_PATH:=../../../../../../File/WebVideoParser
CPDSRC_PATH:=../../../../../../File/PD2_Cookies

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CPDSRC_PATH)/log.cpp \
	$(CPDSRC_PATH)/vo_http_downloader.cpp \
	$(CNWSRC_PATH)/vo_socket.cpp \
	$(CPDSRC_PATH)/vo_thread.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CSRC_PATH)/voWebVideoParser.cpp \
	$(CSRC_PATH)/WebVideoParser.cpp \
	$(CSRC_PATH)/WPManager.cpp \
	$(CSRC_PATH)/WPSina.cpp \
	$(CSRC_PATH)/WPTuDou.cpp \
	$(CSRC_PATH)/WPYouKu.cpp \
	$(CSRC_PATH)/WPYouTube.cpp

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CNWSRC_PATH) \
	$(CPDSRC_PATH)



VOMM:= -D_VO_FORCE_PASS_DIVX -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR

# about info option, do not need to care it
LOCAL_CFLAGS := -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=soft -fsigned-char
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

