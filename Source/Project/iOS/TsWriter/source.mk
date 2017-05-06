# Source files for Ts Wtiter.
# Dolby Du
# 2012.09.10


# path variables for CAPTION file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
MTV_COMMON_PATH:=$(SOURCE_PATH)/MTV/Common
MTV_TS_PATH:=$(SOURCE_PATH)/MTV/TS
MTV_TS_WRITER_PATH:=$(SOURCE_PATH)/MTV/TS/Writer


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
		$(TRUNK_INCLUDE_PATH) \
		$(TRUNK_COMMON_PATH) \
		$(FILE_COMMON_PATH) \
		$(MTV_COMMON_PATH) \
		$(MTV_TS_PATH) \
		$(MTV_TS_WRITER_PATH) \


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	\
	$(FILE_COMMON_PATH)/CBaseAssembler.cpp \
	$(FILE_COMMON_PATH)/CBaseDumper.cpp \
	$(FILE_COMMON_PATH)/CBaseWriter.cpp \
	$(FILE_COMMON_PATH)/CMemPool.cpp \
	$(FILE_COMMON_PATH)/CvoBaseFileOpr.cpp \
	$(FILE_COMMON_PATH)/CvoBaseMemOpr.cpp \
	\
	$(MTV_COMMON_PATH)/CDumper.cpp \
	$(MTV_COMMON_PATH)/adtshead.cpp \
	$(MTV_COMMON_PATH)/CAnalyseAAC.cpp \
	$(MTV_COMMON_PATH)/CAnalyseMPEG4.cpp \
	$(MTV_COMMON_PATH)/CAnalysePCM.cpp \
	$(MTV_COMMON_PATH)/CAnalyseH264.cpp \
	$(MTV_COMMON_PATH)/CBaseAnalyseData.cpp \
	$(MTV_COMMON_PATH)/voBscApi.cpp \
	$(MTV_COMMON_PATH)/LatmParser.cpp \
	\
	$(MTV_TS_WRITER_PATH)/CBasePSI.cpp \
	$(MTV_TS_WRITER_PATH)/CCacheBuffer.cpp \
	$(MTV_TS_WRITER_PATH)/CTsAssembler.cpp \
	$(MTV_TS_WRITER_PATH)/CTsPacket.cpp \
	$(MTV_TS_WRITER_PATH)/CTsWriter.cpp \
	$(MTV_TS_WRITER_PATH)/bstype.cpp \
	$(MTV_TS_WRITER_PATH)/voTsWriter.cpp
