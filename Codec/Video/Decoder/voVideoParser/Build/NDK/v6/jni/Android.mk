LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyVideoParser


CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../Src

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/voReadBits.c \
	$(CSRC_PATH)/voVideoParserFront.c \
	$(CSRC_PATH)/voVP6Parser.c \
	$(CSRC_PATH)/voVP8Parser.c \
	$(CSRC_PATH)/voWMVParser.c \
	$(CSRC_PATH)/voMPEG2Parser.c \
	$(CSRC_PATH)/voMPEG4Parser.c \
	$(CSRC_PATH)/voH263Parser.c \
	$(CSRC_PATH)/voH264Parser.c \
	


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../Inc\
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= 

LOCAL_CFLAGS := -D_VOMODULEID=0x020d0000   -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp
LOCAL_LDLIBS := -llog

include $(BUILD_STATIC_LIBRARY)

