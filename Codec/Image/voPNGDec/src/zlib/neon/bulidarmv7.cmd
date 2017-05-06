armasm --cpu Cortex-A8 --diag_warning=1563 DO16_armv7.asm DO16_armv7.o
ELF2COFF.exe DO16_armv7.o obj/DO16_armv7.obj


del *.txt
del *.o