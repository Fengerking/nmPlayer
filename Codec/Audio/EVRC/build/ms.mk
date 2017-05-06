# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../Include \
	  ../../inc \
          ../../src/c_src \
	  ../../../../../Common 

# please list all objects needed by your target here
OBJS:= a2lsp.o acb_ex.o acelp_pf.o apf.o auto.o bitpack.o bitupack.o bl_intrp.o \
       bqiir.o c3_10pf.o c8_35pf.o comacb.o convh.o cshift.o d3_10pf.o d8_35pf.o \
       d_fer.o d_no_fer.o d_rate_1.o decode.o durbin.o encode.o fcbgq.o fer.o filter.o \
       fndppf.o getext1k.o getgain.o getres.o impulser.o interpol.o intr_cos.o inv_sqrt.o \
       lpcana.o lsp2a.o lspmaq.o math_adv.o math_ext32.o maxeloc.o mdfyorig.o mem_align.o \
       mod.o ns127.o pit_shrp.o pktoav.o pre_enc.o putacbc.o r_fft.o rda.o rom.o stream.o \
       synfltr.o w2res.o weight.o zeroinpt.o cmnMemory.o

ifeq ($(TYPE), Dec)
OBJS+= voEVRCDec.o
else
OBJS+= voEVRCEnc.o
endif
			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../src/linux_asm
endif				

ifeq ($(VOTT), v6)
OBJS+= ConIR.o GetResidual.o Impulse_asm.o bl_intrp_asm.o cor_h_vec.o iir_asm.o \
       cor_h_x_v6.o fir_asm.o
VOSRCDIR+= ../../src/linux_asm
endif				

ifeq ($(VOTT), v7)
OBJS+= ConIR.o GetResidual.o Impulse_asm.o bl_intrp_asm.o cor_h_vec.o iir_asm.o \
       cor_h_x_v6.o fir_asm.o 
VOSRCDIR+= ../../src/linux_asm
endif			
