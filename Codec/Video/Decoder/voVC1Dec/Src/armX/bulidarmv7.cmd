armasm --cpu Cortex-A8 voVC1Interpolate_ARMV7.s voVC1Interpolate_ARMV7.o
ELF2COFF.exe voVC1Interpolate_ARMV7.o voVC1Interpolate_ARMV7.obj

armasm --cpu Cortex-A8 voVC1InterpolateOnly_ARMv7.s voVC1InterpolateOnly_ARMv7.o
ELF2COFF.exe voVC1InterpolateOnly_ARMv7.o voVC1InterpolateOnly_ARMv7.obj

armasm --cpu Cortex-A8 voVC1Trans_ARMV7.s voVC1Trans_ARMV7.o
ELF2COFF.exe voVC1Trans_ARMV7.o voVC1Trans_ARMV7.obj

armasm --cpu Cortex-A8 voVC1Trans_Overlap_ARMV7.s voVC1Trans_Overlap_ARMV7.o
ELF2COFF.exe voVC1Trans_Overlap_ARMV7.o voVC1Trans_Overlap_ARMV7.obj

del *.txt
del *.o