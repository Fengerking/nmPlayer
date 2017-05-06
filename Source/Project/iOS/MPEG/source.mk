# Source files for MPEG file parser.
# rodney zhang
# 2012.03.28


# path variables for MPEG file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_COMMON:=$(SOURCE_PATH)/Common
SOURCE_COMMON_BUF:=$(SOURCE_COMMON)/Buffer
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
MPEG_SOURCE_PATH:=$(SOURCE_FILE_PATH)/MPEG
METADATA_SOURCE_PATH:=$(SOURCE_FILE_PATH)/MetaData
SRCINCLUDE_PATH:=../../../../Include

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		$(TRUNK_INCLUDE_PATH) \
		$(TRUNK_COMMON_PATH) \
		$(SOURCE_COMMON) \
		$(SOURCE_COMMON_BUF) \
		$(FILE_COMMON_PATH) \
		$(METADATA_SOURCE_PATH) \
		$(MPEG_SOURCE_PATH) \
		$(SRCINCLUDE_PATH) \		


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/fVideoHeadDataInfo.cpp \
	$(TRUNK_COMMON_PATH)/voCSemaphore.cpp \
	$(TRUNK_COMMON_PATH)/voProgramInfo.cpp \
	\
	$(SOURCE_COMMON_BUF)/voSourceBufferManager.cpp \
	$(SOURCE_COMMON_BUF)/voSourceDataBuffer.cpp \
	$(SOURCE_COMMON_BUF)/voSourceSubtitleDataBuffer.cpp \
	$(SOURCE_COMMON_BUF)/voSourceVideoDataBuffer.cpp \
	\
	$(SOURCE_COMMON)/vo_thread.cpp \
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
	$(FILE_COMMON_PATH)/fCodec.cpp \
	$(FILE_COMMON_PATH)/voFileReader.cpp \
	$(FILE_COMMON_PATH)/voSource2ParserWrapper.cpp \
	$(FILE_COMMON_PATH)/voSourceBaseWrapper.cpp \
	$(FILE_COMMON_PATH)/CSourceIOSwitch.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperManager.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperPushPlay.cpp \
	\
	$(MPEG_SOURCE_PATH)/CMpegAudioTrack.cpp \
	$(MPEG_SOURCE_PATH)/CMpegAVCVideoTrack.cpp \
	$(MPEG_SOURCE_PATH)/CMpegParser.cpp \
	$(MPEG_SOURCE_PATH)/CMpegReader.cpp \
	$(MPEG_SOURCE_PATH)/CMpegTrack.cpp \
	$(MPEG_SOURCE_PATH)/CMpegVideoTrack.cpp \
	$(MPEG_SOURCE_PATH)/CMpegFileIndex.cpp \
