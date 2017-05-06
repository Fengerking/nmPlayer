
# please list all objects needed by your target here
OBJS:=	voCOMXBaseConfig.o \
		voCOMXBaseObject.o \
		voCOMXTaskQueue.o \
		voCOMXThreadMutex.o \
		voCOMXThreadSemaphore.o \
		voOMXBase.o \
		voOMXFile.o \
		voOMXMemroy.o \
		voOMXOSFun.o \
		voOMXPortAudioConnect.o \
		voOMXPortVideoConnect.o \
		voOMXThread.o \
		voCOMXCoreOne.o \
		voOMXCore.o \
		voComponentEntry.o \
		voCOMXBaseComponent.o \
		voCOMXPortInplace.o \
		voCOMXBasePort.o \
		voCOMXCompFilter.o \
		voCOMXCompInplace.o \
		voCOMXCompSink.o \
		voCOMXCompSource.o \
		voCOMXPortClock.o \
		voCOMXPortSource.o \
		voCOMXAudioDec.o \
		voCOMXAudioEffect.o \
		voCOMXAudioSink.o \
		voCOMXFileAudioPort.o \
		voCOMXFileOutputPort.o \
		voCOMXFileSource.o \
		voCOMXFileVideoPort.o \
		voCOMXDataAudioPort.o \
		voCOMXDataOutputPort.o \
		voCOMXDataSource.o \
		voCOMXDataVideoPort.o \
		voCOMXDataBuffer.o \
		voCOMXVideoEffect.o \
		voCOMXVideoDec.o \
		voCOMXVideoSink.o \
		voCOMXVideoEnc.o \
		voCOMXAudioEnc.o \
		voCOMXFileSink.o \
		voCOMXClockTime.o \
		voCOMXG1AudioCapPort.o \
		voCOMXG1AudioCapture.o \
		voCOMXG1VideoCapPort.o \
		voCOMXG1VideoCapture.o \
		CAudioDecoder.o \
		CAudioRender.o \
		CBaseConfig.o \
		CBaseNode.o \
		CBaseSource.o \
		CBaseLiveSource.o \
		CBaseVideoRender.o \
		CDllLoad.o \
		CCCRRRFunc.o \
		CFileFormatCheck.o \
		CFileSource.o \
		CRTSPSource.o \
		CHTTPPDSource.o \
		cmnFile.o \
		cmnVOMemory.o \
		COutVideoRender.o \
		CVideoDecoder.o \
		CVideoRender.o \
		CAudioEncoder.o \
		CVideoEncoder.o \
		CFileSink.o \
		CvoBaseObject.o \
		voCMutex.o \
		voOSFunc.o \
		voLogoData.o \
	        measure.o \
		voCOMXCfgOne.o \
		voHalInfo.o \
		voLog.o

ifeq "$(findstring -D__VOTT_PC__,$(VOCFLAGS))" "-D__VOTT_PC__"
	OBJS+=CSDLDraw.o
endif

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Include/vome \
		  ../../../../../../Common \
		  ../../../../../Common \
	      ../../../../Common \
		  ../../../../Component/Base \
		  ../../../../Core \
          ../../../../Component/AudioCapture \
          ../../../../Component/AudioDec \
          ../../../../Component/AudioEffect \
          ../../../../Component/AudioEnc \
          ../../../../Component/AudioSink \
          ../../../../Component/FileSource \
          ../../../../Component/DataSource \
          ../../../../Component/VideoCapture \
          ../../../../Component/VideoDec \
          ../../../../Component/VideoEffect \
          ../../../../Component/VideoEnc \
          ../../../../Component/VideoSink \
          ../../../../Component/FileSink \
          ../../../../Component/ClockTime \
	  ../../../../../../Render/Video/Render/Source 


ifeq "$(findstring -D__VOTT_PC__,$(VOCFLAGS))" "-D__VOTT_PC__"
	VOSRCDIR +=../../../../../../Render/Video/Render/Source/linux/sdl
endif


# please modify here to be sure to see the doit.mk
include ../../../../../../build/doit.mk

