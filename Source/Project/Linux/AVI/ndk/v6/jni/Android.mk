LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoAVIFR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/AVI
CSRCMN_PATH:=../../../../../../File/Common
CSRSCMN_PATH:=../../../../../../Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CSRC_PATH)/CAviAudioTrack.cpp \
	$(CSRC_PATH)/CAviIndex.cpp \
	$(CSRC_PATH)/CAviParser.cpp \
	$(CSRC_PATH)/CAviReader.cpp \
	$(CSRC_PATH)/CAviTrack.cpp \
	$(CSRC_PATH)/CAviVideoTrack.cpp \
	$(CSRC_PATH)/CAviMpeg2VideoTrack.cpp \
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
	$(CSRCMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRCMN_PATH)/voSource2ParserWrapper.cpp \
	$(CSRCMN_PATH)/CSourceIOSwitch.cpp \
	$(CSRCMN_PATH)/voSource2WrapperPushPlay.cpp \
	$(CSRCMN_PATH)/voSource2WrapperManager.cpp \
	$(CSRSCMN_PATH)/Buffer/voSourceBufferManager.cpp \
	$(CSRSCMN_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(CSRSCMN_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(CSRSCMN_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	$(CSRSCMN_PATH)/vo_thread.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp 
	
	
	
	

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../File/OGG \
	../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CSRSCMN_PATH) \
	$(CSRSCMN_PATH)/Buffer



VOMM:= -D_AVI_READER -D__arm -D_SOURCE2 -DLINUX -D_LINUX -D__VO_NDK__ -D_VO_FORCE_PASS_DIVX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_WARNING
# -D_VOLOG_RUN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01020000  -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=soft -fsigned-char
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog  

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

