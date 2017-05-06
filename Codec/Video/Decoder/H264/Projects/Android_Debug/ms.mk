
# please list all objects needed by your module here
OBJS:=ARM11_MX31_Deblock.o voIMX31DeblockAndroidImp.o  block.o\
cGetBlock.o cInvTrans.o cShortInline.o decPicBuf.o filehandle.o fmo.o\
h264VdLib.o header.o image.o InPlaceDeblockMBs.o ldecod.o loopFilter.o macroblock.o mb_access.o mbuffer.o\
memalloc.o nal.o nal_part.o nalu.o nalucommon.o neighbor.o output.o parset.o parsetcommon.o sei.o vlc.o voh264DecSDK.o C_Runtime_Lib_Port.o voLog_android.o\
biaridecod.o cCabac.o context_ini.o cShortInline.o cabac.o

ifeq ($(VOTT), v4)
   OBJS +=Armv4GetBlockLumaNxNInBound.o Armv4GetBlockChromaNxNInBound.o Armv4Chroma_InPlaceDeblock.o  Armv4Luma_InPlaceDeblock.o #Armv4InvTrans.o 
endif

ifeq ($(VOTT), v6)
   OBJS +=Armv4GetBlockChromaNxNInBound.o Armv6Chroma_InPlaceDeblock.o Armv6GetBlockLumaNxNInBound.o Armv6InvTrans.o Armv6Luma_InPlaceDeblock.o
endif

ifeq ($(VOTT), v7)
   OBJS +=Armv7GetBlockLumaNxNInBound.o Armv7InvTrans.o  Armv7GetBlockChromaNxNInBound.o  ARMv7DeblockLumaV.o ARMv7DeblockChromaH.o ARMv7DeblockChromaV.o ARMv7DeblockLumaH.o  
endif


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../../Include \
		../../../../Sources \
		../../../../Sources/Imx31 \
		../../../../Sources/old_sdk_inc 

		

ifeq ($(VOTT), v4)
   VOSRCDIR +=../../../../Sources/ARMv4/GCC
endif

ifeq ($(VOTT), v6)
   VOSRCDIR +=../../../../Sources/ARMv6/GCC 
endif

ifeq ($(VOTT), v7)
   VOSRCDIR +=../../../../Sources/NEON/GCC 
endif

ifeq ($(VOTT), v7s)
   VOSRCDIR +=../../../../Sources/NEON/GCC 
endif

