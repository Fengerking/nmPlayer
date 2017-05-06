# Source files for TS file parser.
# Dolby Du
# 2013.08.27


# path variables for TS file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_INCLUDE_PATH:=$(SOURCE_PATH)/Include
SOURCE_COMMON_PATH:=$(SOURCE_PATH)/Common
SOURCE_COMMON_BUFFER_PATH:=$(SOURCE_COMMON_PATH)/Buffer
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
TS_SOURCE_PATH:=$(SOURCE_FILE_PATH)/TS
MTV_COMMON_PATH:=$(SOURCE_PATH)/MTV/Common
MTV_TS_PATH:=$(SOURCE_PATH)/MTV/TS
METADATA_SOURCE_PATH:=$(SOURCE_FILE_PATH)/MetaData


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		$(TRUNK_INCLUDE_PATH) \
		$(TRUNK_COMMON_PATH) \
		$(SOURCE_INCLUDE_PATH) \
		$(SOURCE_COMMON_PATH) \
		$(SOURCE_COMMON_BUFFER_PATH) \
		$(MTV_TS_PATH) \
		$(FILE_COMMON_PATH) \
		$(MTV_COMMON_PATH) \
		$(TS_SOURCE_PATH) \
		$(METADATA_SOURCE_PATH) \


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/fVideoHeadDataInfo.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voCSemaphore.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
    $(TRUNK_COMMON_PATH)/voProgramInfo.cpp \
	\
	$(SOURCE_COMMON_PATH)/vo_thread.cpp \
	\
	$(SOURCE_COMMON_BUFFER_PATH)/voSourceBufferManager.cpp \
	$(SOURCE_COMMON_BUFFER_PATH)/voSourceDataBuffer.cpp \
	$(SOURCE_COMMON_BUFFER_PATH)/voSourceSubtitleDataBuffer.cpp \
	$(SOURCE_COMMON_BUFFER_PATH)/voSourceVideoDataBuffer.cpp \
	\
    $(FILE_COMMON_PATH)/CBaseReader.cpp \
	$(FILE_COMMON_PATH)/CBaseStreamFileIndex.cpp \
	$(FILE_COMMON_PATH)/CBaseStreamFileReader.cpp \
	$(FILE_COMMON_PATH)/CBaseStreamFileTrack.cpp \
	$(FILE_COMMON_PATH)/CBaseTrack.cpp \
	$(FILE_COMMON_PATH)/CGBuffer.cpp \
	$(FILE_COMMON_PATH)/CGFileChunk.cpp \
	$(FILE_COMMON_PATH)/CMemPool.cpp \
	$(FILE_COMMON_PATH)/CvoBaseDrmCallback.cpp \
	$(FILE_COMMON_PATH)/CvoBaseFileOpr.cpp \
	$(FILE_COMMON_PATH)/CvoBaseMemOpr.cpp \
	$(FILE_COMMON_PATH)/CvoFileParser.cpp \
	$(FILE_COMMON_PATH)/CvoStreamFileDataParser.cpp \
	$(FILE_COMMON_PATH)/fCodec.cpp \
	$(FILE_COMMON_PATH)/CSourceIOSwitch.cpp \
	$(FILE_COMMON_PATH)/voFileReader.cpp \
	$(FILE_COMMON_PATH)/voSourceBaseWrapper.cpp \
	$(FILE_COMMON_PATH)/voSource2ParserWrapper.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperPushPlay.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperManager.cpp \
	\
	$(MTV_TS_PATH)/CAnalyseAAC.cpp \
	$(MTV_TS_PATH)/CAnalyseH264.cpp \
	$(MTV_TS_PATH)/CAnalyseMPEG4.cpp \
	$(MTV_TS_PATH)/CBaseAnalyseData.cpp \
	$(MTV_TS_PATH)/CBaseParser.cpp \
	$(MTV_TS_PATH)/CBaseTools.cpp \
	$(MTV_TS_PATH)/CDumper.cpp \
	$(MTV_TS_PATH)/CMTVReader.cpp \
	$(MTV_TS_PATH)/CMTVStreamFileIndex.cpp \
	$(MTV_TS_PATH)/adtshead.cpp \
	$(MTV_TS_PATH)/adtspacker.cpp \
	$(MTV_TS_PATH)/adtsparser.cpp \
	$(MTV_TS_PATH)/voMTVParser.cpp \
	$(MTV_TS_PATH)/CCheckTsPacketSize.cpp \
	$(MTV_TS_PATH)/CStream.cpp \
	$(MTV_TS_PATH)/CTsParser.cpp \
	$(MTV_TS_PATH)/CTsReader.cpp \
	$(MTV_TS_PATH)/CTsFileDataParser.cpp \
	$(MTV_TS_PATH)/CTsParseCtroller.cpp \
	$(MTV_TS_PATH)/CTsTrack.cpp \
	$(MTV_TS_PATH)/FormatInit.cpp \
	$(MTV_TS_PATH)/LatmParser.cpp \
	$(MTV_TS_PATH)/PacketBuffer.cpp \
	$(MTV_TS_PATH)/ParseSPS.cpp \
	$(MTV_TS_PATH)/Repacker.cpp \
	$(MTV_TS_PATH)/StreamFormat.cpp \
	$(MTV_TS_PATH)/TSParser.cpp \
	$(MTV_TS_PATH)/framechk.cpp \
	$(MTV_TS_PATH)/isombase.cpp \
	$(MTV_TS_PATH)/isomio.cpp \
	$(MTV_TS_PATH)/isomutil.cpp \
	$(MTV_TS_PATH)/mp4cfg.cpp \
	$(MTV_TS_PATH)/mp4desc.cpp \
	$(MTV_TS_PATH)/tsbscls.cpp \
	$(MTV_TS_PATH)/tsbspas.cpp \
	$(MTV_TS_PATH)/tsdesc.spec.cpp \
	$(MTV_TS_PATH)/tsparse.cpp \
	$(MTV_TS_PATH)/tssi.spec.cpp \
	$(MTV_TS_PATH)/tsstruct.cpp
