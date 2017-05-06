LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvompEngn


CMNSRC_PATH:=../../../../../../../Common
MFWSRC_PATH:=../../../../../../Common
VOME2SRC_PATH:=../../../../../Source

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CBaseConfig.cpp \
	$(CMNSRC_PATH)/CAndroidUtilities.cpp \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/CFileFormatCheck.cpp \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnVOMemory.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCBaseThread.cpp	\
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	../../../../../../../Render/Audio/Source/CBaseAudioRender.cpp \
	../../../../../../../Render/Video/Render/Source/CBaseVideoRender.cpp \
	../../../../../../../Render/Video/Render/Source/CCCRRRFunc.cpp \
	$(MFWSRC_PATH)/CBaseNode.cpp \
	$(MFWSRC_PATH)/CAudioDecoder.cpp \
	$(MFWSRC_PATH)/CAudioEffect.cpp \
	$(MFWSRC_PATH)/CAudioSpeed.cpp \
	$(MFWSRC_PATH)/CVideoEffect.cpp \
	$(MFWSRC_PATH)/CBaseSource.cpp \
	$(MFWSRC_PATH)/CFileSource.cpp \
	$(MFWSRC_PATH)/CRTSPSource.cpp \
	$(MFWSRC_PATH)/CHTTPPDSource.cpp \
	$(MFWSRC_PATH)/CBaseLiveSource.cpp \
	$(MFWSRC_PATH)/CFileSink.cpp \
	$(MFWSRC_PATH)/CVideoDecoder.cpp \
	$(VOME2SRC_PATH)/voCBasePlayer.cpp \
	$(VOME2SRC_PATH)/voCDataBuffer.cpp \
	$(VOME2SRC_PATH)/voCDataSource.cpp \
	$(VOME2SRC_PATH)/voCDataSourceOP.cpp \
	$(VOME2SRC_PATH)/voCLiveDataSource.cpp \
	$(VOME2SRC_PATH)/voCAudioRenderBufferMgr.cpp \
	$(VOME2SRC_PATH)/voCMediaPlayer.cpp \
	$(VOME2SRC_PATH)/voCMediaPlayerInit.cpp \
	$(VOME2SRC_PATH)/voCMediaPlayerMeta.cpp \
	$(VOME2SRC_PATH)/voCEditPlayer.cpp \
	$(VOME2SRC_PATH)/voCPlayerLogo.cpp \
	$(VOME2SRC_PATH)/voMP.cpp


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(MFWSRC_PATH) \
	$(VOME2SRC_PATH) \
	../../../../../../../Render/Audio/Source \
	../../../../../../../Render/Video/Render/Source
	


VOMM:= -DPD -DMS_HTTP -DLINUX -D_LINUX -D_LINUX_ANDROID -D__VO_NDK__ -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VONAMESPACE=libvompEngn

LOCAL_CFLAGS := -D_VOMODULEID=0x08040000  $(VOMM) -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -Wno-multichar

#LOCAL_LDLIBS := 
LOCAL_LDLIBS  := -llog -ldl -lstdc++ -L../../../../../../../Lib/ndk/x86/ -lvodl\
				../../../../../../../Lib/ndk/x86/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

