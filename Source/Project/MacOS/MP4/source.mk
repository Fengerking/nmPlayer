
CMNSRC_PATH:= ../../../../../Common
CSRC_PATH:= ../../../../File/MP4
CSRCLOCALMN_PATH:= ../../../../File/Common
CSRCMN_PATH:= ../../../../Common
CMDSRC_PATH:= ../../../../File/MetaData
CSOURCE_PATH:= ../../../../../Source
SRCINCLUDE_PATH:=../../../../Include

# please list all objects needed by your module here
VOMSRC:= \
	$(CMDSRC_PATH)/CMP4MetaData.cpp \
	$(CMDSRC_PATH)/CBaseMetaData.cpp \
	$(CMDSRC_PATH)/CID3MetaData.cpp \
	$(CMDSRC_PATH)/CMP4MetaData.cpp \
	\
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_http_stream.cpp \
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_mem_stream.cpp \
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_stream.cpp \
	$(CSOURCE_PATH)/SMTHParser/voDrmCallback.cpp \
	\
	$(CSRC_PATH)/ISOMedia/3gpTTextOp.cpp \
	$(CSRC_PATH)/ISOMedia/fragmentTrack.cpp \
	$(CSRC_PATH)/ISOMedia/isomrd.cpp \
	$(CSRC_PATH)/ISOMedia/isomscan.cpp \
	$(CSRC_PATH)/ISOMedia/subtitleTrack.cpp \
	$(CSRC_PATH)/ISOMedia/SegmentTrack.cpp \
	$(CSRC_PATH)/MP4Base/mp4cfg.cpp \
	$(CSRC_PATH)/MP4Base/mp4desc.cpp \
	$(CSRC_PATH)/MP4Base/mpxio.cpp \
	$(CSRC_PATH)/MP4Base/mpxobj.cpp \
	$(CSRC_PATH)/MP4Base/mpxutil.cpp \
	$(CSRC_PATH)/Reader/CMp4BaseFileStream.cpp \
	$(CSRC_PATH)/Reader/CMp4FileStream.cpp \
	$(CSRC_PATH)/Reader/CMp4PushFileStream.cpp \
	$(CSRC_PATH)/Reader/CMp4PushReader2.cpp \
	$(CSRC_PATH)/Reader/CMp4Reader2.cpp \
	$(CSRC_PATH)/Reader/CMp4Track2.cpp \
	$(CSRC_PATH)/Reader/CMp4TrackPushPlay.cpp \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/cmnVOMemory.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/NetWork/vo_socket.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	\
	$(CSRCLOCALMN_PATH)/CBaseReader.cpp \
	$(CSRCLOCALMN_PATH)/CBaseTrack.cpp \
	$(CSRCLOCALMN_PATH)/CGFileChunk.cpp \
	$(CSRCLOCALMN_PATH)/CMemPool.cpp \
	$(CSRCLOCALMN_PATH)/CvoBaseDrmCallback.cpp \
	$(CSRCLOCALMN_PATH)/CvoBaseFileOpr.cpp \
	$(CSRCLOCALMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRCLOCALMN_PATH)/CvoFileParser.cpp \
	$(CSRCLOCALMN_PATH)/fCodec.cpp \
	$(CSRCLOCALMN_PATH)/voSource2WrapperManager.cpp \
	$(CSRCLOCALMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRCLOCALMN_PATH)/voSource2ParserWrapper.cpp \
	$(CSRCLOCALMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRCLOCALMN_PATH)/CSourceIOSwitch.cpp \
	$(CSRCLOCALMN_PATH)/base64.cpp \
	$(CSRCLOCALMN_PATH)/strutil.cpp \
	$(CSRCLOCALMN_PATH)/fortest.cpp \
	$(CSRCLOCALMN_PATH)/voFileReader.cpp \
	$(CSRCLOCALMN_PATH)/voStreamReader.cpp \
	$(CSRCLOCALMN_PATH)/vo_file_io.cpp \
	$(CSRCLOCALMN_PATH)/vo_file_parser.cpp \
	\
	$(CSRCMN_PATH)/vo_thread.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceBufferManager.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(CSRCMN_PATH)/Buffer/voSourceVideoDataBuffer.cpp
	
	
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRC_PATH)/ISOMedia \
	$(CSRC_PATH)/MP4Base \
	$(CSRC_PATH)/Reader \
	$(CSRCLOCALMN_PATH) \
	$(CSRCMN_PATH)/Buffer \
	$(CMDSRC_PATH) \
	$(CSOURCE_PATH)/IISSmoothStreaming \
	$(CSOURCE_PATH)/SMTHParser \
	$(CMNSRC_PATH)/NetWork \
	$(CSRCMN_PATH) \
	$(SRCINCLUDE_PATH)

