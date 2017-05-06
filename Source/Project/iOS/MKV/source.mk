# Source files for MKV file parser.
# rodney zhang
# 2012.03.21


# path variables for MKV file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_COMMON_PATH:=$(SOURCE_PATH)/Common
SOURCE_INCLUDE_PATH:=$(SOURCE_PATH)/Include
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
MKV_SOURCE_PATH:=$(SOURCE_FILE_PATH)/MKV
METADATA_SOURCE_PATH:=$(SOURCE_FILE_PATH)/MetaData


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		$(TRUNK_INCLUDE_PATH) \
		$(TRUNK_COMMON_PATH) \
		$(FILE_COMMON_PATH) \
		$(ID3_SOURCE_PATH) \
		$(METADATA_SOURCE_PATH) \
		$(MKV_SOURCE_PATH) \
		$(SOURCE_COMMON_PATH) \
		$(SOURCE_COMMON_PATH)/Buffer \
		$(SOURCE_INCLUDE_PATH) \

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/fVideoHeadDataInfo.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/voCSemaphore.cpp \
	$(TRUNK_COMMON_PATH)/voProgramInfo.cpp \
	\
	$(SOURCE_COMMON_PATH)/vo_thread.cpp \
	$(SOURCE_COMMON_PATH)/Buffer/voSourceBufferManager.cpp \
	$(SOURCE_COMMON_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(SOURCE_COMMON_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(SOURCE_COMMON_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	\
	$(FILE_COMMON_PATH)/CBaseReader.cpp \
	$(FILE_COMMON_PATH)/CBaseStreamFileIndex.cpp \
	$(FILE_COMMON_PATH)/CBaseStreamFileReader.cpp \
	$(FILE_COMMON_PATH)/CBaseStreamFileTrack.cpp \
	$(FILE_COMMON_PATH)/CBaseTrack.cpp \
	$(FILE_COMMON_PATH)/CGBuffer.cpp \
	$(FILE_COMMON_PATH)/CGFileChunk.cpp \
	$(FILE_COMMON_PATH)/CvoBaseDrmCallback.cpp \
	$(FILE_COMMON_PATH)/CvoBaseFileOpr.cpp \
	$(FILE_COMMON_PATH)/CvoBaseMemOpr.cpp \
	$(FILE_COMMON_PATH)/CvoFileParser.cpp \
	$(FILE_COMMON_PATH)/CMemPool.cpp \
	$(FILE_COMMON_PATH)/fCodec.cpp \
	$(FILE_COMMON_PATH)/voFileReader.cpp \
	$(FILE_COMMON_PATH)/voSource2ParserWrapper.cpp \
	$(FILE_COMMON_PATH)/voSourceBaseWrapper.cpp \
	$(FILE_COMMON_PATH)/CSourceIOSwitch.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperPushPlay.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperManager.cpp \
	\
	$(MKV_SOURCE_PATH)/CMKVAnalyze.cpp \
	$(MKV_SOURCE_PATH)/CMKVAudioTrack.cpp \
	$(MKV_SOURCE_PATH)/CMKVAudioTrack_RealSpecialize.cpp \
	$(MKV_SOURCE_PATH)/CMKVFileReader.cpp \
	$(MKV_SOURCE_PATH)/CMKVParser.cpp \
	$(MKV_SOURCE_PATH)/CMKVStreamFileIndex.cpp \
	$(MKV_SOURCE_PATH)/CMKVTrack.cpp \
	$(MKV_SOURCE_PATH)/CMKVVideoTrack.cpp \
	$(MKV_SOURCE_PATH)/CMKVVideoTrack_RealSpecialize.cpp \
	$(MKV_SOURCE_PATH)/EBML.cpp
