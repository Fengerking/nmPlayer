
# please list all objects needed by your module here
OBJS:=ARM11_MX31_Deblock.o voIMX31DeblockAndroidImp.o  block.o\
cGetBlock.o cInvTrans.o cShortInline.o decPicBuf.o filehandle.o fmo.o\
h264VdLib.o header.o image.o InPlaceDeblockMBs.o ldecod.o loopFilter.o macroblock.o mb_access.o mbuffer.o\
memalloc.o nal.o nal_part.o nalu.o nalucommon.o neighbor.o output.o parset.o parsetcommon.o sei.o vlc.o voh264DecSDK.o  voLog_android.o\
biaridecod.o cCabac.o context_ini.o cShortInline.o cabac.o


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../../Include \
		../../../../Sources \
		../../../../Sources/Imx31 \
		../../../../Sources/old_sdk_inc 

	

