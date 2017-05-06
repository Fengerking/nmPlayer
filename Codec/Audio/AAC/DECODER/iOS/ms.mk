
# please list all objects needed by your target here
VOMSRC:=voLog.c cmnAudioMemory.c bitstream.c bsac_dec_spectra.c decframe.c decode_bsac.c decoder.c downMatrix.c Header.c \
			latmheader.c ic_predict.c lc_dequant.c lc_huff.c lc_imdct.c lc_mdct.c lc_pns.c lc_stereo.c lc_syntax.c lc_tns.c \
			ltp_dec.c ps_dec.c ps_syntax.c sam_decode_bsac.c sam_fadecode.c sbr_dec.c sbr_hfadj.c sbr_hfgen.c \
			sbr_huff.c sbr_qmf.c sbr_syntax.c sbr_utility.c table.c unit.c lc_hufftab.c
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../source ../../include \
					../../source/sbr ../../source/PS \
					../../source/BSAC \
					../../../../../../Include \
					../../../../../../Common \
					../../../../Common			

ifeq ($(VOTT), v6)
VOMSRC+= R4_Core_v6.S R8FirstPass_v6.S writePCM_ARMV6.S PostMultiply_V6.S PreMultiply_V6.S 
VOSRCDIR+= ../../source/iphone/armv6
endif

ifeq ($(VOTT), v7)
VOMSRC+= R4_Core_V7.s R8FirstPass_v7.s writePCM_ARMV6.s writePCM_ARMV7.s PostMultiply_V7.s PreMultiply_V7.s  WinLong_V7.s 
VOSRCDIR+= ../../source/iphone/armv7
endif

ifeq ($(VOTT), v7s)
VOMSRC+= R4_Core_V7.s R8FirstPass_v7.s writePCM_ARMV6.s writePCM_ARMV7.s PostMultiply_V7.s PreMultiply_V7.s  WinLong_V7.s 
VOSRCDIR+= ../../source/iphone/armv7
endif					
