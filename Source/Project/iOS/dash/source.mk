# Source files for DASH file parser.
# rodney zhang
# 2012.05.17


# path variables for DASH file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common
TRUNK_COMMON_BUFFER_PATH:=$(TRUNK_COMMON_PATH)/Buffer
TRUNK_COMMON_NETWORK_PATH:=$(TRUNK_COMMON_PATH)/NetWork
TRUNK_UTILITY_VOUTF8CONV_PATH:=$(TRUNK_PATH)/Utility/voutf8conv

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
SOURCE_FILE_ASController_PATH:=$(SOURCE_PATH)/ASController
DASH_SOURCE_PATH:=$(SOURCE_PATH)/DASH


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(TRUNK_COMMON_NETWORK_PATH) \
	$(TRUNK_UTILITY_VOUTF8CONV_PATH) \
	$(FILE_COMMON_PATH) \
	$(SOURCE_FILE_ASController_PATH) \
	$(TRUNK_COMMON_BUFFER_PATH) \
	$(DASH_SOURCE_PATH)


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(FILE_COMMON_PATH)/voASControllerAPI.cpp \
	$(FILE_COMMON_PATH)/vo_thread.cpp \
	$(FILE_COMMON_PATH)/tinystr.cpp \
	$(FILE_COMMON_PATH)/tinyxml.cpp \
	$(FILE_COMMON_PATH)/tinyxmlerror.cpp \
	$(FILE_COMMON_PATH)/tinyxmlparser.cpp \
	$(FILE_COMMON_PATH)/fortest.cpp \
	$(FILE_COMMON_PATH)/vo_thread.cpp \
	$(FILE_COMMON_PATH)/CBitrateMap.cpp \
	\
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/voThread.cpp \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/NetWork/vo_socket.cpp \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/CPtrList.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/fAudioHeadDataInfo.cpp \
	../../../../../../../Utility/voutf8conv/voutf8conv.c \
	$(TRUNK_COMMON_PATH)/Buffer/voSourceBufferManager.cpp \
	$(TRUNK_COMMON_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(TRUNK_COMMON_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(TRUNK_COMMON_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	\
	$(SOURCE_FILE_ASController_PATH)/CASController.cpp \
	\
	$(DASH_SOURCE_PATH)/vo_http_stream.cpp \
	$(DASH_SOURCE_PATH)/vo_mpd_manager.cpp \
	$(DASH_SOURCE_PATH)/vo_mpd_reader.cpp \
	$(DASH_SOURCE_PATH)/vo_mem_stream.cpp \
	$(DASH_SOURCE_PATH)/vo_mpd_streaming.cpp \
	$(DASH_SOURCE_PATH)/vo_network_judgment.cpp \
	$(DASH_SOURCE_PATH)/vo_stream.cpp \
