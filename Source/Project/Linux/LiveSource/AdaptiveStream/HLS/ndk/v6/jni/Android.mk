LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyHLSParserLib

CMNSRC_PATH:=../../../../../../../../../Common

LOCAL_SRC_FILES := \
  $(CMNSRC_PATH)/CvoBaseObject.cpp \
  $(CMNSRC_PATH)/voLog.c \
	\
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	../../../../../../../../../Source/Common/vo_thread.cpp \
	../../../../../../../../../Source/Common/CDataBox.cpp \
	../../../../../../../../../Source/AdaptiveStreaming/HLS/hls_parser.cpp \
	../../../../../../../../../Source/AdaptiveStreaming/HLS/hls_manager.cpp \
	../../../../../../../../../Source/AdaptiveStreaming/HLS/hls_entity.cpp \
  ../../../../../../../../../Source/AdaptiveStreaming/HLS/voAdaptiveStreamHLS.cpp
LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
        ../../../../../../../../../Include/vome \
        ../../../../../../../../../Common \
        ../../../../../../../../../Thirdparty/ndk \
        ../../../../../../../../../Include/vome \
        ../../../../../../../../../Common \
        ../../../../../../../../../Common/NetWork \
        ../../../../../../../../../Common/Buffer \
        ../../../../../../../../../MFW/voME/Common \
        ../../../../../../../../../Source/Common \
        ../../../../../../../../../Source/Include \
        ../../../../../../../../../Source/AdaptiveStreaming/Include \
        ../../../../../../../../../Source/AdaptiveStreaming/Common \
        ../../../../../../../../../Source/AdaptiveStreaming/DRM \
        ../../../../../../../../../Source/File/Common \
        ../../../../../../../../../Source/File/Common/Utility \


VOMM:= -DLINUX -D_LINUX -DHAVE_PTHREADS -D_LINUX_ANDROID -D__VO_NDK__ -D_new_programinfo -D_VONDBG

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01121000 -D_WITHOUT_LIC_CHECK_ -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp  -mfloat-abi=softfp -msoft-float
LOCAL_LDLIBS := -llog -ldl -lstdc++ -lgcc

include $(VOTOP)/build/vondk.mk
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyHLSParser


LOCAL_STATIC_LIBRARIES := libyyHLSParserLib


include $(BUILD_SHARED_LIBRARY)
