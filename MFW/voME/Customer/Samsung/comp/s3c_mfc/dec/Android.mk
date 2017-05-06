LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/SsbSipH264Decode.c \
	src/SsbSipMpeg4Decode.c \
	src/SsbSipVC1Decode.c \
	src/SsbSipLogMsg.c

LOCAL_MODULE := libmfcdecapi

LOCAL_CFLAGS :=   $(PV_CFLAGS)

LOCAL_CFLAGS += -DDIVX_ENABLE

LOCAL_ARM_MODE := arm

LOCAL_STATIC_LIBRARIES := 

LOCAL_SHARED_LIBRARIES :=

LOCAL_C_INCLUDES := \
	$(PV_TOP)/codecs_v2/video/s3c_mfc/dec/include \
	$(PV_TOP)/codecs_v2/video/s3c_mfc/dec/src \
	$(PV_INCLUDES) 

LOCAL_COPY_HEADERS_TO := $(PV_COPY_HEADERS_TO)

LOCAL_COPY_HEADERS := \
	include/SsbSipH264Decode.h \
	include/SsbSipMpeg4Decode.h \
	include/SsbSipVC1Decode.h \

include $(BUILD_STATIC_LIBRARY)

