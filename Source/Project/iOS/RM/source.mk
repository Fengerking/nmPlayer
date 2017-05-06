# Source files for rm file parser.
# rodney zhang
# 2012.03.23


# path variables for rm file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_INCLUDE_PATH:=$(SOURCE_PATH)/Include
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
FILE_ID3_PATH:=$(SOURCE_FILE_PATH)/ID3
METADATA_SOURCE_PATH:=$(SOURCE_FILE_PATH)/MetaData
RM_SOURCE_PATH:=$(SOURCE_FILE_PATH)/RM
SOURCE_COMMON_PATH:=$(SOURCE_PATH)/Common
COMMON_BUFF_PATH:=$(SOURCE_COMMON_PATH)/Buffer


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(SOURCE_INCLUDE_PATH) \
	$(FILE_COMMON_PATH) \
	$(FILE_ID3_PATH) \
	$(METADATA_SOURCE_PATH) \
	$(RM_SOURCE_PATH)	\
	$(COMMON_BUFF_PATH)	\
	$(SOURCE_COMMON_PATH) 


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
#	$(TRUNK_COMMON_PATH)/fVideoHeadDataInfo.cpp \
#	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/fVideoHeadDataInfo.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
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
	$(FILE_COMMON_PATH)/voSourceBaseWrapper.cpp \
	$(FILE_COMMON_PATH)/voSource2ParserWrapper.cpp \
	$(FILE_COMMON_PATH)/CSourceIOSwitch.cpp \
	\
	$(METADATA_SOURCE_PATH)/CBaseMetaData.cpp \
	$(METADATA_SOURCE_PATH)/CRealMetaData.cpp \
	\
	$(RM_SOURCE_PATH)/CRealAudioTrack.cpp \
	$(RM_SOURCE_PATH)/CRealParser.cpp \
	$(RM_SOURCE_PATH)/CRealReader.cpp \
	$(RM_SOURCE_PATH)/CRealTrack.cpp \
	$(RM_SOURCE_PATH)/CRealVideoTrack.cpp	\
	$(FILE_COMMON_PATH)/voSource2WrapperPushPlay.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperManager.cpp \
	$(TRUNK_COMMON_PATH)/voCSemaphore.cpp \
	$(COMMON_BUFF_PATH)/voSourceBufferManager.cpp	\
	$(COMMON_BUFF_PATH)/voSourceDataBuffer.cpp	\
	$(COMMON_BUFF_PATH)/voSourceSubtitleDataBuffer.cpp	\
	$(COMMON_BUFF_PATH)/voSourceVideoDataBuffer.cpp	\
	$(SOURCE_COMMON_PATH)/vo_thread.cpp	\
	$(SOURCE_COMMON_PATH)/voProgramInfo.cpp	\
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm
	
