LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoTsParser

CMNSRC_PATH:=../../../../../../../Common
FILECMNSRC_PATH:=../../../../../../File/Common
MTVCMNSRC_PATH:=../../../../../../MTV/Common
CSRC_PATH:=../../../../../../MTV/TS

LOCAL_SRC_FILES := \
	$(FILECMNSRC_PATH)/CBaseReader.cpp \
	$(FILECMNSRC_PATH)/CBaseStreamFileIndex.cpp \
	$(FILECMNSRC_PATH)/CBaseStreamFileReader.cpp \
	$(FILECMNSRC_PATH)/CBaseStreamFileTrack.cpp \
	$(FILECMNSRC_PATH)/CvoStreamFileDataParser.cpp \
	$(FILECMNSRC_PATH)/CBaseTrack.cpp \
	$(FILECMNSRC_PATH)/CGBuffer.cpp \
	$(FILECMNSRC_PATH)/CGFileChunk.cpp \
	$(FILECMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(FILECMNSRC_PATH)/CvoBaseFileOpr.cpp \
	$(FILECMNSRC_PATH)/CvoBaseMemOpr.cpp \
	$(FILECMNSRC_PATH)/CvoFileParser.cpp \
	$(FILECMNSRC_PATH)/fCodec.c \
	$(FILECMNSRC_PATH)/CvoBaseDrmCallback.cpp \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	\
	$(MTVCMNSRC_PATH)/CBaseParser.cpp \
	$(MTVCMNSRC_PATH)/CDumper.cpp \
	$(MTVCMNSRC_PATH)/CMTVReader.cpp \
	$(MTVCMNSRC_PATH)/CMTVStreamFileIndex.cpp \
	$(MTVCMNSRC_PATH)/voMTVParser.cpp \
	$(MTVCMNSRC_PATH)/voMTVReader.cpp \
	$(MTVCMNSRC_PATH)/CBaseTools.cpp \
	$(MTVCMNSRC_PATH)/CAnalyseH264.cpp \
	$(MTVCMNSRC_PATH)/CAnalyseAAC.cpp \
	$(MTVCMNSRC_PATH)/CAnalyseMPEG4.cpp \
	$(MTVCMNSRC_PATH)/CBaseAnalyseData.cpp \
	$(CSRC_PATH)/CTsParser.cpp \
	$(CSRC_PATH)/CTsReader.cpp \
	$(CSRC_PATH)/CTsFileDataParser.cpp \
	$(CSRC_PATH)/CTsTrack.cpp \
	$(MTVCMNSRC_PATH)/adtshead.cpp \
	$(MTVCMNSRC_PATH)/adtspacker.cpp \
	$(MTVCMNSRC_PATH)/adtsparser.cpp \
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
	
LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(FILECMNSRC_PATH) \
	$(MTVCMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:=-D__VO_NDK__ -DHAVE_NEON=1 -DMTV_TS -D_SUPPORT_CACHE -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -DLINUX -D_LINUX

LOCAL_CFLAGS := -D_VOMODULEID=0x010c1000  -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS := -llog ../../../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
