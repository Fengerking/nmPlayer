# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=


# please specify the name of your module
VOTARGET:=voOMXAR


# please modify here to be sure to see the g1.mk
include ../../../../../../build/g1.mk 


# please list all objects needed by your target here
OBJS:= voComponentEntry.o voCOMXBaseComponent.o  voCOMXBasePort.o voCOMXCompSink.o voCOMXPortClock.o voOMXFile.o \
       voCOMXBaseObject.o voCOMXTaskQueue.o    voCOMXThreadMutex.o voCOMXThreadSemaphore.o voOSFunc.o \
       voOMXBase.o voOMXMemroy.o voOMXOSFun.o voOMXPortAudioConnect.o voOMXPortVideoConnect.o voCMutex.o \
       voOMXThread.o voAudioSink.o voCOMXAudioSink.o CAudioRender.o CBaseNode.o CDllLoad.o cmnMemory.o CvoBaseObject.o \
       CBaseConfig.o cmnFile.o
       
# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Common \
		  ../../../../../Common \
          ../../../../Component/AudioSink \
	      ../../../../Component/Base \
          ../../../../Include \
	      ../../../../Common 

# please specify where is the voRelease on your PC, relative path is suggested
ifeq ($(VOTT), v6)
  VORELDIR:=../../../../../../../voRelease/Customer/htc/g1
else
  VORELDIR:=../../../../../../../voRelease
endif



# please modify here to be sure to see the doit.mk
include ../../../../../../build/doit.mk
