
# please list all objects needed by your module here
OBJS:= bluejni.o \
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
	vomeRGB565VideoRender.o 
	

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Include/vome \
		  ../../../../../../Common \
		  ../../../../../voME/Include \
		  ../../../../../voME/Common \
			../../../source \
			../../../source/base \
			../../../source/njarender \
			../../../source/njvrender \
			../../../source/bluestreak


