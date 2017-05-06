# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
    ../../../src/

# please list all objects needed by your target here
OBJS:= h265_decoder.o h265dec_cabac.o h265dec_cu.o h265dec_debug.o h265dec_dequantCoef.o h265dec_front.o \
	h265dec_idct.o h265dec_inter.o h265dec_intra.o h265dec_list.o h265dec_mem.o h265dec_nal.o \
	h265dec_parser.o h265dec_pic.o h265dec_recon.o h265dec_sao.o h265dec_slice.o h265dec_deblock.o
			
ifeq ($(VOTT), v4)
OBJS+= 
#VOSRCDIR+= ../../../src/asm/gnu_asm/armv4
endif	

ifeq ($(VOTT), v5)
OBJS+= 
#VOSRCDIR+= ../../../src/asm/gnu_asm/armv4
endif

ifeq ($(VOTT), v5x)
#OBJS+= idct_wmmx2.o interp4_chroma_wmmx2.o interp4_part2_wmmx2.o interp4_wmmx2.o InterpolateDirect_wmmx2.o copy_wmmx2.o
#VOSRCDIR+= ../../../src/asm/gnu_asm/wmmx2	
endif			

ifeq ($(VOTT), v6)
#OBJS+= ARMV6_rv8_cinterp.o IDCT_armv6.o deblock_armv6.o interp4_armv6.o interp4Part2_armv6.o\
       #interp4Part3_armv6.o mem_copy_armv6.o intraMB_pred_armv6.o
#VOSRCDIR+= ../../../src/asm/gnu_asm/armv6
endif				

ifeq ($(VOTT), v7)
OBJS+= luma_mc_neon.o chroma_mc_neon.o copy_mem_neon.o IDCT4X4_GNU.o IDCT8X8_GNU.o IDCT16X16_GNU.o IDCT32X32_GNU.o \
	IDST4X4_GNU.o
VOSRCDIR+= ../../../src/asm/armv7/gnu
endif			
