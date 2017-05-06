LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoASFFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/ASF
CSRCMN_PATH:=../../../../../../File/Common
CMDSRC_PATH:=../../../../../../File/MetaData
SRCMAINCMN_PATH:=../../../../../../Common
SRCINCLUDE_PATH:=../../../../../../Include

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	$(CSRC_PATH)/CAsfAudioTrack.cpp \
	$(CSRC_PATH)/AsfIndex.cpp \
	$(CSRC_PATH)/CAsfParser.cpp \
	$(CSRC_PATH)/CAsfReader.cpp \
	$(CSRC_PATH)/CAsfTrack.cpp \
	$(CSRC_PATH)/CAsfVideoTrack.cpp \
	$(CSRCMN_PATH)/CBaseReader.cpp \
	$(CSRCMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRCMN_PATH)/voSource2WrapperManager.cpp \
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
	$(CMDSRC_PATH)/CBaseMetaData.cpp \
	$(CMDSRC_PATH)/CASFMetaData.cpp \
	$(CSRCMN_PATH)/voSource2ParserWrapper.cpp \
	$(CSRCMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRCMN_PATH)/CSourceIOSwitch.cpp  \
	$(SRCMAINCMN_PATH)/Buffer/voSourceBufferManager.cpp  \
	$(SRCMAINCMN_PATH)/Buffer/voSourceDataBuffer.cpp  \
	$(SRCMAINCMN_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp  \
	$(SRCMAINCMN_PATH)/Buffer/voSourceVideoDataBuffer.cpp  \
	$(SRCMAINCMN_PATH)/vo_thread.cpp



LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CMDSRC_PATH) \
	$(SRCMAINCMN_PATH) \
	$(SRCMAINCMN_PATH)/Buffer \
	$(SRCINCLUDE_PATH) \



VOMM:= -D_ASF_READER -D_NOT_USE_ASFFILEGUID -D_SUPPORT_PARSER_METADATA -D__VO_NDK__ -DLINUX -D_LINUX -D_SOURCE2 -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO
# -D_VOLOG_RUN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01030000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_CPPFLAGS := -Wno-conversion-null 
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

