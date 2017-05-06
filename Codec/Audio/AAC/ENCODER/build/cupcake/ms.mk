
# please list all objects needed by your target here
OBJS:=basicop2.o oper_32b.o aac_ram.o aac_rom.o aacenc.o aacenc_core.o adj_thr.o \
			cmnMemory.o band_nrg.o bit_cnt.o bitbuffer.o bitenc.o block_switch.o channel_map.o \
			dyn_bits.o grp_data.o interface.o line_pe.o mem_align.o ms_stereo.o \
			pre_echo_control.o psy_configuration.o psy_main.o qc_main.o quantize.o sf_estim.o \
			spreading.o stat_bits.o tns.o transform.o
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../src \
					../../../basic_op\
					../../../../../../../Include \
					../../../../../../../Common 
					
ifeq ($(VOTT), v5)
OBJS+= band_nrg_asm.o CalcWindowEnergy_asm.o TNS_ASM.o
VOSRCDIR+= ../../../src/linuxasm/ARMV5E/
endif	

ifeq ($(VOTT), v6)
OBJS+= band_nrg_asm.o CalcWindowEnergy_asm.o TNS_ASM.o
VOSRCDIR+= ../../../src/linuxasm/ARMV5E/
endif					

ifeq ($(VOTT), v7)
OBJS+= band_nrg_asm.o CalcWindowEnergy_asm.o TNS_ASM.o \
			 PPMdct.o R4_Core_V7.o R8FirstPass_v7.o
VOSRCDIR+= ../../../src/linuxasm/ARMV5E/
VOSRCDIR+= ../../../src/linuxasm/ARMV7Neon/
endif		