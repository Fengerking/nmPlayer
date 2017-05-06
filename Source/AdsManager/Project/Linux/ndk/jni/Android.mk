LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoAdsManager

CMNSRC_PATH:=../../../../../../Common
ADSMANAGER_PATH:=../../../../Source
MAINCMNSRC_PATH:=../../../../../Common
FILECMNSRC_PATH:=../../../../../File/Common

LOCAL_SRC_FILES := \
  	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(MAINCMNSRC_PATH)/CSourceIOUtility.cpp \
	$(MAINCMNSRC_PATH)/vo_thread.cpp \
	$(FILECMNSRC_PATH)/base64.cpp \
	$(ADSMANAGER_PATH)/AdsManager.cpp \
	$(ADSMANAGER_PATH)/vo_ads_manager.cpp \
	$(ADSMANAGER_PATH)/vo_ads_utility.cpp \
	$(ADSMANAGER_PATH)/vo_smil.cpp \
	$(ADSMANAGER_PATH)/vo_vmap.cpp \
	$(ADSMANAGER_PATH)/vo_message_thread.cpp \
	$(ADSMANAGER_PATH)/voHMAC_SHA1.cpp \
	$(ADSMANAGER_PATH)/voSHA1.cpp
	

LOCAL_C_INCLUDES := \
	../../../../../../Include \
     	../../../../../../Common \
	../../../../../Common \
	../../../../../Include \
	../../../../../File/Common
        
        




VOMM:= -DLINUX -D_LINUX -DHAVE_PTHREADS -D_LINUX_ANDROID -D__VO_NDK__ -D_VONAMESPACE=libvoAdsManager -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_VOLOG_RUN

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x01160000  -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp  -mfloat-abi=softfp -msoft-float
LOCAL_LDLIBS := -llog -ldl -lstdc++ -lgcc -L../../../../../../Lib/ndk/ -lvodl

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
