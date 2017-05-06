LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoRawDataFR

CMNSRC_PATH:=../../../../../../../Common
CSRCMN_PATH:=../../../../../../File/Common
CSRC_PATH:=../../../../../../File/RawDataParser

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
	$(CSRC_PATH)/PCMRawData.cpp \
	$(CSRC_PATH)/PCMRawDataInterface.cpp \
	$(CSRC_PATH)/RawDataInterface.cpp \
	$(CSRC_PATH)/voList.cpp
	

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH)




VOMM:= -D_RAW_DATA_PARSER -DVO_FILE_USE_BIG_ENDIAN -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_INFO -D_VOLOG_WARNING

# about info option, do not need to care it
LOCAL_CFLAGS := -DHAVE_NEON=1 -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

