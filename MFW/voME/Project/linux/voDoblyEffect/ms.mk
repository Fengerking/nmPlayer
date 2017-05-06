
# please list all objects needed by your target here
OBJS:=	voCOMXBaseConfig.o \
		voCOMXBaseObject.o \
		voCOMXTaskQueue.o \
		voCOMXThreadMutex.o \
		voCOMXThreadSemaphore.o \
		voOMXBase.o \
		voOMXFile.o \
		voOMXMemroy.o \
		voOMXOSFun.o \
		voOMXPortAudioConnect.o \
		voOMXPortVideoConnect.o \
		voOMXThread.o \
		voCOMXCoreDoblyEffect.o \
		voOMXCoreDAE.o \
		voComponentEntry.o \
		voCOMXBaseComponent.o \
		voCOMXPortInplace.o \
		voCOMXBasePort.o \
		voCOMXCompFilter.o \
		CBaseConfig.o \
		CBaseNode.o \
		CBaseSource.o \
		CDllLoad.o \
		cmnFile.o \
		cmnVOMemory.o \
		CvoBaseObject.o \
		voCMutex.o \
		voOSFunc.o \
		voCOMXAudioDoblyEffect.o \
		CAudioDEffect.o \
	        voHalInfo.o 	

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
					../../../../../../Include/vome \
		  ../../../../../../Common \
		  ../../../../../Common \
          ../../../../Include \
	      ../../../../Common \
		  ../../../../Component/Base \
          ../../../../Component/AudioDoblyEffect \
          ../../../../Component/AudioDoblyEffect/Core \
				  ../../../../../../Codec/Audio/DoblyEffect


# please modify here to be sure to see the doit.mk
include ../../../../../../build/doit.mk

