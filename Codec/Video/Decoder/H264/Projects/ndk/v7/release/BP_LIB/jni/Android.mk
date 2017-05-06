LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../../../..

include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoH264DecBP
#cd Numen/trunk/Codec/Video/Decoder/H264/Projects/ndk/v7/release/BP_LIB/jni/
ROOT_PATH := ../../../../../../../../../..
CMNSRC_PATH:=$(ROOT_PATH)/Common
CSRC_PATH:=../../../../../../Sources

LOCAL_SRC_FILES := $(CSRC_PATH)/ldecod.c \
	$(CSRC_PATH)/image.c \
	$(CSRC_PATH)/annexb.c \
	$(CSRC_PATH)/context_ini.c \
	$(CSRC_PATH)/cShortInline.c \
	$(CSRC_PATH)/mbuffer.c \
	$(CSRC_PATH)/errorconcealment.c \
	$(CSRC_PATH)/erc_api.c \
	$(CSRC_PATH)/InPlaceDeblockMBs.c \
	$(CSRC_PATH)/nalu.c \
	$(CSRC_PATH)/erc_do_i.c \
	$(CSRC_PATH)/cabac.c \
	$(CSRC_PATH)/voh264DecSDK.c \
	$(CSRC_PATH)/nalucommon.c \
	$(CSRC_PATH)/parset.c \
	$(CSRC_PATH)/nal_part.c \
	$(CSRC_PATH)/memalloc.c \
	$(CSRC_PATH)/block.c \
	$(CSRC_PATH)/biaridecod.c \
	$(CSRC_PATH)/cInvTrans.c \
	$(CSRC_PATH)/erc_do_p.c \
	$(CSRC_PATH)/mb_access.c \
	$(CSRC_PATH)/decPicBuf.c \
	$(CSRC_PATH)/vlc.c \
	$(CSRC_PATH)/rtp.c \
	$(CSRC_PATH)/loopFilter.c \
	$(CSRC_PATH)/h264VdLib.c \
	$(CSRC_PATH)/output.c \
	$(CSRC_PATH)/cGetBlock.c \
	$(CSRC_PATH)/header.c \
	$(CSRC_PATH)/neighbor.c \
	$(CSRC_PATH)/cCabac.c \
	$(CSRC_PATH)/filehandle.c \
	$(CSRC_PATH)/macroblock.c \
	$(CSRC_PATH)/sei.c \
	$(CSRC_PATH)/fmo.c \
	$(CSRC_PATH)/nal.c \
	$(CSRC_PATH)/parsetcommon.c 
	


LOCAL_C_INCLUDES := \
	$(ROOT_PATH)/Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRC_PATH)/old_sdk_inc 
	

VOMM:=-DNDEBUG -DLINUX -D_LINUX -DARM  -DARM_ASM -D_LINUX_ANDROID -DCOMBINE_ALL_PROFILE -DNEON -DHAVE_NEON=1 -DNDK_BUILD -DUSE_JOBPOOL

LOCAL_CFLAGS := -D_VOMODULEID=0x02010000  -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoH264Dec
LOCAL_STATIC_LIBRARIES := libvoH264DecBP 
include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)