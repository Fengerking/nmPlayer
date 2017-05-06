
# please list all objects needed by your module here
OBJS:= vomejni.o \
	vomeplayer.o \
	vommretriever.o \
	CVOMEPlayer.o \
	CVOWPlayer.o \
	CVOMEMetadata.o \
	CVOMEMetadataRetriever.o \
	voOMXThread.o \
	voOMXOSFun.o \
	voCOMXThreadMutex.o \
	cmnFile.o \
	voCMutex.o \
	CvoBaseObject.o \
	voOSFunc.o \
 	vonjar.o \
	voVideoRender.o \
	voCCRR.o 

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Common \
		  ../../../../../voME/Include \
		  ../../../../../voME/Common \
			../../../source \
			../../../source/base \
			../../../source/bitmap \
			../../../source/njvrender \
			../../../source/njarender \
			../../../source/sourceplayer \
			../../../../../../Include/vome


