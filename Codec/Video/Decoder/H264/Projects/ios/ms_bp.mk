# please list all objects needed by your module here
VOMSRC:= block.c cGetBlock.c \
          cInvTrans.c decPicBuf.c erc_api.c \
          erc_do_i.c erc_do_p.c filehandle.c \
          h264VdLib.c header.c image.c InPlaceDeblockMBs.c ldecod.c \
          macroblock.c mb_access.c mbuffer.c \
          memalloc.c nal.c nalu.c neighbor.c \
          output.c parset.c sei.c vlc.c \
          voh264DecSDK.c voLog_android.c 
#annexb.o biaridecod.o cabac.o cCabac.o context_ini.o cShortInline.o \
#errorconcealment.o fmo.o leaky_bucket.o loopFilter.o nal_part.o \
#nalucommon.o parsetcommon.o rtp.o

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../Sources \
            ../../../../../../../../Include \
            ../../../../../../../../Common \
            ../../../../Sources/old_sdk_inc 
