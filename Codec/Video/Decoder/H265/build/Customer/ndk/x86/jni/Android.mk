LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoH265Dec

CMNSRC_PATH:=../../../../../../../../../Common
CSRC_PATH:=../../../../../src


LOCAL_SRC_FILES := \
        $(CMNSRC_PATH)/voLog.c\
        $(CSRC_PATH)/h265dec_cabac.c\
	$(CSRC_PATH)/h265dec_deblock.c\
	$(CSRC_PATH)/h265dec_debug.c\
	$(CSRC_PATH)/h265dec_frameThd.c\
	$(CSRC_PATH)/h265dec_front.c\
	$(CSRC_PATH)/h265dec_idct.c\
	$(CSRC_PATH)/h265dec_inter.c\
	$(CSRC_PATH)/h265dec_list.c\
	$(CSRC_PATH)/h265dec_mem.c \
	$(CSRC_PATH)/h265dec_mvpred.c\
	$(CSRC_PATH)/h265_decoder.c\
	$(CSRC_PATH)/h265dec_parser.c\
	$(CSRC_PATH)/h265dec_recon.c\
	$(CSRC_PATH)/h265dec_sao.c\
	$(CSRC_PATH)/h265dec_sbac.c\
	$(CSRC_PATH)/h265dec_slice.c\
	$(CSRC_PATH)/h265dec_vlc.c\
	$(CSRC_PATH)/h265dec_dequant.c\
	$(CSRC_PATH)/HMLog.c
	  



LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH)

			  
VOMM:= -DVOANDROID -DANDROID -DENABLE_LICENSE_CHECK -DNDEBUG -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID

LOCAL_CFLAGS := -D_VOMODULEID=0x020e0000  -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := -llog ../../../../../../../../../Lib/ndk/x86/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

