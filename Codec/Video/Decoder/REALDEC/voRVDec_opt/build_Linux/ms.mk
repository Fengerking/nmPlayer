# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
    ../../../src/

# please list all objects needed by your target here
OBJS:= ai.o basic.o bsx.o cinterp4.o cinterp.o deblock8.o deblock.o dec4x4md.o decoder.o\
       drv.o frame.o idct.o parsebs.o recon.o rv_decode.o tables.o voRVDecSDK.o voRVThread.o
			
ifeq ($(VOTT), v4)
OBJS+= 
VOSRCDIR+= ../../../src/asm/gnu_asm/armv4
endif	

ifeq ($(VOTT), v5)
OBJS+= 
VOSRCDIR+= ../../../src/asm/gnu_asm/armv4
endif

ifeq ($(VOTT), v5x)
OBJS+= idct_wmmx2.o interp4_chroma_wmmx2.o interp4_part2_wmmx2.o interp4_wmmx2.o InterpolateDirect_wmmx2.o copy_wmmx2.o
VOSRCDIR+= ../../../src/asm/gnu_asm/wmmx2	
endif			

ifeq ($(VOTT), v6)
OBJS+= ARMV6_rv8_cinterp.o IDCT_armv6.o deblock_armv6.o interp4_armv6.o interp4Part2_armv6.o\
       interp4Part3_armv6.o mem_copy_armv6.o intraMB_pred_armv6.o
VOSRCDIR+= ../../../src/asm/gnu_asm/armv6
endif				

ifeq ($(VOTT), v7)
OBJS+= ARMV7_rv8_edge_filter.o cinterp_rv8.o deblock_CtxA8.o IDCT_CtxA8.o interpolate4_Chroma_CtxA8.o\
       interpolate4_Luma_CtxA8.o intraMB_Pred_CtxA8.o memcopy_CtxA8.o UVPackMB.o fillEdge_CtxA8.o
VOSRCDIR+= ../../../src/asm/gnu_asm/armv7
endif			

