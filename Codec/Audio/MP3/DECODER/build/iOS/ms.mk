
# please list all objects needed by your target here
VOMSRC:=bit.c dct32.c frame.c huffman.c layer3.c layer12.c mem_align.c mp3dec.c \
			polyphase.c rq_table.c stream.c
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../src \
					../../../../../../../Include \
					../../../../../../../Common

ifneq ($(VOPRJ), arkamys)
#VOMSRC += cmnMemory.c
endif

ifeq ($(VOTT), v6)
VOMSRC+= dct32_asm.S layer3_asm.S Synth_asm.S
VOSRCDIR+= ../../../src/ios/armv6
endif

ifeq ($(VOTT), v7)
VOMSRC+= dct32_asm.S layer3_asm.S Synth_asm.S
VOSRCDIR+= ../../../src/ios/armv7
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= dct32_asm.S layer3_asm.S Synth_asm.S
VOSRCDIR+= ../../../src/ios/armv7
endif			
