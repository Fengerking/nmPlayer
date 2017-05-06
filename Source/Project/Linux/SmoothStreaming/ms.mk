
# please list all objects needed by your module here
OBJS:=  voCMutex.o \
	voOSFunc.o \
	CDllLoad.o \
	voThread.o \
	voXMLLoad.o \
	CvoBaseObject.o \
	fortest.o \
	vo_socket.o \
	vo_http_stream.o \
	vo_manifest_manager.o \
	vo_manifest_reader.o \
	vo_stream.o \
	vo_mem_stream.o \
	vo_smooth_streaming.o \
	CLiveSrcISS.o \
	voLiveSource.o \
	CLiveSrcBase.o \
	CLiveParserBase.o \
	ConfigFile.o \
	cmnMemory.o \
	cmnFile.o \
	ChannelInfo.o \
	voutf8conv.o \
	vo_network_judgment.o \
	vo_thread.o	\
	CPtrList.o \
	CCodeCC.o \
	fCodec.o \
	voHalInfo.o

#	tinystr.o \
#	tinyxml.o \
#	tinyxmlerror.o \
#	tinyxmlparser.o \


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	../../IISSmoothStreaming \
		../../../../../../Include \
		../../../../../../Include/vome \
		../../../../../../Common \
		../../../../../../Common/NetWork \
		../../../../../../MFW/voME/Common \
		../../../../../../Source/IISSmoothStreaming \
		../../../../../../Source/File/XML \
		../../../../../../Source/MTV/LiveSource/Common \
		../../../../../../Source/MTV/CMMB/TinyXML \
		../../../../../../Utility/voutf8conv \
		../../../../../../Source/File/Common


