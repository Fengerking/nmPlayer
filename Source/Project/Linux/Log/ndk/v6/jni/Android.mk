LOCAL_PATH:=$(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE:=arm

LOCAL_MODULE:=libvoLog


LOGSRC_PATH:=../../../../../../File/Log
LOGCMN_PATH:=../../../../../../File/Common
TOP_INCLUDE_PATH:=../../../../../../../Include
TOP_CMN_PATH:=../../../../../../../Common

LOCAL_SRC_FILES := \
	\
	$(LOGSRC_PATH)/PD_log_client.cpp	\
	$(TOP_CMN_PATH)/voCMutex.cpp	\
	$(TOP_CMN_PATH)/CvoBaseObject.cpp	\
	$(TOP_CMN_PATH)/CBaseConfig.cpp	\
	$(TOP_CMN_PATH)/voOSFunc.cpp	\
	$(TOP_CMN_PATH)/cmnMemory.c	\
	$(TOP_CMN_PATH)/cmnFile.cpp	
	
		


LOCAL_C_INCLUDES:= \
	$(TOP_INCLUDE_PATH)	\
	$(TOP_CMN_PATH)	\
	$(LOGSRC_PATH) \
	$(PDCMN_PATH)	
	


VOMM:=-DLINUX -D_LINUX -D_LINUX_ANDROID -DLINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D__VO_NDK__

# about info option, do not need to care it

LOCAL_CFLAGS :=  -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char
LOCAL_LDLIBS := -llog -ldl  -lgcc -lstdc++

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

