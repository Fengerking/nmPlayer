	
# please list all objects needed by your target here
OBJS:=ChanDnUp.o commontables.o FirFilter.o RateDU.o Resample.o voMalloc.o voResample.o ReSampleBy2.o cmnMemory.o
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../src/ \
					../../../inc/ \
					../../../../../../Include \
					../../../../../../Common \
					../../../sfile/linuxasm 


ifeq ($(VOTT), v4)
OBJS+=voMemory.o
VOSRCDIR+= ../../../sfile/linuxasm 
endif				

ifeq ($(VOTT), v5)
OBJS+=voMemory.o
VOSRCDIR+= ../../../sfile/linuxasm 
endif

ifeq ($(VOTT), v6)
OBJS+=voMemory.o
VOSRCDIR+= ../../../sfile/linuxasm 
endif

ifeq ($(VOTT), v7)
OBJS+=voMemory.o
VOSRCDIR+= ../../../sfile/linuxasm 
endif
