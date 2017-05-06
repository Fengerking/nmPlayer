armasm --cpu Cortex-A8 idct_dequant_full_2x_neon.asm idct_dequant_full_2x_neon.o
ELF2COFF.exe idct_dequant_full_2x_neon.o obj/idct_dequant_full_2x_neon.obj

del *.txt
del *.o