LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyDataIOLib

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
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voLog.c \
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
	

VOMM:= -DLINUX -D_LINUX -DHAVE_PTHREADS -D_LINUX_ANDROID -D_HLS_SOURCE_ -D__VO_NDK__ -D_VONDBG

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x00127000 -D_VONAMESPACE=yyDataIOLib -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp -msoft-float
LOCAL_LDLIBS := -llog -lstdc++ -lgcc
#../../../../../../../Thirdparty/OpenSSL/static_libs/libvossl.a ../../../../../../../Thirdparty/OpenSSL/static_libs/libvocrypto.a
		

include $(VOTOP)/build/vondk.mk
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyDataIO


LOCAL_STATIC_LIBRARIES := libyyDataIOLib


include $(BUILD_SHARED_LIBRARY)


