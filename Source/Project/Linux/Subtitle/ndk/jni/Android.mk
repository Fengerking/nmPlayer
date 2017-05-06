LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../..
LOCAL_ARM_MODE := arm

include $(CLEAR_VARS)
LOCAL_MODULE := libvoSubtitleParser
LOCAL_MODULE_TAGS := optional

VOMM:=-D__VO_NDK__ -D_SUPPORT_CACHE -D_LINUX_ANDROID -DLINUX -D_LINUX -D__arm

LOCAL_CPPFLAGS  := \
				-D_LINUX_ANDROID \
				-D__VO_NDK__ \
				-fsigned-char -fno-short-enums -g -Wno-write-strings -mfloat-abi=soft -fsigned-char \
				-D_VOLOG_ERROR \
				-D_VOLOG_WARNING \
				-D_VOLOG_INFO 
				

CMNSRC_PATH:=../../../../../../Common
CSRC_PATH:=../../../../../Subtitle
CMNFILE_PATH:=../../../../../File/Common

LOCAL_SRC_FILES := $(CSRC_PATH)/voOMXFile.cpp \
	$(CMNSRC_PATH)/cmnFile.cpp \
	\
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CSRC_PATH)/CBaseSubtitleParser.cpp \
	$(CSRC_PATH)/CBaseSubtitleTrack.cpp \
	$(CSRC_PATH)/CSMIParser.cpp \
	$(CSRC_PATH)/CSrtParser.cpp \
	$(CSRC_PATH)/CSubtitleMgr.cpp \
	$(CSRC_PATH)/CTextTrack.cpp \
	$(CSRC_PATH)/subtitleAPI.cpp \
	$(CSRC_PATH)/CCharsetDecoder.cpp \
	$(CSRC_PATH)/CFileFormatConverter.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CPtrList.cpp \
	$(CSRC_PATH)/CTimedTextMLParser.cpp \
	$(CSRC_PATH)/CWebVTTParser.cpp \
	$(CSRC_PATH)/CSubtitleHtmlMgr.cpp \
	$(CSRC_PATH)/CSMPTETTMLParser.cpp \
	$(CMNFILE_PATH)/base64.cpp


LOCAL_C_INCLUDES :=	../../../../../../Include \
			$(JNI_H_INCLUDE) \
			../../../../../../Include/vome \
			$(call include-path-for, graphics corecg)    \
			../../../../../../Common \
			$(CSRC_PATH) \
			$(CMNFILE_PATH) \

#LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_LDLIBS    := -llog



LOCAL_CFLAGS := -D_VOMODULEID=0x00510000 

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
