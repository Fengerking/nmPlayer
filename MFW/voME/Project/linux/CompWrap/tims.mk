# please list all objects needed by your target here
OBJS:=voCOMXThreadMutex.o \
	  voOMXMemroy.o \
	  voOMXOSFun.o \
	  voCBaseThread.o \
	  voCMutex.o \
	  CvoBaseObject.o \
	  voCSemaphore.o \
	  voOSFunc.o \
	  voThread.o \
	  CBaseComp.o \
	  CBaseCompWrap.o \
	  CBasePort.o \
	  OMXCompWrap.o \
	  fVideoHeadDataInfo.o \
	  CTIPort.o \
	  CTIComp.o \
	  voLog.o

#CBaseSource.o 
#CFileSource.o 
# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	   ../../../../../../../Include/vome \
	   ../../../../../../../Common \
       ../../../../../../Common \
       ../../../../../CompWrap/ics \
	   ../../../../../Common

