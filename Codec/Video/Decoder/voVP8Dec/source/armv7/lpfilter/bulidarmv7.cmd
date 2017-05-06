armasm --cpu Cortex-A8 loopfilter_neon.asm loopfilter_neon.o
ELF2COFF.exe loopfilter_neon.o obj/loopfilter_neon.obj

armasm --cpu Cortex-A8 loopfiltersimplehorizontaledge_neon.asm loopfiltersimplehorizontaledge_neon.o
ELF2COFF.exe loopfiltersimplehorizontaledge_neon.o obj/loopfiltersimplehorizontaledge_neon.obj

armasm --cpu Cortex-A8 loopfiltersimpleverticaledge_neon.asm loopfiltersimpleverticaledge_neon.o
ELF2COFF.exe loopfiltersimpleverticaledge_neon.o obj/loopfiltersimpleverticaledge_neon.obj

armasm --cpu Cortex-A8 mbloopfilter_neon.asm mbloopfilter_neon.o
ELF2COFF.exe mbloopfilter_neon.o obj/mbloopfilter_neon.obj

del *.txt
del *.o