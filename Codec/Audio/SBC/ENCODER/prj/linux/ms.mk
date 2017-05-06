# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../../../../../Common \
	  ../../../inc \
	  ../../../src 

# please list all objects needed by your target here
OBJS:= encode.o sbccommon.o voSBCEnc.o vosbcmemory.o cmnMemory.o
			

ifeq ($(VOTT), v4)
OBJS+=
VOSRCDIR+=
endif				

ifeq ($(VOTT), v5)
OBJS+=
VOSRCDIR+=
endif

ifeq ($(VOTT), v6)
OBJS+=
VOSRCDIR+=
endif      

ifeq ($(VOTT), v7)
OBJS+=
VOSRCDIR+= 
endif			
