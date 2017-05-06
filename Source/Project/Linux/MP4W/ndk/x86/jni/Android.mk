LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoMP4FW

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../../../../File/MP4
CSRCMN_PATH:=../../../../../../File/Common

LOCAL_SRC_FILES := \
				   $(CMNSRC_PATH)/cmnFile.cpp \
				   $(CMNSRC_PATH)/voLog.c \
				   $(CMNSRC_PATH)/cmnMemory.c \
				   $(CMNSRC_PATH)/CvoBaseObject.cpp \
				   $(CMNSRC_PATH)/voCMutex.cpp \
				   $(CSRC_PATH)/Writer/CAnalyseAAC.cpp \
				   $(CSRC_PATH)/Writer/CAnalyseAMR.cpp \
				   $(CSRC_PATH)/Writer/CAnalyseH263.cpp\
				   $(CSRC_PATH)/Writer/CAnalyseH264.cpp \
				   $(CSRC_PATH)/Writer/CAnalyseMPEG4.cpp \
				   $(CSRC_PATH)/Writer/CAnalyseQCELP.cpp \
				   $(CSRC_PATH)/Writer/CTempBufStream.cpp \
				   $(CSRC_PATH)/Writer/CBaseAnalyseData.cpp \
				   $(CSRC_PATH)/Writer/CMP4Writer.cpp \
				   $(CSRC_PATH)/Writer/CMp4WriterStream.cpp \
				   $(CSRC_PATH)/Writer/CReserveSpace2.cpp \
				   $(CSRC_PATH)/Writer/CTempFileStream.cpp \
				   $(CSRC_PATH)/Writer/voMP4Writer.cpp \
				   $(CSRC_PATH)/Writer/ParseSPS.cpp \
				   $(CSRC_PATH)/ISOMedia/isombox.cpp \
				   $(CSRC_PATH)/ISOMedia/isomlogi.cpp \
				   $(CSRC_PATH)/Writer/isomwr.cpp \
				   $(CSRC_PATH)/ISOMedia/isomscan.cpp \
				   $(CSRC_PATH)/MP4Base/mp4cfg.cpp \
				   $(CSRC_PATH)/MP4Base/mp4desc.cpp \
				   $(CSRC_PATH)/MP4Base/mpxio.cpp \
				   $(CSRC_PATH)/MP4Base/mpxobj.cpp \
				   $(CSRC_PATH)/MP4Base/mpxutil.cpp \
				   $(CSRCMN_PATH)/CvoBaseFileOpr.cpp 


LOCAL_C_INCLUDES := \
					../../../../../../../Include \
					$(CMNSRC_PATH) \
					$(CSRC_PATH) \
					$(CSRC_PATH)/ISOMedia \
					$(CSRC_PATH)/MP4Base \
					$(CSRC_PATH)/Writer \
					$(CSRCMN_PATH) \
					$(CMDSRC_PATH)



VOMM:= -D_USE_J3LIB -D__VO_NDK__ -DLINUX -D_LINUX -D_LINUX_ANDROID -D_Writer -D_VOLOG_FUNC -D_VOLOG_INFO -D_VOLOG_WARNING

# about info option, do not need to care it
LOCAL_CFLAGS := -D_VOMODULEID=0x05110000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

