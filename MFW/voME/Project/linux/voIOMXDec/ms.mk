# please list all objects needed by your target here
OBJS:= voLog.o \
		voCOMXBaseObject.o \
		voCOMXTaskQueue.o \
		voCOMXThreadMutex.o \
		voCOMXThreadSemaphore.o \
		voOMXThread.o \
		voOMXOSFun.o \
		voOMXMemroy.o \
		voIOMXDec.o \
		CvoIOMXComponent.o \
		CvoIOMXPort.o \
		CvoIOMXInPort.o \
		CvoIOMXOutPort.o 

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
			../../../../../../Common \
			../../../../../../Include/vome \
			../../../../IOMXDec \
	   	../../../../Common 
