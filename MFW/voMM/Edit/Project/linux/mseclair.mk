
# please list all objects needed by your module here
OBJS:=	CBasePlay.o \
		voMMPlay.o \
		CPlayFile.o \
		CPlayGraph.o \
		CBaseNode.o \
		CAudioDecoder.o \
		CVideoDecoder.o \
		CBaseSource.o \
		CFileSource.o \
		CRTSPSource.o \
		CHTTPPDSource.o \
		CAudioRender.o \
		CVideoRender.o \
		CDllLoad.o \
		CFileFormatCheck.o \
		cmnFile.o \
		cmnVOMemory.o \
		CvoBaseObject.o \
		voCMutex.o \
		voOSFunc.o \
		voThread.o \
		CDumpPlayFile.o \
		CBaseConfig.o \
		COutAudioRender.o \
		CBaseVideoRender.o \
		CCCRRRFunc.o \
		COutVideoRender.o

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
		  		../../../../../../../Common \
		  		../../../../../../Common \
		  		../../../../Source \
		  		../../../../../../../Render/Video/Render/Source


