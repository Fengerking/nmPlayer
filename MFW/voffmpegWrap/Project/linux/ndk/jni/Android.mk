LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoH264Dec


CMNSRC_PATH:=../../../../../../Common
VOME2SRC_PATH:=../../../../Source

LOCAL_SRC_FILES := \
	$(CMNSRC_PATH)/voLog.c \
	$(VOME2SRC_PATH)/voffmpegWrap.c


LOCAL_C_INCLUDES := \
	../../../../../../Include \
	$(CMNSRC_PATH) \
	$(VOME2SRC_PATH) \
	../../../../../../ffmpeg/jni/ffmpeg \
	../../../../../../ffmpeg/jni/ffmpeg/libavcodec

			


VOMM:= -DPD -DMS_HTTP -DLINUX -D_LINUX -D_LINUX_ANDROID -D__VO_NDK__ -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO

LOCAL_CFLAGS := -D_VOMODULEID=0x02010000   -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char

LOCAL_LDLIBS  := -llog -ldl -lstdc++ \
				 -L../../../../../../ffmpeg/libs/armeabi-v7a -lffmpeg
#LOCAL_LDLIBS  := -llog -ldl -lstdc++

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

