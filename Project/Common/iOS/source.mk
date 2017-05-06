
CMNSRC_PATH:=../../../../Common
A_RENDER_PATH:=../../../../Render/Audio/Source
V_RENDER_PATH:=../../../../Render/Video/Render/Source

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:=    $(CMNSRC_PATH)/CAndroidUtilities.cpp \
    $(CMNSRC_PATH)/CBaseConfig.cpp \
    $(CMNSRC_PATH)/CDllLoad.cpp \
    $(CMNSRC_PATH)/CDrmEngine.cpp \
    $(CMNSRC_PATH)/CFileFormatCheck.cpp \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/cmnVOMemory.cpp \
    $(CMNSRC_PATH)/CPtrList.cpp \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/fAudioHeadDataInfo.cpp \
    $(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
    $(CMNSRC_PATH)/voCBaseThread.cpp \
    $(CMNSRC_PATH)/voCMutex.cpp \
    $(CMNSRC_PATH)/voCSemaphore.cpp \
    $(CMNSRC_PATH)/voLog.c \
    $(CMNSRC_PATH)/voLogoData.cpp \
    $(CMNSRC_PATH)/voOSFunc_iOS.mm \
    $(CMNSRC_PATH)/voThread.cpp \
    $(CMNSRC_PATH)/XMLSaxDefaultHandler.cpp \
    $(CMNSRC_PATH)/CVideoSubtitle.cpp \
    $(CMNSRC_PATH)/voSubtitleFunc.cpp \
    $(CMNSRC_PATH)/NetWork/CUdpClientRecv.cpp \
    $(CMNSRC_PATH)/NetWork/CUdpClientSource.cpp \
    $(CMNSRC_PATH)/NetWork/vo_socket.cpp \
    $(CMNSRC_PATH)/Buffer/voSourceBufferManager.cpp \
    $(CMNSRC_PATH)/Buffer/voSourceDataBuffer.cpp \
    $(CMNSRC_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
    $(CMNSRC_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
    $(CMNSRC_PATH)/voXMLLoad.cpp \
    $(CMNSRC_PATH)/voHalInfo.cpp \
    $(A_RENDER_PATH)/CBaseAudioRender.cpp \
    $(A_RENDER_PATH)/iOS/CAudioQueueRender.cpp \
    $(A_RENDER_PATH)/iOS/CAudioQueueService.cpp \
    $(A_RENDER_PATH)/iOS/CAudioUnitRender.cpp \
    $(A_RENDER_PATH)/iOS/CWaveOutAudio.cpp \
    $(V_RENDER_PATH)/CBaseVideoRender.cpp \
    $(V_RENDER_PATH)/CCCRRRFunc.cpp \
    $(V_RENDER_PATH)/iOS/CiOSVideoRender.mm \
    $(V_RENDER_PATH)/iOS/GLRenderExt.mm \
    $(V_RENDER_PATH)/iOS/voGLRenderManager.mm 


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		../../../../Include \
		$(CMNSRC_PATH) \
		$(CMNSRC_PATH)/NetWork \
		$(CMNSRC_PATH)/Buffer \
		$(A_RENDER_PATH) \
		$(A_RENDER_PATH)/iOS \
		$(V_RENDER_PATH) \
		$(V_RENDER_PATH)/iOS
		
#		../../../../Source/File/XML \
