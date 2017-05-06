LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoPNGDec


CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../src

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/libpng/png.c\
	$(CSRC_PATH)/libpng/pngerror.c\
	$(CSRC_PATH)/libpng/pnggccrd.c\
	$(CSRC_PATH)/libpng/pngget.c\
	$(CSRC_PATH)/libpng/pngmem.c\
	$(CSRC_PATH)/libpng/pngpread.c\
	$(CSRC_PATH)/libpng/pngread.c\
	$(CSRC_PATH)/libpng/pngrio.c\
	$(CSRC_PATH)/libpng/pngrtran.c\
	$(CSRC_PATH)/libpng/pngrutil.c\
	$(CSRC_PATH)/libpng/pngset.c\
	$(CSRC_PATH)/libpng/pngtrans.c\
	$(CSRC_PATH)/libpng/pngvcrd.c\
	$(CSRC_PATH)/libpng/pngwio.c\
	$(CSRC_PATH)/libpng/pngwrite.c\
  $(CSRC_PATH)/libpng/pngwtran.c\
  $(CSRC_PATH)/libpng/pngwutil.c\
  $(CSRC_PATH)/zlib/adler32.c\
  $(CSRC_PATH)/zlib/compress.c\
  $(CSRC_PATH)/zlib/crc32.c\
  $(CSRC_PATH)/zlib/deflate.c\
  $(CSRC_PATH)/zlib/gzio.c\
  $(CSRC_PATH)/zlib/infback.c\
  $(CSRC_PATH)/zlib/inffast.c\
  $(CSRC_PATH)/zlib/inflate.c\
  $(CSRC_PATH)/zlib/inftrees.c\
  $(CSRC_PATH)/zlib/trees.c\
  $(CSRC_PATH)/zlib/uncompr.c\
  $(CSRC_PATH)/zlib/zutil.c\
  $(CSRC_PATH)/voPNGDecFront.c\
  $(CSRC_PATH)/voPNGMemmory.c
  
LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH)\
	$(CSRC_PATH)/libpng\
	$(CSRC_PATH)/zlib\
	$(CSRC_PATH)



VOMM:= -D__VO_NDK__ -D_VOLOG_INFO -DLINUX -D_LINUX -D_LINUX_ANDROID -DARM  -DRVDS -DLINUX

LOCAL_CFLAGS := -D_VOMODULEID=0x02430000  -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

