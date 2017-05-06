armasm --cpu Cortex-A8 recon2b_neon.asm recon2b_neon.o
ELF2COFF.exe recon2b_neon.o obj/recon2b_neon.obj

armasm --cpu Cortex-A8 recon4b_neon.asm recon4b_neon.o
ELF2COFF.exe recon4b_neon.o obj/recon4b_neon.obj

armasm --cpu Cortex-A8 recon16x16mb_neon.asm recon16x16mb_neon.o
ELF2COFF.exe recon16x16mb_neon.o obj/recon16x16mb_neon.obj

armasm --cpu Cortex-A8 reconb_neon.asm reconb_neon.o
ELF2COFF.exe reconb_neon.o obj/reconb_neon.obj


del *.txt
del *.o