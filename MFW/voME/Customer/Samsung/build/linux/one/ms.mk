
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
	voComponentEntry.o \
	voCOMXBaseComponent.o \
	voCOMXPortInplace.o \
	voCOMXBasePort.o \
	voCOMXCompFilter.o \
	voCMutex.o \
	voOSFunc.o \
	CvoBaseObject.o \
	SsbSipH264Decode.o \
	SsbSipLogMsg.o \
	SsbSipMpeg4Decode.o \
	SsbSipVC1Decode.o \
	SsbSipH264Encode.o \
	SsbSipMpeg4Encode.o \
	voComSamsVideoDec.o \
	voCOMXCoreOne4Sams.o \
	SamsVideoDec.o \
	SamsVideoH264Dec.o \
	SamsVideoMP4Dec.o \
	voOMXCore4Sams.o 

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../../Include \
	  ../../../../../../../../Common \
	  ../../../../../../../Common \
          ../../../../../../Include \
	  ../../../../../../Common \
	  ../../../../../../Component/Base \
	  ../../../../../../Customer/Samsung/core \
	  ../../../../../../Customer/Samsung/comp/src \
	  ../../../../../../Customer/Samsung/comp/s3c_mfc/dec/include \
	  ../../../../../../Customer/Samsung/comp/s3c_mfc/dec/src \
	  ../../../../../../Customer/Samsung/comp/s3c_mfc/enc/include \
	  ../../../../../../Customer/Samsung/comp/s3c_mfc/enc/src 
# please modify here to be sure to see the doit.mk
include ../../../../../../../../build/doit.mk

