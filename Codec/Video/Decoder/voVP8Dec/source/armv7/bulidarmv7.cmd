armasm --cpu Cortex-A8 dequant_idct_neon.asm dequant_idct_neon.o
ELF2COFF.exe dequant_idct_neon.o dequant_idct_neon.obj

del *.txt
del *.o