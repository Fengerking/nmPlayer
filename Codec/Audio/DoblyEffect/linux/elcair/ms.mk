
# please list all objects needed by your target here
OBJS:=	voEFTDolby.o \
		cmnMemory.o \


# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
			  ../../../../../../Common \
				../../../


# please modify here to be sure to see the doit.mk
include ../../../../../../build/doit.mk

