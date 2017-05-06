# please list all objects needed by your module here
OBJS:=  cmnMemory.o \
		voOSFunc.o \
		voLog.o \
		CvoBaseObject.o \
		voCMutex.o \
		voAudioRender.o \
		CBaseAudioRender.o \
		CAudioTrackRender.o \

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	../../../../../Include \
			../../../../../Common \
			../../../Source \
			../../../Source/Android

