LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoWMVDec


CMNSRC_PATH  :=../../../../../../../../../Common
CSRC_PATH    :=../../../../../Src/c
CSRC_EMB_PATH:=../../../../../Src/emb
CSRC_GNU_PATH:=../../../../../Src/armX_gnu


LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/Affine.c \
	$(CSRC_PATH)/effects_wmv.c \
	$(CSRC_PATH)/idctdec_wmv9.c \
	$(CSRC_PATH)/InterlaceDecPV2.c \
	$(CSRC_PATH)/localhuffman_wmv.c \
	$(CSRC_PATH)/reflibrary_wmv9.c \
	$(CSRC_PATH)/tables_wmv.c \
	$(CSRC_PATH)/wmvdec.c \
	$(CSRC_PATH)/blkdec_wmv.c \
	$(CSRC_PATH)/FrameTimer.c \
	$(CSRC_PATH)/init_wmv.c \
	$(CSRC_PATH)/InterlaceDecTable.c \
	$(CSRC_PATH)/motioncomp_wmv.c \
	$(CSRC_PATH)/share.c \
	$(CSRC_PATH)/threadctr_dec_wmv9.c \
	$(CSRC_PATH)/deblock.c \
	$(CSRC_PATH)/frmdec_wmv.c \
	$(CSRC_PATH)/InterlaceCore_wmv9.c \
	$(CSRC_PATH)/InterlaceDecV2.c \
	$(CSRC_PATH)/multiresdec_wmv9.c \
	$(CSRC_PATH)/sideroll_wmv.c \
	$(CSRC_PATH)/uncache_wmv.c \
	$(CSRC_PATH)/dectable.c \
	$(CSRC_PATH)/headdec.c \
	$(CSRC_PATH)/InterlaceDec_BFrame.c \
	$(CSRC_PATH)/InterlaceDec_wmv9.c \
	$(CSRC_PATH)/multires_wmv9.c \
	$(CSRC_PATH)/spatialpredictor_wmv.c \
	$(CSRC_PATH)/vopdec9_wmv9.c \
	$(CSRC_PATH)/dectable_mp4x.c \
	$(CSRC_PATH)/huffdec_wmv.c \
	$(CSRC_PATH)/InterlaceDecBV2.c \
	$(CSRC_PATH)/interpolate_wmv9.c \
	$(CSRC_PATH)/pageroll_wmv.c \
	$(CSRC_PATH)/startcodedec.c \
	$(CSRC_PATH)/vopdec_BFrame.c \
	$(CSRC_PATH)/deinterlace_wmv9.c \
	$(CSRC_PATH)/idctdec_wmv2.c \
	$(CSRC_PATH)/InterlaceDecCoreV2.c \
	$(CSRC_PATH)/intradec.c \
	$(CSRC_PATH)/postfilter_wmv.c \
	$(CSRC_PATH)/strmdec_wmv.c \
	$(CSRC_PATH)/vopdec_mp4x.c \
	$(CSRC_EMB_PATH)/idctdec_wmv9_emb.c \
	$(CSRC_EMB_PATH)/idctdec_emb.c \
	$(CSRC_EMB_PATH)/interpolate_wmv9_emb.c \
	$(CSRC_EMB_PATH)/motioncomp_emb.c \
	$(CSRC_EMB_PATH)/vopdec9_wmv9_emb.c \
	$(CSRC_EMB_PATH)/vopdecB_emb.c \
	$(CSRC_EMB_PATH)/vopdec_emb.c

LOCAL_C_INCLUDES := \
	../../../../../../../../../Include \
	$(CMNSRC_PATH)		\
	$(CSRC_PATH)		\
	$(CSRC_EMB_PATH)    \
	$(CSRC_GNU_PATH)    \
	$(CSRC_GNU_PATH)/incgen

VOMM:= -DLINUX -D_LINUX_ANDROID -D_VOLOG_INFO -D_VOLOG_ERROR -D__VO_NDK__ -DNDEBUG

LOCAL_CFLAGS := -D_VOMODULEID=0x02060000  -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := -llog ../../../../../../../../../Lib/ndk/x86/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)


