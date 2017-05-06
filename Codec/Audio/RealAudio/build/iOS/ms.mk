
# please list all objects needed by your target here
VOMSRC:=ra_decode.c voRABUFDec.c voRADecSDK.c voRARawDec.c voRARMDec.c ra8lbr_decode.c \
			bitpack.c buffers.c category.c couple.c envelope.c fft.c gainctrl.c gecko2codec.c \
			huffman.c hufftabs.c mlt.c sqvh.c trigtabs.c aac_decode.c aac_bitstream.c ga_config.c 

			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../src/interface \
	  ../../../include \
	  ../../../src/ra8lbr \
	  ../../../src/ra8lbr/decoder \
	  ../../../src/aac \
	  ../../../../../../Include \
	  ../../../../../../Common


ifeq ($(VOTT), v6)
VOMSRC+= DecWindowNoAttacks_v6.S RA_PostMultiply_V6.S RA_PreMultiply_V6.S RA_R4_Core_v6.S RA_R8FirstPass_v6.S
VOSRCDIR+= ../../../src/ra8lbr/decoder/asm/iOS/ARMV6
endif				

ifeq ($(VOTT), v7)
VOMSRC+= DecWindowNoAttacks_v7.s RA_PostMultiply_V7.s RA_PreMultiply_V7.s RA_R4_Core_V7.s RA_R8FirstPass_v7.s
VOSRCDIR+= ../../../src/ra8lbr/decoder/asm/iOS/Neon
endif	

ifeq ($(VOTT), v7s)
VOMSRC+= DecWindowNoAttacks_v7.s RA_PostMultiply_V7.s RA_PreMultiply_V7.s RA_R4_Core_V7.s RA_R8FirstPass_v7.s
VOSRCDIR+= ../../../src/ra8lbr/decoder/asm/iOS/Neon
endif	
