LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoRawDataFR

CMNSRC_PATH:=../../../../../../../Common
CSRCMN_PATH:=../../../../../../File/Common
CSRC_PATH:=../../../../../../File/RawDataParser
CSRSCMN_PATH:=../../../../../../Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CFileFormatCheck.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/voLog.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CSRCMN_PATH)/CBaseReader.cpp \
	$(CSRCMN_PATH)/CGFileChunk.cpp \
	$(CSRCMN_PATH)/CMemPool.cpp \
	$(CSRCMN_PATH)/CvoBaseDrmCallback.cpp \
	$(CSRCMN_PATH)/CvoBaseFileOpr.cpp \
	$(CSRCMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRCMN_PATH)/CvoFileParser.cpp \
	$(CSRCMN_PATH)/fCodec.c \
	$(CSRC_PATH)/H264RawData.cpp \
	$(CSRC_PATH)/H264RawDataInterface.cpp \
	$(CSRC_PATH)/H265RawData.cpp \
	$(CSRC_PATH)/H265RawDataInterface.cpp \
	$(CSRC_PATH)/PCMRawData.cpp \
	$(CSRC_PATH)/PCMRawDataInterface.cpp \
	$(CSRC_PATH)/RawDataInterface.cpp \
	$(CSRC_PATH)/CH264Track.cpp \
	$(CSRC_PATH)/CH265Track.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CSRCMN_PATH)/CBaseTrack.cpp \
	$(CSRC_PATH)/voList.cpp\
	$(CSRCMN_PATH)/voFileReader.cpp \
	$(CSRCMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRCMN_PATH)/voSource2ParserWrapper.cpp \
	$(CSRCMN_PATH)/CSourceIOSwitch.cpp \
	$(CSRCMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRCMN_PATH)/voSource2WrapperManager.cpp \
	$(CSRSCMN_PATH)/Buffer/voSourceBufferManager.cpp \
	$(CSRSCMN_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(CSRSCMN_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	$(CSRSCMN_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(CSRSCMN_PATH)/vo_thread.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp
	
	

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH)\
	$(CSRSCMN_PATH)\
	$(CSRSCMN_PATH)/Buffer



VOMM:= -D_RAW_DATA_PARSER -DVO_FILE_USE_BIG_ENDIAN -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_INFO -D_VOLOG_WARNING -D_SOURCE2

# about info option, do not need to care it
LOCAL_CFLAGS := -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp 
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

