LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoVC1Dec


CMNSRC_PATH  :=../../../../../../../../../Common
CSRC_PATH    :=../../../../../Src/c
CSRC_EMB_PATH:=../../../../../Src/emb


LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/idctdec_wmv9.c\
  $(CSRC_PATH)/InterlaceDecPV2.c \
  $(CSRC_PATH)/localhuffman_wmv.c\
  $(CSRC_PATH)/reflibrary_wmv9.c\
  $(CSRC_PATH)/tables_wmv.c \
  $(CSRC_PATH)/wmvdec.c \
  $(CSRC_PATH)/blkdec_wmv.c\
  $(CSRC_PATH)/init_wmv.c \
  $(CSRC_PATH)/InterlaceDecTable.c \
  $(CSRC_PATH)/motioncomp_wmv.c \
  $(CSRC_PATH)/share.c \
  $(CSRC_PATH)/threadctr_dec_wmv9.c \
  $(CSRC_PATH)/deblock.c \
  $(CSRC_PATH)/frmdec_wmv.c \
  $(CSRC_PATH)/InterlaceCore_wmv9.c \
  $(CSRC_PATH)/InterlaceDecV2.c\
  $(CSRC_PATH)/multiresdec_wmv9.c\
  $(CSRC_PATH)/dectable.c \
  $(CSRC_PATH)/headdec.c\
  $(CSRC_PATH)/InterlaceDec_BFrame.c\
  $(CSRC_PATH)/InterlaceDec_wmv9.c\
  $(CSRC_PATH)/multires_wmv9.c\
  $(CSRC_PATH)/spatialpredictor_wmv.c\
  $(CSRC_PATH)/vopdec9_wmv9.c\
  $(CSRC_PATH)/huffdec_wmv.c\
  $(CSRC_PATH)/InterlaceDecBV2.c\
  $(CSRC_PATH)/interpolate_wmv9.c\
  $(CSRC_PATH)/startcodedec.c\
  $(CSRC_PATH)/vopdec_BFrame.c \
  $(CSRC_PATH)/idctdec_wmv2.c\
  $(CSRC_PATH)/InterlaceDecCoreV2.c\
  $(CSRC_PATH)/intradec.c\
  $(CSRC_PATH)/strmdec_wmv.c\
  $(CSRC_PATH)/voVc1Thread.c \
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
	$(CSRC_EMB_PATH)


#-DVOARMV7 -DHAVE_NEON=1 -DARM -D_ARM_ -D__arm -DLINUX -D_LINUX_ANDROID -D_VOLOG_INFO -D_VOLOG_ERROR -D__VO_NDK__ -DVOANDROID -DMULTI_CORE_DEC -DADJ_NEW -DUSE_FRAME_THREAD -DNDEBUG
VOMM:= -DLINUX -D_LINUX_ANDROID -D_VOLOG_INFO -D_VOLOG_ERROR -D__VO_NDK__ -DVOANDROID -DMULTI_CORE_DEC -DADJ_NEW -DUSE_FRAME_THREAD -DNDEBUG

LOCAL_CFLAGS := -D_VOMODULEID=0x020c0000   -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := -llog ../../../../../../../../../Lib/ndk/x86/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)


