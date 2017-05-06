LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)
LOCAL_MODULE := voOSEng

# Note: Two jni .so files are built in ContentSDK/Android/Java/jni.  The top
#	level Android.mk cannot have two ndkversion.o built from the top source
#	.  We copy it manually to vome
#	subdirectory and specify a local source to build
#	a separate ndkversion.o.


# about info option
LOCAL_CFLAGS := -D_VOMODULEID=0x09030000  -DNDEBUG -fsigned-char -Wall

LOCAL_CPPFLAGS  := \
		-D_LINUX_ANDROID \
		-D__VO_NDK__ \
		-D_VOLOG_ERROR \
		-D_VOLOG_WARNING \
		-D_VOLOG_INFO \
		-D_LINUX \
		-D__ODM__ \
		-DLINUX \
		-fsigned-char -fno-short-enums -g -Wno-write-strings -mfloat-abi=soft -fsigned-char

LOCAL_SRC_FILES := \
		\
		../../../../../../../Common/voOSFunc.cpp \
		../../../../../../../Common/CDllLoad.cpp \
		../../../../../../../Common/voHalInfo.cpp \
		../../../../../../../Common/CVideoSubtitle.cpp \
		../../../../../../../Common/voLog.c \
		../../../../../../../Common/voCMutex.cpp \
		../../../../../../../Common/CvoBaseObject.cpp \
		../../../../../../../Common/voSubtitleFunc.cpp\
		../../../../../../../Common/voThread.cpp\
		../../../../../../../Common/Buffer/voSourceSubtitleDataBuffer.cpp \
		../../../../../Source/Common/COSBasePlayer.cpp \
		../../../../../Source/Common/COSVomeEngine.cpp \
		../../../../../Source/Common/COSVomePlayer.cpp \
		../../../../../Source/Android/odm/ODMAAudioRender.cpp \
		../../../../../Source/Android/odm/ODMAVideoRender.cpp \
		../../../../../Source/Android/odm/ODMAVomePlayer.cpp \
		../../../../../Source/Android/odm/voOnStreamSDK.cpp

LOCAL_C_INCLUDES :=	../../../../../../../Include \
			../../../../../../../Common \
			../../../../../../../Common/Buffer \
			../../../../../Source/Common \
			../../../../../Source/Android/odm

LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_LDLIBS    := -llog ../../../../../../../Lib/ndk/libvoVideoParser.a \

LOCAL_LDLIBS += -fuse-ld=gold

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
$(call import-module,cpufeatures)
