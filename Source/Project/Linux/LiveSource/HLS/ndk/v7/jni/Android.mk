LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoLiveSrcHLS

CMNSRC_PATH:=../../../../../../../../Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/NetWork/vo_socket.cpp \
	$(CMNSRC_PATH)/CBaseConfig.cpp \
	../../../../../../../../Source/File/Common/CvoBaseDrmCallback.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/CCheckTsPacketSize.cpp \
	../../../../../../../../Source/File/Common/fortest.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_http_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_https_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_m3u_manager.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_m3u_reader.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_mem_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_aes_engine.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_drm_mem_stream.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_http_live_streaming.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_network_judgment.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_thread.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/bitrate_adaptation_buffer.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/CLiveSrcHLS.cpp \
	../../../../../../../../Source/HTTPLiveStreaming/vo_webdownload_stream.cpp \
	../../../../../../../MTV/LiveSource/Common/voLiveSource.cpp \
	../../../../../../../MTV/LiveSource/Common/CLiveSrcBase.cpp \
	../../../../../../../MTV/LiveSource/Common/ChannelInfo.cpp \
	../../../../../../../MTV/LiveSource/Common/CLiveParserBase.cpp \
	../../../../../../../MTV/LiveSource/Common/ConfigFile.cpp \

LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
        ../../../../../../../../Include/vome \
        ../../../../../../../../Common \
        ../../../../../../../../Common/NetWork \
        ../../../../../../../../MFW/voME/Common \
        ../../../../../../../../Source/File/Common \
        ../../../../../../../../Source/HTTPLiveStreaming \
        ../../../../../../../../Source/MTV/LiveSource/Common \
        ../../../../../../../../Thirdparty/ndk \
        /opt/froyo/external/openssl/include




VOMM:= -DLINUX -D_LINUX -DHAVE_PTHREADS -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOLOG_FUNC -D_HLS_SOURCE_ -D__VO_NDK__

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x0a630000  -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp  -mfloat-abi=softfp
LOCAL_LDLIBS := ../../../../../../../../Lib/ndk/libvoCheck.a -llog -ldl -lstdc++ -lgcc\
		../shared_libs/libvocrypto.so \
		../shared_libs/libvossl.so

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

