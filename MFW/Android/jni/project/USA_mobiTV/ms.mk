
# please list all objects needed by your module here
VERBOSE:=@ 
OBJS:= mobiTVjni.o \
	voAudioRender.o \
	voJAVAAudioRender.o \
	VOJAVAVideoRender.o \
	VOVideoRender.o \
	voBaseAudioRender.o \
	voOMXThread.o \
	voOMXOSFun.o \
	voCOMXThreadMutex.o voCOMXTaskQueue.o voCOMXThreadMutex.o voCOMXThreadSemaphore.o voCOMXBaseObject.o voOMXMemroy.o voOMXFile.o \
	cmnFile.o \
	voCMutex.o \
	CvoBaseObject.o \
	voOSFunc.o \
	voCCRR.o \
	COMXEngine.o \
	CVOMEPlayer2.o \
	COMXMobiTVDataSourcePlayer.o \
	vomeplayer.o \
	voCOMXBaseConfig.o \
	TestDumpFile.o \
	CPtrList.o

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
##../../../source/njarender \
#../../../source/njvrender \

VOSRCDIR:=../../../source/USA_mobiTV/vomeRender \
				../../../../../../Include \
				../../../../../../Include/vome \
		   ../../../../../../Common \
		  ../../../../../voME/Common \
			../../../source \
			../../../source/bitmap \
			../../../source/USA_mobiTV \
			../../../source/USA_mobiTV/include \
			../../../source/USA_mobiTV/include/VOME_OMX


