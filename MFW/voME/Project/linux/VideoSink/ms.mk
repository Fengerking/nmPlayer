

# please list all objects needed by your target here
OBJS:= voComponentEntry.o voCOMXBaseComponent.o  voCOMXBasePort.o voCOMXCompSink.o voCOMXPortClock.o voOMXFile.o \
       voCOMXBaseObject.o voCOMXTaskQueue.o    voCOMXThreadMutex.o voCOMXThreadSemaphore.o  voOSFunc.o \
       voOMXBase.o voOMXMemroy.o voOMXOSFun.o voOMXPortAudioConnect.o voOMXPortVideoConnect.o voCMutex.o \
       voOMXThread.o voVideoSink.o voCOMXVideoSink.o CVideoRender.o CBaseNode.o CDllLoad.o cmnMemory.o CvoBaseObject.o \
       CBaseConfig.o cmnFile.o CBaseVideoRender.o COutVideoRender.o CCCRRRFunc.o cmnMemory.o voLogoData.o

ifeq ($(VOPRJ), htc.g1)
OBJS+=CG1VideoRender.o
endif

ifeq ($(VOPRJ), ti.zoom2)
OBJS+=ZM2VideoRender.o
endif

# please list all directories that all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Common \
		  ../../../../../Common \
          ../../../../Component/VideoSink \
	      ../../../../Component/Base \
          ../../../../Include \
	      ../../../../Common \
	      ../../../../../../Render/Video/Render/Source


# please modify here to be sure to see the doit.mk
include ../../../../../../build/doit.mk
