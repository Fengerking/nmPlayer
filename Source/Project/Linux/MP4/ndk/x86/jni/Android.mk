LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoMP4FR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=  ../../../../../../File/MP4
CSRCLOCALMN_PATH:=../../../../../../File/Common
CSRCMN_PATH:=../../../../../../Common
CMDSRC_PATH:=../../../../../../File/MetaData
CSOURCE_PATH:=../../../../../../../Source
SRCINCLUDE_PATH:=../../../../../../Include

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/cmnVOMemory.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/NetWork/vo_socket.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CMDSRC_PATH)/CBaseMetaData.cpp \
	$(CMDSRC_PATH)/CID3MetaData.cpp \
	$(CMDSRC_PATH)/CMP4MetaData.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	\
	$(CSRC_PATH)/ISOMedia/isomrd.cpp \
	$(CSRC_PATH)/ISOMedia/isomscan.cpp \
	$(CSRC_PATH)/ISOMedia/SegmentTrack.cpp \
	$(CSRC_PATH)/ISOMedia/fragmentTrack.cpp \
	$(CSRC_PATH)/ISOMedia/subtitleTrack.cpp \
	$(CSRC_PATH)/ISOMedia/3gpTTextOp.cpp \
	$(CSRC_PATH)/MP4Base/mp4cfg.cpp \
	$(CSRC_PATH)/MP4Base/mp4desc.cpp \
	$(CSRC_PATH)/MP4Base/mpxio.cpp \
	$(CSRC_PATH)/MP4Base/mpxobj.cpp \
	$(CSRC_PATH)/MP4Base/mpxutil.cpp \
	$(CSRC_PATH)/Reader/CMp4FileStream.cpp \
	$(CSRC_PATH)/Reader/CMp4Reader2.cpp \
	$(CSRC_PATH)/Reader/CMp4Track2.cpp \
	$(CSRC_PATH)/Reader/CMp4PushReader2.cpp \
	$(CSRC_PATH)/Reader/CMp4PushFileStream.cpp \
	$(CSRC_PATH)/Reader/CMp4BaseFileStream.cpp \
	$(CSRC_PATH)/Reader/CMp4TrackPushPlay.cpp \
	\
	$(CSRCLOCALMN_PATH)/CBaseReader.cpp \
	$(CSRCLOCALMN_PATH)/CBaseTrack.cpp \
	$(CSRCLOCALMN_PATH)/CGFileChunk.cpp \
	$(CSRCLOCALMN_PATH)/CMemPool.cpp \
	$(CSRCLOCALMN_PATH)/CvoBaseDrmCallback.cpp \
	$(CSRCLOCALMN_PATH)/CvoBaseFileOpr.cpp \
	$(CSRCLOCALMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRCLOCALMN_PATH)/CvoFileParser.cpp \
	$(CSRCLOCALMN_PATH)/fCodec.c \
	$(CSRCLOCALMN_PATH)/voSource2ParserWrapper.cpp \
	$(CSRCLOCALMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRCLOCALMN_PATH)/CSourceIOSwitch.cpp \
	$(CSRCLOCALMN_PATH)/base64.cpp \
	$(CSRCLOCALMN_PATH)/strutil.cpp \
	$(CSRCLOCALMN_PATH)/fortest.cpp \
	$(CSRCLOCALMN_PATH)/voFileReader.cpp \
	$(CSRCLOCALMN_PATH)/voStreamReader.cpp \
	$(CSRCLOCALMN_PATH)/vo_file_io.cpp \
	$(CSRCLOCALMN_PATH)/vo_file_parser.cpp \
	$(CSOURCE_PATH)/Common/vo_thread.cpp \
	\
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_http_stream.cpp \
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_mem_stream.cpp \
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_stream.cpp \
	$(CSOURCE_PATH)/SMTHParser/voDrmCallback.cpp \
	$(CSRCLOCALMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRCLOCALMN_PATH)/voSource2WrapperManager.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceBufferManager.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceVideoDataBuffer.cpp



	
LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRC_PATH)/ISOMedia \
	$(CSRC_PATH)/MP4Base \
	$(CSRC_PATH)/Reader \
	$(CSRCLOCALMN_PATH) \
	$(CSRCMN_PATH)/Buffer \
	$(CMDSRC_PATH) \
	$(CSOURCE_PATH)/IISSmoothStreaming \
	$(CSOURCE_PATH)/SMTHParser \
	$(CMNSRC_PATH)/NetWork \
	$(CSOURCE_PATH)/Common \
	$(SRCINCLUDE_PATH)
	
	


VOMM:= -D_MP4_READER -D_USE_J3LIB -D_SUPPORT_PARSER_METADATA -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_FUNC -D_SUPPORT_PIFF -D_SUPPORT_SEGMENT -D_SOURCE2 -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOLOG_ERRORR -D_VOLOG_RUN #-D_SUPPORT_TTEXT 

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01010000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

