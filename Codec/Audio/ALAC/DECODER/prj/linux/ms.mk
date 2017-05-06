# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../../Include \
	  ../../../../inc \
          ../../../../src \
	  ../../../../parser \
	  ../../../../sfile \
	  ../../../../../../../../Common 

# please list all objects needed by your target here
OBJS:= alac.o alacdec.o alacstrmdec.o headerparse.o vomemory.o cmnMemory.o
			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../../../sfile/linuxasm
endif				

ifeq ($(VOTT), v6)
OBJS+=
VOSRCDIR+= ../../../../sfile/linuxasm
endif				

ifeq ($(VOTT), v7)
OBJS+=
VOSRCDIR+= ../../../../sfile/linuxasm
endif			

