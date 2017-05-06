
# please list all objects needed by your target here
VOMSRC:=voxPlayerApis.c voRingBuffer.c voxPlayerStrech.c mem_align.c cmnMemory.c
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../src ../../../inc \
	   ../../../../../../Include \
	   ../../../../../../Common			


ifeq ($(VOTT), v7)
VOMSRC+=  
VOSRCDIR+= 
endif			
