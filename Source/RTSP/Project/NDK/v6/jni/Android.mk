LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoSrcRTSP

CMNSRC_PATH:=../../../../../../Common
BASE_PATH:=../../../../base
RTSP_PATH:=../../../../rtsp
THIPA_CMN_PATH:=../../../../third_party/real_streaming/src/common
THIPA_RDT_PATH:=../../../../third_party/real_streaming/src/rdt
THIPA_RV_PATH:=../../../../third_party/real_streaming/src/rv_depack
THIPA_INCLUDE_PATH:=../../../../third_party/real_streaming/include
WIN_ROOT_PATH:=../../../../voapi4win
WIN_WIN_PATH:=../../../../voapi4win/voapi4win

LOCAL_SRC_FILES := \
	\
	$(BASE_PATH)/getmp4vol.cpp \
	$(BASE_PATH)/BitVector.cpp \
	$(BASE_PATH)/HashTable.cpp \
	$(BASE_PATH)/network.cpp \
	$(BASE_PATH)/ParseSPS.cpp \
	$(BASE_PATH)/utility.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/fAudioHeadDataInfo.cpp \
	$(RTSP_PATH)/VOUtility.cpp \
	$(RTSP_PATH)/voRTSPSource.cpp \
	$(RTSP_PATH)/voRTSPSDK2.cpp \
	$(RTSP_PATH)/TaskSchedulerEngine.cpp \
	$(RTSP_PATH)/RTSPTrack.cpp \
	$(RTSP_PATH)/RTSPSource.cpp \
	$(RTSP_PATH)/RTSPSession.cpp \
	$(RTSP_PATH)/RTSPMediaStreamSyncEngine.cpp \
	$(RTSP_PATH)/RTSPClientEngine.cpp \
	$(RTSP_PATH)/RTPSenderTable.cpp \
	$(RTSP_PATH)/RTPParser.cpp \
	$(RTSP_PATH)/RTPPacketReorder.cpp \
	$(RTSP_PATH)/RTPPacket.cpp \
	$(RTSP_PATH)/RTCPReportPacket.cpp \
	$(RTSP_PATH)/RTCPParser.cpp \
	$(RTSP_PATH)/RealTimeStreamingEngine.cpp \
	$(RTSP_PATH)/RealRTSPClientEngine.cpp \
	$(RTSP_PATH)/RDTParser.cpp \
	$(RTSP_PATH)/RDTMediaStream.cpp \
	$(RTSP_PATH)/QcelpAudioRTPParser.cpp \
	$(RTSP_PATH)/MPEG4LATMAudioRTPParser.cpp \
	$(RTSP_PATH)/MPEG4GenericAudioRTPParser.cpp \
	$(RTSP_PATH)/MPEG4ESVideoRTPParser.cpp \
	$(RTSP_PATH)/MP3AudioRTPParser.cpp \
	$(RTSP_PATH)/MediaStreamSocket.cpp \
	$(RTSP_PATH)/mediastream.cpp \
	$(RTSP_PATH)/MediaFrameSink.cpp \
	$(RTSP_PATH)/H264VideoRTPParser.cpp \
	$(RTSP_PATH)/H263VideoRTPParser.cpp \
	$(RTSP_PATH)/DIMSRTPParser.cpp \
	$(RTSP_PATH)/ASFRTSPClientEngine.cpp \
	$(RTSP_PATH)/ASFRTPParser.cpp \
	$(RTSP_PATH)/ASFHeadParser.cpp \
	$(RTSP_PATH)/AMRWBPAudioRTPParser.cpp \
	$(RTSP_PATH)/AMRAudioRTPParser.cpp \
	$(RTSP_PATH)/G711AudioRTPParser.cpp \
	$(RTSP_PATH)/vostream_digest_auth.cpp \
	$(RTSP_PATH)/voSource2ParserWrapper.cpp \
	$(THIPA_CMN_PATH)/helix_date.cpp \
	$(THIPA_CMN_PATH)/memory_utils.cpp \
	$(THIPA_CMN_PATH)/pack_utils.cpp \
	$(THIPA_CMN_PATH)/rm_error_default.cpp \
	$(THIPA_CMN_PATH)/rm_io_default.cpp \
	$(THIPA_CMN_PATH)/rm_memory_default.cpp \
	$(THIPA_CMN_PATH)/rm_packet.cpp \
	$(THIPA_CMN_PATH)/rm_property.cpp \
	$(THIPA_CMN_PATH)/rm_stream.cpp \
	$(THIPA_CMN_PATH)/stream_hdr_utils.cpp \
	$(THIPA_CMN_PATH)/string_utils.cpp \
	$(THIPA_INCLUDE_PATH)/challenge.cpp \
	$(THIPA_INCLUDE_PATH)/md5.cpp \
	$(THIPA_INCLUDE_PATH)/md5hl.cpp \
	$(THIPA_INCLUDE_PATH)/rdtpck.cpp \
	$(THIPA_INCLUDE_PATH)/tngpktrtsp.cpp \
	$(THIPA_RV_PATH)/ra_depack.cpp \
	$(THIPA_RV_PATH)/ra_depack_internal.cpp \
	$(THIPA_RV_PATH)/rasl.cpp \
	$(THIPA_RV_PATH)/rv_depack.cpp \
	$(THIPA_RV_PATH)/rv_depack_internal.cpp \
	$(WIN_ROOT_PATH)/VOCrossPlatformWrapper_Linux.cpp \
	$(WIN_ROOT_PATH)/voLog_android.c \
	$(WIN_WIN_PATH)/Lin2WinEvent.cpp \
	$(WIN_WIN_PATH)/Lin2WinFile.cpp \
	$(WIN_WIN_PATH)/Lin2WinPrivate.cpp \
	$(WIN_WIN_PATH)/Lin2WinThread.cpp \
	$(WIN_WIN_PATH)/vocrsapis.cpp \
	$(WIN_WIN_PATH)/vosyncmutex.cpp

LOCAL_C_INCLUDES := \
	../../../../../../Include \
	../../../../base \
	../../../../rtsp \
	../../../../engine_sdk \
	../../../../../../Common \
	../../../../third_party \
	../../../../third_party/real_streaming/include \
	../../../../third_party/real_streaming/src \
	../../../../third_party/real_streaming/src/common \
	../../../../third_party/real_streaming/src/common/pub \
	../../../../third_party/real_streaming/src/rdt \
	../../../../third_party/real_streaming/src/rv_depack \
	../../../../third_party/real_streaming/voSDKInc \
	../../../../voapi4win/voapi4win \
	../../../../voapi4win 	



VOMM:=-D__VO_NDK__ -DHAVE_NEON=1 -D_LINUX_ANDROID -DLINUX  -DMAC -DG1 -DH264_ANNEXB_FORMAT -DFORCE_ADD_ADTS -D_LINUX #-D_VOLOG_INFO 

LOCAL_CFLAGS := -D_VOMODULEID=0x01110000  -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp
LOCAL_LDLIBS :=../../../../../../Lib/ndk/libvoCheck.a -llog -ldl

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

