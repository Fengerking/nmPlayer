LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := voDRM_Showtime_PRWM

CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../Source
CSRCHTTP_PATH:=../../../../../../../File/WindowsMediaHTTPStreaming
CDRMMN_PATH:=../../../../../../Common
CSRCMN_PATH:=../../../../../../../Common
CSRCLOCALMN_PATH:=../../../../../../../File/Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CDRMMN_PATH)/DRM.cpp \
	$(CDRMMN_PATH)/voDRM.cpp \
	$(CDRMMN_PATH)/DRMStreaming.cpp \
	$(CSRCLOCALMN_PATH)/base64.cpp \
	$(CSRCLOCALMN_PATH)/strutil.cpp \
	$(CSRC_PATH)/DRM_Showtime_PRWM.cpp \
	$(CSRCHTTP_PATH)/HTTP_Transaction.cpp \
	$(CSRCHTTP_PATH)/HTTPs_Transaction.cpp \
	$(CSRCHTTP_PATH)/TCPClientSocket.cpp \
	$(CSRCHTTP_PATH)/TCPClientSocket_ssl.cpp \
	$(CSRCHTTP_PATH)/Thread.cpp \
	$(CSRCMN_PATH)/CSourceIOUtility.cpp


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../../Include \
	../../../../../../../../Thirdparty/OpenSSL \
	../../../../../../../../Thirdparty/AuthenTec/forShowtime/inc \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCHTTP_PATH) \
	$(CDRMMN_PATH) \
	$(CSRCLOCALMN_PATH) \
	$(CSRCMN_PATH)



VOMM:= -D_DRM_SHOWTIME -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_WARNING
#

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x0a020000  -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=soft -fsigned-char
LOCAL_LDFLAGS:= -Wl,-Map,voDRM_Showtime_PRWM.map
LOCAL_LDLIBS := -llog ../../../../../../../../Thirdparty/AuthenTec/forShowtime/lib/libdrmagent.a ../../../../../../../Project/Linux/SourceIO/ndk/v6/shared_libs/libvocrypto.so ../../../../../../../Project/Linux/SourceIO/ndk/v6/shared_libs/libvossl.so

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

