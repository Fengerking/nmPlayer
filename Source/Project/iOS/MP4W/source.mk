
CMNSRC_PATH:= ../../../../../Common
CSRC_PATH:= ../../../../File/MP4/Writer
CSRCISO_PATH:= ../../../../File/MP4/ISOMedia
CSRMP4_PATH:= ../../../../File/MP4/MP4Base
CSOURCE_PATH:= ../../../../../Source

# please list all objects needed by your module here
VOMSRC:= \
$(CSRC_PATH)/CAnalyseAAC.cpp \
$(CSRC_PATH)/CAnalyseAMR.cpp \
$(CSRC_PATH)/CAnalyseH263.cpp \
$(CSRC_PATH)/CAnalyseH264.cpp \
$(CSRC_PATH)/CAnalyseMPEG4.cpp \
$(CSRC_PATH)/CBaseAnalyseData.cpp \
$(CSRC_PATH)/CAnalyseQCELP.cpp \
$(CSRC_PATH)/CMP4Writer.cpp \
$(CSRC_PATH)/CMp4WriterStream.cpp \
$(CSRC_PATH)/CReserveSpace2.cpp \
$(CSRC_PATH)/CTempFileStream.cpp \
$(CSRC_PATH)/voMP4Writer.cpp \
$(CSRC_PATH)/isomwr.cpp \
$(CSRC_PATH)/CTempBufStream.cpp \
$(CSRCISO_PATH)/isombox.cpp \
$(CSRCISO_PATH)/isomlogi.cpp \
$(CSRCISO_PATH)/isomscan.cpp \
$(CSRMP4_PATH)/mp4cfg.cpp \
$(CSRMP4_PATH)/mp4desc.cpp \
$(CSRMP4_PATH)/mpxio.cpp \
$(CSRMP4_PATH)/mpxobj.cpp \
$(CSRMP4_PATH)/mpxutil.cpp \
$(CMNSRC_PATH)/CvoBaseFileOpr.cpp \
$(CMNSRC_PATH)/CvoBaseObject.cpp \
$(CMNSRC_PATH)/cmnFile.cpp \
$(CMNSRC_PATH)/cmnMemory.cpp \
$(CMNSRC_PATH)/voCMutex.cpp \
$(CMNSRC_PATH)/voLog.c 


VOSRCDIR:=../../../../File/MP4/ISOMedia \
	../../../../File/MP4/MP4Base \
	../../../../File/MP4/Writer \
	../../../../File/Common \
	../../../../../Include \
	../../../../../Common




