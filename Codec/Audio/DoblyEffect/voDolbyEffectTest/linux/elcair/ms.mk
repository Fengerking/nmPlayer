
# please list all objects needed by your target here
OBJS:=	voDolbyEffectTest.o \
		cmnMemory.o \


# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
			  ../../../../../../../Common \
				../../../voDolbyEffectTest/


# please modify here to be sure to see the doit.mk
include ../../../../../../../build/doit.mk

