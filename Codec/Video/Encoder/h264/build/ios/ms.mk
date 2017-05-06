# please list all objects needed by your module here
VOMSRC:=voH264Dct.c\
  voH264Deblock.c\
  voH264Frame.c\
  voH264EncBitStream.c\
  voH264EncCAVLC.c\
  voH264EncMB.c\
  voH264EncME.c\
  voH264EncPicSlice.c\
  voH264EncRC.c\
  voH264EncSAD.c\
  voH264IntraPrediction.c\
  voH264MC.c\
  voH264Quant.c

ifeq ($(VOTT), v6)
#    VOMSRC +=Armv4GetBlockChromaNxNInBound.s Armv6Chroma_InPlaceDeblock.s Armv6GetBlockLumaNxNInBound.s Armv6InvTrans.s Armv6Luma_InPlaceDeblock.s Armv6_SIAVG.s
endif

ifeq ($(VOTT), v7)
   VOMSRC +=voH264Dct_ARMV7_gnu.S voH264EncSAD_ARMV7_gnu.S voH264IntraPrediction_ARMV7_gnu.S voH264Quant_ARMV7_gnu.S voH264Mc_ARMV7_gnu.S\
	voH264HPel_ARMV7_gnu.S voH264Deblock_ARMV7_gnu.S voH264Pic_ARMV7_gnu.S
endif

ifeq ($(VOTT), v7s)
   VOMSRC +=voH264Dct_ARMV7_gnu.S voH264EncSAD_ARMV7_gnu.S voH264IntraPrediction_ARMV7_gnu.S voH264Quant_ARMV7_gnu.S voH264Mc_ARMV7_gnu.S\
	voH264HPel_ARMV7_gnu.S voH264Deblock_ARMV7_gnu.S voH264Pic_ARMV7_gnu.S
endif

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../src \
	  ../../../../../../../../Include\


ifeq ($(VOTT), v6)
#   VOSRCDIR +=../../../../Sources/ARMv6/arm_mac 
endif

ifeq ($(VOTT), v7)
   VOSRCDIR +=../../../../src/arm_mac/armv7
endif

ifeq ($(VOTT), v7s)
   VOSRCDIR +=../../../../src/arm_mac/armv7
endif
