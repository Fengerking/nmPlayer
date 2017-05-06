LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoMKVFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/MKV
CSRCMN_PATH:=../../../../../../File/Common
CSRCOURCE_COM:=../../../../../../Common
CSRCOURCE_INCLUDE:=../../../../../../Include

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	\
	$(CSRCOURCE_COM)/Buffer/voSourceBufferManager.cpp \
	$(CSRCOURCE_COM)/Buffer/voSourceDataBuffer.cpp \
	$(CSRCOURCE_COM)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(CSRCOURCE_COM)/Buffer/voSourceVideoDataBuffer.cpp \
	\
	$(CSRC_PATH)/CMKVAnalyze.cpp \
	$(CSRC_PATH)/EBML.cpp \
	$(CSRC_PATH)/CMKVAudioTrack.cpp \
	$(CSRC_PATH)/CMKVAudioTrack_RealSpecialize.cpp \
	$(CSRC_PATH)/CMKVFileReader.cpp \
	$(CSRC_PATH)/CMKVParser.cpp \
	$(CSRC_PATH)/CMKVStreamFileIndex.cpp \
	$(CSRC_PATH)/CMKVTrack.cpp \
	$(CSRC_PATH)/CMKVVideoTrack.cpp \
	$(CSRC_PATH)/CMKVVideoTrack_RealSpecialize.cpp \
	\
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
	$(CSRCMN_PATH)/fCodec.c \
	$(CSRCMN_PATH)/voFileReader.cpp \
	$(CSRCMN_PATH)/voSource2ParserWrapper.cpp \
	$(CSRCMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRCMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRCMN_PATH)/voSource2WrapperManager.cpp \
	$(CSRCMN_PATH)/CSourceIOSwitch.cpp \
	\
	$(CSRCOURCE_COM)/vo_thread.cpp \

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CSRCOURCE_COM)/Buffer \
	$(CSRCOURCE_COM) \
	$(CSRCOURCE_INCLUDE) \



VOMM:= -D_MKV_READER -DVO_FILE_USE_BIG_ENDIAN -D_VOLOG_FUNC -D_SOURCE2 -DLINUX -D_LINUX -D__VO_NDK__ -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOSYMOBF

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01080000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_CPPFLAGS := -Wno-conversion-null -Wno-write-strings
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/x86/libvoCheck.a -llog ../../../../../../../Lib/ndk/x86/libvoVideoParser.a -L../../../../../../../Lib/ndk/x86/ -lvodl 

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

