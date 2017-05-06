
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

ifeq ($(VOTT), v4)
OBJS+= dct32_asm_v4.o layer3_asm_v4.o Synth_asm_v4.o
VOSRCDIR+= ../../../src/linuxasm/armv4
endif				

ifeq ($(VOTT), v6)
OBJS+= dct32_asm.o layer3_asm.o Synth_asm.o
VOSRCDIR+= ../../../src/linuxasm/armv6
endif				

ifeq ($(VOTT), v7)
OBJS+= dct32_asm.o layer3_asm.o Synth_asm.o
VOSRCDIR+= ../../../src/linuxasm/armv7
endif			
