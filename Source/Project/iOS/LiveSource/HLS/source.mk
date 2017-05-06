# Source files for HTTP Live Streaming file parser.
# rodney zhang
# 2012.03.21


# path variables for HTTP Live Streaming file parser.
TRUNK_PATH:=../../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common
TRUNK_COMMON_BUFFER_PATH:=$(TRUNK_COMMON_PATH)/Buffer
TRUNK_INCLUDE_VOME_PATH:=$(TRUNK_INCLUDE_PATH)/vome
TRUNK_COMMON_NETWORK_PATH:=$(TRUNK_COMMON_PATH)/NetWork
TRUNK_MFW_VOME_COMMON_PATH:=$(TRUNK_PATH)/MFW/voME/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
FILE_COMMON_UTILITY_PATH:=$(FILE_COMMON_PATH)/Utility
HLS_SOURCE_PATH:=$(SOURCE_PATH)/HTTPLiveStreaming
MTV_LS_COMMON_PATH:=$(SOURCE_PATH)/MTV/LiveSource/Common
IOS_HLS_SSL_INCLUDE_PATH:=$(SOURCE_PATH)/Project/iOS/voLiveSrcHLS/openssl/include


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		$(TRUNK_INCLUDE_PATH) \
		$(TRUNK_INCLUDE_VOME_PATH) \
    $(HLS_SOURCE_PATH) \
		$(TRUNK_COMMON_PATH) \
		$(TRUNK_COMMON_BUFFER_PATH) \
		$(TRUNK_COMMON_NETWORK_PATH) \
		$(TRUNK_MFW_VOME_COMMON_PATH) \
		$(FILE_COMMON_PATH) \
		$(FILE_COMMON_UTILITY_PATH) \
		$(MTV_LS_COMMON_PATH) \
		$(IOS_HLS_SSL_INCLUDE_PATH)



# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/CBaseConfig.cpp \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_NETWORK_PATH)/vo_socket.cpp \
	$(FILE_COMMON_PATH)/CvoBaseDrmCallback.cpp \
	$(FILE_COMMON_PATH)/fortest.cpp \
	$(FILE_COMMON_UTILITY_PATH)/voSourceBufferManager.cpp \
	$(FILE_COMMON_UTILITY_PATH)/voSourceDataBuffer.cpp \
	$(FILE_COMMON_UTILITY_PATH)/voSourceSubtitleDataBuffer.cpp \
	$(FILE_COMMON_UTILITY_PATH)/voSourceVideoDataBuffer.cpp \
	$(HLS_SOURCE_PATH)/voLiveSource.cpp \
	$(HLS_SOURCE_PATH)/bitrate_adaptation_buffer.cpp \
	$(HLS_SOURCE_PATH)/CCheckTsPacketSize.cpp \
	$(HLS_SOURCE_PATH)/CLiveSrc2HLS.cpp \
	$(HLS_SOURCE_PATH)/CLiveSrcHLS.cpp \
	$(HLS_SOURCE_PATH)/vo_stream.cpp \
	$(HLS_SOURCE_PATH)/vo_http_live_streaming.cpp \
	$(HLS_SOURCE_PATH)/vo_http_live_streaming_new.cpp \
	$(HLS_SOURCE_PATH)/vo_http_stream.cpp \
	$(HLS_SOURCE_PATH)/vo_https_stream.cpp \
	$(HLS_SOURCE_PATH)/vo_m3u_manager.cpp \
	$(HLS_SOURCE_PATH)/vo_m3u_reader.cpp \
	$(HLS_SOURCE_PATH)/vo_mem_stream.cpp \
	$(HLS_SOURCE_PATH)/vo_network_judgment.cpp \
	$(HLS_SOURCE_PATH)/vo_thread.cpp \
	$(HLS_SOURCE_PATH)/vo_webdownload_stream.cpp \
	$(HLS_SOURCE_PATH)/helix_date.cpp \
	$(HLS_SOURCE_PATH)/md5.cpp \
	$(HLS_SOURCE_PATH)/md5hl.cpp \
	$(HLS_SOURCE_PATH)/vostream_digest_auth.cpp \
	$(HLS_SOURCE_PATH)/CLiveSrcBase.cpp \
	$(HLS_SOURCE_PATH)/ChannelInfo.cpp \
  $(HLS_SOURCE_PATH)/CLiveParserBase.cpp \
  $(HLS_SOURCE_PATH)/ConfigFile.cpp \
  $(HLS_SOURCE_PATH)/voHLS2.cpp