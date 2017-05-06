LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoMPGFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/MPEG
CSRCMN_PATH:=../../../../../../File/Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CSRCMN_PATH)/CBaseReader.cpp \
	$(CSRCMN_PATH)/CBaseStreamFileIndex.cpp \
	$(CSRCMN_PATH)/CBaseStreamFileReader.cpp \
	$(CSRCMN_PATH)/CBaseStreamFileTrack.cpp \
	$(CSRCMN_PATH)/CBaseTrack.cpp \
	$(CSRCMN_PATH)/CGBuffer.cpp \
	$(CSRCMN_PATH)/CGFileChunk.cpp \
	$(CSRCMN_PATH)/CMemPool.cpp \
	$(CSRCMN_PATH)/CvoBaseDrmCallback.cpp \
	$(CSRCMN_PATH)/CvoBaseFileOpr.cpp \
	$(CSRCMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRCMN_PATH)/CvoFileParser.cpp \
	$(CSRCMN_PATH)/fCodec.c \
	$(CSRCMN_PATH)/voSource2ParserWrapper.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CSRCMN_PATH)/voFileReader.cpp \
    $(CSRC_PATH)/CMpegAudioTrack.cpp \
	$(CSRC_PATH)/CMpegAVCVideoTrack.cpp \
	$(CSRC_PATH)/CMpegParser.cpp \
	$(CSRC_PATH)/CMpegReader.cpp \
	$(CSRC_PATH)/CMpegTrack.cpp \
	$(CSRC_PATH)/CMpegVideoTrack.cpp
LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \



VOMM:= -D_MPEG_READER -DVO_FILE_USE_BIG_ENDIAN -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_SOURCE2

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01090000  -DHAVE_NEON=1 -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

