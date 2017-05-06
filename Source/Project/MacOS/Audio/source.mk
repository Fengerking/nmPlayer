# Source files for AUDIO file parser.
# Dolby Du
# 2013.08.28


# path variables for AUDIO file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_COMMON:=$(SOURCE_PATH)/Common
SOURCE_COMMON_BUF:=$(SOURCE_COMMON)/Buffer
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
AUDIO_SOURCE_PATH:=$(SOURCE_FILE_PATH)/Audio
METADATA_SOURCE_PATH:=$(SOURCE_FILE_PATH)/MetaData
ID3_SOURCE_PATH:=$(SOURCE_FILE_PATH)/ID3
SRCINCLUDE_PATH:=$(SOURCE_PATH)/Include


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(SOURCE_COMMON) \
	$(SOURCE_COMMON_BUF) \
	$(FILE_COMMON_PATH) \
	$(METADATA_SOURCE_PATH) \
	$(ID3_SOURCE_PATH) \
	$(AUDIO_SOURCE_PATH) \
	$(SRCINCLUDE_PATH) \


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/CFileFormatCheck.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
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
	$(FILE_COMMON_PATH)/CvoBaseDrmCallback.cpp \
	$(FILE_COMMON_PATH)/CvoBaseFileOpr.cpp \
	$(FILE_COMMON_PATH)/CvoBaseMemOpr.cpp \
	$(FILE_COMMON_PATH)/CvoFileParser.cpp \
	$(FILE_COMMON_PATH)/CMemPool.cpp \
	$(FILE_COMMON_PATH)/fCodec.cpp \
	$(FILE_COMMON_PATH)/voFileReader.cpp \
	$(FILE_COMMON_PATH)/voSource2ParserWrapper.cpp \
	$(FILE_COMMON_PATH)/voSourceBaseWrapper.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperManager.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperPushPlay.cpp \
	$(FILE_COMMON_PATH)/CSourceIOSwitch.cpp \
	$(FILE_COMMON_PATH)/CGFileChunk.cpp \
	$(FILE_COMMON_PATH)/CBasePushPaser.cpp \
	\
	$(METADATA_SOURCE_PATH)/CBaseMetaData.cpp \
	$(METADATA_SOURCE_PATH)/CID3MetaData.cpp \
	\
	$(AUDIO_SOURCE_PATH)/CBaseAudioReader.cpp \
	$(AUDIO_SOURCE_PATH)/ape_reader.cpp \
	$(AUDIO_SOURCE_PATH)/CAacReader.cpp \
	$(AUDIO_SOURCE_PATH)/CAacParser.cpp \
	$(AUDIO_SOURCE_PATH)/voAAC2.cpp \
	$(AUDIO_SOURCE_PATH)/CSource2AAC.cpp \
	$(AUDIO_SOURCE_PATH)/CAmrReader.cpp \
	$(AUDIO_SOURCE_PATH)/CAuReader.cpp \
	$(AUDIO_SOURCE_PATH)/CAC3Reader.cpp \
	$(AUDIO_SOURCE_PATH)/CFlacReader.cpp \
	$(AUDIO_SOURCE_PATH)/CMp3Reader.cpp \
	$(AUDIO_SOURCE_PATH)/CQcpReader.cpp \
	$(AUDIO_SOURCE_PATH)/CWavReader.cpp \
	$(AUDIO_SOURCE_PATH)/DTSReader.cpp \
	$(AUDIO_SOURCE_PATH)/CID3Parser.cpp \
	$(AUDIO_SOURCE_PATH)/CMemStream.cpp \
	$(AUDIO_SOURCE_PATH)/CBaseAudioPushParser.cpp \
	$(AUDIO_SOURCE_PATH)/voSource2AudioPushParser.cpp \
	$(AUDIO_SOURCE_PATH)/CMp3PushParser.cpp
