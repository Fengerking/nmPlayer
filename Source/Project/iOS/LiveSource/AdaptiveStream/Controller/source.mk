# Source files for Contorller TWO file parser.
# rodney zhang
# 2012.08.09


# path variables for Contorller file parser.
TRUNK_PATH:=../../../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common
TRUNK_COMMON_NETWORK_PATH:=$(TRUNK_COMMON_PATH)/NetWork
TRUNK_UTILITY_VOUTF8CONV_PATH:=$(TRUNK_PATH)/Utility/voutf8conv

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
ADAPTIVESTREAMING_PATH:=$(SOURCE_PATH)/AdaptiveStreaming


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(SOURCE_FILE_PATH)/ID3 \
	$(TRUNK_COMMON_NETWORK_PATH) \
	$(TRUNK_UTILITY_VOUTF8CONV_PATH) \
	$(SOURCE_ADAPTIVESTREAMING_COMMON_PATH) \
	$(ADAPTIVESTREAMING_PATH)/DRM \
	$(ADAPTIVESTREAMING_PATH)/BA \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source \
	$(ADAPTIVESTREAMING_PATH)/Common \
	$(TRUNK_PATH)/Thirdparty/OpenSSL \
	$(SOURCE_PATH)/Common \
	$(SOURCE_PATH)/Include \
	$(SOURCE_PATH)/Common/Buffer_OptimizeBA \
	$(ADAPTIVESTREAMING_PATH)/EventHandling \
	$(SOURCE_PATH)/File/Common \
	$(SOURCE_PATH)/File/Audio \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/Cache \



# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/voCSemaphore.cpp \
	$(TRUNK_COMMON_PATH)/fAudioHeadDataInfo.cpp \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/CFileFormatCheck.cpp \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/NetWork/vo_socket.cpp \
	$(TRUNK_COMMON_PATH)/voProgramInfo.cpp \
	\
	$(SOURCE_PATH)/Common/CSourceIOUtility.cpp \
	$(SOURCE_PATH)/Common/vo_thread.cpp \
	$(ADAPTIVESTREAMING_PATH)/Common/voFormatChecker.cpp \
	$(ADAPTIVESTREAMING_PATH)/BA/voBitrateAdaptationImp.cpp \
	$(ADAPTIVESTREAMING_PATH)/BA/voDelayTimeMap.cpp \
	$(SOURCE_PATH)/Common/Buffer_OptimizeBA/MediaBuffer.cpp\
	$(SOURCE_PATH)/Common/Buffer_OptimizeBA/SubtitleTrackBuffer.cpp\
	$(SOURCE_PATH)/Common/Buffer_OptimizeBA/TrackBuffer.cpp\
	$(SOURCE_PATH)/Common/Buffer_OptimizeBA/VideoTrackBuffer.cpp\
	$(SOURCE_PATH)/Common/Buffer_OptimizeBA/voSourceBufferManager_AI.cpp \
	$(SOURCE_PATH)/Common/voBufferIO.cpp \
	$(SOURCE_PATH)/Common/indent_file_writer.cpp \
	$(SOURCE_PATH)/Common/SourceInfoXMLDumper.cpp \
	$(SOURCE_PATH)/Common/xml_document.cpp \
	$(SOURCE_PATH)/Common/xml_element.cpp \
	$(SOURCE_PATH)/Common/xml_writer.cpp \
	\
	$(ADAPTIVESTREAMING_PATH)/DRM/AdaptiveStreamingDRM.cpp \
	$(ADAPTIVESTREAMING_PATH)/DRM/voStreamingDRM.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingControllerInfo.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingTimeStampSync.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingFileParser.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingFileParser_aac.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingFileParser_mp4.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingFileParser_ts.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingFileParser_ID3.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingFileParser_smth.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingFileParser_unknow.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingFileParser_Subtitle.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingClassFactory.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingController.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamingControllerAPI.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voAdaptiveStreamParserWrapper.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voChannelItemThread.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voChannelItemThreadHLS.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voChannelItemThreadISS.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voChannelItemThreadDASH.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voItemThread.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/vo_timestamp_recalculator.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voProgramInfoOp.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voTrackInfoGroup.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/SourceSelecter.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/vo_tracksample_filter.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voTimestampRollbackInfo.cpp \
	$(ADAPTIVESTREAMING_PATH)/EventHandling/voEVENTHandler.cpp \
	$(ADAPTIVESTREAMING_PATH)/EventHandling/CommonError.cpp \
	$(ADAPTIVESTREAMING_PATH)/EventHandling/EventHandler.cpp \
	\
	$(SOURCE_PATH)/File/Common/CBasePushPaser.cpp \
	$(SOURCE_PATH)/File/Audio/CID3Parser.cpp \
	\
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/Cache/CacheManager.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/Cache/ChunkDataCache.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/Cache/StreamCache.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/Cache/MediaCache.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/Cache/TrackCache.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voBufferCommander.cpp \
	$(ADAPTIVESTREAMING_PATH)/Controller/Source/voDownloadBufferMgr.cpp \
	\

