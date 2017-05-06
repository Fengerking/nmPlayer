
# please list all objects needed by your module here
OBJS:=	voaEngine.o \
		CVOMEEngine.o \
		CVOMEPlayer.o \
		CDataSourcePlayer.o \
		CVOWPlayer.o \
		CVOWThumbnail.o \
		CVOMERecorder.o \
		CVOMEMetadata.o \
		CVOMEMetadataRetriever.o \
		CVOMEMetadataMediaFormat.o \
		voOMXFile.o \
		voOMXMemroy.o \
		voOMXThread.o \
		voOMXOSFun.o \
		voCOMXBaseConfig.o \
		voCOMXBaseObject.o \
		voCOMXThreadMutex.o \
		cmnFile.o \
		voCMutex.o \
		CvoBaseObject.o \
		CDllLoad.o \
		CDrmEngine.o \
		voOSFunc.o \
		CVideoResize.o \
		CAudioResample.o \
		CVOMETranscode.o \
		voHalInfo.o \
		voLog.o

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
		  ../../../../../../../Include/vome \
		  ../../../../../../../Common \
		  ../../../../../../voME/Common \
		  ../../../../Source \
		  ../../../../../../../Source/File/PreHTTP


