
# please list all objects needed by your module here
OBJS:=	voaEngine.o \
		CVOMEPlayer.o \
		CVOWPlayer.o \
		CExtVOMEPlayer.o \
		CVOMEMetadata.o \
		CVOMEMetadataRetriever.o \
		voOMXThread.o \
		voOMXOSFun.o \
		voCOMXThreadMutex.o \
		cmnFile.o \
		voCMutex.o \
		CvoBaseObject.o \
		CDllLoad.o \
		CDrmEngine.o \
		voOSFunc.o \
		voThread.o \
		CCheckTsPacketSize.o \
		vo_http_stream.o \
		vo_m3u_manager.o \
		vo_m3u_reader.o \
		vo_stream.o \
		vo_mem_stream.o

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
		  ../../../../../../../Include/vome \
		  ../../../../../../../Common \
		  ../../../../../../../Source/HTTPLiveStreaming \
		  ../../../../../../voME/Common \
		  ../../../../Source \
		  ../../../../../../../Source/HTTPLiveStreaming



