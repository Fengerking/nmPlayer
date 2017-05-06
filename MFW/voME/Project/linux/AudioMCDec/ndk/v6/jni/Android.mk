LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoAudioMCDec


CMNSRC_PATH:=../../../../../../../../Common
ACODEC_PATH:=../../../../../../AMediaCodec
DRM_COM_PATH:=../../../../../../../../Source/DRM/Common
SRC_COM_PATH:=../../../../../../../../Source/Common
SRC_INC_PATH:=../../../../../../../../Source/Include
SRC_FS_COM_PATH=../../../../../../../../Source/File/Common
JC_PATH:=$(ACODEC_PATH)/JCCodec
UTL_PATH=$(ACODEC_PATH)/Util

LOCAL_SRC_FILES := \
	\
	$(UTL_PATH)/JCEnvUtil.cpp \
	$(CMNSRC_PATH)/fAudioHeadDataInfo.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(ACODEC_PATH)/voAudioMCDec.cpp \
	$(ACODEC_PATH)/AMCWrap.cpp \
	$(JC_PATH)/JCMediaCodec.cpp \
	$(JC_PATH)/JCMediaFormat.cpp \
	$(JC_PATH)/JCBufferInfo.cpp \
	$(JC_PATH)/JCObject.cpp  \
	$(JC_PATH)/JCCryptoInfo.cpp \
	$(JC_PATH)/JCMediaCrypto.cpp \
	$(JC_PATH)/JCUUID.cpp  \
	$(JC_PATH)/JCMediaCodecList.cpp \
	$(JC_PATH)/JCMediaCodecInfo.cpp \
	$(DRM_COM_PATH)/DRMToolUtility.cpp \
	$(SRC_FS_COM_PATH)/base64.cpp

LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	$(UTL_PATH) \
	$(CMNSRC_PATH) \
	$(ACODEC_PATH) \
	$(JC_PATH) \
	$(DRM_COM_PATH) \
	$(SRC_COM_PATH) \
	$(SRC_INC_PATH) \
	$(SRC_FS_COM_PATH)



#-D_VONAMESPACE=$(LOCAL_MODULE)
VOMM:= -DLINUX -D_LINUX -D_LINUX_ANDROID -D__VO_NDK__ -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO 

LOCAL_CFLAGS := -D_VOMODULEID=0x020d0000   -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char -Wno-multichar

#LOCAL_LDLIBS := 
LOCAL_LDLIBS  := -llog -ldl -lstdc++ -L../../../../../../../../Lib/ndk/ -lvodl -lvoVideoParser				

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

