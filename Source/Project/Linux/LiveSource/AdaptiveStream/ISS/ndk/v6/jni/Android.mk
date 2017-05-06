LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := voAdaptiveStreamISS

CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../../../../../Source

LOCAL_SRC_FILES := \
  $(CMNSRC_PATH)/CvoBaseObject.cpp \
  $(CMNSRC_PATH)/voLog.c \
	\
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voXMLLoad.cpp \
	$(CSRC_PATH)/File/Common/fCodec.cpp \
	$(CSRC_PATH)/File/Common/CCodeCC.cpp \
	$(CSRC_PATH)/Common/vo_thread.cpp \
	$(CSRC_PATH)/Common/CDataBox.cpp \
	$(CSRC_PATH)/AdaptiveStreaming/ISS/ISS_ManifestParser.cpp \
	$(CSRC_PATH)/AdaptiveStreaming/ISS/ISS_ManifestManager.cpp \
	$(CSRC_PATH)/AdaptiveStreaming/ISS/ISS_SmoothStreaming.cpp \
	$(CSRC_PATH)/AdaptiveStreaming/ISS/voAdaptiveStreamISS.cpp
		
LOCAL_C_INCLUDES := \
../../../../../../../../../Include \
../../../../../../../../../Common \
$(CSRC_PATH)/AdaptiveStreaming/Common \
$(CSRC_PATH)/AdaptiveStreaming/ISS \
$(CSRC_PATH)/File/XML \
$(CSRC_PATH)/File/Common \
$(CSRC_PATH)/Common \
$(CSRC_PATH)/Include        


LOCAL_STATIC_LIBRARIES := cpufeatures


VOMM:= -DLINUX -D_LINUX -DHAVE_PTHREADS -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOLOG_RUN -D_DASH_SOURCE_ -D__VO_NDK__ -D_new_programinfo

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01122000  -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp  -mfloat-abi=softfp -msoft-float
LOCAL_LDLIBS := ../../../../../../../../../Lib/ndk/libvoCheck.a -llog -ldl -lstdc++ -lgcc -L../../../../../../../../../Lib/ndk/ -lvodl

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

$(call import-module,cpufeatures)