LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyTSDemuxLib

CMNSRC_PATH:=../../../../../../../Common
FILECMNSRC_PATH:=../../../../../../File/Common
MTVCMNSRC_PATH:=../../../../../../MTV/Common
CSRC_PATH:=../../../../../../MTV/TS
SOURCECMN_PATH:=../../../../../../Common
SOURCEINC_PATH:=../../../../../../Include

LOCAL_SRC_FILES := \
  \
	$(FILECMNSRC_PATH)/CBaseReader.cpp \
	$(FILECMNSRC_PATH)/CBaseStreamFileIndex.cpp \
	$(FILECMNSRC_PATH)/CBaseStreamFileReader.cpp \
	$(FILECMNSRC_PATH)/CBaseStreamFileTrack.cpp \
	$(FILECMNSRC_PATH)/CvoStreamFileDataParser.cpp \
	$(FILECMNSRC_PATH)/CBaseTrack.cpp \
	$(FILECMNSRC_PATH)/CGBuffer.cpp \
	$(FILECMNSRC_PATH)/CGFileChunk.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(FILECMNSRC_PATH)/CvoBaseFileOpr.cpp \
	$(FILECMNSRC_PATH)/CvoBaseMemOpr.cpp \
	$(FILECMNSRC_PATH)/CvoFileParser.cpp \
	$(FILECMNSRC_PATH)/fCodec.c \
	$(FILECMNSRC_PATH)/CvoBaseDrmCallback.cpp \
	$(FILECMNSRC_PATH)/voSource2WrapperPushPlay.cpp \
	$(FILECMNSRC_PATH)/voSource2WrapperManager.cpp \
	$(FILECMNSRC_PATH)/CSourceIOSwitch.cpp \
	$(SOURCECMN_PATH)/vo_thread.cpp \
	$(SOURCECMN_PATH)/Buffer/voSourceBufferManager.cpp \
	$(SOURCECMN_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(SOURCECMN_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(SOURCECMN_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	$(CSRC_PATH)/CBaseParser.cpp \
	$(CSRC_PATH)/CDumper.cpp \
	$(CSRC_PATH)/CMTVReader.cpp \
	$(CSRC_PATH)/CMTVStreamFileIndex.cpp \
	$(CSRC_PATH)/voMTVParser.cpp \
	$(CSRC_PATH)/CBaseTools.cpp \
	$(CSRC_PATH)/CAnalyseH264.cpp \
	$(CSRC_PATH)/CAnalyseAAC.cpp \
	$(CSRC_PATH)/CAnalyseMPEG4.cpp \
	$(CSRC_PATH)/CBaseAnalyseData.cpp \
	$(CSRC_PATH)/CTsParser.cpp \
	$(CSRC_PATH)/CTsReader.cpp \
	$(CSRC_PATH)/CTsFileDataParser.cpp \
	$(CSRC_PATH)/CTsTrack.cpp \
	$(CSRC_PATH)/adtshead.cpp \
	$(CSRC_PATH)/adtspacker.cpp \
	$(CSRC_PATH)/adtsparser.cpp \
	$(CSRC_PATH)/CStream.cpp \
	$(CSRC_PATH)/framechk.cpp \
	$(CSRC_PATH)/LatmParser.cpp \
	$(CSRC_PATH)/PacketBuffer.cpp \
	$(CSRC_PATH)/ParseSPS.cpp \
	$(CSRC_PATH)/StreamFormat.cpp \
	$(CSRC_PATH)/isombase.cpp \
	$(CSRC_PATH)/isomio.cpp \
	$(CSRC_PATH)/isomutil.cpp \
	$(CSRC_PATH)/mp4cfg.cpp \
	$(CSRC_PATH)/mp4desc.cpp \
	$(CSRC_PATH)/CTsParseCtroller.cpp \
	$(CSRC_PATH)/FormatInit.cpp \
	$(CSRC_PATH)/Repacker.cpp \
	$(CSRC_PATH)/TSParser.cpp \
	$(CSRC_PATH)/tsdesc.spec.cpp \
	$(CSRC_PATH)/tssi.spec.cpp \
	$(CSRC_PATH)/tsbscls.cpp \
	$(CSRC_PATH)/tsbspas.cpp \
	$(CSRC_PATH)/tsparse.cpp \
	$(CSRC_PATH)/tsstruct.cpp \
	$(CSRC_PATH)/CCheckTsPacketSize.cpp \
	$(FILECMNSRC_PATH)/CMemPool.cpp \
	$(FILECMNSRC_PATH)/voSourceBaseWrapper.cpp \
	$(FILECMNSRC_PATH)/voSource2ParserWrapper.cpp \
	$(FILECMNSRC_PATH)/voFileReader.cpp
	
LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(FILECMNSRC_PATH) \
	$(MTVCMNSRC_PATH) \
	$(CSRC_PATH) \
	$(SOURCEINC_PATH) \
	$(SOURCECMN_PATH) \
	$(SOURCECMN_PATH)/Buffer


VOMM:=-D__VO_NDK__ -DMTV_TS -D_SUPPORT_CACHE -D_LINUX_ANDROID  -D_SOURCE2 -DLINUX -D_LINUX -D__arm  -D_VONDBG


LOCAL_CFLAGS := -D_VOMODULEID=0x010c1000 -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char
LOCAL_LDLIBS := -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyTSDemux


LOCAL_STATIC_LIBRARIES := libyyTSDemuxLib


include $(BUILD_SHARED_LIBRARY)
