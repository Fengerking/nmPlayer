LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoMP4FR

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=  ../../../../../../File/MP4
CSRCMN_PATH:=../../../../../../File/Common
CMDSRC_PATH:=../../../../../../File/MetaData
CSOURCE_PATH:=../../../../../../../Source

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CSRC_PATH)/ISOMedia/isomrd.cpp \
	$(CSRC_PATH)/ISOMedia/isomscan.cpp \
	$(CSRC_PATH)/MP4Base/mp4cfg.cpp \
	$(CSRC_PATH)/MP4Base/mp4desc.cpp \
	$(CSRC_PATH)/MP4Base/mpxio.cpp \
	$(CSRC_PATH)/MP4Base/mpxobj.cpp \
	$(CSRC_PATH)/MP4Base/mpxutil.cpp \
	$(CSRC_PATH)/Reader/CMp4FileStream.cpp \
	$(CSRC_PATH)/Reader/CMp4Reader2.cpp \
	$(CSRC_PATH)/Reader/CMp4Track2.cpp \
	$(CSRCMN_PATH)/CBaseReader.cpp \
	$(CSRCMN_PATH)/CBaseTrack.cpp \
	$(CSRCMN_PATH)/CGFileChunk.cpp \
	$(CSRCMN_PATH)/CMemPool.cpp \
	$(CSRCMN_PATH)/CvoBaseDrmCallback.cpp \
	$(CSRCMN_PATH)/CvoBaseFileOpr.cpp \
	$(CSRCMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRCMN_PATH)/CvoFileParser.cpp \
	$(CSRCMN_PATH)/fCodec.c \
	$(CSRCMN_PATH)/fVideoHeadDataInfo.cpp \
	$(CSRCMN_PATH)/voFileReader.cpp \
	$(CMDSRC_PATH)/CBaseMetaData.cpp \
	$(CMDSRC_PATH)/CID3MetaData.cpp \
	$(CMDSRC_PATH)/CMP4MetaData.cpp \
	$(CSOURCE_PATH)/MTV/LiveSource/ATSCMH/Common/base64.cpp \
	$(CSOURCE_PATH)/MTV/LiveSource/ATSCMH/Common/strutil.cpp \
	$(CSOURCE_PATH)/IISSmoothStreaming/fortest.cpp \
	$(CMNSRC_PATH)/NetWork/vo_socket.cpp \
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_http_stream.cpp \
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_mem_stream.cpp \
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_stream.cpp \
	$(CSOURCE_PATH)/IISSmoothStreaming/vo_thread.cpp \
	$(CSOURCE_PATH)/SMTHParser/voDrmCallback.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CSRC_PATH)/ISOMedia/fragmentTrack.cpp 

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRC_PATH)/ISOMedia \
	$(CSRC_PATH)/MP4Base \
	$(CSRC_PATH)/Reader \
	$(CSRCMN_PATH) \
	$(CMDSRC_PATH) \
	$(CSOURCE_PATH)/MTV/LiveSource/ATSCMH/Common \
	$(CSOURCE_PATH)/IISSmoothStreaming \
	$(CSOURCE_PATH)/SMTHParser \
	$(CMNSRC_PATH)/NetWork 
	


VOMM:= -D_MP4_READER -D_USE_J3LIB -D_SUPPORT_PARSER_METADATA -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_FUNC -D_SUPPORT_PIFF#-D_VOLOG_INFO -D_VOLOG_WARNING -D_VOLOG_ERROR

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01010000  -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=soft -fsigned-char
LOCAL_LDLIBS := -llog ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

