# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
	  ../../inc \
          ../../src \
	  ../../../../../../Common 

# please list all objects needed by your target here
OBJS:= common.o mem_align.o dct4_a.o encoder.o g722_enc_api.o \
       huff_tab.o sam2coef.o tables.o cmnMemory.o
			
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
