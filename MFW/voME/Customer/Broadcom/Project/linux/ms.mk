
# please list all objects needed by your target here
OBJS:=	voCOMXCoreOne.o \
		voOMXCore.o \
		voCOMXAudioDec.o \
		voCOMXVideoDec.o \
		voCOMXVideoSink.o \
		voCOMXVideoEnc.o \
		voCOMXAudioEnc.o

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
          ../../../../../Include \
	      ../../../../../Common \
		  ../../../../../Component/Base \
		  ../../../Source

VOSTCLIBS:=../../../../../../../../voRelease/Customer/htc/g1/static/voOMXLib.a

# please modify here to be sure to see the doit.mk
include ../../../../../../../build/doit.mk

