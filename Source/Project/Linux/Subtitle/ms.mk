
# please list all objects needed by your module here
OBJS:=	\
	CBaseSubtitleParser.o \
	CBaseSubtitleTrack.o \
	CSMIParser.o \
	CSrtParser.o \
	CSubtitleMgr.o \
	CTextTrack.o \
	cmnFile.o \
	voCMutex.o \
	CvoBaseObject.o \
	voOMXFile.o \
	subtitleAPI.o \
	CCharsetDecoder.o \
	CFileFormatConverter.o \
	voOSFunc.o \
	CBaseSubtitleFile.o \
	CTimedTextMLParser.o \
	voLog.o\
	CPtrList.o\
	CSubtitleHtmlMgr.o


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
	../../../../../../Include/vome \
	../../../../../../Common \
	../../../../../../MFW/voME/Common \
	../../../../source/subtitle3 \

