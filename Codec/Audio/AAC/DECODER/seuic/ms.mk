
# please list all objects needed by your target here
OBJS:=bitstream.o bsac_dec_spectra.o cmnMemory.o decframe.o decode_bsac.o decoder.o downMatrix.o Header.o \
			latmheader.o lc_dequant.o lc_huff.o lc_imdct.o lc_mdct.o lc_pns.o lc_stereo.o lc_syntax.o lc_tns.o \
			ltp_dec.o ps_dec.o ps_syntax.o sam_decode_bsac.o sam_fadecode.o sbr_dec.o sbr_hfadj.o sbr_hfgen.o \
			sbr_huff.o sbr_qmf.o sbr_syntax.o sbr_utility.o table.o unit.o ic_predict.o
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../source ../../include \
					../../source/sbr ../../source/PS \
					../../source/BSAC \
					../../../../../../Include \
					../../../../../../Common


