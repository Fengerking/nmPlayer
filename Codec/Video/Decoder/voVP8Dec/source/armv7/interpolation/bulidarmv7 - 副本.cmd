armasm --cpu Cortex-A8 vp8_predict_intra4x4.asm vp8_predict_intra4x4.o
ELF2COFF.exe vp8_predict_intra4x4.o obj/vp8_predict_intra4x4.obj


del *.txt
del *.o