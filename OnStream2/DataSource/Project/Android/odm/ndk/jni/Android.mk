LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

Src_Com=../../../../../Source/Common
Top_Com=../../../../../../../Common
Top_Inc=../../../../../../../Include

include $(CLEAR_VARS)
LOCAL_MODULE := voOSSource

# Note: Two jni .so files are built in ContentSDK/Android/Java/jni.  The top
#	level Android.mk cannot have two ndkversion.o built from the top source
#	.  We copy it manually to vome
#	subdirectory and specify a local source to build
#	a separate ndkversion.o.


# about info option
LOCAL_CFLAGS := -D_VOMODULEID=0x09020000  -DNDEBUG -fsigned-char -Wall

LOCAL_CPPFLAGS  := \
		-D_LINUX_ANDROID \
		-D__VO_NDK__ \
		-D_VOLOG_ERROR \
		-D_VOLOG_WARNING \
		-D_VOLOG_INFO \
		-DLINUX \
		-D_LINUX \
		-D__ODM__ \
		-D_VONAMESPACE=voOSSource \
		-fsigned-char -fno-short-enums -g -Wno-write-strings -mfloat-abi=soft -fsigned-char


LOCAL_SRC_FILES := $(Src_Com)/voOnStreamSrc.cpp \
		$(Src_Com)/COSBaseSource.cpp \
		$(Src_Com)/osCMutex.cpp \
		$(Src_Com)/COSHLSSource.cpp \
		$(Src_Com)/COSAdapStrSource.cpp \
		$(Src_Com)/COSSourceBase.cpp \
		$(Src_Com)/COSDASHSource.cpp \
		$(Src_Com)/COSISSSource.cpp \
		$(Src_Com)/COSLocalSource.cpp \
		$(Src_Com)/COSPDMSSource.cpp \
		$(Src_Com)/COSRTSPSource.cpp \
		$(Src_Com)/COSCMMBSource.cpp \
		$(Src_Com)/COSTimeText.cpp \
		$(Top_Com)/CFileFormatCheck.cpp \
		$(Top_Com)/cmnFile.cpp \
		$(Top_Com)/voLog.c \
		$(Top_Com)/cmnVOMemory.cpp \
		$(Top_Com)/CvoBaseObject.cpp \
		$(Top_Com)/CBaseConfig.cpp \
		$(Top_Com)/voOSFunc.cpp \
		

LOCAL_C_INCLUDES := $(Top_Inc) \
		    $(Src_Com) \
		    $(Top_Com)

LOCAL_LDLIBS    := -llog  -fuse-ld=gold

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

