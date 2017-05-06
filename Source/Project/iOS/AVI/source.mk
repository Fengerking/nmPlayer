

CMNSRC_PATH:=../../../../../Common
CSRC_PATH:=../../../../File/AVI
CSRCMN_PATH:=../../../../File/Common
CMDSRC_PATH:=../../../../File/MetaData
CSRSCMN_PATH:=../../../../Common
CSRSCMNBF_PATH:=../../../../Common/Buffer

# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CSRC_PATH)/CAviAudioTrack.cpp \
	$(CSRC_PATH)/CAviIndex.cpp \
	$(CSRC_PATH)/CAviParser.cpp \
	$(CSRC_PATH)/CAviReader.cpp \
	$(CSRC_PATH)/CAviTrack.cpp \
	$(CSRC_PATH)/CAviVideoTrack.cpp \
	$(CSRC_PATH)/CAviMpeg2VideoTrack.cpp \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnVOMemory.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CSRCMN_PATH)/CBaseReader.cpp \
	$(CSRCMN_PATH)/CBaseStreamFileIndex.cpp \
	$(CSRCMN_PATH)/CBaseStreamFileReader.cpp \
	$(CSRCMN_PATH)/CBaseStreamFileTrack.cpp \
	$(CSRCMN_PATH)/CBaseTrack.cpp \
	$(CSRCMN_PATH)/CGBuffer.cpp \
	$(CSRCMN_PATH)/CGFileChunk.cpp \
	$(CSRCMN_PATH)/CMemPool.cpp \
	$(CSRCMN_PATH)/CvoBaseDrmCallback.cpp \
	$(CSRCMN_PATH)/CvoBaseFileOpr.cpp \
	$(CSRCMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRCMN_PATH)/CvoFileParser.cpp \
	$(CSRCMN_PATH)/fCodec.cpp \
	$(CSRCMN_PATH)/voFileReader.cpp \
	$(CSRCMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRCMN_PATH)/voSource2ParserWrapper.cpp \
	$(CSRCMN_PATH)/CSourceIOSwitch.cpp \
	$(CSRCMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRCMN_PATH)/voSource2WrapperManager.cpp \
	$(CSRSCMNBF_PATH)/voSourceBufferManager.cpp \
	$(CSRSCMNBF_PATH)/voSourceDataBuffer.cpp \
	$(CSRSCMNBF_PATH)/voSourceSubtitleDataBuffer.cpp \
	$(CSRSCMNBF_PATH)/voSourceVideoDataBuffer.cpp \
	$(CSRSCMN_PATH)/vo_thread.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../../Include \
	../../../../Include \
	../../../../File/OGG \
	$(CSRC_PATH) \
	$(CMNSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CMDSRC_PATH) \
	$(CSRSCMN_PATH) \
	$(CSRSCMNBF_PATH)
	
