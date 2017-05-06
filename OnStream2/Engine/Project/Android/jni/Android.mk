LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../..

include $(CLEAR_VARS)
LOCAL_MODULE := voOSEng

# Note: Two jni .so files are built in ContentSDK/Android/Java/jni.  The top
#	level Android.mk cannot have two ndkversion.o built from the top source
#	.  We copy it manually to vome
#	subdirectory and specify a local source to build
#	a separate ndkversion.o.


# about info option
VOMM:= 	-D_LINUX_ANDROID \
		-D__VO_NDK__ \
		-D_VOLOG_ERROR \
		-D_VOLOG_WARNING \
		-D_VOLOG_INFO \
		-D_LINUX \
		-DLINUX \

LOCAL_CFLAGS := $(VOMM) -D_VOMODULEID=0x09030000  -DNDEBUG -fsigned-char -Wall

LOCAL_CPPFLAGS  := \
		$(VOMM) \
		-D_VONAMESPACE=libvoOSEng \
		-fsigned-char -fno-short-enums -g -Wno-write-strings -mfloat-abi=soft -fsigned-char

LOCAL_SRC_FILES := OnStreamjni.cpp \
    		voAudioRenderjni.cpp \
		\
		../../../../../Common/voOSFunc.cpp \
		../../../../../Common/CDllLoad.cpp \
		../../../../../Common/cmnVOMemory.cpp \
		../../../../../Common/cmnFile.cpp \
		../../../../../Common/CBaseConfig.cpp \
		../../../../../Common/voHalInfo.cpp \
		../../../../../Common/CVideoSubtitle.cpp \
		../../../../../Common/voLog.c \
		../../../../../Common/voCMutex.cpp \
		../../../../../Common/CvoBaseObject.cpp \
		../../../../../Common/CJniEnvUtil.cpp \
		../../../../../Common/CJavaParcelWrap.cpp \
		../../../../../Common/CJavaParcelWrapOSMP.cpp\
		../../../../../Common/voSubtitleFunc.cpp\
		../../../../../Common/Buffer/voSourceSubtitleDataBuffer.cpp \
		../../../../../Common/Buffer/voH264SEIDataBuffer.cpp\
		../../../../../Common/CModuleVersion.cpp \
		../../../Source/Common/COSBasePlayer.cpp \
		../../../Source/Common/COSVomeEngine.cpp \
		../../../Source/Common/COSVomePlayer.cpp \
		../../../Source/Common/COMXALEngine.cpp	\
		../../../Source/Common/COMXALPlayer.cpp	\
		../../../Source/Common/CSEIInfo.cpp \
		../../../Source/Android/COSNdkVomePlayer.cpp	\
		../../../Source/Android/COSNdkVideoRender.cpp	\
		

LOCAL_C_INCLUDES :=	../../../../../Include \
				../../../../../Common \
				../../../../../Common/Buffer \
				../../../Source/Common \
				../../../Source/Android/ \
	

LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_LDLIBS    := -llog -L../../../../../Lib/ndk/ -lvodl ../../../../../Lib/ndk/libvoVideoParser.a \
                  ../../../../../Lib/ndk/libvoCheck.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
$(call import-module,cpufeatures)
