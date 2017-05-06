LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := voDRM_Comcast_PlayReady

CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../Source
CDRMMN_PATH:=../../../../../../Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CDRMMN_PATH)/DRM.cpp \
	$(CDRMMN_PATH)/voDRM.cpp\
	$(CSRC_PATH)/DRM_Comcast_PlayReady.cpp


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Thirdparty/Irdeto/forComcast/inc \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CDRMMN_PATH)



VOMM:= -D_DRM_COMCAST -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_WARNING -D_VOLOG_RUN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x0a060000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := -llog ../../../../../../../../Thirdparty/Irdeto/forComcast/lib/x86/libacm_drm_stub.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

