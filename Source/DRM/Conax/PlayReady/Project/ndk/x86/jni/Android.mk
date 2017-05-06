LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := voDRMConaxPlayReady

CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../Source

LOCAL_SRC_FILES := \
	$(CSRC_PATH)/voDRMConaxPlayReady.cpp \
	$(CSRC_PATH)/DRM_Conax_PlayReady.cpp
	

LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Include/OSMP_V3 \
	../../../../../../../../Thirdparty/Conax/Inc \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)
	

VOMM:= -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_WARNING
# -D_VOLOG_RUN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x0a320000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := -llog
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

