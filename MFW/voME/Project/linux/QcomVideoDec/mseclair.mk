
# please list all objects needed by your target here
OBJS:= CAdsp.o \
		CQcomVideoDec.o \
		CvoBaseObject.o \
		pmem.o \
		voCMutex.o \
		voComponentEntry.o \
		voCOMXBaseComponent.o \
		voCOMXBaseObject.o \
		voCOMXBasePort.o \
		voCOMXCoreQcomVdec.o \
		voCOMXQcomVideoDec.o \
		voCOMXTaskQueue.o \
		voCOMXThreadMutex.o \
		voCOMXThreadSemaphore.o \
		voOMXBase.o \
		voOMXCoreQcomVdec.o \
		voOMXFile.o \
		voOMXMemroy.o \
		voOMXOSFun.o \
		voOMXPortAudioConnect.o \
		voOMXPortVideoConnect.o \
		voOMXThread.o

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
					../../../../../../../Include/vome \
	  	  	../../../../../../../Common \
	  	  	../../../../../Common \
          ../../../../../Component/Base \
          ../../../../../Component/QcomVideoDec \
          ../../../../../Component/QcomVideoDec/Core 

# please modify here to be sure to see the doit.mk
include ../../../../../../../build/doit.mk

