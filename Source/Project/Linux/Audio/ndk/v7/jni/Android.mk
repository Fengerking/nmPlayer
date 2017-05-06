LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoAudioFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/Audio
CSRCMN_PATH:=../../../../../../File/Common
CMDSRC_PATH:=../../../../../../File/MetaData

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CFileFormatCheck.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMDSRC_PATH)/CBaseMetaData.cpp \
	$(CMDSRC_PATH)/CID3MetaData.cpp \
	$(CSRC_PATH)/CBaseAudioReader.cpp \
	$(CSRC_PATH)/CAacReader.cpp \
	$(CSRC_PATH)/CAC3Reader.cpp \
	$(CSRC_PATH)/CAmrReader.cpp \
	$(CSRC_PATH)/ape_reader.cpp \
	$(CSRC_PATH)/CAuReader.cpp \
	$(CSRC_PATH)/CFlacReader.cpp \
	$(CSRC_PATH)/CMp3Reader.cpp \
	$(CSRC_PATH)/CQcpReader.cpp \
	$(CSRC_PATH)/CWavReader.cpp \
	$(CSRC_PATH)/DTSReader.cpp \
	$(CSRCMN_PATH)/CBaseReader.cpp \
	$(CSRCMN_PATH)/CGFileChunk.cpp \
	$(CSRCMN_PATH)/CMemPool.cpp \
	$(CSRCMN_PATH)/CvoBaseDrmCallback.cpp \
	$(CSRCMN_PATH)/CvoBaseFileOpr.cpp \
	$(CSRCMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRCMN_PATH)/CvoFileParser.cpp \
	$(CSRCMN_PATH)/fCodec.c \
	$(CSRCMN_PATH)/voFileReader.cpp


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../File/ID3 \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CMDSRC_PATH)



VOMM:= -D_AUDIO_READER -DVO_FILE_USE_BIG_ENDIAN -D_SUPPORT_PARSER_METADATA -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_INFO -D_VOLOG_WARNING -D_VOLOG_ERROR

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01050000  -DHAVE_NEON=1 -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

