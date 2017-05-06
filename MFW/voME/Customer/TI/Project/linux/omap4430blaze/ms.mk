
# please list all objects needed by your target here
OBJS:=	voCOMXCoreOne.o \
		voOMXCore.o \
		voCOMXTIVideoSink.o \
		voCOMXTIOverlayPort.o \
		voTIVideoRender.o \
		voCOMXBaseObject.o \
		voOMXOSFun.o \
		voCOMXThreadMutex.o \
		voOMXBase.o \
		voCOMXBaseComponent.o \
		voCOMXCompSink.o \
		voCOMXThreadSemaphore.o \
		voCOMXBasePort.o \
		voOMXFile.o \
		voCOMXTaskQueue.o \
		voOMXThread.o \
		voCOMXPortClock.o \
		voOMXPortAudioConnect.o \
		voOMXPortVideoConnect.o \
		voComponentEntry.o \
		voOMXMemroy.o \
		CIntQueue.o

#CPtrList.o

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../../Include \
					../../../../../../../../Include/vome \
					../../../../../../../../Common \
					../../../../../../Common \
					../../../../../../Component/Base \
					../../../../Source/omap4430blaze \
					../../../../Source/pub 


# please modify here to be sure to see the doit.mk
include ../../../../../../../../build/doit.mk

