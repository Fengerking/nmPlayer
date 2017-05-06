
# please list all objects needed by your target here
VOMSRC:= voEFTDolby.cpp cmnMemory.c


# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../../../../Include \
	../../../../../Common \
        ../../


# please modify here to be sure to see the idoit.mk
include ../../../../../build/idoit.mk

