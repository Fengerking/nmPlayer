LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoSourceIO

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../IO
CSR_CMN_PATH:=../../../../../../Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/NetWork/vo_socket.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CSR_CMN_PATH)/vo_thread.cpp \
	$(CSRC_PATH)/voSourceIO.cpp \
	$(CSRC_PATH)/CSourceIOLocalFile.cpp \
	$(CSRC_PATH)/DRMLocal.cpp \
	$(CSRC_PATH)/vo_http_session.cpp \
	$(CSRC_PATH)/vo_http_stream.cpp \
	$(CSRC_PATH)/vo_mem_stream.cpp \
	$(CSRC_PATH)/vo_http_cookie.cpp \
	$(CSRC_PATH)/vo_http_sessions_info.cpp	\
	$(CSRC_PATH)/vo_http_authentication.cpp \
	$(CSRC_PATH)/vo_http_utils.cpp \
	$(CSRC_PATH)/vo_http_md5.cpp \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CSRC_PATH)/COpenSSL.cpp \


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSR_CMN_PATH) \
	../../../../../../../Common/NetWork \
	../../../../../../../Common/NetWork/ares
	



VOMM:= -DLINUX -D_LINUX -DHAVE_PTHREADS -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOLOG_RUN -D_HLS_SOURCE_ -D__VO_NDK__ -DENABLE_ASYNCDNS -DCARES_STATICLIB

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x00127000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := -llog -ldl -lstdc++ -lgcc  ../../../../../../../Lib/ndk/x86/libvodl.so ../../../../../../../Lib/ndk/x86/libcares.a
#../../../../../../../Thirdparty/OpenSSL/static_libs/libvossl.a ../../../../../../../Thirdparty/OpenSSL/static_libs/libvocrypto.a
		

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

