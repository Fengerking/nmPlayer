# please list all objects needed by your module here
VOMSRC:= biaridecod.c block.c C_Runtime_Lib_Port.c cabac.c cCabac.c \
             cGetBlock.c cInvTrans.c context_ini.c decPicBuf.c \
             erc_api.c erc_do_i.c erc_do_p.c errorconcealment.c filehandle.c \
             h264VdLib.c header.c image.c InPlaceDeblockMBs.c ldecod.c \
             macroblock.c mb_access.c mbuffer.c \
             memalloc.c nal.c nalu.c neighbor.c output.c \
             parset.c sei.c vlc.c voh264DecSDK.c \
             voLog_android.c voh264DecCombineSDK.c
#annexb.o cShortInline.o fmo.o leaky_bucket.o loopFilter.o nal_part.o \
#nalucommon.o parsetcommon.o rtp.o

ifeq ($(VOTT), v6)
    VOMSRC +=Armv4GetBlockChromaNxNInBound.s Armv6Chroma_InPlaceDeblock.s Armv6GetBlockLumaNxNInBound.s Armv6InvTrans.s Armv6Luma_InPlaceDeblock.s Armv6_SIAVG.s
endif

ifeq ($(VOTT), v7)
   VOMSRC +=Armv7GetBlockLumaNxNInBound.s Armv7InvTrans.s  Armv7GetBlockChromaNxNInBound.s  ARMv7DeblockLumaV.s ARMv7DeblockChromaH.s ARMv7DeblockChromaV.s ARMv7DeblockLumaH.s  Armv7_SIAVG.s 
endif

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../Sources \
             ../../../../../../../../Include \
             ../../../../../../../../Common \
             ../../../../Sources/old_sdk_inc 


ifeq ($(VOTT), v6)
   VOSRCDIR +=../../../../Sources/ARMv6/arm_mac 
endif

ifeq ($(VOTT), v7)
   VOSRCDIR +=../../../../Sources/NEON/arm_mac 
endif

ifeq ($(VOTT), v7s)
   VOSRCDIR +=../../../../Sources/NEON/arm_mac 
endif
