
# please list all objects needed by your target here
OBJS:=ra_decode.o voRABUFDec.o voRADecSDK.o voRARawDec.o voRARMDec.o ra8lbr_decode.o \
			bitpack.o buffers.o category.o couple.o envelope.o fft.o gainctrl.o gecko2codec.o \
			huffman.o hufftabs.o mlt.o sqvh.o trigtabs.o aac_decode.o aac_bitstream.o ga_config.o \
			bitstream.o bsac_dec_spectra.o cmnMemory.o decframe.o decode_bsac.o decoder.o downMatrix.o Header.o \
			latmheader.o lc_dequant.o lc_huff.o lc_imdct.o lc_mdct.o lc_pns.o lc_stereo.o lc_syntax.o lc_tns.o \
			ltp_dec.o ps_dec.o ps_syntax.o sam_decode_bsac.o sam_fadecode.o sbr_dec.o sbr_hfadj.o sbr_hfgen.o \
			sbr_huff.o sbr_qmf.o sbr_syntax.o sbr_utility.o table.o unit.o ic_predict.o lc_hufftab.o
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../src/interface \
					../../../include ../../../src/ra8lbr \
					../../../src/ra8lbr/decoder \
					../../../src/aac \
					../../../../AAC/DECODER/source \
					../../../../AAC/DECODER/include \
					../../../../AAC/DECODER/source/sbr \
					../../../../AAC/DECODER/source/PS \
					../../../../AAC/DECODER/source/BSAC \
					../../../../../../Include \
					../../../../../../Common

ifeq ($(VOTT), v4)
OBJS+= qmf.o
VOSRCDIR+= ../../../../AAC/DECODER/source/linuxasm/
endif

ifeq ($(VOTT), v6)
OBJS+= DecWindowNoAttacks_v6.o RA_PostMultiply_V6.o RA_PreMultiply_V6.o RA_R4_Core_v6.o RA_R8FirstPass_v6.o
VOSRCDIR+= ../../../src/ra8lbr/decoder/asm/linux/ARMV6
OBJS+= qmf.o PostMultiply_V6.o PreMultiply_V6.o R4_Core_v6.o R8FirstPass_v6.o writePCM_ARMV6.o
VOSRCDIR+= ../../../../AAC/DECODER/source/linuxasm/
VOSRCDIR+= ../../../../AAC/DECODER/source/linuxasm/armv6
endif				

ifeq ($(VOTT), v7)
OBJS+= DecWindowNoAttacks_v7.o RA_PostMultiply_V7.o RA_PreMultiply_V7.o RA_R4_Core_V7.o RA_R8FirstPass_v7.o
VOSRCDIR+= ../../../src/ra8lbr/decoder/asm/linux/Neon
OBJS+= qmf.o WinLong_V7.o PostMultiply_V7.o PreMultiply_V7.o R4_Core_V7.o R8FirstPass_v7.o writePCM_ARMV6.o writePCM_ARMV7.o
VOSRCDIR+= ../../../../AAC/DECODER/source/linuxasm/
VOSRCDIR+= ../../../../AAC/DECODER/source/linuxasm/armv7
endif	
