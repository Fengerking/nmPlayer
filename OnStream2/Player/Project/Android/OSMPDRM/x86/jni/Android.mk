LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)
LOCAL_MODULE := voDRMJNI

# Note: Two jni .so files are built in ContentSDK/Android/Java/jni.  The top
#	level Android.mk cannot have two ndkversion.o built from the top source
#	.  We copy it manually to vome
#	subdirectory and specify a local source to build
#	a separate ndkversion.o.


# about info option
LOCAL_CFLAGS := -D_VOMODULEID=0x09070000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32

LOCAL_CPPFLAGS  := \
		-D_LINUX_ANDROID \
		-D__VO_NDK__ \
		-D_VOLOG_ERROR \
		-D_VOLOG_WARNING \
		-D_VOLOG_INFO \
		-D_LINUX \
		-DLINUX \
		-Wno-conversion-null -Wno-write-strings


LOCAL_SRC_FILES := 	../../jni/voDRMJni.cpp \
					../../../../../../../Common/CJniEnvUtil.cpp \
					../../../../../../../Common/voLog.c



LOCAL_C_INCLUDES :=	../../../../../../../Include \
				../../../../../../../Common \

LOCAL_STATIC_LIBRARIES :=

LOCAL_SHARED_LIBRARIES := 

LOCAL_LDLIBS    :=  -llog -L../../../../../../../Lib/ndk/x86/ -lvodl \

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

