# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
	  ../../../inc \
          ../../../src \
	  ../../../../../../../Common 

# please list all objects needed by your target here
OBJS:= g711.o g711_enc_api.o cmnMemory.o mem_align.o
			
ifeq ($(VOTT), v4)
OBJS += 
VOSRCDIR += 
endif				

ifeq ($(VOTT), v6)
OBJS +=
VOSRCDIR += 
endif				

ifeq ($(VOTT), v7)
OBJS +=
VOSRCDIR +=
endif			
