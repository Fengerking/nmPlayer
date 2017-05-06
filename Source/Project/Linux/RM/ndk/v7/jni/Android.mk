LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoRealFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/RM
CSRCMN_PATH:=../../../../../../File/Common
CMDSRC_PATH:=../../../../../../File/MetaData

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnVOMemory.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CSRC_PATH)/CRealAudioTrack.cpp \
	$(CSRC_PATH)/CRealParser.cpp \
	$(CSRC_PATH)/CRealReader.cpp \
	$(CSRC_PATH)/CRealTrack.cpp \
	$(CSRC_PATH)/CRealVideoTrack.cpp \
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
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CSRCMN_PATH)/voFileReader.cpp \
	$(CMDSRC_PATH)/CBaseMetaData.cpp \
	$(CMDSRC_PATH)/CRealMetaData.cpp \
	$(CSRCMN_PATH)/voSource2ParserWrapper.cpp	\
	$(CMNSRC_PATH)/voLog.c


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../File/ID3 \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CMDSRC_PATH)



VOMM:= -D_RM_READER -DVO_FILE_USE_BIG_ENDIAN -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_SOURCE2 -D_VOLOG_FUNC -D_VOLOG_INFO -D_VOLOG_WARNING -D_VONAMESPACE=libvoRealFR

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x00040000  $(VOHIDESYM) -DHAVE_NEON=1 -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

