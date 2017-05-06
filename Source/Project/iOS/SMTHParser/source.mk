# Source files for smooth streaming file parser.
# rodney zhang
# 2012.03.22


# path variables for smooth streaming file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common
TRUNK_COMMON_NETWORK_PATH:=$(TRUNK_COMMON_PATH)/NetWork

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
IISSS_SOURCE_PATH:=$(SOURCE_PATH)/IISSmoothStreaming
SMTH_SOURCE_PATH:=$(SOURCE_PATH)/SMTHParser

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(TRUNK_COMMON_NETWORK_PATH) \
	$(FILE_COMMON_PATH) \
	$(IISSS_SOURCE_PATH) \
	$(SMTH_SOURCE_PATH) \
	$(SOURCE_PATH)/Common \
	$(SOURCE_PATH)/AdaptiveStreaming/DRM \
	$(SOURCE_PATH)/Include 

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(FILE_COMMON_PATH)/base64.cpp \
	$(FILE_COMMON_PATH)/strutil.cpp \
	$(FILE_COMMON_PATH)/CvoBaseDrmCallback.cpp \
	$(FILE_COMMON_PATH)/fortest.cpp \
	$(FILE_COMMON_PATH)/vo_thread.cpp \
	\
	$(IISSS_SOURCE_PATH)/vo_http_stream.cpp \
	$(IISSS_SOURCE_PATH)/vo_mem_stream.cpp \
	$(IISSS_SOURCE_PATH)/vo_stream.cpp \
	\
	$(SMTH_SOURCE_PATH)/CSMTHParser.cpp \
	$(SMTH_SOURCE_PATH)/CSMTHParserCtrl.cpp \
	$(SMTH_SOURCE_PATH)/voDrmCallback.cpp \
	$(SMTH_SOURCE_PATH)/voSmthParser.cpp \
	\
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
  $(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
  $(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/NetWork/vo_socket.cpp \
	\
	$(IISSS_SOURCE_PATH)/vo_http_stream.cpp\
	$(IISSS_SOURCE_PATH)/vo_mem_stream.cpp \
	$(IISSS_SOURCE_PATH)/vo_stream.cpp \
	\
	$(SOURCE_PATH)/Common/vo_thread.cpp \
	\

	
