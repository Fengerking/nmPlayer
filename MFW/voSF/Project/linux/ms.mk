# please list all objects needed by your module here
OBJS:=CBaseNode.o \
		CAudioDecoder.o \
		CVideoDecoder.o \
		CBaseSource.o \
		CFileSource.o \
		CRTSPSource.o \
		CHTTPPDSource.o \
		CDllLoad.o \
		CFileFormatCheck.o \
		cmnFile.o \
		cmnVOMemory.o \
		CvoBaseObject.o \
		CBaseConfig.o \
		voCMutex.o \
		voOSFunc.o \
		voSF.o \
		CDataSourceOP.o \
		CBaseMediaExtractor.o \
		CBaseMediaSource.o \
		CFileMediaExtractor.o \
		CFileMediaSource.o \
		CMediaDecoderSource.o \
		CVideoEncoder.o \
		CAudioEncoder.o \
		CMediaEncoderSource.o \
		CBaseLiveSource.o \
		CLiveSource.o \
		CDataBuffer.o \
		CFileSink.o \
		CBaseMediaSink.o \
		CFileMediaSink.o \
		CMediaBufferList.o \
		voThread.o \
		ParseSPS.o \
		cConstructCodecInfo.o \
		fVideoHeadDataInfo.o \
		voHalInfo.o \
		voLog.o

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Include/vome \
		  ../../../../../../Common \
		  ../../../../../Common \
		  ../../../../Source \
		  ../../../../../../Source/file/Common
