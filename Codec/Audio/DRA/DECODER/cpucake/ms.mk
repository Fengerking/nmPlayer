
# please list all objects needed by your target here
OBJS:=bit.o decframe.o dradec.o frame.o frameTabs.o huffman.o mem_align.o huffmanTab.o \
			cmnMemory.o mfilterband.o stream.o
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../src \
					../../../../../../Include \
					../../../../../../Common