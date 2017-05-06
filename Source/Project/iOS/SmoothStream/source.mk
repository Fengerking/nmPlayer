# Source files for smooth streaming.
# rodney zhang
# 2012.03.22


# path variables for smooth streaming.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common
TRUNK_COMMON_BUFFER_PATH:=$(TRUNK_COMMON_PATH)/Buffer
MFW_VOME_COMMON_PATH:=$(TRUNK_PATH)/MFW/voME/Common
TRUNK_UTILITY_UTF8_PATH:=$(TRUNK_PATH)/Utility/voutf8conv
TRUNK_COMMON_NETWORK_PATH:=$(TRUNK_COMMON_PATH)/NetWork
INCLUDE_VOME_PATH:=$(TRUNK_INCLUDE_PATH)/vome

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_ASCONTROLLER_PATH:=$(SOURCE_PATH)/ASController
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
FILE_XML_PATH:=$(SOURCE_FILE_PATH)/XML
LS_COMMON_PATH:=$(SOURCE_PATH)/MTV/LiveSource/Common
IISSS_SOURCE_PATH:=$(SOURCE_PATH)/IISSmoothStreaming

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(TRUNK_COMMON_BUFFER_PATH) \
	$(MFW_VOME_COMMON_PATH) \
	$(TRUNK_UTILITY_UTF8_PATH) \
	$(TRUNK_COMMON_NETWORK_PATH) \
	$(INCLUDE_VOME_PATH) \
	$(SOURCE_ASCONTROLLER_PATH) \
	$(FILE_COMMON_PATH) \
	$(FILE_XML_PATH) \
	$(LS_COMMON_PATH) \
	$(IISSS_SOURCE_PATH) \
	$(SOURCE_PATH)/Common \


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(FILE_COMMON_PATH)/voASControllerAPI.cpp \
	$(FILE_COMMON_PATH)/fortest.cpp \
	$(FILE_COMMON_PATH)/CCodeCC.cpp \
	$(FILE_COMMON_PATH)/fCodec.cpp \
	$(FILE_COMMON_PATH)/CBitrateMap.cpp \
	$(FILE_COMMON_PATH)/vo_thread.cpp \
	\
	$(SOURCE_ASCONTROLLER_PATH)/CASController.cpp \
	\
	$(IISSS_SOURCE_PATH)/vo_http_stream.cpp \
	$(IISSS_SOURCE_PATH)/vo_manifest_manager.cpp \
	$(IISSS_SOURCE_PATH)/vo_manifest_reader.cpp \
	$(IISSS_SOURCE_PATH)/vo_stream.cpp \
	$(IISSS_SOURCE_PATH)/vo_mem_stream.cpp \
	$(IISSS_SOURCE_PATH)/vo_smooth_streaming.cpp \
	$(IISSS_SOURCE_PATH)/vo_network_judgment.cpp \
	\
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/NetWork/vo_socket.cpp \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/voXMLLoad.cpp \
	$(TRUNK_COMMON_PATH)/CPtrList.cpp \
	$(TRUNK_COMMON_PATH)/Buffer/voSourceBufferManager.cpp \
	$(TRUNK_COMMON_PATH)/Buffer/voSourceDataBuffer.cpp \
	$(TRUNK_COMMON_PATH)/Buffer/voSourceSubtitleDataBuffer.cpp \
	$(TRUNK_COMMON_PATH)/Buffer/voSourceVideoDataBuffer.cpp \
	\
	$(SOURCE_PATH)/Common/voThread.cpp \
  \
	../../../../../../../Utility/voutf8conv/voutf8conv.c \
	


