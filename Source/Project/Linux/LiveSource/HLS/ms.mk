
# please list all objects needed by your module here
OBJS:=  voCMutex.o \
	voOSFunc.o \
	CDllLoad.o \
	voThread.o \
	CvoBaseObject.o \
	CCheckTsPacketSize.o \
	CBaseConfig.o \
	fortest.o \
	vo_socket.o \
	vo_http_stream.o \
	vo_https_stream.o \
	vo_m3u_manager.o \
	vo_m3u_reader.o \
	vo_stream.o \
	vo_mem_stream.o \
	vo_aes_engine.o \
	vo_http_live_streaming.o \
	vo_network_judgment.o \
	vo_webdownload_stream.o \
	vo_thread.o \
	bitrate_adaptation_buffer.o \
	CLiveSrcHLS.o \
	voLiveSource.o \
	CLiveSrcBase.o \
	ChannelInfo.o \
	CLiveParserBase.o \
	ConfigFile.o \
	cmnMemory.o \
	CvoBaseDrmCallback.o \
	cmnFile.o \
	voHalInfo.o \


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	../../../../../../../Include \
		../../../../../../../Include/vome \
		../../../../../../../Common \
		../../../../../../../Common/NetWork \
		../../../../../../../MFW/voME/Common \
		../../../../../../../Source/HTTPLiveStreaming \
		../../../../../../../Source/File/Common \
		../../../../../../../Source/MTV/LiveSource/Common \
		../../../../../../../../../trunk/Include \
		../../../../../../../Thirdparty/ndk \
		/opt/froyo/external/openssl/include
