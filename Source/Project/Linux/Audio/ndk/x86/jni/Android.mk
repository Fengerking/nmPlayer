LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoAudioFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/Audio
CSRFILECMN_PATH:=../../../../../../File/Common
CMDSRC_PATH:=../../../../../../File/MetaData
CSRCMN_PATH:=../../../../../../Common
CSRCMNBUF_PATH:=../../../../../../Common/Buffer
SRCINCLUDE_PATH:=../../../../../../Include

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CFileFormatCheck.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	\
	$(CSRCMNBUF_PATH)/voSourceBufferManager.cpp \
	$(CSRCMNBUF_PATH)/voSourceDataBuffer.cpp \
	$(CSRCMNBUF_PATH)/voSourceSubtitleDataBuffer.cpp \
	$(CSRCMNBUF_PATH)/voSourceVideoDataBuffer.cpp \
	\
	$(CSRCMN_PATH)/vo_thread.cpp \
	\
	$(CMDSRC_PATH)/CBaseMetaData.cpp \
	$(CMDSRC_PATH)/CID3MetaData.cpp \
	\
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
	$(CSRC_PATH)/CAacParser.cpp \
	$(CSRC_PATH)/voAAC2.cpp \
	$(CSRC_PATH)/CSource2AAC.cpp \
	$(CSRC_PATH)/CID3Parser.cpp \
	$(CSRC_PATH)/CMemStream.cpp \
	$(CSRC_PATH)/CBaseAudioPushParser.cpp \
	$(CSRC_PATH)/voSource2AudioPushParser.cpp \
	$(CSRC_PATH)/CMp3PushParser.cpp \
	\
	$(CSRFILECMN_PATH)/CBaseReader.cpp \
	$(CSRFILECMN_PATH)/CGFileChunk.cpp \
	$(CSRFILECMN_PATH)/CMemPool.cpp \
	$(CSRFILECMN_PATH)/CvoBaseDrmCallback.cpp \
	$(CSRFILECMN_PATH)/CvoBaseFileOpr.cpp \
	$(CSRFILECMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRFILECMN_PATH)/CvoFileParser.cpp \
	$(CSRFILECMN_PATH)/fCodec.c \
	$(CSRFILECMN_PATH)/voFileReader.cpp \
	$(CSRFILECMN_PATH)/voSource2ParserWrapper.cpp \
	$(CSRFILECMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRFILECMN_PATH)/voSource2WrapperManager.cpp \
	$(CSRFILECMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRFILECMN_PATH)/CSourceIOSwitch.cpp \
	$(CSRFILECMN_PATH)/CBasePushPaser.cpp

	LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../File/ID3 \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CSRFILECMN_PATH) \
	$(CMDSRC_PATH) \
	$(CSRCMNBUF_PATH) \
	$(SRCINCLUDE_PATH) \



VOMM:= -D_AUDIO_READER -DVO_FILE_USE_BIG_ENDIAN -D_SUPPORT_PARSER_METADATA -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_SOURCE2 #-D_VOLOG_INFO -D_VOLOG_WARNING -D_VOLOG_ERROR

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01050000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_CPPFLAGS := -Wno-conversion-null 
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

