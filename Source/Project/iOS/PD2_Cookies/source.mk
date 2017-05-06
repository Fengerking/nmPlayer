# Source files for PD2 Cookies file parser.
# rodney zhang
# 2012.03.29


# path variables for PD2 Cookies file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common
TRUNK_COMMON_NETWORK_PATH:=$(TRUNK_PATH)/Common/NetWork

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_CMN_PATH:=$(SOURCE_PATH)/Common
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
PD2_COOKIES_PATH:=$(SOURCE_FILE_PATH)/PD2_Cookies
METADATA_SOURCE_PATH:=$(SOURCE_FILE_PATH)/MetaData

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(TRUNK_COMMON_NETWORK_PATH) \
	$(PD2_COOKIES_PATH) \
	$(FILE_COMMON_PATH) \
	$(METADATA_SOURCE_PATH) \
	$(SOURCE_CMN_PATH)	

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/voThread.cpp \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/voCSemaphore.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	\
 	$(FILE_COMMON_PATH)/CvoBaseMemOpr.cpp \
	$(FILE_COMMON_PATH)/CvoBaseFileOpr.cpp \
	$(FILE_COMMON_PATH)/voSourceBaseWrapper.cpp \
	\
	$(TRUNK_COMMON_NETWORK_PATH)/vo_socket.cpp \
	\
	$(PD2_COOKIES_PATH)/PD.cpp \
	$(PD2_COOKIES_PATH)/vo_buffer_manager.cpp \
	$(PD2_COOKIES_PATH)/vo_buffer_stream.cpp \
	$(PD2_COOKIES_PATH)/vo_download_manager.cpp \
	$(PD2_COOKIES_PATH)/vo_file_buffer.cpp \
	$(PD2_COOKIES_PATH)/vo_file_stream.cpp \
	$(PD2_COOKIES_PATH)/vo_headerdata_buffer.cpp \
	$(PD2_COOKIES_PATH)/vo_http_downloader.cpp \
	$(PD2_COOKIES_PATH)/vo_largefile_buffer.cpp \
	$(PD2_COOKIES_PATH)/vo_largefile_buffer_manager.cpp \
	$(PD2_COOKIES_PATH)/vo_mem_stream.cpp \
	$(PD2_COOKIES_PATH)/vo_playlist_parser.cpp \
	$(PD2_COOKIES_PATH)/vo_playlist_m3u_parser.cpp \
	$(PD2_COOKIES_PATH)/vo_playlist_pls_parser.cpp \
	$(PD2_COOKIES_PATH)/vo_smallfile_buffer_manager.cpp	\
	$(SOURCE_CMN_PATH)/vo_thread.cpp \
	$(PD2_COOKIES_PATH)/vo_PD_manager.cpp \
	$(PD2_COOKIES_PATH)/voSource2PDWrapper.cpp	\
	$(PD2_COOKIES_PATH)/vo_http_authentication.cpp	\
	$(PD2_COOKIES_PATH)/vo_http_md5.cpp	\
	$(PD2_COOKIES_PATH)/voDLNA_Param.cpp	\
	$(PD2_COOKIES_PATH)/COpenSSL.cpp	\
	$(PD2_COOKIES_PATH)/vo_http_cookie.cpp	\
	$(PD2_COOKIES_PATH)/vo_http_utils.cpp
