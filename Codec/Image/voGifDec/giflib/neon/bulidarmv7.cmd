armasm --cpu Cortex-A8 --diag_warning=1563 reset_prefix.asm reset_prefix.o
ELF2COFF.exe reset_prefix.o obj/reset_prefix.obj


del *.txt
del *.o