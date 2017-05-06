

INC_PATH           :=../../../../../Include
INC_PATH2          :=../../../../../Include/vome
CMNSRC_PATH        :=../../../../../Common
SUBTITLE_SRC_PATH        :=../../../../Subtitle
CMNFILE_PATH        :=../../../../File/Common

# please list all objects needed by your module here
VOMSRC:= $(SUBTITLE_SRC_PATH)/CBaseSubtitleParser.cpp \
    $(SUBTITLE_SRC_PATH)/CBaseSubtitleTrack.cpp \
    $(SUBTITLE_SRC_PATH)/CSMIParser.cpp \
    $(SUBTITLE_SRC_PATH)/CSrtParser.cpp \
    $(SUBTITLE_SRC_PATH)/CSubtitleMgr.cpp \
    $(SUBTITLE_SRC_PATH)/CTextTrack.cpp \
    $(SUBTITLE_SRC_PATH)/subtitleAPI.cpp \
    $(SUBTITLE_SRC_PATH)/CCharsetDecoder.cpp \
    $(SUBTITLE_SRC_PATH)/CFileFormatConverter.cpp \
    $(SUBTITLE_SRC_PATH)/CSubtitleHtmlMgr.cpp \
    $(SUBTITLE_SRC_PATH)/voOMXFile.cpp \
    $(SUBTITLE_SRC_PATH)/CWebVTTParser.cpp \
    $(SUBTITLE_SRC_PATH)/CSMPTETTMLParser.cpp \
    $(SUBTITLE_SRC_PATH)/voFormatConversion.mm \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/voLog.c \
    $(CMNSRC_PATH)/voCMutex.cpp \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/CPtrList.cpp \
    $(SUBTITLE_SRC_PATH)/CTimedTextMLParser.cpp \
    $(CMNSRC_PATH)/voOSFunc_iOS.mm \
    $(CMNFILE_PATH)/base64.cpp \



# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	$(INC_PATH) \
		$(INC_PATH2) \
		$(CMNSRC_PATH) \
		$(SUBTITLE_SRC_PATH) \
		$(CMNFILE_PATH) \
