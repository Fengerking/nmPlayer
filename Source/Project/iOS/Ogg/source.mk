# Source files for ogg file parser.
# rodney zhang
# 2012.03.22


# path variables for ogg file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common
SRCMAINCMN_PATH:=$(TRUNK_PATH)/Source/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
SOURCE_INCLUDE_PATH:=$(SOURCE_PATH)/Include
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
METADATA_SOURCE_PATH:=$(SOURCE_FILE_PATH)/MetaData
OGG_SOURCE_PATH:=$(SOURCE_FILE_PATH)/Ogg


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		$(TRUNK_INCLUDE_PATH) \
		$(TRUNK_COMMON_PATH) \
		$(FILE_COMMON_PATH) \
		$(METADATA_SOURCE_PATH) \
		$(OGG_SOURCE_PATH) \
		$(SRCMAINCMN_PATH) \
		$(SRCMAINCMN_PATH)/Buffer \
		$(SOURCE_INCLUDE_PATH)


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
    $(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/fVideoHeadDataInfo.cpp \
    	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/voProgramInfo.cpp \
    	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
    	$(TRUNK_COMMON_PATH)/CFileFormatCheck.cpp \
    	$(FILE_COMMON_PATH)/CBaseReader.cpp \
    	$(FILE_COMMON_PATH)/CvoBaseDrmCallback.cpp \
    	$(FILE_COMMON_PATH)/CvoBaseFileOpr.cpp \
    	$(FILE_COMMON_PATH)/CvoBaseMemOpr.cpp \
	$(FILE_COMMON_PATH)/CvoFileParser.cpp \
	$(FILE_COMMON_PATH)/CBaseStreamFileIndex.cpp \
	$(FILE_COMMON_PATH)/CBaseStreamFileReader.cpp \
	$(FILE_COMMON_PATH)/CBaseStreamFileTrack.cpp \
	$(FILE_COMMON_PATH)/CBaseTrack.cpp \
	$(FILE_COMMON_PATH)/CGBuffer.cpp \
	$(FILE_COMMON_PATH)/CGFileChunk.cpp \
    	$(FILE_COMMON_PATH)/CMemPool.cpp \
    	$(FILE_COMMON_PATH)/fCodec.cpp \
    	$(FILE_COMMON_PATH)/voFileReader.cpp \
	$(FILE_COMMON_PATH)/voSource2ParserWrapper.cpp \
	$(FILE_COMMON_PATH)/voSourceBaseWrapper.cpp \
	$(FILE_COMMON_PATH)/CSourceIOSwitch.cpp \
	$(OGG_SOURCE_PATH)/oggparser.cpp\
	$(OGG_SOURCE_PATH)/oggreader.cpp \
	$(OGG_SOURCE_PATH)/oggtrack.cpp \
	$(OGG_SOURCE_PATH)/ogg_vorbis_track.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperPushPlay.cpp \
	$(FILE_COMMON_PATH)/voSource2WrapperManager.cpp \
	$(SRCMAINCMN_PATH)/Buffer/voSourceBufferManager.cpp \
	$(SRCMAINCMN_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(SRCMAINCMN_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(SRCMAINCMN_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	$(SRCMAINCMN_PATH)/vo_thread.cpp \
	$(TRUNK_COMMON_PATH)/voCSemaphore.cpp
