LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoRealVideoDec

CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../src

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/ai.c \
	$(CSRC_PATH)/basic.c \
	$(CSRC_PATH)/bsx.c \
	$(CSRC_PATH)/cinterp4.c \
	$(CSRC_PATH)/cinterp.c \
	$(CSRC_PATH)/deblock8.c \
	$(CSRC_PATH)/deblock.c \
	$(CSRC_PATH)/dec4x4md.c \
	$(CSRC_PATH)/decoder.c \
	$(CSRC_PATH)/drv.c \
	$(CSRC_PATH)/frame.c \
	$(CSRC_PATH)/idct.c \
	$(CSRC_PATH)/parsebs.c \
	$(CSRC_PATH)/recon.c \
	$(CSRC_PATH)/rv_decode.c \
	$(CSRC_PATH)/tables.c \
	$(CSRC_PATH)/voRVDecSDK.c \
	$(CSRC_PATH)/voRVThread.c

LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__  -DLINUX -D_LINUX -D_LINUX_ANDROID -DVOANDROID -DLICENSEFILE

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x02050000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32

LOCAL_LDLIBS:=../../../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

