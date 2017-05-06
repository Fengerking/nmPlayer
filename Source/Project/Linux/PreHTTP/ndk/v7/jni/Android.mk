LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoPreHTTP

CMNSRC_PATH:=../../../../../../../Common
CSRCNW_PATH:=../../../../../../../Common/NetWork
CSRCMN_PATH:=../../../../../../Common
CSRC_PATH:=../../../../../../File/PreHTTP
CSRCPD_PATH:=../../../../../../File/PD2_Cookies

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CBaseConfig.cpp \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CSRCNW_PATH)/vo_socket.cpp \
	$(CSRC_PATH)/ASXParser.cpp \
	$(CSRC_PATH)/PreHTTP.cpp \
	$(CSRC_PATH)/voPreHTTP.cpp \
	$(CSRCMN_PATH)/vo_thread.cpp \
	$(CSRCPD_PATH)/vo_http_downloader.cpp \
	$(CSRCPD_PATH)/vo_http_authentication.cpp \
	$(CSRCPD_PATH)/vo_http_md5.cpp \
	$(CSRCPD_PATH)/log.cpp


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../Thirdparty/OpenSSL \
	$(CMNSRC_PATH) \
	$(CSRCNW_PATH) \
	$(CSRC_PATH) \
	$(CSRCPD_PATH) \
	$(CSRCMN_PATH)



VOMM:= -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_WARNING

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x00126000  -DHAVE_NEON=1 -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char
LOCAL_LDLIBS := -llog ../../../../../../Project/Linux/SourceIO/ndk/v6/shared_libs/libvocrypto.so ../../../../../../Project/Linux/SourceIO/ndk/v6/shared_libs/libvossl.so -L../../../../../../../Lib/ndk -lvodl

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

