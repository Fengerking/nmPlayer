LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := voDRMAdapter

CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../Source
CDRMMN_PATH:=../../../../../../Common

LOCAL_SRC_FILES := \
	$(CMNSRC_PATH)/voLog.c \
	$(CDRMMN_PATH)/DRM.cpp \
	$(CDRMMN_PATH)/voDRM.cpp \
	$(CSRC_PATH)/DRMAdapter.cpp
	

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../../Include \
	../../../../../../../../Include/OSMP_V3 \
	../../../../../../../Common \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CDRMMN_PATH)



VOMM:= -D_DRM_ADAPTER -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_INFO -D_VOLOG_WARNING
# -D_VOLOG_RUN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x0a300000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := -llog
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

