# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include \
    ../../../src/

# please list all objects needed by your target here
VOMSRC:= ai.c basic.c bsx.c cinterp4.c cinterp.c deblock8.c deblock.c dec4x4md.c decoder.c\
       drv.c frame.c idct.c parsebs.c recon.c rv_decode.c tables.c voRVDecSDK.c voRVThread.c
			
#ifeq ($(VOTT), v4)
#OBJS+= 
#VOSRCDIR+= ../../../src/asm/arm_mac/armv4
#endif	

#ifeq ($(VOTT), v5)
#OBJS+= 
#VOSRCDIR+= ../../../src/asm/arm_mac/armv4
#endif

#ifeq ($(VOTT), v5x)
#OBJS+= idct_wmmx2.o interp4_chroma_wmmx2.o interp4_part2_wmmx2.o interp4_wmmx2.o InterpolateDirect_wmmx2.o copy_wmmx2.o
#VOSRCDIR+= ../../../src/asm/arm_mac/wmmx2	
#endif			

ifeq ($(VOTT), v6)
VOMSRC+= ARMV6_rv8_cinterp.S IDCT_armv6.S deblock_armv6.S interp4_armv6.S interp4Part2_armv6.S\
       interp4Part3_armv6.S mem_copy_armv6.S intraMB_pred_armv6.S
VOSRCDIR+= ../../../src/asm/arm_mac/armv6
endif				

ifeq ($(VOTT), v7)
VOMSRC+= ARMV7_rv8_edge_filter.s cinterp_rv8.s deblock_CtxA8.s IDCT_CtxA8.s interpolate4_Chroma_CtxA8.s\
       interpolate4_Luma_CtxA8.s intraMB_Pred_CtxA8.s memcopy_CtxA8.S UVPackMB.s fillEdge_CtxA8.S
VOSRCDIR+= ../../../src/asm/arm_mac/armv7
endif			


ifeq ($(VOTT), v7s)
VOMSRC+= ARMV7_rv8_edge_filter.s cinterp_rv8.s deblock_CtxA8.s IDCT_CtxA8.s interpolate4_Chroma_CtxA8.s\
       interpolate4_Luma_CtxA8.s intraMB_Pred_CtxA8.s memcopy_CtxA8.S UVPackMB.s fillEdge_CtxA8.S
VOSRCDIR+= ../../../src/asm/arm_mac/armv7
endif			

#VOSRCNO:=21537