LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../..

include $(CLEAR_VARS)
LOCAL_MODULE := voOSSource

# Note: Two jni .so files are built in ContentSDK/Android/Java/jni.  The top
#	level Android.mk cannot have two ndkversion.o built from the top source
#	.  We copy it manually to vome
#	subdirectory and specify a local source to build
#	a separate ndkversion.o.


# about info option
LOCAL_CFLAGS := -D_VOMODULEID=0x09020000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -Wno-write-strings

LOCAL_CPPFLAGS  := \
		-D_LINUX_ANDROID \
		-D__VO_NDK__ \
		-D_VOLOG_ERROR \
		-D_VOLOG_WARNING \
		-D_VOLOG_INFO \
		-DLINUX \
		-D_LINUX \
		-D_VONAMESPACE=voOSSource

LOCAL_SRC_FILES := ../../jni/osmpSourceJni.cpp \
				   ../../../../Source/Common/COSBaseSource.cpp \
				   ../../../../Source/Common/COSHLSSource.cpp \
				   ../../../../Source/Common/COSAdapStrSource.cpp \
				   ../../../../Source/Common/COSSourceBase.cpp \
				   ../../../../Source/Common/COSDASHSource.cpp \
				   ../../../../Source/Common/COSISSSource.cpp \
				   ../../../../Source/Common/COSLocalSource.cpp \
				   ../../../../Source/Common/COSPDMSSource.cpp \
				   ../../../../Source/Common/COSRTSPSource.cpp \
				   ../../../../Source/Common/COSCMMBSource.cpp \
				   ../../../../Source/Common/COSBaseSubtitle.cpp \
				   ../../../../Source/Common/COSCloseCaption.cpp \
				   ../../../../Source/Common/COSDVBSubtitle.cpp \
				   ../../../../Source/Common/COSTimeText.cpp \
				   ../../../../Source/Common/cioFile.cpp \
				   ../../../../../../Common/CFileFormatCheck.cpp \
				   ../../../../../../Common/cmnFile.cpp \
				   ../../../../../../Common/voLog.c \
				   ../../../../../../Common/cmnVOMemory.cpp \
				   ../../../../../../Common/CvoBaseObject.cpp \
				   ../../../../../../Common/CBaseConfig.cpp \
				   ../../../../../../Common/voOSFunc.cpp \
				   ../../../../../../Common/CJavaParcelWrap.cpp \
				   ../../../../../../Common/CJavaParcelWrapOSMP.cpp\
				   ../../../../../../Common/CJniEnvUtil.cpp \
				   ../../../../../../Common/voThread.cpp \
				   ../../../../../../Common/voCMutex.cpp \
				   ../../../../../../Common/voCSemaphore.cpp \
				   ../../../../../../Common/voCBaseThread.cpp \
				   ../../../../../../Common/voSubtitleFunc.cpp \
				   ../../../../../../Common/voProgramInfo.cpp \
				   ../../../../../../Common/Buffer/voSourceSubtitleDataBuffer.cpp \


LOCAL_C_INCLUDES :=	../../../../../../Include \
					../../../../../../Common/Buffer \
					../../../../Source/Common \
					../../../../../../Common 


LOCAL_LDLIBS    := -llog -L../../../../../../Lib/ndk/x86/ -lvodl \
				   ../../../../../../Lib/ndk/x86/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

