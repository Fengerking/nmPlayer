

MFWSRC_PATH   :=../../../Source
MFWCMNSRC_PATH:=../../../../Common

CMNSRC_PATH   :=../../../../../Common
A_RENDER_PATH :=../../../../../Render/Audio/Source
V_RENDER_PATH :=../../../../../Render/Video/Render/Source

# please list all objects needed by your module here
VOMSRC:=    $(MFWCMNSRC_PATH)/voCBasePlayer.cpp \
    $(MFWCMNSRC_PATH)/voCDataBuffer.cpp \
    $(MFWCMNSRC_PATH)/voCDataSource.cpp \
    $(MFWCMNSRC_PATH)/voCDataSourceOP.cpp \
    $(MFWCMNSRC_PATH)/voCEditPlayer.cpp \
    $(MFWCMNSRC_PATH)/voCLiveDataSource.cpp \
    $(MFWCMNSRC_PATH)/voCMediaPlayer.cpp \
    $(MFWCMNSRC_PATH)/voCMediaPlayerInit.cpp \
    $(MFWCMNSRC_PATH)/voCMediaPlayerMeta.cpp \
    $(MFWCMNSRC_PATH)/voCAudioRenderBufferMgr.cpp \
    $(MFWCMNSRC_PATH)/voLicenseMng.cpp \
    $(MFWCMNSRC_PATH)/voMP.cpp \
    $(MFWCMNSRC_PATH)/voCPlayerLogo.cpp \
    $(MFWSRC_PATH)/CAudioDecoder.cpp \
    $(MFWSRC_PATH)/CAudioEffect.cpp \
    $(MFWSRC_PATH)/CAudioSpeed.cpp \
    $(MFWSRC_PATH)/CBaseLiveSource.cpp \
    $(MFWSRC_PATH)/CBaseNode.cpp \
    $(MFWSRC_PATH)/CBaseSource.cpp \
    $(MFWSRC_PATH)/CFileSource.cpp \
    $(MFWSRC_PATH)/CHTTPPDSource.cpp \
    $(MFWSRC_PATH)/CRTSPSource.cpp \
    $(MFWSRC_PATH)/CVideoDecoder.cpp \
    $(MFWSRC_PATH)/CVideoEffect.cpp \
    \
    $(CMNSRC_PATH)/voLog.c \
    $(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
    $(CMNSRC_PATH)/voOSFunc_iOS.mm \
    $(CMNSRC_PATH)/CAndroidUtilities.cpp \
    $(CMNSRC_PATH)/cmnVOMemory.cpp \
    $(CMNSRC_PATH)/CBaseConfig.cpp \
    $(CMNSRC_PATH)/CDllLoad.cpp \
    $(CMNSRC_PATH)/CFileFormatCheck.cpp \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/CPtrList.cpp \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/voCBaseThread.cpp \
    $(CMNSRC_PATH)/voCMutex.cpp \
    $(CMNSRC_PATH)/voCSemaphore.cpp \
    $(CMNSRC_PATH)/voThread.cpp \
    $(CMNSRC_PATH)/voHalInfo.cpp \
    $(CMNSRC_PATH)/iOS/voRunRequestOnMain.mm \
    $(CMNSRC_PATH)/iOS/voRunRequestOnMainObjC.mm \
    $(CMNSRC_PATH)/iOS/voSystemStatus.mm \
    \
    $(A_RENDER_PATH)/CBaseAudioRender.cpp \
    $(A_RENDER_PATH)/MacOS/CWaveOutAudio.cpp \
    $(A_RENDER_PATH)/MacOS/CAudioQueueRender.cpp \
    $(V_RENDER_PATH)/CBaseVideoRender.cpp \
    $(V_RENDER_PATH)/CCCRRRFunc.cpp \
    $(V_RENDER_PATH)/MacOS/CMacOSVideoRender.mm \
    $(V_RENDER_PATH)/MacOS/voGLRenderBase.mm \


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		../../../../../Include \
		$(MFWSRC_PATH) \
		$(MFWCMNSRC_PATH) \
    $(CMNSRC_PATH)/iOS/ \
    $(CMNSRC_PATH) \
    $(CMNSRC_PATH)/Buffer \
    $(A_RENDER_PATH) \
    $(A_RENDER_PATH)/MacOS \
    $(V_RENDER_PATH) \
    $(V_RENDER_PATH)/MacOS
