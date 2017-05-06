LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoMKVFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/MKV
CSRCMN_PATH:=../../../../../../File/Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CSRC_PATH)/CMKVAnalyze.cpp \
	$(CSRC_PATH)/EBML.cpp \
	$(CSRC_PATH)/CMKVAudioTrack.cpp \
	$(CSRC_PATH)/CMKVAudioTrack_RealSpecialize.cpp \
	$(CSRC_PATH)/CMKVFileReader.cpp \
	$(CSRC_PATH)/CMKVParser.cpp \
	$(CSRC_PATH)/CMKVStreamFileIndex.cpp \
	$(CSRC_PATH)/CMKVTrack.cpp \
	$(CSRC_PATH)/CMKVVideoTrack.cpp \
	$(CSRC_PATH)/CMKVVideoTrack_RealSpecialize.cpp \
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
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CSRCMN_PATH)/voFileReader.cpp \
	$(CSRCMN_PATH)/voSource2ParserWrapper.cpp

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \



VOMM:= -D_MKV_READER -DVO_FILE_USE_BIG_ENDIAN -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_FUNC -D_VOLOG_EORROR -D_SOURCE2

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01080000  -DHAVE_NEON=1 -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

