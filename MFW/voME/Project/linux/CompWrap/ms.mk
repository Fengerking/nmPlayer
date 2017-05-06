# please list all objects needed by your target here
OBJS:= 	voCOMXBaseObject.o \
		voCOMXThreadMutex.o \
		voOMXMemroy.o \
		voOMXOSFun.o \
		voOMXFile.o \
		voCBaseThread.o \
		voCMutex.o \
		CvoBaseObject.o \
		voCSemaphore.o \
		voOSFunc.o \
		voThread.o \
	   	CBaseComp.o \
	   	CBaseCompWrap.o \
	   	CBasePort.o \
	   	COMXLoader.o \
	   	OMXCompWrap.o \
		CBaseNode.o \
		CBaseSource.o \
		CFileSource.o \
		CDllLoad.o \
		voHalInfo.o \
	        fVideoHeadDataInfo.o \
		CTIPort.o \
		voLog.o

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	   ../../../../../../../Include/vome \
	   ../../../../../../../Common \
           ../../../../../../Common \
           ../../../../../CompWrap \
	   ../../../../../Common

