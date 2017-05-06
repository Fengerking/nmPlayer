

OSTREAMCMNSRC_PATH  :=../../../Source/Common
OSTREAMIOSSRC_PATH  :=../../../Source/iOS
OSTREAMLOADSRC_PATH :=$(OSTREAMIOSSRC_PATH)/LoadConrol
OSTREAMRENDER_PATH  :=$(OSTREAMIOSSRC_PATH)/Render
INC_PATH            :=../../../../../Include
CMNSRC_PATH         :=../../../../../Common
OSTREAM_CC_RENDER   :=$(OSTREAMIOSSRC_PATH)/SubTitleRender/
A_RENDER_PATH       :=../../../../../Render/Audio/Source
V_RENDER_PATH       :=../../../../../Render/Video/Render/Source

# please list all objects needed by your module here
VOMSRC:= $(OSTREAMCMNSRC_PATH)/COSVomeEngine.cpp \
    $(OSTREAMCMNSRC_PATH)/COSVomePlayer.cpp \
    $(OSTREAMCMNSRC_PATH)/COSBasePlayer.cpp \
    $(OSTREAMCMNSRC_PATH)/COMXALPlayer.cpp \
    $(OSTREAMCMNSRC_PATH)/COMXALEngine.cpp \
    $(OSTREAMCMNSRC_PATH)/CSEIInfo.cpp \
    \
    $(OSTREAMIOSSRC_PATH)/voOSPlayer.mm \
    $(OSTREAMIOSSRC_PATH)/voOnStreamEngine.cpp \
    $(OSTREAMIOSSRC_PATH)/voOnStreamEngine_ns.mm \
    $(OSTREAMIOSSRC_PATH)/voOSPlayerData.mm \
    $(OSTREAMIOSSRC_PATH)/voOSPlayerAdapter.mm \
    $(OSTREAMIOSSRC_PATH)/voOSNPPlayer.mm \
    $(OSTREAMIOSSRC_PATH)/voPlayerView.mm \
    \
    $(OSTREAMRENDER_PATH)/voBaseRender.cpp \
    $(OSTREAMRENDER_PATH)/CAudioRenderManager.cpp \
    $(OSTREAMRENDER_PATH)/CVideoRenderManager.cpp \
    $(OSTREAMRENDER_PATH)/voNPRenderIOS.mm \
    \
    $(OSTREAM_CC_RENDER)/voCCManager.mm \
    $(OSTREAM_CC_RENDER)/voSubTitleRender.mm \
    $(OSTREAM_CC_RENDER)/voTextInfoEntryView.mm \
    $(OSTREAM_CC_RENDER)/voRenderObjectInfo.mm \
    $(OSTREAM_CC_RENDER)/voSubtitleProperty.mm \
    \
    $(OSTREAMLOADSRC_PATH)/voRealVideo.cpp \
    $(OSTREAMLOADSRC_PATH)/voWMA.cpp \
    $(OSTREAMLOADSRC_PATH)/voDIVX3.cpp \
    $(OSTREAMLOADSRC_PATH)/voRealAudio.cpp \
    $(OSTREAMLOADSRC_PATH)/voAAC.cpp \
    $(OSTREAMLOADSRC_PATH)/voEAC3.cpp \
    $(OSTREAMLOADSRC_PATH)/voWMV.cpp \
    $(OSTREAMLOADSRC_PATH)/voMJPEG.cpp \
    $(OSTREAMLOADSRC_PATH)/voEVRC.cpp \
    $(OSTREAMLOADSRC_PATH)/voOGG.cpp \
    $(OSTREAMLOADSRC_PATH)/voMPEG4.cpp \
    $(OSTREAMLOADSRC_PATH)/voAC3.cpp \
    $(OSTREAMLOADSRC_PATH)/voMPEG2.cpp \
    $(OSTREAMLOADSRC_PATH)/voCaptionParser.cpp \
    $(OSTREAMLOADSRC_PATH)/voVC1.cpp \
    $(OSTREAMLOADSRC_PATH)/voH264.cpp \
    $(OSTREAMLOADSRC_PATH)/voH265.cpp \
    $(OSTREAMLOADSRC_PATH)/voAMRWB.cpp \
    $(OSTREAMLOADSRC_PATH)/voEFTDolby.cpp \
    $(OSTREAMLOADSRC_PATH)/voVP8.cpp \
    $(OSTREAMLOADSRC_PATH)/vompAPI.cpp \
    $(OSTREAMLOADSRC_PATH)/voAMRWBP.cpp \
    $(OSTREAMLOADSRC_PATH)/voFLAC.cpp \
    $(OSTREAMLOADSRC_PATH)/voAMRNB.cpp \
    $(OSTREAMLOADSRC_PATH)/voAPEDec.cpp \
    $(OSTREAMLOADSRC_PATH)/voQCELP.cpp \
    $(OSTREAMLOADSRC_PATH)/voVP6.cpp \
    $(OSTREAMLOADSRC_PATH)/voALAC.cpp \
    $(OSTREAMLOADSRC_PATH)/voADPCM.cpp \
    $(OSTREAMLOADSRC_PATH)/voMP3.cpp \
    $(OSTREAMLOADSRC_PATH)/voG711.cpp \
    $(OSTREAMLOADSRC_PATH)/voNPWrap.cpp \
    $(OSTREAMLOADSRC_PATH)/voAudioSpeed.cpp \
    \
    $(A_RENDER_PATH)/CBaseAudioRender.cpp \
    $(A_RENDER_PATH)/iOS/CAudioUnitRender.cpp \
    $(A_RENDER_PATH)/iOS/CAudioUnitRenderEx.mm \
    $(A_RENDER_PATH)/iOS/voAudioSession.mm \
    $(A_RENDER_PATH)/iOS/voAudioRenderFactory.mm \
    $(V_RENDER_PATH)/CBaseVideoRender.cpp \
    $(V_RENDER_PATH)/iOS/CCCRRRFunc.cpp \
    $(V_RENDER_PATH)/iOS/CiOSVideoRender.mm \
    $(V_RENDER_PATH)/iOS/voGLRenderFactory.mm \
    $(V_RENDER_PATH)/iOS/voGLRenderBase.mm \
    $(V_RENDER_PATH)/iOS/voGLRenderES1.mm \
    $(V_RENDER_PATH)/iOS/voGLRenderES2.mm \
    $(V_RENDER_PATH)/iOS/voGLRenderES2_FTU.mm \
    $(V_RENDER_PATH)/iOS/voVideoView.mm \
    \
    $(CMNSRC_PATH)/voLog.c \
    $(CMNSRC_PATH)/voOSFunc_iOS.mm \
    $(CMNSRC_PATH)/CDllLoad.cpp \
    $(CMNSRC_PATH)/voCMutex.cpp \
    $(CMNSRC_PATH)/voThread.cpp \
    $(CMNSRC_PATH)/cmnVOMemory.cpp \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/CBaseConfig.cpp \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/CVideoSubtitle.cpp \
    $(CMNSRC_PATH)/voSubtitleFunc.cpp \
    $(CMNSRC_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
    $(CMNSRC_PATH)/Buffer/voH264SEIDataBuffer.cpp \
    $(CMNSRC_PATH)/iOS/voRunRequestOnMain.mm \
    $(CMNSRC_PATH)/iOS/voSystemStatus.mm \
    $(CMNSRC_PATH)/iOS/voNSRecursiveLock.mm \
    $(CMNSRC_PATH)/voHalInfo.cpp \
    $(CMNSRC_PATH)/CModuleVersion.cpp


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	$(OSTREAMCMNSRC_PATH) \
    $(OSTREAMIOSSRC_PATH) \
    $(OSTREAMLOADSRC_PATH) \
    $(OSTREAMRENDER_PATH) \
    $(OSTREAM_CC_RENDER) \
		$(INC_PATH) \
		$(CMNSRC_PATH) \
		$(CMNSRC_PATH)/iOS \
		$(CMNSRC_PATH)/Buffer \
		$(A_RENDER_PATH) \
		$(A_RENDER_PATH)/iOS \
		$(V_RENDER_PATH) \
		$(V_RENDER_PATH)/iOS
