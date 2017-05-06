

OSTREAMCMNSRC_PATH  :=../../../Source/Common
OSTREAMIOSSRC_PATH  :=../../../Source/iOS
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
    $(OSTREAMCMNSRC_PATH)/CSEIInfo.cpp \
    \
    $(OSTREAMIOSSRC_PATH)/voOSPlayer.mm \
    $(OSTREAMIOSSRC_PATH)/voOnStreamEngine.cpp \
    $(OSTREAMIOSSRC_PATH)/voOnStreamEngine_ns.mm \
    $(OSTREAMIOSSRC_PATH)/voOSPlayerData.mm \
    $(OSTREAMIOSSRC_PATH)/voOSPlayerAdapter.mm \
    \
    $(OSTREAMRENDER_PATH)/voBaseRender.cpp \
    $(OSTREAMRENDER_PATH)/CAudioRenderManager.cpp \
    $(OSTREAMRENDER_PATH)/CVideoRenderManager.cpp \
    \
    $(OSTREAM_CC_RENDER)/voCCManager.mm \
    $(OSTREAM_CC_RENDER)/voSubTitleRender.mm \
    $(OSTREAM_CC_RENDER)/voTextInfoEntryView.mm \
    $(OSTREAM_CC_RENDER)/voRenderObjectInfo.mm \
    $(OSTREAM_CC_RENDER)/voSubtitleProperty.mm \
    \
    $(A_RENDER_PATH)/CBaseAudioRender.cpp \
    $(A_RENDER_PATH)/MacOS/CWaveOutAudio.cpp \
    $(A_RENDER_PATH)/MacOS/CAudioQueueRender.cpp \
    $(V_RENDER_PATH)/CBaseVideoRender.cpp \
    $(V_RENDER_PATH)/CCCRRRFunc.cpp \
    $(V_RENDER_PATH)/MacOS/CMacOSVideoRender.mm \
    $(V_RENDER_PATH)/MacOS/voGLRenderBase.mm \
    \
    $(CMNSRC_PATH)/iOS/voSystemStatus.mm \
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
    $(CMNSRC_PATH)/voHalInfo.cpp \
    $(CMNSRC_PATH)/CModuleVersion.cpp \
    $(CMNSRC_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
    $(CMNSRC_PATH)/Buffer/voH264SEIDataBuffer.cpp \


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	$(OSTREAMCMNSRC_PATH) \
    $(OSTREAMIOSSRC_PATH) \
    $(OSTREAMLOADSRC_PATH) \
    $(OSTREAMRENDER_PATH) \
    $(OSTREAM_CC_RENDER) \
    $(INC_PATH) \
    $(CMNSRC_PATH)/iOS/ \
    $(CMNSRC_PATH) \
    $(CMNSRC_PATH)/Buffer \
    $(A_RENDER_PATH) \
    $(A_RENDER_PATH)/MacOS \
    $(V_RENDER_PATH) \
    $(V_RENDER_PATH)/MacOS
