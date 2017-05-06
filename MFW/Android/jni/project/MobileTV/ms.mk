
# please list all objects needed by your module here
OBJS:= mtvjni.o \
	vomelink.o \
	wdrm-dry.o \
	wlinker-dry.o \
	vomeplayer.o \
	CVOMEPlayer.o \
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
	voJAVAAudioRender 

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Common \
		  ../../../../../voME/Include \
		  ../../../../../voME/Common \
			../../../../eclair/Source \
			../../../source \
			../../../source/njarender \
			../../../source/bluestreak


