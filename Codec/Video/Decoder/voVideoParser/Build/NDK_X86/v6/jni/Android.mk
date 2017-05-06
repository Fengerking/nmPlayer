LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoVideoParser


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



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID -DVOANDROID=1

LOCAL_CFLAGS := -D_VOMODULEID=0x020d0000   -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := -llog

include $(BUILD_STATIC_LIBRARY)

