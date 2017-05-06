LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoAudioFW

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/AudioFW
CSRCMN_PATH:=../../../../../../File/Common

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnMemory.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CSRC_PATH)/CQCPWriter.cpp \
	$(CSRC_PATH)/voQCPWriter.cpp \
	$(CSRC_PATH)/CAACWriter.cpp\
	$(CSRC_PATH)/voAACWriter.cpp \
	$(CSRC_PATH)/CMP3Writer.cpp \
	$(CSRC_PATH)/voMP3Writer.cpp \
	$(CSRC_PATH)/CAMRWriter.cpp \
	$(CSRC_PATH)/voAMRWriter.cpp \
	$(CSRCMN_PATH)/CvoBaseMemOpr.cpp \
	$(CSRCMN_PATH)/CvoBaseFileOpr.cpp 
	


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH)



VOMM:= -D_USE_J3LIB -D__arm -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_Writer -D_VOLOG_FUNC -D_VOLOG_INFO -D_VOLOG_WARNING

# about info option, do not need to care it
LOCAL_CFLAGS := -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp
LOCAL_LDLIBS := -llog 

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

