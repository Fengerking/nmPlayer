LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoMPGFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/MPEG
CSRFILECMN_PATH:=../../../../../../File/Common
CSRCMN_PATH:=../../../../../../Common
CSRCMNBUF_PATH:=../../../../../../Common/Buffer
SRCINCLUDE_PATH:=../../../../../../Include

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	\
	$(CSRCMNBUF_PATH)/voSourceBufferManager.cpp \
	$(CSRCMNBUF_PATH)/voSourceDataBuffer.cpp \
	$(CSRCMNBUF_PATH)/voSourceSubtitleDataBuffer.cpp \
	$(CSRCMNBUF_PATH)/voSourceVideoDataBuffer.cpp \
	\
	$(CSRCMN_PATH)/vo_thread.cpp \
	\
	$(CSRFILECMN_PATH)/CBaseReader.cpp \
	$(CSRFILECMN_PATH)/CBaseStreamFileIndex.cpp \
	$(CSRFILECMN_PATH)/CBaseStreamFileReader.cpp \
	$(CSRFILECMN_PATH)/CBaseStreamFileTrack.cpp \
	$(CSRFILECMN_PATH)/CBaseTrack.cpp \
	$(CSRFILECMN_PATH)/CGBuffer.cpp \
	$(CSRFILECMN_PATH)/CGFileChunk.cpp \
	$(CSRFILECMN_PATH)/CMemPool.cpp \
	$(CSRFILECMN_PATH)/CvoBaseDrmCallback.cpp \
	$(CSRFILECMN_PATH)/CvoBaseFileOpr.cpp \
	$(CSRFILECMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRFILECMN_PATH)/CvoFileParser.cpp \
	$(CSRFILECMN_PATH)/fCodec.c \
	$(CSRFILECMN_PATH)/voSource2ParserWrapper.cpp \
	$(CSRFILECMN_PATH)/voSource2WrapperManager.cpp \
	$(CSRFILECMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRFILECMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRFILECMN_PATH)/CSourceIOSwitch.cpp \
	$(CSRFILECMN_PATH)/voFileReader.cpp	\
	$(CSRC_PATH)/CMpegAudioTrack.cpp \
	$(CSRC_PATH)/CMpegAVCVideoTrack.cpp \
	$(CSRC_PATH)/CMpegParser.cpp \
	$(CSRC_PATH)/CMpegReader.cpp \
	$(CSRC_PATH)/CMpegTrack.cpp \
	$(CSRC_PATH)/CMpegVideoTrack.cpp \
	$(CSRC_PATH)/CMpegFileIndex.cpp


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CSRFILECMN_PATH) \
	$(CSRCMNBUF_PATH) \
	$(SRCINCLUDE_PATH)



VOMM:= -D_MPEG_READER -DVO_FILE_USE_BIG_ENDIAN -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_SOURCE2

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01090000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

