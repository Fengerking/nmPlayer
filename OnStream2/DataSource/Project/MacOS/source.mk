

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