LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoVMAPParser

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../VMAP
CSRCMN_PATH:=../../../../../../File/Common
CMNMTV_PATH:=../../../../../../MTV/Common
CVAST_PAYH:=../../../../../../VAST

LOCAL_SRC_FILES := \
	\
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/voOSFunc.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voXMLLoad.cpp \
	\
	$(CSRC_PATH)/CVMAPParser.cpp \
	$(CSRC_PATH)/voVMAPParser.cpp \
	\
	$(CVAST_PAYH)/CVASTParser.cpp\

LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CMNMTV_PATH) \
	$(CVAST_PAYH) \



VOMM:= -DLINUX -D_LINUX_ANDROID -D_LINUX -D_VOLOG_ERROR -D_VOLOG_INFO -D__arm -D__VO_NDK__

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x010d0000  -DNDEBUG -DARM -DARM_ASM -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog -L../../../../../../../Lib/ndk/ -lvodl

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

