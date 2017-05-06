
# please list all objects needed by your module here
OBJS:= mobiTVjni.o \
	voOMXThread.o \
	voOMXOSFun.o \
	voCOMXThreadMutex.o \
	cmnFile.o \
	voCMutex.o \
	CvoBaseObject.o \
	voOSFunc.o \
 	vonjar.o \
	voVideoRender.o \
	voCCRR.o \
	vomeRGB565VideoRender.o \
	vomeQCMVideoRender.o \
	COMXEngine.o \
	CVOMEPlayer2.o \
	vomeplayer.o \
	COMXMobiTVDataSourcePlayer.o \
	CRTSPSource.o 

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Common \
		  ../../../../../voME/Include \
		  ../../../../../voME/Common \
			../../../../eclair/Source \
			../../../source \
			../../../source/base \
			../../../source/njarender \
			../../../source/njvrender \
			../../../source/USA_mobiTV\
			../../../source/USA_mobiTV/voImpForTest


