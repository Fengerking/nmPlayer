
# please list all objects needed by your target here
OBJS:= 	voCOMXBaseConfig.o \
	voCOMXBaseObject.o \
	voCOMXTaskQueue.o \
	voCOMXThreadMutex.o \
	voCOMXThreadSemaphore.o \
       	voOMXBase.o \
       	voOMXMemroy.o \
       	voOMXOSFun.o \
       	voOMXFile.o \
       	voOMXPortAudioConnect.o \
       	voOMXPortVideoConnect.o \
       	voOMXThread.o \
       	voCOMXBaseBox.o \
       	voCOMXBaseGraph.o \
       	voCOMXBaseHole.o \
       	voCOMXCompBaseChain.o \
       	vomeAPI.o \
       	voCOMXCfgComp.o \
       	voCOMXCfgCore.o \
       	voCOMXCompList.o \
       	voCOMXCoreLoader.o \
	voLog.o

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
		  ../../../../../../Include/vome \
		../../../../../../../Common \
		  ../../../../../Common \
      	  ../../../../FrameWork\
		  ../../../../Component/Base \
		  ../../../../Common 


# please modify here to be sure to see the doit.mk
include ../../../../../../build/doit.mk
