LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../

include $(CLEAR_VARS)
LOCAL_MODULE := libvoDVBSubtitleParser
LOCAL_MODULE_TAGS := optional

VOMM:=-D__VO_NDK__ -D_SUPPORT_CACHE -D_LINUX_ANDROID -DLINUX -D_LINUX

LOCAL_CPPFLAGS  := \
				-D_LINUX_ANDROID \
				-D__VO_NDK__ \
				-D_VOLOG_ERROR \
				-D_VOLOG_WARNING \
				-D_VOLOG_INFO 
				

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../DVBSubtitle

LOCAL_SRC_FILES :=	$(CSRC_PATH)/DVBParser.cpp \
			$(CSRC_PATH)/tools.cpp \
			$(CSRC_PATH)/voDVBSubtitleParser.cpp \
			$(CSRC_PATH)/CSubtitlePacker.cpp \
			$(CMNSRC_PATH)/voLog.c \
			

LOCAL_C_INCLUDES :=	../../../../../../../Include \
			$(JNI_H_INCLUDE) \
			../../../../../../../Include/vome \
			$(call include-path-for, graphics corecg)    \
			../../../../../../../Common \
			$(CSRC_PATH)

LOCAL_LDLIBS    := -llog

LOCAL_CFLAGS := -D_VOMODULEID=0x00520000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
