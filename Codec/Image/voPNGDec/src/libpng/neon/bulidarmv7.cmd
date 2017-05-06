armasm --cpu Cortex-A8 --diag_warning=1563 expend_to_final_width.asm expend_to_final_width.o
ELF2COFF.exe expend_to_final_width.o obj/expend_to_final_width.obj


del *.txt
del *.o