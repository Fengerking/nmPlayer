LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoTsFW

CMNSRC_PATH:=../../../../../../../Common
FILECMNSRC_PATH:=../../../../../../File/Common
MTVCMNSRC_PATH:=../../../../../../MTV/Common
CSRC_PATH:=../../../../../../MTV/TS/Writer
CSRCTS_PATH:=../../../../../../MTV/TS

LOCAL_SRC_FILES := \
    \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/voLog.c \
    $(CMNSRC_PATH)/cmnMemory.c \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/voCMutex.cpp \
    $(CMNSRC_PATH)/voOSFunc.cpp \
    $(FILECMNSRC_PATH)/CBaseAssembler.cpp \
    $(FILECMNSRC_PATH)/CBaseDumper.cpp \
    $(FILECMNSRC_PATH)/CBaseWriter.cpp \
    $(FILECMNSRC_PATH)/CMemPool.cpp \
    $(FILECMNSRC_PATH)/CvoBaseFileOpr.cpp \
    $(FILECMNSRC_PATH)/CvoBaseMemOpr.cpp \
    $(MTVCMNSRC_PATH)/adtshead.cpp \
    $(MTVCMNSRC_PATH)/CAnalyseAAC.cpp \
    $(MTVCMNSRC_PATH)/CAnalyseMPEG4.cpp \
    $(MTVCMNSRC_PATH)/CAnalysePCM.cpp \
    $(MTVCMNSRC_PATH)/CAnalyseH264.cpp \
    $(MTVCMNSRC_PATH)/CBaseAnalyseData.cpp \
    $(MTVCMNSRC_PATH)/CDumper.cpp \
    $(MTVCMNSRC_PATH)/voBscApi.cpp \
    $(CSRCTS_PATH)/LatmParser.cpp \
    $(CSRC_PATH)/bstype.cpp \
    $(CSRC_PATH)/CBasePSI.cpp \
    $(CSRC_PATH)/CTsAssembler.cpp \
    $(CSRC_PATH)/CTsPacket.cpp \
    $(CSRC_PATH)/CTsWriter.cpp \
    $(CSRC_PATH)/voTsWriter.cpp \
    $(CSRC_PATH)/CCacheBuffer.cpp
	
LOCAL_C_INCLUDES := \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(FILECMNSRC_PATH) \
	$(MTVCMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CSRCTS_PATH)



VOMM:= -DLINUX -D_LINUX_ANDROID -D_LINUX -D_VOLOG_ERROR -D__VO_NDK__ -D_SINK_TO_MEMORY_FILE

LOCAL_CFLAGS := -D_VOMODULEID=0x05140000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32
LOCAL_LDLIBS := ../../../../../../../Lib/ndk/x86/libvoVideoParser.a ../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
