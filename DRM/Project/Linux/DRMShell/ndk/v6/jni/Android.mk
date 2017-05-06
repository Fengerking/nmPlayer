LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := voDRMShell

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../DRMShell

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CSRC_PATH)/DRMShell.cpp \
	$(CSRC_PATH)/DRMShell_Labgency1.cpp \
	$(CSRC_PATH)/DRMShell_Labgency2.cpp \
	$(CSRC_PATH)/voDRMShell.cpp
	
#	$(CSRC_PATH)/DRMShell_Manager.cpp \

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	../../../../../../../../project/bluestreak/ndkPlayer_WMV/ndk2/NDK2Player/jni \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_FUNC -D_VOLOG_INFO -D_VOLOG_WARNING

# about info option, do not need to care it
LOCAL_CFLAGS := -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=soft -fsigned-char
LOCAL_LDLIBS := -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

