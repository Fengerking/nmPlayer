

ADAPTIVE_PATH       :=../../../../../AdaptiveStreaming
MAINCMNSRC_PATH     :=../../../../../Common
INC_PATH            :=../../../../../../Include
CMNSRC_PATH         :=../../../../../../Common

# please list all objects needed by your module here
VOMSRC:= ../../../../../File/Common/CBasePushPaser.cpp \
	       ../../../../../File/Audio/CID3Parser.cpp \
	  \
    $(ADAPTIVE_PATH)/Common/voFormatChecker.cpp \
	  $(ADAPTIVE_PATH)/BA/voBitrateAdaptationImp.cpp \
	  $(ADAPTIVE_PATH)/BA/voDelayTimeMap.cpp \
	  $(ADAPTIVE_PATH)/DRM/AdaptiveStreamingDRM.cpp \
	  $(ADAPTIVE_PATH)/DRM/voStreamingDRM.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingControllerInfo.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingTimeStampSync.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingFileParser.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingFileParser_aac.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingFileParser_mp4.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingFileParser_smth.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingFileParser_ts.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingFileParser_ID3.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingFileParser_unknow.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingFileParser_Subtitle.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingClassFactory.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingController.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamingControllerAPI.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voAdaptiveStreamParserWrapper.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voChannelItemThread.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voChannelItemThreadHLS.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voChannelItemThreadDASH.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voChannelItemThreadISS.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voItemThread.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/vo_timestamp_recalculator.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voProgramInfoOp.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voTrackInfoGroup.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/SourceSelecter.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/vo_tracksample_filter.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voTimestampRollbackInfo.cpp \
	$(ADAPTIVE_PATH)/EventHandling/voEVENTHandler.cpp \
	$(ADAPTIVE_PATH)/EventHandling/CommonError.cpp \
	$(ADAPTIVE_PATH)/EventHandling/EventHandler.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/Cache/CacheManager.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/Cache/ChunkDataCache.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/Cache/StreamCache.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/Cache/MediaCache.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/Cache/TrackCache.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voBufferCommander.cpp \
	$(ADAPTIVE_PATH)/Controller/Source/voDownloadBufferMgr.cpp \
    \
    $(MAINCMNSRC_PATH)/vo_thread.cpp \
	  $(MAINCMNSRC_PATH)/CSourceIOUtility.cpp \
	  $(MAINCMNSRC_PATH)/voBufferIO.cpp \
	  $(MAINCMNSRC_PATH)/indent_file_writer.cpp \
	  $(MAINCMNSRC_PATH)/SourceInfoXMLDumper.cpp \
	  $(MAINCMNSRC_PATH)/xml_document.cpp \
	  $(MAINCMNSRC_PATH)/xml_element.cpp \
  	$(MAINCMNSRC_PATH)/xml_writer.cpp \
  	$(MAINCMNSRC_PATH)/Buffer_OptimizeBA/MediaBuffer.cpp\
	  $(MAINCMNSRC_PATH)/Buffer_OptimizeBA/SubtitleTrackBuffer.cpp\
	  $(MAINCMNSRC_PATH)/Buffer_OptimizeBA/TrackBuffer.cpp\
	  $(MAINCMNSRC_PATH)/Buffer_OptimizeBA/VideoTrackBuffer.cpp\
	  $(MAINCMNSRC_PATH)/Buffer_OptimizeBA/voSourceBufferManager_AI.cpp \
    \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/voCMutex.cpp \
    $(CMNSRC_PATH)/voCSemaphore.cpp \
    $(CMNSRC_PATH)/fAudioHeadDataInfo.cpp \
	  $(CMNSRC_PATH)/CFileFormatCheck.cpp \
	  $(CMNSRC_PATH)/voProgramInfo.cpp \
	  $(CMNSRC_PATH)/cmnVOMemory.cpp \
	  $(CMNSRC_PATH)/NetWork/vo_socket.cpp \
    $(CMNSRC_PATH)/voLog.c \
    $(CMNSRC_PATH)/CDllLoad.cpp \
    $(CMNSRC_PATH)/voHalInfo.cpp \
    $(CMNSRC_PATH)/voOSFunc_iOS.mm

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
    $(INC_PATH) \
    $(CMNSRC_PATH)/iOS/ \
    $(CMNSRC_PATH) \
    ../../../../../../Common/NetWork \
	../../../../../Common \
	../../../../../Common/Buffer_OptimizeBA \
		../../../../../Include \
        ../../../../../AdaptiveStreaming/Common \
        ../../../../../AdaptiveStreaming/BA \
        ../../../../../AdaptiveStreaming/Buffer \
	../../../../../AdaptiveStreaming/DRM \
        ../../../../../AdaptiveStreaming/Controller/Source \
	../../../../../AdaptiveStreaming/Include \
        ../../../../../../Thirdparty/OpenSSL \
        ../../../../../AdaptiveStreaming/EventHandling \
        ../../../../../File/Common \
        ../../../../../File/Audio \
        ../../../../../File/ID3 \
../../../../../AdaptiveStreaming/Controller/Source/Cache
