LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := MagicvoiceTest

CMM_SRC1:=../../../../source/common
CMM_SRC:=../../../../source/SoundTouch
CMM_SRC2:=../../../../../../../Common
TEST_SRC:=../../../../test
LOCAL_SRC_FILES := \
	        $(CMM_SRC)/AAFilter.cpp \
		$(CMM_SRC)/BPMDetect.cpp \
		$(CMM_SRC)/FIFOSampleBuffer.cpp \
		$(CMM_SRC)/FIRFilter.cpp \
		$(CMM_SRC)/mmx_optimized.cpp \
		$(CMM_SRC)/PeakFinder.cpp \
		$(CMM_SRC)/RateTransposer.cpp \
		$(CMM_SRC)/SoundTouch.cpp \
		$(CMM_SRC)/sse_optimized.cpp \
		$(CMM_SRC)/TDStretch.cpp \
		$(CMM_SRC)/VoSoundTouch.cpp \
		$(CMM_SRC1)/mem_align.cpp \
	        $(CMM_SRC2)/cmnMemory.c \
	        $(TEST_SRC)/sound_touch_sample.c \
	        $(TEST_SRC)/WavFile.c	


LOCAL_C_INCLUDES := \
	../../../../include/common \
	../../../../include \
	../../../../source/SoundTouch \
	../../../../../../../Include \
	../../../../../../../Common

JPDEFINE:=-DLINUX -DARM -DARMV7 

# about info option, do not need to care it

LOCAL_CFLAGS := $(JPDEFINE)  -DNDEBUG -O3 -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS:=-llog

include $(BUILD_EXECUTABLE)

