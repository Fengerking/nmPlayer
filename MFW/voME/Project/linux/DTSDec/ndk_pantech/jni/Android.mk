LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libvoOMXDTSDec

CMN_SRC:=../../../../../Common
BASE_SRC:=../../../../../Component/Base
DTS_SRC:=../../../../../Component/AudioDTSDec
DTS_CORE:=../../../../../Component/AudioDTSDec/Core
CMN_SRC1:=../../../../../../Common
CMN_SRC2:=../../../../../../../Common
LOCAL_SRC_FILES := \
                $(CMN_SRC)/voCOMXBaseConfig.cpp \
		$(CMN_SRC)/voCOMXBaseObject.cpp \
		$(CMN_SRC)/voCOMXTaskQueue.cpp \
		$(CMN_SRC)/voCOMXThreadMutex.cpp \
		$(CMN_SRC)/voCOMXThreadSemaphore.cpp \
		$(CMN_SRC)/voOMXBase.cpp \
		$(CMN_SRC)/voOMXFile.cpp \
		$(CMN_SRC)/voOMXMemroy.cpp \
		$(CMN_SRC)/voOMXOSFun.cpp \
		$(CMN_SRC)/voOMXPortAudioConnect.cpp \
		$(CMN_SRC)/voOMXPortVideoConnect.cpp \
		$(CMN_SRC)/voOMXThread.cpp \
		$(DTS_CORE)/voCOMXCoreDTSDec.cpp \
		$(DTS_CORE)/voOMXCoreDTS.cpp \
		$(BASE_SRC)/voComponentEntry.cpp \
		$(BASE_SRC)/voCOMXBaseComponent.cpp \
		$(BASE_SRC)/voCOMXBasePort.cpp \
		$(BASE_SRC)/voCOMXCompFilter.cpp \
		$(CMN_SRC2)/CBaseConfig.cpp \
		$(CMN_SRC2)/cmnFile.cpp \
		$(CMN_SRC2)/cmnVOMemory.cpp \
		$(CMN_SRC2)/CvoBaseObject.cpp \
		$(CMN_SRC2)/voCMutex.cpp \
	        $(CMN_SRC2)/voOSFunc.cpp \
		$(DTS_SRC)/voCOMXAudioDTSDec.cpp \
		$(CMN_SRC2)/voLog.cpp \
		$(CMN_SRC2)/voHalInfo.cpp 



LOCAL_C_INCLUDES := \
                 ../../../../../../../Include \
		 ../../../../../../../Include/vome \
		 ../../../../../../../Common \
		 ../../../../../../Common \
                 ../../../../../Include \
	         ../../../../../Common \
		 ../../../../../Component/Base \
                 ../../../../../Component/AudioDTSDec \
                 ../../../../../Component/AudioDTSDec/Core \
		 ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1 \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/common/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/player/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/file_player/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/fileio/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/core/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/DRC/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/downmixer/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/frame_parser/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/frame_player/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/decoder_api/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/dial_norm/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/file_reader/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/file_writer/private/src \
				  ../../../../../../../Codec/Audio/DTS/dtshd_cdecoder1/pubheader


VOMM:=-D_ARM_VER -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -DENV_INT32 -DDTS_DECODER_NO_ASSERT -DDTS_DECODER_NO_DEBUG -fsigned-char 


# about info option, do not need to care it

LOCAL_CFLAGS := -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon -fsigned-char

LOCAL_LDLIBS:=../../../../../../../Codec/Audio/DTS/lib/lib_dtshd_dec.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

