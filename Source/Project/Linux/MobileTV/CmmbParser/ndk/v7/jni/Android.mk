LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoCMMBParser

CMNSRC_PATH  := ../../../../../../../../Common
CMNHEAD_PATH := ../../../../../../../../Include
GENERAL_PATH := ../../../../../../../MTV/CMMB/General
CMMB_PATH    := ../../../../../../../MTV/CMMB
FILECMN_PATH := ../../../../../../../File/Common
MTVCMN_PATH  := ../../../../../../../MTV/Common

LOCAL_SRC_FILES := \
    $(GENERAL_PATH)/CDemultiplexer.cpp \
    $(GENERAL_PATH)/CDemultiplexerTp.cpp \
    $(GENERAL_PATH)/CEsgParseMng.cpp \
    $(GENERAL_PATH)/dmxbasicapi.cpp \
    $(GENERAL_PATH)/CEsgContentInfoParser.cpp \
    $(GENERAL_PATH)/CEsgScheduleInfoParser.cpp \
    $(GENERAL_PATH)/CEsgServiceAuxInfoParser.cpp \
    $(GENERAL_PATH)/CEsgServiceInfoParser.cpp \
    $(GENERAL_PATH)/CEsgServiceParamParser.cpp \
    $(GENERAL_PATH)/CEsgXmlBaseParser.cpp \
    $(CMMB_PATH)/XML/xmlsp.cpp \
    $(CMMB_PATH)/XML/xmlsp_dom.cpp \
    $(CMMB_PATH)/CCmmbParser.cpp \
    $(CMMB_PATH)/CCmmbFileDataParser.cpp \
    $(CMMB_PATH)/CCmmbReader.cpp \
    $(CMMB_PATH)/CCmmbTrack.cpp \
    $(CMMB_PATH)/General/CEsgParser.cpp \
    $(CMMB_PATH)/TinyXML/tinystr.cpp \
    $(CMMB_PATH)/TinyXML/tinyxml.cpp \
    $(CMMB_PATH)/TinyXML/tinyxmlerror.cpp \
    $(CMMB_PATH)/TinyXML/tinyxmlparser.cpp \
    $(FILECMN_PATH)/CBaseReader.cpp \
    $(FILECMN_PATH)/CBaseStreamFileIndex.cpp \
    $(FILECMN_PATH)/CBaseStreamFileReader.cpp \
    $(FILECMN_PATH)/CBaseStreamFileTrack.cpp \
    $(FILECMN_PATH)/CBaseTrack.cpp \
    $(FILECMN_PATH)/CGBuffer.cpp \
    $(FILECMN_PATH)/CGFileChunk.cpp \
    $(FILECMN_PATH)/CMemPool.cpp \
    $(FILECMN_PATH)/CvoBaseDrmCallback.cpp \
    $(FILECMN_PATH)/CvoBaseFileOpr.cpp \
    $(FILECMN_PATH)/CvoBaseMemOpr.cpp \
    $(FILECMN_PATH)/CvoFileParser.cpp \
    $(FILECMN_PATH)/CvoStreamFileDataParser.cpp \
    $(MTVCMN_PATH)/adtshead.cpp \
    $(MTVCMN_PATH)/adtspacker.cpp \
    $(MTVCMN_PATH)/adtsparser.cpp \
    $(MTVCMN_PATH)/CAnalyseAAC.cpp \
    $(MTVCMN_PATH)/CAnalyseH264.cpp \
    $(MTVCMN_PATH)/CAnalyseMPEG4.cpp \
    $(MTVCMN_PATH)/CBaseAnalyseData.cpp \
    $(MTVCMN_PATH)/CBaseParser.cpp \
    $(MTVCMN_PATH)/CBaseTools.cpp \
    $(MTVCMN_PATH)/CDumper.cpp \
    $(MTVCMN_PATH)/CMTVReader.cpp \
    $(MTVCMN_PATH)/CMTVStreamFileIndex.cpp \
    $(MTVCMN_PATH)/LatmParser.cpp \
    $(MTVCMN_PATH)/voMTVParser.cpp \
    $(MTVCMN_PATH)/voMTVReader.cpp \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/CPtrList.cpp \
    $(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
    $(CMNSRC_PATH)/voCMutex.cpp \
    $(CMNSRC_PATH)/voLog.cpp \
    $(CMNSRC_PATH)/voOSFunc.cpp \
    $(CMNSRC_PATH)/cmnMemory.c \
    ../../../../../../../../Source/File/Common/fCodec.c \
    $(CMMB_PATH)/ZLIB/adler32.c \
    $(CMMB_PATH)/ZLIB/crc32.c \
    $(CMMB_PATH)/ZLIB/infblock.c \
    $(CMMB_PATH)/ZLIB/infcodes.c \
    $(CMMB_PATH)/ZLIB/inffast.c \
    $(CMMB_PATH)/ZLIB/inflate.c \
    $(CMMB_PATH)/ZLIB/inftrees.c \
    $(CMMB_PATH)/ZLIB/infutil.c \
    $(CMMB_PATH)/ZLIB/zutil.c  
	
LOCAL_C_INCLUDES := \
  $(CMMB_PATH)/ZLIB \
  $(CMNHEAD_PATH) \
  $(CMNSRC_PATH) \
  $(MTVCMN_PATH) \
  $(CMMB_PATH) \
  $(GENERAL_PATH) \
  $(CMMB_PATH)/TinyXML \
  $(CMMB_PATH)/XML \
  $(FILECMN_PATH)



VOMM:=-D__VO_NDK__ -D_LINUX_ANDROID -DLINUX -D_LINUX -D__arm -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -DMTV_CMMB

LOCAL_CFLAGS := -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_CPPFLAGS := $(VOABOUT) $(VOMM) -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp

LOCAL_LDLIBS := -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)
