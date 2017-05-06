armasm --cpu Cortex-A8 bilinearpredict4x4_neon.asm bilinearpredict4x4_neon.o
ELF2COFF.exe bilinearpredict4x4_neon.o obj/bilinearpredict4x4_neon.obj

armasm --cpu Cortex-A8 bilinearpredict8x4_neon.asm bilinearpredict8x4_neon.o
ELF2COFF.exe bilinearpredict8x4_neon.o obj/bilinearpredict8x4_neon.obj

armasm --cpu Cortex-A8 bilinearpredict8x8_neon.asm bilinearpredict8x8_neon.o
ELF2COFF.exe bilinearpredict8x8_neon.o obj/bilinearpredict8x8_neon.obj

armasm --cpu Cortex-A8 bilinearpredict16x16_neon.asm bilinearpredict16x16_neon.o
ELF2COFF.exe bilinearpredict16x16_neon.o obj/bilinearpredict16x16_neon.obj

armasm --cpu Cortex-A8 copymem8x4_neon.asm copymem8x4_neon.o
ELF2COFF.exe copymem8x4_neon.o obj/copymem8x4_neon.obj

armasm --cpu Cortex-A8 copymem8x8_neon.asm copymem8x8_neon.o
ELF2COFF.exe copymem8x8_neon.o obj/copymem8x8_neon.obj

armasm --cpu Cortex-A8 copymem16x16_neon.asm copymem16x16_neon.o
ELF2COFF.exe copymem16x16_neon.o obj/copymem16x16_neon.obj

armasm --cpu Cortex-A8 sixtappredict4x4_neon.asm sixtappredict4x4_neon.o
ELF2COFF.exe sixtappredict4x4_neon.o obj/sixtappredict4x4_neon.obj

armasm --cpu Cortex-A8 sixtappredict8x4_neon.asm sixtappredict8x4_neon.o
ELF2COFF.exe sixtappredict8x4_neon.o obj/sixtappredict8x4_neon.obj

armasm --cpu Cortex-A8 sixtappredict8x8_neon.asm sixtappredict8x8_neon.o
ELF2COFF.exe sixtappredict8x8_neon.o obj/sixtappredict8x8_neon.obj

armasm --cpu Cortex-A8 sixtappredict16x16_neon.asm sixtappredict16x16_neon.o
ELF2COFF.exe sixtappredict16x16_neon.o obj/sixtappredict16x16_neon.obj

del *.txt
del *.o