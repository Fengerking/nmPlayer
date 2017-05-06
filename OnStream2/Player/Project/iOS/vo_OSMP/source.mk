

OSTREAMCMNSRC_PATH :=../../../../Source/Common
OSTREAMIOSSRC_PATH :=../../../../Source/iOS
OSTREAMIOS_V3_PATH :=../../../../Source/iOS/OSMP_V3
INC_PATH           :=../../../../../../Include
CMN_PATH           :=../../../../../../Common

# please list all objects needed by your module here
VOMSRC:= $(OSTREAMIOSSRC_PATH)/voOSEngWrapper.cpp \
    $(OSTREAMIOSSRC_PATH)/voOSSourceWrapper.cpp \
    $(OSTREAMIOSSRC_PATH)/voOSMediaPlayer.mm \
    $(OSTREAMIOSSRC_PATH)/voCapXMLParser.mm \
    $(OSTREAMIOSSRC_PATH)/voOSLog.mm \
    $(OSTREAMIOSSRC_PATH)/voOnStreamMP.cpp \
    $(OSTREAMIOSSRC_PATH)/voOSEvent.mm \
    \
    $(CMN_PATH)/CDllLoad.cpp \
    $(CMN_PATH)/CvoBaseObject.cpp \
    $(CMN_PATH)/voLog.c \
    $(CMN_PATH)/voOSFunc_iOS.mm \
    $(CMN_PATH)/cmnFile.cpp \
    $(CMN_PATH)/cmnVOMemory.cpp \
    $(CMN_PATH)/CBaseConfig.cpp \
    $(CMN_PATH)/voHalInfo.cpp \
    $(CMN_PATH)/voCheckUseTime.cpp \
    $(CMN_PATH)/iOS/voNSRecursiveLock.mm \
    $(CMN_PATH)/iOS/voStreamDownload.mm \
    $(CMN_PATH)/iOS/voRunRequestOnMain.mm \
    \
    $(OSTREAMIOS_V3_PATH)/VOOSMPAnalyticsFilter.m \
    $(OSTREAMIOS_V3_PATH)/VOOSMPAnalyticsInfoImpl.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPChunkInfoImpl.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPPCMBufferImpl.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPAssetInfo.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPInitParam.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPOpenParam.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPRTSPStatisticsImpl.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPAssetInfo.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPVerificationInfo.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPRTSPPort.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPHTTPProxy.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPHTTPDownloadFailureImpl.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPSEIClockTimeStampImpl.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPSEIPicTimingImpl.mm \
    $(OSTREAMIOS_V3_PATH)/VOOSMPImageDataImpl.mm \
    $(OSTREAMIOS_V3_PATH)/VOCommonPlayerImpl.mm \
    $(OSTREAMIOS_V3_PATH)/VOCommonPlayerImplement.mm \
    $(OSTREAMIOS_V3_PATH)/VOCommonPlayerHDMIImpl.mm

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	$(OSTREAMIOS_V3_PATH) \
    $(OSTREAMCMNSRC_PATH) \
    $(OSTREAMIOSSRC_PATH) \
		$(INC_PATH) \
		$(INC_PATH)/OSMP_V3/iOS \
		$(CMN_PATH) \
		$(CMN_PATH)/iOS