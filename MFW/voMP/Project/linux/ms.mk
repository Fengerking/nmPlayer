# please list all objects needed by your module here
OBJS:=CBaseNode.o \
		CBaseConfig.o \
		CAndroidUtilities.o \
		CDllLoad.o \
		CFileFormatCheck.o \
		cmnFile.o \
		cmnVOMemory.o \
		CvoBaseObject.o \
		voCBaseThread.o	\
		voCMutex.o \
		voCSemaphore.o \
		voOSFunc.o \
		voThread.o \
		voCDataSourceOP.o \
		CBaseAudioRender.o \
		CBaseVideoRender.o \
		CCCRRRFunc.o \
		CAudioDecoder.o \
		CAudioEffect.o \
		CVideoEffect.o \
		CVideoDecoder.o \
		CVideoEffect.o \
		CBaseSource.o \
		CFileSource.o \
		CFileSink.o \
		CRTSPSource.o \
		CHTTPPDSource.o \
		CBaseLiveSource.o \
		CVideoDecoder.o \
		voCBasePlayer.o \
		voCDataBuffer.o \
		voCDataSource.o \
		voCLiveDataSource.o \
		voCMediaPlayer.o \
		voCMediaPlayerInit.o \
		voCMediaPlayerMeta.o \
		voCMixDataSource.o \
		voCLivePlayer.o \
		voCEditPlayer.o \
		fVideoHeadDataInfo.o \
		voMP.o \
    voLog.o \
    voHalInfo.o \
    voCPlayerLogo.o
		
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Include/vome \
		  ../../../../../../Common \
		  ../../../../../../Render/Audio/Source \
		  ../../../../../../Render/Video/Render/Source \
		  ../../../../../../Thirdparty/ndk \
		  ../../../../../Common \
      ../../../../Source 
  		  
  		  
		 
