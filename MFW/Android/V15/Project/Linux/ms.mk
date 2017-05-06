
# please list all objects needed by your module here
OBJS:=	voaEngine.o \
		CVOMEPlayer.o \
		CVOWPlayer.o \
		CVOMEMetadata.o \
		CVOMEMetadataRetriever.o \
		voOMXThread.o \
		voOMXOSFun.o \
		voCOMXThreadMutex.o \
		cmnFile.o \
		voCMutex.o \
		CvoBaseObject.o

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Include/vome \
		  ../../../../../../Common \
		  ../../../../../voME/Common \
		  ../../../Source


