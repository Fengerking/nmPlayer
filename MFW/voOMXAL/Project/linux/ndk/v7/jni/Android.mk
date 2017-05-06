LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoOMXALWrap


CMNSRC_PATH:=../../../../../../../Common
OMXSRC_PATH:=../../../../../Source
MFWCMN_PATH:=../../../../../../Common
DLSRC_PATH:=../../../../../../../Utility/vodl/src

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/CJniEnvUtil.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/cmnVOMemory.cpp \
	$(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CPtrList.cpp \
	$(OMXSRC_PATH)/CADecoder.cpp \
	$(OMXSRC_PATH)/COMXALWrapper.cpp \
	$(OMXSRC_PATH)/OpenMAXVideoFrameSplit.cpp \
	$(OMXSRC_PATH)/voOMXALWrapAPI.cpp \
	$(OMXSRC_PATH)/COMXALMediaPlayer.cpp \
	$(OMXSRC_PATH)/vo_mem_stream.cpp \
	$(OMXSRC_PATH)/CMuxStream.cpp \
	$(OMXSRC_PATH)/CAEffect.cpp \
	$(OMXSRC_PATH)/CAVFrameTools.cpp \
	$(MFWCMN_PATH)/CBaseNode.cpp \
	


LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(MFWCMN_PATH) \
	


VOMM:= -DPD -DLINUX -D_LINUX -D_LINUX_ANDROID -D__VO_NDK__ -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VONAMESPACE=libvoOMXALWrap

LOCAL_CFLAGS := -D_VOMODULEID=0x21000000   -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=vfp -fsigned-char -Wall


#LOCAL_LDLIBS := 
LOCAL_LDLIBS  := -llog -ldl -lstdc++ -L../../../../../../../Lib/ndk/ -lvodl\

# for native multimedia
 LOCAL_LDLIBS    += -lOpenMAXAL
# for logging
LOCAL_LDLIBS    += -llog
# for native windows
 LOCAL_LDLIBS    += -landroid
# for video frame tools
 LOCAL_LDLIBS	+= ../../../../../../../Lib/ndk/libvoVideoParser.a


include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

