
# please list all objects needed by your target here
OBJS:=bit.o dct32.o frame.o huffman.o layer3.o layer12.o mem_align.o mp3dec.o \
			polyphase.o rq_table.o stream.o
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../src \
					../../../../../../../Include \
					../../../../../../../Common

ifneq ($(VOPRJ), arkamys)
OBJS += cmnMemory.o
endif

	
