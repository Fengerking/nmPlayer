LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoLiveSrcHLS

CMNSRC_PATH:=../../../../../../../../Common

LOCAL_SRC_FILES := \
  $(CMNSRC_PATH)/voLog.c \
	\
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/NetWork/vo_socket.cpp \
	$(CMNSRC_PATH)/CBaseConfig.cpp \
  $(CMNSRC_PATH)/Buffer/voSourceBufferManager.cpp \
	$(CMNSRC_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(CMNSRC_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(CMNSRC_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
  ../../../../../../../../Source/Common/vo_thread.cpp \
  ../../../../../../../../Source/File/Common/CvoBaseDrmCallback.cpp \
	../../../../../../../../Source/File/Common/fortest.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/helix_date.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/md5.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/md5hl.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vostream_digest_auth.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/CCheckTsPacketSize.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_http_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_https_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_m3u_manager.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_m3u_reader.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_mem_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_http_live_streaming.cpp \
  ../../../../../../../../Source/HTTPLiveStreaming/vo_network_judgment.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/bitrate_adaptation_buffer.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/CLiveSrcHLS.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_http_live_streaming_new.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/CLiveSrc2HLS.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_webdownload_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/voLiveSource.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/CLiveSrcBase.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/ChannelInfo.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/CLiveParserBase.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/ConfigFile.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/voHLS2.cpp \

LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
        ../../../../../../../../Include/vome \
        ../../../../../../../../Common \
        ../../../../../../../../Common/NetWork \
        ../../../../../../../../Common/Buffer \
        ../../../../../../../../MFW/voME/Common \
        ../../../../../../../../Source/Common \
        ../../../../../../../../Source/File/Common \
        ../../../../../../../../Source/File/Common/Utility \
        ../../../../../../../../Source/File/Audio \
        ../../../../../../../../Source/HTTPLiveStreaming \
        ../../../../../../../../Source/MTV/LiveSource/Common \
        ../../../../../../../../Thirdparty/ndk \
        ../../../../../../../../Thirdparty/OpenSSL
        




VOMM:= -DLINUX -D_LINUX -DHAVE_PTHREADS -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_HLS_SOURCE_ -D__VO_NDK__ -D_SOURCE2

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x0a630000  -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp  -mfloat-abi=softfp -msoft-float
LOCAL_LDLIBS := ../../../../../../../../Lib/ndk/libvoCheck.a -llog -ldl -lstdc++ -lgcc\
		../shared_libs/libvocrypto.so \
		../shared_libs/libvossl.so

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)