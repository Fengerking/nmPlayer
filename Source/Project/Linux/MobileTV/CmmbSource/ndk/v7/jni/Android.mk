LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoCmmbSrc

CMNSRC_PATH:=../../../../../../../../Common
CMNHEAD_PATH:=../../../../../../../../Include
LIVESRC_PATH:=../../../../../../../MTV/LiveSource/Common
CMMBSRC_PATH:=../../../../../../../MobileTV/CmmbSource

LOCAL_SRC_FILES := \
	  \
		$(CMNSRC_PATH)/CDllLoad.cpp \
		$(LIVESRC_PATH)/CLiveParserBase.cpp \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/cmnMemory.c \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/voCMutex.cpp \
		$(CMNSRC_PATH)/voCSemaphore.cpp \
		$(CMNSRC_PATH)/voHalInfo.cpp \
		$(CMNSRC_PATH)/voLog.cpp \
		$(CMNSRC_PATH)/voThread.cpp \
		$(CMNSRC_PATH)/voOSFunc.cpp \
		$(CMMBSRC_PATH)/BufList.cpp \
		$(CMMBSRC_PATH)/CmmbSource.cpp \
		$(CMMBSRC_PATH)/voCmmbSrc.cpp
	
LOCAL_C_INCLUDES := \
	$(CMNHEAD_PATH) \
	$(LIVESRC_PATH) \
	$(CMNSRC_PATH) \
	$(CMMBSRC_PATH)



VOMM:=-D__VO_NDK__ -D_LINUX_ANDROID -DLINUX -D_LINUX -D__arm -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO

LOCAL_CFLAGS := -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_CPPFLAGS := $(VOABOUT) $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS := -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
