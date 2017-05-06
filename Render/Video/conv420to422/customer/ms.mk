
# please list all objects needed by your module here

OBJS:=YUV420_YUV422.o YUVto422.o

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

ifeq ($(VOTT), v7)
VOSRCDIR:=../../../src/v7
endif

