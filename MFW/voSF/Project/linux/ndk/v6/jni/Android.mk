LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvosfEngn

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../Source
MFW_CMN_PATH:=../../../../../../Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/CFileFormatCheck.cpp \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnVOMemory.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/CBaseConfig.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voLog.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(MFW_CMN_PATH)/CBaseNode.cpp \
	$(MFW_CMN_PATH)/CBaseSource.cpp \
	$(MFW_CMN_PATH)/CBaseLiveSource.cpp \
	$(MFW_CMN_PATH)/CFileSource.cpp \
	$(MFW_CMN_PATH)/CRTSPSource.cpp \
	$(MFW_CMN_PATH)/CHTTPPDSource.cpp \
	$(MFW_CMN_PATH)/CAudioDecoder.cpp \
	$(MFW_CMN_PATH)/CVideoDecoder.cpp \
	$(MFW_CMN_PATH)/CAudioEncoder.cpp \
	$(MFW_CMN_PATH)/CVideoEncoder.cpp \
	$(MFW_CMN_PATH)/CFileSink.cpp \
	$(CSRC_PATH)/CBaseMediaExtractor.cpp \
	$(CSRC_PATH)/CBaseMediaSink.cpp \
	$(CSRC_PATH)/CBaseMediaSource.cpp \
	$(CSRC_PATH)/CDataBuffer.cpp \
	$(CSRC_PATH)/CDataSourceOP.cpp \
	$(CSRC_PATH)/CFileMediaExtractor.cpp \
	$(CSRC_PATH)/CFileMediaSink.cpp \
	$(CSRC_PATH)/CFileMediaSource.cpp \
	$(CSRC_PATH)/CLiveSource.cpp \
	$(CSRC_PATH)/CMediaDecoderSource.cpp \
	$(CSRC_PATH)/CMediaEncoderSource.cpp \
        $(CSRC_PATH)/CMediaBufferList.cpp \
 	$(CSRC_PATH)/ParseSPS.cpp \
	$(CSRC_PATH)/cConstructCodecInfo.cpp \
	$(CSRC_PATH)/voSF.cpp	

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../Include/vome \
	../../../../../../../Thirdparty/ndk \
	$(CMNSRC_PATH) \
	$(MFW_CMN_PATH)\
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID  -D_GINGERBREAD  -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO

LOCAL_CFLAGS := -Wno-multichar -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS :=	-llog -ldl -lstdc++ \
              ../../../../../../../Thirdparty/ndk/libcpufeatures.a \

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

