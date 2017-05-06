

OSTREAMCMNSRC_PATH :=../../../Source/Common
OSTREAMIOSSRC_PATH :=../../../Source/iOS
OSTREAMLOADSRC_PATH :=../../../Source/iOS/LoadControl
INC_PATH           :=../../../../../Include
CMNSRC_PATH        :=../../../../../Common

# please list all objects needed by your module here
VOMSRC:= $(OSTREAMCMNSRC_PATH)/COSBaseSource.cpp \
    $(OSTREAMCMNSRC_PATH)/COSAdapStrSource.cpp \
    $(OSTREAMCMNSRC_PATH)/COSDASHSource.cpp \
    $(OSTREAMCMNSRC_PATH)/COSHLSSource.cpp \
    $(OSTREAMCMNSRC_PATH)/COSISSSource.cpp \
    $(OSTREAMCMNSRC_PATH)/COSLocalSource.cpp \
    $(OSTREAMCMNSRC_PATH)/COSPDMSSource.cpp \
    $(OSTREAMCMNSRC_PATH)/COSRTSPSource.cpp \
    $(OSTREAMCMNSRC_PATH)/COSCMMBSource.cpp \
    $(OSTREAMCMNSRC_PATH)/COSSourceBase.cpp \
    $(OSTREAMCMNSRC_PATH)/COSTimeText.cpp \
    $(OSTREAMCMNSRC_PATH)/voOnStreamSrc.cpp \
    $(OSTREAMCMNSRC_PATH)/COSBaseSubtitle.cpp \
    $(OSTREAMCMNSRC_PATH)/COSCloseCaption.cpp \
    $(OSTREAMCMNSRC_PATH)/COSDVBSubtitle.cpp \
    $(OSTREAMIOSSRC_PATH)/voOnStreamSource_ns.mm \
    \
    $(OSTREAMLOADSRC_PATH)/voASF.cpp \
    $(OSTREAMLOADSRC_PATH)/voASIDASH.cpp \
    $(OSTREAMLOADSRC_PATH)/voASISS.cpp \
    $(OSTREAMLOADSRC_PATH)/voAudioFile.cpp \
    $(OSTREAMLOADSRC_PATH)/voAVI.cpp \
    $(OSTREAMLOADSRC_PATH)/voDRM2.cpp \
    $(OSTREAMLOADSRC_PATH)/voFLV.cpp \
    $(OSTREAMLOADSRC_PATH)/voHLS2.cpp \
    $(OSTREAMLOADSRC_PATH)/voMKV.cpp \
    $(OSTREAMLOADSRC_PATH)/voMP4.cpp \
    $(OSTREAMLOADSRC_PATH)/voMpegFile.cpp \
    $(OSTREAMLOADSRC_PATH)/voOGGFile.cpp \
    $(OSTREAMLOADSRC_PATH)/voPD.cpp \
    $(OSTREAMLOADSRC_PATH)/voPreHTTP.cpp \
    $(OSTREAMLOADSRC_PATH)/voRealFile.cpp \
    $(OSTREAMLOADSRC_PATH)/voRTSP.cpp \
    $(OSTREAMLOADSRC_PATH)/voTS.cpp \
    $(OSTREAMLOADSRC_PATH)/voWMSP2.cpp \
    $(OSTREAMLOADSRC_PATH)/voAAC2.cpp \
    $(OSTREAMLOADSRC_PATH)/voAdaptiveStreamHLS.cpp \
    $(OSTREAMLOADSRC_PATH)/voAdaptiveStreamController.cpp \
    $(OSTREAMLOADSRC_PATH)/voSource2_IO.cpp \
    $(OSTREAMLOADSRC_PATH)/voAdaptiveStreamDASH.cpp \
    $(OSTREAMLOADSRC_PATH)/voParser.cpp \
    $(OSTREAMLOADSRC_PATH)/voParserMP4.cpp \
    $(OSTREAMLOADSRC_PATH)/voSubtitleParser.cpp \
    $(OSTREAMLOADSRC_PATH)/voSMTHParser.cpp \
    $(OSTREAMLOADSRC_PATH)/voAdaptiveStreamISS.cpp \
    $(OSTREAMLOADSRC_PATH)/voSSL.cpp \
    $(OSTREAMLOADSRC_PATH)/voDVBSubtitleParser.cpp \
    \
    $(CMNSRC_PATH)/cioFile.cpp \
    $(CMNSRC_PATH)/voLog.c \
    $(CMNSRC_PATH)/voOSFunc_iOS.mm \
    $(CMNSRC_PATH)/cmnVOMemory.cpp \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/CBaseConfig.cpp \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/CFileFormatCheck.cpp \
    $(CMNSRC_PATH)/voCBaseThread.cpp \
    $(CMNSRC_PATH)/voCMutex.cpp \
    $(CMNSRC_PATH)/voCSemaphore.cpp \
    $(CMNSRC_PATH)/voThread.cpp \
    $(CMNSRC_PATH)/voSubtitleFunc.cpp \
    $(CMNSRC_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	$(OSTREAMCMNSRC_PATH) \
    $(OSTREAMIOSSRC_PATH) \
    $(OSTREAMLOADSRC_PATH) \
		$(INC_PATH) \
        ../../../../../Source/Include \
		$(CMNSRC_PATH) \
        $(CMNSRC_PATH)/Buffer