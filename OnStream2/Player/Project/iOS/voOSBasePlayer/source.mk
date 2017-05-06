

OSTREAMCMNSRC_PATH :=../../../../Source/Common
OSTREAMIOSSRC_PATH :=../../../../Source/iOS
INC_PATH           :=../../../../../../Include
CMN_PATH           :=../../../../../../Common

# please list all objects needed by your module here
VOMSRC:= $(OSTREAMIOSSRC_PATH)/voOnStreamMP.cpp \
    $(OSTREAMIOSSRC_PATH)/voOnStreamMP_ns.mm \
    $(OSTREAMIOSSRC_PATH)/voOSEngWrapper.cpp \
    $(OSTREAMIOSSRC_PATH)/voOSSourceWrapper.cpp \
    $(OSTREAMIOSSRC_PATH)/voOSMediaPlayer.mm \
    $(OSTREAMIOSSRC_PATH)/voCapXMLParser.mm \
    $(OSTREAMIOSSRC_PATH)/voOSLog.mm \
    \
    $(CMN_PATH)/CDllLoad.cpp \
    $(CMN_PATH)/CvoBaseObject.cpp \
    $(CMN_PATH)/voLog.c \
    $(CMN_PATH)/voOSFunc_iOS.mm \
    $(CMN_PATH)/cmnFile.cpp \
    $(CMN_PATH)/cmnVOMemory.cpp \
    $(CMN_PATH)/CBaseConfig.cpp \
    $(CMN_PATH)/voCheckUseTime.cpp \
    $(CMN_PATH)/iOS/voStreamDownload.mm \
    $(CMN_PATH)/iOS/voRunRequestOnMain.mm \
    $(CMN_PATH)/iOS/voNSRecursiveLock.mm \


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	$(OSTREAMCMNSRC_PATH) \
    $(OSTREAMIOSSRC_PATH) \
		$(INC_PATH) \
		$(CMN_PATH) \
		$(CMN_PATH)/iOS
		