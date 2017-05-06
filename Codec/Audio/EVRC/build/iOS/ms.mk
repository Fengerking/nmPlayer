# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Include \
	  ../../../inc \
          ../../../src/c_src \
	  ../../../../../../Common 

# please list all objects needed by your target here
VOMSRC:=a2lsp.c acb_ex.c acelp_pf.c apf.c auto.c bitpack.c bitupack.c bl_intrp.c \
       bqiir.c c3_10pf.c c8_35pf.c comacb.c convh.c cshift.c d3_10pf.c d8_35pf.c \
       d_fer.c d_no_fer.c d_rate_1.c decode.c durbin.c encode.c fcbgq.c fer.c filter.c \
       fndppf.c getext1k.c getgain.c getres.c impulser.c interpol.c intr_cos.c inv_sqrt.c \
       lpcana.c lsp2a.c lspmaq.c math_adv.c math_ext32.c maxeloc.c mdfyorig.c mem_align.c \
       mod.c ns127.c pit_shrp.c pktoav.c pre_enc.c putacbc.c r_fft.c rda.c rom.c stream.c \
       synfltr.c w2res.c weight.c zeroinpt.c voEVRCDec.c voEVRCEnc.c
			
ifeq ($(VOTT), v4)
VOMSRC+= 
VOSRCDIR+= ../../../src/iOS
endif				

ifeq ($(VOTT), v6)
VOMSRC+= ConIR.S GetResidual.S Impulse_asm.S bl_intrp_asm.S cor_h_vec.S iir_asm.S \
       cor_h_x_v6.S fir_asm.S
VOSRCDIR+= ../../../src/iOS
endif				

ifeq ($(VOTT), v7)
VOMSRC+= ConIR.S GetResidual.S Impulse_asm.S bl_intrp_asm.S cor_h_vec.S iir_asm.S \
       cor_h_x_v6.S fir_asm.S 
VOSRCDIR+= ../../../src/iOS
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= ConIR.S GetResidual.S Impulse_asm.S bl_intrp_asm.S cor_h_vec.S iir_asm.S \
       cor_h_x_v6.S fir_asm.S 
VOSRCDIR+= ../../../src/iOS
endif			
