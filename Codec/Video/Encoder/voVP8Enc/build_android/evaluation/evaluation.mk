# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../../Include \
		../../source \
		../../source/libmkv \
		../../test \
		../../test/complete_encoder


# please list all objects needed by your target here
OBJS:= vpxenc.o y4minput.o args.o tools_common.o EbmlWriter.o

			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../eclairv4
endif	

ifeq ($(VOTT), v5)
OBJS+= 
VOSRCDIR+= ../eclairv5
endif	

ifeq ($(VOTT), v6)
OBJS+= 
VOSRCDIR+= ../eclairv6
endif	

ifeq ($(VOTT), v7)
OBJS+= 
VOSRCDIR+= ../eclairv7
endif	
