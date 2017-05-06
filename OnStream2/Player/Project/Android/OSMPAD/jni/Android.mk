LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../..

include $(CLEAR_VARS)
LOCAL_MODULE := voOSMPADSManagerJni

# Note: Two jni .so files are built in ContentSDK/Android/Java/jni.  The top
#	level Android.mk cannot have two ndkversion.o built from the top source
#	.  We copy it manually to vome
#	subdirectory and specify a local source to build
#	a separate ndkversion.o.


# about info option
LOCAL_CFLAGS := -D_VOMODULEID=0x09050000  -DNDEBUG -fsigned-char -Wall

LOCAL_CPPFLAGS  := \
		-D_LINUX_ANDROID \
		-D__VO_NDK__ \
		-D_VOLOG_ERROR \
		-D_VOLOG_WARNING \
		-D_VOLOG_INFO \
		-D_LINUX \
		-DLINUX \
		-fsigned-char -fno-short-enums -g -Wno-write-strings -mfloat-abi=soft -fsigned-char

LOCAL_SRC_FILES := 	com_visualon_OSMPPlayerImpl_VOOSMPADSManager.cpp \
            VOOSMPADSManagerJNI.cpp \
			../../../../../../Common/CJniEnvUtil.cpp \
			../../../../../../Common/CJavaParcelWrap.cpp \
			../../../../../../Common/CJavaParcelWrapOSMP.cpp\
			../../../../../../Common/CJavaJNI.cpp\
			../../../../../../Common/voLog.c
					


LOCAL_C_INCLUDES :=	../../../../../../Include/ \
../../../../../../Include/OSMP_V3 \
../../../../../../Common \

LOCAL_STATIC_LIBRARIES :=

LOCAL_SHARED_LIBRARIES := 

LOCAL_LDLIBS    :=  -llog -L../../../../../../Lib/ndk/ -lvodl \

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

