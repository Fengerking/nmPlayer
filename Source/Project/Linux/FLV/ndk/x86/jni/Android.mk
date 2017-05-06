LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoFLVFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/FLV
CSRCMN_PATH:=../../../../../../File/Common
SRCMAINCMN_PATH:=../../../../../../Common
SOURCEINC_PATH:=../../../../../../Include

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	$(CSRC_PATH)/CFlvAudioTrack.cpp \
	$(CSRC_PATH)/CFlvParser.cpp \
	$(CSRC_PATH)/CFlvReader.cpp \
	$(CSRC_PATH)/CFlvTrack.cpp \
	$(CSRC_PATH)/CFlvVideoTrack.cpp \
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
	$(CSRCMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRCMN_PATH)/voSource2WrapperManager.cpp \
	$(CSRCMN_PATH)/CSourceIOSwitch.cpp \
	$(SRCMAINCMN_PATH)/Buffer/voSourceBufferManager.cpp \
	$(SRCMAINCMN_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(SRCMAINCMN_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(SRCMAINCMN_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	$(SRCMAINCMN_PATH)/vo_thread.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CSRCMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRCMN_PATH)/voSource2ParserWrapper.cpp


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \
	$(SOURCEINC_PATH) \
	$(SRCMAINCMN_PATH) \
	$(SRCMAINCMN_PATH)/Buffer



VOMM:= -DFLV_READER -DVO_FILE_USE_BIG_ENDIAN -D_SOURCE2 -DLINUX -D_LINUX -D__VO_NDK__ -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_WARNING
# -D_VOLOG_RUN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01060000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_CPPFLAGS := -Wno-conversion-null
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

