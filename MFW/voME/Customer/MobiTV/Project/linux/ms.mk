
# please list all objects needed by your target here
OBJS:=	CFileSource.o \
		CRTSPSource.o \
		voOMXMobiTV.o \
		voCOMXCoreMobiTV.o \
		voCOMXRTSPSource.o \
		voCOMXRTSPOutputPort.o \
		voCOMXRTSPAudioPort.o \
		voCOMXRTSPVideoPort.o

# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../Include/OMX \
          ../../../Include/Base \
          ../../../Include/VOSDK \
          ../../../Source

VOSTCLIBS:=../../../../../../../../voRelease/Customer/Google/cupcake/so/v6/voOMXBase.so

# please modify here to be sure to see the doit.mk
include ../../../../../../../build/doit.mk

